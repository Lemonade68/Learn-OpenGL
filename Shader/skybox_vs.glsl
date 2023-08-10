#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;			//�������λ��������Ϊ�����Ƭ����ɫ������������
    vec4 pos = projection * view * vec4(aPos, 1.0);
	gl_Position = pos.xyww;		//ʹ�þ���͸�ӳ�����skybox��zֵ�����ֵ����ԶΪ1.0���������ֵ
//	gl_Position = projection * view * vec4(aPos, 1.0);
}