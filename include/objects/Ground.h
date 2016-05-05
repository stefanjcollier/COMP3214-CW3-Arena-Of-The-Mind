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
#include "GrassClump.h"

GrassClump clumps;

class Ground {
public:
	Ground(GLfloat w, GLuint n, glm::vec3 color) {
		this->width = w;
		this->nodes = n;
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
	
	GLfloat width;
	GLuint nodes;
	glm::vec3 baseColor, lastXPos;
	vector<glm::vec3> lastZs;

	void setColor(Shader shader, glm::vec3 color) {
		GLint objectColorLoc = glGetUniformLocation(shader.Program, "objectColor");
		glUniform3f(objectColorLoc, color.x, color.y, color.z);
	}


	void populateData() {
		GLfloat start = -this->width/2;
		GLfloat inc = this->width / float(this->nodes);
		GLfloat x, y, z;
		for (GLuint xnode = 0; xnode < this->nodes; xnode++) {
		for (GLuint znode = 0; znode < this->nodes; znode++) {
				x = start + xnode*inc;
				z = start + znode*inc;
				y = this->getRand(0, 0.75);

				this->addToData(x, y, z);

				//Grass stuff
				///   | 0   1   2   3
				///---|-------------------
				/// 0 | o   o   o   o
				///   |         ^
				/// 1 | o   o < x   o
				///   |
				/// 2 | o   o   o   o
				//0th row and column have no previous cells (as shown in diagram above)
				if(xnode > 0 && znode > 0){
					//Given the last node is on the same point
					clumps.addClump(glm::vec3(x, y, z), this->lastXPos, this->lastZs[xnode]);
				}
				//populate the lastZs
				if (znode == 0) {
					//We must populate it during the first row before we reference it
					this->lastZs.push_back(glm::vec3(x, y, z));
				}
				else {
					this->lastZs[xnode] = glm::vec3(x, y, z);
				}
			}
		}
	}

	void populateIndexes() {
		GLuint bot_left , bot_right, top_left, top_right;
		for (GLuint xnode = 0; xnode < this->nodes-1; xnode++) {
			for (GLuint znode = 0; znode < this->nodes-1; znode++) {
				bot_left = xnode + this->nodes * znode;
				bot_right = bot_left + 1;
				top_left = bot_left + this->nodes;
				top_right = top_left + 1;

				this->addToIndexes(bot_left, bot_right, top_right);
				this->addToIndexes(bot_left, top_left, top_right);
			}
		}
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
	
	GLfloat getRand(GLfloat LO, GLfloat HI) {
		return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	}





};

#endif