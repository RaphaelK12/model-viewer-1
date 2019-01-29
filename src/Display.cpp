#define GLM_ENABLE_EXPERIMENTAL

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "Camera.hpp"
#include "Constants.hpp"
#include "Display.hpp"
#include "ObjectLoader.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "ShaderLoader.hpp"


namespace Display {

    /********************************************************************************
     *                              DISPLAY VARIABLES                               *
     ********************************************************************************/

    char current_model[100] = "models\\bunny.obj";

    std::vector<GLfloat> vertexCoords;
    std::vector<GLuint> faceVertices;
    std::vector<GLfloat> vertexNormals;

    /* Bounding coordinates of model */
    GLfloat maxx = -10000, maxy = -10000, maxz = -10000;
    GLfloat minx = 10000, miny = 10000, minz = 10000;
    GLfloat max_xy = 0;

    /* Light colors (synced between all light components) */
    GLfloat red = 0.5;
    GLfloat green = 0.5;
    GLfloat blue = 0.5;

    GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    GLfloat halfVector[] = { 0.0f, 0.0f, 0.0f };

    unsigned light_on = ALL_ON;
    bool smooth_shading = true;

    /* Default to solid polygon */
    char render_mode = SOLID;
    GLenum primitive_type = GL_TRIANGLES;

    /* Addresses for each display window */
    int window_fixed, window_shaders;


    /********************************************************************************
     *                              DISPLAY FUNCTIONS                               *
     ********************************************************************************/

     /* 
      * Main display method for the fixed function pipeline rendering.
      *
      * Sets up the projection and modelview matrices based on camera variables.
      * Calls glDrawElements once color, polygon mode, and viewing parameters have
      * been accounted for. Uses glFrustum() and gluLookAt() to define the camera.
      */
    void displayFixed() {
        /* Update the projection matrix */
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glFrustum(-max_xy / 4, max_xy / 4,
            -max_xy / 4, max_xy / 4,
            Camera::near_clip, Camera::far_clip);

        /* Update the modelview matrix */
        glMatrixMode(GL_MODELVIEW);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glLoadIdentity();

        gluLookAt(Camera::camera[0], Camera::camera[1], Camera::camera[2],
            Camera::target[0], Camera::target[1], Camera::target[2],
            Camera::up[0], Camera::up[1], Camera::up[2]);

        glColor3f(red, green, blue);
        setPolygonMode();

        glVertexPointer(3, GL_FLOAT, 3 * sizeof(GL_FLOAT), &vertexCoords[0]);
        glEnableClientState(GL_VERTEX_ARRAY);

        glNormalPointer(GL_FLOAT, 3 * sizeof(GL_FLOAT), &vertexNormals[0]);
        glEnableClientState(GL_NORMAL_ARRAY);

        if (light_on) {
            glEnable(GL_LIGHTING);

            GLfloat global_ambient[4] = { red, green, blue, 1.0 };
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

            if (light_on == ALL_ON) {
                glEnable(GL_LIGHT0);

                /* All components of light change color together */
                GLfloat light_ambient[] = { 0.2f * red, 0.2f * green, 0.2f * blue, 1.0 };
                GLfloat light_diffuse[] = { 0.8f * red, 0.8f * green, 0.8f * blue, 1.0 };
                GLfloat light_specular[] = { 0.5f * red, 0.5f * green, 0.5f * blue, 1.0 };

                glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
                glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
                glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
                glLightfv(GL_LIGHT0, GL_POSITION, light_position);
            }

            /* Material properties */
            glMaterialfv(GL_FRONT, GL_AMBIENT, Constants::mat_am);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, Constants::mat_di);
            glMaterialfv(GL_FRONT, GL_SPECULAR, Constants::mat_sp);
            glMaterialfv(GL_FRONT, GL_SHININESS, Constants::mat_sh);
        } else {
            /* Turn off all lights */
            GLfloat global_ambient[4] = { 0.0, 0.0, 0.0, 1.0 };
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
            glDisable(GL_LIGHT0);
        }

        /* Toggle flat/Gouraud shading */
        if (smooth_shading) {
            glShadeModel(GL_SMOOTH);
        } else {
            glShadeModel(GL_FLAT);
        }

        glDrawElements(primitive_type, faceVertices.size(),
            GL_UNSIGNED_INT, &faceVertices[0]);

        if (Constants::RENDER_AXES) renderAxes();
        if (Constants::RENDER_NORMALS) renderNormals();
        if (Constants::DEBUG_MATRICES) Camera::printProjectionMatrix();
        if (Constants::DEBUG_MATRICES) Camera::printModelViewMatrix();

        glFlush();
        glutSwapBuffers();
    }


    /*
     * Main display method for the custom GLSL shader rendering.
     *
     * Uses the program, shaders, and VAO set up in ShaderLoader to render in the second
     * window using. Calls functions in Camera to calculate the projection and modelview
     * matrices, based on the current parameters used by glFrustum and gluLookAt, as in
     * the displayFixed function for the first window.
     */
    void displayShaders() {
        glUseProgram(ShaderLoader::pID);

        glValidateProgram(ShaderLoader::pID);
        GLint validate = 0;
        glGetProgramiv(ShaderLoader::pID, GL_VALIDATE_STATUS, &validate);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        Camera::calcProjectionMat();
        Camera::calcModelViewMat();

        GLint modelViewMatLocation = glGetUniformLocation(ShaderLoader::pID, "modelViewMatrix");
        GLint projectionMatLocation = glGetUniformLocation(ShaderLoader::pID, "projectionMatrix");

        glUniformMatrix4fv(modelViewMatLocation, 1, GL_FALSE, ShaderLoader::modelViewMat);
        glUniformMatrix4fv(projectionMatLocation, 1, GL_FALSE, ShaderLoader::projectionMat);

        setPolygonMode();
        updateShadingUniform();
        updateLightOnUniform();
        updateHalfVector();

        glBindVertexArray(ShaderLoader::VAO);
        glDrawElements(GL_TRIANGLES, faceVertices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glutSwapBuffers();
    }


    /* 
     * Handles input that should be executed simultaneously (such as translation in 
     * multiple directions), that can't be handled in the keyboard or mouse functions.
     */
    void timer(int t) {

        /* Translation along camera axes */
        if (Keyboard::keyPressed['a']) Camera::translateCamera('u', false);	// left (-u)
        if (Keyboard::keyPressed['d']) Camera::translateCamera('u', true);	// right (+u)
        if (Keyboard::keyPressed['w']) Camera::translateCamera('v', true);	// up (+v)
        if (Keyboard::keyPressed['s']) Camera::translateCamera('v', false);	// down (-v)

        /* Camera tilt */
        if (Mouse::left_press) Camera::rotateCamera('n', Constants::rotate_speed);
        else if (Mouse::right_press) Camera::rotateCamera('n', -Constants::rotate_speed);

        /* Color controls */
        if (Keyboard::keyPressed['r'] && Keyboard::increase) colorUp(&red);
        if (Keyboard::keyPressed['g'] && Keyboard::increase) colorUp(&green);
        if (Keyboard::keyPressed['b'] && Keyboard::increase) colorUp(&blue);

        if (Keyboard::keyPressed['r'] && !Keyboard::increase) colorDown(&red);
        if (Keyboard::keyPressed['g'] && !Keyboard::increase) colorDown(&green);
        if (Keyboard::keyPressed['b'] && !Keyboard::increase) colorDown(&blue);

        /* Clipping controls */
        if (Keyboard::keyPressed['n'] && Keyboard::increase) Camera::increaseNearClip();
        if (Keyboard::keyPressed['f'] && Keyboard::increase) Camera::far_clip += Constants::clip_speed;

        if (Keyboard::keyPressed['n'] && !Keyboard::increase) Camera::near_clip -= Constants::clip_speed;
        if (Keyboard::keyPressed['f'] && !Keyboard::increase) Camera::decreaseFarClip();

        /* Redraw renderings */
        glutSetWindow(window_fixed);
        glutPostRedisplay();

        glutSetWindow(window_shaders);
        glutPostRedisplay();

        glutTimerFunc((unsigned int)(1000.0 / Constants::framerate), timer, 0);
    }


    /*
     * Sets the polygon mode (solid, wireframe, or points). 
     */
    void setPolygonMode() {
        if (render_mode == SOLID) {
            primitive_type = GL_TRIANGLES;
            glPolygonMode(GL_FRONT, GL_FILL);
        } else if (render_mode == WIREFRAME) {
            primitive_type = GL_TRIANGLES;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else if (render_mode == POINTS) {
            primitive_type = GL_POINTS;
            glPolygonMode(GL_FRONT, GL_POINT);
        }

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }


    /*
     * Optional method to render the world/camera axes.
     */
    void renderAxes() {
        glBegin(GL_LINES);

        /* World axes */
        glColor3f(1, 0, 0);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.5f, 0.0f, 0.0f);

        glColor3f(0, 1, 0);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.5f, 0.0f);

        glColor3f(0, 0, 1);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.5f);

        /* Camera axes */
        Camera::updateCameraAxes();

        glColor3f(0.5, 0.0, 0.0);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(Camera::u_axis[0] - Camera::camera[0],
            Camera::u_axis[1] - Camera::camera[1],
            Camera::u_axis[2] - Camera::camera[2]);

        glColor3f(0.0, 0.5, 0.0);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(Camera::v_axis[0] - Camera::camera[0],
            Camera::v_axis[1] - Camera::camera[1],
            Camera::v_axis[2] - Camera::camera[2]);

        glColor3f(0.0, 0.0, 0.5);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(Camera::n_axis[0] - Camera::camera[0],
            Camera::n_axis[1] - Camera::camera[1],
            Camera::n_axis[2] - Camera::camera[2]);

        glEnd();
    }

    /*
     * Draws a line outwards from each vertex, along its vertex normal.
     * Warning: may slow down rendering significantly, depending on vertex count.
     */
    void renderNormals() {
        for (unsigned int i = 0; i < vertexCoords.size(); i += 3) {
            glBegin(GL_LINES);

            glColor3f(1, 0, 0);
            glVertex3f(vertexCoords[i], vertexCoords[i + 1], vertexCoords[i + 2]);
            glVertex3f(vertexCoords[i] + vertexNormals[i] * 0.1f,
                vertexCoords[i + 1] + vertexNormals[i + 1] * 0.1f,
                vertexCoords[i + 2] + vertexNormals[i + 2] * 0.1f);

            glEnd();
        }
    }


    /* 
     * Increments specified color, enforcing 1.0 as the maximum value.
     */
    void colorUp(GLfloat *color) {
        if (*color >= 1.0) *color = 1.0;
        else *color += Constants::color_speed;
        updateColorUniform();
    }


    /* 
     * Decrements specified color, enforcing 0.0 as the minimum value.
     */
    void colorDown(GLfloat *color) {
        if (*color <= 0.0) *color = 0.0;
        else *color -= Constants::color_speed;
        updateColorUniform();
    }


    /* 
     * Updates the currentColor uniform of fragmentshader using the latest values.
     */
    void updateColorUniform() {
        GLint currentColorLocation = glGetUniformLocation(ShaderLoader::pID, "currentColor");
        glUniform4f(currentColorLocation, red, green, blue, 1.0);
    }


    /* 
     * Updates the smoothShading uniform of fragmentshader.
     */
    void updateShadingUniform() {
        GLint smoothShadingLocation = glGetUniformLocation(ShaderLoader::pID, "smoothShading");
        glUniform1i(smoothShadingLocation, Display::smooth_shading);
    }


    /* 
     * Updates the lightOn uniform of fragmentshader.
     */
    void updateLightOnUniform() {
        GLint lightOnLocation = glGetUniformLocation(ShaderLoader::pID, "lightOn");
        glUniform1i(lightOnLocation, light_on);
    }


    /* 
     * Calculates and updates the halfVector uniform of fragmentshader.
     */
    void updateHalfVector() {
        glm::vec3 L(light_position[0], light_position[1], light_position[2]);
        glm::vec3 V(Camera::target[0] - Camera::camera[0],
            Camera::target[1] - Camera::camera[1],
            Camera::target[2] - Camera::camera[2]);

        L = normalize(L);
        V = -normalize(V);

        glm::vec3 H = glm::normalize(V + L);
        halfVector[0] = H[0]; halfVector[1] = H[1]; halfVector[2] = H[2];

        GLint halfVectorLocation = glGetUniformLocation(ShaderLoader::pID, "halfVector");
        glUniform3fv(halfVectorLocation, 1, halfVector);
    }


    /* 
     * Called by ObjectLoader when the model is changed, and the VAO for the shaders
     * needs to be updated.
     */
    void reinitializeShaders() {
        ShaderLoader::initBufferObject();
    }

}


void main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitContextFlags(GLUT_COMPATIBILITY_PROFILE);

    if (!ObjectLoader::loadObject(Display::current_model)) {
        exit(1);
    }


    /********************************************************************************
     *                         Left window (fixed pipeline)                         *
     ********************************************************************************/

    glutInitWindowSize(Constants::window_w, Constants::window_h);
    glutInitWindowPosition(Constants::window1_x, Constants::window1_y);
    Display::window_fixed = glutCreateWindow("Fixed Pipeline");

    Camera::resetCamera();
    glutDisplayFunc(Display::displayFixed);


    /********************************************************************************
     *                         Right window (custom shaders)                        *
     ********************************************************************************/

    glutInitWindowSize(Constants::window_w, Constants::window_h);
    glutInitWindowPosition(Constants::window2_x, Constants::window2_y);
    Display::window_shaders = glutCreateWindow("Custom Shaders");
    glewInit();

    ShaderLoader::initBufferObject();
    ShaderLoader::setShaders();

    glutDisplayFunc(Display::displayShaders);

    /* Assign all keyboard and mouse functions to the second window, including the
     * cursor reset to its center; user input controls rendering in both windows */

    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(Constants::window_w / 2, Constants::window_h / 2);

    glutMouseFunc(Mouse::mouseButton);
    glutPassiveMotionFunc(Mouse::mouseMove);
    glutKeyboardFunc(Keyboard::keyPress);
    glutKeyboardUpFunc(Keyboard::keyRelease);

    /* Begin timer function for changing most rendering variables */
    glutTimerFunc((unsigned int) (1000.0 / Constants::framerate), Display::timer, 0);

    glutMainLoop();
}