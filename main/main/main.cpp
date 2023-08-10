#include<glad/glad.h>		//ȷ��������ǰ�棬��Ҫ������������opengl��ͷ�ļ�ǰ����
#include<GLFW/glfw3.h>

#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>

#include"shader.h"
#include"camera.h"
#include"model.h"

#include<iostream>

//ͼ����ؿ�
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);		//���ڻص�����
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);			//���ص�����
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);		//���ֻص�����
void processInput(GLFWwindow *window);											//���̼�������

unsigned int loadTexture(const char *path);										//��Ӳ��ʵĺ���
unsigned int loadCubemap(std::vector<std::string> faces);						//������պеĺ���

//settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//���������(ʵ�������������˶���ͨ��loolAt�����������巴���˶����������ƶ��ļ���)
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2;	//�����һ֡��λ��
float lastY = SCR_HEIGHT / 2;	//�����һ֡��λ��
bool firstMouse = true;

//������Ⱦ��ʱ���Ӷ���֤��ͬӲ��������ƶ��ٶ���Ӧƽ��  timing
float deltaTime = 0.0f;		//��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f;		//��һ֡��ʱ��

//��Դ����
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);		//��¼��Դλ�õ�λ�ƾ��󣨸�model�õģ�
//glm::vec3 lightColor;

int main() {
	// glfw: initialize and configure
	//-----------------------------------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//�������ڶ���
	//-----------------------------------------------------------
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Screen", NULL, NULL);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//ע��ص�����
	//-----------------------------------------------------------
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);	//ע�ᴰ�ڻص�����
	glfwSetCursorPosCallback(window, mouse_callback);					//ע�����ص�����
	glfwSetScrollCallback(window, scroll_callback);						//ע������ص�����


	//����GLFW��׽���
	//-----------------------------------------------------------
	//���ù��������ͣ��(���봰�ں�)
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//����opengl����ǰ��Ҫ��ʼ��glad(load all opengl function pointers)
	//-----------------------------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//configure global opengl state
	//-----------------------------------------------------------
	//1.ģ����ԣ��Ƚ��У�
	glEnable(GL_STENCIL_TEST);
	//��������
	//glStencilMask(0xFF);		// ����д�룬ÿһλд��ģ�建��ʱ������ԭ����ԭ����1λ�룬Ĭ��Ҳ��1)
	//glStencilMask(0x00);		// ÿһλ��д��ģ�建��ʱ������0������д�룩
	//Ӧ�öԻ���������ʲô��glStencilFunc
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);		//ֻ��Ƭ�ε�ģ��ֵΪ0ʱ����ͨ�����Բ�������
	
	//=============================================
	//��������ֵĲ��
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);		//���ذ���סʱ�����ȫ��
	glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);		//���ذ���סʱֻ���Ʊ߿�

	//ԭ�����ذ���ס������ͨ������Ȳ��ԣ���˻�ִ�еڶ���������Ҫ�󣬽��žͺ�������
	//=============================================


	//2.��Ȳ��ԣ�����У�
	glEnable(GL_DEPTH_TEST);				//����������Ȳ���
	//glDepthMask(GL_FALSE);				//�Ƿ������Ȼ����д�룬������ʹ�ò���Ч
	//glDepthFunc(GL_ALWAYS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))
	glDepthFunc(GL_LESS); // ��������ͬ����С�Ż�ͨ�����Զ������ƣ���Զ��Ƭ�λᱻ������

	//  **********  ��Ȼ����е�ֵ����Ļ�ռ��в������Ե�!!!!!!!!!!!!!!!!!
	// ��ʵ�ϣ�������������Ⱦ��ȴ�Զ����������Ⱦ��Ⱥ�С
	// ʵ���Ͼ���projection model����games101���������⣬Զ���Ŀռ�ᱻ��ѹ�ĸ�С


	//3.��ϣ������У�
	//Ч����ʹ��ÿ��OpenGL��Ⱦ��һ��Ƭ��ʱ�������Ὣ��ǰƬ�ε���ɫ�͵�ǰ��ɫ�����е�Ƭ����ɫ����alphaֵ�����л��
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		//��������
	//���⣺��Ȳ���ʱ���ῼ��͸�������⣬�����Ҫ����ǰ�����������������͸�����岻��ʾ�����������


	//���޳� ���� ����������ʱȫ��������ʱ��ķ��������壬�������Ժܺõ�������޳�����
	glEnable(GL_CULL_FACE);			//�����潫�ᱻ��������Է����״��
	//���Ҫ�������ֵĻ����ر����޳�����Ϊ�����涼Ӧ�ÿɼ�
	glCullFace(GL_BACK);	//ֻ�޳������棨Ĭ����BACK��˳���棺FRNOT��
	glFrontFace(GL_CCW);	//������ʱ�����Ϊ�����棬CW��˳ʱ��
	
	glDisable(GL_CULL_FACE);


	//bulid shader program
	//-----------------------------------------------------------
	//�����shader
	Shader shader("../../Shader/vertex_shader.glsl", "../../Shader/fragment_shader.glsl");
	Shader shaderSingleColor("../../Shader/stencil_vs.glsl", "../../Shader/stencil_fs.glsl");
	
	//��Դshader
	Shader lightCubeShader("../../Shader/light_vs.glsl", "../../Shader/light_fs.glsl");
	
	//͸��/��͸������shader
	Shader transparentShader("../../Shader/basic_vs.glsl", "../../Shader/basic_fs.glsl");
	
	//��򵥵�shader����Ϊֻ��Ҫ����һ���ı���֮�󣬰�֡�������ɫ���帽��(texture)����ȥ����
	Shader screenShader("../../Shader/framebuffer_screen_vs.glsl", "../../Shader/framebuffer_screen_fs.glsl");

	//��պ�shader
	Shader skyboxShader("../../Shader/skybox_vs.glsl", "../../Shader/skybox_fs.glsl");

	//�����shader
	Shader reflectShader("../../Shader/reflectbox_vs.glsl", "../../Shader/reflectbox_fs.glsl");

	//�����shader
	Shader refractShader("../../Shader/refractbox_vs.glsl", "../../Shader/refractbox_fs.glsl");

	//����ģ��
	Model ourModel("../../Models/nanosuit/nanosuit.obj");
	Shader modelShader("../../Shader/nano_vs.glsl", "../../Shader/nano_fs.glsl");

	float cubeVertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	float planeVertices[] = {
		// positions		  //Normals			// texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f, 0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f, 2.0f, 2.0f
	};

	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};
	//�����Ⱦͼ��ʱ���ı��Σ�������Ļ��������
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	   // positions   // texCoords
	   -1.0f,  1.0f,  0.0f, 1.0f,
	   -1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	//��պж���λ��
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	//����еĶ�������  ����+����
	float reflectVertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	//͸�������λ��
	vector<glm::vec3> windows;
	windows.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
	windows.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	windows.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
	windows.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	windows.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(-2.3f, 0.1f, -1.0f),
		glm::vec3(0.0f,  2.0f, -2.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	//��Դ
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	
	// plane VAO
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	// transparent VAO
	unsigned int transparentVAO, transparentVBO;
	glGenVertexArrays(1, &transparentVAO);
	glGenBuffers(1, &transparentVBO);
	glBindVertexArray(transparentVAO);
	glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	// screen quad VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	//skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//reflectVAO
	unsigned int reflectVAO, reflectVBO;
	glGenVertexArrays(1, &reflectVAO);
	glGenBuffers(1, &reflectVBO);
	glBindVertexArray(reflectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, reflectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(reflectVertices), &reflectVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int cubeTexture = loadTexture("../../Textures/marble.jpg");
	unsigned int floorTexture = loadTexture("../../Textures/metal.png");
	//unsigned int transparentTexture = loadTexture("../../Textures/grass.png");
	unsigned int transparentTexture = loadTexture("../../Textures/window.png");

	//������պУ�
	vector<std::string> faces{
		"../../Textures/skybox/right.jpg",
		"../../Textures/skybox/left.jpg",
		"../../Textures/skybox/top.jpg",
		"../../Textures/skybox/bottom.jpg",
		"../../Textures/skybox/front.jpg",
		"../../Textures/skybox/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	// shader configuration
	// --------------------
	shader.use();
	shader.setInt("texture1", 0);

	transparentShader.use();
	transparentShader.setInt("texture1", 0);

	screenShader.use();
	screenShader.setInt("screenTexture", 0);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	reflectShader.use();
	reflectShader.setInt("skybox", 0);

	refractShader.use();
	refractShader.setInt("skybox", 0);

	modelShader.use();
	modelShader.setInt("skybox", 0);

	//framebuffer configuration
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	//����color attachment texture����ɫ����������������Ⱥ�ģ�壩,���²���������һ��
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	//** ��ߣ�ѡ����Ļ�ĳ��ȺͿ�ȣ�Ȼ��������NULL��ֻ�Ƿ���ռ䣬��ʱû�д����ɫ���ݣ�֮����Ⱦʱ�������ȥ
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	//ʹ�ú˽��в���ʱʹ�� ==================================================
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//====================================================================
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//����ɫ��������framebuffer��
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	//create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	// ���������в�����������д��� ���� ʹ����Ⱦ������� Ҫ���� ���� ʹ��������
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);		//����Ⱦ�������֮������������
	//˵��rboһ����Ⱥ�ģ����Ⱦ�������
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	//�������rbo��framebuffer��
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//�Ƿ�ʹ���߿�ģʽ
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//������Ⱦѭ��
	//-----------------------------------------------------------
	while (!glfwWindowShouldClose(window)) {
		//����ÿ֡��ʱ���  per-frame time logic
		//----------------------------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//����
		//----------------------------------------
		processInput(window);

		//��Ⱦָ��
		//----------------------------------------

		// bind to framebuffer and draw scene as we normally would to color texture
		// ���ử�����ڣ����ử����������ȥ
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		//*****************  �������ã�����ûд ���� д�ϵĺô���ÿ��ѭ������Ϊ��ʼ״̬����ֹ�ϴ�ѭ����Ӱ��  ********************
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);			

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);	//״̬����
		//ÿ����Ⱦ����ǰ�����Ȼ���&��ɫ����&ģ�建�壨����ǰһ֡�������Ϣ��Ȼ�����ڻ����У�
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// ********************* ע�⿴�����glStencilMask(0xFF)������

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		////��һ�����Ȼ���պУ����Ҳ��������д�룬�Ӷ���Զ���������������
		//glDepthMask(GL_FALSE);		//�������д�루�Ȼ�����������������Ļ��ͱ����ǣ�
		//skyboxShader.use();
		////���ù۲��ͶӰ����
		//skyboxShader.setMat4("projection", projection);
		////skyboxShader.setMat4("view", view);		//�����Ļ���պоͺ�һ��ĺ���û���𣬻ᶯ
		////�������£��൱��ȥ����view�����е�λ�Ʋ��֣�����������ת�ͱ任
		//skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));	
		//glBindVertexArray(skyboxVAO);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDepthMask(GL_TRUE);		//�������д��
		
		// set uniforms
		shaderSingleColor.use();
		shaderSingleColor.setMat4("view", view);
		shaderSingleColor.setMat4("projection", projection);

		transparentShader.use();
		transparentShader.setMat4("view", view);
		transparentShader.setMat4("projection", projection);

		reflectShader.use();
		reflectShader.setMat4("view", view);
		reflectShader.setMat4("projection", projection);

		refractShader.use();
		refractShader.setMat4("view", view);
		refractShader.setMat4("projection", projection);

		modelShader.use();
		modelShader.setMat4("view", view);
		modelShader.setMat4("projection", projection);


		shader.use();
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		//һ���ǵ�Ҫ�� ������
		shader.setVec3("viewPos", camera.Position);

		// directional light
		shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		
		// movable light
		shader.setVec3("moveableLight.position", lightPos);
		shader.setVec3("moveableLight.ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("moveableLight.diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("moveableLight.specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("moveableLight.constant", 1.0f);
		shader.setFloat("moveableLight.linear", 0.09f);
		shader.setFloat("moveableLight.quadratic", 0.032f);

		// point light 1
		shader.setVec3("pointLights[0].position", pointLightPositions[0]);
		shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[0].constant", 1.0f);
		shader.setFloat("pointLights[0].linear", 0.09f);
		shader.setFloat("pointLights[0].quadratic", 0.032f);
		// point light 2
		shader.setVec3("pointLights[1].position", pointLightPositions[1]);
		shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[1].constant", 1.0f);
		shader.setFloat("pointLights[1].linear", 0.09f);
		shader.setFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		shader.setVec3("pointLights[2].position", pointLightPositions[2]);
		shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[2].constant", 1.0f);
		shader.setFloat("pointLights[2].linear", 0.09f);
		shader.setFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		shader.setVec3("pointLights[3].position", pointLightPositions[3]);
		shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[3].constant", 1.0f);
		shader.setFloat("pointLights[3].linear", 0.09f);
		shader.setFloat("pointLights[3].quadratic", 0.032f);

		//�Ȼ����в�͸��������
		//1.��ʼʱ���Ƶذ� ���� ����Ҫ�߿�������ò�����ģ�建��
		glStencilMask(0x00);		//����д��

		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);		//�������û�У�Ĭ�ϼ���0��
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		shader.setMat4("model", glm::mat4(1.0f));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		//2.�������壬����Ҫ����ģ����ԣ��޸�stencil buffer
		glStencilMask(0xFF);			//����д��
		glStencilFunc(GL_ALWAYS, 1, 0xFF);		//ֻҪ���л��ƣ�����Զͨ�����Ҹ���ģ��ֵΪ1��op�涨�ģ�
		//glEnable(GL_CULL_FACE);		//�������޳�

		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, -0.5f, -1.0f));
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, -0.5f, 0.0f));
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		//glDisable(GL_CULL_FACE);

		//������Ŵ�һ������ƣ���ֹд��ģ��ֵ����ֻ�з�1��λ�ûᱻ���ƣ��ﵽ��ԵЧ��
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);	//��Ϊ1�Ĳ��ܻ��ƣ�������Ļ���� ���� ���Բ�������������������Ǳ�Ե��
		glStencilMask(0x00);					//��ֹд��ģ��ֵ����ֹ���Ƴɹ�ʱģ��ֵ��op�ĳ�1��
		glDisable(GL_DEPTH_TEST);				//������Ȳ��ԣ��Ӷ��߿����͸��
		shaderSingleColor.use();
		float scale = 1.05f;

		// cubes
		glBindVertexArray(cubeVAO);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, -0.5f, -1.0f));
		model = glm::scale(model, glm::vec3(scale));		//������scale
		shaderSingleColor.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, -0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(scale));		//������scale
		shaderSingleColor.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		
		//�ָ���Ĭ��״̬
		glStencilMask(0xFF);		//�ָ��ɿ�д�������Ļ���Ⱦѭ���е�glClear�������ģ�建��
		glStencilFunc(GL_ALWAYS, 0, 0xFF);		//Ĭ��д0��ֻ������Ҫģ��д��ʱ��д1
		glEnable(GL_DEPTH_TEST);

		//�����Դ����
		// also draw the lamp object(s)
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);

		// we now draw as many light bulbs as we have point lights.
		glBindVertexArray(lightCubeVAO);
		for (unsigned int i = 0; i < 4; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lightCubeShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		lightCubeShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		//��������壺
		reflectShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0, 0.0, 2.0));
		reflectShader.setMat4("model", model);
		reflectShader.setVec3("cameraPos", camera.Position);
		glBindVertexArray(reflectVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//��������壺
		refractShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-4.0, 0.0, 2.0));
		refractShader.setMat4("model", model);
		refractShader.setVec3("cameraPos", camera.Position);
		glBindVertexArray(reflectVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//����/����ģ�ͣ�������nano_fs����ģ�
		modelShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0, -0.5, 2.0));
		model = glm::scale(model, glm::vec3(0.15f));
		modelShader.setMat4("model", model);
		modelShader.setVec3("viewPos", camera.Position);
		ourModel.Draw(modelShader);

		
		//�Ż��������պ�
		//��ע��Ҫ�ڰ�͸������ǰ������Ȼ͸�����������û��������ɫ�Ļ����ɱ���ɫ(д���������)��
		//��ʹ��պ�ͨ������Ȳ��ԣ����´�����������պУ�

		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		//ʹ��LEQUAL��ԭ����Ȳ��Ե�Ĭ�ϳ�ʼֵΪ1���ھ����Ż�����պе����һֱΪ1�������LESS������ͨ�������ԣ��޷�������
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		//������
		//��¼����λ��
		std::map<float, glm::vec3> sorted;
		for (unsigned int i = 0; i < windows.size(); i++) {
			float distance = glm::length(camera.Position - windows[i]);
			sorted[distance] = windows[i];
		}

		transparentShader.use();
		glBindVertexArray(transparentVAO);
		glBindTexture(GL_TEXTURE_2D, transparentTexture);
		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			transparentShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		//���ڻص�default framebuffer�У���һ���ı��Σ�������������ȥ
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		//=======================  ���ɣ�Ϊʲô���ע�͵���������⣿ =========================
		glDisable(GL_DEPTH_TEST);	// disable depth test so screen-space quad isn't discarded due to depth test.
		
		//���disable stencil_test�������ӱ��߿򸲸ǵ�ԭ��
		//����ѭ����ʼû��enable stencil test��������һ��ѭ����Ⱦʱģ�����δ�����Ӷ���������Ⱦ����
		//���Ѿ��ڿ�ͷ���ϣ������ﱾ��Ҳ����Ҫdisableģ�����
		//glDisable(GL_STENCIL_TEST);

		// clear all relevant buffers
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);

		screenShader.use();
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//glEnable(GL_STENCIL_TEST);

		//��鲢�����¼�����������
		//----------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteVertexArrays(1, &transparentVAO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteBuffers(1, &transparentVBO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteRenderbuffers(1, &rbo);
	glDeleteFramebuffers(1, &framebuffer);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	//-----------------------------------------------------------
	glfwTerminate();
	return 0;
}


void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);	//���½�λ�ã��ӿڿ�ȣ��ӿڸ߶�
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	//��ֹ��ʼ��һ��
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	//��¼��ǰ֡����һ֡�����ƫ����
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;	//�෴����Ϊy�����Ǵ����������������

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow *window) {
	//esc���رմ���
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);		//��ֹ��һ��whileѭ��������

	//�ƶ�����(��camera�ཻ��)
	float cameraSpeed = 3.0f * deltaTime;		//��ͷλ���ƶ��ٶ�
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	//ע����Ҫ��׼������Ȼ����ͬ�ƶ����ٶȾͲ�ͬ�ˣ����صĲ�˽����ͬ��
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);

	//�ƹ��ƶ� (��������12)
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		lightPos.y += cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		lightPos.y -= cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		lightPos.x -= cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		lightPos.x += cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)	//С���̣�KP
		lightPos.z -= cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
		lightPos.z += cameraSpeed;
}


unsigned int loadTexture(char const * path)
{
	//��������
	unsigned int textureID;
	glGenTextures(1, &textureID);

	//������������
	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		//���ø��������֮��󶨸ö���Ϊ�󶨴˴����ã�
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//ע���µı仯 ���� �����repeat���ᵼ�������͸�����ֺ͵��µĴ�ɫ���ֽ��л�ɫ���Ӷ����ֺ�խ�ı߿�
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}


unsigned int loadCubemap(std::vector<std::string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); ++i) {
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			//����openGLĬ�ϵ�˳����������������ͼ������
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	//���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	return textureID;
}