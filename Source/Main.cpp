#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "ShaderHandler.cpp"

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 1080


void framebuf_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLFWwindow* InitWindow();
void InitTexture(GLuint& texture, const char* path, int& width, int& height, int& channels);
void InitBuffers(GLuint& VBO, GLuint& EBO, GLuint &VAO);

float vertices[] = {
	//pos					//tex
	 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,	//top right
	-0.5f,  0.5f, 0.0f,		0.0f, 1.0f,	//top left
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,	//bottom left
	 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,	//bottom right
	 - 0.5f, -0.5f, 0.0f,		0.0f, 0.0f,	//bottom left
	0.5f,  0.5f, 0.0f,		1.0f, 1.0f	//top right
};

//int indices[] = {
//	0, 1, 2,
//	2, 3, 0
//};

int main()
{
	GLFWwindow* window = InitWindow();

	unsigned int VBO, EBO, VAO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	InitBuffers(VBO, EBO, VAO);

	Shader currShader("Source/Shader/vertex.vert", "Source/Shader/fragment.frag");

	unsigned int texture;
	int width, height, channels;
	glGenTextures(1, &texture);
	InitTexture(texture, "Source/Tex/container.jpg", width, height, channels);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), (float) WINDOW_HEIGHT / WINDOW_WIDTH, 0.1f, 100.0f);

	currShader.Use();
	//glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


	glEnable(GL_DEPTH_TEST);

	//Render Loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.5f, 0.5f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		float part = 360.0f / 8;
		for (int i = 0; i < 8; i++)
		{
			glm::mat4 temprot = glm::mat4(1.0f);
			temprot = glm::rotate(temprot, glm::radians(part * (i + 1)), glm::vec3(0.0f, 1.0f, 0.0f));
			temprot = glm::translate(temprot, glm::vec3(0.0f, 0.0f, -1.3f));
			glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(temprot));
			view = glm::rotate(view, (float)glfwGetTime() * glm::radians(0.001f), glm::vec3(0.1f, 0.1f, 0.1f));
			glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

void InitBuffers(GLuint& VBO, GLuint &EBO, GLuint& VAO)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void framebuf_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

GLFWwindow* InitWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window, exiting" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD, exiting" << std::endl;
		return NULL;
	}
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuf_size_callback);

	return window;
}


void InitTexture(GLuint &texture, const char* path, int &width, int &height, int &channels)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
	if (!data)
	{
		std::cout << "Failed to load texture, exiting." << std::endl;
		return;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}