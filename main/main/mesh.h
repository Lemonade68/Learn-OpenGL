#ifndef MESH_H
#define MESH_H

#include<glad\glad.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include"shader.h"

#include<string>
#include<vector>
using std::vector;
using std::string;

#define MAX_BONE_INFLUENCE 4

//����
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 TexCoords;

	//----------------------------------------------
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
	//----------------------------------------------
};

//����
struct Texture {
	unsigned int id;
	string type;
	string path;		//�洢�����·�������ں�����������бȽϣ�ʵ��������
};

class Mesh {
public:
	//��������
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;
	//����
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	void Draw(Shader shader);		//���ջ�������
private:
	//��Ⱦ����
	unsigned int VBO, EBO;
	//����
	void setupMesh();			//��ʼ������

};

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//����λ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	//���㷨��
	//offset�����������ṹ��ͱ����������ر�����ṹ��ͷ�����ֽ�ƫ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);
	//������������
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(2);

	//----------------------------------------------
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	// ids
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
	// weights
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
	//----------------------------------------------
}


void Mesh::Draw(Shader shader) {
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	//----------------------------------------------
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	//----------------------------------------------

	for (unsigned int i = 0; i < textures.size(); ++i) {
		glActiveTexture(GL_TEXTURE0 + i);	//��ǰ�����Ӧ����Ԫ(�Ӷ����԰󶨶������)TEXTUREI

		//��ȡ������ţ�diffuse_textureN �е� N��
		string number;
		string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		//----------------------------------------------
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); // transfer unsigned int to string
		else if (name == "texture_height")
			number = std::to_string(heightNr++); // transfer unsigned int to string
		//----------------------------------------------

		shader.use();
		shader.setInt((name + number).c_str(), i);	//����openGL��ǰshader�����sampler��ӦTEXTUREI�������Ԫ
		glBindTexture(GL_TEXTURE_2D, textures[i].id);		//��textures[i]��ʾ������󶨵���ǰ����ԪTEXTUREI��
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//�ڽ�����һ�лָ���Ĭ��״̬��һ����ϰ��
	glActiveTexture(GL_TEXTURE0);
}

#endif // !MESH_H