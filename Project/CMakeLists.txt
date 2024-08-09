# Source files
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)
set(STB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/stb")

add_custom_target(
        copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/Project/Models
        ${PROJECT_BINARY_DIR}/Project/Models
        COMMENT "Copying resources into binary directory"
)

set(SOURCES
    "main.cpp"
    "vulkanbase/VulkanBase.h" 
    "vulkanbase/VulkanUtil.h"
    # Add other source files here
        "ShaderModule.cpp"
    "ShaderModule.h"
        CommandPool.cpp
        CommandPool.h
        CommandBuffer.cpp
        CommandBuffer.h
        vulkanbase/VulkanBase.cpp
        DataBuffer.cpp
        DataBuffer.h
        Mesh.cpp
        Mesh.h
        GraphicsPipeline.cpp
        GraphicsPipeline.h
        Scene.cpp
        Scene.h
        Vertex.h
        Camera.cpp
        Camera.h
        DescriptorPool.cpp
        DescriptorPool.h
        Mesh3D.h
        Mesh3D.cpp
        DeltaTime.cpp
        DeltaTime.h
        Singleton.h
        Textures/Texture.cpp
        Textures/Texture.h
        Textures/Texture.h
        DepthBuffer.cpp
        DepthBuffer.h
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES} "vulkanbase/VulkanUtil.cpp" )
add_dependencies(${PROJECT_NAME} Shaders)
add_dependencies(${PROJECT_NAME} copy_resources)
# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE ${TINYOBJLOADER_LIBRARY_DIR} tinyobjloader)
target_include_directories(${PROJECT_NAME} PRIVATE "${STB_DIR}/include/")