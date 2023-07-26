#include<glad\glad.h>		//确保放在最前面，需要在其他依赖于opengl的头文件前包含
#include<GLFW\glfw3.h>
#include<iostream>

//窗口回调函数
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

	//创建窗口对象
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Screen", NULL, NULL);
	if (window == NULL) {
		std::cout << "failed to create GLFW window" << std::endl;	
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);	//使用设置的窗口变化函数


	//调用opengl函数前需要初始化glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "failed to init glad" << std::endl;
		return -1;
	}


	//进入渲染循环
	while (!glfwWindowShouldClose(window)) {
		//输入
		processInput(window);
		
		//渲染指令
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	//状态设置
		glClear(GL_COLOR_BUFFER_BIT);			//状态使用

		//检查并调用事件，交换缓冲
		glfwSwapBuffers(window);		//等后缓冲加载完毕后呈现图像，防止图像闪烁问题（双缓冲）
		glfwPollEvents();
	}

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
