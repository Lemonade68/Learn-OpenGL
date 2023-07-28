#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;


void main() {
	//���Բ�ֵ��20% awesomeface, 80% container
	FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2,TexCoord),0.2);
}