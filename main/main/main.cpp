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
unsigned int loadCubemap(std::vector<std::string> faces);						//加载天空盒的函数

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
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);		//记录光源位置的位移矩阵（给model用的）
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
	//1.模板测试（先进行）
	glEnable(GL_STENCIL_TEST);
	//设置掩码
	//glStencilMask(0xFF);		// 启用写入，每一位写入模板缓冲时都保持原样（原理：与1位与，默认也是1)
	//glStencilMask(0x00);		// 每一位在写入模板缓冲时都会变成0（禁用写入）
	//应该对缓冲内容做什么：glStencilFunc
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);		//只有片段的模板值为0时才能通过测试并被绘制
	
	//=============================================
	//体会这两种的差别：
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);		//被地板遮住时会绘制全绿
	glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);		//被地板遮住时只绘制边框

	//原理：被地板遮住的物体通不过深度测试，因此会执行第二个参数的要求，接着就好明白了
	//=============================================


	//2.深度测试（后进行）
	glEnable(GL_DEPTH_TEST);				//设置启用深度测试
	//glDepthMask(GL_FALSE);				//是否禁用深度缓冲的写入，开启后使用才有效
	//glDepthFunc(GL_ALWAYS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))
	glDepthFunc(GL_LESS); // 与正常相同（更小才会通过测试而被绘制，更远的片段会被丢弃）

	//  **********  深度缓冲中的值在屏幕空间中不是线性的!!!!!!!!!!!!!!!!!
	// 事实上，近处的物体深度精度大，远处的物体深度精度很小
	// 实际上就是projection model里面games101讲到的问题，远处的空间会被挤压的更小


	//3.混合（最后进行）
	//效果：使得每当OpenGL渲染了一个片段时，它都会将当前片段的颜色和当前颜色缓冲中的片段颜色根据alpha值来进行混合
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		//设置因子
	//问题：深度测试时不会考虑透明的问题，因此需要考虑前后距离的问题来解决半透明物体不显示后方物体的问题


	//面剔除 —— 定义三角形时全部按照逆时针的方向来定义，这样可以很好的完成面剔除工作
	glEnable(GL_CULL_FACE);			//背向面将会被丢弃（针对封闭形状）
	//如果要画草这种的话：关闭面剔除，因为正反面都应该可见
	glCullFace(GL_BACK);	//只剔除逆向面（默认是BACK，顺向面：FRNOT）
	glFrontFace(GL_CCW);	//定义逆时针的面为正向面，CW是顺时针
	
	glDisable(GL_CULL_FACE);


	//bulid shader program
	//-----------------------------------------------------------
	//物体的shader
	Shader shader("../../Shader/vertex_shader.glsl", "../../Shader/fragment_shader.glsl");
	Shader shaderSingleColor("../../Shader/stencil_vs.glsl", "../../Shader/stencil_fs.glsl");
	
	//光源shader
	Shader lightCubeShader("../../Shader/light_vs.glsl", "../../Shader/light_fs.glsl");
	
	//透明/半透明物体shader
	Shader transparentShader("../../Shader/basic_vs.glsl", "../../Shader/basic_fs.glsl");
	
	//最简单的shader，因为只需要画出一个四边形之后，把帧缓冲的颜色缓冲附件(texture)贴上去就行
	Shader screenShader("../../Shader/framebuffer_screen_vs.glsl", "../../Shader/framebuffer_screen_fs.glsl");

	//天空盒shader
	Shader skyboxShader("../../Shader/skybox_vs.glsl", "../../Shader/skybox_fs.glsl");

	//反射盒shader
	Shader reflectShader("../../Shader/reflectbox_vs.glsl", "../../Shader/reflectbox_fs.glsl");

	//折射盒shader
	Shader refractShader("../../Shader/refractbox_vs.glsl", "../../Shader/refractbox_fs.glsl");

	//物体模型
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
	//最后渲染图像时的四边形（整个屏幕）的坐标
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	   // positions   // texCoords
	   -1.0f,  1.0f,  0.0f, 1.0f,
	   -1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	//天空盒顶点位置
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

	//反射盒的顶点数据  顶点+法线
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

	//透明纹理的位置
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

	//光源
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

	//加载天空盒：
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

	//创建color attachment texture（颜色纹理附件，还包括深度和模板）,大致操作和纹理一样
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	//** 这边，选择屏幕的长度和宽度，然后数据填NULL，只是分配空间，暂时没有存放颜色数据，之后渲染时会输入进去
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	//使用核进行采样时使用 ==================================================
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//====================================================================
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//将颜色附件附到framebuffer上
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	//create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	// 不会在其中采样（不会进行处理） —— 使用渲染缓冲对象； 要采样 —— 使用纹理附件
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);		//绑定渲染缓冲对象，之后对其进行设置
	//说明rbo一个深度和模板渲染缓冲对象
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	//附加这个rbo到framebuffer上
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


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

		// bind to framebuffer and draw scene as we normally would to color texture
		// 不会画到窗口，而会画到纹理附件上去
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		//*****************  进行重置，本来没写 —— 写上的好处：每次循环重置为初始状态，防止上次循环的影响  ********************
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);			

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);	//状态设置
		//每次渲染迭代前清除深度缓冲&颜色缓冲&模板缓冲（否则前一帧的深度信息仍然保存在缓冲中）
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// ********************* 注意看后面的glStencilMask(0xFF)的作用

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		////第一步：先画天空盒，并且不允许深度写入，从而永远绘制在其他物体后
		//glDepthMask(GL_FALSE);		//禁用深度写入（先画，后面有其他物体的话就被覆盖）
		//skyboxShader.use();
		////设置观察和投影矩阵
		//skyboxShader.setMat4("projection", projection);
		////skyboxShader.setMat4("view", view);		//这样的话天空盒就和一般的盒子没区别，会动
		////更改如下：相当于去掉了view矩阵中的位移部分，而保留了旋转和变换
		//skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));	
		//glBindVertexArray(skyboxVAO);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDepthMask(GL_TRUE);		//开启深度写入
		
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
		//一定记得要加 ！！！
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

		//先画所有不透明的物体
		//1.开始时绘制地板 —— 不需要边框，因此设置不经过模板缓冲
		glStencilMask(0x00);		//禁用写入

		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);		//这里可以没有（默认激活0）
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		shader.setMat4("model", glm::mat4(1.0f));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		//2.绘制物体，但是要经过模板测试，修改stencil buffer
		glStencilMask(0xFF);			//允许写入
		glStencilFunc(GL_ALWAYS, 1, 0xFF);		//只要进行绘制，都永远通过，且更改模板值为1（op规定的）
		//glEnable(GL_CULL_FACE);		//启用面剔除

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

		//将物体放大一点点后绘制（禁止写入模板值），只有非1的位置会被绘制，达到边缘效果
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);	//不为1的才能绘制（对于屏幕而言 —— 所以不是整个覆盖在外面而是边缘）
		glStencilMask(0x00);					//禁止写入模板值（防止绘制成功时模板值被op改成1）
		glDisable(GL_DEPTH_TEST);				//禁用深度测试，从而边框可以透视
		shaderSingleColor.use();
		float scale = 1.05f;

		// cubes
		glBindVertexArray(cubeVAO);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, -0.5f, -1.0f));
		model = glm::scale(model, glm::vec3(scale));		//新增的scale
		shaderSingleColor.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, -0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(scale));		//新增的scale
		shaderSingleColor.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		
		//恢复到默认状态
		glStencilMask(0xFF);		//恢复成可写，这样的话渲染循环中的glClear才能清除模板缓存
		glStencilFunc(GL_ALWAYS, 0, 0xFF);		//默认写0，只有在需要模板写入时才写1
		glEnable(GL_DEPTH_TEST);

		//画点光源物体
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
		
		//反射光物体：
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

		//折射光物体：
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

		//反射/折射模型（具体在nano_fs里面改）
		modelShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0, -0.5, 2.0));
		model = glm::scale(model, glm::vec3(0.15f));
		modelShader.setMat4("model", model);
		modelShader.setVec3("viewPos", camera.Position);
		ourModel.Draw(modelShader);

		
		//优化：最后画天空盒
		//（注意要在半透明物体前画，不然透明部分如果还没有其他颜色的话会变成背景色(写入深度数据)，
		//致使天空盒通不过深度测试，导致窗户看不到天空盒）

		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		//使用LEQUAL的原因：深度测试的默认初始值为1，在经过优化后，天空盒的深度一直为1，如果是LESS，可能通不过测试，无法画出来
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

		//画窗户
		//记录距离位置
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

		//现在回到default framebuffer中，画一个四边形，并将纹理贴上去
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		//=======================  存疑：为什么这个注释掉后会有问题？ =========================
		glDisable(GL_DEPTH_TEST);	// disable depth test so screen-space quad isn't discarded due to depth test.
		
		//这边disable stencil_test导致箱子被边框覆盖的原因：
		//本来循环开始没有enable stencil test，导致下一次循环渲染时模板测试未开，从而纹理内渲染错误
		//现已经在开头加上，且这里本来也不需要disable模板测试
		//glDisable(GL_STENCIL_TEST);

		// clear all relevant buffers
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);

		screenShader.use();
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//glEnable(GL_STENCIL_TEST);

		//检查并调用事件，交换缓冲
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

	//灯光移动 (上下左右12)
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		lightPos.y += cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		lightPos.y -= cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		lightPos.x -= cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		lightPos.x += cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)	//小键盘：KP
		lightPos.z -= cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
		lightPos.z += cameraSpeed;
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

		//注意新的变化 —— 如果是repeat，会导致上面的透明部分和底下的纯色部分进行混色，从而出现很窄的边框
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
			//按照openGL默认的顺序来进行立方体贴图的设置
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	//设置纹理环绕方式
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	return textureID;
}