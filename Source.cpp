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
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

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

//Control of precision of circle curve
const GLuint nodes = 51;


/**************************************************************
********************[  Function Outlines  ]***********************
***************************************************************/
GLuint indices[6 * nodes*nodes];
GLuint indicesIndex = 0;
void addIndx(GLuint value) {
	indices[indicesIndex] = value;
	indicesIndex++;
}


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
	//glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetScrollCallback(window, scroll_callback);

	// GLFW Options
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

	Sphere genericSphere(nodes);
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

	ButterflyFlock flock1(2.0f, 1.8f, glm::vec3(-5.0f, 2.0f, 0.0f), 6);
	flock1.instantiate();

	Island island;
	island.instantiate();


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
		********************[  Drawing Time! ]*******************
		***************************************************************/
		fancyLightShader.Use();


			island.draw(fancyLightShader);

			fancyLightShader.Use();
			glm::mat4 model1;
			glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 butter_offset = glm::vec3(0.0f, 3.0f, 1.8f);
			model1 = glm::translate(model1, pos);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));
			windmill.draw(fancyLightShader);
			lobutter.draw(textureShader, butter_offset, glm::radians(-8.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.7, 30);

			fancyLightShader.Use();
			glm::mat4 model2;
			pos = glm::vec3(13.0f, 0.0f, -12.0f);
			model2 = glm::translate(model2, pos);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
			windmill.draw(fancyLightShader);
			lbbutter.draw(textureShader, pos+butter_offset, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f), 0.9, 0);


			fancyLightShader.Use();
			glm::mat4 model3;
			pos = glm::vec3(-14.0f, 0.0f, -25.0f);
			glm::vec3 adjustment(1.5f, 0.0f, 0.0f);
			model3 = glm::translate(model3, pos);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));
			windmill.draw(fancyLightShader);
			lbbutter.draw(textureShader, pos + butter_offset + adjustment, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.5, 85);


			flock1.draw(textureShader);


			//Transparent objects are drawn last
			sbbutter.draw(textureShader, glm::vec3(2.0f, 1.2f, 2.4), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), 5, 30);
			sobutter.draw(textureShader, glm::vec3(2.5f, 1.0f, 2.6), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), 5, 60);




		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	windmill.kill();
	cube.kill();

	lobutter.kill();
	lbbutter.kill();
	sobutter.kill();
	sbbutter.kill();

	flock1.kill();

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
	if (keys[GLFW_KEY_T])
		camera.startTour();
	if (keys[GLFW_KEY_E])
		camera.endTour();


	camera.updatePosition(deltaTime);
}
/*
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
}*/
/*
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

*/