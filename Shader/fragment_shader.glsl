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

out vec4 FragColor;

bool isMovableLight = true;		//只针对可移动点光源进行阴影的模拟

uniform vec3 viewPos;
uniform bool torchMode;
uniform sampler2D diffuseTexture;		//正常材质
uniform samplerCube shadowMap;			//阴影材质
uniform float far_plane;
uniform bool shadows;


//深度测试线性变化部分
float near = 0.1;
float far  = 100.0;


// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos, vec3 lightPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // use the fragment to light vector to sample from the depth map    
    // float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    // closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    // float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    // float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    // PCF
    // float shadow = 0.0;
    // float bias = 0.05; 
    // float samples = 4.0;
    // float offset = 0.1;
    // for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    // {
        // for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        // {
            // for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            // {
                // float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
                // closestDepth *= far_plane;   // Undo mapping [0;1]
                // if(currentDepth - bias > closestDepth)
                    // shadow += 1.0;
            // }
        // }
    // }
    // shadow /= (samples * samples * samples);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
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
		float shadow = shadows ? ShadowCalculation(FragPos, light.position) : 0.0;
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