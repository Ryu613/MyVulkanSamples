cmake_minimum_required(VERSION 3.22)

project(vulkan-game-engine)

include(cmake/externals.cmake)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.hpp *.cpp)


add_executable(VulkanGameEngine ${srcs})

include_directories(include ${Vulkan_INCLUDE_DIR})

target_link_libraries(VulkanGameEngine PRIVATE ${Vulkan_LIBRARIES} glfw glm::glm)

add_custom_command(TARGET VulkanGameEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:VulkanGameEngine>/shaders
        COMMENT "Copying shaders folder to output directory"
)