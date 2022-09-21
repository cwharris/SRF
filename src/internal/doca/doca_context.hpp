#pragma once

#include <doca_gpu.h>

#include <string>

namespace srf::internal::doca
{

struct doca_context
{
private:
  doca_gpu* _gpu;
public:
  doca_context(std::string nic_addr, std::string gpu_addr);
  ~doca_context();
};

}
