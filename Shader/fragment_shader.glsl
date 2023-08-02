#version 330 core

struct Material{
	vec3 ambient;		//��ͬ���ʵĻ��������
	vec3 diffuse;		//���������
	vec3 specular;		//���淴�����
	float shininess;	//����ȣ�2���ݣ�Խ�߸߹��ԽСԽ���У�,Ӱ�쾵��߹��ɢ��/�뾶
};

struct Light{
	vec3 position;
	vec3 ambient;		//����ǿ�Ⱥ�Ĳ�ͬ��Դ��ambient����
	vec3 diffuse;
	vec3 specular;
};

in vec3 FragPos;	//���붥�����������
in vec3 Normal;
out vec4 FragColor;

uniform Material material;
uniform Light light;

//uniform vec3 objectColor;	//��Material���
//uniform vec3 lightColor;	//��Light���
uniform vec3 lightPos;		//��Դ����������
uniform vec3 viewPos;		//��������������꣨��Ⱦѭ����ʱ�̸��£�

void main() {
	//���壺����lightColor * objectColor

	//������
	vec3 abmbient = light.ambient * material.ambient;	//��������(Ӱ��Ӧ��С��)

	//�����䣨ע���׼����
	vec3 norm = normalize(Normal);	//���ߵ�λ��
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm,lightDir), 0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);	//��������

	//�����
	vec3 viewDir = normalize(viewPos - FragPos);	//����۲�ķ���
	vec3 reflectDir = reflect(-lightDir,norm);		//���㷴��ķ��򣨵õ���λ������
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);	//���㷴���������Ƿ�ӽ�����ˣ�
	vec3 specular = light.specular * (spec * material.specular);	//ע����Ϲ����ɫ

	vec3 lightResult = abmbient + diffuse + specular;
	FragColor = vec4(lightResult, 1.0);
}