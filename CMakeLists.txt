cmake_minimum_required(VERSION 4.1)
project(VulkanLearning)

set(CMAKE_CXX_STANDARD 20)

# Fetch dependencies
include(FetchContent)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git)
FetchContent_MakeAvailable(glfw glm)

find_package(Vulkan REQUIRED)

# Add all source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "headers/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/headers)
target_link_libraries(${PROJECT_NAME} glfw glm Vulkan::Vulkan)

