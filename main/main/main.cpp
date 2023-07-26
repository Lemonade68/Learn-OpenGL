#include<glad\glad.h>		//ȷ��������ǰ�棬��Ҫ������������opengl��ͷ�ļ�ǰ����
#include<GLFW\glfw3.h>
#include<iostream>

//���ڻص�����
void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	//3.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	//.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//�������ڶ���
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Screen", NULL, NULL);
	if (window == NULL) {
		std::cout << "failed to create GLFW window" << std::endl;	
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);	//ʹ�����õĴ��ڱ仯����


	//����opengl����ǰ��Ҫ��ʼ��glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "failed to init glad" << std::endl;
		return -1;
	}


	//������Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
		//����
		processInput(window);
		
		//��Ⱦָ��
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	//״̬����
		glClear(GL_COLOR_BUFFER_BIT);			//״̬ʹ��

		//��鲢�����¼�����������
		glfwSwapBuffers(window);		//�Ⱥ󻺳������Ϻ����ͼ�񣬷�ֹͼ����˸���⣨˫���壩
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}


void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);	//���½�λ�ã��ӿڿ�ȣ��ӿڸ߶�
}

void processInput(GLFWwindow* window) {
	//esc���رմ���
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)	
		glfwSetWindowShouldClose(window, true);		//��ֹ��һ��whileѭ��������
}
