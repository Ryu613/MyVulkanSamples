cmake_minimum_required(VERSION 3.22)

project(vulkan-cookbook)

include(cmake/externals.cmake)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.hpp *.cpp)


add_executable(vcook ${srcs})

target_include_directories(vcook PRIVATE include ${Vulkan_INCLUDE_DIR})

#target_link_libraries(vcook PRIVATE ${Vulkan_LIBRARIES})

# copy vulkan layer settings from path "config" to executable directory
# this approach is one of 3 ways of configuration of vulkan layers
# ref: https://vulkan.lunarg.com/doc/sdk/1.3.250.1/windows/layer_configuration.html
add_custom_command(TARGET vcook POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/config"
        $<TARGET_FILE_DIR:vcook>)