#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <iostream>

#include "GL/freeglut.h"

#include "Display.hpp"
#include "Camera.hpp"
#include "ObjectLoader.hpp"

namespace ObjectLoader
{
	const bool DEBUG(false);

	// used to calculate vertex normals for Gouraud shading
	std::vector<GLfloat> faceNormals;
	std::vector<GLfloat> faceAreas;

	// maps vertices to all faces they belong to
	std::vector< std::list< GLuint > > memberFaces;

	/********************************************************************************
	 *                            OBJECT LOADER FUNCTIONS                           *
	 ********************************************************************************/

	/* Reads the vertex/face data from the argument file; stores this data in the
	* global vectors vertexCoords and faceVertices for display() to use with
	* glDrawElements(). Also stores min/max vertex coordinates globally.
	*
	* Returns true for successful load; false otherwise.
	*/
	bool loadObject(char *filepath)
	{
		FILE *fp;
		fp = fopen(filepath, "r");

		if (fp == NULL)
		{
			printf("Can't open \"%s\"\n", filepath);
			return false;
		}

		// for error checking/printing during file reading
		int numRead;
		int line_count = 1;

		char ch; // limited to 'v' and 'f' for this loader

		GLfloat x, y, z; // vertex coords
		GLuint v1, v2, v3; // vertex indices (for faces)

		while (!(feof(fp)))
		{
			fscanf(fp, "%c", &ch);

			if (ch == 'v')
			{
				numRead = fscanf(fp, "%f %f %f\n", &x, &y, &z);

				if (numRead != 3)
				{
					printf("Less than 3 values on line %d\n", line_count);
					return false;
				}

				Display::vertexCoords.push_back(x);
				Display::vertexCoords.push_back(y);
				Display::vertexCoords.push_back(z);

				if (x > Display::maxx) Display::maxx = x;
				if (y > Display::maxy) Display::maxy = y;
				if (z > Display::maxz) Display::maxz = z;

				if (x < Display::minx) Display::minx = x;
				if (y < Display::miny) Display::miny = y;
				if (z < Display::minz) Display::minz = z;
			}
			else if (ch == 'f')
			{
				numRead = fscanf(fp, "%d %d %d\n", &v1, &v2, &v3);

				if (numRead != 3)
				{
					printf("Less than 3 values on line %d\n", line_count);
					return false;
				}

				Display::faceVertices.push_back(v1-1);
				Display::faceVertices.push_back(v2-1);
				Display::faceVertices.push_back(v3-1);
			}
			else
			{
				fscanf(fp, "\n"); // character other than 'v' or 'f'; skip line
			}

			line_count++;
		}

		if (abs(Display::maxx - Display::minx) > abs(Display::maxy - Display::miny))
		{
			Display::max_xy = abs(Display::maxx - Display::minx);
		}
		else
		{
			Display::max_xy = abs(Display::maxy - Display::miny);
		}

		if (DEBUG) printf("max: \t%.5f, %.5f, %.5f\n", Display::maxx, Display::maxy, Display::maxz);
		if (DEBUG) printf("min: \t%.5f, %.5f, %.5f\n\n", Display::minx, Display::miny, Display::minz);

		processFaces(); // once all faces have been read, calculate normals for Gouraud based on them

		return true;
	}

	/* Changes the object model to that in the given filepath; clears previous data
	* and calls loadObject() */
	void changeModel(char *filepath)
	{
		if (!strcmp(filepath, Display::current_model)) return; // ignore redundant loads

		strcpy(Display::current_model, filepath);

		Display::vertexCoords.clear();
		Display::faceVertices.clear();
		Display::vertexNormals.clear();

		faceNormals.clear();
		faceAreas.clear();

		loadObject(filepath);
		Camera::resetCamera();

		Display::reinitializeShaders();
	}

	/* Calculates the normal of each face, store in ObjectLoader::faceNormals;
	   stores the area of each face in ObjectLoader::faceAreas, builds map of
	   vertices to faces containing them in ObjectLoader::memberFaces */
	void processFaces()
	{
		int numIndices = Display::faceVertices.size();
		int numVertices = Display::vertexCoords.size() / 3;

		// reserve space for every vertex in the map of vertices to faces
		memberFaces.resize(numVertices);

		// loop through all faces
		for (int i = 0; i < numIndices; i += 3)
		{
			int ind0 = Display::faceVertices[i];
			int ind1 = Display::faceVertices[i+1];
			int ind2 = Display::faceVertices[i+2];

			// add this face to the lists of faces for each member vertex
			memberFaces[ind0].push_back(i / 3);
			memberFaces[ind1].push_back(i / 3);
			memberFaces[ind2].push_back(i / 3);

			// calculate the normal for this face
			glm::vec3 v0(Display::vertexCoords[ind0 * 3],
				Display::vertexCoords[ind0 * 3 + 1],
				Display::vertexCoords[ind0 * 3 + 2]);

			glm::vec3 v1(Display::vertexCoords[ind1 * 3],
				Display::vertexCoords[ind1 * 3 + 1],
				Display::vertexCoords[ind1 * 3 + 2]);

			glm::vec3 v2(Display::vertexCoords[ind2 * 3],
				Display::vertexCoords[ind2 * 3 + 1],
				Display::vertexCoords[ind2 * 3 + 2]);

			glm::vec3 edge1(v1 - v0);
			glm::vec3 edge2(v2 - v0);

			glm::vec3 crossProd(glm::cross(edge1, edge2));
			glm::vec3 normal(glm::normalize(crossProd));

			faceNormals.push_back(normal.x);
			faceNormals.push_back(normal.y);
			faceNormals.push_back(normal.z);

			// store the area of this face
			faceAreas.push_back(0.5f * glm::length(crossProd));
		}

		// once all face data has been processed, calculate vertex normals
		calculateVertexNormals();
	}

	/* For each vertex, loops through member faces to calculate normal;
	 * uses the average of adjacent face normals, weighted by face area,
	 * to implement smooth Gouraud shading */
	void calculateVertexNormals()
	{
		int numVertices = Display::vertexCoords.size() / 3;

		// loop through all vertices
		for (int i = 0; i < numVertices; i++)
		{
			glm::vec3 vertNormal(0.0f, 0.0f, 0.0f);

			// loop through member faces
			while (!memberFaces[i].empty())
			{
				GLuint faceIndex = memberFaces[i].front();
				memberFaces[i].pop_front();

				glm::vec3 faceNormal(faceNormals[faceIndex*3], 
					faceNormals[faceIndex*3+1], 
					faceNormals[faceIndex*3+2]);

				vertNormal += faceAreas[faceIndex] * faceNormal;
			}

			vertNormal = glm::normalize(vertNormal);

			Display::vertexNormals.push_back(vertNormal.x);
			Display::vertexNormals.push_back(vertNormal.y);
			Display::vertexNormals.push_back(vertNormal.z);
		}
	}

	/* Debugging */
	void printMemberFaces()
	{
		int numVertices = Display::vertexCoords.size() / 3;

		for (int i = 0; i < numVertices; i++)
		{
			printf("Faces containing vertex %d: ", i);
			printf("[%d] ", memberFaces[i].size());
			while (!memberFaces[i].empty())
			{
				std::cout << ' ' << memberFaces[i].front();
				memberFaces[i].pop_front();
			}
			printf("\n");
		}
	}

}