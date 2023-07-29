#version 330 core
out vec4 FragColor;

//in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float transparancy;


void main() {
	//线性插值：20% awesomeface, 80% container
	//笑脸往另一个方向看：
//	FragColor = mix(texture(texture1, TexCoord), texture(texture2,vec2(-TexCoord.x,TexCoord.y)),0.2);
	FragColor = mix(texture(texture1, TexCoord), texture(texture2,TexCoord), transparancy);
//	FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);
}