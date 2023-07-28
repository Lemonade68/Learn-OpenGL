#include<glad\glad.h>		//确保放在最前面，需要在其他依赖于opengl的头文件前包含
#include<GLFW\glfw3.h>
#include<iostream>
#include"shader.h"

//窗口回调函数
void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
	// glfw: initialize and configure
	//-----------------------------------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);		//3.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		//.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//创建窗口对象
	//-----------------------------------------------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Screen", NULL, NULL);
	if (window == NULL) {
		std::cout << "failed to create GLFW window" << std::endl;	
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);	//使用设置的窗口变化函数

	//调用opengl函数前需要初始化glad(load all opengl function pointers)
	//-----------------------------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "failed to init glad" << std::endl;
		return -1;
	}

	//bulid shader program
	//-----------------------------------------------------------
	Shader ourShader("..//..//Shader//vertex_shader.glsl", "..//..//Shader//fragment_shader.glsl");

	//设置顶点数据、顶点缓冲以及确认顶点缓冲解读方式(vertex attributes)
	//-----------------------------------------------------------
	float vertices[] = {
		// 位置              // 颜色
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
	};
	
	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);		
	glGenVertexArrays(1, &VAO);
	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	//位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//是否使用线框模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//进入渲染循环	
	//-----------------------------------------------------------
	while (!glfwWindowShouldClose(window)) {
		//输入
		//----------------------------------------
		processInput(window);
		
		//渲染指令
		//----------------------------------------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	//状态设置
		glClear(GL_COLOR_BUFFER_BIT);			//状态使用
		
		//第一步永远是激活shader program
		ourShader.use();

		//使用着色器程序画三角形
		glBindVertexArray(VAO);			//绑定到VAO也会自动绑定对应的EBO
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//检查并调用事件，交换缓冲
		//----------------------------------------
		glfwSwapBuffers(window);		//等后缓冲加载完毕后呈现图像，防止图像闪烁问题（双缓冲）
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	//-----------------------------------------------------------
	glDeleteVertexArrays(1,&VAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	//-----------------------------------------------------------
	glfwTerminate();
	return 0;
}


void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);	//左下角位置，视口宽度，视口高度
}

void processInput(GLFWwindow* window) {
	//esc键关闭窗口
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)	
		glfwSetWindowShouldClose(window, true);		//终止下一次while循环的条件
}
