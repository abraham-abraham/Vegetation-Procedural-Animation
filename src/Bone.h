#pragma once

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
#include "Branch.h"
#include "Wind.h"

/*Taken from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/bone.h*/
struct KeyPosition
{
	glm::vec3 position;
	float timeStamp;
};

struct KeyRotation
{
	glm::quat orientation;
	float timeStamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float timeStamp;
};

class Bone {
private:
	std::vector<KeyPosition> m_Positions;
	std::vector<KeyRotation> m_Rotations;
	std::vector<KeyScale> m_Scales;
	int m_NumPositions;
	int m_NumRotations;
	int m_NumScalings;

	glm::mat4 m_LocalTransform;
	std::string m_Name;
	int m_ID;

	
public:
	glm::vec3 originX = glm::vec3(1, 0, 0);

	Bone(const std::string& name, int ID, const aiNodeAnim* channel);
	void Update(float animationTime);
	glm::mat4 GetLocalTransform() { return m_LocalTransform; }
	std::string GetBoneName() const { return m_Name; }
	int GetBoneID() { return m_ID; }
	int GetPositionIndex(float animationTime);
	int GetRotationIndex(float animationTime);
	int GetScaleIndex(float animationTime); 
	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
	glm::mat4 InterpolatePosition(float animationTime);
	glm::mat4 InterpolateRotation(float animationTime);
	glm::mat4 InterpolateScaling(float animationTime);

	glm::quat BendBranch(glm::vec3 pos, glm::vec3 branchOrigin, glm::vec3 branchUp, float branchNoise, glm::vec3 windDir, float windPower);

	Branch* associatedBranch; 

	void UpdateTree(float animationTime, Wind* windModel);

};


