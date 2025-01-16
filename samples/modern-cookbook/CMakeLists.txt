cmake_minimum_required(VERSION 3.22)

add_definitions(-DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

project(modern-cookbook)

include(cmake/third_party_library.cmake)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.hpp *.cpp)


add_executable(mcook ${srcs})

include_directories( include ${Vulkan_INCLUDE_DIR})

target_link_directories(mcook PUBLIC ${Vulkan_INCLUDE_DIR}/../lib)

target_link_libraries(mcook PRIVATE glfw vulkan)