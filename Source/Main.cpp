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

	Shader lightShader("Source/Shader/defVert.vert", "Source/Shader/defFrag.frag");
	Shader skyboxShader("Source/Shader/skyboxVert.vert", "Source/Shader/skyboxFrag.frag");

	//Transforms Setup
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);

	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	glm::vec4 viewActual = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	Model guitarModel("Source/Model/backpack/backpack.obj");

	//Cubemap Setup
	unsigned int cubeMap = loadCubemap("Source/Tex/Skybox/skybox", ".jpg");
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
	unsigned int skyVBO, skyVAO;
	glGenVertexArrays(1, &skyVAO);
	glGenBuffers(1, &skyVBO);
	glBindVertexArray(skyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//Shader Setup
	lightShader.Use();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//Material Values
	{
		lightShader.setInt("material.diffuse", 0);
		lightShader.setInt("material.specular", 1);
		lightShader.setFloat("material.shininess", 32.0f);
	}

	//Directional Light Values
	{
		lightShader.setVec3("dirLight.ambient", 0.6f, 0.6f, 0.6f);
		lightShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
		lightShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
		lightShader.setVec3("dirLight.direction", 0.3f, 0.3f, 0.5f);
	}


	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	//Point Lights Values
	{
		lightShader.setVec3("pointLights[0].ambient", 0.2f, 0.2f, 0.2f);
		lightShader.setVec3("pointLights[0].diffuse", 0.5f, 0.5f, 0.5f);
		lightShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		lightShader.setFloat("pointLights[0].constant", 1.0f);
		lightShader.setFloat("pointLights[0].linear", 0.09f);
		lightShader.setFloat("pointLights[0].quadratic", 0.032f);

		lightShader.setVec3("pointLights[1].ambient", 0.2f, 0.2f, 0.2f);
		lightShader.setVec3("pointLights[1].diffuse", 0.5f, 0.5f, 0.5f);
		lightShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		lightShader.setFloat("pointLights[1].constant", 1.0f);
		lightShader.setFloat("pointLights[1].linear", 0.09f);
		lightShader.setFloat("pointLights[1].quadratic", 0.032f);

		lightShader.setVec3("pointLights[2].ambient", 0.2f, 0.2f, 0.2f);
		lightShader.setVec3("pointLights[2].diffuse", 0.5f, 0.5f, 0.5f);
		lightShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightShader.setVec3("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		lightShader.setFloat("pointLights[2].constant", 1.0f);
		lightShader.setFloat("pointLights[2].linear", 0.09f);
		lightShader.setFloat("pointLights[2].quadratic", 0.032f);

		lightShader.setVec3("pointLights[3].ambient", 0.2f, 0.2f, 0.2f);
		lightShader.setVec3("pointLights[3].diffuse", 0.5f, 0.5f, 0.5f);
		lightShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightShader.setVec3("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		lightShader.setFloat("pointLights[3].constant", 1.0f);
		lightShader.setFloat("pointLights[3].linear", 0.09f);
		lightShader.setFloat("pointLights[3].quadratic", 0.032f);
	}
	

	glm::vec3 modelPositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(5.0f,  0.0f,  0.0f),
		glm::vec3(0.0f,  0.0f,  5.0f),
		glm::vec3(0.0f,  0.0f,  -5.0f),
		glm::vec3(-5.0f,  0.0f,  0.0f),
		glm::vec3(5.0f,  0.0f,  5.0f),
		glm::vec3(-5.0f,  0.0f,  -5.0f),
		glm::vec3(5.0f,  0.0f,  -5.0f),
		glm::vec3(-5.0f,  0.0f,  5.0f)
	};

	//Culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	while (!kill)
	{
		float currentFrame = SDL_GetPerformanceCounter() / (float)SDL_GetPerformanceFrequency();
		deltaTime = currentFrame - lastFrame;

		processInput();

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		//glBindVertexArray(VAO);
		model = glm::mat4(1.0f);
		lightShader.Use();

		//Transform Bindings
		lightShader.setMat4("model", model);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render(s)
		////Main Render
		lightShader.Use();
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		for (unsigned int i = 0; i < 9; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, modelPositions[i]);
			lightShader.setMat4("model", model);
			lightShader.setMat4("view", view);
			lightShader.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
			guitarModel.Draw(lightShader);
		}

		////Skybox Render
		glDepthFunc(GL_LEQUAL);
		skyboxShader.Use();
		//skyboxShader.setInt("skybox", 0);
		glm::mat4 newView = glm::mat4(glm::mat3(view));
		skyboxShader.setMat4("view", newView);
		skyboxShader.setMat4("projection", projection);
		glBindVertexArray(skyVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);

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
