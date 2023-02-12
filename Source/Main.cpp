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
	Shader lightSourceShader("Source/Shader/defVert.vert", "Source/Shader/lighting.frag");
	Shader fbTestShader("Source/Shader/fbTestVert.vert", "Source/Shader/fbTestFrag.frag");

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

	Model grassModel("Source/Model/TestFloor.obj");

	//Shader Setup
	lightShader.Use();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//Framebuffer setup
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE))
	{
		std::cout << "Couldn't initiate Framebuffer, exiting." << std::endl;
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return NULL;
	}

	else std::cout << "Framebuffer and attachments successfully initialized." << std::endl;

	//Material Values
	lightShader.setInt("material.diffuse", 0);
	lightShader.setInt("material.specular", 1);
	lightShader.setFloat("material.shininess", 32.0f);

	//Directional Light Values
	lightShader.setVec3("dirLight.ambient", 0.6f, 0.6f, 0.6f);
	lightShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
	lightShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
	lightShader.setVec3("dirLight.direction", 0.3f, 0.3f, 0.5f);


	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	//Point Lights Values
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

	//Spot Light Values
	lightShader.setVec3("spotLight.ambient", 0.2f, 0.2f, 0.2f);
	lightShader.setVec3("spotLight.diffuse", 0.5f, 0.5f, 0.5f);
	lightShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	lightShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	lightShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LESS);

	float fbCoords[]{
		 -1.0f,  1.0f,  0.0f, 1.0f,
		 -1.0f, -1.0f,  0.0f, 0.0f,
		  1.0f, -1.0f,  1.0f, 0.0f,

		 -1.0f,  1.0f,  0.0f, 1.0f,
		  1.0f, -1.0f,  1.0f, 0.0f,
		  1.0f,  1.0f,  1.0f, 1.0f
	};

	//Framebuffer Test VAO
	//unsigned int FBVAO, FBVBO;
	//glGenBuffers(1, &FBVBO);
	//glGenVertexArrays(1, &FBVAO);
	//glBindVertexArray(FBVAO);
	//glBindBuffer(GL_ARRAY_BUFFER, FBVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(fbCoords), fbCoords, GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fbCoords), &fbCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	while (!kill)
	{
		float currentFrame = SDL_GetPerformanceCounter() / (float)SDL_GetPerformanceFrequency();
		deltaTime = currentFrame - lastFrame;

		processInput();

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		for (int i = 0; i < 4; i++)
		{
			glm::mat4 pos = glm::mat4(1.0f);
			pos = glm::translate(pos, pointLightPositions[i]);
			pos = glm::scale(pos, glm::vec3(0.2f));
			lightSourceShader.Use();

			//Transform Bindings
			lightSourceShader.setMat4("model", pos);
			lightSourceShader.setMat4("view", view);
			lightSourceShader.setMat4("projection", projection);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//glBindVertexArray(VAO);
		model = glm::mat4(1.0f);
		lightShader.Use();

		//Transform Bindings
		lightShader.setMat4("model", model);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		
		//Position Bindings
		lightShader.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
		lightShader.setVec3("spotLight.direction", cameraFront.x, cameraFront.y, cameraFront.z);


		//Render
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (unsigned int i = 0; i < 9; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, modelPositions[i]);
			lightShader.setMat4("model", model);
			guitarModel.Draw(lightShader);
		}

		GLint currentVAO;
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		fbTestShader.Use();
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, texture);
		//glUniform1i(glGetUniformLocation(fbTestShader.ID, "screenTexture"), FBO);
		glActiveTexture(GL_TEXTURE0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		glBindVertexArray(currentVAO);

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
