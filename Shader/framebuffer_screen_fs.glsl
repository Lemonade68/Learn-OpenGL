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
//	//1.ԭЧ��
//    vec3 col = texture(screenTexture, TexCoords).rgb;
//    FragColor = vec4(col, 1.0);

//	//2.��ɫЧ����
//	FragColor = vec4(vec3(1.0 - texture(screenTexture,TexCoords)), 1.0);

//	//3.�ҶȻ������� �� ��Ȩ
//	//����-------
//	FragColor = texture(screenTexture, TexCoords);
//    float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
//    FragColor = vec4(average, average, average, 1.0);
//	//��Ȩ-------
//	FragColor = texture(screenTexture, TexCoords);
//    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
//    FragColor = vec4(average, average, average, 1.0);

	//4.��Ч��

	vec3 sampleTex[9];
    for(int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++) {
        col += sampleTex[i] * kernel_Sharpen[i];		//��Ч��
//        col += sampleTex[i] * kernel_Border[i];		//��Ե���Ч��
//		col += sampleTex[i] * kernel_Blurring[i];		//ģ��Ч�� 

	}

    FragColor = vec4(col, 1.0);

} 