#pragma once
//Including OpenGL Libraries:

#include <GL/glew.h>  
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  
#include "glm/gtc/matrix_transform.hpp"
class Wind
{
public:
	Wind() = default; 
	float intensity;
	float direction[3];

	glm::mat3 directionField; 

	//Shader parameters:

	glm::vec4 vWind = glm::vec4(0, 0, 0, 0);
	float fBendScale = 0;
	float fLength = 0;
	float fTimeScale = 1; 
	//glm::vec3 vWindPosition = glm::vec3(-10.0f, 5.0f, 0.0f);

	glm::vec3 vWindPosition = glm::vec3(0.0f, 0.0f, 0.0f);

	

	float SmoothCurve(float x) { return x * x * (3.0 - 2.0 * x); }
	float TriangleWave(float x) { return abs(glm::fract(x + 0.5) * 2.0 - 1.0); }
	float SmoothTriangleWave(float x) { return SmoothCurve(TriangleWave(x)); }

	float random(glm::vec2 st) { //2D Perlin random generator
		return glm::fract(sin(dot(glm::vec2(st.x, st.y),
			glm::vec2(12.9898, 78.233)))
			* 43758.5453123);
	}

	float generatePerlin(glm::vec2 st) {

		glm::vec2 i = floor(st);
		glm::vec2 f = fract(st);

		float a = random(i);
		float b = random(i + glm::vec2(1.0, 0.0));
		float c = random(i + glm::vec2(0.0, 1.0));
		float d = random(i + glm::vec2(1.0, 1.0));
		glm::vec2 u = f * f * (glm::vec2(3, 3) - glm::vec2(2, 2) * f);

		return glm::mix(a, b, u.x) +
			(c - a) * u.y * (1.0 - u.x) +
			(d - b) * u.x * u.y;


	}
	
};