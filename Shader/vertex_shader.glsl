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
    gl_Position = projection * view * model * vec4(aPos, 1.0);		//���������
	FragPos = vec3(model * vec4(aPos,1.0));					//����任����������
	//���߾��󱻶���Ϊ��ģ�;������Ͻ�3x3���ֵ�������ת�þ���
	Normal = mat3(transpose(inverse(model))) * aNormal;		//ʹ�÷��߾���*����������Ҳ�䵽��������
}
	