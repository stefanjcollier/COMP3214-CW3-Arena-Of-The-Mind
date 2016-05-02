#ifndef WINDMILL_H
#define WINDMILL_H

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




const GLuint NODES_IN_CIRCLE = 20;
const GLfloat PI = 3.1415;
const GLfloat TWO_PI = 6.2832;

class Windmill {
public:
	Windmill(GLfloat tr, GLfloat br, GLfloat h, GLfloat rh, glm::vec3 bC, glm::vec3 rC) {
		topRad = tr;
		botRad = br;
		height = h;
		roofHeight = rh;
		baseColor = bC;
		roofColor = rC;
	}

	/* 
	 * Create the data
	 */
	void instantiate() {
		println("WINDMILL::INSTANTIATE");
		//Make the data
		populateData();
		populateIndicies();
		//fill the buffers
		makeBuffers();
	}

	void draw(Shader shader) {
		this->setColor(shader, this->baseColor);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);
	}

	void kill() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
	

private:
	GLuint VBO, VAO, EBO;
	GLuint RoofVBO, RoofVAO, RoofEBO;

	GLuint bufferWidth = 6;
	GLfloat topRad, botRad, height, roofHeight;

	vector<GLfloat> Roofdata, data;
	vector<GLuint> Roofindexes, indexes;

	glm::vec3 baseColor, roofColor;


	void setColor(Shader shader, glm::vec3 color) {
		GLint objectColorLoc = glGetUniformLocation(shader.Program, "objectColor");
		glUniform3f(objectColorLoc, color.x, color.y, color.z);
	}


	void println(GLchar* text) {
		printf(text);
		printf("\n");
	}
	void populateData() {
		GLfloat thetaInc = TWO_PI / NODES_IN_CIRCLE;
		GLfloat theta, x, y, z;

		//----------------[ Bottom Circle ]---------------------------------
		for (GLuint node = 0; node < NODES_IN_CIRCLE; node++) {
			theta = thetaInc * node;
			this->addToData(
				botRad * glm::cos(theta),
				0,
				botRad * glm::sin(theta));
		}

		//----------------[ Top Circle ]---------------------------------
		for (GLuint node = 0; node < NODES_IN_CIRCLE; node++) {
			theta = thetaInc * node;
			this->addToData(
				topRad * glm::cos(theta),
				height,
				topRad * glm::sin(theta));
		}

		//-------------------[ Top and Bottom Circle Face Points ]------------------------------
		this->addToData(0, 0, 0); //Bottom Point
		this->addToData(0, height, 0); //Top Point
	}

	void populateIndicies() {
		GLuint bot_left, bot_right, top_left, top_right;
		
		//---------------[ Shell - no overlap]-----------------------
		for (bot_left = 0; bot_left < NODES_IN_CIRCLE-1; bot_left++) {
			//find points
			bot_right = bot_left + 1;
			top_left = bot_left + NODES_IN_CIRCLE;
			top_right = bot_right + NODES_IN_CIRCLE;

			//Store points
			addToIndexes(
				bot_left, 
				bot_right,
				top_right);

			addToIndexes(
				bot_left,
				top_left,
				top_right);
		}

		//----------------------------
		//Store shell overlap
		bot_left = NODES_IN_CIRCLE-1;
		bot_right = 0;
		top_left = bot_left + NODES_IN_CIRCLE;
		top_right = bot_right + NODES_IN_CIRCLE;

		addToIndexes(
			bot_left,
			bot_right,
			top_right);

		addToIndexes(
			bot_left,
			top_left,
			top_right);
		
		//-----------------------------
		//Store bot face
		GLuint bot_1, bot_2;
		GLuint bot_mid = (NODES_IN_CIRCLE * 2);

		for (bot_1 = 0; bot_1 < NODES_IN_CIRCLE ; bot_1++) {
			bot_2 = bot_1 + 1;
			addToIndexes(
				bot_1,
				bot_2,
				bot_mid);
		}

		addToIndexes(
			NODES_IN_CIRCLE-1,	
			0,
			bot_mid);

		//-----------------------------
		//Store top face
		GLuint top_1, top_2;
		GLuint top_mid = (NODES_IN_CIRCLE * 2)+1 ;
		for (top_1 = NODES_IN_CIRCLE; top_1 < (NODES_IN_CIRCLE*2 - 1); top_1++) {
			top_2 = top_1 + 1;
			addToIndexes(
				top_1,
				top_2,
				top_mid);
		}

		addToIndexes(
			NODES_IN_CIRCLE * 2-1,
			NODES_IN_CIRCLE,
			top_mid);
	}

	void makeBuffers() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0]	, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		//// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);


		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

		glBindVertexArray(0); // Unbind VAO
		println("WINDMILL::BUFFERS::DONE");
	}

	void addToData(GLfloat x, GLfloat y, GLfloat z) {
		data.push_back(x);
		data.push_back(y);
		data.push_back(z);

		//Add the normal
		data.push_back(x);//dx = x - x0 = x - 0
		data.push_back(y-height/2);//dy = y - y0 = y - h/2
		data.push_back(z);//dz = z - z0 = z - 0

	}

	void addToRoofData(GLfloat x, GLfloat y, GLfloat z) {
		data.push_back(x);
		data.push_back(y);
		data.push_back(z);

		//Add the normal
		data.push_back(x);//dx = x - x0 = x - 0
		data.push_back(y - height +(roofHeight/2));//dy = y - y0 = y - h/2
		data.push_back(z);//dz = z - z0 = z - 0

	}


	void addToIndexes(GLfloat p1, GLfloat p2, GLfloat p3) {
		indexes.push_back(p1);
		indexes.push_back(p2);
		indexes.push_back(p3);
	}

	void addToRoofIndexes(GLfloat p1, GLfloat p2, GLfloat p3) {
		indexes.push_back(p1);
		indexes.push_back(p2);
		indexes.push_back(p3);
	}



};

#endif