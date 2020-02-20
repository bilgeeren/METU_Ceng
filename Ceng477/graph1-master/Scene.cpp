#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Shape.h"
#include "tinyxml2.h"
#include <algorithm>
#include <limits>
#include <cmath>
using namespace tinyxml2;
/*
 * Must render the scene from each camera's viewpoint and create an image.
 * You can use the methods of the Image class to save the image as a PPM file.
 */
 void Scene::renderScene(void)
 {

 	for(int i=0; i<cameras.size(); i++){
 		Camera* cam1 = cameras[i];
 		Image img1 = Image(cam1->imgPlane.nx,cam1->imgPlane.ny);
 		vector<vector<float>> vec1(cam1->imgPlane.nx, vector<float> (cam1->imgPlane.ny, 0));  //NOT TO SET A PIXEL TWICE
 		this->cam = cam1;
 		this->img = img1;
 		this->vec = vec1;

 		for(int col=0; col<cam->imgPlane.nx; col++){
 				int width = cam->imgPlane.ny;
 				auto t1 = thread(&Scene::threadFunc,this, col,0, width/8);
				auto t2 = thread(&Scene::threadFunc,this, col,width/8, 2*width/8);
				auto t3 = thread(&Scene::threadFunc,this, col,2*width/8, 3*width/8);
				auto t4 = thread(&Scene::threadFunc,this, col,3*width/8, 4*width/8);
				auto t5 = thread(&Scene::threadFunc,this, col,4*width/8, 5*width/8);
				auto t6 = thread(&Scene::threadFunc,this, col,5*width/8, 6*width/8);
				auto t7 = thread(&Scene::threadFunc,this, col,6*width/8, 7*width/8);
				auto t8 = thread(&Scene::threadFunc,this, col,7*width/8, width);

				t1.join();
				t2.join();
				t3.join();
				t4.join();
				t5.join();
				t6.join();
				t7.join();
				t8.join();

 		}

 		this->img.saveImage(cam->imageName);
 	}
 }
 void Scene::threadFunc(int col, int start, int end) {
 			int currentDepth = 0;
 	for(int row=start; row<end; row++){
 		Ray ray = cam->getPrimaryRay(col,row);
 		for(int j=0; j<this->objects.size(); j++){

 				Vector3f color,fin;

 				color = recursiveRayEq(currentDepth, j,col, row, vec, ray, cam);
 				if(color.x == -1) continue;
 				if (this->vec[col][row] != 0) {
 					fin = clamp(color);
 					Color res;
 					res.red = fin.x;
 					res.grn = fin.y;
 					res.blu = fin.z;

 					this->img.setPixelValue(col,row,res);
 				}

 		}
 		if (this->vec[col][row] == 0)  {
 			Vector3f color;
 			color.x = this->backgroundColor.x;
 			color.y = this->backgroundColor.y;
 			color.z = this->backgroundColor.z;
 			Vector3f fin;
 			fin = clamp(color);
 			Color res;
 			res.red = fin.x;
 			res.grn = fin.y;
 			res.blu = fin.z;

 			this->img.setPixelValue(col,row,res);
 		}
 	}
 }
Vector3f Scene::recursiveRayEq(int currentDepth, int j, int col, int row, vector<vector<float>> &vec, Ray ray, Camera* cam ) {
	// std::cout<<currentDepth<<"x"<<std::endl;

	Shape* obj = this->objects[j];
	ReturnVal ret = obj->intersect(ray);
	if(ret.distance != -1) {
		VectorOperators oops;
		Vector3f rayOrigin = ray.origin;
		Material* obj_mat = this->materials[obj->matIndex-1];
		if(currentDepth == 0 and vec[col][row] != 0 and vec[col][row] < ret.distance){
			Vector3f color;
			color.x = -1;
			color.y = -1;
			color.z = -1;
			return color;
		}
		//cout<<obj->id<<endl;
		if(currentDepth == 0) vec[col][row] = ret.distance;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		int flag = 0;

		// Mirror:
		Vector3f tempDir = oops.subtraction(rayOrigin,ret.intersection);
		tempDir = oops.normalize(tempDir);
		tempDir = oops.addition(oops.multWithConst(tempDir,-1),
								oops.multWithConst(oops.multWithConst(ret.normal,2),
													oops.dotProduct(tempDir,ret.normal)));
		Ray refRay;
		refRay.direction = tempDir;
		refRay.origin =oops.addition(ret.intersection,oops.multWithConst(ret.normal,this->shadowRayEps));

		if (!(obj_mat->mirrorRef.x == 0.0f and
			obj_mat->mirrorRef.y == 0.0f and
			obj_mat->mirrorRef.z == 0.0f) and !(currentDepth > this->maxRecursionDepth)) {
				Vector3f mirrorVec; mirrorVec.x = 0.0f; mirrorVec.y = 0.0f; mirrorVec.z = 0.0f;
				float tempDist = INT_MAX;
				int myObj = -1;
				for(int i=0; i<this->objects.size(); i++){
					Shape* tmpobj = this->objects[i];
					ReturnVal tempret = tmpobj->intersect(refRay);
					if(tempret.distance != -1 and tempret.distance < tempDist) {
						tempDist= tempret.distance;
						myObj = i;
					}

				}
				if (myObj != -1) mirrorVec = oops.addition(mirrorVec,recursiveRayEq((currentDepth+1),myObj,col, row, vec, refRay,cam));
				x += mirrorVec.x*obj_mat->mirrorRef.x;
				y += mirrorVec.y*obj_mat->mirrorRef.y;
				z += mirrorVec.z*obj_mat->mirrorRef.z;
			}
		for(int l=0; l<this->lights.size(); l++){
			flag = 0;
			PointLight* pl = this->lights[l];
			Vector3f lightContribution = (pl->computeLightContribution(ret.intersection));
			Vector3f lightDir = oops.subtraction(pl->position,ret.intersection) ;
			lightDir = oops.normalize(lightDir);

			float dP = oops.dotProduct(lightDir,ret.normal);
			float cos =  0.f < dP ? dP : 0.f;

			//Specular:
			Vector3f w0Vec = oops.normalize(oops.subtraction(rayOrigin,ret.intersection));
			Vector3f halfVec = oops.normalize(oops.addition(lightDir,w0Vec));
			float cosA = oops.dotProduct(halfVec,ret.normal);
			cosA =  0.f < cosA ? cosA : 0.f;

			// Shadow:
			Ray shadowRay;
			shadowRay.origin = oops.addition(ret.intersection,oops.multWithConst(ret.normal,this->shadowRayEps));
			shadowRay.direction = oops.normalize(oops.subtraction(pl->position, shadowRay.origin));
			int ob;
			for (ob=0 ;ob<this->objects.size();ob++){
					Shape* tmpobj = this->objects[ob];
					ReturnVal tempret = tmpobj->intersect(shadowRay);
					if(tempret.distance != -1 and
					tempret.distance
					<=
					oops.length(oops.subtraction(pl->position,shadowRay.origin))) {

						flag = 1;
						break;
					}
			}
			if(flag == 0){
				x += (obj_mat->diffuseRef.x)*cos*lightContribution.x;
				y += (obj_mat->diffuseRef.y)*cos*lightContribution.y;
				z += (obj_mat->diffuseRef.z)*cos*lightContribution.z;
				x += obj_mat->specularRef.x*(pow(cosA,obj_mat->phongExp))*lightContribution.x;
				y += obj_mat->specularRef.y*(pow(cosA,obj_mat->phongExp))*lightContribution.y ;
				z += obj_mat->specularRef.z*(pow(cosA,obj_mat->phongExp))*lightContribution.z ;
			}
		}
		Vector3f color,fin;

		color.x = obj_mat->ambientRef.x*this->ambientLight.x + x;
		color.y = obj_mat->ambientRef.y*this->ambientLight.y + y;
		color.z = obj_mat->ambientRef.z*this->ambientLight.z + z;

		return color;
	}

	Vector3f color;
	color.x = -1;
	color.y = -1;
	color.z = -1;
	return color;
}

Vector3f Scene::clamp(Vector3f vec){
	vec.x = std::min(255.f,vec.x);
	vec.y = std::min(255.f,vec.y);
	vec.z = std::min(255.f,vec.z);

	vec.x = std::max(0.f,vec.x);
	vec.y = std::max(0.f,vec.y);
	vec.z = std::max(0.f,vec.z);
	return vec;
}

// Parses XML file.
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLError eResult;
	XMLElement *pElement;

	maxRecursionDepth = 1;
	shadowRayEps = 0.001;

	eResult = xmlDoc.LoadFile(xmlPath);

	XMLNode *pRoot = xmlDoc.FirstChild();

	pElement = pRoot->FirstChildElement("MaxRecursionDepth");
	if(pElement != nullptr)
		pElement->QueryIntText(&maxRecursionDepth);

	pElement = pRoot->FirstChildElement("BackgroundColor");
	str = pElement->GetText();
	sscanf(str, "%f %f %f", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	pElement = pRoot->FirstChildElement("ShadowRayEpsilon");
	if(pElement != nullptr)
		pElement->QueryFloatText(&shadowRayEps);

	pElement = pRoot->FirstChildElement("IntersectionTestEpsilon");
	if(pElement != nullptr)
		eResult = pElement->QueryFloatText(&intTestEps);

	// Parse cameras
	pElement = pRoot->FirstChildElement("Cameras");
	XMLElement *pCamera = pElement->FirstChildElement("Camera");
	XMLElement *camElement;
	while(pCamera != nullptr)
	{
        int id;
        char imageName[64];
        Vector3f pos, gaze, up;
        ImagePlane imgPlane;

		eResult = pCamera->QueryIntAttribute("id", &id);
		camElement = pCamera->FirstChildElement("Position");
		str = camElement->GetText();
		sscanf(str, "%f %f %f", &pos.x, &pos.y, &pos.z);
		camElement = pCamera->FirstChildElement("Gaze");
		str = camElement->GetText();
		sscanf(str, "%f %f %f", &gaze.x, &gaze.y, &gaze.z);
		camElement = pCamera->FirstChildElement("Up");
		str = camElement->GetText();
		sscanf(str, "%f %f %f", &up.x, &up.y, &up.z);
		camElement = pCamera->FirstChildElement("NearPlane");
		str = camElement->GetText();
		sscanf(str, "%f %f %f %f", &imgPlane.left, &imgPlane.right, &imgPlane.bottom, &imgPlane.top);
		camElement = pCamera->FirstChildElement("NearDistance");
		eResult = camElement->QueryFloatText(&imgPlane.distance);
		camElement = pCamera->FirstChildElement("ImageResolution");
		str = camElement->GetText();
		sscanf(str, "%d %d", &imgPlane.nx, &imgPlane.ny);
		camElement = pCamera->FirstChildElement("ImageName");
		str = camElement->GetText();
		strcpy(imageName, str);

		cameras.push_back(new Camera(id, imageName, pos, gaze, up, imgPlane));

		pCamera = pCamera->NextSiblingElement("Camera");
	}

	// Parse materals
	pElement = pRoot->FirstChildElement("Materials");
	XMLElement *pMaterial = pElement->FirstChildElement("Material");
	XMLElement *materialElement;
	while(pMaterial != nullptr)
	{
		materials.push_back(new Material());

		int curr = materials.size() - 1;

		eResult = pMaterial->QueryIntAttribute("id", &materials[curr]->id);
		materialElement = pMaterial->FirstChildElement("AmbientReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->ambientRef.r, &materials[curr]->ambientRef.g, &materials[curr]->ambientRef.b);
		materialElement = pMaterial->FirstChildElement("DiffuseReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->diffuseRef.r, &materials[curr]->diffuseRef.g, &materials[curr]->diffuseRef.b);
		materialElement = pMaterial->FirstChildElement("SpecularReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->specularRef.r, &materials[curr]->specularRef.g, &materials[curr]->specularRef.b);
		materialElement = pMaterial->FirstChildElement("MirrorReflectance");
		if(materialElement != nullptr)
		{
			str = materialElement->GetText();
			sscanf(str, "%f %f %f", &materials[curr]->mirrorRef.r, &materials[curr]->mirrorRef.g, &materials[curr]->mirrorRef.b);
		}
				else
		{
			materials[curr]->mirrorRef.r = 0.0;
			materials[curr]->mirrorRef.g = 0.0;
			materials[curr]->mirrorRef.b = 0.0;
		}
		materialElement = pMaterial->FirstChildElement("PhongExponent");
		if(materialElement != nullptr)
			materialElement->QueryIntText(&materials[curr]->phongExp);

		pMaterial = pMaterial->NextSiblingElement("Material");
	}

	// Parse vertex data
	pElement = pRoot->FirstChildElement("VertexData");
	int cursor = 0;
	Vector3f tmpPoint;
	str = pElement->GetText();
	while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
		cursor++;
	while(str[cursor] != '\0')
	{
		for(int cnt = 0 ; cnt < 3 ; cnt++)
		{
			if(cnt == 0)
				tmpPoint.x = atof(str + cursor);
			else if(cnt == 1)
				tmpPoint.y = atof(str + cursor);
			else
				tmpPoint.z = atof(str + cursor);
			while(str[cursor] != ' ' && str[cursor] != '\t' && str[cursor] != '\n')
				cursor++;
			while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
				cursor++;
		}
		vertices.push_back(tmpPoint);
	}

	// Parse objects
	pElement = pRoot->FirstChildElement("Objects");

	// Parse spheres
	XMLElement *pObject = pElement->FirstChildElement("Sphere");
	XMLElement *objElement;
	while(pObject != nullptr)
	{
		int id;
		int matIndex;
		int cIndex;
		float R;

		eResult = pObject->QueryIntAttribute("id", &id);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);
		objElement = pObject->FirstChildElement("Center");
		eResult = objElement->QueryIntText(&cIndex);
		objElement = pObject->FirstChildElement("Radius");
		eResult = objElement->QueryFloatText(&R);

		objects.push_back(new Sphere(id, matIndex, cIndex, R));

		pObject = pObject->NextSiblingElement("Sphere");
	}

	// Parse triangles
	pObject = pElement->FirstChildElement("Triangle");
	while(pObject != nullptr)
	{
		int id;
		int matIndex;
		int p1Index;
		int p2Index;
		int p3Index;

		eResult = pObject->QueryIntAttribute("id", &id);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);
		objElement = pObject->FirstChildElement("Indices");
		str = objElement->GetText();
		sscanf(str, "%d %d %d", &p1Index, &p2Index, &p3Index);

		objects.push_back(new Triangle(id, matIndex, p1Index, p2Index, p3Index));

		pObject = pObject->NextSiblingElement("Triangle");
	}

	// Parse meshes
	pObject = pElement->FirstChildElement("Mesh");
	while(pObject != nullptr)
	{
		int id;
		int matIndex;
		int p1Index;
		int p2Index;
		int p3Index;
		int cursor = 0;
		int vertexOffset = 0;
		vector<Triangle> faces;

		eResult = pObject->QueryIntAttribute("id", &id);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);
		objElement = pObject->FirstChildElement("Faces");
		objElement->QueryIntAttribute("vertexOffset", &vertexOffset);
		str = objElement->GetText();
		while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
			cursor++;
		while(str[cursor] != '\0')
		{
			for(int cnt = 0 ; cnt < 3 ; cnt++)
			{
				if(cnt == 0)
					p1Index = atoi(str + cursor) + vertexOffset;
				else if(cnt == 1)
					p2Index = atoi(str + cursor) + vertexOffset;
				else
					p3Index = atoi(str + cursor) + vertexOffset;
				while(str[cursor] != ' ' && str[cursor] != '\t' && str[cursor] != '\n')
					cursor++;
				while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
					cursor++;
			}
			faces.push_back(*(new Triangle(-1, matIndex, p1Index, p2Index, p3Index)));
		}

		objects.push_back(new Mesh(id, matIndex, faces));

		pObject = pObject->NextSiblingElement("Mesh");
	}

	// Parse lights
	int id;
	Vector3f lightDir;
	Vector3f intensity;
	pElement = pRoot->FirstChildElement("Lights");

	XMLElement *pLight = pElement->FirstChildElement("AmbientLight");
	XMLElement *lightElement;
	str = pLight->GetText();
	sscanf(str, "%f %f %f", &ambientLight.r, &ambientLight.g, &ambientLight.b);

	pLight = pElement->FirstChildElement("PointLight");
	while(pLight != nullptr)
	{
		eResult = pLight->QueryIntAttribute("id", &id);
		lightElement = pLight->FirstChildElement("Position");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &lightDir.x, &lightDir.y, &lightDir.z);
		lightElement = pLight->FirstChildElement("Intensity");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &intensity.r, &intensity.g, &intensity.b);

		lights.push_back(new PointLight(lightDir, intensity));

		pLight = pLight->NextSiblingElement("PointLight");
	}
}
