#include<glad/glad.h>		//确保放在最前面，需要在其他依赖于opengl的头文件前包含
#include<GLFW/glfw3.h>

#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>

#include"shader.h"
#include"camera.h"
#include"model.h"

#include<iostream>

//图像加载库
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);		//窗口回调函数
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);			//鼠标回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);		//滚轮回调函数
void processInput(GLFWwindow *window);											//键盘监听函数
unsigned int loadTexture(const char *path);										//添加材质的函数

//settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//创建摄像机(实际上是物体在运动，通过loolAt矩阵来让物体反着运动造成摄像机移动的假象)
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2;	//鼠标上一帧的位置
float lastY = SCR_HEIGHT / 2;	//鼠标上一帧的位置
bool firstMouse = true;

//跟踪渲染的时间差，从而保证不同硬件上相机移动速度相应平衡  timing
float deltaTime = 0.0f;		//当前帧与上一帧的时间差
float lastFrame = 0.0f;		//上一帧的时间

//光源设置
//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);		//记录光源位置的位移矩阵（给model用的）
//glm::vec3 lightColor;

int main() {
	// glfw: initialize and configure
	//-----------------------------------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//创建窗口对象
	//-----------------------------------------------------------
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Screen", NULL, NULL);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//注册回调函数
	//-----------------------------------------------------------
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);	//注册窗口回调函数
	glfwSetCursorPosCallback(window, mouse_callback);					//注册鼠标回调函数
	glfwSetScrollCallback(window, scroll_callback);						//注册滚动回调函数


	//告诉GLFW捕捉鼠标
	//-----------------------------------------------------------
	//设置光标隐藏与停留(进入窗口后)
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//调用opengl函数前需要初始化glad(load all opengl function pointers)
	//-----------------------------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//configure global opengl state
	//-----------------------------------------------------------
	glEnable(GL_DEPTH_TEST);				//设置启用深度测试

	//bulid shader program
	//-----------------------------------------------------------
	//物体的shader
	Shader ourShader("../../Shader/nano_vs.glsl", "../../Shader/nano_fs.glsl");

	//加载模型
	Model ourModel("../../Models/nanosuit/nanosuit.obj");

	//是否使用线框模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//进入渲染循环	
	//-----------------------------------------------------------
	while (!glfwWindowShouldClose(window)) {
		//跟踪每帧的时间差  per-frame time logic
		//----------------------------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//输入
		//----------------------------------------
		processInput(window);

		//渲染指令
		//----------------------------------------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);	//状态设置
		//每次渲染迭代前清除深度缓冲&颜色缓冲（否则前一帧的深度信息仍然保存在缓冲中）
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();

		//projection和view矩阵
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

		//检查并调用事件，交换缓冲
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
	glViewport(0, 0, width, height);	//左下角位置，视口宽度，视口高度
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	//防止开始跳一下
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	//记录当前帧与上一帧的鼠标偏移量
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;	//相反：因为y坐标是从下往上依次增大的

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow *window) {
	//esc键关闭窗口
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);		//终止下一次while循环的条件

	//移动部分(与camera类交互)
	float cameraSpeed = 3.0f * deltaTime;		//镜头位置移动速度
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	//注意需要标准化，不然朝向不同移动的速度就不同了（返回的叉乘结果不同）
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);

	////灯光移动 (上下左右12)
	//if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	//	lightPos.y += cameraSpeed;
	//if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	//	lightPos.y -= cameraSpeed;
	//if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	//	lightPos.x -= cameraSpeed;
	//if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	//	lightPos.x += cameraSpeed;
	//if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)	//小键盘：KP
	//	lightPos.z -= cameraSpeed;
	//if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
	//	lightPos.z += cameraSpeed;
}


unsigned int loadTexture(char const * path)
{
	//生成纹理
	unsigned int textureID;
	glGenTextures(1, &textureID);

	//加载纹理属性
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
		//设置该纹理对象（之后绑定该对象即为绑定此次设置）
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