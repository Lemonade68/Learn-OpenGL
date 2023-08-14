//#include<glad/glad.h>		//ȷ��������ǰ�棬��Ҫ������������opengl��ͷ�ļ�ǰ����
//#include<GLFW/glfw3.h>
//
//#include<glm\glm.hpp>
//#include<glm\gtc\matrix_transform.hpp>
//#include<glm\gtc\type_ptr.hpp>
//
//#include"shader.h"
//#include"camera.h"
//#include"model.h"
//
//#include<iostream>
//
////ͼ����ؿ�
//#define STB_IMAGE_IMPLEMENTATION
//#include"stb_image.h"
//
//void framebuffer_size_callback(GLFWwindow *window, int width, int height);		//���ڻص�����
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);			//���ص�����
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);		//���ֻص�����
//void processInput(GLFWwindow *window);											//���̼�������
//
//unsigned int loadTexture(const char *path);										//��Ӳ��ʵĺ���
//unsigned int loadCubemap(std::vector<std::string> faces);						//������պеĺ���
//void loadShaderLightPara(Shader &shader, glm::vec3 pointLightPositions[]);
//
////settings
//const unsigned int SCR_WIDTH = 800;
//const unsigned int SCR_HEIGHT = 600;
//
////���������(ʵ�������������˶���ͨ��loolAt�����������巴���˶����������ƶ��ļ���)
//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//float lastX = SCR_WIDTH / 2;	//�����һ֡��λ��
//float lastY = SCR_HEIGHT / 2;	//�����һ֡��λ��
//bool firstMouse = true;
//
////������Ⱦ��ʱ���Ӷ���֤��ͬӲ��������ƶ��ٶ���Ӧƽ��  timing
//float deltaTime = 0.0f;		//��ǰ֡����һ֡��ʱ���
//float lastFrame = 0.0f;		//��һ֡��ʱ��
//
////��Դ����
//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);		//��¼��Դλ�õ�λ�ƾ��󣨸�model�õģ�
////glm::vec3 lightColor;
//
//int main() {
//	// glfw: initialize and configure
//	//-----------------------------------------------------------
//	glfwInit();
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//	//�������ڶ���
//	//-----------------------------------------------------------
//	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Screen", NULL, NULL);
//	if (window == nullptr) {
//		std::cout << "Failed to create GLFW window" << std::endl;
//		glfwTerminate();
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//
//	//ע��ص�����
//	//-----------------------------------------------------------
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);	//ע�ᴰ�ڻص�����
//	glfwSetCursorPosCallback(window, mouse_callback);					//ע�����ص�����
//	glfwSetScrollCallback(window, scroll_callback);						//ע������ص�����
//
//
//	//����GLFW��׽���
//	//-----------------------------------------------------------
//	//���ù��������ͣ��(���봰�ں�)
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//	//����opengl����ǰ��Ҫ��ʼ��glad(load all opengl function pointers)
//	//-----------------------------------------------------------
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//		std::cout << "Failed to initialize GLAD" << std::endl;
//		return -1;
//	}
//
//	//configure global opengl state
//	//-----------------------------------------------------------
//	glEnable(GL_DEPTH_TEST);
//
//	//bulid shader program
//	//-----------------------------------------------------------
//	Shader planetShader("../../Shader/basic_vs.glsl", "../../Shader/basic_fs.glsl");
//	Shader rockShader("../../Shader/rock_vs.glsl", "../../Shader/rock_fs.glsl");
//
//	Model rock("../../Models/rock/rock.obj");
//	Model planet("../../Models/planet/planet.obj");
//
//	// generate a large list of semi-random model transformation matrices
//	// ------------------------------------------------------------------
//	unsigned int amount = 10000;
//	glm::mat4* modelMatrices;
//	modelMatrices = new glm::mat4[amount];
//	srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
//	float radius = 50.0;
//	float offset = 12.5f;
//	for (unsigned int i = 0; i < amount; i++) {
//		glm::mat4 model = glm::mat4(1.0f);
//		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
//		float angle = (float)i / (float)amount * 360.0f;
//		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
//		float x = sin(angle) * radius + displacement;
//		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
//		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
//		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
//		float z = cos(angle) * radius + displacement;
//		model = glm::translate(model, glm::vec3(x, y, z));
//
//		// 2. scale: Scale between 0.05 and 0.25f
//		float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
//		model = glm::scale(model, glm::vec3(scale));
//
//		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
//		float rotAngle = static_cast<float>((rand() % 360));
//		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
//
//		// 4. now add to list of matrices
//		modelMatrices[i] = model;
//	}
//
//	//ʹ��ʵ��������ķ�ʽ��
//	unsigned int buffer;
//	glGenBuffers(1, &buffer);
//	glBindBuffer(GL_ARRAY_BUFFER, buffer);
//	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
//
//	for (unsigned int i = 0; i < rock.meshes.size(); ++i) {
//		unsigned int VAO = rock.meshes[i].VAO;
//		glBindVertexArray(VAO);
//		//��������(�൱����Ĭ�ϵ����Ժ������д)
//		GLsizei vec4Size = sizeof(glm::vec4);
//		glEnableVertexAttribArray(3);
//		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
//		glEnableVertexAttribArray(4);
//		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
//		glEnableVertexAttribArray(5);
//		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
//		glEnableVertexAttribArray(6);
//		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
//
//		//����ʵ����
//		glVertexAttribDivisor(3, 1);
//		glVertexAttribDivisor(4, 1);
//		glVertexAttribDivisor(5, 1);
//		glVertexAttribDivisor(6, 1);
//
//		glBindVertexArray(0);
//	}
//
//
//	//�Ƿ�ʹ���߿�ģʽ
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//	//������Ⱦѭ��
//	//-----------------------------------------------------------
//	while (!glfwWindowShouldClose(window)) {
//		//����ÿ֡��ʱ���  per-frame time logic
//		//----------------------------------------
//		float currentFrame = static_cast<float>(glfwGetTime());
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//
//		//����
//		//----------------------------------------
//		processInput(window);
//
//		//��Ⱦָ��
//		//----------------------------------------
//
//		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		// configure transformation matrices
//		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
//		glm::mat4 view = camera.GetViewMatrix();;
//		rockShader.use();
//		rockShader.setMat4("projection", projection);
//		rockShader.setMat4("view", view);
//
//		planetShader.use();
//		planetShader.setMat4("projection", projection);
//		planetShader.setMat4("view", view);
//
//		// draw planet
//		glm::mat4 model = glm::mat4(1.0f);
//		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
//		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
//		planetShader.setMat4("model", model);
//		planet.Draw(planetShader);
//
//		rockShader.use();
//		rockShader.setInt("texture_diffuse1", 0);		//Ҫ���ֶ�instanced��������Ҫ�ֶ���������
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id);
//
//		for (unsigned int i = 0; i < rock.meshes.size(); i++) {		//���ﲻ�Ǳ�����ʯ�����Σ�����rock�������������
//			glBindVertexArray(rock.meshes[i].VAO);
//			//��������amount�ű�ʾҪ��amount����ʯ
//			glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rock.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);
//			glBindVertexArray(0);
//		}
//
//		//��鲢�����¼�����������
//		//----------------------------------------
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	// glfw: terminate, clearing all previously allocated GLFW resources.
//	//-----------------------------------------------------------
//	glfwTerminate();
//	return 0;
//}
//
//
//void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
//	glViewport(0, 0, width, height);	//���½�λ�ã��ӿڿ�ȣ��ӿڸ߶�
//}
//
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
//	float xpos = static_cast<float>(xposIn);
//	float ypos = static_cast<float>(yposIn);
//
//	//��ֹ��ʼ��һ��
//	if (firstMouse) {
//		lastX = xpos;
//		lastY = ypos;
//		firstMouse = false;
//	}
//
//	//��¼��ǰ֡����һ֡�����ƫ����
//	float xoffset = xpos - lastX;
//	float yoffset = lastY - ypos;	//�෴����Ϊy�����Ǵ����������������
//
//	lastX = xpos;
//	lastY = ypos;
//
//	camera.ProcessMouseMovement(xoffset, yoffset);
//}
//
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
//	camera.ProcessMouseScroll(static_cast<float>(yoffset));
//}
//
//void processInput(GLFWwindow *window) {
//	//esc���رմ���
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);		//��ֹ��һ��whileѭ��������
//
//	//�ƶ�����(��camera�ཻ��)
//	float cameraSpeed = 100.0f * deltaTime;		//��ͷλ���ƶ��ٶ�
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		camera.ProcessKeyboard(FORWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		camera.ProcessKeyboard(BACKWARD, deltaTime);
//	//ע����Ҫ��׼������Ȼ����ͬ�ƶ����ٶȾͲ�ͬ�ˣ����صĲ�˽����ͬ��
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		camera.ProcessKeyboard(LEFT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		camera.ProcessKeyboard(RIGHT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
//		camera.ProcessKeyboard(UP, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
//		camera.ProcessKeyboard(DOWN, deltaTime);
//
//	//�ƹ��ƶ� (��������12)
//	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
//		lightPos.y += cameraSpeed;
//	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
//		lightPos.y -= cameraSpeed;
//	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
//		lightPos.x -= cameraSpeed;
//	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
//		lightPos.x += cameraSpeed;
//	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)	//С���̣�KP
//		lightPos.z -= cameraSpeed;
//	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
//		lightPos.z += cameraSpeed;
//}
//
//
//unsigned int loadTexture(char const * path)
//{
//	//��������
//	unsigned int textureID;
//	glGenTextures(1, &textureID);
//
//	//������������
//	int width, height, nrComponents;
//	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
//	if (data) {
//		GLenum format;
//		if (nrComponents == 1)
//			format = GL_RED;
//		else if (nrComponents == 3)
//			format = GL_RGB;
//		else if (nrComponents == 4)
//			format = GL_RGBA;
//		//���ø��������֮��󶨸ö���Ϊ�󶨴˴����ã�
//		glBindTexture(GL_TEXTURE_2D, textureID);
//		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//
//		//ע���µı仯 ���� �����repeat���ᵼ�������͸�����ֺ͵��µĴ�ɫ���ֽ��л�ɫ���Ӷ����ֺ�խ�ı߿�
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		stbi_image_free(data);
//	}
//	else {
//		std::cout << "Texture failed to load at path: " << path << std::endl;
//		stbi_image_free(data);
//	}
//	return textureID;
//}
//
//
//unsigned int loadCubemap(std::vector<std::string> faces) {
//	unsigned int textureID;
//	glGenTextures(1, &textureID);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
//
//	int width, height, nrChannels;
//	for (unsigned int i = 0; i < faces.size(); ++i) {
//		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
//		if (data) {
//			//����openGLĬ�ϵ�˳����������������ͼ������
//			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
//				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//			stbi_image_free(data);
//		}
//		else {
//			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
//			stbi_image_free(data);
//		}
//	}
//	//���������Ʒ�ʽ
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//	return textureID;
//}
//
//void loadShaderLightPara(Shader &shader, glm::vec3 pointLightPositions[]) {
//	shader.use();
//
//	//directional light
//	shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
//	shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
//	shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
//	shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
//
//	// movable light
//	shader.setVec3("moveableLight.position", lightPos);
//	shader.setVec3("moveableLight.ambient", 0.05f, 0.05f, 0.05f);
//	shader.setVec3("moveableLight.diffuse", 0.8f, 0.8f, 0.8f);
//	shader.setVec3("moveableLight.specular", 1.0f, 1.0f, 1.0f);
//	shader.setFloat("moveableLight.constant", 1.0f);
//	shader.setFloat("moveableLight.linear", 0.09f);
//	shader.setFloat("moveableLight.quadratic", 0.032f);
//
//	// point light 1
//	shader.setVec3("pointLights[0].position", pointLightPositions[0]);
//	shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
//	shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
//	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
//	shader.setFloat("pointLights[0].constant", 1.0f);
//	shader.setFloat("pointLights[0].linear", 0.09f);
//	shader.setFloat("pointLights[0].quadratic", 0.032f);
//	// point light 2
//	shader.setVec3("pointLights[1].position", pointLightPositions[1]);
//	shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
//	shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
//	shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
//	shader.setFloat("pointLights[1].constant", 1.0f);
//	shader.setFloat("pointLights[1].linear", 0.09f);
//	shader.setFloat("pointLights[1].quadratic", 0.032f);
//	// point light 3
//	shader.setVec3("pointLights[2].position", pointLightPositions[2]);
//	shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
//	shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
//	shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
//	shader.setFloat("pointLights[2].constant", 1.0f);
//	shader.setFloat("pointLights[2].linear", 0.09f);
//	shader.setFloat("pointLights[2].quadratic", 0.032f);
//	// point light 4
//	shader.setVec3("pointLights[3].position", pointLightPositions[3]);
//	shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
//	shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
//	shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
//	shader.setFloat("pointLights[3].constant", 1.0f);
//	shader.setFloat("pointLights[3].linear", 0.09f);
//	shader.setFloat("pointLights[3].quadratic", 0.032f);
//
//	//spot light
//	shader.setVec3("spotLight.position", camera.Position);
//	shader.setVec3("spotLight.direction", camera.Front);
//	shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
//	shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
//	shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
//	shader.setFloat("spotLight.constant", 1.0f);
//	shader.setFloat("spotLight.linear", 0.09f);
//	shader.setFloat("spotLight.quadratic", 0.032f);
//	shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
//	shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
//}