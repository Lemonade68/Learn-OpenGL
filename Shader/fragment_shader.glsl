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
in vec4 FragPosLightSpace;		//���ƶ����Դ�µ���������

out vec4 FragColor;

uniform vec3 viewPos;

//��Ȳ������Ա仯����
float near = 0.1;
float far  = 100.0;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

uniform sampler2D diffuseTexture;		//��������
uniform sampler2D shadowMap;			//��Ӱ����

float ShadowCalculation(vec4 fragPosLightSpace){
	// ִ��͸�ӳ���
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // [-1,1]�任��[0,1]�ķ�Χ, ����Դ�ӽ��£����z�ķ�ΧӦ����0-1�� ��������ķ�ΧҲӦ����0-1��
    projCoords = projCoords * 0.5 + 0.5;
    // ȡ�����������(ʹ��[0,1]��Χ�µ�fragPosLight������)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // ȡ�õ�ǰƬ���ڹ�Դ�ӽ��µ����
    float currentDepth = projCoords.z;
    // ��鵱ǰƬ���Ƿ�����Ӱ��
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}


bool isMovableLight = true;		//ֻ��Կ��ƶ����Դ������Ӱ��ģ��

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
		float shadow = ShadowCalculation(FragPosLightSpace);
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