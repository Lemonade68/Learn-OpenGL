#version 330 core

in vec3 FragPos;	//传入顶点的世界坐标
in vec3 Normal;
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;		//光源的世界坐标
uniform vec3 viewPos;		//摄像机的世界坐标（渲染循环内时刻更新）

void main() {
	float ambientStrength = 0.1;	//环境光因子
	vec3 abmbient = ambientStrength * lightColor;	//环境光结果

	//注意在光照计算中，关心的都是方向，进行计算时要记得标准化
	vec3 norm = normalize(Normal);	//法线单位化
	vec3 lightDir = normalize(lightPos - FragPos);
	
	//计算光源对当前片段的漫反射影响
	float diff = max(dot(norm,lightDir), 0);
	vec3 diffuse = diff * lightColor;	//漫反射结果

	//定义摄像机的镜面强度分量
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);	//计算观察的方向
	vec3 reflectDir = reflect(-lightDir,norm);		//计算反射的方向（得到单位向量）
	//计算镜面分量
	float shininess = 32;		//反光度（2的幂，越高高光点越小越集中）
	float spec = pow(max(dot(viewDir,reflectDir),0.0),32);	//计算反射光和视线是否接近（点乘）
	vec3 spcular = specularStrength * spec * lightColor;	//注意乘上光的颜色

	vec3 lightResult = abmbient + diffuse + spcular;

	vec3 result = lightResult * objectColor;	//颜色模拟就是光的颜色*物体颜色
	FragColor = vec4(result, 1.0);
}