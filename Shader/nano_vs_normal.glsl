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

	//���߾��󱻶���Ϊ��ģ�;������Ͻ�3x3���ֵ�������ת�þ���
	vs_out.Normal = normalize(mat3(transpose(inverse(model))) * aNormal);		//ʹ�÷��߾���*����������Ҳ�䵽��������

}