#version 330 core

struct Material{
	//注意sampler2D是不透明类型，不能将其实例化（只能通过uniform来定义）
	//更新：将原来的vec3（纯色）改为采样（采样纹理上的颜色）
	sampler2D diffuse;	//删除了环境光向量（因为环境光颜色基本就等于漫反射颜色）
	sampler2D specular;	//镜面反射参数
	float shininess;	//反光度（2的幂，越高高光点越小越集中）,影响镜面高光的散射/半径
};
uniform Material material;

//定向光
struct DirLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirlight;
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

//点光源
struct PointLight{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};
#define NR_POINT_LIGHTS 4				//点光源个数
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform PointLight moveableLight;
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

//聚光
struct SpotLight{
	vec3 position;		//平行光不需要(光源的世界坐标)，点光源、聚光需要
	vec3 direction;		//平行光需要，聚光（手电筒）需要，点光源不需要
	float cutOff;		//聚光需要，点光源和平行光不需要     ——  内圆锥切光角
	float outerCutOff;	//外圆锥的切光角（模糊边缘来使用）
	vec3 ambient;		//计算强度后的不同光源的ambient分量
	vec3 diffuse;
	vec3 specular;
	//光线衰减需要的公式中的常数项、一次项和二次项
	float constant;
	float linear;
	float quadratic;
};
uniform SpotLight spotLight;
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

in vec2 TexCoords;
in vec3 FragPos;	//传入顶点的世界坐标
in vec3 Normal;
out vec4 FragColor;

uniform vec3 viewPos;		//摄像机的世界坐标（渲染循环内时刻更新）

void main() {
	vec3 norm = normalize(Normal);	//法线单位化
	vec3 viewDir = normalize(viewPos - FragPos);	//计算观察的方向
	
	//第一阶段：定向光照
	vec3 result = CalcDirLight(dirlight, norm, viewDir);

	//第二阶段：点光源
	for(int i = 0; i < NR_POINT_LIGHTS; ++i){
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	}
	result += CalcPointLight(moveableLight, norm, FragPos, viewDir);		//可移动点光源

	//第三阶段：聚光
	result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

	FragColor = vec4(result, 1.0);
}

//定向光
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(-light.direction);
	//漫反射
	float diff = max(dot(normal, lightDir), 0.0);
	//镜面反射
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir,viewDir),0.0), material.shininess);
	//合并结果
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	return ambient + diffuse + specular;
}

//点光源
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.position - fragPos);
	//漫反射
	float diff = max(dot(normal, lightDir), 0.0);
	//镜面反射
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir,viewDir),0.0), material.shininess);
	//衰减
	float distance = length(light.position - fragPos);
	float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);

	//合并结果
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return ambient + diffuse + specular;
}

//聚光
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.position - fragPos);

	//模拟手电筒：(if结构放在大的框架上，减少非必要的计算)
	float theta = dot(lightDir,normalize(-light.direction));		//得到的是余弦值
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff)/epsilon, 0.0, 1.0);	//将范围限制在0.0到1.0间
	//漫反射
	float diff = max(dot(normal, lightDir), 0.0);
	//镜面反射
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir,viewDir),0.0), material.shininess);
	//衰减
	float distance = length(light.position - fragPos);
	float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);

	//合并结果
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	//考虑手电(不考虑环境光，使其总能有一点光)
	diffuse *= intensity;
	specular *= intensity;

	return ambient + diffuse + specular;
}