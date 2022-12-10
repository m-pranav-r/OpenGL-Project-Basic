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
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

//Kill flag
bool kill = false;

SDL_Window* InitWindowSDL();
void InitBuffers(GLuint& VBO, GLuint& EBO, GLuint& VAO, GLuint& LVAO);
void InitTexture(GLuint& texture, const char* path);
void processInput();

//Vertex array
float vertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};



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



//Camera Vectors
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//Frametime calculation
float deltaTime = 0.0f;
float lastFrame = 0.0f;


//Keyboard Event handle
SDL_Event KeyEvent;


int main(int argc, char* args[])
{
	SDL_GLContext* context = NULL;
	SDL_Window* window = InitWindowSDL();
	SDL_Surface* screenSurface = NULL;
	screenSurface = SDL_GetWindowSurface(window);

	unsigned int VBO, EBO, VAO, LVAO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &LVAO);
	InitBuffers(VBO, EBO, VAO, LVAO);

	Shader lightShader("Source/Shader/lightVertex.vert", "Source/Shader/lightFragment.frag");
	Shader lightSourceShader("Source/Shader/lightVertex.vert", "Source/Shader/lightSourceFragment.frag");

	unsigned int diffuseTexture;
	glGenTextures(1, &diffuseTexture);
	InitTexture(diffuseTexture, "Source/Tex/container2.png");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);

	unsigned int  specularTexture;
	glGenTextures(1, &specularTexture);
	InitTexture(specularTexture, "Source/Tex/container2_specular.png");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularTexture);

	//Transforms Setup
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);

	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	glm::vec4 viewActual = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	//Shader Setup
	lightShader.Use();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	lightShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	lightShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	lightShader.setInt("material.diffuse", 0);
	lightShader.setInt("material.specular", 1);
	lightShader.setFloat("material.shininess", 32.0f);
	lightShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
	lightShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
	lightShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
	lightShader.setFloat("light.constant", 1.0f);
	lightShader.setFloat("light.linear", 0.09f);
	lightShader.setFloat("light.quadratic", 0.032f);
	lightShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
	lightShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));

	//Enable Z-Tests
	glEnable(GL_DEPTH_TEST);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularTexture);
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, emissiveTexture);

	while (!kill)
	{
		float currentFrame = SDL_GetPerformanceCounter() / (float)SDL_GetPerformanceFrequency();
		deltaTime = currentFrame - lastFrame;

		processInput();

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightSourceShader.Use();

		//Transform Bindings
		lightSourceShader.setMat4("model", model);
		lightSourceShader.setMat4("view", view);
		lightSourceShader.setMat4("projection", projection);
		glBindVertexArray(LVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(VAO);
		model = glm::mat4(1.0f);
		lightShader.Use();

		//Transform Bindings
		lightShader.setMat4("model", model);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		
		//Position Bindings
		lightShader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
		lightShader.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
		lightShader.setVec3("light.position", cameraPos.x, cameraPos.y, cameraPos.z);
		lightShader.setVec3("light.direction", cameraFront.x, cameraFront.y, cameraFront.z);


		//Render
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		SDL_GL_SwapWindow(window);
		lastFrame = currentFrame;
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

void InitBuffers(GLuint& VBO, GLuint& EBO, GLuint& VAO, GLuint &LVAO)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//Light Setup
	glBindVertexArray(LVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void InitTexture(GLuint& texture, const char* path)
{
	int width, height, channels;
	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
	if (!data)
	{
		std::cout << "Failed to load texture, exiting." << std::endl;
		return;
	}
	unsigned int format;
	if (channels == 1) format = GL_RED;
	else if (channels == 3) format = GL_RGB;
	else if (channels == 4) format = GL_RGBA;
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}

void processInput()
{
	const float cameraSpeed = 4.5f * deltaTime;

	SDL_PumpEvents();

	const Uint8* keyState = SDL_GetKeyboardState(NULL);

	if (keyState[SDL_SCANCODE_W])
	{
		cameraPos += cameraSpeed * cameraFront;
	}

	if (keyState[SDL_SCANCODE_S])
	{
		cameraPos -= cameraSpeed * cameraFront;
	}

	if (keyState[SDL_SCANCODE_A])
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	if (keyState[SDL_SCANCODE_D])
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	if (keyState[SDL_SCANCODE_E])
	{
		cameraPos += cameraSpeed * cameraUp;
	}

	if (keyState[SDL_SCANCODE_Q])
	{
		cameraPos -= cameraSpeed * cameraUp;
	}

	if (keyState[SDL_SCANCODE_LEFT])
	{
		glm::mat4 rotate = glm::mat4(1.0f);
		glm::vec4 tempvec = glm::vec4(cameraFront, 1.0f);
		rotate = glm::rotate(rotate, glm::radians(cameraSpeed * 10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		tempvec = rotate * tempvec;
		cameraFront = glm::vec3(tempvec);
	}

	if (keyState[SDL_SCANCODE_RIGHT])
	{
		glm::mat4 rotate = glm::mat4(1.0f);
		glm::vec4 tempvec = glm::vec4(cameraFront, 1.0f);
		rotate = glm::rotate(rotate, glm::radians(cameraSpeed * 10.0f), glm::vec3(0.0f,-1.0f, 0.0f));
		tempvec = rotate * tempvec;
		cameraFront = glm::vec3(tempvec);
	}

	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		kill = true;
	}
}
