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

set(GLSLC_EXECUTABLE Vulkan::glslc)
file(GLOB_RECURSE SHADER_SOURCES "shaders/*.vert" "shaders/*.frag")

set(COMPILED_SHADERS)

foreach (SHADER ${SHADER_SOURCES})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(SPV "${CMAKE_BINARY_DIR}/shaders/${FILE_NAME}.spv")

    add_custom_command(
            OUTPUT ${SPV}
            COMMAND ${GLSLC_EXECUTABLE} ${SHADER} -o ${SPV}
            DEPENDS ${SHADER}
            COMMENT "Compiling shader ${FILE_NAME}"
    )

    list(APPEND COMPILED_SHADERS ${SPV})
endforeach ()

add_executable(${PROJECT_NAME} ${SOURCES})

add_custom_target(Shaders ALL DEPENDS ${COMPILED_SHADERS})
add_dependencies(${PROJECT_NAME} Shaders)


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/headers)
target_link_libraries(${PROJECT_NAME} glfw glm Vulkan::Vulkan)