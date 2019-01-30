#pragma once

#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>

#include "GL/freeglut.h"

namespace Display {

    /* Rendering modes */
    #define SOLID		1
    #define WIREFRAME	2
    #define POINTS		3

    /* Lighting on/off */
    #define OFF			0
    #define GLOBAL_ON	1
    #define ALL_ON		2

    extern char current_model[100];

    extern std::vector<GLfloat> vertexCoords;
    extern std::vector<GLuint> faceVertices;
    extern std::vector<GLfloat> vertexNormals;

    extern GLfloat maxx, maxy, maxz;
    extern GLfloat minx, miny, minz;
    extern GLfloat max_xy;

    extern GLfloat red, green, blue;

    extern GLfloat light_position[];
    extern GLfloat halfVector[];

    extern unsigned light_on;
    extern bool smooth_shading;

    extern char render_mode;
    extern GLenum primitive_type;

    extern int window_fixed, window_shaders;


    void displayFixed();
    void displayShaders();
    void timer(int t);
    void setPolygonMode();
    void renderAxes();
    void renderNormals();
    void colorUp(GLfloat *color);
    void colorDown(GLfloat *color);
    void updateColorUniform();
    void updateShadingUniform();
    void updateLightOnUniform();
    void updateHalfVector();
    void reinitializeShaders();

}

void main(int argc, char **argv);

#endif