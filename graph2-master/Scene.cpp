#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <cmath>

#include "Scene.h"
#include "Camera.h"
#include "Color.h"
#include "Model.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Triangle.h"
#include "Vec3.h"
#include "tinyxml2.h"
#include "Helpers.h"

using namespace tinyxml2;
using namespace std;

void Scene::forwardRenderingPipeline(Camera *camera)
{
	// TODO: Implement this function.
	Matrix4 cam = camera_trans(camera);
	Matrix4 orth = orthographic(camera);
	double r = camera->right;
	double l = camera->left;
	double t = camera->top;
	double b = camera->bottom;
	double f = camera->far;
	double n = camera->near;	

	vector< Vec3* > ver = this->vertices;	
	for(int m=0; m<this->models.size();m++){
		Model* model = this->models[m];
		vector< Vec3 > new_vertices;
		for(int i=0; i<ver.size(); i++){
			new_vertices.push_back(*(ver[i]));	
		}	
		Matrix4 modeling = modeling_trans(new_vertices,model);	

		if(this->projectionType == 0){       //orthographic
			Matrix4 cam_mod = multiplyMatrixWithMatrix(cam,modeling);
			Matrix4 per = multiplyMatrixWithMatrix(orth,cam_mod);
			for(int i=0; i<new_vertices.size();i++){			
				Vec4 p = multiplyMatrixWithVec4(per,matrix4Transformation(new_vertices[i]));
				Vec3 temp = matrix3Transformation(p);
				new_vertices[i] = temp;

			}	
		}
		else{                                //perspective
			Matrix4 p2o = perspectiveProjection(camera);
			Matrix4 cam_mod = multiplyMatrixWithMatrix(cam,modeling);
			Matrix4 p2o_cam = multiplyMatrixWithMatrix(p2o,cam_mod);
			Matrix4 per = multiplyMatrixWithMatrix(orth,p2o_cam);
			for(int i=0; i<new_vertices.size();i++){
				Vec4 p = multiplyMatrixWithVec4(per,matrix4Transformation(new_vertices[i]));
				Vec3 temp = matrix3Transformation(p);
				new_vertices[i] = temp;

			}
		} 
		vector<Triangle> new_triangles;
		Matrix4 vp = viewport( camera);
		if(model->type == 0){
			if(this->cullingEnabled){
				new_triangles = backfaceCulling(camera,new_vertices,model->triangles);
			}
			else{
				new_triangles = model->triangles;
			}
			for(int j = 0; j<new_triangles.size();j++) {
				int first_vertex_id = new_triangles[j].getFirstVertexId();
				int second_vertex_id = new_triangles[j].getSecondVertexId();
				int third_vertex_id = new_triangles[j].getThirdVertexId();

				Vec3 first_vertex = (new_vertices[first_vertex_id-1]);
				Vec3 second_vertex = (new_vertices[second_vertex_id-1]);
				Vec3 third_vertex = (new_vertices[third_vertex_id-1]);

				liangBarskyClipping(first_vertex,second_vertex,-1,-1,-1,1,1,1,vp);
				liangBarskyClipping(second_vertex, third_vertex,-1,-1,-1,1,1,1,vp);
				liangBarskyClipping(third_vertex,first_vertex,-1,-1,-1,1,1,1,vp);		
			}			
		}


		
		

		if(this->cullingEnabled){	

			if(model->type == 1){
				new_triangles = backfaceCulling(camera,new_vertices,model->triangles);
				for(int i=0; i<new_vertices.size();i++){
					Vec4 per = multiplyMatrixWithVec4(vp,matrix4Transformation(new_vertices[i]));
					Vec3 temp = matrix3Transformation(per);
					new_vertices[i] = temp;
				}
				rasterization(model,new_vertices,new_triangles,camera->horRes, camera->verRes);
			}	
						
		}
		else{
			if(model->type == 1){
				for(int i=0; i<new_vertices.size();i++){
					Vec4 per = multiplyMatrixWithVec4(vp,matrix4Transformation(new_vertices[i]));
					Vec3 temp = matrix3Transformation(per);
					new_vertices[i] = temp;
				}
				rasterization(model,new_vertices,model->triangles,camera->horRes, camera->verRes);	
			}
	
		}
		

	}
}


vector<Triangle> Scene::backfaceCulling(Camera *camera,vector< Vec3 > &vertices,vector<Triangle> triangles){
	vector<Triangle> new_triangles;
	for(int i=0; i<triangles.size(); i++){
		Triangle tri = triangles[i];
		Vec3 origin = Vec3(0,0,0,0);
		Vec3 v0 = vertices[tri.vertexIds[0]-1];
		Vec3 v1 = vertices[tri.vertexIds[1]-1];
		Vec3 v2 = vertices[tri.vertexIds[2]-1];
		Vec3 n = crossProductVec3(subtractVec3(v2,v0),subtractVec3(v1,v0));
		Vec3 v_0 = normalizeVec3(subtractVec3(origin,v0));
		Vec3 v_1 = normalizeVec3(subtractVec3(origin,v1));
		Vec3 v_2 = normalizeVec3(subtractVec3(origin,v2));
		if(dotProductVec3(n,v_0) > 0 /*or dotProductVec3(n,v_1)>0 or dotProductVec3(n,v_2)>0*/ ){
			new_triangles.push_back(tri);
		}
	}
	return new_triangles;
}

Matrix4 Scene::camera_trans(Camera *camera){
	Vec3 u = camera->u;
	Vec3 v = camera->v;
	Vec3 w = camera->w;
	v = normalizeVec3(crossProductVec3(u,camera->gaze));
	camera->v = v;
	Vec3 pos = camera->pos;
	double cam[4][4] ={{0}};
	cam[3][3] = 1;
	cam[0][0] = u.x;
	cam[1][0] = v.x;
	cam[2][0] = w.x;
	cam[0][1] = u.y;
	cam[1][1] = v.y;
	cam[2][1] = w.y;
	cam[0][2] = u.z;
	cam[1][2] = v.z;
	cam[2][2] = w.z;
	cam[0][3] = -1*(u.x*pos.x + u.y*pos.y + u.z*pos.z );		
	cam[1][3] = -1*(v.x*pos.x + v.y*pos.y + v.z*pos.z );
	cam[2][3] = -1*(w.x*pos.x + w.y*pos.y + w.z*pos.z );
	Matrix4 camera_transform = Matrix4(cam);
	return camera_transform; 
}
Matrix4 Scene::orthographic(Camera* camera){
	double ort[4][4] ={{0}};
	double r = camera->right;
	double l = camera->left;
	double t = camera->top;
	double b = camera->bottom;
	double f = camera->far;
	double n = camera->near;
	ort[3][3] = 1;
	ort[0][0] = 2/(r-l);
	ort[0][3] = -(r+l)/(r-l);
	ort[1][1] = 2/(t-b);
	ort[1][3] = -(t+b)/(t-b);
	ort[2][2] = -2/(f-n);
	ort[2][3] = -(f+n)/(f-n);
	Matrix4 ortho = Matrix4(ort);
	return ortho;	
}
Matrix4 Scene::perspectiveProjection(Camera *camera) {
	double p2oMatrix[4][4] ={{0}};
	p2oMatrix[0][0] = camera->near;
	p2oMatrix[1][1] = camera->near;
	p2oMatrix[2][2] = camera->near + camera->far;
	p2oMatrix[2][3] = camera->near * camera->far;
	p2oMatrix[3][2] = -1;
	Matrix4 p2oM = Matrix4(p2oMatrix);
	return p2oM;
}
Matrix4 Scene::viewport(Camera* camera){
	double vp[4][4] ={{0}};
	double nx = camera->horRes;
	double ny = camera->verRes;
	vp[3][3] = 1;
	vp[2][2] = double(0.5);
	vp[2][3] = double(0.5);
	vp[0][0] = nx/2;
	vp[0][3] = double((nx-1)/2);
	vp[1][1] = ny/2;
	vp[1][3] = double((ny-1)/2);
	return Matrix4(vp);
}
Matrix4 Scene::modeling_trans(vector< Vec3 > &vertices,Model* model){
	int number_trans = model->numberOfTransformations;
	int number_tri = model->numberOfTriangles;
	Matrix4 start = getIdentityMatrix();
	Matrix4 n = getIdentityMatrix();
	for (int k=0; k<number_trans; k++){
		char trans_type = model->transformationTypes[k];
		int trans_id = model->transformationIds[k]-1;		
		if(trans_type == 't'){
			n = translate(trans_id,number_tri,model,vertices);					
		}
		else if(trans_type == 'r'){
			n = rotate(trans_id,number_tri,model,vertices);
		}	
		else{
			n = scale(trans_id,number_tri,model,vertices);
		}
		start = multiplyMatrixWithMatrix(n,start);			
	}
	return start;
	
}
Matrix4 Scene::translate(int trans_id,int number_tri,Model* model,vector< Vec3 > &vertices){
	Translation* t = this->translations[trans_id];
	Vec3 translation = Vec3(t->tx,t->ty,t->tz,0); // 0 for color ID 
	Matrix4 tr = construct_tr(translation);
	return tr;	
	
}
Matrix4 Scene::scale(int trans_id,int number_tri,Model* model,vector< Vec3 > &vertices){
	Scaling* s = this->scalings[trans_id];
	Matrix4 scale = construct_scale(s); 
	return scale;
	
}
Matrix4 Scene::rotate(int trans_id,int number_tri,Model* model,vector< Vec3 > &vertices){
	Rotation* r = this->rotations[trans_id];
	Vec3 v = Vec3(r->ux,r->uy,r->uz,0);   // 0 for color ID
	Vec3 u = normalizeVec3(v);
	Matrix4 t_inv = construct_tr(multiplyVec3WithScalar(v,-1));
	Matrix4 t = construct_tr(v);
	vector<Matrix4> r_x = construct_rx(u);
	vector<Matrix4> r_y = construct_ry(u);
	double angle = ((r->angle)*M_PI)/180.0;
	Matrix4 r_z = construct_rz(angle);	
	Matrix4 op_0 = multiplyMatrixWithMatrix(r_x[0],t_inv);
	Matrix4 op_1 =multiplyMatrixWithMatrix(r_y[1],op_0);
	Matrix4 op_2 =multiplyMatrixWithMatrix(r_z,op_1);
	Matrix4 op_3 =multiplyMatrixWithMatrix(r_y[0],op_2);
	Matrix4 op_4 =multiplyMatrixWithMatrix(r_x[1],op_3);
	Matrix4 final =multiplyMatrixWithMatrix(t,op_4);
	return final;
		
}

Matrix4 Scene::construct_tr(Vec3 u){
	Matrix4 id = getIdentityMatrix();
	id.val[0][3] = u.x;
	id.val[1][3] = u.y;
	id.val[2][3] = u.z;
	return id;
}
vector<Matrix4> Scene::construct_rx(Vec3 u){
	double d = sqrt(pow(u.y,2) + pow(u.z,2));
	double cos_alfa = (u.z)/d;
	double sin_alfa = (u.y)/d;
	double rot_x[4][4] ={{0}};
	rot_x[0][0] = 1;
	rot_x[3][3] = 1;
	rot_x[1][1] = cos_alfa;
	rot_x[1][2] = -sin_alfa;
	rot_x[2][1] = sin_alfa;
	rot_x[2][2] = cos_alfa;

	double rot_x_inv[4][4] ={{0}};
	rot_x_inv[0][0] = 1;
	rot_x_inv[3][3] = 1;
	rot_x_inv[1][1] = cos_alfa;
	rot_x_inv[1][2] = sin_alfa;
	rot_x_inv[2][1] = -sin_alfa;
	rot_x_inv[2][2] = cos_alfa;	
	vector<Matrix4> vec;
	Matrix4 r = Matrix4(rot_x);
	Matrix4 r_inv = Matrix4(rot_x_inv);
	vec.push_back(r);
	vec.push_back(r_inv);
	return vec;

}
vector<Matrix4> Scene::construct_ry(Vec3 u){
	double d = sqrt(pow(u.y,2) + pow(u.z,2));
	double cos_beta = d;
	double sin_beta = u.x;
	double rot_y[4][4] ={{0}};
	rot_y[1][1] = 1;
	rot_y[3][3] = 1;
	rot_y[0][0] = cos_beta;
	rot_y[0][2] = -sin_beta;
	rot_y[2][0] = sin_beta;
	rot_y[2][2] = cos_beta;

	double rot_y_inv[4][4] ={{0}};
	rot_y_inv[1][1] = 1;
	rot_y_inv[3][3] = 1;
	rot_y_inv[0][0] = cos_beta;
	rot_y_inv[0][2] = sin_beta;
	rot_y_inv[2][0] = -sin_beta;
	rot_y_inv[2][2] = cos_beta;	

	vector<Matrix4> vec;
	vec.push_back(Matrix4(rot_y));
	vec.push_back(Matrix4(rot_y_inv));	
	return vec;

}
Matrix4 Scene::construct_rz(double angle){
	double rot_z[4][4] ={{0}};
	rot_z[0][0] = cos(angle);
	rot_z[0][1] = -sin(angle);
	rot_z[1][0] = sin(angle);
	rot_z[1][1] = cos(angle);
	rot_z[2][2] = 1;
	rot_z[3][3] = 1;
	return Matrix4(rot_z);

}
Matrix4 Scene::construct_scale(Scaling* s){
	double sx = s->sx;
	double sy = s->sy;
	double sz = s->sz;
	double rot_x[4][4] ={{0}};
	rot_x[3][3] = 1;
	rot_x[0][0] = sx;
	rot_x[1][1] = sy;
	rot_x[2][2] = sz;
	return Matrix4(rot_x);
}

void Scene::liangBarskyClipping(Vec3 start, Vec3 end, double Xmin,double Ymin,double Zmin,double Xmax,double Ymax,double Zmax,Matrix4 vp) {
	double tE = 0;
	double tL = 1;
	double dx = end.x - start.x;
	double dy = end.y - start.y;
	double dz = end.z - start.z;
	double X0 = start.x;
	double Y0 = start.y;
	double Z0 = start.z;
	bool isVisible = false; 
	if (visible(dx, Xmin - X0,tE,tL)){
		if(visible(-dx, X0 - Xmax,tE,tL)) {
			if (visible(dy, Ymin - Y0,tE,tL)){
				if(visible(-dy, Y0 - Ymax,tE,tL)) {
					if (visible(dz, Zmin - Z0,tE,tL)){
						if(visible(-dz, Z0 - Zmax,tE,tL)) {
							isVisible = true;
							if(tL < 1) {
								end.x = X0 +dx*tL;
								end.y = Y0 +dy*tL;
								end.z = Z0 +dz*tL;
							}
							if(tE > 0) {
								start.x = X0 +dx*tE;
								start.y = Y0 +dy*tE;
								start.z = Z0 +dz*tE;
							}
							Vec4 s = multiplyMatrixWithVec4(vp,matrix4Transformation(start));
							Vec3 s_new = matrix3Transformation(s);
							Vec4 e = multiplyMatrixWithVec4(vp,matrix4Transformation(end));
							Vec3 e_new = matrix3Transformation(e);
							wireRasterization(s_new,e_new);						
				
						}
					}
				}
			}
		}
	}
}
bool Scene::visible(double den, double num, double &tE, double &tL ) {
	double t;
	if(den > 0) {
		t = num/den;
		if(t > tL) return false;
		if(t > tE) tE = t;
	}
	else if (den < 0 ) {
		t = num/den;
		if(t < tE) return false;
		if(t < tL) tL = t;
	}
	else if(num > 0) return false;
	return true;
}


void Scene::wireRasterization(Vec3 first, Vec3 second) {
	double x_0 = first.x;
	double x_1 = second.x;
	double y_0 = first.y;
	double y_1 = second.y;
	int fstColorId = first.colorId;
	int sndColorId = second.colorId;
	Color *firstColor = this->colorsOfVertices[fstColorId-1];
	Color *secondColor = this->colorsOfVertices[sndColorId-1];
	double x, y,d;
	double x_min,x_max,y_min,y_max;
	x_min = x_0 < x_1 ? x_0 : x_1;
	y_min = y_0 < y_1 ? y_0 : y_1;
	x_max = x_0 > x_1 ? x_0 : x_1;
	y_max = y_0 > y_1 ? y_0 : y_1;	
	double m = (y_1-y_0)/(x_1-x_0);
	double xDiff = (x_max - x_min);
	double yDiff = (y_max - y_min);

	if(0.0 < m && m <= 1.0){
		x = x_min;
		y = y_min;

		d = -2*yDiff+xDiff;
		double alpha = x_max - x_min;
		for( x = x_min ; x <= x_max; x++){
			assignColor(x,y,x_0,x_1,y_0,y_1,false,firstColor,secondColor,alpha);
			if(d < 0){     
				y++;     
				d += 2*(-yDiff + xDiff);
			}
			else{                
				d += 2*(-yDiff);
			}
		}

	}
	else if(1.0 < m){

		x = x_min;
		y = x_max;
		d =-2*xDiff+yDiff;
		double alpha = y_max - y_min;
		for(y = y_min; y <= y_max; y++){
			assignColor(x,y,x_0,x_1,y_0,y_1,true,firstColor,secondColor,alpha);
			if(d < 0){      
				x++;       
				d += 2*(-xDiff + yDiff);

			}
			else{                   
				d += 2*(-xDiff);
			}
		}
	}
	else if(m  <= 0 && m > -1.0){
		x = x_min;
		y = y_max;

		d = -2*yDiff+xDiff;
		double alpha = x_max - x_min;
		for( x = x_min ; x <= x_max; x++){
			assignColor(x,y,x_0,x_1,y_0,y_1,false,firstColor,secondColor,alpha);
			if(d < 0){     
				y--;     
				d += 2*(-yDiff + xDiff);
			}
			else{                
				d += 2*(-yDiff);
			}
		}
	}
	else if(-1.0 >= m){
		x = x_max;
		y = y_min;
		d =-2*xDiff+yDiff;
		double alpha = y_max- y_min;
		for(y = y_min; y <= y_max; y++){
			assignColor(x,y,x_0,x_1,y_0,y_1,true,firstColor,secondColor,alpha);
			if(d < 0){      
				x--;       
				d += 2*(-xDiff + yDiff);

			}
			else{                   
				d += 2*(-xDiff);
			}
		}

	}
	
} 

void Scene::assignColor(int x, int y, double x_0,double x_1,double y_0,double y_1, bool isVertical, Color *firstColor, Color * secondColor, double alpha){
	double firstScalar;
	double secondScalar;
	if(isVertical) {
		firstScalar = abs(y-y_1);
		secondScalar = abs(y_0-y);
	}
	else {
		firstScalar = abs(x-x_1);
		secondScalar = abs(x_0-x);
	}
	
	image[x][y].r = (firstColor->r*firstScalar + secondColor->r*secondScalar)/alpha;
	image[x][y].g = (firstColor->g*firstScalar + secondColor->g*secondScalar)/alpha;
	image[x][y].b = (firstColor->b*firstScalar + secondColor->b*secondScalar)/alpha;
}

void Scene::rasterization(Model* model,vector< Vec3 > &vertices,vector<Triangle> new_triangles,int horRes, int verRes) {
	
	for(int i = 0; i<new_triangles.size();i++) {
		int first_vertex_id = new_triangles[i].getFirstVertexId();
		int second_vertex_id = new_triangles[i].getSecondVertexId();
		int third_vertex_id = new_triangles[i].getThirdVertexId();

		Vec3 first_vertex = (vertices[first_vertex_id-1]);
		Vec3 second_vertex = (vertices[second_vertex_id-1]);
		Vec3 third_vertex = (vertices[third_vertex_id-1]);
		//printVec3(first_vertex);
		//printVec3(second_vertex);
		//printVec3(third_vertex);

		if(model->type == 1) triangleRasterization(model, vertices,new_triangles,first_vertex,second_vertex,third_vertex,horRes,verRes);

	}

}

void Scene::triangleRasterization(Model* model,vector< Vec3 > &vertices,vector<Triangle> new_triangles, Vec3 first,Vec3 second,Vec3 third,int horRes, int verRes){
	
	double x_0 = first.x;
	double x_1 = second.x;
	double x_2 = third.x;
	double y_0 = first.y;
	double y_1 = second.y;
	double y_2 = third.y;

	Color *c_0 = this->colorsOfVertices[first.colorId-1];
	Color *c_1 = this->colorsOfVertices[second.colorId-1];
	Color *c_2 = this->colorsOfVertices[third.colorId-1];


	int x_min = min(x_0, min(x_1, x_2));
	int y_min = min(y_0, min(y_1, y_2));

	int x_max = max(x_0, max(x_1, x_2));
	int y_max = max(y_0, max(y_1, y_2));

	if(x_min<0){
		x_min = 0;
	}
	if(y_min<0){
		y_min = 0;
	}
	if(x_max>horRes){
		x_max = horRes;
	}
	if(y_max>verRes){
		y_max = verRes;
	}		
	double f12_min = x_min*(y_1-y_2) + y_min*(x_2-x_1) + x_1*y_2 - y_1*x_2;
	double f20_min = x_min*(y_2-y_0) + y_min*(x_0-x_2) + x_2*y_0 - y_2*x_0;
	double f01_min = x_min*(y_0-y_1) + y_min*(x_1-x_0) + x_0*y_1 - y_0*x_1;

	double f12_max = x_0*(y_1-y_2) + y_0*(x_2-x_1) + x_1*y_2 - y_1*x_2;
	double f20_max = x_1*(y_2-y_0) + y_1*(x_0-x_2) + x_2*y_0 - y_2*x_0;
	double f01_max = x_2*(y_0-y_1) + y_2*(x_1-x_0) + x_0*y_1 - y_0*x_1;

	double f01_dx = (x_1-x_0);
	double f01_dy = (y_0-y_1);
	double f12_dx = (x_2-x_1);
	double f12_dy = (y_1-y_2);
	double f20_dx = (x_0-x_2);
	double f20_dy = (y_2-y_0);

	for(int y=max(0,y_min); y <= y_max; y++){	
		double f12_temp = f12_min;
		double f20_temp = f20_min;
		double f01_temp = f01_min;
		for(int x=max(0,x_min); x <= x_max; x++){			
			double alpha = f12_temp/f12_max;
			double beta = f20_temp/f20_max;
			double gama = f01_temp/f01_max;
			if(alpha >= 0 && beta >= 0 && gama >= 0){
				if( x >= horRes or y>= verRes or x < 0 or y < 0  ){
					continue;
				}
				image[x][y].r = alpha*c_0->r + beta*c_1->r + gama*c_2->r;
				image[x][y].g = alpha*c_0->g + beta*c_1->g + gama*c_2->g;
				image[x][y].b = alpha*c_0->b + beta*c_1->b + gama*c_2->b;
			}

			f12_temp += f12_dy;
			f20_temp += f20_dy;
			f01_temp += f01_dy;
		}
		f12_min += f12_dx;
		f20_min += f20_dx;
		f01_min += f01_dx;
	}
}

Vec4 Scene::matrix4Transformation(Vec3 inputM) {
	return Vec4(inputM.x,inputM.y,inputM.z,1,inputM.colorId);
}

Vec3 Scene::matrix3Transformation(Vec4 inputM) {
	return Vec3(inputM.x/inputM.t,inputM.y/inputM.t,inputM.z/inputM.t,inputM.colorId);
}
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLElement *pElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *pRoot = xmlDoc.FirstChild();

	// read background color
	pElement = pRoot->FirstChildElement("BackgroundColor");
	str = pElement->GetText();
	sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	// read culling
	pElement = pRoot->FirstChildElement("Culling");
	if (pElement != NULL)
		pElement->QueryBoolText(&cullingEnabled);

	// read projection type
	pElement = pRoot->FirstChildElement("ProjectionType");
	if (pElement != NULL)
		pElement->QueryIntText(&projectionType);

	// read cameras
	pElement = pRoot->FirstChildElement("Cameras");
	XMLElement *pCamera = pElement->FirstChildElement("Camera");
	XMLElement *camElement;
	while (pCamera != NULL)
	{
		Camera *cam = new Camera();

		pCamera->QueryIntAttribute("id", &cam->cameraId);

		camElement = pCamera->FirstChildElement("Position");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam->pos.x, &cam->pos.y, &cam->pos.z);

		camElement = pCamera->FirstChildElement("Gaze");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam->gaze.x, &cam->gaze.y, &cam->gaze.z);
		camElement = pCamera->FirstChildElement("Up");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam->v.x, &cam->v.y, &cam->v.z);

		cam->gaze = normalizeVec3(cam->gaze);
		cam->u = crossProductVec3(cam->gaze, cam->v);
		cam->u = normalizeVec3(cam->u);

		cam->w = inverseVec3(cam->gaze);
		cam->v = crossProductVec3(cam->u, cam->gaze);
		cam->v = normalizeVec3(cam->v);

		camElement = pCamera->FirstChildElement("ImagePlane");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &cam->left, &cam->right, &cam->bottom, &cam->top,
			   &cam->near, &cam->far, &cam->horRes, &cam->verRes);

		camElement = pCamera->FirstChildElement("OutputName");
		str = camElement->GetText();
		cam->outputFileName = string(str);

		cameras.push_back(cam);

		pCamera = pCamera->NextSiblingElement("Camera");
	}

	// read vertices
	pElement = pRoot->FirstChildElement("Vertices");
	XMLElement *pVertex = pElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (pVertex != NULL)
	{
		Vec3 *vertex = new Vec3();
		Color *color = new Color();

		vertex->colorId = vertexId;

		str = pVertex->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

		str = pVertex->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

		vertices.push_back(vertex);
		colorsOfVertices.push_back(color);

		pVertex = pVertex->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	pElement = pRoot->FirstChildElement("Translations");
	XMLElement *pTranslation = pElement->FirstChildElement("Translation");
	while (pTranslation != NULL)
	{
		Translation *translation = new Translation();

		pTranslation->QueryIntAttribute("id", &translation->translationId);

		str = pTranslation->Attribute("value");
		sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

		translations.push_back(translation);

		pTranslation = pTranslation->NextSiblingElement("Translation");
	}

	// read scalings
	pElement = pRoot->FirstChildElement("Scalings");
	XMLElement *pScaling = pElement->FirstChildElement("Scaling");
	while (pScaling != NULL)
	{
		Scaling *scaling = new Scaling();

		pScaling->QueryIntAttribute("id", &scaling->scalingId);
		str = pScaling->Attribute("value");
		sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

		scalings.push_back(scaling);

		pScaling = pScaling->NextSiblingElement("Scaling");
	}

	// read rotations
	pElement = pRoot->FirstChildElement("Rotations");
	XMLElement *pRotation = pElement->FirstChildElement("Rotation");
	while (pRotation != NULL)
	{
		Rotation *rotation = new Rotation();

		pRotation->QueryIntAttribute("id", &rotation->rotationId);
		str = pRotation->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

		rotations.push_back(rotation);

		pRotation = pRotation->NextSiblingElement("Rotation");
	}

	// read models
	pElement = pRoot->FirstChildElement("Models");

	XMLElement *pModel = pElement->FirstChildElement("Model");
	XMLElement *modelElement;
	while (pModel != NULL)
	{
		Model *model = new Model();

		pModel->QueryIntAttribute("id", &model->modelId);
		pModel->QueryIntAttribute("type", &model->type);

		// read model transformations
		XMLElement *pTransformations = pModel->FirstChildElement("Transformations");
		XMLElement *pTransformation = pTransformations->FirstChildElement("Transformation");

		pTransformations->QueryIntAttribute("count", &model->numberOfTransformations);

		while (pTransformation != NULL)
		{
			char transformationType;
			int transformationId;

			str = pTransformation->GetText();
			sscanf(str, "%c %d", &transformationType, &transformationId);

			model->transformationTypes.push_back(transformationType);
			model->transformationIds.push_back(transformationId);

			pTransformation = pTransformation->NextSiblingElement("Transformation");
		}

		// read model triangles
		XMLElement *pTriangles = pModel->FirstChildElement("Triangles");
		XMLElement *pTriangle = pTriangles->FirstChildElement("Triangle");

		pTriangles->QueryIntAttribute("count", &model->numberOfTriangles);

		while (pTriangle != NULL)
		{
			int v1, v2, v3;

			str = pTriangle->GetText();
			sscanf(str, "%d %d %d", &v1, &v2, &v3);

			model->triangles.push_back(Triangle(v1, v2, v3));

			pTriangle = pTriangle->NextSiblingElement("Triangle");
		}

		models.push_back(model);

		pModel = pModel->NextSiblingElement("Model");
	}
}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
	if (this->image.empty())
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			vector<Color> rowOfColors;

			for (int j = 0; j < camera->verRes; j++)
			{
				rowOfColors.push_back(this->backgroundColor);
			}

			this->image.push_back(rowOfColors);
		}
	}
	// if image is filled before, just change color rgb values with the background color
	else
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			for (int j = 0; j < camera->verRes; j++)
			{
				this->image[i][j].r = this->backgroundColor.r;
				this->image[i][j].g = this->backgroundColor.g;
				this->image[i][j].b = this->backgroundColor.b;
			}
		}
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}

/*
	Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFileName.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFileName << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
		}
		fout << endl;
	}
	fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
	os_type == 1 		-> Ubuntu
	os_type == 2 		-> Windows
	os_type == other	-> No conversion
*/
void Scene::convertPPMToPNG(string ppmFileName, int osType)
{
	string command;

	// call command on Ubuntu
	if (osType == 1)
	{
		command = "convert " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// call command on Windows
	else if (osType == 2)
	{
		command = "magick convert " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// default action - don't do conversion
	else
	{
	}
}