#version 330 core
layout(triangles) in;			//��������������ͼԪ��GL_TRIANGLE��
layout(triangle_strip, max_vertices = 3) out;	
//��ȥ�Ļ��������Σ�����ඥ����Ϊ3������һ�������γ�ȥһ�������Σ�

in VS_OUT{
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
} gs_in[];		//ʹ�����飺һ��������Ƕ������

out vec2 TexCoords;		//���ݸ�Ƭ����ɫ���Ĳ����������ڶ����Ƭ���м䣩
out vec3 FragPos;
out vec3 Normal;

uniform float time;		//��ȡʱ��

vec4 explode(vec4 position, vec3 normal){
	float magnitude = 2.0;
	//ÿ�ζ��Ǵ�ԭλ�ó�����������Ⱦ�µ�λ�ã��ﵽ�����任��Ч����
	vec3 direction = normal * ((sin(time)+1.0)/2.0) * magnitude;
	return position + vec4(direction, 1.0);
}

vec3 GetNormal(){
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));			//ע��a,b˳�� ���� ���Ļ������෴�����ƶ�
}

void main() {
	vec3 normal = GetNormal();

	//����0
	gl_Position = explode(gl_in[0].gl_Position, normal);
    TexCoords = gs_in[0].TexCoords;
	FragPos = gs_in[0].FragPos;
//	FragPos = vec3(explode(vec4(gs_in[0].FragPos, 1.0), normal));
	Normal = gs_in[0].Normal;
    EmitVertex();		//���䣨����������gl_position���Ҵ��ݸ�Ƭ����ɫ��ԭ�еĲ�����
    
	//����1
	gl_Position = explode(gl_in[1].gl_Position, normal);
    TexCoords = gs_in[1].TexCoords;
	FragPos = gs_in[1].FragPos;
//	FragPos = vec3(explode(vec4(gs_in[1].FragPos, 1.0), normal));
	Normal = gs_in[1].Normal;
    EmitVertex();

	//����2
    gl_Position = explode(gl_in[2].gl_Position, normal);
    TexCoords = gs_in[2].TexCoords;
	FragPos = gs_in[2].FragPos;
//	FragPos = vec3(explode(vec4(gs_in[2].FragPos, 1.0), normal));
	Normal = gs_in[2].Normal;
    EmitVertex();

    EndPrimitive();		
	//ʹ��endprimitiveʱ�����з���Ķ����ϳ�Ϊָ���������ȾͼԪ
	//�ظ�����EndPrimitive�ܹ����ɶ��ͼԪ
}
	