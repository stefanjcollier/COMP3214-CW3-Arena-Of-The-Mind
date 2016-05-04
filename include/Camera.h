#pragma once

// Std. Includes
#include <vector>
#include <stdlib.h>     /* abs */
#include <stdio.h>	/* printf  */
using namespace std;

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 10.0f;
const GLfloat SENSITIVTY = 0.1f;
const GLfloat ZOOM = 45.0f;

const GLfloat MAX_SPEED = 8.0f;

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Eular Angles
	GLfloat Yaw;
	GLfloat Pitch;
	// Camera options
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
		this->MovementSpeed = 0.0f;
		this->tourStartPos = glm::vec3(-9.0f, 27.5f, 28.0f);
		this->initTour();
	}
	// Constructor with scalar values
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->Position = glm::vec3(posX, posY, posZ);
		this->WorldUp = glm::vec3(upX, upY, upZ);
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
		this->MovementSpeed = 0.0f;
		this->initTour();
	}

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		//No key presses during tour mode
		if (this->tourMode) return;

		GLfloat deltaYaw = 0.2f;
		GLfloat deltaV = 0.1f;
		if (direction == FORWARD)
			this->MovementSpeed += deltaV;
		if (direction == BACKWARD)
			this->MovementSpeed -= deltaV;

		//Ensure the camera doesn't go to fast
		if (this->MovementSpeed > MAX_SPEED)
			this->MovementSpeed = MAX_SPEED;
		if (this->MovementSpeed < 0)
			this->MovementSpeed = 0;

		if (direction == LEFT){
			this->Yaw -= deltaYaw;
			this->updateCameraVectors();
		}
		if (direction == RIGHT) {
			this->Yaw += deltaYaw;
			this->updateCameraVectors();
		}
	}
	void stopCamera() {
		//No key presses during tour mode
		if (this->tourMode) return;
		this->MovementSpeed = 0.0f;
	}

	void endTour() {
		if (!this->tourMode) return;
		println("TOUR::Ending");
		this->tourMode = false;
		this->Pitch = PITCH;
		this->stopCamera();
		this->updateCameraVectors();
	}
	void startTour() {
		if (this->tourMode) return;

		println("TOUR::Starting");
		printf("TOUR::No of points %d\n", this->pos.size());
		this->tourMode = true;
		this->currentLoc = 0;
		this->updateFocusToNextPoint();//start travelling towards pos[1]
		this->MovementSpeed = 2.0f;

		//Set tour position
		this->Position = this->tourStartPos;
		this->Yaw = YAW;
		this->Pitch = PITCH;
		this->updateCameraVectors();
	}

	void updatePosition(GLfloat deltaTime) {
		if (this->tourMode) {
			this->tourModeUpdate(deltaTime);
		}
		else {
			this->freeModeUpdate(deltaTime);
		}
	}


	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(GLfloat yoffset)
	{
		if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
			this->Zoom -= yoffset;
		if (this->Zoom <= 1.0f)
			this->Zoom = 1.0f;
		if (this->Zoom >= 45.0f)
			this->Zoom = 45.0f;
	}

private:
	GLboolean tourMode;

	GLuint currentLoc;
	GLfloat currentWaitTime;

	glm::vec3 tourStartPos;
	vector<glm::vec3> pos;
	vector<GLfloat> wait;

	void initTour() {
		addPoint(-9.0f, 27.5f, 28.0f, 0.0f);
		addPoint(-9.0f, 27.5f, 14.0f, 0.0f);

	}
	void addPoint(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
		this->pos.push_back(glm::vec3(x, y, z));
		this->wait.push_back(w);
	}


	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		this->Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));
	}

	void freeModeUpdate(GLfloat deltaTime) {
		GLfloat velocity = this->MovementSpeed * deltaTime;
		this->Position += this->Front * velocity;

		//Stefan's check to make sure that they do not go beneath the floor
		GLfloat floor = 26.0f;
		if (this->Position.y < floor) {
			this->Position.y = floor;
		}
	}
	glm::vec3 direction;
	void tourModeUpdate(GLfloat deltaTime) {
		if (reachedDestination(this->currentLoc)) {
			printf("TOUR::Reached Point %d\n", this->currentLoc);
			this->updateFocusToNextPoint();
		}
		else { //Travel to destination
			this->moveTowardsPoint(deltaTime);
		}
	}
	// A point has reached it destination when each element of the camera location in the equation
	// camera pos - dest pos * direction >= 0
	GLboolean reachedDestination(GLuint loc) {
		println("TOUR::REACH::Testing reach");
		glm::vec3 dest = this->pos[loc];
		glm::vec3 reach = this->Position - dest;
		reach.x *= this->direction.x;
		reach.y *= this->direction.y;
		reach.z *= this->direction.z;

		GLboolean reachX, reachY, reachZ;
		reachX = reach.x >= 0;
		reachY = reach.y >= 0;
		reachZ = reach.z >= 0;
		printf("TOUR::Reach:: x:%f,%d  y:%f,%d,   %f,%d\n", reach.x, reachX, reach.y, reachY, reach.z, reachZ);

		GLboolean result = reachX && reachY && reachZ;
		printf("TOUR::REACH::Reached Point:%d\n",result);
		return result;
	}


	void moveTowardsPoint(GLfloat deltaTime) {
		GLfloat velocity = this->MovementSpeed * deltaTime;
		printf("\n\nTOUR::MOVE::Velocity: %f   =(%f * %f)\n", velocity, this->MovementSpeed, deltaTime);
		printf("TOUR::MOVE:: (x,y,z):(%f,%f,%f) ->", this->Position.x, this->Position.y, this->Position.z);
		this->Position += this->direction * deltaTime;
		printf("(%f,%f,%f)\n", this->Position.x, this->Position.y, this->Position.z);
	}




	void updateFocusToNextPoint() {
		println("TOUR::Updating To Next Point");
		this->currentLoc++;
		if (this->currentLoc == this->pos.size()) {
			this->currentLoc = 0;
		}
		printf("TOUR::Current Point %d\n", this->currentLoc);
		glm::vec3 point = this->pos[this->currentLoc];

		//Set the direction for new point
		GLfloat max = this->max(point);
		this->direction = (point - this->Position);
		this->currentWaitTime = 0.0f;
		printf("TOUR:: Direction Now: (%f,%f,%f)\n",direction.x, direction.y, direction.z);
	}
	/* Returns the abs of the maximum value */
	GLfloat max(glm::vec3 list) {
		GLfloat max = 999999.0f;
		for (GLuint i = 0; i < 3; i++) {
			if (abs(list[i]) > max) {
				max = abs(list[i]);
			}
		}
		return abs(max);
	}

	void println(char* text) {
		printf("%s\n", text);
	}


};