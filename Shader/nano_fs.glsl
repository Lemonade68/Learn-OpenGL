#version 330 core

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

in vec3 FragPos;	//传入顶点的世界坐标
in vec3 Normal;
in vec2 TexCoords;
out vec4 FragColor;

uniform vec3 viewPos;

//反射模型使用
uniform samplerCube skybox;

//注意使用的命名规范
uniform sampler2D texture_diffuse1;		//漫反射材质
uniform sampler2D texture_diffuse2;		//实际上是反射贴图，欺骗assimp为diffuse
uniform sampler2D texture_specular1;	//镜面反射材质

uniform bool torchMode;

void main()
{   
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
	if(torchMode)
		result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

	//添加反射
	vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, normalize(Normal));
    result += texture(texture_diffuse2, TexCoords).rgb * texture(skybox, R).rgb;		//用反射贴图乘上天空盒反射的颜色

	FragColor = vec4(result, 1.0);

//	//反射模型：
//	vec3 I = normalize(FragPos - viewPos);
//    vec3 R = reflect(I, normalize(Normal));
//    FragColor = vec4(texture(skybox, R).rgb, 1.0);

//	//折射模型
//    float ratio = 1.00 / 1.52;
//    vec3 I = normalize(FragPos - viewPos);
//    vec3 R = refract(I, normalize(Normal), ratio);
//    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}

//定向光
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(-light.direction);
	//漫反射
	float diff = max(dot(normal, lightDir), 0.0);
	//镜面反射
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir,viewDir),0.0), 32.0f);
	//合并结果
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));

	return ambient + diffuse + specular;
}

//点光源
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.position - fragPos);
	//漫反射
	float diff = max(dot(normal, lightDir), 0.0);
	//镜面反射
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir,viewDir),0.0), 32.0f);
	//衰减
	float distance = length(light.position - fragPos);
	float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);

	//合并结果
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));

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
	float spec = pow(max(dot(reflectDir,viewDir),0.0), 32.0f);
	//衰减
	float distance = length(light.position - fragPos);
	float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);

	//合并结果
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	//考虑手电(不考虑环境光，使其总能有一点光)
	diffuse *= intensity;
	specular *= intensity;

	return ambient + diffuse + specular;
}