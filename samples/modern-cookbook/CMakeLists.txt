cmake_minimum_required(VERSION 3.22)

project(modern-cookbook)

include(cmake/third_party_library.cmake)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.hpp *.cpp)


add_executable(mcook ${srcs})

include_directories(include ${Vulkan_INCLUDE_DIR})

target_link_libraries(mcook PRIVATE glfw ${Vulkan_LIBRARIES} volk::volk)