#ifndef CLOUD
#define CLOUD



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





class Cloud {
public:
	Cloud(GLfloat w, GLfloat h) {
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

		prepareTexture();
	}

	void draw(Shader textureshader) {
		glBindVertexArray(this->VAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glUniform1i(glGetUniformLocation(textureshader.Program, "ourTexture"), 0);

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

	vector<GLfloat>  data;
	vector<GLuint>  indexes;

	GLuint texture;



	void setColor(Shader shader, glm::vec3 color) {
		GLint objectColorLoc = glGetUniformLocation(shader.Program, "objectColor");
		glUniform3f(objectColorLoc, color.x, color.y, color.z);
	}


	void populateDataAndIndices() {
		//Right Wing
		GLfloat x_off = 0.0;
		this->addToData(x_off + 0.0f, 0.0f, 0.0f, 0.999f, 0.0f);
		this->addToData(x_off + 0.0f, height, 0.0f, 0.999f, 0.999f);
		this->addToData(x_off + width, height, 0.0f, 0.0f, 0.999f);
		this->addToData(x_off + width, 0, 0.0f, 0.0f, 0.0f);

		this->addToIndexes(0, 1, 2);
		this->addToIndexes(0, 3, 2);
	}

	void prepareTexture() {
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, this->texture); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

													 // Set our texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Load, create texture and generate mipmaps
		int width, height, comp;
		unsigned char* image = stbi_load("resources/textures/cloud.png", &width, &height, &comp, STBI_rgb_alpha);
		if (image == nullptr)
			throw(std::string("Failed to load texture"));

		if (comp == 3) {
			printf("TEXTURE::1::RGB\n");
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		}
		else if (comp == 4) {
			printf("TEXTURE::1::RGBA\n");
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
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
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
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