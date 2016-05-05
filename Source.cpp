/**************************************************************
********************[  Imports   ]*****************************
***************************************************************/
#define GLEW_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)

// GLEW
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Other Externals
#include <stdio.h>
#include <iostream>

// My includes
#include "Shader.h"
#include "Camera.h"
#include "Sphere.h"
#include "World.h"
#include "stb_image.h"

#include "Windmill.h"
#include "Cube.h"

#include "LargeOrangeButterFly.h"
#include "LargeBlueButterFly.h"
#include "SmallOrangeButterFly.h"
#include "SmallBlueButterFly.h"
#include "ButterflyFlock.h"

#include "Island.h"




/**************************************************************
********************[  Outside Def's   ]***********************
***************************************************************/
//This method will change the scene between A-E
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

GLuint loadCubemap(vector<const GLchar*> faces);

void do_movement();
void moveSun(Shader lightShader, Island island, Cube cube);
void handleDayOrNight(GLfloat y, Shader lightShader);
// Window dimensions
const GLuint WIDTH = 2*800, HEIGHT = 2*600;

// Camera
Camera  camera(glm::vec3(-9.0f, 27.5f, 14.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool showPos = true;

// Light attributes
glm::vec3 lightPos(50.0f, 100.0f, 2.0f);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame


/**************************************************************
********************[  The Action Code!   ]********************
***************************************************************/
int main()
{
	/**************************************************************
	********************[  Boring Set up   ]********************
	***************************************************************/
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "This is Stefan's Amazing Coursework", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetScrollCallback(window, scroll_callback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to  true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	// Build and compile our shader program
	Shader fancyLightShader("resources/shaders/lightShader.vs", "resources/shaders/lightShader.frag");
	Shader textureShader("resources/shaders/textureShader.vs", "resources/shaders/textureShader.frag");
	Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.frag");

	GLfloat scale = 7;
	Windmill windmill(12.0f/scale, 16.0f/scale, 14.0f/scale, 50.0f/ scale, 10.0f/ scale, glm::vec3(0.75f,0.75f,0.0f), glm::vec3(0.75f, 0.1f, 0.0f) );
	windmill.instantiate();

	Cube cube(3);
	cube.instantiate();

	LargeOrangeButterfly lobutter;
	LargeBlueButterfly lbbutter;
	SmallOrangeButterfly sobutter;
	SmallBlueButterfly sbbutter;
	lobutter.instantiate();
	lbbutter.instantiate();
	sobutter.instantiate();
	sbbutter.instantiate();

	vector<ButterflyFlock> flocks;
	flocks.push_back(ButterflyFlock::ButterflyFlock(2.0f, 1.8f, glm::vec3(-5.0f, 2.0f, 0.0f), 6));
	flocks.push_back(ButterflyFlock::ButterflyFlock(4.0f, 1.3f, glm::vec3(5.0f, 2.0f, -10.0f), 8));
	flocks.push_back(ButterflyFlock::ButterflyFlock(3.1f, 0.5f, glm::vec3(8.0f, 1.1f, 10.0f), 5));
	flocks.push_back(ButterflyFlock::ButterflyFlock(6.0f, 3.0f, glm::vec3(-14.0f, 5.0f, -25.0f), 5));

	for (GLuint flock = 0; flock < flocks.size(); flock++) {
		flocks[flock].instantiate();
	}


	Island island;
	island.instantiate();
	/**************************************************************
	********************[  Skybox ]*******************
	***************************************************************/
	GLfloat skyboxVertices[] = {
		// Positions          
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
	// Setup skybox VAO
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Cubemap (Skybox)
	vector<const GLchar*> faces;

	faces.push_back("resources/skybox/right.jpg");
	faces.push_back("resources/skybox/left.jpg");
	faces.push_back("resources/skybox/top.jpg");
	faces.push_back("resources/skybox/bottom.jpg");
	faces.push_back("resources/skybox/back.jpg");
	faces.push_back("resources/skybox/front.jpg");

	GLuint cubemapTexture = loadCubemap(faces);

	/**************************************************************
	********************[  GAME LOOP ]*******************
	***************************************************************/	
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) 
		//    and call corresponding response functions
		glfwPollEvents();
		do_movement();

		// Render
		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/**************************************************************
		********************[  Lighting Magic! ]**************************
		***************************************************************/
		// Activate shader
		fancyLightShader.Use();
		GLint objectColorLoc = glGetUniformLocation(fancyLightShader.Program, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(fancyLightShader.Program, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(fancyLightShader.Program, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(fancyLightShader.Program, "viewPos");
		
		//Set the outline to black
		glUniform3f(objectColorLoc, 0.0f, 0.0f, 0.0f);

		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		/**************************************************************
		********************[  Camera Magic! ]**************************
		***************************************************************/
		// Camera/View transformation
		glm::mat4 view;
		view = glm::translate(camera.GetViewMatrix(), glm::vec3(-10.0f, 25.0f, -3.0f));
		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		// Get the uniform locations	
		GLint modelLoc = glGetUniformLocation(fancyLightShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(fancyLightShader.Program, "view");
		GLint projLoc = glGetUniformLocation(fancyLightShader.Program, "projection");
		GLint isDayLoc = glGetUniformLocation(fancyLightShader.Program, "isDay");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		textureShader.Use();
		// Get the uniform locations
		GLint texModelLoc = glGetUniformLocation(textureShader.Program, "model");
		GLint texviewLoc = glGetUniformLocation(textureShader.Program, "view");
		GLint texprojLoc = glGetUniformLocation(textureShader.Program, "projection");
		GLint vtexViewPosLoc = glGetUniformLocation(fancyLightShader.Program, "viewPos");

		// Pass the matrices to the shader
		glUniformMatrix4fv(texviewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(texprojLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(vtexViewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		/**************************************************************
		********************[  Move Sun ]*******************
		***************************************************************/
		fancyLightShader.Use();
		moveSun(fancyLightShader, island, cube);

		/**************************************************************
		********************[  Drawing Time! ]*******************
		***************************************************************/


			island.draw(fancyLightShader,textureShader);

			fancyLightShader.Use();
			glm::mat4 model1;
			glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 butter_offset = glm::vec3(0.0f, 3.0f, 1.9f);
			model1 = glm::translate(model1, pos);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));
			windmill.draw(fancyLightShader);
			lobutter.draw(textureShader, butter_offset, glm::radians(-8.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.7, 30);

			fancyLightShader.Use();
			glm::mat4 model2;
			pos = glm::vec3(13.0f, 0.0f, -11.9f);
			model2 = glm::translate(model2, pos);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
			windmill.draw(fancyLightShader);
			lbbutter.draw(textureShader, pos+butter_offset, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f), 1.2, 0);


			fancyLightShader.Use();
			glm::mat4 model3;
			pos = glm::vec3(-14.0f, 0.0f, -24.6f);
			glm::vec3 adjustment(1.5f, 0.0f, 0.0f);
			model3 = glm::translate(model3, pos);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));
			windmill.draw(fancyLightShader);
			lbbutter.draw(textureShader, pos + butter_offset + adjustment, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.8, 40);

			for (GLuint flock = 0; flock < flocks.size(); flock++) {
				flocks[flock].draw(textureShader);
			}



			//Transparent objects are drawn last
			sbbutter.draw(textureShader, glm::vec3(2.0f, 1.2f, 2.4), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), 5, 30);
			sobutter.draw(textureShader, glm::vec3(2.5f, 1.0f, 2.6), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), 5, 60);



		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.Use();
		glm::mat4 skyview = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(skyview));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(skyboxShader.Program, "texture_diffuse1"), 0);
		glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	windmill.kill();
	cube.kill();

	lobutter.kill();
	lbbutter.kill();
	sobutter.kill();
	sbbutter.kill();

	for (GLuint flock = 0; flock < flocks.size(); flock++) {
		flocks[flock].kill();
	}

	island.kill();


	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
};

//Called whenever the keyboard is used! Will controll our scenes (A-E).
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if ((key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)|| key == GLFW_KEY_Q)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if ((key == GLFW_KEY_N && action == GLFW_PRESS))
		showPos = !showPos;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void do_movement()
{
	// Camera controls
	if (keys[GLFW_KEY_UP])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_DOWN])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_LEFT])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_RIGHT])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	
	if (keys[GLFW_KEY_SPACE])
		camera.stopCamera();
	if (keys[GLFW_KEY_P])
		camera.goToPerfectView();

	if (keys[GLFW_KEY_T])
		camera.startTour();
	if (keys[GLFW_KEY_E])
		camera.endTour();


	camera.updatePosition(deltaTime);
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void moveSun(Shader lightShader, Island island, Cube cube) {
	GLfloat x, y;
	x = -island.getWidth() / 2 + island.getWidth()*glm::sin(glm::radians(glfwGetTime()*4));
	y = -island.getWidth() / 2 + island.getWidth()*glm::sin(glm::radians(glfwGetTime()*4)+90.0f);

	//y = island.getWidth()/2*  glm::sin(glfwGetTime()/2);
	GLint lightPosLoc = glGetUniformLocation(lightShader.Program, "lightPos");
	glUniform3f(lightPosLoc, x, y, lightPos.z);

	GLint modelLoc = glGetUniformLocation(lightShader.Program, "model");
	glm::mat4 modelX, modelY;
	modelX = glm::translate(modelX, glm::vec3(x, 4.0f, 3.0f));
	modelY = glm::translate(modelY, glm::vec3(x, y, 3.0f));
	
	handleDayOrNight(y, lightShader);
}

//Use of the boool reduces the number of computations needed
GLboolean isDay = true;
void handleDayOrNight(GLfloat y, Shader lightShader) {
	//if isDay and now y < 0  => isDay = false
	if (isDay && y < 0) {
		isDay = false;
		GLint isDayLoc = glGetUniformLocation(lightShader.Program, "isDay");
		glUniform1f(isDayLoc, 0.0f);
	}
	//if !isDay and now y > 0 => isDay = true
	else if (!isDay && y > 0) {
		isDay = true;
	}
	else if (isDay & y > 0) {
		GLint isDayLoc = glGetUniformLocation(lightShader.Program, "isDay");
		glUniform1f(isDayLoc, y/6);

	}
}


// Loads a cubemap texture from 6 individual texture faces
// Order should be:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
GLuint loadCubemap(vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = stbi_load(faces[i], &width, &height, 0, STBI_rgb);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		stbi_image_free(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}
