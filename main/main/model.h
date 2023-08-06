#ifndef MODEL_H
#define MODEL_H

#include<glad\glad.h>

#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<assimp\Importer.hpp>
#include<assimp\scene.h>
#include<assimp\postprocess.h>

#include"stb_image.h"
#include"mesh.h"
#include"shader.h"

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<map>
#include<vector>

using std::string;
using std::vector;

//加载一个纹理，并返回其ID
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model {
public:
	//模型数据
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;	//存储被加载的texture，用于优化
	bool gammaCorrection;

	// constructor, expects a filepath to a 3D model.
	Model(string const &path, bool gamma = false) : gammaCorrection(gamma) {
		loadModel(path);
	}
	void Draw(Shader shader);

private:
	//函数
	void loadModel(string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);

};

void Model::Draw(Shader shader) {
	for (unsigned int i = 0; i < meshes.size(); ++i) {
		meshes[i].Draw(shader);
	}
}

void Model::loadModel(string path) {
	//加载模型至scene这一数据结构中
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR:ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	//保存相对地址的目录
	directory = path.substr(0, path.find_last_of('/'));

	//处理场景中的所有结点（通过递归函数）
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
	//处理节点的所有网格（如果有的话）
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	//对其子节点重复这一过程
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
	//三个数据，用于创建一个Mesh对象
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;

		//处理顶点位置
		glm::vec3 vec;
		vec.x = mesh->mVertices[i].x;
		vec.y = mesh->mVertices[i].y;
		vec.z = mesh->mVertices[i].z;
		vertex.Position = vec;

		//处理法线
		if (mesh->HasNormals()) {
			vec.x = mesh->mNormals[i].x;
			vec.y = mesh->mNormals[i].y;
			vec.z = mesh->mNormals[i].z;
			vertex.Normal = vec;
		}

		//处理纹理坐标(只关心第一组纹理坐标)
		if (mesh->mTextureCoords[0]) {		//是否真的包含坐标
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords.x = vec.x;
			vertex.TexCoords.y = vec.y;
		}
		else {
			vertex.TexCoords.x = 0.0f;
			vertex.TexCoords.y = 0.0f;
		}

		vertices.push_back(vertex);
	}

	//处理索引
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}

	//处理材质
	if (mesh->mMaterialIndex >= 0) {		//检查网格是否包含材质
		//aiMesh只包含了对材质的索引，真实材质包括在scene场景中
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		//加载漫反射贴图
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		//加载镜面光贴图
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	//生成新的Mesh类对象并返回
	return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName) {
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); ++j) {
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {		//string.data()和string.c_str()一样
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {		//如果纹理还没有被加载，则加载它
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);		//加入到已经加载的texture中
		}
	}
	return textures;
}

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma) {
	//默认模型文件中纹理文件的路径是相对路径
	string filename = string(path);
	filename = directory + '/' + filename;

	//生成纹理
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

#endif // !MODEL_H

