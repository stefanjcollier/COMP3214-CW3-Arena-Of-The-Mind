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

class Butterfly {
public:
	Butterfly(glm::vec3 color) {

	}

	/*
	* Create the data
	*/
	void instantiate() {
		//Make the data
		populateData();
		populateIndicies();
		//fill the buffers
		makeBuffers();
	}

	void draw(Shader shader) {

	}

	void kill() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}


private:
	GLuint VBO, VAO, EBO;

	GLuint bufferWidth = 6;
	GLfloat topRad, botRad, height, roofHeight, roofRadius;

	vector<GLfloat>  data;
	vector<GLuint>  indexes;

	glm::vec3 baseColor, roofColor;


	void setColor(Shader shader, glm::vec3 color) {
		GLint objectColorLoc = glGetUniformLocation(shader.Program, "objectColor");
		glUniform3f(objectColorLoc, color.x, color.y, color.z);
	}


	void populateData() {
	
	}

	void populateIndicies() {
	}


	void makeBuffers() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW);

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
		data.push_back(x);
		data.push_back(y);
		data.push_back(z);

		//Add the normal
		data.push_back(x);//dx = x - x0 = x - 0
		data.push_back(y - height / 2);//dy = y - y0 = y - h/2
		data.push_back(z);//dz = z - z0 = z - 0

	}

	void addToIndexes(GLfloat p1, GLfloat p2, GLfloat p3) {
		indexes.push_back(p1);
		indexes.push_back(p2);
		indexes.push_back(p3);
	}
	
};

#endif