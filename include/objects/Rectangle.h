#ifndef RECT_H
#define RECT_H



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





class Rectangle {
public:
	Rectangle(GLfloat w, GLfloat h) {
		this->width = w;
		this->height = h;
	}

		GLfloat getHeight() {
		return this->height;
	}
	GLfloat getWidth() {
		return this->width;
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

	void draw(Shader shader, glm::vec3 color) {
		this->repopulateData();

		shader.Use();
		glBindVertexArray(this->VAO);

		this->setColor(shader, color);
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

	GLfloat height, width;
	GLuint nodes = 50;

	vector<GLfloat>  data;
	vector<GLuint>  indexes;

	GLuint texture;


	void setColor(Shader shader, glm::vec3 color) {
		GLint objectColorLoc = glGetUniformLocation(shader.Program, "objectColor");
		glUniform3f(objectColorLoc, color.x, color.y, color.z);
	}

	void repopulateData() {
		vector<GLfloat> newData;
		for (GLfloat n = 0; n < nodes; n++) {
			GLfloat w = width*n;
			GLfloat z_1 = glm::sin(-glfwGetTime() +  w * 3) / 2;
			GLfloat z_2 = glm::sin(-glfwGetTime() + (w + width) * 3) / 2;

			newData = this->addToNewData(newData, w + 0.0f, 0.0f, z_1);		//0  Bot Lef Back
			newData = this->addToNewData(newData, w + 0.0f, height, z_1);	//1  Top Lef Back
			newData = this->addToNewData(newData, w + width, height, z_2);	//2  Top Rig Back
			newData = this->addToNewData(newData, w + width, 0.0f, z_2);		//3  Bot Rig Back
		}

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, newData.size() * sizeof(GLfloat), &newData[0], GL_STREAM_DRAW);
	}
	void populateDataAndIndices() {
		for (GLfloat n = 0; n < nodes-1; n++) {
			GLfloat w = width*n;
			GLfloat z_1 = glm::sin(w*3)/2;
			GLfloat z_2 = glm::sin((w+width)*3)/2;

			this->addToData(w + 0.0f,  0.0f,   z_1);	//0  Bot Lef Back
			this->addToData(w + 0.0f,  height, z_1);	//1  Top Lef Back
			this->addToData(w + width, height, z_2);	//2  Top Rig Back
			this->addToData(w + width, 0.0f,   z_2);	//3  Bot Rig Back

				this->addToIndexes(0 + n, 1 + n, 2 + n);
				this->addToIndexes(0 + n, 3 + n, 2 + n);
		}
	}

	void makeBuffers() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data[0], GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STREAM_DRAW);

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

	vector<GLfloat> addToNewData(vector<GLfloat> newData, GLfloat x, GLfloat y, GLfloat z) {
		//Position
		newData.push_back(x);
		newData.push_back(y);
		newData.push_back(z);
		//Add the normal
		newData.push_back(x);//dx = x - x0 = x - 0
		newData.push_back(y - height / 2);//dy = y - y0 = y - h/2
		newData.push_back(z);//dz = z - z0 = z - 0

		return newData;
	}


	void addToIndexes(GLfloat p1, GLfloat p2, GLfloat p3) {
		indexes.push_back(p1);
		indexes.push_back(p2);
		indexes.push_back(p3);
	}

};

#endif