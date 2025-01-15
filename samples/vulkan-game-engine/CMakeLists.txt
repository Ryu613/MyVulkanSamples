cmake_minimum_required(VERSION 3.22)

add_definitions(-DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

project(vulkan-game-engine)

include(cmake/third_library.cmake)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.hpp *.cpp)


add_executable(VulkanGameEngine ${srcs})

include_directories(include ${Vulkan_INCLUDE_DIR})

target_link_libraries(VulkanGameEngine PRIVATE ${Vulkan_LIBRARIES} glfw glm::glm)