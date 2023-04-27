#include "Bone.h"

/*Code logic taken from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/bone.h*/

Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
{
	m_Name = name;
	m_ID = ID; 
	m_LocalTransform = glm::mat4(1.0f);

	m_NumPositions = channel->mNumPositionKeys;

	for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
	{
		aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
		float timeStamp = channel->mPositionKeys[positionIndex].mTime;
		KeyPosition data;
		data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
		data.timeStamp = timeStamp;
		m_Positions.push_back(data);
	}

	m_NumRotations = channel->mNumRotationKeys;
	for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
	{
		aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
		float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
		KeyRotation data;
		data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
		data.timeStamp = timeStamp;
		m_Rotations.push_back(data);
	}

	m_NumScalings = channel->mNumScalingKeys;
	for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
	{
		aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
		float timeStamp = channel->mScalingKeys[keyIndex].mTime;
		KeyScale data;
		data.scale = AssimpGLMHelpers::GetGLMVec(scale);
		data.timeStamp = timeStamp;
		m_Scales.push_back(data);
	}
}

void Bone::Update(float animationTime)
{

	glm::mat4 translation = InterpolatePosition(animationTime);
	glm::mat4 rotation = InterpolateRotation(animationTime);
	glm::mat4 scale = InterpolateScaling(animationTime);

	m_LocalTransform = translation * rotation * scale;

	//Tree Bending:
	if (associatedBranch != NULL) {
		//glm::quat branchRotation = BendBranch();

		//m_LocalTransform = glm::toMat4(branchRotation) * m_LocalTransform;
	}

	
}

int Bone::GetPositionIndex(float animationTime)
{
	for (int index = 0; index < m_NumPositions - 1; ++index)
	{
		if (animationTime < m_Positions[index + 1].timeStamp)
			return index;
	}
	assert(0);
	return 0;
}

int Bone::GetRotationIndex(float animationTime)
{
	for (int index = 0; index < m_NumRotations - 1; ++index)
	{
		if (animationTime < m_Rotations[index + 1].timeStamp)
			return index;
	}
	assert(0);
	return 0;
}

int Bone::GetScaleIndex(float animationTime)
{
	for (int index = 0; index < m_NumScalings - 1; ++index)
	{
		if (animationTime < m_Scales[index + 1].timeStamp)
			return index;
	}
	assert(0);
	return 0;
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	float scaleFactor = 0.0f;
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	scaleFactor = midWayLength / framesDiff;
	return scaleFactor;
}

glm::mat4 Bone::InterpolatePosition(float animationTime)
{
	if (1 == m_NumPositions)
		return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

	int p0Index = GetPositionIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
		m_Positions[p1Index].timeStamp, animationTime);
	glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position
		, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime)
{
	if (1 == m_NumRotations)
	{
		auto rotation = glm::normalize(m_Rotations[0].orientation);
		return glm::toMat4(rotation);
	}

	int p0Index = GetRotationIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
		m_Rotations[p1Index].timeStamp, animationTime);
	glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation
		, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::toMat4(finalRotation);
}

glm::mat4 Bone::InterpolateScaling(float animationTime)
{
	if (1 == m_NumScalings)
		return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

	int p0Index = GetScaleIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
		m_Scales[p1Index].timeStamp, animationTime);
	glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale
		, scaleFactor);
	return glm::scale(glm::mat4(1.0f), finalScale);
}


glm::quat Bone::BendBranch(glm::vec3 pos, glm::vec3 branchOrigin, glm::vec3 branchUp, float  branchNoise, glm::vec3 windDir, float  windPower) {

	//glm::vec3 posInBranchSpace = pos - glm::vec3(branchOrigin.x, branchOrigin.y, branchOrigin.z);

	glm::vec3 posInBranchSpace = pos;
	float towardsX = glm::dot((glm::vec3(posInBranchSpace.x, 0, posInBranchSpace.z)),glm::vec3(1, 0, 0));
	float facingWind = glm::dot((glm::vec3(posInBranchSpace.x, 0, posInBranchSpace.z)), windDir);
	float a = branchNoise;
	float x = branchNoise;
	float oldA = a;
	a = -0.5 * a + 1 *1;
	x *= windPower;
	a = glm::mix(oldA * windPower, a * windPower, windPower * glm::clamp(1 - facingWind, 0.0f, 1 - facingWind));
	glm::vec3 windTangent = glm::vec3(-windDir.z, windDir.y, windDir.x);
	glm::quat rotation1 = angleAxis(a, windTangent);

	glm::quat rotation2 = glm::angleAxis(x, glm::vec3(0, 1, 0));

	glm::quat mixedResult = mix(rotation1, rotation2, 1 - abs(facingWind));

	
	return mixedResult;

}

void Bone::UpdateTree(float animationTime, Wind* windModel)
{
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Positions[0].position);
	glm::mat4 rotation = glm::toMat4(normalize(m_Rotations[0].orientation));
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

	//float noise = windModel->generatePerlin(glm::vec2(dot(windModel->vWind, glm::vec4(m_Positions[0].position, 1)), animationTime));
	float animation = cos(glfwGetTime() * windModel->fTimeScale);

	glm::quat resultInBending = BendBranch(m_Positions.at(0).position, m_Positions.at(0).position, glm::eulerAngles(m_Rotations.at(0).orientation), animation,
		glm::vec3(windModel->vWind.x, windModel->vWind.y, windModel->vWind.z), windModel->fBendScale);

	glm::mat4 resultingBend = glm::toMat4(resultInBending);

	m_LocalTransform = translation * rotation * scale * resultingBend;

	//m_LocalTransform = translation * rotation * scale;


}
