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
#define NR_POINT_LIGHTS 4				//点光源个数
uniform PointLight pointLights[NR_POINT_LIGHTS];
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

in vec3 FragPos;	//传入顶点的世界坐标
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;		//可移动点光源下的物体坐标

out vec4 FragColor;

bool isMovableLight = true;		//只针对可移动点光源进行阴影的模拟

uniform vec3 viewPos;
uniform sampler2D diffuseTexture;		//正常材质
uniform sampler2D shadowMap;			//阴影材质
uniform bool torchMode;

//深度测试线性变化部分
float near = 0.1;
float far  = 100.0;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}


float ShadowCalculation(PointLight light, vec3 normal, vec4 fragPosLightSpace){
	// 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // [-1,1]变换到[0,1]的范围, （光源视角下，深度z的范围应该在0-1， 纹理坐标的范围也应该在0-1）
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;

	//修正：添加bias偏移量，从而防止阴影失真
	float bias = 0.005;				//经试验，这个数值最好
//	vec3 lightDir = normalize(light.position - FragPos);
//	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // 检查当前片段是否在阴影中
//    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
//    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

	//进行PCF来将阴影柔和化，减少锯齿的操作：
	//==========================================================================
	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
	//==========================================================================

	//最终：深度贴图不渲染floor，从而不需要bias来偏移  ——  问题：立方体表面仍然会有摩尔纹
	//因此还是使用bias，但不渲染floor
	//如果超出远平面：即投影坐标z坐标>1.0时，默认没有阴影
	if(projCoords.z > 1.0)
		shadow = 0.0;

    return shadow;
}



void main() {    
	vec3 norm = normalize(Normal);	//法线单位化
	vec3 viewDir = normalize(viewPos - FragPos);	//计算观察的方向
	
	//第一阶段：定向光照
	vec3 result = CalcDirLight(dirlight, norm, viewDir);

	//第二阶段：点光源
	result += CalcPointLight(moveableLight, norm, FragPos, viewDir);		//可移动点光源
	for(int i = 0; i < NR_POINT_LIGHTS; ++i){
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	}

	//第三阶段：聚光/手电筒
	if(torchMode)
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
	vec3 halfwayDir = normalize(lightDir + viewDir);		//blinn-phong shading
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	//合并结果
	vec3 ambient = light.ambient * vec3(texture(diffuseTexture, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseTexture, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(diffuseTexture, TexCoords));

	return ambient + diffuse + specular;
}

//点光源
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.position - fragPos);
	//漫反射
	float diff = max(dot(normal, lightDir), 0.0);
	//镜面反射
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);		//blinn-phong shading
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	//衰减
	float distance = length(light.position - fragPos);
	float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);

	//合并结果
	vec3 ambient = light.ambient * vec3(texture(diffuseTexture, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseTexture, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(diffuseTexture, TexCoords));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	if(isMovableLight){
		float shadow = ShadowCalculation(light, normal, FragPosLightSpace);
		vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);		//shadow的值为0或1
		isMovableLight = false;
		return result;
	}
	else
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
	vec3 ambient = light.ambient * vec3(texture(diffuseTexture, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseTexture, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(diffuseTexture, TexCoords));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	//考虑手电(不考虑环境光，使其总能有一点光)
	diffuse *= intensity;
	specular *= intensity;

	return ambient + diffuse + specular;
}