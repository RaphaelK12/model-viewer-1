#pragma once

#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H

#include <list>

namespace ObjectLoader {

    extern std::vector<GLfloat> faceNormals;
    extern std::vector<GLfloat> faceAreas;
    extern std::vector< std::list< GLuint > > memberFaces;
    extern const bool debug;

    bool loadObject(char *filepath);
    void changeModel(char *filepath);
    void processFaces();
    void calculateVertexNormals();
    void printMemberFaces();

}

#endif