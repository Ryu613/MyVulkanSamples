cmake_minimum_required(VERSION 3.22)

project(modern-cookbook)


find_package(vulkan REQUIRED)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.hpp *.cpp)


add_executable(mcook ${srcs})

include_directories(include ${Vulkan_INCLUDE_DIR})

link_directories(${Vulkan_INCLUDE_DIR}/../lib)