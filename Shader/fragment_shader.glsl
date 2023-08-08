#version 330 core
out vec4 FragColor;

float near = 0.1; 
float far  = 100.0; 

in vec2 TexCoords;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

uniform sampler2D texture1;

void main() {    
	vec4 texColor = texture(texture1, TexCoords);
	if(texColor.a < 0.2)			//����͸����С��0.1�Ĳ��֣�90%����͸���Ĳ��֣�
		discard;
    FragColor = texture(texture1, TexCoords);

	//�����Ի���zֵ������-----------------------------------------------------
	//Զ�����Ǵ��ף�ֻ�кܽӽ�ʱ�Ż���
//    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);		

	//���Ի���zֵ������-------------------------------------------------------
	//����ʱ���𽥱�ڣ�Զ��ʱ�𽥱��
//	float depth = LinearizeDepth(gl_FragCoord.z) / far; // Ϊ����ʾ���� far
//    FragColor = vec4(vec3(depth), 1.0);
}