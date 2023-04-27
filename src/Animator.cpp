#include "Animator.h"

#pragma once

Animator::Animator(Animation* animation)
{
	m_CurrentTime = 0.0;
	m_CurrentAnimation = animation;
	//m_DeltaTime = 0.0; 

	m_FinalBoneMatrices.reserve(100);
	for (int i = 0; i < 100; i++) {
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f)); //Matrix initialization
	}

	UpdateAnimation(0); 
}

void Animator::UpdateAnimation(float timeChange)
{
	m_DeltaTime = timeChange; 

	if (m_CurrentAnimation) {
		m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * timeChange;
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
}

void Animator::PlayAnimation(Animation* pAnimation)
{
	m_CurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* bone = m_CurrentAnimation->FindBone(nodeName);

	if (bone)
	{
		bone->originX= origin; 
		bone->Update(m_CurrentTime);
		nodeTransform = bone->GetLocalTransform();
	}

	//Applying the LBS formula down the hierarchy
	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		if (index < m_FinalBoneMatrices.size() && index >= 0) {
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}
		//m_FinalBoneMatrices[index] = globalTransformation * offset;
		
	}

	for (int i = 0; i < node->childrenCount; i++) {
		CalculateBoneTransform(&node->children[i], globalTransformation);
	}
		
}

std::vector<glm::mat4> Animator::GetFinalBoneMatrices()
{
	return m_FinalBoneMatrices;
}

quat Animator::BendBranch(vec3 pos, vec3 branchOrigin, vec3 branchUp, float  branchNoise, vec3 windDir, float  windPower, Branch& b) {

	vec3 posInBranchSpace = pos - vec3(branchOrigin.x, branchOrigin.y, branchOrigin.z);
	float towardsX = dot(normalize(vec3(posInBranchSpace.x, 0, posInBranchSpace.z)), vec3(1, 0, 0));
	float facingWind = dot(normalize(vec3(posInBranchSpace.x, 0, posInBranchSpace.z)), windDir);
	float a = b.branchSwayPowerA * cos(b.deltaTime + branchNoise * b.branchMovementRandomization);
	float x = b.branchSwayPowerB * cos(b.timeWithDelay + branchNoise * b.branchMovementRandomization);
	float oldA = a;
	a = -0.5 * a + b.branchSuppressPower * b.branchSwayPowerA;
	x *= windPower;
	a = mix(oldA * windPower, a * windPower, b.delayedWindPower * clamp(1 - facingWind, 0.0f, 1.0f));
	vec3 windTangent = vec3(-windDir.z, windDir.y, windDir.x);
	quat rotation1 = angleAxis(a, windTangent);  
	
	quat rotation2 = angleAxis(x, vec3(0, 1, 0));
	return mix(rotation1, rotation2, 1 - abs(facingWind));

}

void Animator::UpdateAnimationTree(float timeChange)
{
	m_DeltaTime = timeChange;

	if (m_CurrentAnimation) {
		m_CurrentTime = glfwGetTime();
		//m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		CalculateBoneTransformTree(&m_CurrentAnimation->GetRootNode(), modelRotation);
	}
}

void Animator::CalculateBoneTransformTree(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* bone = m_CurrentAnimation->FindBone(nodeName);

	if (bone)
	{
		bone->UpdateTree(m_CurrentTime, windModel);
		nodeTransform = bone->GetLocalTransform();
	}

	//Applying the LBS formula down the hierarchy
	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	std::map<std::string, BoneInfo> boneInfoMap = m_CurrentAnimation->GetBoneIDMap();

	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		if (index < m_FinalBoneMatrices.size() && index >= 0) {
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}
		//m_FinalBoneMatrices[index] = globalTransformation * offset;

	}

	for (int i = 0; i < node->childrenCount; i++) {
		CalculateBoneTransformTree(&node->children[i], globalTransformation);
	}
}
