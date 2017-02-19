/*
 * Bone.cpp
 *
 *  Created on: 02/12/2016
 *      Author: rey
 */

#include "Headers/Bones.h"
#include "Headers/mathUtil.h"

Bones::Bones(GLuint VAO, unsigned int numVertex) {
	this->m_NumBones = 0;
	this->VAO = VAO;
	this->VBO = 0;
	bones.resize(numVertex);
}

Bones::~Bones() {
}

void Bones::loadBones(uint meshIndex, const aiMesh* pMesh) {
	for (uint i = 0; i < pMesh->mNumBones; i++) {
		uint boneIndex = 0;
		std::string boneName(pMesh->mBones[i]->mName.data);

		if (m_BoneMapping.find(boneName) == m_BoneMapping.end()) {
			boneIndex = m_NumBones;
			m_NumBones++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
			glm::mat4 mat;
			CopyMat(pMesh->mBones[i]->mOffsetMatrix, mat);
			m_BoneInfo[boneIndex].boneOffset = mat;
			m_BoneMapping[boneName] = boneIndex;
		} else
			boneIndex = m_BoneMapping[boneName];
		for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
			uint vertexID = pMesh->mBones[i]->mWeights[j].mVertexId;
			float weight = pMesh->mBones[i]->mWeights[j].mWeight;
			bones[vertexID].AddBoneData(boneIndex, weight);
		}
	}

	glBindVertexArray(this->VAO);
	glGenBuffers(1, &this->VBO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bones[0]) * bones.size(), &bones[0],
	GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData),
			(const GLvoid*) 0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData),
			(const GLvoid*) 16);

	glBindVertexArray(0);

}

void Bones::bonesTransform(float timeInSeconds,
		std::vector<glm::mat4>& transforms, const aiScene* scene) {

	glm::mat4 identity;

	float ticksPerSecond = (float) (
			scene->mAnimations[0]->mTicksPerSecond != 0 ?
					scene->mAnimations[0]->mTicksPerSecond : 25.0f);
	float timeInTicks = timeInSeconds * ticksPerSecond;
	float animationTime = fmod(timeInTicks,
			(float) scene->mAnimations[0]->mDuration);

	readNodeHeirarchy(animationTime, scene, scene->mRootNode, identity);

	transforms.resize(m_NumBones);

	for (uint i = 0; i < m_NumBones; i++) {
		transforms[i] = m_BoneInfo[i].finalTransformation;
	}

}

void Bones::readNodeHeirarchy(float animationTime, const aiScene* scene,
		const aiNode* pNode, const glm::mat4 & parentTransform) {

	std::string nodeName(pNode->mName.data);

	const aiAnimation* pAnimation = scene->mAnimations[0];

	glm::mat4 nodeTransformation;
	CopyMat(pNode->mTransformation, nodeTransformation);

	const aiNodeAnim* pNodeAnim = findNodeAnim(pAnimation, nodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		calcInterpolatedScaling(Scaling, animationTime, pNodeAnim);
		glm::mat4 ScalingM;
		ScalingM = glm::scale(ScalingM,
				glm::vec3(Scaling.x, Scaling.y, Scaling.z));

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		calcInterpolatedRotation(RotationQ, animationTime, pNodeAnim);
		glm::mat4 RotationM;
		aiMatrix4x4 RotationMA = aiMatrix4x4(RotationQ.GetMatrix());
		CopyMat(RotationMA, RotationM);

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		calcInterpolatedPosition(Translation, animationTime, pNodeAnim);
		glm::mat4 TranslationM;
		TranslationM = glm::translate(TranslationM,
				glm::vec3(Translation.x, Translation.y, Translation.z));

		// Combine the above transformations
		nodeTransformation = TranslationM * RotationM * ScalingM;
	}

	glm::mat4 GlobalTransformation = parentTransform * nodeTransformation;

	if (m_BoneMapping.find(nodeName) != m_BoneMapping.end()) {
		uint boneIndex = m_BoneMapping[nodeName];
		m_BoneInfo[boneIndex].finalTransformation = GlobalTransformation
				* m_BoneInfo[boneIndex].boneOffset;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) {
		readNodeHeirarchy(animationTime, scene, pNode->mChildren[i],
				GlobalTransformation);
	}

}

const aiNodeAnim* Bones::findNodeAnim(const aiAnimation* pAnimation,
		const std::string nodeName) {
	for (uint i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == nodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}

uint Bones::findPosition(float AnimationTime, const aiNodeAnim* pNodeAnim) {
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float) pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

uint Bones::findRotation(float AnimationTime, const aiNodeAnim* pNodeAnim) {
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float) pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

uint Bones::findScaling(float AnimationTime, const aiNodeAnim* pNodeAnim) {
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float) pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

void Bones::calcInterpolatedPosition(aiVector3D& Out, float AnimationTime,
		const aiNodeAnim* pNodeAnim) {
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = findPosition(AnimationTime, pNodeAnim);
	uint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float) (pNodeAnim->mPositionKeys[NextPositionIndex].mTime
			- pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime
			- (float) pNodeAnim->mPositionKeys[PositionIndex].mTime)
			/ DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void Bones::calcInterpolatedRotation(aiQuaternion& Out, float AnimationTime,
		const aiNodeAnim* pNodeAnim) {
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = findRotation(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float) (pNodeAnim->mRotationKeys[NextRotationIndex].mTime
			- pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime
			- (float) pNodeAnim->mRotationKeys[RotationIndex].mTime)
			/ DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ =
			pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ =
			pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

void Bones::calcInterpolatedScaling(aiVector3D& Out, float AnimationTime,
		const aiNodeAnim* pNodeAnim) {
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = findScaling(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float) (pNodeAnim->mScalingKeys[NextScalingIndex].mTime
			- pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime
			- (float) pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

