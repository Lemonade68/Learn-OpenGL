#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

//uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * model * vec4(aPos, 1.0);		//矩阵乘向量
	FragPos = vec3(model * vec4(aPos,1.0));					//顶点变换到世界坐标
	//法线矩阵被定义为：模型矩阵左上角3x3部分的逆矩阵的转置矩阵
	Normal = mat3(transpose(inverse(model))) * aNormal;		//使用法线矩阵*将法线向量也变到世界坐标
}
	