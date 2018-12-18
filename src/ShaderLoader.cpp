#include "GL/glew.h"
#include "GL/freeglut.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream> 
#include <sstream>

#include "Display.hpp"
#include "Camera.hpp"
#include "Constants.hpp"

namespace ShaderLoader
{
	/********************************************************************************
	 *                               SHADER VARIABLES                               *
	 ********************************************************************************/

	GLuint vsID, fsID, pID, pVBO, nVBO, VAO, EBO;

	GLfloat projectionMat[16], modelViewMat[16];

	/********************************************************************************
	 *                               SHADER FUNCTIONS                               *
	 ********************************************************************************/

	/* Reads a shader file and stores it as a string at &shaderCode; acknowledgement:
	 * heavily based on the code supplied on the course website as "HelloGLSL" */
	void readShaderFile(const GLchar* shaderFilePath, std::string& shaderString)
	{
		std::ifstream shaderFile;

		shaderFile.exceptions(std::ifstream::badbit);
		try
		{
			shaderFile.open(shaderFilePath);
			std::stringstream shaderStream;
			shaderStream << shaderFile.rdbuf();
			shaderFile.close();

			shaderString = shaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
	}

	/* Creates a program with the given vertex and fragment shaders; calls
	 * readShaderFile() to get the shaders as a string, then compiles and attaches
	 * them to a new program; specifies locations of uniform inputs; acknowledgement:
	 * heavily based on the code supplied on the course website as "HelloGLSL"
	 */
	void setShaders()
	{
		char *vs = NULL, *fs = NULL;

		std::string vertShaderString;
		std::string fragShaderString;

		vsID = glCreateShader(GL_VERTEX_SHADER);
		fsID = glCreateShader(GL_FRAGMENT_SHADER);

		readShaderFile("vertexshader.txt", vertShaderString);
		readShaderFile("fragmentshader.txt", fragShaderString);

		const GLchar * pVertShaderSource = vertShaderString.c_str();
		const GLchar * pFragShaderSource = fragShaderString.c_str();

		glShaderSource(vsID, 1, &pVertShaderSource, NULL);
		glShaderSource(fsID, 1, &pFragShaderSource, NULL);

		glCompileShader(vsID);
		glCompileShader(fsID);

		pID = glCreateProgram();
		glAttachShader(pID, vsID);
		glAttachShader(pID, fsID);

		glLinkProgram(pID);
		glUseProgram(pID);

		GLint modelViewMatLocation = glGetUniformLocation(pID, "modelViewMatrix");
		GLint projectionMatLocation = glGetUniformLocation(pID, "projectionMatrix");

		GLint currentColorLocation = glGetUniformLocation(pID, "currentColor");
		GLint smoothShadingLocation = glGetUniformLocation(pID, "smoothShading");
		GLint lightDirectionLocation = glGetUniformLocation(pID, "lightDirection");
		GLint lightOnLocation = glGetUniformLocation(pID, "lightOn");
		GLint halfVectorLocation = glGetUniformLocation(pID, "halfVector");

		glUniformMatrix4fv(modelViewMatLocation, 1, GL_FALSE, modelViewMat);
		glUniformMatrix4fv(projectionMatLocation, 1, GL_FALSE, projectionMat);
		glUniform4f(currentColorLocation, Display::red, Display::green, Display::blue, 1.0);
		glUniform1i(smoothShadingLocation, Display::smooth_shading);
		glUniform3fv(lightDirectionLocation, 1, Display::light_position);
		glUniform1i(lightOnLocation, Display::light_on);
		glUniform3fv(halfVectorLocation, 1, Display::halfVector);

		glDeleteShader(vsID);
		glDeleteShader(fsID);
	}

	/* Initializes the VAO for the shader display function (Display::displayShaders()) 
	 * to use; uses the vertex data in Display::vertexCoords and Display::faceVertices,
	 * which is read by ObjectLoader::loadObject(); acknowledgement: heavily based on 
	 * the code supplied on the course website as "HelloGLSL"
	 */
	void initBufferObject(void)
	{
		glGenBuffers(1, &pVBO);
		glGenBuffers(1, &nVBO);
		glGenBuffers(1, &EBO);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// buffer vertex position
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, pVBO);
		glBufferData(GL_ARRAY_BUFFER, (Display::vertexCoords).size() * sizeof(GLfloat), 
			&(Display::vertexCoords[0]), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		// buffer vertex normals
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, nVBO);
		glBufferData(GL_ARRAY_BUFFER, (Display::vertexNormals).size() * sizeof(GLfloat), 
			&(Display::vertexNormals[0]), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (Display::faceVertices).size() * sizeof(GLuint), 
			&(Display::faceVertices[0]), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// calculate the initial projection and modelview matrices
		Camera::calcProjectionMat();
		Camera::calcModelViewMat();
	}
}