#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

//�ĳ������   (��������ɫ��ʹ��)
out VS_OUT{
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
} vs_out;

uniform mat4 model;

//ʹ��uniform buffer object
layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

	vs_out.FragPos = vec3(model * vec4(aPos,1.0));					//����任����������
	//���߾��󱻶���Ϊ��ģ�;������Ͻ�3x3���ֵ�������ת�þ���
	vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;		//ʹ�÷��߾���*����������Ҳ�䵽��������
	vs_out.TexCoords = aTexCoords;
}