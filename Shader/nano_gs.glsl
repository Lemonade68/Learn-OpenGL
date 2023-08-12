#version 330 core
layout(triangles) in;			//进来的是三角形图元（GL_TRIANGLE）
layout(triangle_strip, max_vertices = 3) out;	
//出去的还是三角形，且最多顶点数为3（进来一个三角形出去一个三角形）

in VS_OUT{
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
} gs_in[];		//使用数组：一次输入的是多个顶点

out vec2 TexCoords;		//传递给片段着色器的参数（几何在顶点和片段中间）
out vec3 FragPos;
out vec3 Normal;

uniform float time;		//获取时间

vec4 explode(vec4 position, vec3 normal){
	float magnitude = 2.0;
	//每次都是从原位置出发，重新渲染新的位置（达到持续变换的效果）
	vec3 direction = normal * ((sin(time)+1.0)/2.0) * magnitude;
	return position + vec4(direction, 1.0);
}

vec3 GetNormal(){
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));			//注意a,b顺序 —— 反的话是向相反方向移动
}

void main() {
	vec3 normal = GetNormal();

	//顶点0
	gl_Position = explode(gl_in[0].gl_Position, normal);
    TexCoords = gs_in[0].TexCoords;
	FragPos = gs_in[0].FragPos;
//	FragPos = vec3(explode(vec4(gs_in[0].FragPos, 1.0), normal));
	Normal = gs_in[0].Normal;
    EmitVertex();		//发射（重新设置了gl_position，且传递给片段着色器原有的参数）
    
	//顶点1
	gl_Position = explode(gl_in[1].gl_Position, normal);
    TexCoords = gs_in[1].TexCoords;
	FragPos = gs_in[1].FragPos;
//	FragPos = vec3(explode(vec4(gs_in[1].FragPos, 1.0), normal));
	Normal = gs_in[1].Normal;
    EmitVertex();

	//顶点2
    gl_Position = explode(gl_in[2].gl_Position, normal);
    TexCoords = gs_in[2].TexCoords;
	FragPos = gs_in[2].FragPos;
//	FragPos = vec3(explode(vec4(gs_in[2].FragPos, 1.0), normal));
	Normal = gs_in[2].Normal;
    EmitVertex();

    EndPrimitive();		
	//使用endprimitive时，所有发射的顶点会合成为指定的输出渲染图元
	//重复调用EndPrimitive能够生成多个图元
}
	