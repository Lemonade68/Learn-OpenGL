#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;
//�������Դ���vec4ʱ��ҪΪ��Ԥ��n��vec4��λ��  ����  mat4����4��vec4��λ��ֵ����3 4 5 6

out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoords = aTexCoords;
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0);
}