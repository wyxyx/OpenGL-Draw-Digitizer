// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <array>
#include <stack>   
#include <sstream>
#include <iostream>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace glm;
using namespace std;
// Include AntTweakBar
#include <AntTweakBar.h>

#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

const int window_width = 1024, window_height = 768;

typedef struct Vertex {
	float Position[4];
	float Color[4];
	float Normal[3];
	void SetPosition(float *coords) {
		Position[0] = coords[0];
		Position[1] = coords[1];
		Position[2] = coords[2];
		Position[3] = 1.0;
	}
	void SetColor(float *color) {
		Color[0] = color[0];
		Color[1] = color[1];
		Color[2] = color[2];
		Color[3] = color[3];
	}
	void SetNormal(float *coords) {
		Normal[0] = coords[0];
		Normal[1] = coords[1];
		Normal[2] = coords[2];
	}
};

// function prototypes
int initWindow(void);
void initOpenGL(void);
void loadObject(char*, glm::vec4, Vertex * &, GLushort* &, int);
void createVAOs(Vertex[], GLushort[], int);
void createObjects(void);
void pickObject(void);
void renderScene(void);
void cleanup(void);
static void keyCallback(GLFWwindow*, int, int, int, int);
static void mouseCallback(GLFWwindow*, int, int, int);

// GLOBAL VARIABLES
GLFWwindow* window;

glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix;

glm::mat4 baseMatrix;
glm::mat4 tombstoneMatrix;
glm::mat4 arm1Matrix;
glm::mat4 jointMatrix;
glm::mat4 arm2Matrix;
glm::mat4 stylusMatrix;
glm::mat4 stylusMatrix1;
glm::mat4 buttonMatrix;
glm::mat4 ProjectileMatrix;

glm::mat4 ModelMatrix1;

GLuint gPickedIndex = -1;
std::string gMessage;

GLuint programID;
GLuint pickingProgramID;
float pickingColor[10];

const GLuint NumObjects = 10;	// ATTN: THIS NEEDS TO CHANGE AS YOU ADD NEW OBJECTS
GLuint VertexArrayId[NumObjects] = { 0 };
GLuint VertexBufferId[NumObjects] = { 0 };
GLuint IndexBufferId[NumObjects] = { 0 };

size_t NumIndices[NumObjects] = { 0 };
size_t VertexBufferSize[NumObjects] = { 0 };
size_t IndexBufferSize[NumObjects] = { 0 };

GLuint MatrixID;
GLuint ModelMatrixID;
GLuint ViewMatrixID;
GLuint ProjMatrixID;
GLuint PickingMatrixID;
GLuint pickingColorID;
GLuint LightID;
GLuint LightLeftID;
GLuint LightRightID;

GLint gX = 0.0;
GLint gZ = 0.0;

// animation control
bool animation = false;
GLfloat phi = 0.0;

const float PI = 3.1415926;
float startAngle1 = PI / 4;
float startAngle2 = asin(sqrt(3) / 3);

Vertex* Verts_Base;
Vertex* Verts_Tombstone;
Vertex* Verts_Arm1;
Vertex* Verts_Joint;
Vertex* Verts_Arm2;
Vertex* Verts_Stylus;
Vertex* Verts_Button;
Vertex* Verts_Projectile;

GLushort* Idcs_Base;
GLushort* Idcs_Tombstone;
GLushort* Idcs_Arm1;
GLushort* Idcs_Joint;
GLushort* Idcs_Arm2;
GLushort* Idcs_Stylus;
GLushort* Idcs_Button;
GLushort* Idcs_Projectile;

bool transBase = false;
bool transTombstone = false;
bool transArm1 = false;
bool transArm2 = false;
bool transStylus = false;
bool rotateStylus = false;

bool selBase = false;
bool selTombstone = false;
bool selArm1 = false;
bool selArm2 = false;
bool selStylus = false;
bool pressShift = false;

bool moveCameraLEFT = false;
bool moveCameraRIGHT = false;
bool moveCameraUP = false;
bool moveCameraDOWN = false;

bool inCamera = false;
bool selectBase = false;
bool selectTombstone = false;
bool selectArm1 = false;
bool selectArm2 = false;
bool selectStylus = false;

bool moveBaseLEFT = false;
bool moveBaseRIGHT = false;
bool moveBaseUP = false;
bool moveBaseDOWN = false;

bool moveTombLEFT = false;
bool moveTombRIGHT = false;

bool moveArm1UP = false;
bool moveArm1DOWN = false;

bool moveArm2UP = false;
bool moveArm2DOWN = false;

bool moveStylusLEFT = false;
bool moveStylusRIGHT = false;
bool moveStylusUP = false;
bool moveStylusDOWN = false;

bool rotateStylusLEFT = false;
bool rotateStylusRIGHT = false;

bool openLightLeft = false;
bool openLightRight = false;

bool animate = false;
bool selProjectile = false;

glm::vec4 vecColorBase = glm::vec4(0.9, 0.0, 0.3, 1.0);
glm::vec4 vecColorTombstone = glm::vec4(0.1, 0.9, 0.5, 1.0);
glm::vec4 vecColorArm1 = glm::vec4(0.2, 0.4, 0.9, 1.0);
glm::vec4 vecColorJoint = glm::vec4(0.9, 0.5, 1.0, 1.0);
glm::vec4 vecColorArm2 = glm::vec4(0.1, 1.0, 1.0, 1.0);
glm::vec4 vecColorStylus = glm::vec4(1.0, 1.0, 0.0, 1.0);
glm::vec4 vecColorButton = glm::vec4(0.9, 0.0, 0.3, 1.0);
glm::vec4 vecColorProjectile = glm::vec4(1.0,1.0,1.0,1.0);

glm::vec4 startPointStylus = glm::vec4(0.0, 1.4, 0.93, 1.0);
glm::vec4 startPointProjectile = glm::vec4(0.0, 2.55, 2.58, 1.0);
glm::vec4 startPointArm1 = glm::vec4(0.0, 2.58, 0, 1.0);
glm::vec4 startPointArm2 = glm::vec4(0.0,3.6,1.0,1.0);

float t = 0.0;
glm::vec3 coorBase = glm::vec3(0.0,0.0,0.0);
float veclen = sqrt((startPointProjectile.x-startPointStylus.x) * (startPointProjectile.x - startPointStylus.x) + (startPointProjectile.y - startPointStylus.y) * (startPointProjectile.y - startPointStylus.y) + (startPointProjectile.z - startPointStylus.z) * (startPointProjectile.z - startPointStylus.z));
float velocity0 = veclen;

float scopeBaseLEFT = 0.0;
float scopeBaseRIGHT = 0.0;
float scopeBaseUP = 0.0;
float scopeBaseDOWN = 0.0;

std::string instr1;
std::string instr2;
std::string instr3;
std::string instr4;
std::string instr5;
std::string instr6;
std::string instr7;
std::string instr8;
std::string instr9;

void loadObject(char* file, glm::vec4 color, Vertex * &out_Vertices, GLushort* &out_Indices, int ObjectId)
{
	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ(file, vertices, normals);

	std::vector<GLushort> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, normals, indices, indexed_vertices, indexed_normals);

	const size_t vertCount = indexed_vertices.size();
	const size_t idxCount = indices.size();

	// populate output arrays
	out_Vertices = new Vertex[vertCount];
	for (int i = 0; i < vertCount; i++) {
		out_Vertices[i].SetPosition(&indexed_vertices[i].x);
		out_Vertices[i].SetNormal(&indexed_normals[i].x);
		out_Vertices[i].SetColor(&color[0]);
	}
	out_Indices = new GLushort[idxCount];
	for (int i = 0; i < idxCount; i++) {
		out_Indices[i] = indices[i];
	}

	// set global variables!!
	NumIndices[ObjectId] = idxCount;
	VertexBufferSize[ObjectId] = sizeof(out_Vertices[0]) * vertCount;
	IndexBufferSize[ObjectId] = sizeof(GLushort) * idxCount;
}


void createObjects(void)
{
	//-- COORDINATE AXES --//
	Vertex CoordVerts[] =
	{
		{ { 0.0, 0.0, 0.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
		{ { 5.0, 0.0, 0.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
		{ { 0.0, 5.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0 } },
		{ { 0.0, 0.0, 5.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0 } },
	};

	VertexBufferSize[0] = sizeof(CoordVerts);	// ATTN: this needs to be done for each hand-made object with the ObjectID (subscript)
	createVAOs(CoordVerts, NULL, 0);
	
	//-- GRID --//
	
	// ATTN: create your grid vertices here!
	Vertex CoordGridVerts[52];
	int i = 0;
	for (float x = -6; x <= 6; x++)
	{
		CoordGridVerts[i] = { { x, 0.0, -6.0, 1.0 }, { 1.0, 1.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0 } };
		i++;
		CoordGridVerts[i] = { { x, 0.0, 6.0, 1.0 }, { 1.0, 1.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0 } };
		i++;
	}
	for (float z = -6; z <= 6; z++)
	{
		CoordGridVerts[i] = { { -6.0, 0.0, z, 1.0 }, { 1.0, 1.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0 } };
		i++;
		CoordGridVerts[i] = { { 6.0, 0.0, z, 1.0 }, { 1.0, 1.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0 } };
		i++;
	}
	VertexBufferSize[1] = sizeof(CoordGridVerts);	// ATTN: this needs to be done for each hand-made object with the ObjectID (subscript)
	createVAOs(CoordGridVerts, NULL, 1);
	//-- .OBJs --//

	// ATTN: load your models here
	//Vertex* Verts;
	//GLushort* Idcs;
	loadObject("models/Base.obj", vecColorBase , Verts_Base, Idcs_Base, 2);
	//loadObject("models/Base.obj", glm::vec4(1.0, 0.0, 0.0, 1.0), Verts, Idcs, ObjectID);
	createVAOs(Verts_Base, Idcs_Base, 2);

	loadObject("models/Tombstone.obj", vecColorTombstone, Verts_Tombstone, Idcs_Tombstone, 3);
	createVAOs(Verts_Tombstone, Idcs_Tombstone, 3);

	loadObject("models/Arm1.obj", vecColorArm1, Verts_Arm1, Idcs_Arm1, 4);
	createVAOs(Verts_Arm1, Idcs_Arm1, 4);

	loadObject("models/Joint.obj", vecColorJoint, Verts_Joint, Idcs_Joint, 5);
	createVAOs(Verts_Joint, Idcs_Joint, 5);

	loadObject("models/Arm2.obj", vecColorArm2, Verts_Arm2, Idcs_Arm2, 6);
	createVAOs(Verts_Arm2, Idcs_Arm2, 6);

	loadObject("models/Stylus.obj", vecColorStylus, Verts_Stylus, Idcs_Stylus, 7);
	createVAOs(Verts_Stylus, Idcs_Stylus, 7);

	loadObject("models/Button.obj", vecColorButton, Verts_Button, Idcs_Button, 8);
	createVAOs(Verts_Button, Idcs_Button, 8);

	loadObject("models/Projectile.obj", vecColorProjectile, Verts_Projectile, Idcs_Projectile, 9);
	createVAOs(Verts_Projectile, Idcs_Projectile, 9);
}

void renderScene(void)
{
	//ATTN: DRAW YOUR SCENE HERE. MODIFY/ADAPT WHERE NECESSARY!
	for (int i = 0; i < 10; i++)
	{
		pickingColor[i] = i / 255.0f;
	}
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
	// Re-clear the screen for real rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (moveCameraLEFT == true) {
		startAngle1 -= 0.0005f;
	}
	if (moveCameraRIGHT == true) {
		startAngle1 += 0.0005f;
	}
	if (moveCameraUP == true) {
		startAngle2 += 0.0005f;
	}
	if (moveCameraDOWN == true) {
		startAngle2 -= 0.0005f;
	}
	if (inCamera && (moveCameraLEFT || moveCameraRIGHT || moveCameraUP || moveCameraDOWN))
	{
		float cameraRadius = 10 * sqrt(3);
		float cameraXcoord = cameraRadius * cos(startAngle2) * sin(startAngle1);
		float cameraYcoord = cameraRadius * sin(startAngle2);
		float cameraZcoord = cameraRadius * cos(startAngle2) * cos(startAngle1);
		gViewMatrix = glm::lookAt(glm::vec3(cameraXcoord, cameraYcoord, cameraZcoord),	// eye
			glm::vec3(0.0, 0.0, 0.0),	// center
			glm::vec3(0.0, 1.0, 0.0));	// up
	}
	if (selectBase == true) {
		if (selBase == true)
		{
			selBase = false;
			loadObject("models/Base.obj", glm::vec4(1.0, 0.0, 0.0, 1.0), Verts_Base, Idcs_Base, 2);
			createVAOs(Verts_Base, Idcs_Base, 2);
		}

		if (moveBaseLEFT) {
			if (abs(scopeBaseLEFT) < 6.0) {
				baseMatrix = glm::translate(baseMatrix, glm::vec3(-0.001f, 0.0f, 0.0f));
				coorBase += glm::vec3(-0.001f, 0.0f, 0.0f);
				scopeBaseLEFT -= 0.001f;
				transBase = true;
			}
		}
		if (moveBaseRIGHT) {
			if (scopeBaseRIGHT < 6.0) {
				baseMatrix = glm::translate(baseMatrix, glm::vec3(0.001f, 0.0f, 0.0f));
				coorBase += glm::vec3(0.001f, 0.0f, 0.0f);
				scopeBaseRIGHT += 0.001f;
				transBase = true;
			}
		}
		if (moveBaseUP) {
			if (scopeBaseUP < 6.0) {
				baseMatrix = glm::translate(baseMatrix, glm::vec3(0.0f, 0.0f, 0.001f));
				coorBase += glm::vec3(0.0f, 0.0f, 0.001f);
				scopeBaseUP += 0.001f;
				transBase = true;
			}
		}
		if (moveBaseDOWN) {
			if (scopeBaseDOWN < 6.0) {
				baseMatrix = glm::translate(baseMatrix, glm::vec3(0.0f, 0.0f, -0.001f));
				coorBase += glm::vec3(0.0f, 0.0f, -0.001f);
				scopeBaseDOWN -= 0.001f;
				transBase = true;
			}
		}
	}
	else {
		if (selBase == false)
		{
			selBase = true;
			loadObject("models/Base.obj", vecColorBase, Verts_Base, Idcs_Base, 2);
			createVAOs(Verts_Base, Idcs_Base, 2);
		}
	}

	if (selectTombstone == true) {
		if (selTombstone == true)
		{
			selTombstone = false;
			loadObject("models/Tombstone.obj", glm::vec4(0.0, 1.0, 0.0, 1.0), Verts_Tombstone, Idcs_Tombstone, 3);
			createVAOs(Verts_Tombstone, Idcs_Tombstone, 3);
		}
		if (moveTombLEFT) {
			tombstoneMatrix = glm::rotate(tombstoneMatrix, glm::radians(-0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
		    transTombstone = true;
		}
		if (moveTombRIGHT) {
			tombstoneMatrix = glm::rotate(tombstoneMatrix, glm::radians(0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
			transTombstone = true;
		}
		
	}
	else {
		if (selTombstone == false)
		{
			selTombstone = true;
			loadObject("models/Tombstone.obj", vecColorTombstone, Verts_Tombstone, Idcs_Tombstone, 3);
			createVAOs(Verts_Tombstone, Idcs_Tombstone, 3);
		}
	}

	if (selectArm1 == true) {

		glm::vec4 coorArm1 = ModelMatrix1 * startPointArm1;
		glm::vec4 coorArm2 = ModelMatrix1 * startPointArm2;

		float angle = acos(dot(normalize(vec3(coorArm2.x - coorArm1.x, coorArm2.y - coorArm1.y, coorArm2.z - coorArm1.z)), normalize(vec3(coorArm2.x - coorArm1.x, 0.0, coorArm2.z - coorArm1.z))));

		if (selArm1 == true)
		{
			selArm1 = false;
			loadObject("models/Arm1.obj", glm::vec4(0.0, 0.7, 1.0, 1.0), Verts_Arm1, Idcs_Arm1, 4);
			createVAOs(Verts_Arm1, Idcs_Arm1, 4);
		}
		if (moveArm1UP) {
			if (angle < 1.50) {
				arm1Matrix = glm::translate(arm1Matrix, glm::vec3(0.0f, 2.3f, 0.0f));
				arm1Matrix = glm::rotate(arm1Matrix, glm::radians(-0.01f), glm::vec3(1.0f, 0.0f, 0.0f));
				arm1Matrix = glm::translate(arm1Matrix, glm::vec3(0.0f, -2.3f, 0.0f));
				transArm1 = true;
			}
		}
		if (moveArm1DOWN) {
			if (angle > 0.6) {
				arm1Matrix = glm::translate(arm1Matrix, glm::vec3(0.0f, 2.3f, 0.0f));
				arm1Matrix = glm::rotate(arm1Matrix, glm::radians(0.01f), glm::vec3(1.0f, 0.0f, 0.0f));
				arm1Matrix = glm::translate(arm1Matrix, glm::vec3(0.0f, -2.3f, 0.0f));
				transArm1 = true;
			}
		}
	}
	else {
		if (selArm1 == false)
		{
			selArm1 = true;
			loadObject("models/Arm1.obj", vecColorArm1, Verts_Arm1, Idcs_Arm1, 4);
			createVAOs(Verts_Arm1, Idcs_Arm1, 4);
		}
	}

	if (selectArm2 == true) {
		if (selArm2 == true)
		{
			selArm2 = false;
			loadObject("models/Arm2.obj", glm::vec4(0.5, 1.0, 1.0, 1.0), Verts_Arm2, Idcs_Arm2, 6);
			createVAOs(Verts_Arm2, Idcs_Arm2, 6);
		}
		if (moveArm2UP) {
			arm2Matrix = glm::translate(arm2Matrix, glm::vec3(0.02f, 3.5f, 1.2f));
			arm2Matrix = glm::rotate(arm2Matrix, glm::radians(-0.01f), glm::vec3(1.0f, 0.0f, 0.0f));
			arm2Matrix = glm::translate(arm2Matrix, glm::vec3(-0.02f, -3.5f, -1.2f)); 
			transArm2 = true;
		}
		if (moveArm2DOWN) {
			arm2Matrix = glm::translate(arm2Matrix, glm::vec3(0.02f, 3.5f, 1.2f));
			arm2Matrix = glm::rotate(arm2Matrix, glm::radians(0.01f), glm::vec3(1.0f, 0.0f, 0.0f));
			arm2Matrix = glm::translate(arm2Matrix, glm::vec3(-0.02f, -3.5f, -1.2f));
			transArm2 = true;
		}
	}
	else {
		if (selArm2 == false)
		{
			selArm2 = true;
			loadObject("models/Arm2.obj", vecColorArm2, Verts_Arm2, Idcs_Arm2, 6);
			createVAOs(Verts_Arm2, Idcs_Arm2, 6);
		}
	}

	if (selectStylus == true) {
		if (selStylus == true)
		{
			selStylus = false;
			loadObject("models/Stylus.obj", glm::vec4(1.0, 1.0, 0.6, 1.0), Verts_Stylus, Idcs_Stylus, 7);
			createVAOs(Verts_Stylus, Idcs_Stylus, 7);
		}
		if (moveStylusLEFT)
		{
			stylusMatrix = glm::translate(stylusMatrix, glm::vec3(0.0f, 1.55f, 1.0f));
			stylusMatrix = glm::rotate(stylusMatrix, glm::radians(-0.01f), glm::vec3(0.0f, 1.0f, 0.0f));
			stylusMatrix = glm::translate(stylusMatrix, glm::vec3(0.0f, -1.55f, -1.0f));
			transStylus = true;
		}
		if (moveStylusRIGHT)
		{
			stylusMatrix = glm::translate(stylusMatrix, glm::vec3(0.0f, 1.55f, 1.0f));
			stylusMatrix = glm::rotate(stylusMatrix, glm::radians(0.01f), glm::vec3(0.0f, 1.0f, 0.0f));
			stylusMatrix = glm::translate(stylusMatrix, glm::vec3(0.0f, -1.55f, -1.0f));
			transStylus = true;
		}
		if (moveStylusUP)
		{
			stylusMatrix = glm::translate(stylusMatrix, glm::vec3(0.0f, 1.55f, 1.0f));
			stylusMatrix = glm::rotate(stylusMatrix, glm::radians(-0.01f), glm::vec3(1.0f, 0.0f, 0.0f));
			stylusMatrix = glm::translate(stylusMatrix, glm::vec3(0.0f, -1.55f, -1.0f));
			transStylus = true;
		}
		if (moveStylusDOWN)
		{
			stylusMatrix = glm::translate(stylusMatrix, glm::vec3(0.0f, 1.55f, 1.0f));
			stylusMatrix = glm::rotate(stylusMatrix, glm::radians(0.01f), glm::vec3(1.0f, 0.0f, 0.0f));
			stylusMatrix = glm::translate(stylusMatrix, glm::vec3(0.0f, -1.55f, -1.0f));
			transStylus = true;
		}
		// rotate about its own axis 
		if (rotateStylusLEFT)
		{
			stylusMatrix1 = glm::translate(stylusMatrix1, glm::vec3(0.0f, 1.55f, 1.0f));
			stylusMatrix1 = glm::rotate(stylusMatrix1, glm::radians(-0.1f), glm::vec3(0.0f, 0.58f, 1.0f));
			stylusMatrix1 = glm::translate(stylusMatrix1, glm::vec3(0.0f, -1.55f, -1.0f));
			rotateStylus = true;
		}
		if (rotateStylusRIGHT)
		{
			stylusMatrix1 = glm::translate(stylusMatrix1, glm::vec3(0.0f, 1.55f, 1.0f));
			stylusMatrix1 = glm::rotate(stylusMatrix1, glm::radians(0.1f), glm::vec3(0.0f, 0.58f, 1.0f));
			stylusMatrix1 = glm::translate(stylusMatrix1, glm::vec3(0.0f, -1.55f, -1.0f));
			rotateStylus = true;
		}
	}
	else {
		if (selStylus == false)
		{
			selStylus = true;
			loadObject("models/Stylus.obj", vecColorStylus, Verts_Stylus, Idcs_Stylus, 7);
			createVAOs(Verts_Stylus, Idcs_Stylus, 7);
		}
	}
	glUseProgram(programID);
	{
		glm::vec3 lightPos = glm::vec3(4, 4, 4);

		glm::vec3 lightLeftPos = glm::vec3(10, 10.9, 9);
		glm::vec3 lightRightPos = glm::vec3(10.9, 10, 9);

		glm::mat4x4 ModelMatrix = glm::mat4(1.0);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		if (openLightLeft == true)
			glUniform3f(LightLeftID, lightLeftPos.x, lightLeftPos.y, lightLeftPos.z);
		if (openLightRight == true)
			glUniform3f(LightRightID, lightRightPos.x, lightRightPos.y, lightRightPos.z);

		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glBindVertexArray(VertexArrayId[0]);	// draw CoordAxes
		glDrawArrays(GL_LINES, 0, 6);

		glBindVertexArray(VertexArrayId[1]);	// draw grid
		glDrawArrays(GL_LINES, 0, 52);

		//translate base
		if (transBase == true) {
			ModelMatrix = baseMatrix;
		}

		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glBindVertexArray(VertexArrayId[2]);        //draw base
		glDrawElements(GL_TRIANGLES, NumIndices[2], GL_UNSIGNED_SHORT, (void*)0);

		//rotate tombstone
		if (transTombstone == true) {
			ModelMatrix = tombstoneMatrix;
			if (transBase == true) {
				ModelMatrix = baseMatrix * ModelMatrix;
			}
		}
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glBindVertexArray(VertexArrayId[3]);        //draw tombstone
		glDrawElements(GL_TRIANGLES, NumIndices[3], GL_UNSIGNED_SHORT, (void*)0);

		//translate arm1
		if (transArm1 == true) {
			ModelMatrix = arm1Matrix;
			if (transTombstone == true) {
				ModelMatrix = tombstoneMatrix * ModelMatrix;
			}
			if (transBase == true) {
				ModelMatrix = baseMatrix * ModelMatrix;
			}
		}

		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glBindVertexArray(VertexArrayId[4]);        //draw arm1
		glDrawElements(GL_TRIANGLES, NumIndices[4], GL_UNSIGNED_SHORT, (void*)0);


		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glBindVertexArray(VertexArrayId[5]);        //draw Joint
		glDrawElements(GL_TRIANGLES, NumIndices[5], GL_UNSIGNED_SHORT, (void*)0);

		//translate arm2
		if (transArm2 == true) {
			ModelMatrix = arm2Matrix;
			if (transArm1 == true) {
				ModelMatrix = arm1Matrix * ModelMatrix;
			}
			if (transTombstone == true) {
				ModelMatrix = tombstoneMatrix * ModelMatrix;
			}
			if (transBase == true) {
				ModelMatrix = baseMatrix * ModelMatrix;
			}
		}


		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glBindVertexArray(VertexArrayId[6]);        //draw arm2
		glDrawElements(GL_TRIANGLES, NumIndices[6], GL_UNSIGNED_SHORT, (void*)0);

		if (transStylus == true) {
			ModelMatrix = stylusMatrix;
			if (transArm2 == true) {
				ModelMatrix = arm2Matrix * ModelMatrix;
			}
			if (transArm1 == true) {
				ModelMatrix = arm1Matrix * ModelMatrix;
			}
			if (transTombstone == true) {
				ModelMatrix = tombstoneMatrix * ModelMatrix;
			}
			if (transBase == true) {
				ModelMatrix = baseMatrix * ModelMatrix;
			}
		}

		if (rotateStylus == true) {
			ModelMatrix = stylusMatrix1;
			if (transStylus == true) {
				ModelMatrix = stylusMatrix * ModelMatrix;
			}
			if (transArm2 == true) {
				ModelMatrix = arm2Matrix * ModelMatrix;
			}
			if (transArm1 == true) {
				ModelMatrix = arm1Matrix * ModelMatrix;
			}
			if (transTombstone == true) {
				ModelMatrix = tombstoneMatrix * ModelMatrix;
			}
			if (transBase == true) {
				ModelMatrix = baseMatrix * ModelMatrix;
			}
		}
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glBindVertexArray(VertexArrayId[7]);        //draw Stylus
		glDrawElements(GL_TRIANGLES, NumIndices[7], GL_UNSIGNED_SHORT, (void*)0);

		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glBindVertexArray(VertexArrayId[8]);        //draw Button
		glDrawElements(GL_TRIANGLES, NumIndices[8], GL_UNSIGNED_SHORT, (void*)0);

		//move projectile 
		if (animate == true)
		{
			glm::vec4 coorProjectile = ModelMatrix * startPointProjectile;
			glm::vec4 coorStylus = ModelMatrix * startPointStylus;
			glm::vec3 xaxis = glm::vec3(1, 0, 0);
			glm::vec3 yaxis = glm::vec3(0, 1, 0);
			glm::vec3 zaxis = glm::vec3(0, 0, 1);
			float anglex = acos(dot(normalize(vec3(coorProjectile.x-coorStylus.x, 0, coorProjectile.z - coorStylus.z)), normalize(xaxis)));
			float anglez = acos(dot(normalize(vec3(coorProjectile.x - coorStylus.x, 0, coorProjectile.z-coorStylus.z)), normalize(zaxis)));
			float theta = acos(dot(normalize(vec3(coorProjectile.x - coorStylus.x, 0, coorProjectile.z - coorStylus.z)), normalize(vec3(coorProjectile.x - coorStylus.x, coorProjectile.y - coorStylus.y, coorProjectile.z- coorStylus.z))));

			float disY = velocity0 * sin(theta) * t - 9.8 / 2 * t * t;
			glm::vec3 vecP;
			float dt = 0.001;

			if (coorProjectile.y + disY > 0.0)
			{
				t = t + dt;
				float dx = velocity0 * cos(theta) * dt * cos(anglex);
				float dz = velocity0 * cos(theta) * dt * cos(anglez);
				float dy = velocity0 * sin(theta) * dt - 9.8/2 * t * t + 9.8/2*(t-dt)*(t-dt);
				vecP = glm::vec3(dx, dy, dz);
				ProjectileMatrix = glm::translate(ProjectileMatrix, vecP);
			}
			else {
				animate = false;
				float endx = velocity0 * cos(theta) * t * cos(anglex) + coorProjectile.x;
				float endz = velocity0 * cos(theta) * t * cos(anglez) + coorProjectile.z;
				glm::vec3 coorend = glm::vec3(endx,0.0,endz);
				
				glm::vec3 coortrans = coorend - coorBase;

				baseMatrix= glm::translate(baseMatrix, coortrans);
				transBase = true;
			}
			//!! first rotate then translate
			// consider about the translation of ProjectileMatrix in the last step
			ModelMatrix = ProjectileMatrix * ModelMatrix;
		}
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glBindVertexArray(VertexArrayId[9]);        //draw projectile 
		glDrawElements(GL_TRIANGLES, NumIndices[9], GL_UNSIGNED_SHORT, (void*)0);

		ModelMatrix1 = ModelMatrix;
		glBindVertexArray(0);

	}
	glUseProgram(0);
	// Draw GUI
	TwDraw();

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void pickObject(void)
{
	// Clear the screen in white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(pickingProgramID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		
		// ATTN: DRAW YOUR PICKING SCENE HERE. REMEMBER TO SEND IN A DIFFERENT PICKING COLOR FOR EACH OBJECT BEFOREHAND
		//translate base
		if (transBase == true) {
			ModelMatrix = baseMatrix;
		}
		MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;
		glUniform1f(pickingColorID, pickingColor[2]);
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glBindVertexArray(VertexArrayId[2]);        //draw base
		glDrawElements(GL_TRIANGLES, NumIndices[2], GL_UNSIGNED_SHORT, (void*)0);

		//rotate tombstone
		if (transTombstone == true) {
			ModelMatrix = tombstoneMatrix;
			if (transBase == true) {
				ModelMatrix = baseMatrix * ModelMatrix;
			}
		}
		MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;
		glUniform1f(pickingColorID, pickingColor[3]);
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glBindVertexArray(VertexArrayId[3]);        //draw tombstone
		glDrawElements(GL_TRIANGLES, NumIndices[3], GL_UNSIGNED_SHORT, (void*)0);

		//translate arm1
		if (transArm1 == true) {
			ModelMatrix = arm1Matrix;
			if (transTombstone == true) {
				ModelMatrix = tombstoneMatrix * ModelMatrix;
			}
			if (transBase == true) {
				ModelMatrix = baseMatrix * ModelMatrix;
			}
		}
		MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;
		glUniform1f(pickingColorID, pickingColor[4]);
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glBindVertexArray(VertexArrayId[4]);        //draw arm1
		glDrawElements(GL_TRIANGLES, NumIndices[4], GL_UNSIGNED_SHORT, (void*)0);

		MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;
		glUniform1f(pickingColorID, pickingColor[5]);
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glBindVertexArray(VertexArrayId[5]);        //draw Joint
		glDrawElements(GL_TRIANGLES, NumIndices[5], GL_UNSIGNED_SHORT, (void*)0);

		//translate arm2
		if (transArm2 == true) {
			ModelMatrix = arm2Matrix;
			if (transArm1 == true) {
				ModelMatrix = arm1Matrix * ModelMatrix;
			}
			if (transTombstone == true) {
				ModelMatrix = tombstoneMatrix * ModelMatrix;
			}
			if (transBase == true) {
				ModelMatrix = baseMatrix * ModelMatrix;
			}
		}

		MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;
		glUniform1f(pickingColorID, pickingColor[6]);
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glBindVertexArray(VertexArrayId[6]);        //draw arm2
		glDrawElements(GL_TRIANGLES, NumIndices[6], GL_UNSIGNED_SHORT, (void*)0);

		if (transStylus == true) {
			ModelMatrix = stylusMatrix;
			if (transArm2 == true) {
				ModelMatrix = arm2Matrix * ModelMatrix;
			}
			if (transArm1 == true) {
				ModelMatrix = arm1Matrix * ModelMatrix;
			}
			if (transTombstone == true) {
				ModelMatrix = tombstoneMatrix * ModelMatrix;
			}
			if (transBase == true) {
				ModelMatrix = baseMatrix * ModelMatrix;
			}
		}

		if (rotateStylus == true) {
			ModelMatrix = stylusMatrix1;
			if (transStylus == true) {
				ModelMatrix = stylusMatrix * ModelMatrix;
			}
			if (transArm2 == true) {
				ModelMatrix = arm2Matrix * ModelMatrix;
			}
			if (transArm1 == true) {
				ModelMatrix = arm1Matrix * ModelMatrix;
			}
			if (transTombstone == true) {
				ModelMatrix = tombstoneMatrix * ModelMatrix;
			}
			if (transBase == true) {
				ModelMatrix = baseMatrix * ModelMatrix;
			}

		}
		
		MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;
		glUniform1f(pickingColorID, pickingColor[7]);
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glBindVertexArray(VertexArrayId[7]);        //draw Stylus
		glDrawElements(GL_TRIANGLES, NumIndices[7], GL_UNSIGNED_SHORT, (void*)0);

		MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;
		glUniform1f(pickingColorID, pickingColor[8]);
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glBindVertexArray(VertexArrayId[8]);        //draw Button
		glDrawElements(GL_TRIANGLES, NumIndices[8], GL_UNSIGNED_SHORT, (void*)0);
		
		if (animate == true) {
			ModelMatrix = ProjectileMatrix * ModelMatrix;
		}
		MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;
		glUniform1f(pickingColorID, pickingColor[9]);
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glBindVertexArray(VertexArrayId[9]);        //draw projectile 
		glDrawElements(GL_TRIANGLES, NumIndices[9], GL_UNSIGNED_SHORT, (void*)0);

		ModelMatrix1 = ModelMatrix;
		glBindVertexArray(0);
	}
	glUseProgram(0);
	// Wait until all the pending drawing commands are really done.
	// Ultra-mega-over slow ! 
	// There are usually a long time between glDrawElements() and
	// all the fragments completely rasterized.
	glFlush();
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Read the pixel at the center of the screen.
	// You can also use glfwGetMousePos().
	// Ultra-mega-over slow too, even for 1 pixel, 
	// because the framebuffer is on the GPU.
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	unsigned char data[4];
	glReadPixels(xpos, window_height - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

	// Convert the color back to an integer ID
	gPickedIndex = int(data[0]);
	
	if (gPickedIndex == 255){ // Full white, must be the background !
		gMessage = "background";
	}
	else {
		std::ostringstream oss;
		oss << "point " << gPickedIndex;
		gMessage = oss.str();
		//Base
		if (gPickedIndex == 2) {
			if (selectBase == false) {
				selectBase = true;
			}
			else {
				selectBase = false;
			}
		}
		//Tombstone
		if (gPickedIndex == 3) {
			if (selectTombstone == false)
			{
				selectTombstone = true;
			}
			else
			{
				selectTombstone = false;
			}
		}
		//Arm1
		if (gPickedIndex == 4) {
			if (selectArm1 == false)
			{
				selectArm1 = true;
			}
			else
			{
				selectArm1 = false;
			}
		}
		//Arm2
		if (gPickedIndex == 6) {
			if (selectArm2 == false)
			{
				selectArm2 = true;
			}
			else
			{
				selectArm2 = false;
			}
		}
		//Stylus
		if (gPickedIndex == 7) {
			if (selectStylus == false)
			{
				selectStylus = true;
			}
			else {
				selectStylus = false;
			}
		}
	}

	// Uncomment these lines to see the picking shader in effect
	//glfwSwapBuffers(window);
	//continue; // skips the normal rendering
}

int initWindow(void)
{
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(window_width, window_height, "Wei,Yixin(5114-6181)", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(window_width, window_height);
	TwBar * GUI = TwNewBar("Picking");
	TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &gMessage, NULL);

	TwBar* InforUI = TwNewBar("Instru");
	TwDefine(" Instru position='20 400' ");
	TwDefine(" Instru size='250 200' ");
	TwSetParam(InforUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");

	instr1 = "Press C";
	TwAddVarRW(InforUI, "Camera:", TW_TYPE_STDSTRING, &instr1, NULL);
	instr2 = "Press B";
	TwAddVarRW(InforUI, "Base:", TW_TYPE_STDSTRING, &instr2, NULL);
	instr3 = "Press T";
	TwAddVarRW(InforUI, "Tombstone:", TW_TYPE_STDSTRING, &instr3, NULL);
	instr4 = "Press 1";
	TwAddVarRW(InforUI, "Arm1:", TW_TYPE_STDSTRING, &instr4, NULL);
	instr5 = "Press 2";
	TwAddVarRW(InforUI, "Arm2:", TW_TYPE_STDSTRING, &instr5, NULL);
	instr6 = "Press S";
	TwAddVarRW(InforUI, "Stylus:", TW_TYPE_STDSTRING, &instr6, NULL);
	instr7 = "Press P";
	TwAddVarRW(InforUI, "Animation:", TW_TYPE_STDSTRING, &instr7, NULL);
	instr8 = "Press L";
	TwAddVarRW(InforUI, "Left Light:", TW_TYPE_STDSTRING, &instr8, NULL);
	instr9 = "Press R";
	TwAddVarRW(InforUI, "Right Light:", TW_TYPE_STDSTRING, &instr9, NULL);
	// Set up inputs
	glfwSetCursorPos(window, window_width / 2, window_height / 2);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseCallback);

	return 0;
}

void initOpenGL(void)
{

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	gProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	//gProjectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	gViewMatrix = glm::lookAt(glm::vec3(10.0, 10.0, 10.0f),	// eye
		glm::vec3(0.0, 0.0, 0.0),	// center
		glm::vec3(0.0, 1.0, 0.0));	// up

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("p2_StandardShading.vertexshader", "p2_StandardShading.fragmentshader");
	pickingProgramID = LoadShaders("p2_Picking.vertexshader", "p2_Picking.fragmentshader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ProjMatrixID = glGetUniformLocation(programID, "P");
	
	PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
	// Get a handle for our "pickingColorID" uniform
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");
	// Get a handle for our "LightPosition" uniform
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	LightLeftID = glGetUniformLocation(programID, "LightLeftPosition_worldspace");
	LightRightID = glGetUniformLocation(programID, "LightRightPosition_worldspace");

	createObjects();
}

void createVAOs(Vertex Vertices[], unsigned short Indices[], int ObjectId) {

	GLenum ErrorCheckValue = glGetError();
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].Position);
	const size_t Normaloffset = sizeof(Vertices[0].Color) + RgbOffset;

	// Create Vertex Array Object
	glGenVertexArrays(1, &VertexArrayId[ObjectId]);	
	glBindVertexArray(VertexArrayId[ObjectId]);		

	// Create Buffer for vertex data
	glGenBuffers(1, &VertexBufferId[ObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[ObjectId]);
	glBufferData(GL_ARRAY_BUFFER, VertexBufferSize[ObjectId], Vertices, GL_STATIC_DRAW);

	// Create Buffer for indices
	if (Indices != NULL) {
		glGenBuffers(1, &IndexBufferId[ObjectId]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[ObjectId]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferSize[ObjectId], Indices, GL_STATIC_DRAW);
	}

	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset); 
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)Normaloffset);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color
	glEnableVertexAttribArray(2);	// normal

	// Disable our Vertex Buffer Object 
	glBindVertexArray(0);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n",
			gluErrorString(ErrorCheckValue)
			);
	}
}

void cleanup(void)
{
	// Cleanup VBO and shader
	for (int i = 0; i < NumObjects; i++) {
		glDeleteBuffers(1, &VertexBufferId[i]);
		glDeleteBuffers(1, &IndexBufferId[i]);
		glDeleteVertexArrays(1, &VertexArrayId[i]);
	}
	glDeleteProgram(programID);
	glDeleteProgram(pickingProgramID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// ATTN: MODIFY AS APPROPRIATE
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_A:
			break;
		case GLFW_KEY_B:
			if (selectBase == false) {
				selectBase = true;
			}
			else {
				selectBase = false;
			}
			break;
		case GLFW_KEY_C:
			if (inCamera == false)
			{
				inCamera = true;
			}
			else
			{
				inCamera = false;
			}
			break;
		case GLFW_KEY_D:
			break;
		case GLFW_KEY_W:
			break;
		case GLFW_KEY_L:
			if (openLightLeft == false) {
				openLightLeft = true;
			}
			else {
				openLightLeft = false;
			}
			break;
		case GLFW_KEY_P:
			if (animate == false) {
				animate = true;
			}
			else {
				animate = false;
			}
			break;
		case GLFW_KEY_R:
			if (openLightRight == false) {
				openLightRight = true;
			}
			else {
				openLightRight = false;
			}
			break;
		case GLFW_KEY_S:
			if (selectStylus == false)
			{
				selectStylus = true;
			}
			else {
				selectStylus = false;
			}
			break;
		case GLFW_KEY_T:
			if (selectTombstone == false)
			{
				selectTombstone = true;
			}
			else
			{
				selectTombstone = false;
			}
			break;
		case GLFW_KEY_SPACE:
			break;
		case GLFW_KEY_1:
			if (selectArm1 == false)
			{
				selectArm1 = true;
			}
			else
			{
				selectArm1 = false;
			}
			break;
		case GLFW_KEY_2:
			if (selectArm2 == false)
			{
				selectArm2 = true;
			}
			else
			{
				selectArm2 = false;
			}
			break;
		case GLFW_KEY_LEFT:
			if (inCamera == true) {
				moveCameraLEFT = true;
			}
			if (selectBase == true) {
				moveBaseLEFT = true;
			}
			if (selectTombstone == true) {
				moveTombLEFT = true;
			}
			if (selectStylus == true) {
				if (pressShift == true)
				{
					rotateStylusLEFT = true;
				}
				else {
					moveStylusLEFT = true;
				}
			}
			break;
		case GLFW_KEY_RIGHT:
			if (inCamera == true) {
				moveCameraRIGHT = true;
			}
			if (selectBase == true) {
				moveBaseRIGHT = true;
			}
			if (selectTombstone == true) {
				moveTombRIGHT = true;
			}
			if (selectStylus == true) {
				if (pressShift == true)
				{
					rotateStylusRIGHT = true;
				}
				else {
					moveStylusRIGHT = true;
				}
			}
			break;
		case GLFW_KEY_UP:
			if (inCamera == true) {
				moveCameraUP = true;
			}
			if (selectBase == true) {
				moveBaseUP = true;
			}
			if (selectArm1 == true) {
				moveArm1UP = true;
			}
			if (selectArm2 == true) {
				moveArm2UP = true;
			}
			if (selectStylus == true) {
				moveStylusUP = true;
			}
			break;
		case GLFW_KEY_DOWN:
			if (inCamera == true) {
				moveCameraDOWN = true;
			}
			if (selectBase == true) {
				moveBaseDOWN = true;
			}
			if (selectArm1 == true) {
				moveArm1DOWN = true;
			}
			if (selectArm2 == true) {
				moveArm2DOWN = true;
			}
			if (selectStylus == true) {
				moveStylusDOWN = true;
			}
			break;
		case GLFW_KEY_LEFT_SHIFT:
			pressShift = true;
			break;
		case GLFW_KEY_RIGHT_SHIFT:
			pressShift = true;
			break;
		default:
			break;
		}
	}
	else if (action == GLFW_RELEASE) {
		switch (key)
		{
		case GLFW_KEY_LEFT:
			if (inCamera == true) {
				moveCameraLEFT = false;
			}
			if (selectBase == true) {
				moveBaseLEFT = false;
			}
			if (selectTombstone == true) {
				moveTombLEFT = false;
			}
			if (selectStylus == true) {
				rotateStylusLEFT = false;
				moveStylusLEFT = false;
			}
			break;
		case GLFW_KEY_RIGHT:
			if (inCamera == true) {
				moveCameraRIGHT = false;
			}
			if (selectBase == true) {
				moveBaseRIGHT = false;
			}
			if (selectTombstone == true) {
				moveTombRIGHT = false;
			}
			if (selectStylus == true) {
				moveStylusRIGHT = false;
				rotateStylusRIGHT = false;
			}
			break;
		case GLFW_KEY_UP:
			if (inCamera == true) {
				moveCameraUP = false;
			}
			if (selectBase == true) {
				moveBaseUP = false;
			}
			if (selectArm1 == true) {
				moveArm1UP = false;
			}
			if (selectArm2 == true) {
				moveArm2UP = false;
			}
			if (selectStylus == true) {
				moveStylusUP = false;
			}
			break;
		case GLFW_KEY_DOWN:
			if (inCamera == true) {
				moveCameraDOWN = false;
			}
			if (selectBase == true) {
				moveBaseDOWN = false;
			}
			if (selectArm1 == true) {
				moveArm1DOWN = false;
			}
			if (selectArm2 == true) {
				moveArm2DOWN = false;
			}
			if (selectStylus == true) {
				moveStylusDOWN = false;
			}
			break;
		case GLFW_KEY_LEFT_SHIFT:
			pressShift = false;
			break;
		case GLFW_KEY_RIGHT_SHIFT:
			pressShift = false;
			break;
		default:
			break;
		}
	}
}

static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		pickObject();
	}
}

int main(void)
{
	// initialize window
	int errorCode = initWindow();
	if (errorCode != 0)
		return errorCode;

	// initialize OpenGL pipeline
	initOpenGL();

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	do {
		//// Measure speed
		//double currentTime = glfwGetTime();
		//nbFrames++;
		//if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
		//	// printf and reset
		//	printf("%f ms/frame\n", 1000.0 / double(nbFrames));
		//	nbFrames = 0;
		//	lastTime += 1.0;
		//}
		
		if (animation){
			phi += 0.01;
			if (phi > 360)
				phi -= 360;
		}

		// DRAWING POINTS
		renderScene();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	cleanup();

	return 0;
}