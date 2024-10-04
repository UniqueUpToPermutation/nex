set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB MAIN_SOURCES ${SOURCES_DIR}/*.cpp)

add_library(nex-core ${MAIN_SOURCES})
target_include_directories(nex-core PUBLIC ${HEADER_DIR})

target_link_libraries(nex-core PRIVATE glm::glm)
target_link_libraries(nex-core PRIVATE glfw)
target_link_libraries(nex-core PRIVATE plog::plog)
target_link_libraries(nex-core PRIVATE 
    Diligent-GraphicsEngineVk-shared 
    Diligent-TargetPlatform 
    Diligent-PlatformInterface
    Diligent-Common
)
target_link_libraries(nex-core PUBLIC nonstd::expected-lite)