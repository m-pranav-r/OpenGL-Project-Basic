//SDL Includes Setup
#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

//GLAD Include
#include <glad/glad.h>

//STB Include
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//GLM Includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//C++ Lib Includes
#include <iostream>

//Custom Includes
#include "ShaderHandler.cpp"

//Window Sizes
#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 1080

//Kill flag
bool kill = false;

SDL_Window* InitWindowSDL();
void InitBuffers(GLuint& VBO, GLuint& EBO, GLuint& VAO);
void InitTexture(GLuint& texture, const char* path, int& width, int& height, int& channels);
void processInput(SDL_Window* window);

//Vertex array
float vertices[] = {
	//pos					//tex
	 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,	//top right
	-0.5f,  0.5f, 0.0f,		0.0f, 1.0f,	//top left
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,	//bottom left
	 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,	//bottom right
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,	//bottom left
	 0.5f,  0.5f, 0.0f,		1.0f, 1.0f	//top right
};

//Camera Vectors
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//Frametime calculation
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Keyboard Event handle
SDL_Event KeyEvent;

//Bye, GLFW! <3

int main(int argc, char* args[])
{
	SDL_GLContext* context = NULL;
	SDL_Window* window = InitWindowSDL();
	SDL_Surface* screenSurface = NULL;
	screenSurface = SDL_GetWindowSurface(window);

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

	//Transforms Setup
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_HEIGHT / WINDOW_WIDTH, 0.1f, 100.0f);

	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	//Shader Setup
	currShader.Use();
	//glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//Enable Z-Tests
	glEnable(GL_DEPTH_TEST);

	while (!kill)
	{
		Uint64 currentFrame = SDL_GetPerformanceCounter();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		glClearColor(0.5f, 0.5f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//Render initial band
		float part = 360.0f / 8;
		for (int i = 0; i < 8; i++)
		{
			glm::mat4 temprot = glm::mat4(1.0f);
			temprot = glm::scale(temprot, glm::vec3(0.5f, 0.5f, 0.5f));
			temprot = glm::rotate(temprot, glm::radians(part * (i + 1)), glm::vec3(0.0f, 1.0f, 0.0f));
			temprot = glm::translate(temprot, glm::vec3(0.0f, 0.0f, -1.3f));
			glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(temprot));
			//view = glm::rotate(view, (float)glfwGetTime() * glm::radians(0.001f), glm::vec3(0.1f, 0.1f, 0.1f));
			glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}


		//Second band
		for (int i = 0; i < 8; i++)
		{
			glm::mat4 temprot = glm::mat4(1.0f);
			temprot = glm::scale(temprot, glm::vec3(0.5f, 0.5f, 0.5f));
			temprot = glm::rotate(temprot, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			temprot = glm::rotate(temprot, glm::radians(part * (i + 1)), glm::vec3(0.0f, 1.0f, 0.0f));
			temprot = glm::translate(temprot, glm::vec3(0.0f, 0.0f, -1.3f));
			glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(temprot));
			//view = glm::rotate(view, (float)glfwGetTime() * glm::radians(0.001f), glm::vec3(0.1f, 0.1f, 0.1f));
			glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		//Third band
		for (int i = 0; i < 8; i++)
		{
			glm::mat4 temprot = glm::mat4(1.0f);
			temprot = glm::scale(temprot, glm::vec3(0.5f, 0.5f, 0.5f));
			temprot = glm::rotate(temprot, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			temprot = glm::rotate(temprot, glm::radians(part * (i + 1)), glm::vec3(0.0f, 1.0f, 0.0f));
			temprot = glm::translate(temprot, glm::vec3(0.0f, 0.0f, -1.3f));
			glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(temprot));
			//view = glm::rotate(view, (float)glfwGetTime() * glm::radians(0.001f), glm::vec3(0.1f, 0.1f, 0.1f));
			glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		//Fourth band
		for (int i = 0; i < 8; i++)
		{
			glm::mat4 temprot = glm::mat4(1.0f);
			temprot = glm::scale(temprot, glm::vec3(0.5f, 0.5f, 0.5f));
			temprot = glm::rotate(temprot, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			temprot = glm::rotate(temprot, glm::radians(part * (i + 1)), glm::vec3(0.0f, 1.0f, 0.0f));
			temprot = glm::translate(temprot, glm::vec3(0.0f, 0.0f, -1.3f));
			glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(temprot));
			//view = glm::rotate(view, (float)glfwGetTime() * glm::radians(0.001f), glm::vec3(0.1f, 0.1f, 0.1f));
			glUniformMatrix4fv(glGetUniformLocation(currShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		SDL_GL_SwapWindow(window);
	}

	SDL_Quit();
	return 0;
}

SDL_Window* InitWindowSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		std::cout << "Couldn't initiate SDL, exiting." << std::endl;
		return NULL;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Window* window = NULL;

	window = SDL_CreateWindow("LearnOpenGL - SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL)
	{
		std::cout << "Failed to create window, exiting" << std::endl;
		SDL_Quit();
		return NULL;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);

	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		std::cout << "Failed to initialize GLAD, exiting" << std::endl;
		return NULL;
	}

	return window;
}

void InitBuffers(GLuint& VBO, GLuint& EBO, GLuint& VAO)
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

void InitTexture(GLuint& texture, const char* path, int& width, int& height, int& channels)
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

void processInput(SDL_Window* window)
{
	while (SDL_PollEvent(&KeyEvent))
	{
		std::cout << "KeyEventGen:"<<KeyEvent.type<< std::endl;
		const float cameraSpeed = 0.00000050f * deltaTime;
		if(KeyEvent.type == SDL_KEYDOWN) switch (KeyEvent.key.keysym.sym)
		{
			case SDLK_w:
			cameraPos += cameraSpeed * cameraFront;
			break;

			case SDLK_s:
			cameraPos -= cameraSpeed * cameraFront;
			break;

			case SDLK_a:
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			break;

			case SDLK_d:
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			break;

			case SDLK_ESCAPE:
			kill = true;
		}
	}
}
