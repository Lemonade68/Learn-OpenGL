#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;  

vec2 offsets[9] = vec2[](	//ƫ��������
    vec2(-offset,  offset), // ����
    vec2( 0.0f,    offset), // ����
    vec2( offset,  offset), // ����
    vec2(-offset,  0.0f),   // ��
    vec2( 0.0f,    0.0f),   // ��
    vec2( offset,  0.0f),   // ��
    vec2(-offset, -offset), // ����
    vec2( 0.0f,   -offset), // ����
    vec2( offset, -offset)  // ����
);

float kernel_Sharpen[9] = float[](	//��1 ���� �������񻯺ˣ�������ɫֵ���񻯣�ԭ���ٿ�����
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
);

float kernel_Blurring[9] = float[](	//��2 ���� ģ���ˣ����ȨֵΪ1�������ܵĻ�ϣ�
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

float kernel_Border[9] = float[](	//��3 ���� ��Ե����(������Ϊ0����ͬ�Ļ�����������Ե������󣬲�������)
	-1, -1, -1,
    -1,  8, -1,
    -1, -1, -1
);

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(col, 1.0);

} 