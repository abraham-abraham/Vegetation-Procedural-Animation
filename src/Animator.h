#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include <GL/glew.h>  
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Animation.h"

#include "Branch.h"

#include "Wind.h"

class Animator {

private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;
public:
	Animator(Animation* animation);
	void UpdateAnimation(float timeChange);
	void PlayAnimation(Animation* pAnimation);
	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
	std::vector<glm::mat4> GetFinalBoneMatrices();

	quat BendBranch(vec3 pos, vec3 branchOrigin, vec3 branchUp, float branchNoise, vec3 windDir, float windPower, Branch& b);

	void UpdateAnimationTree(float timeChange);
	void CalculateBoneTransformTree(const AssimpNodeData* node, glm::mat4 parentTransform);


	Wind* windModel; 

	glm::vec3 origin = glm::vec3(0, 0, 0);
	glm::mat4 modelRotation = glm::mat4(1.0f); 



};