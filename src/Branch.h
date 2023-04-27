#pragma once
//Including OpenGL Libraries:

#include <GL/glew.h>  
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  
#include "glm/gtc/matrix_transform.hpp"

class Branch
{
public:
	
	 float  branchSwayPowerA;
	 float  branchSwayPowerB;
	 float  branchMovementRandomization;
	 float  deltaTime;
	 float  timeWithDelay;
	 float  branchSuppressPower;
	 float  delayedWindPower;
};