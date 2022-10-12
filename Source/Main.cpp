#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "ShaderHandler.cpp"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600


GLFWwindow* window;

void framebuf_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void RenderRecursive(int depth, Shader currShader, GLuint VAO, float xpos1, float xpos2, float xpos3, float ypos1, float ypos2, float ypos3, float scale)
{
	if (depth-- == -1) return;

	float xtl = (xpos1 + xpos3) / 2;
	float xtr = (xpos1 + xpos2) / 2;
	float xlr = (xpos2 + xpos3) / 2;
	float ytl = (ypos1 + ypos3) / 2;
	float ytr = (ypos1 + ypos2) / 2;
	float ylr = (ypos2 + ypos3) / 2;

	//Math Stuff
	glm::mat4 transform = glm::mat4(1.0f);
	

	transform = glm::translate(transform, glm::vec3((xtr + xtl + xlr) / 3, (ytr + ytl + ylr) / 3, 0.0f));
	transform = glm::scale(transform, glm::vec3(scale));
	transform = glm::rotate(transform, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	

	//Bind stuff
	currShader.Use();
	glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(transform));

	//Render Stuff
	glDrawArrays(GL_TRIANGLES, 0, 3);

	


	RenderRecursive(depth, currShader, VAO, xpos1, xtr, xtl, ypos1, ytr, ytl, scale / 4);
	RenderRecursive(depth, currShader, VAO, xtr, xpos2, xlr, ytr, ypos2, ylr, scale / 4);
	RenderRecursive(depth, currShader, VAO, xtl, xlr, xpos3, ytl, ylr, ypos3, scale / 4);

	glfwSwapBuffers(window);

	return;
}

int main()
{
	//Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Window Init
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window, exiting" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD, exiting" << std::endl;
		return -1;
	}
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuf_size_callback);


	//Main Shading Setup
	float vertices[] = {
		//pos						//tex
		 0.0f,  0.5f, 0.0f,			0.5f, 1.0f,	//top
		 0.5f, -0.5f, 0.0f,			1.0f, 0.0f,	//bottom right
		-0.5f, -0.5f, 0.0f,			0.0f, 0.0f	//bottom left
	};

	Shader currShader("Source/Shader/vertex.vert", "Source/Shader/fragment.frag");

	unsigned int VBO, VAO;// , EBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//Texture Setup
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	int width, height, channels;
	unsigned char* data = stbi_load("Source/Tex/container.jpg", &width, &height, &channels, 0);
	if (!data)
	{
		std::cout << "Failed to load texture, exiting." << std::endl;
		return -1;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	//Binding texture and using another
	/*GLuint texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("Source/Tex/redsun.jpg", &width, &height, &channels, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);*/

	/*glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));*/
	
	/*currShader.Use();
	glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(trans));*/
	//Render Loop
	RenderRecursive(5, currShader, VAO, vertices[0], vertices[5], vertices[10], vertices[1], vertices[6], vertices[11], 1.0f);
	while (1);
	while (glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.5f, 0.5f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		////glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//Update Transform


		/*currShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(trans));*/

		



		//glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}