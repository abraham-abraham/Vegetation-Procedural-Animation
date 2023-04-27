#pragma once

#include<glm/glm.hpp>

struct BoneInfo
{
	int id; //index in finalBoneMatrices

	glm::mat4 offset;

};
