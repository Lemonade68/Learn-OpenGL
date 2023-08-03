#version 330 core

struct Material{
	//ע��sampler2D�ǲ�͸�����ͣ����ܽ���ʵ������ֻ��ͨ��uniform�����壩
	//���£���ԭ����vec3����ɫ����Ϊ���������������ϵ���ɫ��
	sampler2D diffuse;	//ɾ���˻�������������Ϊ��������ɫ�����͵�����������ɫ��
	sampler2D specular;	//���淴�����
	float shininess;	//����ȣ�2���ݣ�Խ�߸߹��ԽСԽ���У�,Ӱ�쾵��߹��ɢ��/�뾶
};

struct Light{
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

in vec2 TexCoords;
in vec3 FragPos;	//���붥�����������
in vec3 Normal;
out vec4 FragColor;

uniform Material material;
uniform Light light;

uniform vec3 viewPos;		//��������������꣨��Ⱦѭ����ʱ�̸��£�

void main() {
	//���壺����lightColor * objectColor

	//�����ķ���
	vec3 lightDir = normalize(light.position - FragPos);
	
	//�����ķ���
//	vec3 lightDir = normalize(-light.direction);

	//ģ���ֵ�Ͳ��(if�ṹ���ڴ�Ŀ���ϣ����ٷǱ�Ҫ�ļ���)
	float theta = dot(lightDir,normalize(-light.direction));		//�õ���������ֵ
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff)/epsilon, 0.0, 1.0);	//����Χ������0.0��1.0��

	//ע��Ƚϵ���cosֵ�������>��thetaӦ�ñ�cutOffҪС����ʹ����clamp��ֱ����˼��ɣ�����Ҫif���Ƚϣ�
//	if(theta > light.cutOff) {		
	//�����䣨ע���׼����
	vec3 norm = normalize(Normal);	//���ߵ�λ��
	float diff = max(dot(norm,lightDir), 0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));	//�������в�ȡ��������ɫֵ

	//������(��ɫ�����������ɫ��ͬ)
	vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));	//��������(Ӱ��Ӧ��С��)��Ҳ�ǵ�ǰ����������ɫ

	//�����
	vec3 viewDir = normalize(viewPos - FragPos);	//����۲�ķ���
	vec3 reflectDir = reflect(-lightDir,norm);		//���㷴��ķ��򣨵õ���λ������
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);	//���㷴���������Ƿ�ӽ�����ˣ�
	//����߹�ǿ��ͨ���������ͼ�����ȱ�ʾ��Խ�ף���������Խ�������� ��߼���0.1����ľͷ����Ҳ��һ��㾵�淴��
	vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords)+vec4(0.1,0.1,0.1,0.0));	

	//����˥��
	float distance = length(light.position - FragPos);
	float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	//�����ֵ�(�����ǻ����⣬ʹ��������һ���)
	diffuse *= intensity;
	specular *= intensity;

	vec3 lightResult = ambient + diffuse + specular;
	FragColor = vec4(lightResult, 1.0);
}