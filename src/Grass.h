#pragma once
//Including OpenGL Libraries:

#include <vector>
#include <string>
#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "AssimpGLMHelpers.h"

#include <iostream>
#include <sstream>
#include <fstream>


#include <GL/glew.h>  
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Shader.h"

#include "ObjectLoader.h"

#include "Camera.h"

#include "Animator.h"

#include "Animation.h"

#include <random>


#define MAX_GRASS_BLADES 10
#define GRASS_CHUNK_SIZE 0.4f
#define GRASS_LOD_CUTOFF_DISTANCE 3.0f
//Based on the project reviewed at https://www.youtube.com/watch?v=2h5NX9tIdno&t=2532s&ab_channel=TheCherno
class GrassChunk {
public:
	long numberOfBlades; 
	std::vector<glm::vec2> grassPosiions;
	float cameraVolume = 3;
	Camera* camera = NULL;
	glm::vec4 windDir;
	float bendScale; 

	void render(Model* grass, Animator* animator, Animation* animation, Shader& shader); 
	GrassChunk(glm::vec2 start, glm::vec2 stop, float scarcity);
	float const getDistanceToCam(glm::vec3& cameraPosition); 

};
class Grass {
public:
	
	std::vector<GrassChunk> grassChunks;
	glm::vec2 start = glm::vec2(0, 0);
	glm::vec2 stop = glm::vec2(2, 2);
	float scarcity = 0.5;
	

	Model* grass = NULL;
	Animator* animator = NULL; 
	Animation* animation = NULL; 
	glm::vec4 windDir; 
	float bendScale; 
	
	Grass() = default; 
	void initialize(); 
	void render( Camera& camera, Shader& shader); 

}; 