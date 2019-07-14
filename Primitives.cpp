
#include "Angel.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;


///////////////////// Unit Cube ///////////////////////////

const int cubeNumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
point4 cubePoints[cubeNumVertices];
vec3   cubeNormals[cubeNumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.5, -0.5,  0.5, 1.0),
	point4(-0.5,  0.5,  0.5, 1.0),
	point4(0.5,  0.5,  0.5, 1.0),
	point4(0.5, -0.5,  0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5,  0.5, -0.5, 1.0),
	point4(0.5,  0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};
//----------------------------------------------------------------------------
// quad generates two triangles for each face and assigns colors
//    to the vertices
int cubeIdx = 0;

void
quad(int a, int b, int c, int d)
{
	// Initialize temporary vectors along the quad's edge to
	//   compute its face normal 
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];

	vec3 normal = normalize(cross(u, v));

	cubeNormals[cubeIdx] = normal; cubePoints[cubeIdx] = vertices[a]; cubeIdx++;
	cubeNormals[cubeIdx] = normal; cubePoints[cubeIdx] = vertices[b]; cubeIdx++;
	cubeNormals[cubeIdx] = normal; cubePoints[cubeIdx] = vertices[c]; cubeIdx++;
	cubeNormals[cubeIdx] = normal; cubePoints[cubeIdx] = vertices[a]; cubeIdx++;
	cubeNormals[cubeIdx] = normal; cubePoints[cubeIdx] = vertices[c]; cubeIdx++;
	cubeNormals[cubeIdx] = normal; cubePoints[cubeIdx] = vertices[d]; cubeIdx++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
cube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

///////////////////// Unit Cone ///////////////////////////

const int coneSlices = 20;
const int coneNumVertices = 3 * coneSlices;

point4 conePoints[coneNumVertices];
vec3   coneNormals[coneNumVertices];

point4 evalCircle(float u)
{
	return point4(cos(u), sin(u), 0.0, 1.0);
}
// The cone is subdivided around the Z axis into slices.
void
cone(int slices)
{
	int coneIdx = 0;
	point4 northpole(0.0, 0.0, 1.0, 1.0);
	point4 p1, p2;
	vec4 u, v;
	vec3 normal;
	float rad = 2 * M_PI / slices;

	for (int i = 0; i < slices - 1; i++)
	{
		p1 = evalCircle(i * rad);
		p2 = evalCircle((i + 1) * rad);

		u = p1 - northpole;
		v = p2 - p1;
		normal = normalize(cross(u, v));

		coneNormals[coneIdx] = normal; conePoints[coneIdx] = northpole; coneIdx++;
		coneNormals[coneIdx] = normal; conePoints[coneIdx] = p1; coneIdx++;
		coneNormals[coneIdx] = normal; conePoints[coneIdx] = p2; coneIdx++;
	}
	p1 = p2;
	p2 = evalCircle(0.0);
	u = p1 - northpole;
	v = p2 - p1;
	normal = normalize(cross(u, v));

	coneNormals[coneIdx] = normal; conePoints[coneIdx] = northpole; coneIdx++;
	coneNormals[coneIdx] = normal; conePoints[coneIdx] = p1; coneIdx++;
	coneNormals[coneIdx] = normal; conePoints[coneIdx] = p2; coneIdx++;
}


///////////////////// Unit Sphere ///////////////////////////

const int shpereSlices = 20;
const int shpereStacks = 20;
const int sphereNumVertices = 3 * (2 * shpereSlices + 2 * shpereSlices * (shpereStacks - 2));

point4 spherePoints[sphereNumVertices];
vec3   sphereNormals[sphereNumVertices];

point4 evalSphere(float u, float v)
{
	return point4(cos(u)*sin(v), sin(u)*sin(v), cos(v), 1.0);
}
// The sphere is subdivided around the Z axis into slices and along the Z axis into stacks.
void
sphere(int slices, int stacks)
{
	int sphIdx = 0;
	point4 northpole(0.0, 0.0, 1.0, 1.0);
	point4 p1, p2, p3, p4;
	vec4 u, v;
	vec3 normal;
	float u_rad = 2 * M_PI / slices;
	float v_rad = 2 * M_PI / stacks;

	// first stack
	for (int i = 0; i < slices - 1; i++)
	{
		p1 = evalSphere(i * u_rad, v_rad);
		p2 = evalSphere((i + 1) * u_rad, v_rad);

		sphereNormals[sphIdx] = normalize(vec3(northpole.x, northpole.y, northpole.z)); spherePoints[sphIdx] = northpole; sphIdx++;
		sphereNormals[sphIdx] = normalize(vec3(p1.x, p1.y, p1.z)); spherePoints[sphIdx] = p1; sphIdx++;
		sphereNormals[sphIdx] = normalize(vec3(p2.x, p2.y, p2.z)); spherePoints[sphIdx] = p2; sphIdx++;
	}
	p1 = p2;
	p2 = evalSphere(0.0, v_rad);

	sphereNormals[sphIdx] = normalize(vec3(northpole.x, northpole.y, northpole.z)); spherePoints[sphIdx] = northpole; sphIdx++;
	sphereNormals[sphIdx] = normalize(vec3(p1.x, p1.y, p1.z)); spherePoints[sphIdx] = p1; sphIdx++;
	sphereNormals[sphIdx] = normalize(vec3(p2.x, p2.y, p2.z)); spherePoints[sphIdx] = p2; sphIdx++;

	// middle stacks
	for (int j = 1; j < stacks - 2; j++) 
	{
		for (int i = 0; i < slices - 1; i++)
		{
			p1 = evalSphere(i * u_rad, j * v_rad);
			p2 = evalSphere(i * u_rad, (j + 1) * v_rad);
			p3 = evalSphere((i + 1) * u_rad, (j + 1) * v_rad);
			p4 = evalSphere((i + 1) * u_rad, j * v_rad);

			sphereNormals[sphIdx] = normalize(vec3(p1.x, p1.y, p1.z)); spherePoints[sphIdx] = p1; sphIdx++;
			sphereNormals[sphIdx] = normalize(vec3(p2.x, p2.y, p2.z)); spherePoints[sphIdx] = p2; sphIdx++;
			sphereNormals[sphIdx] = normalize(vec3(p3.x, p3.y, p3.z)); spherePoints[sphIdx] = p3; sphIdx++;

			sphereNormals[sphIdx] = normalize(vec3(p1.x, p1.y, p1.z)); spherePoints[sphIdx] = p1; sphIdx++;
			sphereNormals[sphIdx] = normalize(vec3(p3.x, p3.y, p3.z)); spherePoints[sphIdx] = p3; sphIdx++;
			sphereNormals[sphIdx] = normalize(vec3(p4.x, p4.y, p4.z)); spherePoints[sphIdx] = p4; sphIdx++;
		}
		p1 = p4;
		p2 = p3;
		p3 = evalSphere(0.0, (j + 1) * v_rad);
		p4 = evalSphere(0.0, j * v_rad);

		sphereNormals[sphIdx] = normalize(vec3(p1.x, p1.y, p1.z)); spherePoints[sphIdx] = p1; sphIdx++;
		sphereNormals[sphIdx] = normalize(vec3(p2.x, p2.y, p2.z)); spherePoints[sphIdx] = p2; sphIdx++;
		sphereNormals[sphIdx] = normalize(vec3(p3.x, p3.y, p3.z)); spherePoints[sphIdx] = p3; sphIdx++;

		sphereNormals[sphIdx] = normalize(vec3(p1.x, p1.y, p1.z)); spherePoints[sphIdx] = p1; sphIdx++;
		sphereNormals[sphIdx] = normalize(vec3(p3.x, p3.y, p3.z)); spherePoints[sphIdx] = p3; sphIdx++;
		sphereNormals[sphIdx] = normalize(vec3(p4.x, p4.y, p4.z)); spherePoints[sphIdx] = p4; sphIdx++;

	}

	// last stack
	point4 southpole(0.0, 0.0, -1.0, 1.0);
	for (int i = 0; i < slices - 1; i++)
	{
		p2 = evalSphere(i * u_rad, (stacks - 1) * v_rad);
		p1 = evalSphere((i + 1) * u_rad, (stacks - 1) * v_rad);

		u = p1 - southpole;
		v = p2 - p1;
		normal = normalize(cross(u, v));

		sphereNormals[sphIdx] = normal; spherePoints[sphIdx] = southpole; sphIdx++;
		sphereNormals[sphIdx] = normal; spherePoints[sphIdx] = p1; sphIdx++;
		sphereNormals[sphIdx] = normal; spherePoints[sphIdx] = p2; sphIdx++;
	}
	p2 = p1;
	p2 = evalSphere(0.0, (stacks - 1) * v_rad);
	u = p1 - southpole;
	v = p2 - p1;
	normal = normalize(cross(u, v));

	sphereNormals[sphIdx] = normal; spherePoints[sphIdx] = southpole; sphIdx++;
	sphereNormals[sphIdx] = normal; spherePoints[sphIdx] = p1; sphIdx++;
	sphereNormals[sphIdx] = normal; spherePoints[sphIdx] = p2; sphIdx++;
}

