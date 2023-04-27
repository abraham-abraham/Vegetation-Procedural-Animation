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

#define MAX_PARTICLES 25
#define WCX		2
#define WCY		2



struct Particle {
    bool alive;	
    float life;	// particle lifespan
    float fade; // decay
    
   
    glm::vec3 position; 
  
    float vel_y;
   
    float gravity;
};




class Rain {
public:

 
   
	float slowdown = 1.0;
	float velocity = 5.0;
    int maxParticles = 1; 
    int rainEffect = 1;
    int maxRainEffect = 10; 

    glm::vec3 positionsToNeutralize[4] = { glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-9.0f, -0.25f, -5.0f) ,glm::vec3(-5.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 5.0f) };
	
	
    Particle par_sys[MAX_PARTICLES][MAX_PARTICLES];

    Rain();
    void initialize();

    void initializeHelper(int i, int j);

    void drawRain(Model& m, Shader& s);


   




};