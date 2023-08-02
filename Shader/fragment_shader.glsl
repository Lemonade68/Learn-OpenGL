#version 330 core

struct Material{
	vec3 ambient;		//不同材质的环境光参数
	vec3 diffuse;		//漫反射参数
	vec3 specular;		//镜面反射参数
	float shininess;	//反光度（2的幂，越高高光点越小越集中）,影响镜面高光的散射/半径
};

struct Light{
	vec3 position;
	vec3 ambient;		//计算强度后的不同光源的ambient分量
	vec3 diffuse;
	vec3 specular;
};

in vec3 FragPos;	//传入顶点的世界坐标
in vec3 Normal;
out vec4 FragColor;

uniform Material material;
uniform Light light;

//uniform vec3 objectColor;	//被Material替代
//uniform vec3 lightColor;	//被Light替代
uniform vec3 lightPos;		//光源的世界坐标
uniform vec3 viewPos;		//摄像机的世界坐标（渲染循环内时刻更新）

void main() {
	//整体：都是lightColor * objectColor

	//环境光
	vec3 abmbient = light.ambient * material.ambient;	//环境光结果(影响应该小点)

	//漫反射（注意标准化）
	vec3 norm = normalize(Normal);	//法线单位化
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm,lightDir), 0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);	//漫反射结果

	//镜面光
	vec3 viewDir = normalize(viewPos - FragPos);	//计算观察的方向
	vec3 reflectDir = reflect(-lightDir,norm);		//计算反射的方向（得到单位向量）
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);	//计算反射光和视线是否接近（点乘）
	vec3 specular = light.specular * (spec * material.specular);	//注意乘上光的颜色

	vec3 lightResult = abmbient + diffuse + specular;
	FragColor = vec4(lightResult, 1.0);
}