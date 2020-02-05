#ifndef _SCENE_H_
#define _SCENE_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include "Ray.h"
#include "defs.h"
#include "Image.h"

// Forward declarations to avoid cyclic references
class Camera;
class PointLight;
class Material;
class Shape;

using namespace std;

// Class to hold everything related to a scene.
class Scene
{
public:
	int maxRecursionDepth;			// Maximum recursion depth
	float intTestEps = 1e-6;				// IntersectionTestEpsilon. You will need this one while implementing intersect routines in Shape class
	float shadowRayEps;				// ShadowRayEpsilon. You will need this one while generating shadow rays.
	Vector3f backgroundColor;		// Background color
	Vector3f ambientLight;			// Ambient light radiance

	vector<Camera *> cameras;		// Vector holding all cameras
	vector<PointLight *> lights;	// Vector holding all point lights
	vector<Material *> materials;	// Vector holding all materials
	vector<Vector3f> vertices;		// Vector holding all vertices (vertex data)
	vector<Shape *> objects;		// Vector holding all shapes

	Scene(const char *xmlPath);		// Constructor. Parses XML file and initializes vectors above. Implemented for you.

	void renderScene(void);			// Method to render scene, an image is created for each camera in the scene. You will implement this.

private:
    // Write any other stuff here
    Vector3f clamp(Vector3f);
	Vector3f recursiveRayEq(int currentDepth, int j, int col, int row, vector<vector<float>> &vec, Ray ray,Camera* cam );
	void threadFunc(int col, int start, int end);
	Camera* cam;
	Image img = Image(0,0);
	vector<vector<float>> vec;

};

#endif
