#version 330 core

struct Material{
	//ע��sampler2D�ǲ�͸�����ͣ����ܽ���ʵ������ֻ��ͨ��uniform�����壩
	//���£���ԭ����vec3����ɫ����Ϊ���������������ϵ���ɫ��
	sampler2D diffuse;	//ɾ���˻�������������Ϊ��������ɫ�����͵�����������ɫ��
	sampler2D specular;	//���淴�����
	float shininess;	//����ȣ�2���ݣ�Խ�߸߹��ԽСԽ���У�,Ӱ�쾵��߹��ɢ��/�뾶
};

struct Light{
	vec3 position;
	vec3 ambient;		//����ǿ�Ⱥ�Ĳ�ͬ��Դ��ambient����
	vec3 diffuse;
	vec3 specular;
};

in vec2 TexCoords;
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

	//�����䣨ע���׼����
	vec3 norm = normalize(Normal);	//���ߵ�λ��
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm,lightDir), 0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));	//�������в�ȡ��������ɫֵ

	//������(��ɫ�����������ɫ��ͬ)
	vec3 abmbient = light.ambient * vec3(texture(material.diffuse,TexCoords));	//��������(Ӱ��Ӧ��С��)��Ҳ�ǵ�ǰ����������ɫ

	//�����
	vec3 viewDir = normalize(viewPos - FragPos);	//����۲�ķ���
	vec3 reflectDir = reflect(-lightDir,norm);		//���㷴��ķ��򣨵õ���λ������
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);	//���㷴���������Ƿ�ӽ�����ˣ�
	//����߹�ǿ��ͨ���������ͼ�����ȱ�ʾ��Խ�ף���������Խ�������� ��߼���0.1����ľͷ����Ҳ��һ��㾵�淴��
	vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords)+vec4(0.1,0.1,0.1,0.0));	

	vec3 lightResult = abmbient + diffuse + specular;
	FragColor = vec4(lightResult, 1.0);
}