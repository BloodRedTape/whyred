cmake_minimum_required(VERSION 3.10)

project(Sample)

set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(StraitXPackages/Base)
add_subdirectory(StraitXPackages/ImGui)
add_subdirectory(thirdparty/Assimp-FBXLoader)

set(SAMPLE_SOURCES
    "./sources/main.cpp"
    "./sources/application.cpp"
    "./sources/render/renderer.cpp"
    "./sources/render/mesh.cpp"
    "./sources/render/transform.cpp"
    "./sources/render/camera.cpp"
    "./sources/render/base_pass.cpp"
    "./sources/render/shadow_pass.cpp"
    "./sources/camera_controller.cpp"
    
)

set(SAMPLE_INCLUDE
    "./sources/"
)

add_executable(Sample ${SAMPLE_SOURCES})
target_link_libraries(Sample StraitXBase StraitXImGui assimp-fbxloader)
target_include_directories(Sample
    PUBLIC ${SAMPLE_INCLUDE}
)