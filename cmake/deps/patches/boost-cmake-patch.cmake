diff --git a/include/BoostRoot.cmake b/include/BoostRoot.cmake
index f2a51e1..a37f14d 100644
--- a/include/BoostRoot.cmake
+++ b/include/BoostRoot.cmake
@@ -90,7 +90,7 @@ else()
   endif()
 
   set(BUILD_TESTING OFF)
-  set(CMAKE_SKIP_INSTALL_RULES ON)
+  set(CMAKE_SKIP_INSTALL_RULES OFF)
 
 endif()
