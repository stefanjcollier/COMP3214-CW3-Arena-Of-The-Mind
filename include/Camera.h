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

const GLfloat MAX_SPEED = 20.0f;

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
		this->tourMode = false;
		this->Pitch = PITCH;
		this->stopCamera();
		this->updateCameraVectors();
	}
	void startTour() {
		if (this->tourMode) return;

		this->tourMode = true;
		this->currentLoc = 0;
		this->currentYawChange = 0.0f;
		this->currentWaitTime = 0.0f;
		this->Position = this->tourStartPos;

		this->updateFocusToNextPoint();//start travelling towards pos[1]
		this->MovementSpeed = 5.0f;

		//Set tour position
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


	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= this->MouseSensitivity;
		yoffset *= this->MouseSensitivity;

		this->Yaw += xoffset;
		this->Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if (this->Pitch < -89.0f)
				this->Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		this->updateCameraVectors();
	}

	void goToPerfectView() {
		if (!this->tourMode) return;

		this->Position = this->tourStartPos + glm::vec3(-3.0f,5.0f,-9.0f);
		this->Yaw += 3.0f;
		this->Pitch -= 3.0f;
		this->updateCameraVectors();
	}



private:
	GLboolean tourMode;

	GLuint currentLoc;
	GLfloat currentWaitTime, currentYawChange;

	glm::vec3 tourStartPos;
	vector<glm::vec3> pos;
	vector<GLfloat> wait, yaw_change;

	void initTour() {
		this->tourStartPos = 
			glm::vec3(-9.0f, 27.5f, 28.0f);
		//-----------------------------------
		addPoint(-9.0f, 27.5f, 28.0f,  0.0f, -20.0f);
		addPoint(-9.0f, 27.5f, 14.0f,  0.5f, -10.0f);
		addPoint(-15.0f, 27.5f, 3.0f,  5.0f,  10.0f);
		addPoint(-15.0f, 27.5f, 3.0f,  0.0f,  20.0f);
		addPoint( -4.0f, 27.5f, 3.0f,  0.0f,  0.0f);//Approach Windmill 2
		addPoint( -2.0f, 27.5f, -3.0f,  2.0f,  0.0f);//Observe it

	}
	void addPoint(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat d_yaw) {
		this->pos.push_back(glm::vec3(x, y, z));
		this->wait.push_back(w);
		this->yaw_change.push_back(d_yaw);
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
			//Once we have reached the point,
			//Then rotate the screen to point the right way
			if (abs(this->yaw_change[this->currentLoc]) > abs(this->currentYawChange)) {
				this->changeYaw(deltaTime);
				return;
			}

			//Once we have reached the point and rotated
			//Have we waited the allocated time yet?
			if (this->currentWaitTime >= this->wait[this->currentLoc]) {
				this->updateFocusToNextPoint();
			}
			else {
				//If not then wait
				this->currentWaitTime += deltaTime;
			}
		}
		else { //Travel to destination
			this->moveTowardsPoint(deltaTime);
		}
	}
	// A point has reached it destination when each element of the camera location in the equation
	// camera pos - dest pos * direction >= 0
	GLboolean reachedDestination(GLuint loc) {
		glm::vec3 dest = this->pos[loc];
		glm::vec3 reach = this->Position - dest;
		reach.x *= this->direction.x;
		reach.y *= this->direction.y;
		reach.z *= this->direction.z;

		GLboolean reachX, reachY, reachZ;
		reachX = reach.x >= 0;
		reachY = reach.y >= 0;
		reachZ = reach.z >= 0;

		GLboolean result = reachX && reachY && reachZ;
		return result;
	}

	void changeYaw(GLfloat deltaTime) {
		GLfloat polarity = this->yaw_change[this->currentLoc] / abs(this->yaw_change[this->currentLoc]);
		GLfloat rotateSpeed = 6.0f * polarity;
		GLfloat deltaYaw = rotateSpeed * deltaTime;  //this->yaw_change[this->currentLoc]/23;
		this->Yaw += deltaYaw;
		this->currentYawChange += deltaYaw;
		this->updateCameraVectors();
	}


	void moveTowardsPoint(GLfloat deltaTime) {
		GLfloat velocity = this->MovementSpeed * deltaTime;
		this->Position += this->direction * velocity;
	}




	void updateFocusToNextPoint() {
		this->currentLoc++;
		if (this->currentLoc == this->pos.size()) {
			this->currentLoc = 0;
		}
		glm::vec3 point = this->pos[this->currentLoc];

		//Set the direction for new point
		glm::vec3 delta = point - this->Position;
		this->direction = normalise(delta);
		this->currentWaitTime = 0.0f;
		this->currentYawChange = 0.0f;
	}


	glm::vec3 normalise(glm::vec3 p) {
		GLfloat max = this->max(p);

		p.x /= max;
		p.y /= max;
		p.z /= max;

		return p;
	}

	GLfloat max(glm::vec3 p) {
		GLfloat head = abs(p.x);
		if (abs(p.y) > head) {
			head = abs(p.y);
		}
		if (abs(p.z) > head) {
			head = abs(p.z);
		}
		if (head == 0.0f) {
			return 1;
		}
		return head;
	}


};