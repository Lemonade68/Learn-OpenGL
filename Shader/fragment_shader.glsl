#version 330 core

//�����
struct DirLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirlight;
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

//���Դ
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

//�۹�
struct SpotLight{
	vec3 position;		//ƽ�йⲻ��Ҫ(��Դ����������)�����Դ���۹���Ҫ
	vec3 direction;		//ƽ�й���Ҫ���۹⣨�ֵ�Ͳ����Ҫ�����Դ����Ҫ
	float cutOff;		//�۹���Ҫ�����Դ��ƽ�йⲻ��Ҫ     ����  ��Բ׶�й��
	float outerCutOff;	//��Բ׶���й�ǣ�ģ����Ե��ʹ�ã�
	vec3 ambient;		//����ǿ�Ⱥ�Ĳ�ͬ��Դ��ambient����
	vec3 diffuse;
	vec3 specular;
	//����˥����Ҫ�Ĺ�ʽ�еĳ����һ����Ͷ�����
	float constant;
	float linear;
	float quadratic;
};
uniform SpotLight spotLight;
#define NR_POINT_LIGHTS 4				//���Դ����
uniform PointLight pointLights[NR_POINT_LIGHTS];
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

in vec3 FragPos;	//���붥�����������
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

bool isMovableLight = true;		//ֻ��Կ��ƶ����Դ������Ӱ��ģ��

uniform vec3 viewPos;
uniform bool torchMode;
uniform sampler2D diffuseTexture;		//��������
uniform samplerCube shadowMap;			//��Ӱ����
uniform float far_plane;
uniform bool shadows;


//��Ȳ������Ա仯����
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
	vec3 norm = normalize(Normal);	//���ߵ�λ��
	vec3 viewDir = normalize(viewPos - FragPos);	//����۲�ķ���
	
	//��һ�׶Σ��������
	vec3 result = CalcDirLight(dirlight, norm, viewDir);

	//�ڶ��׶Σ����Դ
	result += CalcPointLight(moveableLight, norm, FragPos, viewDir);		//���ƶ����Դ
	for(int i = 0; i < NR_POINT_LIGHTS; ++i){
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	}

	//�����׶Σ��۹�/�ֵ�Ͳ
	if(torchMode)
		result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

	FragColor = vec4(result, 1.0);
}

//�����
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(-light.direction);
	//������
	float diff = max(dot(normal, lightDir), 0.0);
	//���淴��
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);		//blinn-phong shading
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	//�ϲ����
	vec3 ambient = light.ambient * vec3(texture(diffuseTexture, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseTexture, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(diffuseTexture, TexCoords));

	return ambient + diffuse + specular;
}

//���Դ
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.position - fragPos);
	//������
	float diff = max(dot(normal, lightDir), 0.0);
	//���淴��
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);		//blinn-phong shading
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	//˥��
	float distance = length(light.position - fragPos);
	float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);

	//�ϲ����
	vec3 ambient = light.ambient * vec3(texture(diffuseTexture, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseTexture, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(diffuseTexture, TexCoords));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	if(isMovableLight){
		float shadow = shadows ? ShadowCalculation(FragPos, light.position) : 0.0;
		vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);		//shadow��ֵΪ0��1
		isMovableLight = false;
		return result;
	}
	else
		return ambient + diffuse + specular;
}

//�۹�
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.position - fragPos);

	//ģ���ֵ�Ͳ��(if�ṹ���ڴ�Ŀ���ϣ����ٷǱ�Ҫ�ļ���)
	float theta = dot(lightDir,normalize(-light.direction));		//�õ���������ֵ
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff)/epsilon, 0.0, 1.0);	//����Χ������0.0��1.0��
	//������
	float diff = max(dot(normal, lightDir), 0.0);
	//���淴��
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir,viewDir),0.0), 32.0f);
	//˥��
	float distance = length(light.position - fragPos);
	float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);

	//�ϲ����
	vec3 ambient = light.ambient * vec3(texture(diffuseTexture, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseTexture, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(diffuseTexture, TexCoords));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	//�����ֵ�(�����ǻ����⣬ʹ��������һ���)
	diffuse *= intensity;
	specular *= intensity;

	return ambient + diffuse + specular;
}