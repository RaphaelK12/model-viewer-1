#define GLM_ENABLE_EXPERIMENTAL

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "Camera.hpp"
#include "Constants.hpp"
#include "Display.hpp"
#include "ShaderLoader.hpp"


/*
 * Maintains all state for the camera, and logic for transforming it. 
 */
namespace Camera {

    const bool DEBUG = false;

    /********************************************************************************
     *                               CAMERA VARIABLES                               *
     ********************************************************************************/

     /* Camera attributes for gluLookAt() */
    glm::vec3 camera;
    glm::vec3 target;
    glm::vec3 up;

    glm::vec3 n_axis;
    glm::vec3 u_axis;
    glm::vec3 v_axis;

    GLfloat near_clip, far_clip;


    /********************************************************************************
     *                               CAMERA FUNCTIONS                               *
     ********************************************************************************/

     /*
      * Resets and initializes the camera to a view with the model centered in the
      * window, with the target point for gluLookAt() at the center of the model,
      * and with the near and far clipping values set so the entire model is rendered.
      */
    void resetCamera() {

        /* Align camera eye with x and y midpoints of the model, and some ways
         * away from the largest z coordinate (based on model size) */
        camera[0] = (Display::maxx + Display::minx) / 2;
        camera[1] = (Display::maxy + Display::miny) / 2;
        camera[2] = abs(Display::maxz - Display::minz) * 2 + Display::maxz;

        /* Point camera towards 3D midpoint of the model */
        target[0] = camera[0];
        target[1] = camera[1];
        target[2] = (Display::maxz + Display::minz) / 2;

        /* Initialize up vector to global y axis */
        up[0] = 0.0f;
        up[1] = 1.0f;
        up[2] = 0.0f;

        /* Halfway between camera and closest model coordinate */
        near_clip = (camera[2] - Display::maxz) / 2;

        /* Include the whole model, plus some extra space */
        far_clip = (camera[2] - Display::minz) * 1.5f;

        if (DEBUG) {
            glm::vec3 temp(camera - target);
            printf("Camera: [%.4f %.4f %.4f]\n", camera[0], camera[1], camera[2]);
            printf("Target: [%.4f %.4f %.4f]\n", target[0], target[1], target[2]);
            printf("View Length: %.4f\n\n", glm::length(temp));
            printf("camera[2]: %f | maxz: %f | near_clip: %f | far_clip: %f\n",
                camera[2], Display::maxz, near_clip, far_clip);
        }
    }

    /*
     * Calculate the current camera axes (u, v, and n) based on gluLookAt()
     * parameters (camera position, target position, and up axis).
     */
    void updateCameraAxes() {
        n_axis[0] = camera[0] - target[0];
        n_axis[1] = camera[1] - target[1];
        n_axis[2] = camera[2] - target[2];
        n_axis = glm::normalize(n_axis);

        u_axis = glm::cross(up, n_axis);
        v_axis = glm::cross(n_axis, u_axis);
    }


    /*
     * Translate the camera along its u, v, or n axis, updating both the
     * camera and target coordinates by the same amount along the given axis.
     */
    void translateCamera(char axis, bool pos) {
        updateCameraAxes();

        static glm::vec3 offset;
        if (axis == 'u') {
            offset = u_axis * Constants::trans_speed;
        } else if (axis == 'v') {
            offset = v_axis * Constants::trans_speed;
        } else if (axis == 'n') {
            offset = n_axis * Constants::trans_speed;
        }

        if (pos) {
            camera += offset;
            target += offset;
        } else {
            camera -= offset;
            target -= offset;
        }
    }


    /*
     * Rotates the camera by a given angle around its u, v, or n axis.
     */
    void rotateCamera(char axis, float angle) {
        updateCameraAxes();

        static glm::vec3 line_of_sight;
        line_of_sight[0] = target[0] - camera[0];
        line_of_sight[1] = target[1] - camera[1];
        line_of_sight[2] = target[2] - camera[2];

        GLfloat length = glm::length(line_of_sight);

        if (axis == 'u') { // look up/down
            line_of_sight = glm::rotate(line_of_sight, angle, u_axis);
            line_of_sight *= length / glm::length(line_of_sight);
            target = line_of_sight + camera;
            up = glm::rotate(up, angle, u_axis);
            up = glm::normalize(up);
        } else if (axis == 'v') { // look right/left
            line_of_sight = glm::rotate(line_of_sight, angle, v_axis);
            line_of_sight *= length / glm::length(line_of_sight);
            target = line_of_sight + camera;
        } else if (axis == 'n') { // tilt CW/CCW
            up = glm::rotate(up, angle, n_axis);
            up = glm::normalize(up);
        }
    }


    /* 
     * Calculates the modelview matrix for the vertex shader. 
     */
    void calcModelViewMat() {
        updateCameraAxes();

        glm::vec3 origin(camera[0], camera[1], camera[2]);
        origin = (float)(-1) * origin;
        glm::vec3 translate;
        translate[0] = dot(origin, u_axis);
        translate[1] = dot(origin, v_axis);
        translate[2] = dot(origin, n_axis);

        GLfloat *p = ShaderLoader::modelViewMat;
        *p++ = u_axis[0];       *p++ = v_axis[0];       *p++ = n_axis[0];       *p++ = 0.0f;
        *p++ = u_axis[1];       *p++ = v_axis[1];       *p++ = n_axis[1];       *p++ = 0.0f;
        *p++ = u_axis[2];       *p++ = v_axis[2];       *p++ = n_axis[2];       *p++ = 0.0f;
        *p++ = translate[0];    *p++ = translate[1];    *p++ = translate[2];    *p++ = 1.0f;

        if (Constants::DEBUG_MATRICES) {
            printf("Calculated ModelView Matrix:\n");
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    printf("%.3f ", ShaderLoader::modelViewMat[i + j * 4]);
                }
                printf("\n");
            }
            printf("\n");
        }
    }


    /* 
     * Calculates the projection matrix for the vertex shader. 
     */
    void calcProjectionMat() {
        GLfloat l = -Display::max_xy / 4;
        GLfloat r =  Display::max_xy / 4;
        GLfloat b = -Display::max_xy / 4;
        GLfloat t =  Display::max_xy / 4;
        GLfloat n = near_clip;
        GLfloat f = far_clip;

        GLfloat *q = ShaderLoader::projectionMat;
        *q++ = 2 * n / (r - l);		*q++ = 0.0f;				*q++ = 0.0f;					*q++ = 0.0f;
        *q++ = 0.0f;				*q++ = 2 * n / (t - b);		*q++ = 0.0f;					*q++ = 0.0f;
        *q++ = (r + l) / (r - l);	*q++ = (t + b) / (t - b);	*q++ = -(f + n) / (f - n);		*q++ = -1.0f;
        *q++ = 0.0f;				*q++ = 0.0f;				*q++ = -2 * f * n / (f - n);	*q++ = 0.0f;

        if (Constants::DEBUG_MATRICES) {
            printf("Calculated Projection Matrix:\n");
            for (int i = 0; i < 16; i++) {
                printf("%.3f ", ShaderLoader::projectionMat[i]);
                if ((i + 1) % 4 == 0) printf("\n");
            }
            printf("\n");
        }
    }


    /* 
     * Increments near clip, enforcing (near_clip <= far_clip). 
     */
    void increaseNearClip() {
        if (near_clip + Constants::clip_speed >= far_clip) {
            near_clip = far_clip;
        } else {
            near_clip += Constants::clip_speed;
        }
    }


    /* 
     * Decrements far clip, enforcing (far_clip >= near_clip). 
     */
    void decreaseFarClip() {
        if (far_clip - Constants::clip_speed <= near_clip) {
            far_clip = near_clip;
        } else {
            far_clip -= Constants::clip_speed;
        }
    }


    /*
    * Prints the current fixed function pipeline modelview matrix.
    */
    void printModelViewMatrix() {
        GLfloat m[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, m);

        printf("OpenGL ModelView Matrix:\n");
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                printf("%.3f ", m[i + j * 4]);
            }
            printf("\n");
        }
        printf("\n");
    }


    /*
    * Prints the current fixed function pipeline projection matrix.
    */
    void printProjectionMatrix() {
        GLfloat m[16];
        glGetFloatv(GL_PROJECTION_MATRIX, m);

        printf("OpenGL Projection Matrix:\n");
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                printf("%.3f ", m[i + j * 4]);
            }
            printf("\n");
        }
        printf("\n");
    }

}