set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

file(GLOB MAIN_SOURCES ${SOURCES_DIR}/*.cpp)

file(GLOB_RECURSE shader_SRC CONFIGURE_DEPENDS "shaders/*.hlsl")
message("Embedded Shaders: ${shader_SRC}")

# Embed OpenGL Shaders (LINUX)
add_custom_command(
	OUTPUT shader_rc.cpp
	COMMAND embedfile ${CMAKE_CURRENT_SOURCE_DIR}/shaders shader_rc.cpp MakeShaderMap .hlsl
	DEPENDS ${shader_SRC})

add_library(nex-core ${MAIN_SOURCES} shader_rc.cpp)
target_include_directories(nex-core PUBLIC ${HEADER_DIR})
target_include_directories(nex-core PUBLIC ${SHADER_DIR})

target_link_libraries(nex-core PUBLIC glm::glm)
target_link_libraries(nex-core PUBLIC glfw)
target_link_libraries(nex-core PUBLIC plog::plog)
target_link_libraries(nex-core PUBLIC 
    Diligent-GraphicsEngineVk-shared 
    Diligent-TargetPlatform 
    Diligent-PlatformInterface
    Diligent-GraphicsTools
    Diligent-Common
)
target_link_libraries(nex-core PUBLIC nonstd::expected-lite)
target_link_libraries(nex-core PUBLIC unofficial::im3d::im3d)

