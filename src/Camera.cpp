#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "Display.hpp"
#include "Constants.hpp"
#include "ShaderLoader.hpp"

namespace Camera
{
	const bool DEBUG(false);

	/********************************************************************************
	 *                               CAMERA VARIABLES                               *
	 ********************************************************************************/

	// camera attributes (for gluLookAt)
	glm::vec3 camera;
	glm::vec3 target;
	glm::vec3 up;

	GLdouble near_clip, far_clip;

	/********************************************************************************
	 *                               CAMERA FUNCTIONS                               *
	 ********************************************************************************/

	/* Resets (and initializes) the camera to a view where the model is centered
	 * in the window, with the target point for gluLookAt as the center of the
	 * model, and with the near and far clipping values set so the entire
	 * model is rendered.
	 */
	void resetCamera()
	{
		/* align camera eye with x and y midpoints of the model, and some ways
		 * away from the largest z coordinate (based on model size) */
		camera[0] = (Display::maxx + Display::minx) / 2;
		camera[1] = (Display::maxy + Display::miny) / 2;
		camera[2] = abs(Display::maxz - Display::minz) * 2 + Display::maxz;

		// point camera towards the 3D midpoint of the model
		target[0] = camera[0];
		target[1] = camera[1];
		target[2] = (Display::maxz + Display::minz) / 2;

		// initialize up vector to global y axis
		up[0] = 0.0f;
		up[1] = 1.0f;
		up[2] = 0.0f;

		near_clip = (camera[2] - Display::maxz) / 2; // halfway between camera and closest model coordinate
		far_clip = (camera[2] - Display::minz) * 1.5; // include the whole model, plus a little extra space

		if (DEBUG)
		{
			glm::vec3 temp(camera - target);
			printf("Camera: [%.4f %.4f %.4f]\n", camera[0], camera[1], camera[2]);
			printf("Target: [%.4f %.4f %.4f]\n", target[0], target[1], target[2]);
			printf("View Length: %.4f\n\n", glm::length(temp));

			printf("camera[2]: %f | maxz: %f | near_clip: %f | far_clip: %f\n",
				camera[2], Display::maxz, near_clip, far_clip);
		}
	}

	/* Translate the camera along its axes, by updating both the camera and target
	 * coordinates by the same amount along the argument axis
	 */
	void translateCamera(char axis, bool pos)
	{
		glm::vec3 n_axis(camera[0] - target[0], camera[1] - target[1], camera[2] - target[2]);
		n_axis = glm::normalize(n_axis);

		glm::vec3 u_axis(0.0f, 0.0f, 0.0f);
		u_axis = glm::cross(up, n_axis);

		glm::vec3 v_axis(0.0f, 0.0f, 0.0f);
		v_axis = glm::cross(n_axis, u_axis);

		glm::vec3 offset(0.0f, 0.0f, 0.0f);

		if (axis == 'u')	  offset = u_axis * Constants::trans_speed;
		else if (axis == 'v') offset = v_axis * Constants::trans_speed;
		else if (axis == 'n') offset = n_axis * Constants::trans_speed;

		if (pos)
		{
			camera += offset;
			target += offset;
		}
		else
		{
			camera -= offset;
			target -= offset;
		}
	}

	/* Given an axis to rotate around ('u', 'v', 'n'), and an angle, performs the
	 * rotation around that camera axis; called by mouse functions for button drags
	 * and clicks
	 */
	void rotateCamera(char axis, float angle)
	{
		// calculate camera axes based on gluLookAt parameters
		glm::vec3 n_axis(camera[0] - target[0], 
			camera[1] - target[1], 
			camera[2] - target[2]);
		n_axis = glm::normalize(n_axis);

		glm::vec3 u_axis(0.0f, 0.0f, 0.0f);
		u_axis = glm::cross(up, n_axis);

		glm::vec3 v_axis(0.0f, 0.0f, 0.0f);
		v_axis = glm::cross(n_axis, u_axis);

		glm::vec3 line_of_sight(target[0] - camera[0],
			target[1] - camera[1],
			target[2] - camera[2]);

		GLfloat length = glm::length(line_of_sight);

		if (axis == 'u') // look up/down
		{
			// rotate and scale back to proper length
			line_of_sight = glm::rotate(line_of_sight, angle, u_axis);
			line_of_sight *= length / glm::length(line_of_sight);

			target = line_of_sight + camera;

			up = glm::rotate(up, angle, u_axis);
			up = glm::normalize(up);
		}
		else if (axis == 'v') // look right/left
		{
			// rotate and scale back to proper length
			line_of_sight = glm::rotate(line_of_sight, angle, v_axis);
			line_of_sight *= length / glm::length(line_of_sight);

			target = line_of_sight + camera;
		}
		else if (axis == 'n') // tilt CW/CCW
		{
			up = glm::rotate(up, angle, n_axis);
			up = glm::normalize(up);
		}
	}

	/* Prints the current OpenGL Modelview matrix */
	void printModelViewMatrix()
	{
		GLfloat m[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, m);

		printf("OpenGL ModelView Matrix:\n");

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				printf("%.3f ", m[i + j*4]);
			}
			printf("\n");
		}
		printf("\n");
	}

	/* Prints the current OpenGL Projection matrix */
	void printProjectionMatrix()
	{
		GLfloat m[16];
		glGetFloatv(GL_PROJECTION_MATRIX, m);

		printf("OpenGL Projection Matrix:\n");

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				printf("%.3f ", m[i + j * 4]);
			}
			printf("\n");
		}
		printf("\n");
	}

	/* Calculates the modelview matrix for the vertex shader using the camera parameters */
	void calcModelViewMat()
	{
		glm::vec3 n_axis(camera[0] - target[0], camera[1] - target[1], camera[2] - target[2]);
		n_axis = glm::normalize(n_axis);

		glm::vec3 u_axis(0.0f, 0.0f, 0.0f);
		u_axis = glm::cross(Camera::up, n_axis);

		glm::vec3 v_axis(0.0f, 0.0f, 0.0f);
		v_axis = glm::cross(n_axis, u_axis);

		glm::vec3 origin(camera[0], camera[1], camera[2]);

		if (Constants::DEBUG_MATRICES)
		{
			printf("From calcModelViewMat:\n");
			printf("\tu = [%.3f %.3f %.3f]\n\tv = [%.3f %.3f %.3f]\n\tn = [%.3f %.3f %.3f]\n\to = [%.3f %.3f %.3f]\n",
				u_axis[0], u_axis[1], u_axis[2],
				v_axis[0], v_axis[1], v_axis[2],
				n_axis[0], n_axis[1], n_axis[2],
				origin[0], origin[1], origin[2]);
			printf("\n");
		}

		origin = (float)(-1) * origin;
		glm::vec3 translate(0.0f, 0.0f, 0.0f);
		translate[0] = dot(origin, u_axis);
		translate[1] = dot(origin, v_axis);
		translate[2] = dot(origin, n_axis);

		GLfloat *p = ShaderLoader::modelViewMat;
		*p++ = u_axis[0]; *p++ = v_axis[0]; *p++ = n_axis[0]; *p++ = 0.0f;
		*p++ = u_axis[1]; *p++ = v_axis[1]; *p++ = n_axis[1]; *p++ = 0.0f;
		*p++ = u_axis[2]; *p++ = v_axis[2]; *p++ = n_axis[2]; *p++ = 0.0f;
		*p++ = translate[0]; *p++ = translate[1]; *p++ = translate[2]; *p++ = 1.0f;

		if (Constants::DEBUG_MATRICES)
		{
			printf("Calculated ModelView Matrix:\n");

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					printf("%.3f ", ShaderLoader::modelViewMat[i + j * 4]);
				}
				printf("\n");
			}
			printf("\n");
		}
	}

	/* Calculates the projection matrix for the vertex shader using the camera parameters */
	void calcProjectionMat()
	{
		GLfloat l = -Display::max_xy / 4;
		GLfloat r = Display::max_xy / 4;
		GLfloat b = -Display::max_xy / 4;
		GLfloat t = Display::max_xy / 4;
		GLdouble n = Camera::near_clip;
		GLdouble f = Camera::far_clip;

		GLfloat *q = ShaderLoader::projectionMat;
		*q++ = 2 * n / (r - l);		*q++ = 0.0f;				*q++ = 0.0f;					*q++ = 0.0f;
		*q++ = 0.0f;				*q++ = 2 * n / (t - b);		*q++ = 0.0f;					*q++ = 0.0f;
		*q++ = (r + l) / (r - l);	*q++ = (t + b) / (t - b);	*q++ = -(f + n) / (f - n);		*q++ = -1.0f;
		*q++ = 0.0f;				*q++ = 0.0f;				*q++ = -2 * f * n / (f - n);	*q++ = 0.0f;

		if (Constants::DEBUG_MATRICES)
		{
			printf("Calculated Projection Matrix:\n");
			for (int i = 0; i < 16; i++)
			{
				printf("%.3f ", ShaderLoader::projectionMat[i]);
				if ((i + 1) % 4 == 0) printf("\n");
			}
			printf("\n");
		}
	}

	/* Enforces near_clip <= far_clip */
	void increaseNearClip()
	{
		if (near_clip + Constants::clip_speed >= far_clip) near_clip = far_clip;
		else near_clip += Constants::clip_speed;
	}

	/* Enforces far_clip >= near_clip */
	void decreaseFarClip()
	{
		if (far_clip - Constants::clip_speed <= near_clip) far_clip = near_clip;
		else far_clip -= Constants::clip_speed;
	}
}