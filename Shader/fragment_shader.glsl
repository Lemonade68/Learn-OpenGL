#version 330 core

struct Material{
	//注意sampler2D是不透明类型，不能将其实例化（只能通过uniform来定义）
	//更新：将原来的vec3（纯色）改为采样（采样纹理上的颜色）
	sampler2D diffuse;	//删除了环境光向量（因为环境光颜色基本就等于漫反射颜色）
	sampler2D specular;	//镜面反射参数
	float shininess;	//反光度（2的幂，越高高光点越小越集中）,影响镜面高光的散射/半径
};

struct Light{
	vec3 position;		//定向光不需要(光源的世界坐标)
//	vec3 direction;		//定向光需要

	vec3 ambient;		//计算强度后的不同光源的ambient分量
	vec3 diffuse;
	vec3 specular;

	//光线衰减需要的公式中的常数项、一次项和二次项
	float constant;
	float linear;
	float quadratic;
};

in vec2 TexCoords;
in vec3 FragPos;	//传入顶点的世界坐标
in vec3 Normal;
out vec4 FragColor;

uniform Material material;
uniform Light light;

//uniform vec3 objectColor;	//被Material替代
//uniform vec3 lightColor;	//被Light替代
//uniform vec3 lightPos;		//光源的世界坐标
uniform vec3 viewPos;		//摄像机的世界坐标（渲染循环内时刻更新）

void main() {
	//整体：都是lightColor * objectColor

	//定向光的方向
//	vec3 lightDir = normalize(-light.direction);

	//漫反射（注意标准化）
	vec3 norm = normalize(Normal);	//法线单位化
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm,lightDir), 0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));	//从纹理中采取漫反射颜色值

	//环境光(颜色与漫反射光颜色相同)
	vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));	//环境光结果(影响应该小点)，也是当前采样出的颜色

	//镜面光
	vec3 viewDir = normalize(viewPos - FragPos);	//计算观察的方向
	vec3 reflectDir = reflect(-lightDir,norm);		//计算反射的方向（得到单位向量）
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);	//计算反射光和视线是否接近（点乘）
	//镜面高光强度通过镜面光贴图的亮度表示（越白，镜面光分量越亮）—— 这边加上0.1，让木头表面也有一点点镜面反射
	vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords)+vec4(0.1,0.1,0.1,0.0));	

	//考虑衰减
	float distance = length(light.position - FragPos);
	float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 lightResult = ambient + diffuse + specular;
	FragColor = vec4(lightResult, 1.0);
}