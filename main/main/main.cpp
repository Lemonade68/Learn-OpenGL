#include<glad/glad.h>		//ȷ��������ǰ�棬��Ҫ������������opengl��ͷ�ļ�ǰ����
#include<GLFW/glfw3.h>

#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>

#include"shader.h"
#include"camera.h"
#include"model.h"

#include<iostream>

//ͼ����ؿ�
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);		//���ڻص�����
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);			//���ص�����
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);		//���ֻص�����
void processInput(GLFWwindow *window);											//���̼�������

unsigned int loadTexture(const char *path);										//��Ӳ��ʵĺ���
unsigned int loadCubemap(std::vector<std::string> faces);						//������պеĺ���
void loadShaderLightPara(Shader &shader, glm::vec3 pointLightPositions[]);		

//settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//���������(ʵ�������������˶���ͨ��loolAt�����������巴���˶����������ƶ��ļ���)
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2;	//�����һ֡��λ��
float lastY = SCR_HEIGHT / 2;	//�����һ֡��λ��
bool firstMouse = true;

//������Ⱦ��ʱ���Ӷ���֤��ͬӲ��������ƶ��ٶ���Ӧƽ��  timing
float deltaTime = 0.0f;		//��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f;		//��һ֡��ʱ��

bool blinn = false;
bool blinnKeyPressed = false;

//��Դ����
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);		//��¼��Դλ�õ�λ�ƾ��󣨸�model�õģ�
//glm::vec3 lightColor;

int main() {
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// build and compile shaders
	// -------------------------
	Shader shader("../../Shader/blinn_phong_vs.glsl", "../../Shader/blinn_phong_fs.glsl");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float planeVertices[] = {
		// positions            // normals         // texcoords
		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
		 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
	};
	// plane VAO
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	// load textures
	// -------------
	unsigned int floorTexture = loadTexture("../../Textures/wood.png");

	// shader configuration
	// --------------------
	shader.use();
	shader.setInt("texture1", 0);

	// lighting info
	// -------------
	glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw objects
		shader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		// set light uniforms
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("lightPos", lightPos);
		shader.setInt("blinn", blinn);
		// floor
		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		std::cout << (blinn ? "Blinn-Phong" : "Phong") << std::endl;

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);

	glfwTerminate();
	return 0;
}


void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);	//���½�λ�ã��ӿڿ�ȣ��ӿڸ߶�
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	//��ֹ��ʼ��һ��
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	//��¼��ǰ֡����һ֡�����ƫ����
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;	//�෴����Ϊy�����Ǵ����������������

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow *window) {
	//esc���رմ���
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);		//��ֹ��һ��whileѭ��������

	//�ƶ�����(��camera�ཻ��)
	float cameraSpeed = 2.5f * deltaTime;		//��ͷλ���ƶ��ٶ�
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	//ע����Ҫ��׼������Ȼ����ͬ�ƶ����ٶȾͲ�ͬ�ˣ����صĲ�˽����ͬ��
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);

	//�ƹ��ƶ� (��������12)
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		lightPos.y += cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		lightPos.y -= cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		lightPos.x -= cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		lightPos.x += cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)	//С���̣�KP
		lightPos.z -= cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
		lightPos.z += cameraSpeed;

	//���½��л�һ�εĴ��루ѧϰ������
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)		
	{
		blinn = !blinn;
		blinnKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
	{
		blinnKeyPressed = false;
	}
}


unsigned int loadTexture(char const * path)
{
	//��������
	unsigned int textureID;
	glGenTextures(1, &textureID);

	//������������
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
		//���ø��������֮��󶨸ö���Ϊ�󶨴˴����ã�
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//ע���µı仯 ���� �����repeat���ᵼ�������͸�����ֺ͵��µĴ�ɫ���ֽ��л�ɫ���Ӷ����ֺ�խ�ı߿�
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
			//����openGLĬ�ϵ�˳����������������ͼ������
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	//���������Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void loadShaderLightPara(Shader &shader, glm::vec3 pointLightPositions[]) {
	shader.use();

	//directional light
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

	//spot light
	shader.setVec3("spotLight.position", camera.Position);
	shader.setVec3("spotLight.direction", camera.Front);
	shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("spotLight.constant", 1.0f);
	shader.setFloat("spotLight.linear", 0.09f);
	shader.setFloat("spotLight.quadratic", 0.032f);
	shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
}