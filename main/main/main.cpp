#include<glad/glad.h>		//确保放在最前面，需要在其他依赖于opengl的头文件前包含
#include<GLFW/glfw3.h>
#include<iostream>
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>
#include"Shader.h"


//图像加载库
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

//窗口回调函数
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mixValue = 0.2f;

//创建摄像机
//-----------------------------------------------------------
//view matrix	摄像机自由移动		设成全局变量从而让输入函数可以访问
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


//跟踪渲染的时间差，从而保证不同硬件上相机移动速度相应平衡
float deltaTime = 0.0f;		//当前帧与上一帧的时间差
float lastFrame = 0.0f;		//上一帧的时间

int main() {
	// glfw: initialize and configure
	//-----------------------------------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//创建窗口对象
	//-----------------------------------------------------------
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Triangle", NULL, NULL);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//调用opengl函数前需要初始化glad(load all opengl function pointers)
	//-----------------------------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//bulid shader program
	//-----------------------------------------------------------
	Shader ourShader("../../Shader/vertex_shader.glsl", "../../Shader/fragment_shader.glsl");

	//设置顶点数据、顶点缓冲以及确认顶点缓冲解读方式(vertex attributes)
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
	//	0,1,3,          //第一个三角形
	//	1,2,3			//第二个三角形
	//};

	//任何一个对象：定义，生成，绑定    ***********************

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

	//顶点位置属性指针
	//参数依次为：顶点属性值（着色器中），数据个数，类型，是否标准化，stride步长（一个顶点的数据个数），offset（偏移量）
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//顶点颜色属性指针 (起始偏移位置为第一个顶点的3个位置属性)
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	//顶点纹理坐标指针
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//是否使用线框模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	//生成纹理
	//-----------------------------------------------------------
	unsigned int texture1, texture2;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	//设置环绕（2）和过滤（2）方式     木板设置为边缘环绕
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);   //注意缩小和放大的不同过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//加载图片前，首先使用这句语句让图像加载时翻转y轴，让纹理不会上下颠倒
	stbi_set_flip_vertically_on_load(true);

	//加载图片
	int width, height, nrChannels; //通道：例如RGB
	//用加载的图像来填充定义的宽度、高度、颜色通道数量
	unsigned char *data = stbi_load("../../Textures/container.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	//生成纹理和对应多级渐远纹理后，释放图像内存
	stbi_image_free(data);

	//第二个纹理				笑脸设置为重复环绕
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);   //注意缩小和放大的不同过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load("../../Textures/awesomeface.png", &width, &height, &nrChannels, 0);
	if (data) {
		//png注意：四通道 —— RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	//设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元（1次即可）
	ourShader.use();   //设置uniform变量前不要忘记激活货色器程序!
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);		//手动设置
	ourShader.setInt("texture2", 1);     //两种设置方式都可

	//创建变换矩阵
	//-----------------------------------------------------------
	//glm::mat4 trans = glm::mat4(1.0f);		//初始化单位矩阵
	//绕z轴旋转，且用radians转化为弧度制			//注意阅读顺序：从下往上！（矩阵从右往左）
	//trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));	
	//trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
	
	//设置不同立方体在空间中的位置
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

	////创建摄像机
	////-----------------------------------------------------------
	////摄像机位置
	//glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
	////摄像机方向 —— 实际与摄像机指向相反		摄像机+z轴
	//glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
	//glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraDirection);	
	////右轴（使用上向量和方向叉乘）	摄像机+x轴
	//glm::vec3 up(0.0f, 1.0f, 0.0f);
	//glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	////上轴（右轴和方向叉乘）	摄像机+y轴
	//glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));


	//lookat矩阵：定义一个摄像机位置，一个目标位置和一个表示世界空间中的上向量的向量（我们计算右向量使用的那个上向量）
	//glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0),
	//	glm::vec3(0.0, 0.0, 0.0),
	//	glm::vec3(0.0, 1.0, 0.0));
		
	//进入渲染循环	
	//-----------------------------------------------------------
	while (!glfwWindowShouldClose(window)) {
		//输入
		//----------------------------------------
		processInput(window);

		//渲染指令
		//----------------------------------------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	//状态设置
		glEnable(GL_DEPTH_TEST);				//设置启用深度测试
		//每次渲染迭代前清除深度缓冲&颜色缓冲（否则前一帧的深度信息仍然保存在缓冲中）
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			

		//TEXTURE0默认是被激活的，因此只有一个纹理时不需要考虑激活纹理单元的问题
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		//第一步永远是激活shader program
		ourShader.use();

		//设置mixValue值
		ourShader.setFloat("transparancy", mixValue);

		glBindVertexArray(VAO);		//绑定到VAO也会自动绑定对应的EBO

		//坐标系统
		//-----------------------------------------------------------

		////view matrix  摄像机绕y轴旋转
		/*float radius = 10.0f;
		float camX = sin(glfwGetTime())*radius;
		float camZ = cos(glfwGetTime())*radius;
		glm::mat4 view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));*/

		//view	要放在渲染循环里，这样才能时刻更新
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//这里第二个参数的含义：不管镜头怎么动，观察方向都是前方（不会聚焦在一个点上）  点加向量还是点！

		//projection matrix (perspective projection)
		glm::mat4 projection(1.0f);
		float FoV = 45.0f;
		projection = glm::perspective(glm::radians(FoV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		//参数：fov，宽高比，近平面z值，远平面z值，注意这边的float要加，不然不能自动转好像（会报函数没有重载的问题）

		//glm::mat4 trans = projection * view * model;

		for (unsigned int i = 0; i < 10; ++i) {
			//新的model
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

		//跟踪时间差
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//检查并调用事件，交换缓冲
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
	glViewport(0, 0, width, height);	//左下角位置，视口宽度，视口高度
}

void processInput(GLFWwindow *window) {
	//esc键关闭窗口
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);		//终止下一次while循环的条件
	//更改透明度
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
	//移动部分
	float cameraSpeed = 3.0f * deltaTime;		//镜头位置移动速度
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	//注意需要标准化，不然朝向不同移动的速度就不同了（返回的叉乘结果不同）
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}