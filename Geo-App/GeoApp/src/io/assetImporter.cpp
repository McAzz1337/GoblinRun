#include "assetimporter.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include "../model_data/material.h"
#include "../model_data/vertex.h"
#include "../model_data/texture.h"
#include "fileio.h"

#include <vector>
#include <unordered_map>
#include <iostream>

#define MODEL_DIR "models/"
#define SHADER_DIR "shaders/"

namespace assetimporter {

	/*
	std::unordered_map<std::string, AssetType> textureLiterals;
	AssetType convertTextureType(aiTextureType* assimpTextureType, std::string* customTextureType);

	void init() {
		textureLiterals.insert(std::make_pair("diffuse", AssetType::DIFFUSE1));
		textureLiterals.insert(std::make_pair("shader", AssetType::SHADER));
		textureLiterals.insert(std::make_pair("bump", AssetType::BUMP_MAP));
	}
	*/

	std::string checkTexturePath(const std::string& aPath,  const std::string& dir) {

		printf("Looking at: %s\n", aPath.c_str());
		int start = aPath.find_last_of('\\');
		std::string retPath;
		if (start == std::string::npos) {
			retPath = dir + "/" + aPath;
		} else {
			retPath = dir + "/" + aPath.substr(start + 1);
		}

		printf("returning: %s\n", retPath.c_str());
		return retPath;
	}

	std::vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type, std::string name) {

		std::vector<Texture> textures;
		for (int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString path;
			mat->GetTexture(type, i, &path);

			textures.push_back(Texture(std::string(path.C_Str())));
		}
		return textures;
	}

	// Lädt ein 3D-Modell
	void loadModel(const std::string& dir,
				   const std::string& file,
				   std::vector<Vertex>& vertices,
				   std::vector<uint32_t>& indices,
				   Material& material) {

		std::string fullPath = dir + "/" + file;
		const aiScene* scene = aiImportFile(fullPath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
		//std::unordered_map<AssetType, std::string> assets;

		if (!scene) {
			printf("Failed to load View Model: %s\n", fullPath.c_str());
			printf("Error:\n%s\n", aiGetErrorString());
			return;
		}




		for (uint32_t i = 0; i < scene->mNumMeshes; i++) {

			aiMesh* mesh = scene->mMeshes[i];

			vertices.reserve(mesh->mNumVertices);

			// Oberflächenwerte des Materiales laden
			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
			aiColor4D specular;
			aiColor4D diffuse;
			aiColor4D ambient;
			float shininess;
			float specularStrength;
			float roughness;
			float metallic;

			aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &specular);
			aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
			aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &ambient);
			aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shininess);
			aiGetMaterialFloat(mat, AI_MATKEY_SPECULAR_FACTOR, &specularStrength);
			aiGetMaterialFloat(mat, AI_MATKEY_ROUGHNESS_FACTOR, &roughness);
			aiGetMaterialFloat(mat, AI_MATKEY_METALLIC_FACTOR, &metallic);

			auto spec = glm::vec4(specular.r, specular.g, specular.b, specular.a);
			auto diff = glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
			auto amb = glm::vec4(ambient.r, ambient.g, ambient.b, ambient.a);
			Material::fromValues(material, spec, diff, amb, specularStrength, roughness, metallic);
			/*
			material = Material(
				{ specular.r, specular.g, specular.b, shininess },
				{ diffuse.r, diffuse.g, diffuse.b, diffuse.a },
				{ ambient.r, ambient.g, ambient.b, ambient.a },
				roughness, specularStrength, metallic
			);
			*/
			// Scheitelpunktinformationen laden
			for (uint32_t j = 0; j < mesh->mNumVertices; j++) {

				aiVector3D pos = mesh->mVertices[j];
				aiVector3D normal = mesh->mNormals[j];
				/*
				aiVector3D tangent;
				if (j < mesh->mTangents->Length()) {
					tangent  = mesh->mTangents[j];
				} 
				aiVector3D biTangent;
				if (j < mesh->mBitangents->Length()) {
					biTangent = mesh->mBitangents[j];
				}
				*/

				glm::vec2 uv(0.0f);
				if (mesh->mTextureCoords[0]) {
					aiVector3D texCoords = mesh->mTextureCoords[0][j];
					uv.x = texCoords.x;
					uv.y = texCoords.y;
				}

				vertices.push_back(
					Vertex(
					glm::vec3(pos.x, pos.y, pos.z),
					uv,
					glm::vec3(normal.x, normal.y, normal.z)
				)
				);

			}

			indices.reserve(mesh->mNumFaces * 3u);
			for (uint32_t j = 0; j < mesh->mNumFaces; j++) {

				if (mesh->mFaces[j].mNumIndices < 3u) continue;

				assert(mesh->mFaces[j].mNumIndices == 3u);

				indices.push_back(mesh->mFaces[j].mIndices[0u]);
				indices.push_back(mesh->mFaces[j].mIndices[1u]);
				indices.push_back(mesh->mFaces[j].mIndices[2u]);

			}
			

			// Texturpfade auslesen
			if (mesh->mMaterialIndex >= 0) {
				aiMaterial* ai_mat = scene->mMaterials[mesh->mMaterialIndex];

				std::vector<Texture> diffuseMap = loadTextures(ai_mat, aiTextureType_SPECULAR, "diffuse_tex");
				std::vector<Texture> specularMap = loadTextures(ai_mat, aiTextureType_SPECULAR, "specular_tex");
			}


			for (uint32_t j = 0; j < scene->mNumMaterials; j++) {
				const aiMaterial* pMaterial = scene->mMaterials[j];
				if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
					aiString path;

					for (int i = 0; i < AI_TEXTURE_TYPE_MAX; i++) {
						if (pMaterial->GetTexture((aiTextureType) i, 0, &path) == AI_SUCCESS) {
							printf("Found: %s/%s for %i\n", dir.c_str(), path.C_Str(), i);
							auto type = (aiTextureType) i;
						}
					}
				}
			}

		}

		aiReleaseImport(scene);

	}
}