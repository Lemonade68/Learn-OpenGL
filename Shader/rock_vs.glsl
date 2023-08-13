#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;
//顶点属性大于vec4时，要为其预留n个vec4的位置  ——  mat4就是4个vec4，位置值就是3 4 5 6

out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoords = aTexCoords;
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0);
}