cmake_minimum_required (VERSION 3.8)

project(finalproject)

set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(CMAKE_CXX_STANDARD 11) #optional to use c++11


include(C:/vcpkg-master/scripts/buildsystems/vcpkg.cmake)

find_package(assimp REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED
    HINTS /encs/pkg/glfw-3.3.4/root # ENCS installation of glfw
)

 

# find_package(glad CONFIG REQUIRED)
   

set(ALL_LIBS
	OpenGL::GL
	OpenGL::GLU
	GLEW::glew
	glfw
	assimp::assimp
	imgui::imgui
) 

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-D_CRT_SECURE_NO_WARNINGS
	-DGLM_ENABLE_EXPERIMENTAL
	-DUSE_ZLIB_T
)


aux_source_directory(src SOURCES)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${ALL_LIBS})

