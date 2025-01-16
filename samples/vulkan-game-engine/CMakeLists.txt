cmake_minimum_required(VERSION 3.22)

project(vulkan-game-engine)

include(cmake/externals.cmake)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.hpp *.cpp)


add_executable(VulkanGameEngine ${srcs})

include_directories(include ${Vulkan_INCLUDE_DIR})

target_link_libraries(VulkanGameEngine PRIVATE ${Vulkan_LIBRARIES} glfw glm::glm)