#pragma once

#ifndef SHADERLOADER_H
#define SHADERLOADER_H

namespace ShaderLoader
{
	/********************************************************************************
	 *                               SHADER VARIABLES                               *
	 ********************************************************************************/

	extern GLuint vsID, fsID, pID, pVBO, VAO, EBO;

	extern GLfloat projectionMat[16], modelViewMat[16];

	/********************************************************************************
	 *                               SHADER FUNCTIONS                               *
	 ********************************************************************************/

	void readShaderFile(const GLchar* shaderPath, std::string& shaderCode);

	void setShaders();

	void initBufferObject(void);
}

#endif
