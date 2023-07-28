#include<glad\glad.h>		//确保放在最前面，需要在其他依赖于opengl的头文件前包含
#include<GLFW\glfw3.h>
#include<iostream>
#include"shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

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
	Shader ourShader("../../Shader/vertex_shader.glsl", "../../Shader/fragment_shader.glsl");

	//设置顶点数据、顶点缓冲以及确认顶点缓冲解读方式(vertex attributes)
	//-----------------------------------------------------------
	float vertices[] = {
		//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
	};

	unsigned int indices[] = {
		   0, 1, 3,		// first triangle
		   1, 2, 3		// second triangle
	};
	
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);		
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//位置属性
	//参数依次为：顶点属性值（着色器中），数据个数，类型，是否标准化，stride步长（一个顶点的数据个数），offset（偏移量）
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//纹理属性
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glDisableVertexAttribArray(2);

	glBindVertexArray(0);
	//是否使用线框模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	//生成纹理
	//-----------------------------------------------------------
	unsigned int texture1,texture2;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	//为当前纹理设置环绕(2)、过滤(2)方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//加载图片前, tell stb_image.h to flip loaded texture's on the y-axis.(图片上下反转)
	stbi_set_flip_vertically_on_load(true); 

	//加载并生成纹理
	int width, height, nrChannels;
	//用加载的图像来填充定义的宽度、高度、颜色通道数量
	unsigned char *data = stbi_load("../../Textures/container.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture1" << std::endl;
	}
	//生成纹理和对应多级渐远纹理后，释放图像内存
	stbi_image_free(data);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load("../../Textures/awesomeface.png", &width, &height, &nrChannels, 0);
	if (data) {
		//png注意：四通道 —— RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);		
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture2" << std::endl;
	}
	stbi_image_free(data);

	//设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元（1次即可）
	ourShader.use();		
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);		//手动设置
	ourShader.setInt("texture2", 1);		//或使用shader类来设置

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
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		
		//第一步永远是激活shader program
		ourShader.use();

		glBindVertexArray(VAO);			//绑定到VAO也会自动绑定对应的EBO
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//检查并调用事件，交换缓冲
		//----------------------------------------
		glfwSwapBuffers(window);		//等后缓冲加载完毕后呈现图像，防止图像闪烁问题（双缓冲）
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	//-----------------------------------------------------------
	glDeleteVertexArrays(1,&VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

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
