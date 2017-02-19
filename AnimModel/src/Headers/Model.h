/*
 * Model.h
 *
 *  Created on: 13/09/2016
 *      Author: rey
 */

#ifndef MODEL_H_
#define MODEL_H_

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Bones.h"

class Model {
public:
	Model();
	void loadModel(const std::string & path);
	void render(Shader * shader);
	void destroy();
	virtual ~Model();
	std::vector<Mesh> getMeshes() {
		return meshes;
	}
private:
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
			aiTextureType type, std::string typeName);

private:
	Assimp::Importer importer;
	const aiScene* scene;
	std::vector<Mesh> meshes;
	std::vector<Bones> vecBones;
	std::vector<Texture> textureLoaded;
	std::string directory;
	glm::mat4 m_GlobalInverseTransform;
	bool gammaCorrection;
};

#endif /* MODEL_H_ */
