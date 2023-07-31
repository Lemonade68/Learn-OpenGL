#version 330 core

in vec3 FragPos;	//���붥�����������
in vec3 Normal;
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;		//��Դ����������
uniform vec3 viewPos;		//��������������꣨��Ⱦѭ����ʱ�̸��£�

void main() {
	float ambientStrength = 0.1;	//����������
	vec3 abmbient = ambientStrength * lightColor;	//��������

	//ע���ڹ��ռ����У����ĵĶ��Ƿ��򣬽��м���ʱҪ�ǵñ�׼��
	vec3 norm = normalize(Normal);	//���ߵ�λ��
	vec3 lightDir = normalize(lightPos - FragPos);
	
	//�����Դ�Ե�ǰƬ�ε�������Ӱ��
	float diff = max(dot(norm,lightDir), 0);
	vec3 diffuse = diff * lightColor;	//��������

	//����������ľ���ǿ�ȷ���
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);	//����۲�ķ���
	vec3 reflectDir = reflect(-lightDir,norm);		//���㷴��ķ��򣨵õ���λ������
	//���㾵�����
	float shininess = 32;		//����ȣ�2���ݣ�Խ�߸߹��ԽСԽ���У�
	float spec = pow(max(dot(viewDir,reflectDir),0.0),32);	//���㷴���������Ƿ�ӽ�����ˣ�
	vec3 spcular = specularStrength * spec * lightColor;	//ע����Ϲ����ɫ

	vec3 lightResult = abmbient + diffuse + spcular;

	vec3 result = lightResult * objectColor;	//��ɫģ����ǹ����ɫ*������ɫ
	FragColor = vec4(result, 1.0);
}