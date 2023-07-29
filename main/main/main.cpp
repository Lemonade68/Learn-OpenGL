#include<glad/glad.h>		//ȷ��������ǰ�棬��Ҫ������������opengl��ͷ�ļ�ǰ����
#include<GLFW/glfw3.h>
#include<iostream>
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>
#include"Shader.h"


//ͼ����ؿ�
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

//���ڻص�����
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mixValue = 0.2f;

//���������
//-----------------------------------------------------------
//view matrix	����������ƶ�		���ȫ�ֱ����Ӷ������뺯�����Է���
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


//������Ⱦ��ʱ���Ӷ���֤��ͬӲ��������ƶ��ٶ���Ӧƽ��
float deltaTime = 0.0f;		//��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f;		//��һ֡��ʱ��

int main() {
	// glfw: initialize and configure
	//-----------------------------------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//�������ڶ���
	//-----------------------------------------------------------
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Triangle", NULL, NULL);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//����opengl����ǰ��Ҫ��ʼ��glad(load all opengl function pointers)
	//-----------------------------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//bulid shader program
	//-----------------------------------------------------------
	Shader ourShader("../../Shader/vertex_shader.glsl", "../../Shader/fragment_shader.glsl");

	//���ö������ݡ����㻺���Լ�ȷ�϶��㻺������ʽ(vertex attributes)
	//-----------------------------------------------------------
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	//unsigned int indexes[] = {
	//	0,1,3,          //��һ��������
	//	1,2,3			//�ڶ���������
	//};

	//�κ�һ�����󣺶��壬���ɣ���    ***********************

	unsigned int VBO, VAO;
	//unsigned int EBO;
	glGenVertexArrays(1, &VAO);       //gen:generate
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

	//����λ������ָ��
	//��������Ϊ����������ֵ����ɫ���У������ݸ��������ͣ��Ƿ��׼����stride������һ����������ݸ�������offset��ƫ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//������ɫ����ָ�� (��ʼƫ��λ��Ϊ��һ�������3��λ������)
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	//������������ָ��
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//�Ƿ�ʹ���߿�ģʽ
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	//��������
	//-----------------------------------------------------------
	unsigned int texture1, texture2;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	//���û��ƣ�2���͹��ˣ�2����ʽ     ľ������Ϊ��Ե����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);   //ע����С�ͷŴ�Ĳ�ͬ���˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//����ͼƬǰ������ʹ����������ͼ�����ʱ��תy�ᣬ�����������µߵ�
	stbi_set_flip_vertically_on_load(true);

	//����ͼƬ
	int width, height, nrChannels; //ͨ��������RGB
	//�ü��ص�ͼ������䶨��Ŀ�ȡ��߶ȡ���ɫͨ������
	unsigned char *data = stbi_load("../../Textures/container.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	//��������Ͷ�Ӧ�༶��Զ������ͷ�ͼ���ڴ�
	stbi_image_free(data);

	//�ڶ�������				Ц������Ϊ�ظ�����
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);   //ע����С�ͷŴ�Ĳ�ͬ���˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load("../../Textures/awesomeface.png", &width, &height, &nrChannels, 0);
	if (data) {
		//pngע�⣺��ͨ�� ���� RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	//����ÿ���������ķ�ʽ����OpenGLÿ����ɫ�������������ĸ�����Ԫ��1�μ��ɣ�
	ourShader.use();   //����uniform����ǰ��Ҫ���Ǽ����ɫ������!
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);		//�ֶ�����
	ourShader.setInt("texture2", 1);     //�������÷�ʽ����

	//�����任����
	//-----------------------------------------------------------
	//glm::mat4 trans = glm::mat4(1.0f);		//��ʼ����λ����
	//��z����ת������radiansת��Ϊ������			//ע���Ķ�˳�򣺴������ϣ��������������
	//trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));	
	//trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
	
	//���ò�ͬ�������ڿռ��е�λ��
	glm::vec3 cubePositions[] = {
	  glm::vec3(0.0f,  0.0f,  0.0f),
	  glm::vec3(2.0f,  5.0f, -15.0f),
	  glm::vec3(-1.5f, -2.2f, -2.5f),
	  glm::vec3(-3.8f, -2.0f, -12.3f),
	  glm::vec3(2.4f, -0.4f, -3.5f),
	  glm::vec3(-1.7f,  3.0f, -7.5f),
	  glm::vec3(1.3f, -2.0f, -2.5f),
	  glm::vec3(1.5f,  2.0f, -2.5f),
	  glm::vec3(1.5f,  0.2f, -1.5f),
	  glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	////���������
	////-----------------------------------------------------------
	////�����λ��
	//glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
	////��������� ���� ʵ���������ָ���෴		�����+z��
	//glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
	//glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraDirection);	
	////���ᣨʹ���������ͷ����ˣ�	�����+x��
	//glm::vec3 up(0.0f, 1.0f, 0.0f);
	//glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	////���ᣨ����ͷ����ˣ�	�����+y��
	//glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));


	//lookat���󣺶���һ�������λ�ã�һ��Ŀ��λ�ú�һ����ʾ����ռ��е������������������Ǽ���������ʹ�õ��Ǹ���������
	//glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0),
	//	glm::vec3(0.0, 0.0, 0.0),
	//	glm::vec3(0.0, 1.0, 0.0));
		
	//������Ⱦѭ��	
	//-----------------------------------------------------------
	while (!glfwWindowShouldClose(window)) {
		//����
		//----------------------------------------
		processInput(window);

		//��Ⱦָ��
		//----------------------------------------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	//״̬����
		glEnable(GL_DEPTH_TEST);				//����������Ȳ���
		//ÿ����Ⱦ����ǰ�����Ȼ���&��ɫ���壨����ǰһ֡�������Ϣ��Ȼ�����ڻ����У�
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			

		//TEXTURE0Ĭ���Ǳ�����ģ����ֻ��һ������ʱ����Ҫ���Ǽ�������Ԫ������
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		//��һ����Զ�Ǽ���shader program
		ourShader.use();

		//����mixValueֵ
		ourShader.setFloat("transparancy", mixValue);

		glBindVertexArray(VAO);		//�󶨵�VAOҲ���Զ��󶨶�Ӧ��EBO

		//����ϵͳ
		//-----------------------------------------------------------

		////view matrix  �������y����ת
		/*float radius = 10.0f;
		float camX = sin(glfwGetTime())*radius;
		float camZ = cos(glfwGetTime())*radius;
		glm::mat4 view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));*/

		//view	Ҫ������Ⱦѭ�����������ʱ�̸���
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//����ڶ��������ĺ��壺���ܾ�ͷ��ô�����۲췽����ǰ��������۽���һ�����ϣ�  ����������ǵ㣡

		//projection matrix (perspective projection)
		glm::mat4 projection(1.0f);
		float FoV = 45.0f;
		projection = glm::perspective(glm::radians(FoV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		//������fov����߱ȣ���ƽ��zֵ��Զƽ��zֵ��ע����ߵ�floatҪ�ӣ���Ȼ�����Զ�ת���񣨻ᱨ����û�����ص����⣩

		//glm::mat4 trans = projection * view * model;

		for (unsigned int i = 0; i < 10; ++i) {
			//�µ�model
			glm::mat4 model(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * glfwGetTime() * (i+1);
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

			glm::mat4 trans = projection * view * model;
			ourShader.setMat4("transform", trans);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		//����ʱ���
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//��鲢�����¼�����������
		//----------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	//-----------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	//-----------------------------------------------------------
	glfwTerminate();
	return 0;
}


void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);	//���½�λ�ã��ӿڿ�ȣ��ӿڸ߶�
}

void processInput(GLFWwindow *window) {
	//esc���رմ���
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);		//��ֹ��һ��whileѭ��������
	//����͸����
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		mixValue += 0.001f;
		if (mixValue >= 1.0f)
			mixValue = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		mixValue -= 0.001f;
		if (mixValue <= 0.0f)
			mixValue = 0.0f;
	}
	//�ƶ�����
	float cameraSpeed = 3.0f * deltaTime;		//��ͷλ���ƶ��ٶ�
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	//ע����Ҫ��׼������Ȼ����ͬ�ƶ����ٶȾͲ�ͬ�ˣ����صĲ�˽����ͬ��
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}