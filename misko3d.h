/*
 * misko3d.h
 *
 * Created: 29. 08. 2017 15:28:50
 *  Author: Adam Hrastnik
 */ 


#ifndef MISKO3D_H_
#define MISKO3D_H_

#include "ugui.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Colors
#define WIREFRAME_COLOR			C_LIGHT_SKY_BLUE
#define EDIT_VERTEX_COLOR		C_BLUE
#define SELECTED_VERTEX_COLOR	C_ORANGE
#define VIEWPORT_FRAME			C_GRAY
#define BACKGROUND				C_BLACK
#define FOREGROUND				C_WHITE
#define X_AXIS					C_RED
#define Y_AXIS					C_GREEN
#define Z_AXIS					C_LIGHT_BLUE

// View Modes
#define PRESPECTIVE		0
#define ORTHOGRAPHIC	1

// View
extern short viewMode;
extern float cameraLocation[3];
extern float focalLength;
extern char clipping;

// Screen
#define SCREEN_X			320
#define SCREEN_Y			240
#define SCREEN_X_MID		160
#define SCREEN_Y_MID		120
#define SCREEN_CLAMP_X_MIN	10
#define SCREEN_CLAMP_X_MAX	310
#define SCREEN_CLAMP_Y_MIN	30
#define SCREEN_CLAMP_Y_MAX	210

// Coordinate System
#define WORLD 0
#define LOCAL 1

// Objects
#define SELECTED_VERTEX_RADIUS	2
extern struct object active_object;
extern char bIsObjectSet;
extern char bEditing;
extern char bDrawAxis;
extern unsigned int AxisSize;

struct vector3d{
	float x, y, z;
};

struct vector2d{
	float x, y;
};

struct edge{
	unsigned int P1, P2;
};

struct face{
	struct vector3d normal;
};

struct object{
	float location[3];
	float rotation[3];
	float scale[3];
	float size[3];
	float rotationMatrix[3][3];
	float transformMatrix[4][4];
	struct vector3d *verteciesLocal;
	struct vector3d *verteciesWorld;
	char *selectedVertecies;
	unsigned int numOfVertecies;
	struct edge *edges; // Edge contains a index for each of its vertices
	unsigned int numOfEdges;
	struct face *faces;
	unsigned int numOfFaces;
	UG_COLOR color;
};

// Inner
void _project(struct vector3d *vect3d, struct vector2d *vect2d);
void _recalculateObject(struct object *obj);
void _clampScreenPoint(struct vector2d *vect2d);
void _calcCircle(struct object *obj, float radius, float zPos, uint8_t vertices, unsigned int vertexStart, unsigned int edgeStart);
void _initVertexWorldPositions(struct object *obj);
float _toDegrees(float rad);
float _toRadians(float degrees);

// Core
void init3D();
void DrawLine(struct vector3d vert1, struct vector3d vert2, UG_COLOR color);
void DrawTris(struct vector3d vert1, struct vector3d  vert2, struct vector3d  vert3, UG_COLOR color);
void DrawQuad(struct vector3d vert1, struct vector3d  vert2, struct vector3d  vert3, struct vector3d  vert4, UG_COLOR color);
void DrawObjectAxis(struct object *obj, struct vector3d origin, char bClear);
void DrawPolygon(float **points, unsigned int numberOfPoints, UG_COLOR color);

// Viewport
void redrawViewport();
void clearViewport();

// Basic Shapes
void AddLine(float size);
void AddRectangle(float size);
void AddCircle(float radius, uint8_t vertices);
void AddCube(float size);
void AddCylinder(float radius, float size, uint8_t vertices);
void AddSphere(float radius, uint8_t segments, uint8_t rings);
void AddAxis(float size);
void AddMesh(struct object *mesh);

// Object Functions
void _addObject(struct object *obj);
void removeObject(struct object *obj);
void selectNextObject();
void redrawObject(struct object *obj, uint8_t bClear);

void setLocation(struct object *obj);
void setRotation(struct object *obj);
void setScale(struct object *obj);

void resetLocation(struct object *obj);
void resetRotation(struct object *obj);
void resetScale(struct object *obj);
void resetObject(struct object *obj);

char doesObjectHaveSelectedVertices(struct object *obj);
char isObjectOnScreen(struct object *obj);
struct vector3d calcSelectedMedian(struct object *obj);

// Vertex functions
void _translateVertex(struct vector3d *vert, float location[3]);
void _rotateVertex(struct vector3d *vert, float origin[3], float rotation[3]);
void _scaleVertex(struct vector3d *vert, float scale[3]);

// Matrix && Vector operations
void _homogeneusMatrix(float matrix[4][4]);
void _rotationMatrix(float matrix[3][3]);
void _multHomMatrix(float matrix1[4][4], float matrix2[4][4], float newMatrix[4][4]);
void _multRotMatrix(float matrix1[3][3], float matrix2[3][3], float newMatrix[3][3]);
void _multHomMatrixVect(float matrix[4][4], struct vector3d vect3d, struct vector3d *newVect3d);
void _copyMatrix4(float matrixToCopy[4][4], float matrixToPaste[4][4]);
void _copyMatrix3(float matrixToCopy[3][3], float matrixToPaste[3][3]);
void _setTransformRotation(float transform[4][4], float rotation[3][3]);
void printMatrix(float matrix[4][4]);
void printVector3d(struct vector3d vect3d);

#endif /* MISKO3D_H_ */