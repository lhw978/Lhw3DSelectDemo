#pragma once
#ifndef MODEL_H
#define MODEL_H
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "mesh.h"  //#include "assimp/mesh.h"
#include "loadTexture.h"

//using namespace std; //不推荐在.h头文件中 使用using，容易导致命名冲突
//unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model
{
public:
	Model(std::string path, bool gamma = false) : gammaCorrection(gamma)  //构造函数 constructor, expects a filepath to a 3D model.
	{
		loadModel(path);//加载模型的位置
	}

	void Draw(Shader shader)  //遍历每个网格并调用他们自己的draw函数  draws the model, and thus all its meshes
	{
		for (unsigned int i = 0; i < meshes.size(); ++i)
		{
			shader.setInt("gDrawIndex", i);//把绘制索引传给着色器
			meshes[i].Draw(shader);
		}
	}

private:
	void loadModel(std::string const& path) //加载文件： loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	{
		Assimp::Importer importer;  //声明了Assimp命名空间内的一个Importer， read file via ASSIMP
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace); //把文件按照一定规则加载给scene
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // check for errors： if is Not Zero 检测数据是否完整
		{
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
			return;
		}

		directory = path.substr(0, path.find_last_of('/')); //retrieve the directory path of the filepath: 这里实际上就是获取到了/之前的那段路径   但是我目前觉得应该是//
		processNode(scene->mRootNode, scene); // process ASSIMP's root node recursively 将第一个节点（根节点）传入: 递归函数 不停的调用所有函数 直到全部调用完
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene) //对节点和网格进行处理
	{
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) //在这里处理这个节点的所有网格  node->mNumMeshes应该是节点总数
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; //node->mMeshes[i]是每个节点的网格索引  并且通过这个索引场景的mMeshes数组来获取对应的网格   简单来说就是 场景中有节点 节点中有网格索引 节点中的网格索引都对应一个网格 场景通过节点中的网格索引可以找到每一个网格
			meshes.push_back(processMesh(mesh, scene));   //把这个网格传给peocessMesh函数 返回一个mesh对象存储在meshes中
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; ++i) //处理完这个节点 我们就处理他的子节点 用相同的方式
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)  //处理网格函数 也就是在这里对每个网格开始处理他的数据
	{
		std::vector<Vertex> vertices;        //顶点向量对象
		std::vector<unsigned int> indices;   //索引向量对象
		std::vector<Texture> textures;       //纹理向量对象
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)  //Walk through each of the mesh's vertices: mesh->mNumVertices网格的总数量
		{
			Vertex vertex; //这里开始获取通过网格获取网格的顶点 法线
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			vector.x = mesh->mVertices[i].x;	// positions
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
		
			vector.x = mesh->mNormals[i].x;	// normals
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			
			/*这里开始获取网格的纹理坐标 如果有纹理坐标*/
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
			vertices.push_back(vertex);  //这里开始把这些数据存储进vertoices
		}
		// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) //这里遍历网格的每个面（面是网格的三角形）并检索相应的顶点索引
		{
			aiFace face = mesh->mFaces[i];		
			for (unsigned int j = 0; j < face.mNumIndices; ++j) // retrieve all indices of the face and store them in the indices vector
				indices.push_back(face.mIndices[j]);
		}

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];// process materials: 从场景的mMaterials数组中获取aiMaterial对象
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
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) //这个函数也就是通过网格直到了模型中每个纹理 然后通过函数获取到他们的文件位置信息 然后填写纹理的 id  位置 类型进去就行了
	{
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)  //GetTextureCount函数检查储存在材质中纹理的数量
		{
			aiString str;
			mat->GetTexture(type, i, &str); //获取每个纹理的文件位置
		
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

			if (!skip)  //这里也就是防止重复加载纹理:  if texture hasn't been loaded already, load it
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
    std::vector<Texture> textures_loaded;  //纹理的向量对象：stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes;              //网格的向量对象
	std::string directory;                 //模型存放的地方

    bool gammaCorrection;	
};

#endif