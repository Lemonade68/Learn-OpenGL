#include<glad\glad.h>		//ȷ��������ǰ�棬��Ҫ������������opengl��ͷ�ļ�ǰ����
#include<GLFW\glfw3.h>
#include<iostream>

//���ڻص�����
void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//������Ǳ�׼�����꣬��˼���û��ô����ʹ����ˣ�����������w������
const char *vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\0";

int main() {
	// glfw: initialize and configure
	//-----------------------------------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);		//3.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		//.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//�������ڶ���
	//-----------------------------------------------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Screen", NULL, NULL);
	if (window == NULL) {
		std::cout << "failed to create GLFW window" << std::endl;	
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);	//ʹ�����õĴ��ڱ仯����

	//����opengl����ǰ��Ҫ��ʼ��glad(load all opengl function pointers)
	//-----------------------------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "failed to init glad" << std::endl;
		return -1;
	}

	//bulid shader program
	//-----------------------------------------------------------
	//����һ��������ɫ������
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//����ɫ��Դ�븽����ɫ�������ϲ�����
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//��鶥����ɫ���Ƿ����ɹ�
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//�༭Ƭ����ɫ��
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//������ɫ������
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	//��ɫ���������ӵ��������󣬼ǵ�ɾ����ɫ��
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//���ö������ݡ����㻺���Լ�ȷ�϶��㻺������ʽ(vertex attributes)
	//-----------------------------------------------------------
	//���ñ�׼����������
	float vertices[] = {
		0.5f, 0.5f, 0.0f,   // ���Ͻ�
		0.5f, -0.5f, 0.0f,  // ���½�
		-0.5f, -0.5f, 0.0f, // ���½�
		-0.5f, 0.5f, 0.0f   // ���Ͻ�
	};
	//���ö�������    ��EBOʹ��
	unsigned int indices[] = {
		0, 1, 3,	//��һ��������
		1, 2, 3		//�ڶ���������
	};
	
	//���嶥�㻺���������һ���Է��ʹ������ݵ��Կ���
	unsigned int VBO;			
	glGenBuffers(1, &VBO);		//��ID�͸ú�������һ��VBO
	//����һ��VAO��һ��VAO��ʾ��VBO�����ݵ�һ�ֽ����Լ�һ��EBO���൱��һ��״̬���ã�
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	//����EBO���������ƣ�  Ҳ��buffer����VBO����
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	
	//���Ȱ󶨺�VAO�����濪ʼ�������VAO
	glBindVertexArray(VAO);

	//�󶨸�VBO����ǰ�����ģ�֮��ʹ�õ��κΣ���GL_ARRAY_BUFFERĿ���ϵģ�������ö����������õ�ǰ�󶨵Ļ���(VBO)��
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//��֮ǰ����Ķ������ݸ��Ƶ�������ڴ���
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	//��EBO��ע���VAOʱ���󶨵����һ��EBO�Զ��洢Ϊ��VAO��Ԫ�ػ���������
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//����openGL��ν����������ݣ���ǰ�󶨵�VBO�е����ݣ�   ע���ǵ�ǰ
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//unbind VBO��VAO����ʵ�󲿷�����²���Ҫ��
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//ע�ⲻҪ��VAO��Чʱ���EBO
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//���VAO
	glBindVertexArray(0);

	//�Ƿ�ʹ���߿�ģʽ
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//������Ⱦѭ��	
	//-----------------------------------------------------------
	while (!glfwWindowShouldClose(window)) {
		//����
		//----------------------------------------
		processInput(window);
		
		//��Ⱦָ��
		//----------------------------------------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	//״̬����
		glClear(GL_COLOR_BUFFER_BIT);			//״̬ʹ��
		
		//ʹ����ɫ������������
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);			//�󶨵�VAOҲ���Զ��󶨶�Ӧ��EBO
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//����ģʽʹ��glDrawElements������glDrawArrays
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);		//����Ҫÿ�ζ�unbind��

		//��鲢�����¼�����������
		//----------------------------------------
		glfwSwapBuffers(window);		//�Ⱥ󻺳������Ϻ����ͼ�񣬷�ֹͼ����˸���⣨˫���壩
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	//-----------------------------------------------------------
	glDeleteVertexArrays(1,&VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	//-----------------------------------------------------------
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
