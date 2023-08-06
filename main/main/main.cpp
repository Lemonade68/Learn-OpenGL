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
//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);		//��¼��Դλ�õ�λ�ƾ��󣨸�model�õģ�
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
	glEnable(GL_DEPTH_TEST);				//����������Ȳ���

	//bulid shader program
	//-----------------------------------------------------------
	//�����shader
	Shader ourShader("../../Shader/nano_vs.glsl", "../../Shader/nano_fs.glsl");

	//����ģ��
	Model ourModel("../../Models/nanosuit/nanosuit.obj");

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
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);	//״̬����
		//ÿ����Ⱦ����ǰ�����Ȼ���&��ɫ���壨����ǰһ֡�������Ϣ��Ȼ�����ڻ����У�
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();

		//projection��view����
		glm::mat4 projection(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view(1.0f);
		view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.1f));	// it's a bit too big for our scene, so scale it down
		ourShader.setMat4("model", model);
		ourModel.Draw(ourShader);

		//��鲢�����¼�����������
		//----------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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

	////�ƹ��ƶ� (��������12)
	//if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	//	lightPos.y += cameraSpeed;
	//if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	//	lightPos.y -= cameraSpeed;
	//if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	//	lightPos.x -= cameraSpeed;
	//if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	//	lightPos.x += cameraSpeed;
	//if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)	//С���̣�KP
	//	lightPos.z -= cameraSpeed;
	//if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
	//	lightPos.z += cameraSpeed;
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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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