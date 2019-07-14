
//  Display a rotating cube with lighting
//
//  Light and material properties are sent to the shader as uniform
//    variables.  Vertex positions and normals are sent after each
//    rotation.

#include "Angel.h"
#include <gl/glut.h>
#include <Windows.h>

#define GL_PI 3.1415f
GLuint program;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
// Menu option values
const int  Quit = 4;






// Shader transformation matrices
mat4  model_view;

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };


// Light parameters
point4 light_position(0.0, 0.0, -1.0, 0.0);
color4 light_ambient(0.5, 0.5, 0.5, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(0.0, 1.0, 0.0, 1.0);
// Parameters controlling the size of the Robot's arm
const GLfloat BASE_HEIGHT = 2.0;
const GLfloat BASE_WIDTH = 5.0;
const GLfloat LOWER_ARM_HEIGHT = 5.0;
const GLfloat LOWER_ARM_WIDTH = 0.5;
const GLfloat UPPER_ARM_HEIGHT = 5.0;
const GLfloat UPPER_ARM_WIDTH = 0.5;


extern const int cubeNumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
extern point4 cubePoints[cubeNumVertices];
extern vec3   cubeNormals[cubeNumVertices];
extern void cube();


extern const int coneSlices = 20;
extern const int coneNumVertices = 3 * coneSlices;
extern point4 conePoints[coneNumVertices];
extern vec3   coneNormals[coneNumVertices];
extern void cone(int slices);

extern const int shpereSlices = 20;
extern const int shpereStacks = 20;
extern const int sphereNumVertices = 3 * (2 * shpereSlices + 2 * shpereSlices * (shpereStacks - 2));

extern point4 spherePoints[sphereNumVertices];
extern vec3   sphereNormals[sphereNumVertices];
extern void sphere(int slices, int stacks);

const unsigned int cubeOffset = 0;
const unsigned int coneOffset = sizeof(cubePoints) + sizeof(cubeNormals);
const unsigned int sphereOffset = sizeof(cubePoints) + sizeof(cubeNormals) + +sizeof(conePoints) + sizeof(coneNormals);

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	cube();
	cone(coneSlices);
	sphere(shpereSlices, shpereStacks);


	// Load shaders and use the resulting shader program
	program = InitShader("vshader53.glsl", "fshader53.glsl");
	glUseProgram(program);


	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(cubePoints) + sizeof(cubeNormals)
		+ sizeof(conePoints) + sizeof(coneNormals)
		+ sizeof(spherePoints) + sizeof(sphereNormals),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, cubeOffset, sizeof(cubePoints), cubePoints);
	glBufferSubData(GL_ARRAY_BUFFER, cubeOffset + sizeof(cubePoints), sizeof(cubeNormals), cubeNormals);
	glBufferSubData(GL_ARRAY_BUFFER, coneOffset, sizeof(conePoints), conePoints);
	glBufferSubData(GL_ARRAY_BUFFER, coneOffset + sizeof(conePoints), sizeof(coneNormals), coneNormals);
	glBufferSubData(GL_ARRAY_BUFFER, sphereOffset, sizeof(spherePoints), spherePoints);
	glBufferSubData(GL_ARRAY_BUFFER, sphereOffset + sizeof(spherePoints), sizeof(sphereNormals), sphereNormals);


	// Retrieve transformation uniform variable locations
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");

	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(1.0, 1.0, 1.0, 1.0);
}





// Array of rotation angles (in degrees) for each rotation axis
enum { Base = 0, LowerArm = 1, UpperArm = 2, NumAngles = 3 };
int      Axis1 = Base;
//GLfloat  Theta[NumAngles] = { 0.0 };

//----------------------------------------------------------------------------
void
base()
{
	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cubePoints)));

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 material_specular(1.0, 0.8, 0.0, 1.0);
	float  material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	mat4 instance = (Translate(0.0, 0.5 * BASE_HEIGHT, 0.0) *
		Scale(BASE_WIDTH,
			BASE_HEIGHT,
			BASE_WIDTH));

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);

	glDrawArrays(GL_TRIANGLES, 0, cubeNumVertices);
}

//----------------------------------------------------------------------------

void
upper_arm()
{
	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cubePoints)));

	color4 material_ambient(1.0, 1.0, 0.0, 1.0);
	color4 material_diffuse(1.0, 0.0, 0.8, 1.0);
	color4 material_specular(1.0, 1.0, 0.8, 2.0);
	float  material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) *
		Scale(UPPER_ARM_WIDTH,
			UPPER_ARM_HEIGHT,
			UPPER_ARM_WIDTH));

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, cubeNumVertices);
}

//----------------------------------------------------------------------------

void
lower_arm()
{
	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cubePoints)));

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 material_specular(1.0, 0.8, 0.0, 1.0);
	float  material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) *
		Scale(LOWER_ARM_WIDTH,
			LOWER_ARM_HEIGHT,
			LOWER_ARM_WIDTH));

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, cubeNumVertices);
}

void
robot1()
{
	model_view *= RotateY(30);
	base();

	model_view *= (Translate(0.0, BASE_HEIGHT, 0.0) *
		RotateZ(Theta[LowerArm]));
	lower_arm();

	model_view *= (Translate(0.0, LOWER_ARM_HEIGHT, 0.0) *
		RotateZ(Theta[UpperArm]));
	upper_arm();
}

//----------------------------------------------------------------------------
void
cube1(color4 material_ambient,
	color4 material_diffuse,
	color4 material_specular, float material_shininess)
{
	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cubeOffset));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cubeOffset + sizeof(cubePoints)));

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);


	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);

	glDrawArrays(GL_TRIANGLES, 0, cubeNumVertices);
}

//----------------------------------------------------------------------------
void
cone1(color4 material_ambient,
	color4 material_diffuse,
	color4 material_specular, float material_shininess)
{
	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(coneOffset));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(coneOffset + sizeof(conePoints)));

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);


	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);

	glDrawArrays(GL_TRIANGLES, 0, coneNumVertices);
}

//----------------------------------------------------------------------------
void
sphere1(color4 material_ambient,
	color4 material_diffuse,
	color4 material_specular, float material_shininess,
	GLenum Mode = GL_TRIANGLES)
{
	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sphereOffset));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sphereOffset + sizeof(spherePoints)));

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);


	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);

	glDrawArrays(Mode, 0, sphereNumVertices);
}

void
ball()
{
	static vec3 pos(-3.0, 2.0, 0.0);
	static vec3 vel(0.0, 0.0, 0.0);
	static vec3 acc(3.0, 9.0, 0.0);
	//const vec3 grv(0.0, -9.8, 0.0);
	const float deltaT = 0.001;

	//update acc, vel, pos
//	acc += grv * deltaT;
	vel += acc * deltaT;
	pos += vel * deltaT;

	model_view *= Translate(pos);
	sphere1(color4(1.0, 0.0, 1.0, 1.0),
		color4(1.0, 0.8, 0.0, 1.0),
		color4(1.0, 0.8, 0.0, 1.0), 100.0, GL_LINES);
}
void
ball2()
{
	static vec3 pos(-3.0, 1.0, 0.0);
	static vec3 vel(0.0, 0.0, 0.0);
	static vec3 acc(3.0, 9.0, 0.0);
	//const vec3 grv(0.0, -9.8, 0.0);
	const float deltaT = 0.003;

	//update acc, vel, pos
//	acc += grv * deltaT;
	vel += acc * deltaT;
	pos += vel * deltaT;

	model_view *= Translate(pos);
	sphere1(color4(1.0, 0.0, 1.0, 1.0),
		color4(1.0, 0.8, 0.0, 1.0),
		color4(1.0, 0.8, 0.0, 1.0), 100.0, GL_LINES);
}
void
ball3()
{
	static vec3 pos(-3.0, 1.0, 0.0);
	static vec3 vel(0.0, 0.0, 0.0);
	static vec3 acc(3.0, 9.0, 0.0);
	//const vec3 grv(0.0, -9.8, 0.0);
	const float deltaT = 0.005;

	//update acc, vel, pos
//	acc += grv * deltaT;
	vel += acc * deltaT;
	pos += vel * deltaT;

	model_view *= Translate(pos);
	sphere1(color4(1.0, 0.0, 1.0, 1.0),
		color4(1.0, 0.8, 0.0, 1.0),
		color4(1.0, 0.8, 0.0, 1.0), 100.0, GL_LINES);
}
void
ball4()
{
	static vec3 pos(-3.0, 1.0, 0.0);
	static vec3 vel(0.0, 0.0, 0.0);
	static vec3 acc(3.0, 9.0, 0.0);
	//const vec3 grv(0.0, -9.8, 0.0);
	const float deltaT = 0.007;

	//update acc, vel, pos
//	acc += grv * deltaT;
	vel += acc * deltaT;
	pos += vel * deltaT;

	model_view *= Translate(pos);
	sphere1(color4(0.7, 0.2, 0.5, 1.0),
		color4(1.0, 0.8, 0.0, 1.0),
		color4(1.0, 0.8, 0.0, 1.0), 100.0, GL_LINES);
}
void
ball5()
{
	static vec3 pos(-3.0, 1.0, 0.0);
	static vec3 vel(0.0, 0.0, 0.0);
	static vec3 acc(3.0, 9.0, 0.0);
	//const vec3 grv(0.0, -9.8, 0.0);
	const float deltaT = 0.0001;

	//update acc, vel, pos
//	acc += grv * deltaT;
	vel += acc * deltaT;
	pos += vel * deltaT;

	model_view *= Translate(pos);
	sphere1(color4(0.3, 0.8, 0.3, 1.0),
		color4(1.0, 0.8, 0.0, 1.0),
		color4(1.0, 0.8, 0.0, 1.0), 100.0, GL_LINES);
}
void
ball6()
{
	static vec3 pos(-3.0, 1.0, 0.0);
	static vec3 vel(0.0, 0.0, 0.0);
	static vec3 acc(3.0, 9.0, 0.0);
	//const vec3 grv(0.0, -9.8, 0.0);
	const float deltaT = 0.0003;

	//update acc, vel, pos
//	acc += grv * deltaT;
	vel += acc * deltaT;
	pos += vel * deltaT;

	model_view *= Translate(pos);
	sphere1(color4(0.5, 0.5, 0.8, 1.0),
		color4(1.0, 0.8, 0.0, 1.0),
		color4(0.5, 0.0, 0.0, 1.0), 100.0, GL_LINES);
}
void
ball7()
{
	static vec3 pos(-3.0, 1.0, 0.0);
	static vec3 vel(0.0, 0.0, 0.0);
	static vec3 acc(3.0, 9.0, 0.0);
	//const vec3 grv(0.0, -9.8, 0.0);
	const float deltaT = 0.0005;

	//update acc, vel, pos
//	acc += grv * deltaT;
	vel += acc * deltaT;
	pos += vel * deltaT;

	model_view *= Translate(pos);
	sphere1(color4(0.1, 0.5, 0.3, 1.0),
		color4(0.7, 0.8, 0.3, 1.0),
		color4(1.0, 0.8, 0.0, 1.0), 100.0, GL_LINES);
}
void
ball8()
{
	static vec3 pos(-3.0, 1.0, 0.0);
	static vec3 vel(0.0, 0.0, 0.0);
	static vec3 acc(3.0, 9.0, 0.0);
	//const vec3 grv(0.0, -9.8, 0.0);
	const float deltaT = 0.0009;

	//update acc, vel, pos
//	acc += grv * deltaT;
	vel += acc * deltaT;
	pos += vel * deltaT;

	model_view *= Translate(pos);
	sphere1(color4(0.1, 0.3, 0.6, 1.0),
		color4(0.3, 0.2, 0.5, 1.0),
		color4(0.2, 0.1, 0.8, 1.0), 100.0, GL_LINES);
}
//----------------------------------------------------------------------------
void
ball9()
{
	static vec3 pos(-3.0, 1.0, 0.0);
	static vec3 vel(0.0, 0.0, 0.0);
	static vec3 acc(3.0, 9.0, 0.0);
	//const vec3 grv(0.0, -9.8, 0.0);
	const float deltaT = 0.0009;

	//update acc, vel, pos
//	acc += grv * deltaT;
	vel += acc * deltaT;
	pos += vel * deltaT;

	model_view *= Translate(pos);
	sphere1(color4(0.1, 0.2, 0.3, 1.0),
		color4(0.1, 0.5, 0.9, 1.0),
		color4(0.4, 0.3, 0.6, 1.0), 100.0, GL_LINES);
}
void
ball10()
{
	static vec3 pos(-3.0, 1.0, 0.0);
	static vec3 vel(0.0, 0.0, 0.0);
	static vec3 acc(3.0, 9.0, 0.0);
	//const vec3 grv(0.0, -9.8, 0.0);
	const float deltaT = 0.0009;

	//update acc, vel, pos
//	acc += grv * deltaT;
	vel += acc * deltaT;
	pos += vel * deltaT;

	model_view *= Translate(pos);
	sphere1(color4(0.6, 1.0, 0.4, 1.0),
		color4(0.9, 0.1, 0.6, 1.0),
		color4(0.4, 0.3, 0.6, 1.0), 100.0, GL_LINES);
}


void
display(void)
{
	glClearColor(0.75, 0.75, 0.75, 1.0);  //배경색
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//  Generate tha model-view matrixn

	glShadeModel(GL_SMOOTH);                              // 매끄러운 세이딩 사용
	glEnable(GL_DEPTH_TEST);                              // 가려진 면 제거
	glEnable(GL_CULL_FACE);                               // 후면 제거
	glEnable(GL_LIGHTING);                                // 조명 활성화

	   // LIGHT0 에 대한 재질을 설정
	glMaterialfv(GL_FRONT, GL_AMBIENT, light_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, light_diffuse);

	// LIGHT0 설정
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);       // 주변광 성분 설정
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);       // 분산광 성분 설정
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);     // 광원 위치 설정

	// LIGHT0을 켠다.
	glEnable(GL_LIGHT0);

	const vec3 viewer_pos(0.0, 0.0, 2.0);
	model_view = (Translate(-viewer_pos) *
		RotateX(Theta[Xaxis]) *
		RotateY(Theta[Yaxis]) *
		RotateZ(Theta[Zaxis]));
	mat4 model_view_sv = model_view;

	//model_view *= RotateX(90);
	//upper_arm();
	model_view = model_view_sv;
	model_view *= (Translate(-3.0, 0.0, 0.0)*Scale(0.7, 0.7, 1.0));  //자전거 바퀴 
	sphere1(color4(0.2, 0.1, 1.0, 1.0),
		color4(1.0, 0.5, 0.5, 1.0),
		color4(1.0, 0.5, 0.5, 1.0), 100.0, GL_LINES);

	model_view = model_view_sv;
	model_view *= (Translate(-3.0, 3.0, 0.0)*Scale(0.7, 0.7, 1.0));  //자전거 바퀴 
	sphere1(color4(0.4, 0.3, 1.0, 1.0),
		color4(1.0, 0.5, 0.5, 1.0),
		color4(1.0, 0.5, 0.5, 1.0), 100.0, GL_LINES);

	model_view = model_view_sv;
	model_view *= (Translate(-0.5, 0.1, 0.0)*Scale(1.5, 0.3, 1.0));  //자전거 핸들1 
	sphere1(color4(1.0, 0.411765, 0.705882, 1.0),
		color4(0.690196, 0.188235, 0.376471, 1.0),
		color4(0.6, 0.196078, 0.0, 1.0), 100.0, GL_LINES);

	model_view = model_view_sv;
	model_view *= (Translate(-0.5, 0.1, 0.0)*Scale(0.3, 1.5, 1.0));  //자전거 핸들2
	sphere1(color4(1.0, 0.2, 1.0, 1.0),
		color4(1.0, 0.5, -0.5, 1.0),
		color4(1.0, 0.5, 0.5, 1.0), 100.0, GL_LINES);


	model_view = model_view_sv;
	model_view *= (Translate(-3.0, 0.0, 0.0)*Scale(0.3, 0.7, 1.0));  //자전거 몸체 
	upper_arm();

	model_view = model_view_sv;
	model_view *= (Translate(-2.0, -0.1, 0.0)*Scale(5.0, 0.05, 2.0)); //자전거 앞부분 
	upper_arm();

	model_view = model_view_sv;
	model_view *= (Translate(-0.5, 1.0, 0.0)*Scale(0.5, 0.3, 1.0));  //사람 팔 
	lower_arm();

	model_view = model_view_sv;
	model_view *= (Translate(-0.1, 0.8, 0.0)*Scale(0.5, 0.3, 1.0));  //사람 팔 
	lower_arm();

	model_view = model_view_sv;
	model_view *= Translate(1.0, 2.2, 0.0)*Scale(1.0, 1.0, 1.0);
	cone1(color4(0.2, 0.3, 0.11222, 1.0),     //사람 머리 
		color4(0.803922, 0.803922, 0.756863, 1.0),
		color4(0.933333, 0.898039, 0.870588, 1.0), 100.0);

	model_view = model_view_sv;
	model_view *= Translate(1.3, 2.1, 0.0)*Scale(1.3, 1.3, 1.0);
	sphere1(color4(1.0, 0.3, 2.0, 1.0),     //사람 머리 장식 
		color4(0.3, 1.0, 1.0, 1.0),
		color4(0.5, -0.5, 0.0, 1.0), 100.0, GL_LINES);




	model_view = model_view_sv;
	model_view *= (Translate(-0.8, 2.3, 0.0)*RotateY(90)*Scale(0.5, 1.0, 2.0)); //사람 상체  
	cube1(color4(0.7, 1.7, 1.0, 1.0),
		color4(1.0, 0.7, 0.7, 1.0),
		color4(2.0, 0.7, 1.7, 1.0), 100.0);



	model_view = model_view_sv;
	model_view *= (Translate(-2.1, 2.5, 0.0)*RotateY(90)*Scale(0.2, 0.3, 2.0)); //사람 다리1
	cube1(color4(0.7, 0.7, 1.0, 2.0),
		color4(1.0, 1.7, 0.7, 1.0),
		color4(1.0, 0.7, 0.7, 1.0), 100.0);

	model_view = model_view_sv;
	model_view *= (Translate(-2.1, 2.0, 0.0)*RotateY(90)*Scale(0.2, 0.3, 2.0)); //사람다리2
	cube1(color4(0.7, 0.7, 1.0, 2.0),
		color4(1.0, 1.7, 0.7, 1.0),
		color4(1.0, 0.7, 0.7, 1.0), 100.0);


	
	// falling ball
	// falling ball
	model_view = model_view_sv;
	ball();
	
	model_view = model_view_sv;
	ball2();

	model_view = model_view_sv;
	ball3();

	model_view = model_view_sv;
	ball4();


	model_view = model_view_sv;
	ball5();


	model_view = model_view_sv;
	ball6();


	model_view = model_view_sv;
	ball7();

	model_view = model_view_sv;
	ball8();

	model_view = model_view_sv;
	ball9();

	model_view = model_view_sv;
	ball10();
	//  model_view = model_view_sv;
	//  model_view *= (Translate(3.0, 1.0, 0.0)*Scale(0.2, 0.3, 2.0));
	 // robot1()

	glutSwapBuffers();
}



void
menu(int option)
{
	if (option == Quit) {
		exit(EXIT_SUCCESS);
	}
	else {
		Axis1 = option;
	}
}
//----------------------------------------------------------------------------

void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:    Theta[Xaxis] += 10.5; if (Theta[Xaxis] > 360.0) Theta[Xaxis] -= 360.0;  break;
		case GLUT_MIDDLE_BUTTON:  Theta[Yaxis] += 10.5; if (Theta[Yaxis] > 360.0) Theta[Yaxis] -= 360.0;  break;
		case GLUT_RIGHT_BUTTON:   Theta[Zaxis] += 10.5; if (Theta[Zaxis] > 360.0) Theta[Zaxis] -= 360.0;  break;
		}
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// Incrase the joint angle
		Theta[Axis1] += 5.0;
		if (Theta[Axis1] > 360.0) { Theta[Axis1] -= 360.0; }
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		// Decrase the joint angle
		Theta[Axis1] -= 5.0;
		if (Theta[Axis1] < 0.0) { Theta[Axis1] += 360.0; }
	}
}

//----------------------------------------------------------------------------

void
idle(void)
{
	//Theta[Xaxis] += 0.5; if (Theta[Xaxis] > 360.0) Theta[Xaxis] -= 360.0;
	//Theta[Yaxis] += 0.5; if (Theta[Yaxis] > 360.0) Theta[Yaxis] -= 360.0;
	Theta[Zaxis] += 0.07; if (Theta[Zaxis] > 360.0) Theta[Zaxis] -= 360.0;

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------

void
reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	GLfloat aspect = GLfloat(width) / height;
	//mat4  projection = Perspective( 150.0, aspect, 0.5, 3.0 );
	//mat4  projection = Frustum(-5.0, 5.0, -5.0, 5.0, 0.5, 3.0);
	mat4  projection = Ortho(-6.0, 6.0, -6.0, 6.0, 0.5, 3.0);

	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
}

//----------------------------------------------------------------------------





int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(1024, 1024);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("1594029 이충헌");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);
	glutCreateMenu(menu);
	// Set the menu values to the relevant rotation axis values (or Quit)
	glutAddMenuEntry("Simple Form", Base);
	glutAddMenuEntry("Mouse Form", LowerArm);
	glutAddMenuEntry("Twist Form", UpperArm);
	glutAddMenuEntry("quit", Quit);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);


	glutMainLoop();
	return 0;
}