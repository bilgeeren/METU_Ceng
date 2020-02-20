#ifndef _SCENE_H_
#define _SCENE_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "Camera.h"
#include "Color.h"
#include "Model.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Triangle.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix4.h"

using namespace std;

class Scene
{
public:
	Color backgroundColor;
	bool cullingEnabled;
	int projectionType;

	vector< vector<Color> > image;
	vector< Camera* > cameras;
	vector< Vec3* > vertices;
	vector< Color* > colorsOfVertices;
	vector< Scaling* > scalings; 
	vector< Rotation* > rotations;
	vector< Translation* > translations;
	vector< Model* > models;

	Scene(const char *xmlPath);

	void initializeImage(Camera* camera);
	void forwardRenderingPipeline(Camera* camera);
	int makeBetweenZeroAnd255(double value);
	void writeImageToPPMFile(Camera* camera);
	void convertPPMToPNG(string ppmFileName, int osType);
	Matrix4 perspectiveProjection(Camera *camera);
	void liangBarskyClipping(Vec3 start, Vec3 end, double Xmin,double Ymin,double Zmin,double Xmax,double Ymax,double Zmax,Matrix4 vp);
	bool visible(double den, double num, double &tE, double &tL );
	Matrix4 modeling_trans(vector< Vec3 > &vertices,Model* model);
	Matrix4 camera_trans(Camera *camera);
	Matrix4 translate(int trans_id,int number_tri,Model* model,vector< Vec3 > &vertices);
	Matrix4 scale(int trans_id,int number_tri, Model* model,vector< Vec3 > &vertices);
	Matrix4 rotate(int trans_id,int number_tri,Model* model,vector< Vec3 > &vertices);
	vector<Triangle> backfaceCulling(Camera *camera,vector< Vec3 > &vertices,vector<Triangle> triangles);
	Matrix4 viewport(Camera* camera);
	Matrix4 orthographic(Camera* camera);
	Matrix4 construct_scale(Scaling* s);
	vector<Matrix4> construct_rx(Vec3 u);
	vector<Matrix4> construct_ry(Vec3 u);
	Matrix4 construct_rz(double angle);
	Matrix4 construct_tr(Vec3 u);
	void assignColor(int x, int y, double x_0,double x_1,double y_0,double y_1, bool isVertical, Color *firstColor, Color * secondColor, double alpha);
	void wireRasterization(Vec3 first, Vec3 second);
	void rasterization(Model* model,vector< Vec3 > &vertices,vector<Triangle> new_triangles,int horRes, int verRes);
	Vec4 matrix4Transformation(Vec3 inputM);
	Vec3 matrix3Transformation(Vec4 inputM);
	void triangleRasterization(Model* model,vector< Vec3 > &vertices,vector<Triangle> new_triangles, Vec3 first,Vec3 second,Vec3 third,int horRes, int verRes);

};

#endif
