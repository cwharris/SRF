/**
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "internal/ucx/all.hpp"
#include "internal/ucx/endpoint.hpp"
#include "srf/channel/forward.hpp"
#include "srf/types.hpp"

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <ucp/api/ucp.h>
#include <ucp/api/ucp_def.h>
#include <ucs/type/status.h>
#include <boost/fiber/future/future.hpp>
#include <boost/fiber/future/future_status.hpp>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>

using namespace srf;
using namespace internal::ucx;

class TestUCX : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        m_context = std::make_shared<Context>();
    }
    void TearDown() override {}

  public:
    Handle<Context> m_context;
};

TEST_F(TestUCX, Init) {}

TEST_F(TestUCX, CreateWorker)
{
    auto worker = std::make_shared<Worker>(m_context);
    worker->progress();
}

TEST_F(TestUCX, CreateWorkerAddress)
{
    auto worker  = std::make_shared<Worker>(m_context);
    auto address = worker->address();
    EXPECT_GT(address.length(), 0);
}

TEST_F(TestUCX, EndpointsInProcess)
{
    auto worker_1 = std::make_shared<Worker>(m_context);
    auto worker_2 = std::make_shared<Worker>(m_context);

    // get worker 1's address
    auto worker_1_addr = worker_1->address();

    // worker 2 will create an endpoint to worker 1
    auto ep_w2 = worker_2->create_endpoint(worker_1_addr);

    worker_1->progress();
    worker_2->progress();
}
/*
TEST_F(TestUCX, ReceiveManager)
{
    FiberGroup fibers(2);
    auto worker = UCX::global().create_worker();
    auto host_allocator = memory::make_allocator(memory::malloc_allocator()).shared();
    auto device_allocator = memory::make_allocator(memory::cuda_malloc_allocator(0)).shared();
    auto recv_mgr = std::make_shared<ReceiveManager>(worker, host_allocator, device_allocator);

    recv_mgr->shutdown();
}
*/

/*
TEST_F(TestUCX, UCXTaggedMessages)
{
    FiberGroup fibers(2);
    auto recv_worker = UCX::global().create_worker();
    auto send_worker = UCX::global().create_worker();
    auto host_allocator = memory::make_allocator(memory::malloc_allocator()).shared();
    auto device_allocator = memory::make_allocator(memory::cuda_malloc_allocator(0)).shared();
    auto recv_mgr = std::make_shared<ReceiveManager>(recv_worker, host_allocator, device_allocator);
    auto send_mgr = std::make_shared<SendManager>(send_worker);

    auto recv_address = recv_mgr->local_address();

    send_mgr->create_endpoint(42, recv_address);

    auto md = host_allocator->allocate_descriptor(sizeof(std::uint64_t));

    auto fs = send_mgr->async_write(42, 4, std::move(md));

    auto [tag, buff] = recv_mgr->await_read();

    fs.get();

    EXPECT_EQ(tag, 4);
    EXPECT_EQ(buff.size(), sizeof(std::uint64_t));

    recv_mgr->shutdown();
}
*/

struct GetUserData
{
    Promise<void> promise;
    ucp_rkey_h rkey;
};

static void rdma_get_callback(void* request, ucs_status_t status, void* user_data)
{
    DVLOG(1) << "rdma get callback start for request " << request;
    auto* data = static_cast<GetUserData*>(user_data);
    if (status != UCS_OK)
    {
        LOG(FATAL) << "rdma get failure occurred";
        // data->promise.set_exception();
    }
    data->promise.set_value();
    ucp_request_free(request);
    ucp_rkey_destroy(data->rkey);
}

TEST_F(TestUCX, Get)
{
    auto context = std::make_shared<Context>();

    auto worker_get_src = std::make_shared<Worker>(context);
    auto worker_get_dst = std::make_shared<Worker>(context);

    // create an ep from the worker issuing the GET to the worker address that is the source bytes
    auto ep = std::make_shared<Endpoint>(worker_get_dst, worker_get_src->address());

    auto src_data                              = std::malloc(4096);
    auto [src_lkey, src_rbuff, src_rbuff_size] = context->register_memory_with_rkey(src_data, 4096);

    auto dst_data = std::malloc(4096);
    auto dst_lkey = context->register_memory(dst_data, 4096);

    auto user_data = new GetUserData;
    auto rc        = ucp_ep_rkey_unpack(ep->handle(), src_rbuff, &user_data->rkey);
    if (rc != UCS_OK)
    {
        LOG(ERROR) << "ucp_ep_rkey_unpack failed - " << ucs_status_string(rc);
        throw std::runtime_error("ucp_ep_rkey_unpack failed");
    }

    ucp_request_param_t params;
    std::memset(&params, 0, sizeof(params));

    params.op_attr_mask = UCP_OP_ATTR_FIELD_CALLBACK | UCP_OP_ATTR_FIELD_USER_DATA | UCP_OP_ATTR_FLAG_NO_IMM_CMPL;
    params.cb.send      = rdma_get_callback;
    params.user_data    = user_data;

    auto future = user_data->promise.get_future();

    auto get_rc =
        ucp_get_nbx(ep->handle(), dst_data, 4096, reinterpret_cast<std::uint64_t>(src_data), user_data->rkey, &params);
    if (get_rc == nullptr /* UCS_OK */)
    {
        LOG(FATAL)
            << "should be unreachable";  // UCP_OP_ATTR_FLAG_NO_IMM_CMPL is set - should force the completion handler
    }
    else if (UCS_PTR_IS_ERR(get_rc))
    {
        LOG(ERROR) << "rdma get failure";  // << ucs_status_string(status);
        throw std::runtime_error("rdma get failure");
    }

    auto status = boost::fibers::future_status::timeout;
    while (status == boost::fibers::future_status::timeout)
    {
        status = future.wait_for(std::chrono::milliseconds(1));
        worker_get_dst->progress();
        worker_get_src->progress();
    }
    future.get();
}
