#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 Normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;

void main()
{
    gl_Position = view * model * vec4(aPos, 1.0);

	//法线矩阵被定义为：模型矩阵左上角3x3部分的逆矩阵的转置矩阵
	vs_out.Normal = normalize(mat3(transpose(inverse(model))) * aNormal);		//使用法线矩阵*将法线向量也变到世界坐标

}