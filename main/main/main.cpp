#include<glad\glad.h>		//确保放在最前面，需要在其他依赖于opengl的头文件前包含
#include<GLFW\glfw3.h>
#include<iostream>

//窗口回调函数
void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//传入的是标准化坐标，因此几乎没怎么处理就传出了（仅仅加上了w分量）
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
	//创建一个顶点着色器对象
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//将着色器源码附到着色器对象上并编译
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//检查顶点着色器是否编译成功
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//编辑片段着色器
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//创建着色器程序
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	//着色器对象链接到程序对象后，记得删除着色器
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//设置顶点数据、顶点缓冲以及确认顶点缓冲解读方式(vertex attributes)
	//-----------------------------------------------------------
	//设置标准化顶点坐标
	float vertices[] = {
		0.5f, 0.5f, 0.0f,   // 右上角
		0.5f, -0.5f, 0.0f,  // 右下角
		-0.5f, -0.5f, 0.0f, // 左下角
		-0.5f, 0.5f, 0.0f   // 左上角
	};
	//设置顶点索引    给EBO使用
	unsigned int indices[] = {
		0, 1, 3,	//第一个三角形
		1, 2, 3		//第二个三角形
	};
	
	//定义顶点缓冲对象，用于一次性发送大批数据到显卡上
	unsigned int VBO;			
	glGenBuffers(1, &VBO);		//用ID和该函数生成一个VBO
	//创建一个VAO（一个VAO表示对VBO中数据的一种解释以及一个EBO，相当于一种状态配置）
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	//创建EBO（索引绘制）  也是buffer，与VBO类似
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	
	//首先绑定好VAO，下面开始配置这个VAO
	glBindVertexArray(VAO);

	//绑定该VBO到当前上下文，之后使用的任何（在GL_ARRAY_BUFFER目标上的）缓冲调用都会用来配置当前绑定的缓冲(VBO)。
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//将之前定义的顶点数据复制到缓冲的内存中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	//绑定EBO（注意绑定VAO时，绑定的最后一个EBO自动存储为该VAO的元素缓冲区对象）
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//告诉openGL如何解析顶点数据（当前绑定的VBO中的数据）   注意是当前
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//unbind VBO和VAO（其实大部分情况下不需要）
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//注意不要在VAO有效时解绑EBO
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//解绑VAO
	glBindVertexArray(0);

	//是否使用线框模式
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
		
		//使用着色器程序画三角形
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);			//绑定到VAO也会自动绑定对应的EBO
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//绘制模式使用glDrawElements而不是glDrawArrays
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);		//不需要每次都unbind他

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
	glDeleteProgram(shaderProgram);

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
