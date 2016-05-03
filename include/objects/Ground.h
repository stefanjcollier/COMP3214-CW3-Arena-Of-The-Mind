#ifndef GROUND_H
#define GROUND_H

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLFW
#include <GLFW/glfw3.h>
#include <GL/glew.h>

//Externals
#include <vector>
using namespace std;
#include <stdio.h>

//My includes
#include "Shader.h"

class Ground {
public:
	Ground(GLfloat r, glm::vec3 color) {
		this->radius = r;
		this->baseColor = color;
	}

	/*
	* Create the data
	*/
	void instantiate() {
		//Make the data
		populateData();
		populateIndexes();

		//fill the buffers
		makeBuffers();

	}

	void draw(Shader shader) {
		this->setColor(shader, this->baseColor);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, rimIndexes.size(), GL_UNSIGNED_INT, 0);
	}

	void kill() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}


private:
	const GLuint NODES_IN_CIRCLE = 20;
	const GLfloat PI = 3.1415;
	const GLfloat TWO_PI = 6.2832;

	GLuint VBO, VAO, EBO;
	vector<GLfloat> rimData;
	vector<GLuint>  rimIndexes;
	
	GLfloat radius;
	glm::vec3 baseColor;
	
	void setColor(Shader shader, glm::vec3 color) {
		GLint objectColorLoc = glGetUniformLocation(shader.Program, "objectColor");
		glUniform3f(objectColorLoc, color.x, color.y, color.z);
	}


	void populateData() {
		GLfloat thetaInc = TWO_PI / NODES_IN_CIRCLE;
		GLfloat theta;

		//----------------[ Bottom Circle ]---------------------------------
		for (GLuint node = 0; node < NODES_IN_CIRCLE; node++) {
			theta = thetaInc * node;
			this->addToData(
				this->radius * glm::cos(theta),
				0,
				this->radius * glm::sin(theta));
		}
		this->addToData(0, 0, 0); //Mid Face point

	}

	void populateIndexes() {
		GLuint bot_1, bot_2;
		GLuint bot_mid = NODES_IN_CIRCLE;

		for (bot_1 = 0; bot_1 < NODES_IN_CIRCLE; bot_1++) {
			bot_2 = bot_1 + 1;
			addToIndexes(
				bot_1,
				bot_2,
				bot_mid);
		}
		//The overlap of the circle
		addToIndexes(
			NODES_IN_CIRCLE - 1,
			0,
			bot_mid);

	}

	void makeBuffers() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, rimData.size() * sizeof(GLfloat), &rimData[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, rimIndexes.size() * sizeof(GLuint), &rimIndexes[0], GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		//// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);


		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

		glBindVertexArray(0); // Unbind VAO
	}

	void addToData(GLfloat x, GLfloat y, GLfloat z) {
		rimData.push_back(x);
		rimData.push_back(y);
		rimData.push_back(z);

		//Add the normal
		rimData.push_back(x);//dx = x - x0 = x - 0
		rimData.push_back(y);//dy = y - y0 = y - 0
		rimData.push_back(z);//dz = z - z0 = z - 0
	}

	void addToIndexes(GLfloat p1, GLfloat p2, GLfloat p3) {
		rimIndexes.push_back(p1);
		rimIndexes.push_back(p2);
		rimIndexes.push_back(p3);
	}


};

#endif