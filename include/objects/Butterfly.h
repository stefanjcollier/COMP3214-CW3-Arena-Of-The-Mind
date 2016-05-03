#ifndef BUTTERFLY_H
#define BUTTERFLY_H

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





class Butterfly {
public:
	Butterfly(GLfloat wingHeight, GLfloat wingWidth, glm::vec3 color) {
		this->width = wingWidth;
		this->height = wingHeight;
		this->mycolor = color;
	}

	/*
	* Create the data
	*/
	void instantiate() {
		//Make the data
		populateDataAndIndices();
		//fill the buffers
		makeBuffers();
	}

	void draw(Shader shader) {
		this->setColor(shader, this->mycolor);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);
	}

	void kill() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}


private:
	const GLfloat PI = 3.1415;
	const GLfloat TWO_PI = 6.2832;

	GLuint VBO, VAO, EBO;

	GLfloat height, width;

	vector<GLfloat>  data;
	vector<GLuint>  indexes;

	glm::vec3 mycolor;


	void setColor(Shader shader, glm::vec3 color) {
		GLint objectColorLoc = glGetUniformLocation(shader.Program, "objectColor");
		glUniform3f(objectColorLoc, color.x, color.y, color.z);
	}


	void populateDataAndIndices() {
		//Right Wing
		GLfloat x_off = 0.1;
		this->addToData(x_off + 0.0f, 0.0f, 0.0f,		0.0f, 0.0f);
		this->addToData(x_off + 0.0f, height, 0.0f,		0.0f, 1.0f);
		this->addToData(x_off + width, height, 0.0f,	1.0f, 1.0f);
		this->addToData(x_off + width, 0, 0.0f,			1.0f, 0.0f);

		this->addToIndexes(0, 1, 2);
		this->addToIndexes(0, 3, 2);

		//Left Wing
		x_off = -0.1;
		this->addToData(x_off + 0.0f, 0.0f, 0.0f,		0.0f, 0.0f);
		this->addToData(x_off + 0.0f, height, 0.0f,		0.0f, 1.0f);
		this->addToData(x_off - width, height, 0.0f,	1.0f, 1.0f);
		this->addToData(x_off - width, 0, 0.0f,			1.0f, 0.0f);

		this->addToIndexes(4, 5, 6);
		this->addToIndexes(4, 7, 6);
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

		/// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		/// Texture Coord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);


		glBindVertexArray(0); // Unbind VAO
	}


	void addToData(GLfloat x, GLfloat y, GLfloat z, GLfloat tx, GLfloat ty) {
		//Position
		data.push_back(x);
		data.push_back(y);
		data.push_back(z);
		//Texture
		data.push_back(tx);
		data.push_back(ty);
	}

	void addToIndexes(GLfloat p1, GLfloat p2, GLfloat p3) {
		indexes.push_back(p1);
		indexes.push_back(p2);
		indexes.push_back(p3);
	}
	
};

#endif