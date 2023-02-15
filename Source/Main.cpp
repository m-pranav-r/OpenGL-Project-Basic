//SDL Includes Setup
#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

//GLAD Include
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//GLM Includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//C++ Lib Includes
#include <iostream>
#include <vector>

//Custom Includes
#include <Engine/ModelHandler.hpp>
#include <Engine/ShaderHandler.hpp>

//Window Sizes
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720


//Kill flag
bool kill = false;

SDL_Window* InitWindowSDL();
unsigned int loadCubemap(std::string firstFace, std::string extension);
void processInput();

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

	Shader geomTestShader("Source/Shader/geomTestVert.vert", "Source/Shader/geomTestFrag.frag", "Source/Shader/geomTestGeom.geom");

	float points[] = {
		-0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,-0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f,-0.5f, 1.0f, 1.0f, 0.0f
	};

	unsigned int geomVBO, geomVAO;
	glGenBuffers(1, &geomVBO);
	glGenVertexArrays(1, &geomVAO);
	glBindVertexArray(geomVAO);
	glBindBuffer(GL_ARRAY_BUFFER, geomVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

	while (!kill)
	{
		float currentFrame = SDL_GetPerformanceCounter() / (float)SDL_GetPerformanceFrequency();
		deltaTime = currentFrame - lastFrame;

		processInput();
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		geomTestShader.Use();
		glBindVertexArray(geomVAO);
		glDrawArrays(GL_POINTS, 0, 4);

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

unsigned int loadCubemap(std::string firstFace, std::string extension)
{
	unsigned int cubemapTextureID;
	glGenTextures(1, &cubemapTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);
	stbi_set_flip_vertically_on_load(false);
	int cubemapWidth, cubemapHeight, cubemapChannels;
	for (unsigned int i = 0; i < 6; i++)
	{
		std::string currentTextureLocation = firstFace + std::to_string(i) + extension;
		unsigned char* textureData = stbi_load(currentTextureLocation.c_str(), &cubemapWidth, &cubemapHeight, &cubemapChannels, 0);
		if (textureData)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, cubemapWidth, cubemapHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
			std::cout << "Loaded skybox texture at location: " << currentTextureLocation << std::endl;
			stbi_image_free(textureData);
		}
		else
		{
			std::cout << "Unable to load texture at location: " << currentTextureLocation << std::endl;
			stbi_image_free(textureData);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	stbi_set_flip_vertically_on_load(true);
	return cubemapTextureID;
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

	if(keyState[SDL_SCANCODE_1]) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (keyState[SDL_SCANCODE_2]) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		kill = true;
	}
}
