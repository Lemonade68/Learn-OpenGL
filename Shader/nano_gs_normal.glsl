#version 330 core
layout(triangles) in;			//进来的是三角形图元（GL_TRIANGLE）
layout(line_strip, max_vertices = 6) out;	
//出去的线段，且最多顶点数为6（进来一个三角形出去3根线，每个顶点往法线方向画一根线）

in VS_OUT{
	vec3 Normal;
} gs_in[];		//使用数组：一次输入的是多个顶点

const float MAGNITUDE = 0.02;

uniform mat4 projection;	//这里使用projection

void GenerateLine(int index){
	//画出的顶点1：原顶点
	//注意：这里的gl_Position是从顶点着色器里传过来的
	gl_Position = projection * gl_in[index].gl_Position;
	EmitVertex();
	
	//画出的顶点2：法线另一端顶点
	gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].Normal, 0.0) * MAGNITUDE);
	EmitVertex();

	//将两点连起来并绘制
	EndPrimitive();		//使用片段着色器来绘制
}

void main() {
	GenerateLine(0); // 第一个顶点法线
    GenerateLine(1); // 第二个顶点法线
    GenerateLine(2); // 第三个顶点法线
}
	