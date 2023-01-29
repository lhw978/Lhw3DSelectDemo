#pragma once
#ifndef MODEL_H
#define MODEL_H
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "mesh.h"  //#include "assimp/mesh.h"
#include "loadTexture.h"

//using namespace std; //���Ƽ���.hͷ�ļ��� ʹ��using�����׵���������ͻ
//unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model
{
public:
	Model(std::string path, bool gamma = false) : gammaCorrection(gamma)  //���캯�� constructor, expects a filepath to a 3D model.
	{
		loadModel(path);//����ģ�͵�λ��
	}

	void Draw(Shader shader)  //����ÿ�����񲢵��������Լ���draw����  draws the model, and thus all its meshes
	{
		for (unsigned int i = 0; i < meshes.size(); ++i)
		{
			shader.setInt("gDrawIndex", i);//�ѻ�������������ɫ��
			meshes[i].Draw(shader);
		}
	}

private:
	void loadModel(std::string const& path) //�����ļ��� loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	{
		Assimp::Importer importer;  //������Assimp�����ռ��ڵ�һ��Importer�� read file via ASSIMP
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace); //���ļ�����һ��������ظ�scene
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // check for errors�� if is Not Zero ��������Ƿ�����
		{
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
			return;
		}

		directory = path.substr(0, path.find_last_of('/')); //retrieve the directory path of the filepath: ����ʵ���Ͼ��ǻ�ȡ����/֮ǰ���Ƕ�·��   ������Ŀǰ����Ӧ����//
		processNode(scene->mRootNode, scene); // process ASSIMP's root node recursively ����һ���ڵ㣨���ڵ㣩����: �ݹ麯�� ��ͣ�ĵ������к��� ֱ��ȫ��������
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene) //�Խڵ��������д���
	{
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) //�����ﴦ������ڵ����������  node->mNumMeshesӦ���ǽڵ�����
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; //node->mMeshes[i]��ÿ���ڵ����������  ����ͨ���������������mMeshes��������ȡ��Ӧ������   ����˵���� �������нڵ� �ڵ������������� �ڵ��е�������������Ӧһ������ ����ͨ���ڵ��е��������������ҵ�ÿһ������
			meshes.push_back(processMesh(mesh, scene));   //��������񴫸�peocessMesh���� ����һ��mesh����洢��meshes��
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; ++i) //����������ڵ� ���Ǿʹ��������ӽڵ� ����ͬ�ķ�ʽ
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)  //���������� Ҳ�����������ÿ������ʼ������������
	{
		std::vector<Vertex> vertices;        //������������
		std::vector<unsigned int> indices;   //������������
		std::vector<Texture> textures;       //������������
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)  //Walk through each of the mesh's vertices: mesh->mNumVertices�����������
		{
			Vertex vertex; //���￪ʼ��ȡͨ�������ȡ����Ķ��� ����
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			vector.x = mesh->mVertices[i].x;	// positions
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
		
			vector.x = mesh->mNormals[i].x;	// normals
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			
			/*���￪ʼ��ȡ������������� �������������*/
			if (mesh->mTextureCoords[0]) // texture coordinates: does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		
			vector.x = mesh->mTangents[i].x;	// tangent
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
		
			vector.x = mesh->mBitangents[i].x;	// bitangent
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
			vertices.push_back(vertex);  //���￪ʼ����Щ���ݴ洢��vertoices
		}
		// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) //������������ÿ���棨��������������Σ���������Ӧ�Ķ�������
		{
			aiFace face = mesh->mFaces[i];		
			for (unsigned int j = 0; j < face.mNumIndices; ++j) // retrieve all indices of the face and store them in the indices vector
				indices.push_back(face.mIndices[j]);
		}

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];// process materials: �ӳ�����mMaterials�����л�ȡaiMaterial����
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN
		
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse"); // 1. diffuse maps
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");	// 2. specular maps
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");	// 3. normal maps
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");	// 4. height maps
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		return Mesh(vertices, indices, textures);	// return a mesh object created from the extracted mesh data
	}

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) //�������Ҳ����ͨ������ֱ����ģ����ÿ������ Ȼ��ͨ��������ȡ�����ǵ��ļ�λ����Ϣ Ȼ����д����� id  λ�� ���ͽ�ȥ������
	{
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)  //GetTextureCount������鴢���ڲ��������������
		{
			aiString str;
			mat->GetTexture(type, i, &str); //��ȡÿ��������ļ�λ��
		
			bool skip = false; 	// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			for (unsigned int j = 0; j < textures_loaded.size(); ++j)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}

			if (!skip)  //����Ҳ���Ƿ�ֹ�ظ���������:  if texture hasn't been loaded already, load it
			{  
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessarily load duplicate textures.
			}
		}
		return textures;
	}

private:
    std::vector<Texture> textures_loaded;  //�������������stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes;              //�������������
	std::string directory;                 //ģ�ʹ�ŵĵط�

    bool gammaCorrection;	
};

#endif