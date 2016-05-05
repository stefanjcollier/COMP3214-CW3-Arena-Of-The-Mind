#ifndef CASE_H
#define CASE_H



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





class Case {
public:
	Case(GLfloat w, GLfloat h, GLfloat b, glm::vec3 color) {
		this->width = w;
		this->height = h;
		this->border = b;
		this->baseColor = color;
	}
	GLfloat getHeight() {
		return this->height + 2 * this->border;
	}
	GLfloat getWidth() {
		return this->width+2*this->border;
	}
	GLfloat getBorder() {
		return this->border;
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
		shader.Use();
		glBindVertexArray(this->VAO);

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

	GLfloat height, width, border;

	vector<GLfloat>  data;
	vector<GLuint>  indexes;

	GLuint texture;
	glm::vec3 baseColor;


	void setColor(Shader shader, glm::vec3 color) {
		GLint objectColorLoc = glGetUniformLocation(shader.Program, "objectColor");
		glUniform3f(objectColorLoc, color.x, color.y, color.z);
	}


	void populateDataAndIndices() {
		//Right Wing
		GLfloat x_off = 0.0;
		GLfloat wb = width + border;
		GLfloat hb = height + border;
		GLfloat b = 2 * border;
		this->addToData(x_off + 0.0f, 0.0f, 0.0f);	//0  Bot Lef Back
		this->addToData(x_off + 0.0f, hb, 0.0f);	//1  Top Lef Back
		this->addToData(x_off + wb, hb, 0.0f);		//2  Top Rig Back
		this->addToData(x_off + wb, 0.0f, 0.0f);	//3  Bot Rig Back

		//Back Face
		this->addToIndexes(0, 1, 2);
		this->addToIndexes(0, 3, 2);

		this->addToData(x_off + 0.0f, 0.0f, b);	//4  Bot Lef Front
		this->addToData(x_off + 0.0f, hb, b);	//5  Top Lef Front

		//Left Face
		this->addToIndexes(0, 1, 5);
		this->addToIndexes(0, 4, 5);

		this->addToData(x_off + wb, hb, b);		//6  Top Rig Front
		this->addToData(x_off + wb, 0.0f, b);	//7  Bot Rig Front

		//Right Face
		this->addToIndexes(2, 3, 7);
		this->addToIndexes(2, 6, 7);

		//Top face
		this->addToIndexes(1, 2, 6);
		this->addToIndexes(1, 5, 6);

		//Bot face
		this->addToIndexes(0, 4, 7);
		this->addToIndexes(0, 3, 7);


		

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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		/// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);


		glBindVertexArray(0); // Unbind VAO
	}


	void addToData(GLfloat x, GLfloat y, GLfloat z) {
		//Position
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