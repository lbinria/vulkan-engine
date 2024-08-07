cmake_minimum_required(VERSION 3.10)

project(VulkanTriangle)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

file(GLOB SOURCES "*.cpp" "*.h")
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES} glfw)

configure_file(shaders/simple_shader.vert.spv shaders/simple_shader.vert.spv COPYONLY)
configure_file(shaders/simple_shader.frag.spv shaders/simple_shader.frag.spv COPYONLY)