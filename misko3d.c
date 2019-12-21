/*
 * misko3d.c
 *
 * Created: 29. 08. 2017 15:29:05
 *  Author: Adam Hrastnik
 */ 

#include "misko3d.h"

struct object active_object;
char bIsObjectSet = 0;
char bEditing = 0;
char bDrawAxis = 0;
unsigned int AxisSize = 20;
short viewMode = PRESPECTIVE;
float cameraLocation[3] = {0, 0, -100};
float focalLength = 110;
char clipping = 0;

// Core
void init3D(){
	// Draw Viewport
	//redrawViewport();
}

void DrawLine(struct vector3d vert1, struct vector3d vert2, UG_COLOR color){
	struct vector2d screenPoint1, screenPoint2;
	_project(&vert1, &screenPoint1);
	_project(&vert2, &screenPoint2);

	UG_DrawLine(screenPoint1.x, screenPoint1.y, screenPoint2.x, screenPoint2.y, color);
}

void DrawTris(struct vector3d vert1, struct vector3d  vert2, struct vector3d  vert3, UG_COLOR color){
	struct vector2d screenPoint1, screenPoint2, screenPoint3;
	_project(&vert1, &screenPoint1);
	_project(&vert2, &screenPoint2);
	_project(&vert3, &screenPoint3);

	UG_DrawLine(screenPoint1.x, screenPoint1.y, screenPoint2.x, screenPoint2.y, color);
	UG_DrawLine(screenPoint2.x, screenPoint2.y, screenPoint3.x, screenPoint3.y, color);
	UG_DrawLine(screenPoint3.x, screenPoint3.y, screenPoint1.x, screenPoint1.y, color);
}

void DrawQuad(struct vector3d vert1, struct vector3d  vert2, struct vector3d  vert3, struct vector3d  vert4, UG_COLOR color){
	struct vector2d screenPoint1, screenPoint2, screenPoint3, screenPoint4;
	_project(&vert1, &screenPoint1);
	_project(&vert2, &screenPoint2);
	_project(&vert3, &screenPoint3);
	_project(&vert4, &screenPoint4);

	UG_DrawLine(screenPoint1.x, screenPoint1.y, screenPoint2.x, screenPoint2.y, color);
	UG_DrawLine(screenPoint2.x, screenPoint2.y, screenPoint3.x, screenPoint3.y, color);
	UG_DrawLine(screenPoint3.x, screenPoint3.y, screenPoint4.x, screenPoint4.y, color);
	UG_DrawLine(screenPoint4.x, screenPoint4.y, screenPoint1.x, screenPoint1.y, color);
}

void DrawObjectAxis(struct object *obj, struct vector3d origin, char bClear){
	struct vector2d P1, P2;
	struct vector3d V1, V2;
	V1.x = 0;
	V1.y = 0;
	V1.z = 0;
	_multHomMatrixVect(obj->transformMatrix, V1, &V2);
	_project(&V2, &P1);
	V1.x = AxisSize;
	V1.y = 0;
	V1.z = 0;
	_multHomMatrixVect(obj->transformMatrix, V1, &V2);
	_project(&V2, &P2);
	UG_DrawLine(P1.x, P1.y, P2.x, P2.y, bClear ? BACKGROUND : C_RED);
	V1.x = 0;
	V1.y = AxisSize;
	V1.z = 0;
	_multHomMatrixVect(obj->transformMatrix, V1, &V2);
	_project(&V2, &P2);
	UG_DrawLine(P1.x, P1.y, P2.x, P2.y, bClear ? BACKGROUND : C_GREEN);
	V1.x = 0;
	V1.y = 0;
	V1.z = AxisSize;
	_multHomMatrixVect(obj->transformMatrix, V1, &V2);
	_project(&V2, &P2);
	UG_DrawLine(P1.x, P1.y, P2.x, P2.y, bClear ? BACKGROUND : C_BLUE);
}

// Viewport
void redrawViewport(){
	const char x[] = "x+";
	const char y[] = "y+";
	// Viewport Frame
	UG_DrawFrame(SCREEN_CLAMP_X_MIN-1, SCREEN_CLAMP_Y_MIN-1, SCREEN_CLAMP_X_MAX+1, SCREEN_CLAMP_Y_MAX+1, VIEWPORT_FRAME);
	// X Axis
	UG_DrawLine(SCREEN_CLAMP_X_MIN, SCREEN_Y_MID, SCREEN_CLAMP_X_MAX, SCREEN_Y_MID, X_AXIS);
	UG_SetForecolor(X_AXIS);
	UG_PutString(SCREEN_CLAMP_X_MAX-22, SCREEN_Y_MID+2, (const char*)&x);
	// Y Axis
	UG_DrawLine(SCREEN_X_MID, SCREEN_CLAMP_Y_MIN, SCREEN_X_MID, SCREEN_CLAMP_Y_MAX, Y_AXIS);
	UG_SetForecolor(Y_AXIS);
	UG_PutString(SCREEN_X_MID+5, SCREEN_CLAMP_Y_MAX-15, (const char*)&y);
	// Z Axis
	UG_DrawCircle(SCREEN_X_MID, SCREEN_Y_MID, 4, Z_AXIS);
	UG_PutChar('z', SCREEN_X_MID+5, SCREEN_Y_MID+2, Z_AXIS, BACKGROUND);
	UG_SetForecolor(FOREGROUND);
}

void clearViewport(){
	UG_FillFrame(SCREEN_CLAMP_X_MIN, SCREEN_CLAMP_Y_MIN, SCREEN_CLAMP_X_MAX, SCREEN_CLAMP_Y_MAX, BACKGROUND);
	redrawViewport();
}

// Basic Shapes
void AddLine(float size){
	_addObject(&active_object);
	active_object.numOfVertecies = 2;
	active_object.verteciesLocal = calloc(2, sizeof(struct vector3d));
	active_object.verteciesWorld = calloc(2, sizeof(struct vector3d));
	active_object.selectedVertecies = calloc(2, sizeof(char));
	active_object.numOfEdges = 1;
	active_object.edges = calloc(1, sizeof(struct edge));
	// Set Size
	active_object.size[0] = 0;
	active_object.size[1] = size;
	active_object.size[2] = 0;

	// Vertices
	active_object.verteciesLocal[0].x = 0;
	active_object.verteciesLocal[0].y = 0;
	active_object.verteciesLocal[0].z = 0;
	active_object.verteciesLocal[1].x = 0;
	active_object.verteciesLocal[1].y = -size;
	active_object.verteciesLocal[1].z = 0;
	_initVertexWorldPositions(&active_object);

	// Edges
	active_object.edges[0].P1 = 0;
	active_object.edges[0].P2 = 1;

	redrawObject(&active_object, 0);
}

void AddRectangle(float size){
	_addObject(&active_object);
	active_object.numOfVertecies = 4;
	active_object.verteciesLocal = calloc(4, sizeof(struct vector3d));
	active_object.verteciesWorld = calloc(4, sizeof(struct vector3d));
	active_object.selectedVertecies = calloc(4, sizeof(char));
	active_object.numOfEdges = 4;
	active_object.edges = calloc(4, sizeof(struct edge));
	// Set Size
	active_object.size[0] = size;
	active_object.size[1] = size;
	active_object.size[2] = 0;

	// Vertex Local locations
	active_object.verteciesLocal[0].x = (active_object.size[0]/2);
	active_object.verteciesLocal[0].y = (active_object.size[1]/2);
	active_object.verteciesLocal[0].z = 0;
	active_object.verteciesLocal[1].x = (-active_object.size[0]/2);
	active_object.verteciesLocal[1].y = (active_object.size[1]/2);
	active_object.verteciesLocal[1].z = 0;
	active_object.verteciesLocal[2].x = (-active_object.size[0]/2);
	active_object.verteciesLocal[2].y = (-active_object.size[1]/2);
	active_object.verteciesLocal[2].z = 0;
	active_object.verteciesLocal[3].x = (active_object.size[0]/2);
	active_object.verteciesLocal[3].y = (-active_object.size[1]/2);
	active_object.verteciesLocal[3].z = 0;
	// Vertex World locations
	_initVertexWorldPositions(&active_object);
	// Edges
	active_object.edges[0].P1 = 0;
	active_object.edges[0].P2 = 1;
	active_object.edges[1].P1 = 1;
	active_object.edges[1].P2 = 2;
	active_object.edges[2].P1 = 2;
	active_object.edges[2].P2 = 3;
	active_object.edges[3].P1 = 3;
	active_object.edges[3].P2 = 0;

	// Draw the object
	redrawObject(&active_object, 0);
}

void AddCircle(float radius, uint8_t vertices){
	if(vertices >= 3){
		_addObject(&active_object);
		active_object.numOfVertecies = vertices;
		active_object.verteciesLocal = calloc(vertices, sizeof(struct vector3d));
		active_object.verteciesWorld = calloc(vertices, sizeof(struct vector3d));
		active_object.selectedVertecies = calloc(vertices, sizeof(char));
		active_object.numOfEdges = vertices;
		active_object.edges = calloc(vertices, sizeof(struct edge));
		// Set Size
		active_object.size[0] = radius*2;
		active_object.size[1] = radius*2;
		active_object.size[2] = 0;
		// Draw Circles
		_calcCircle(&active_object, radius, 0, vertices, 0, 0);

		_initVertexWorldPositions(&active_object);
		// Draw the object
		redrawObject(&active_object, 0);
	}
}

void AddCube(float size){
	_addObject(&active_object);
	active_object.numOfVertecies = 8;
	active_object.verteciesLocal = calloc(8, sizeof(struct vector3d));
	active_object.verteciesWorld = calloc(8, sizeof(struct vector3d));
	active_object.selectedVertecies = calloc(8, sizeof(char));
	active_object.numOfEdges = 12;
	active_object.edges = calloc(12, sizeof(struct edge));
	// Set Size
	active_object.size[0] = size;
	active_object.size[1] = size;
	active_object.size[2] = size;

	// Vertex Local locations
	active_object.verteciesLocal[0].x = (active_object.size[0]/2);
	active_object.verteciesLocal[0].y = (active_object.size[1]/2);
	active_object.verteciesLocal[0].z = (active_object.size[2]/2);
	active_object.verteciesLocal[1].x = (active_object.size[0]/2);
	active_object.verteciesLocal[1].y = (active_object.size[1]/2);
	active_object.verteciesLocal[1].z = (-active_object.size[2]/2);
	active_object.verteciesLocal[2].x = (active_object.size[0]/2);
	active_object.verteciesLocal[2].y = (-active_object.size[1]/2);
	active_object.verteciesLocal[2].z = (active_object.size[2]/2);
	active_object.verteciesLocal[3].x = (active_object.size[0]/2);
	active_object.verteciesLocal[3].y = (-active_object.size[1]/2);
	active_object.verteciesLocal[3].z = (-active_object.size[2]/2);
	active_object.verteciesLocal[4].x = (-active_object.size[0]/2);
	active_object.verteciesLocal[4].y = (-active_object.size[1]/2);
	active_object.verteciesLocal[4].z = (active_object.size[2]/2);
	active_object.verteciesLocal[5].x = (-active_object.size[0]/2);
	active_object.verteciesLocal[5].y = (-active_object.size[1]/2);
	active_object.verteciesLocal[5].z = (-active_object.size[2]/2);
	active_object.verteciesLocal[6].x = (-active_object.size[0]/2);
	active_object.verteciesLocal[6].y = (active_object.size[1]/2);
	active_object.verteciesLocal[6].z = (active_object.size[2]/2);
	active_object.verteciesLocal[7].x = (-active_object.size[0]/2);
	active_object.verteciesLocal[7].y = (active_object.size[1]/2);
	active_object.verteciesLocal[7].z = (-active_object.size[2]/2);
	// Vertex World locations
	_initVertexWorldPositions(&active_object);
	// Edges
	active_object.edges[0].P1 = 0;
	active_object.edges[0].P2 = 2;
	active_object.edges[1].P1 = 2;
	active_object.edges[1].P2 = 4;
	active_object.edges[2].P1 = 4;
	active_object.edges[2].P2 = 6;
	active_object.edges[3].P1 = 6;
	active_object.edges[3].P2 = 0;

	active_object.edges[4].P1 = 0;
	active_object.edges[4].P2 = 1;
	active_object.edges[5].P1 = 2;
	active_object.edges[5].P2 = 3;
	active_object.edges[6].P1 = 4;
	active_object.edges[6].P2 = 5;
	active_object.edges[7].P1 = 6;
	active_object.edges[7].P2 = 7;

	active_object.edges[8].P1 = 1;
	active_object.edges[8].P2 = 3;
	active_object.edges[9].P1 = 3;
	active_object.edges[9].P2 = 5;
	active_object.edges[10].P1 = 5;
	active_object.edges[10].P2 = 7;
	active_object.edges[11].P1 = 7;
	active_object.edges[11].P2 = 1;

	// Draw the object
	redrawObject(&active_object, 0);
}

void AddCylinder(float radius, float size, uint8_t vertices){
	if(vertices >= 3){
		_addObject(&active_object);
		active_object.numOfVertecies = vertices*2;
		active_object.verteciesLocal = calloc(vertices*2, sizeof(struct vector3d));
		active_object.verteciesWorld = calloc(vertices*2, sizeof(struct vector3d));
		active_object.selectedVertecies = calloc(vertices*2, sizeof(char));
		active_object.numOfEdges = vertices*3;
		active_object.edges = calloc(vertices*3, sizeof(struct edge));
		// Set Size
		active_object.size[0] = radius*2;
		active_object.size[1] = radius*2;
		active_object.size[2] = size;
		// Draw Circles
		_calcCircle(&active_object, radius, size/2, vertices, 0, 0);
		_calcCircle(&active_object, radius, -size/2, vertices, vertices, vertices);
		// Connect Edges
		uint8_t i;
		for(i = 0; i < vertices; i++){
			active_object.edges[i+vertices*2].P1 = i;
			active_object.edges[i+vertices*2].P2 = i+vertices;
		}

		_initVertexWorldPositions(&active_object);
		// Draw the object
		redrawObject(&active_object, 0);
	}
}

void AddSphere(float radius, uint8_t segments, uint8_t rings){
	if((segments >= 3) && (rings >= 3)){
		_addObject(&active_object);
		uint8_t vertices = (segments*(rings-1))+2;
		uint8_t edges = (vertices-2)+(segments*rings);
		active_object.numOfVertecies = vertices;
		active_object.verteciesLocal = calloc(vertices, sizeof(struct vector3d));
		active_object.verteciesWorld = calloc(vertices, sizeof(struct vector3d));
		active_object.selectedVertecies = calloc(vertices, sizeof(char));
		active_object.numOfEdges = edges;
		active_object.edges = calloc(edges, sizeof(struct edge));
		// Set Size
		active_object.size[0] = radius*2;
		active_object.size[1] = radius*2;
		active_object.size[2] = radius*2;

		uint8_t i;
		for(i = 0; i < (rings-1); i++){
			_calcCircle(&active_object, radius, i*10, segments, i*segments, i*segments);
		}
		_calcCircle(&active_object, radius, 0, segments, 0, 0);
		// NOT COMPLETED
	}
}

void AddAxis(float size){
	_addObject(&active_object);
	active_object.numOfVertecies = 4;
	active_object.verteciesLocal = calloc(4, sizeof(struct vector3d));
	active_object.verteciesWorld = calloc(4, sizeof(struct vector3d));
	active_object.selectedVertecies = calloc(4, sizeof(char));
	active_object.numOfEdges = 3;
	active_object.edges = calloc(3, sizeof(struct edge));
	// Set Size
	active_object.size[0] = size;
	active_object.size[1] = size;
	active_object.size[2] = size;

	// Vertices
	active_object.verteciesLocal[0].x = 0;
	active_object.verteciesLocal[0].y = 0;
	active_object.verteciesLocal[0].z = 0;
	active_object.verteciesLocal[1].x = size;
	active_object.verteciesLocal[1].y = 0;
	active_object.verteciesLocal[1].z = 0;
	active_object.verteciesLocal[2].x = 0;
	active_object.verteciesLocal[2].y = size;
	active_object.verteciesLocal[2].z = 0;
	active_object.verteciesLocal[3].x = 0;
	active_object.verteciesLocal[3].y = 0;
	active_object.verteciesLocal[3].z = size;
	_initVertexWorldPositions(&active_object);
	// Edges
	active_object.edges[0].P1 = 0;
	active_object.edges[0].P2 = 1;
	active_object.edges[1].P1 = 0;
	active_object.edges[1].P2 = 2;
	active_object.edges[2].P1 = 0;
	active_object.edges[2].P2 = 3;
	redrawObject(&active_object, 0);
}

void AddMesh(struct object *mesh){
	
}

// Object Functions

void _addObject(struct object *obj){
	// Set Default Color
	obj->color = WIREFRAME_COLOR;
	// Set Default Transform
	resetObject(obj);

	if(!bIsObjectSet){
		bIsObjectSet = 1;
	}else{
		removeObject(obj);
	}
}

void removeObject(struct object *obj){
	free(obj->verteciesLocal);
	free(obj->verteciesWorld);
	free(obj->selectedVertecies);
	free(obj->edges);
}

void redrawObject(struct object *obj, uint8_t bClear){
	struct vector2d P1, P2;
	uint16_t i;
	if(bClear){
		if(bEditing){
			for(i = 0; i < obj->numOfVertecies; i++){
				_project(&obj->verteciesWorld[i], &P1);
				UG_FillFrame(P1.x-SELECTED_VERTEX_RADIUS, P1.y-SELECTED_VERTEX_RADIUS, P1.x+SELECTED_VERTEX_RADIUS, P1.y+SELECTED_VERTEX_RADIUS, BACKGROUND);
			}
		}
		for(i = 0; i < obj->numOfEdges; i++){
			_project(&obj->verteciesWorld[obj->edges[i].P1], &P1);// Project Point 1
			_project(&obj->verteciesWorld[obj->edges[i].P2], &P2);// Project Point 2

			UG_DrawLine(P1.x, P1.y, P2.x, P2.y, BACKGROUND);
		}
	}
	redrawViewport();
	_recalculateObject(obj);
	for(i = 0; i < obj->numOfEdges; i++){
		_project(&obj->verteciesWorld[obj->edges[i].P1], &P1);// Project Point 1
		_project(&obj->verteciesWorld[obj->edges[i].P2], &P2);// Project Point 2

		UG_DrawLine(P1.x, P1.y, P2.x, P2.y, obj->color);
	}
	if(bEditing){
		for(i = 0; i < obj->numOfVertecies; i++){
			_project(&obj->verteciesWorld[i], &P1);
			if(obj->selectedVertecies[i]){
				UG_FillFrame(P1.x-SELECTED_VERTEX_RADIUS, P1.y-SELECTED_VERTEX_RADIUS, P1.x+SELECTED_VERTEX_RADIUS, P1.y+SELECTED_VERTEX_RADIUS, SELECTED_VERTEX_COLOR);
			}else{
				UG_FillFrame(P1.x-SELECTED_VERTEX_RADIUS, P1.y-SELECTED_VERTEX_RADIUS, P1.x+SELECTED_VERTEX_RADIUS, P1.y+SELECTED_VERTEX_RADIUS, EDIT_VERTEX_COLOR);
			}
		}
	}
}

void setLocation(struct object *obj){
	obj->transformMatrix[3][0] = obj->location[0];
	obj->transformMatrix[3][1] = obj->location[1];
	obj->transformMatrix[3][2] = obj->location[2];
}

void setRotation(struct object *obj){
	float rotMatrix[3][3], rotTempMatrix[3][3], newRotMatrix[3][3];
	float cosCalc, sinCalc;
	
	// X Rotation
	_rotationMatrix(rotMatrix);
	cosCalc = cos(obj->rotation[0]);
	sinCalc = sin(obj->rotation[0]);
	rotMatrix[1][1] = cosCalc;
	rotMatrix[1][2] = sinCalc;
	rotMatrix[2][1] = -sinCalc;
	rotMatrix[2][2] = cosCalc;
	_copyMatrix3(rotMatrix, rotTempMatrix);
	
	// Y Rotation
	_rotationMatrix(rotMatrix);
	cosCalc = cos(obj->rotation[1]);
	sinCalc = sin(obj->rotation[1]);
	rotMatrix[0][0] = cosCalc;
	rotMatrix[0][2] = -sinCalc;
	rotMatrix[2][0] = sinCalc;
	rotMatrix[2][2] = cosCalc;
	_multRotMatrix(rotTempMatrix, rotMatrix, newRotMatrix);
	_copyMatrix3(newRotMatrix, rotTempMatrix);
	
	// Z Rotation
	_rotationMatrix(rotMatrix);
	cosCalc = cos(obj->rotation[2]);
	sinCalc = sin(obj->rotation[2]);
	rotMatrix[0][0] = cosCalc;
	rotMatrix[0][1] = sinCalc;
	rotMatrix[1][0] = -sinCalc;
	rotMatrix[1][1] = cosCalc;
	_multRotMatrix(rotTempMatrix, rotMatrix, newRotMatrix);
	
	// Apply new Rotation
	_copyMatrix3(newRotMatrix, obj->rotationMatrix);
	_setTransformRotation(obj->transformMatrix, newRotMatrix);
	// Restore the Scale
	setScale(obj);
}

void setScale(struct object *obj){
	uint8_t i, j;
	for(i = 0; i < 3; i++){
		for(j = 0; j < 3; j++){
			obj->transformMatrix[i][j] = obj->rotationMatrix[i][j] * obj->scale[i];
		}
	}
}

void resetLocation(struct object *obj){
	obj->location[0] = 0;
	obj->location[1] = 0;
	obj->location[2] = 0;
	setLocation(obj);
}

void resetRotation(struct object *obj){
	obj->rotation[0] = 0;
	obj->rotation[1] = 0;
	obj->rotation[2] = 0;
	_rotationMatrix(obj->rotationMatrix);
	_setTransformRotation(obj->transformMatrix, obj->rotationMatrix);
}

void resetScale(struct object *obj){
	obj->scale[0] = 1;
	obj->scale[1] = 1;
	obj->scale[2] = 1;
	_setTransformRotation(obj->transformMatrix, obj->rotationMatrix);
}

void resetObject(struct object *obj){
	resetLocation(obj);
	resetRotation(obj);
	resetScale(obj);
	_homogeneusMatrix(obj->transformMatrix);
	_rotationMatrix(obj->rotationMatrix);
}

char doesObjectHaveSelectedVertices(struct object *obj){
	uint16_t i;
	for(i = 0; i < obj->numOfVertecies; i++){
		if(obj->selectedVertecies[i]){
			return 1;
		}
	}
	return 0;
}

char isObjectOnScreen(struct object *obj){
	/*float screenPoint[2];
	_project(obj->location, screenPoint);
	if(screenPoint[0] > SCREEN_CLAMP_X_MAX
		|| screenPoint[0] < SCREEN_CLAMP_X_MIN
		|| screenPoint[1] > SCREEN_CLAMP_Y_MAX
		|| screenPoint[1] < SCREEN_CLAMP_Y_MIN){
		return 0;
	}*/
	return 1;
}

struct vector3d calcSelectedMedian(struct object *obj){
	struct vector3d median;
	if(!doesObjectHaveSelectedVertices(obj)){
		median.x = 0;
		median.y = 0;
		median.z = 0;
		return median;
	}
	uint16_t i, j;
	j = 0;
	for(i = 0; i < obj->numOfVertecies; i++){
		if(obj->selectedVertecies[i]){
			median.x += obj->verteciesWorld[i].x;
			median.y += obj->verteciesWorld[i].y;
			median.z += obj->verteciesWorld[i].z;
			j++;
		}
	}
	median.x /= j;
	median.y /= j;
	median.z /= j;
	return median;
}

// Inner
void _project(struct vector3d *vect3d, struct vector2d *vect2d){
	struct vector3d offsetVect3d;
	offsetVect3d.x = vect3d->x-cameraLocation[0];
	offsetVect3d.y = vect3d->y-cameraLocation[1];
	offsetVect3d.z = vect3d->z-cameraLocation[2];
	// Project a 3d Point on the Screen
	if(viewMode == PRESPECTIVE){
		if(offsetVect3d.z > 0){
			// X = Xc/Zc
			vect2d->x = focalLength*(offsetVect3d.x/offsetVect3d.z) + SCREEN_X_MID;
			// Y = Yc/Zc
			vect2d->y = focalLength*(offsetVect3d.y/offsetVect3d.z) + SCREEN_Y_MID;
		}else{
			// X = Xc
			vect2d->x = offsetVect3d.x + SCREEN_X_MID;
			// Y = Yc
			vect2d->y = offsetVect3d.y + SCREEN_Y_MID;
		}
	}else{
		// X = Xc
		vect2d->x = offsetVect3d.x + SCREEN_X_MID;
		// Y = Yc
		vect2d->y = offsetVect3d.y + SCREEN_Y_MID;
	}
	_clampScreenPoint(vect2d);
}

void _translateVertex(struct vector3d *vert, float location[3]){
	vert->x += location[0];
	vert->y += location[1];
	vert->z += location[2];
}

void _rotateVertex(struct vector3d *vert, float origin[3], float rotation[3]){
	float x, y, z;
	// X rotation
	if(rotation[0] != 0){
		y = vert->y*cos(rotation[0]) - vert->z*sin(rotation[0]);
		z = vert->y*sin(rotation[0]) + vert->z*cos(rotation[0]);
		vert->y = y;
		vert->z = z;
	}
	// Y rotation
	if(rotation[1] != 0){
		z = vert->z*cos(rotation[1]) - vert->x*sin(rotation[1]);
		x = vert->z*sin(rotation[1]) + vert->x*cos(rotation[1]);
		vert->z = z;
		vert->x = x;
	}
	// Z rotation
	if(rotation[2] != 0){
		x = vert->x*cos(rotation[2]) - vert->y*sin(rotation[2]);
		y = vert->x*sin(rotation[2]) + vert->y*cos(rotation[2]);
		vert->x = x;
		vert->y = y;
	}
}

void _scaleVertex(struct vector3d *vert, float scale[3]){
	vert->x *= scale[0];
	vert->y *= scale[1];
	vert->z *= scale[2];
}

void _recalculateObject(struct object *obj){
	uint8_t i;
	for(i = 0; i < obj->numOfVertecies; i++){
		_multHomMatrixVect(obj->transformMatrix, obj->verteciesLocal[i], &obj->verteciesWorld[i]);
	}
}

void _clampScreenPoint(struct vector2d *vect2d){
	if(vect2d->x > SCREEN_CLAMP_X_MAX){
		vect2d->x = SCREEN_CLAMP_X_MAX;
		}else if(vect2d->x < SCREEN_CLAMP_X_MIN){
		vect2d->x = SCREEN_CLAMP_X_MIN;
	}
	if(vect2d->y > SCREEN_CLAMP_Y_MAX){
		vect2d->y = SCREEN_CLAMP_Y_MAX;
		}else if(vect2d->y < SCREEN_CLAMP_Y_MIN){
		vect2d->y = SCREEN_CLAMP_Y_MIN;
	}
}

void _calcCircle(struct object *obj, float radius, float zPos, uint8_t vertices, unsigned int vertexStart, unsigned int edgeStart){
	float vertexAngle = 360/vertices;
	float A1, A2, c, a, b;

	uint8_t i;
	for(i = 0; i < vertices; i++){
		A1 = ((180 - (vertexAngle * i)) / 2);
		A2 = 90 - A1;
		c = 2*radius*cos(_toRadians(A1));
		a = sin(_toRadians(A2)) * c;
		b = sqrt(pow(c, 2)-pow(a, 2));

		if(i >= trunc(vertices/2)+1){
			obj->verteciesLocal[i+vertexStart].x = -b;
		}else{
			obj->verteciesLocal[i+vertexStart].x = b;
		}
		
		obj->verteciesLocal[i+vertexStart].y = a-radius;
		obj->verteciesLocal[i+vertexStart].z = zPos;

		if(i >= (vertices-1)){
			obj->edges[i+edgeStart].P1 = (vertices+vertexStart-1);
			obj->edges[i+edgeStart].P2 = vertexStart;
		}else{
			obj->edges[i+edgeStart].P1 = i+vertexStart;
			obj->edges[i+edgeStart].P2 = i+vertexStart+1;
		}
	}
}

void _initVertexWorldPositions(struct object *obj){
	uint16_t i;
	for(i = 0; i < obj->numOfVertecies; i++){
		obj->verteciesWorld[i].x = obj->verteciesLocal[i].x;
		obj->verteciesWorld[i].y = obj->verteciesLocal[i].y;
		obj->verteciesWorld[i].z = obj->verteciesLocal[i].z;
	}
}

float _toDegrees(float rad){
	return rad*(180/M_PI);
}

float _toRadians(float degrees){
	return M_PI*(degrees/180);
}

// Matrix && Vector operations
void _homogeneusMatrix(float matrix[4][4]){
	// Generate a homogenus unit Matrix
	uint8_t i, j;
	
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			if(i == j){
				matrix[i][j] = 1;
				}else{
				matrix[i][j] = 0;
			}
		}
	}
}

void _rotationMatrix(float matrix[3][3]){
	// Generate a homogenus unit Matrix
	uint8_t i, j;
	
	for(i = 0; i < 3; i++){
		for(j = 0; j < 3; j++){
			if(i == j){
				matrix[i][j] = 1;
				}else{
				matrix[i][j] = 0;
			}
		}
	}
}

void _multHomMatrix(float matrix1[4][4], float matrix2[4][4], float newMatrix[4][4]){
	uint8_t i, j, k;
	float sum = 0;

	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			for(k = 0; k < 4; k++){
				sum += matrix1[i][k] * matrix2[k][j];
			}
			newMatrix[i][j] = sum;
			sum = 0;
		}
	}
}

void _multRotMatrix(float matrix1[3][3], float matrix2[3][3], float newMatrix[3][3]){
	uint8_t i, j, k;
	float sum = 0;

	for(i = 0; i < 3; i++){
		for(j = 0; j < 3; j++){
			for(k = 0; k < 3; k++){
				sum += matrix1[i][k] * matrix2[k][j];
			}
			newMatrix[i][j] = sum;
			sum = 0;
		}
	}
}

void _multHomMatrixVect(float matrix[4][4], struct vector3d vect3d, struct vector3d *newVect3d){
	uint8_t i, j;
	float sum = 0;

	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			switch(j){
				case 0:
					sum += matrix[j][i] * vect3d.x;
					break;
				case 1:
					sum += matrix[j][i] * vect3d.y;
					break;
				case 2:
					sum += matrix[j][i] * vect3d.z;
					break;
				case 3:
					sum += matrix[j][i];
					break;
			}
		}
		switch(i){
			case 0:
				newVect3d->x = sum;
				break;
			case 1:
				newVect3d->y = sum;
				break;
			case 2:
				newVect3d->z = sum;
				break;
			default:
				break;
		}
		
		sum = 0;
	}
}

void _copyMatrix4(float matrixToCopy[4][4], float matrixToPaste[4][4]){
	uint8_t i, j;
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			matrixToPaste[i][j] = matrixToCopy[i][j];
		}
	}
}

void _copyMatrix3(float matrixToCopy[3][3], float matrixToPaste[3][3]){
	uint8_t i, j;
	for(i = 0; i < 3; i++){
		for(j = 0; j < 3; j++){
			matrixToPaste[i][j] = matrixToCopy[i][j];
		}
	}
}

void _setTransformRotation(float transform[4][4], float rotation[3][3]){
	uint8_t i, j;
	for(i = 0; i < 3; i++){
		for(j = 0; j < 3; j++){
			transform[i][j] = rotation[i][j];
		}
	}
}

void printMatrix(float matrix[4][4]){
	uint8_t i, j;
	char tempText[16];

	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			sprintf(tempText, "%f", matrix[i][j]);
			UG_PutString(i*80, j*15, tempText);
		}
	}
}

void printVector3d(struct vector3d vect3d){
	uint8_t i;
	char tempText[16];

	for(i = 0; i < 4; i++){
		switch(i){
			case 0:
			sprintf(tempText, "%f", vect3d.x);
			UG_PutString(0, i*20, tempText);
			break;
			case 1:
			sprintf(tempText, "%f", vect3d.y);
			UG_PutString(0, i*20, tempText);
			break;
			case 2:
			sprintf(tempText, "%f", vect3d.z);
			UG_PutString(0, i*20, tempText);
			break;
			default:
			break;
		}
	}
}