#version 330 core
layout(triangles) in;			//��������������ͼԪ��GL_TRIANGLE��
layout(line_strip, max_vertices = 6) out;	
//��ȥ���߶Σ�����ඥ����Ϊ6������һ�������γ�ȥ3���ߣ�ÿ�����������߷���һ���ߣ�

in VS_OUT{
	vec3 Normal;
} gs_in[];		//ʹ�����飺һ��������Ƕ������

const float MAGNITUDE = 0.02;

uniform mat4 projection;	//����ʹ��projection

void GenerateLine(int index){
	//�����Ķ���1��ԭ����
	//ע�⣺�����gl_Position�ǴӶ�����ɫ���ﴫ������
	gl_Position = projection * gl_in[index].gl_Position;
	EmitVertex();
	
	//�����Ķ���2��������һ�˶���
	gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].Normal, 0.0) * MAGNITUDE);
	EmitVertex();

	//������������������
	EndPrimitive();		//ʹ��Ƭ����ɫ��������
}

void main() {
	GenerateLine(0); // ��һ�����㷨��
    GenerateLine(1); // �ڶ������㷨��
    GenerateLine(2); // ���������㷨��
}
	