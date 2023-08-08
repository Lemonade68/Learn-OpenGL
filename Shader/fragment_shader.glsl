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
	if(texColor.a < 0.2)			//丢弃透明度小于0.1的部分（90%以上透明的部分）
		discard;
    FragColor = texture(texture1, TexCoords);

	//非线性化的z值处理结果-----------------------------------------------------
	//远处看是纯白，只有很接近时才会变黑
//    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);		

	//线性化的z值处理结果-------------------------------------------------------
	//靠近时会逐渐变黑，远离时逐渐变白
//	float depth = LinearizeDepth(gl_FragCoord.z) / far; // 为了演示除以 far
//    FragColor = vec4(vec3(depth), 1.0);
}