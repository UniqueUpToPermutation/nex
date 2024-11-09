cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(PLATFORM_LINUX True)

project(nex VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(DiligentCore)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(plog CONFIG REQUIRED)
find_package(expected-lite CONFIG REQUIRED)
find_package(unofficial-im3d CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_subdirectory(tools)
add_subdirectory(core)
add_subdirectory(test)