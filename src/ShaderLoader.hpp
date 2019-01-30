#pragma once

#ifndef SHADERLOADER_H
#define SHADERLOADER_H

namespace ShaderLoader {

    extern GLuint vsID, fsID, pID, pVBO, VAO, EBO;
    extern GLfloat projectionMat[16], modelViewMat[16];

    void readShaderFile(const GLchar* shaderPath, std::string& shaderCode);
    void setShaders();
    void initBufferObject(void);

}

#endif
