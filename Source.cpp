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

#include "Island.h"

//Bullet Includes
#include "btBulletDynamicsCommon.h"
#include <stdio.h>
#include <iostream>




/**************************************************************
********************[  Outside Def's   ]***********************
***************************************************************/
//This method will change the scene between A-E
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

// Window dimensions
const GLuint WIDTH = 2*800, HEIGHT = 2*600;

// Camera
Camera  camera(glm::vec3(-30.0f, 25.0f, 50.0f));
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

///Bullet Defs
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;
std::vector<btRigidBody*> MovingBits; // so that can get at all bits
std::vector<btRigidBody*> StaticBits; // especially during clean up.


/**************************************************************
********************[  Function Outlines  ]***********************
***************************************************************/
GLuint indices[6 * nodes*nodes];
GLuint indicesIndex = 0;
void addIndx(GLuint value) {
	indices[indicesIndex] = value;
	indicesIndex++;
}

btRigidBody* SetSphere(float size, int x, int y, int z) {
	btCollisionShape* fallshape = new btSphereShape(size);

	//This is a combo of both
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 1, 1), btVector3(x, y, z)));

	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallshape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallshape, fallInertia);
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
	fallRigidBody->setLinearVelocity(btVector3(-5, 20, 0));
	fallRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(fallRigidBody);
	return fallRigidBody;
}

btRigidBody* SetCube(float width, int x, int y, int z) {
	btCollisionShape* fallshape = new btBoxShape(btVector3(width, width, width));

	//This is a combo of both techniques of lecutrer and the HelloWorld bullet tutorial
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,1,1),btVector3(x,y,z)));

	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallshape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallshape, fallInertia);
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
	fallRigidBody->setLinearVelocity(btVector3(-5, 20, 0));
	fallRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(fallRigidBody);
	return fallRigidBody;
}

//Returns a vec3 containing the location of the object
glm::vec3 bullet_step(int i) {
	btTransform trans;
	btRigidBody* moveRigidBody;

	moveRigidBody = MovingBits[i];
	dynamicsWorld->stepSimulation(1 / 60.f, 10);
	moveRigidBody->getMotionState()->getWorldTransform(trans);
	return glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
}

//Should be run at the end to cleanup, requires OS assistance to do the rest
void bullet_close() {
	btRigidBody* moveRigidBody;
	moveRigidBody = MovingBits[0];
	dynamicsWorld->removeRigidBody(moveRigidBody);
	delete moveRigidBody->getMotionState();
	delete moveRigidBody;
	delete dynamicsWorld;
	delete solver;
	delete collisionConfiguration;
	delete dispatcher;
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
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

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

	Island island;
	island.instantiate();

	/**************************************************************
	********************[  Bullet Def's   ]***********************
	***************************************************************/
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	collisionConfiguration = new btDefaultCollisionConfiguration();
	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new	btCollisionDispatcher(collisionConfiguration);
	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -10, 0));



	/**************************************************************
	********************[  Bullet Objects   ]***********************
	**************[  Floor, Ceiling, 4x Walls   ]*******************
	***************************************************************/
	/*
	 * Floor
	 */
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	groundRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(groundRigidBody);
	StaticBits.push_back(groundRigidBody);
	/*
	* Set up left
	*/
	btCollisionShape* leftShape = new btStaticPlaneShape(btVector3(1, 0, 0), 1);
	btDefaultMotionState* leftMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-50, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo leftRigidBodyCI(0, leftMotionState, leftShape, btVector3(0, 0, 0));
	btRigidBody* leftRigidBody = new btRigidBody(leftRigidBodyCI);
	leftRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(leftRigidBody);
	StaticBits.push_back(leftRigidBody);

	/*
	* Set up right
	*/
	btCollisionShape* rightShape = new btStaticPlaneShape(btVector3(-1, 0, 0), 1);
	btDefaultMotionState* rightMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(50, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo rightRigidBodyCI(0, rightMotionState, rightShape, btVector3(0, 0, 0));
	btRigidBody* rightRigidBody = new btRigidBody(rightRigidBodyCI);
	rightRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(rightRigidBody);
	StaticBits.push_back(rightRigidBody);
	/*
	* Set up back
	*/
	btCollisionShape* backShape = new btStaticPlaneShape(btVector3(0, 0, -1), 1);
	btDefaultMotionState* backMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 50)));
	btRigidBody::btRigidBodyConstructionInfo backRigidBodyCI(0, backMotionState, backShape, btVector3(0, 0, 0));
	btRigidBody* backRigidBody = new btRigidBody(backRigidBodyCI);
	backRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(backRigidBody);
	StaticBits.push_back(backRigidBody);
	/*
	* Set up front
	*/
	btCollisionShape* frontShape= new btStaticPlaneShape(btVector3(0, 0, 1), 1);
	btDefaultMotionState* frontMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, -50)));
	btRigidBody::btRigidBodyConstructionInfo frontRigidBodyCI(0, frontMotionState, frontShape, btVector3(0, 0, 0));
	btRigidBody* frontRigidBody = new btRigidBody(frontRigidBodyCI);
	frontRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(frontRigidBody);
	StaticBits.push_back(frontRigidBody);

	/*
	* Set up top
	*/
	btCollisionShape* topShape = new btStaticPlaneShape(btVector3(0, -1, 0), 1);
	btDefaultMotionState* topMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 100, 0)));
	btRigidBody::btRigidBodyConstructionInfo topRigidBodyCI(0, topMotionState, topShape, btVector3(0, 0, 0));
	btRigidBody* topRigidBody = new btRigidBody(topRigidBodyCI);
	topRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(topRigidBody);
	StaticBits.push_back(topRigidBody);

	/**************************************************************
	********************[  Bullet Objects   ]***********************
	*******************[  2x Spheres & Cube   ]*********************
	***************************************************************/
	MovingBits.push_back(SetSphere(5., 10, 25, -10));
	MovingBits.push_back(SetSphere(5., 10, 26, -10));
	MovingBits.push_back(SetSphere(5., 10, 10, -10));
	MovingBits.push_back(  SetCube(5., 9, 24, -10));


	/**************************************************************
	********************[  Graphics Objs Setup ]*******************
	***************************************************************/
	//It's 4 because 2+2 * (etc) for both (longitude AND latitude coords) +
	//  Cone: (nodes: half nodes in circle => 2 demi circles = 1 circle)
	GLfloat vertices[ 4 * (nodes * nodes * vertIndxs) + (2*(vertIndxs * nodes * 2 * 3)) ];
	genericSphere.populateArrayWithSphere(vertices, 2.0f, 0.0f, 0.0f, 0.0f);

	//TODO This is to draw the filled sphere
	for (GLuint row = 1; row < nodes; row++) {
		for (GLuint column = 1; column < nodes; column++) {
			GLuint bot_left  = (column * nodes + row);
			GLuint bot_right = (column * nodes + (row+1)%nodes);
			GLuint top_left  = (bot_left + nodes) % (nodes*nodes);
			GLuint top_right = (bot_right + nodes) % (nodes*nodes);

			addIndx(bot_left);
			addIndx(bot_right);
			addIndx(top_right);

			addIndx(bot_left);
			addIndx(top_left);
			addIndx(top_right);
		}
	}


	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertIndxs * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertIndxs * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Texture Coord attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertIndxs * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO


	/**************************************************************
	******************[  Others  Stuff ]****************************
	***************************************************************/
	glm::vec3 colours[4] = {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
	};

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

		glBindVertexArray(VAO);
		GLfloat allPos[99];
		for (GLuint modelNo = 0; modelNo < MovingBits.size(); modelNo++) {
			glm::vec3 col = colours[modelNo];
			glUniform3f(objectColorLoc, col.x, col.y, col.z);

			glm::mat4 model;
			glm::vec3 pos = bullet_step(modelNo);
			allPos[modelNo * 3 + 0] = pos[0];
			allPos[modelNo * 3 + 1] = pos[1];
			allPos[modelNo * 3 + 2] = pos[2];

			model = glm::translate(model, pos);
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawElements(GL_TRIANGLES, indicesIndex, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);

			fancyLightShader.Use();

			island.draw(fancyLightShader);

			
			glm::mat4 model;
			glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
			model = glm::translate(model, pos);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			windmill.draw(fancyLightShader);
			lobutter.draw(textureShader, glm::vec3(0.0f, 3.0f, 1.8f), 0.7, 0);



			//Transparent objects are drawn last
			lbbutter.draw(textureShader, glm::vec3(5.0f, 5.0f, 12.0f), 3, 45);
			sbbutter.draw(textureShader, glm::vec3(2.0f, 1.6f, 2.4), 5, 30);
			sobutter.draw(textureShader, glm::vec3(2.0f, 2.0f, 2.6), 5, 60);




		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	///Clean up
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	windmill.kill();
	cube.kill();
	lobutter.kill();
	lbbutter.kill();
	sobutter.kill();
	sbbutter.kill();
	island.kill();

	//Clean up the bullet stuff
	bullet_close();
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

