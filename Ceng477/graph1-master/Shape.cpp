#include "Shape.h"
#include "Scene.h"
#include <cstdio>
#include <cmath>
#include "operators.h"

Shape::Shape(void)
{
}

Shape::Shape(int id, int matIndex)
    : id(id), matIndex(matIndex)
{
}

Sphere::Sphere(void)
{}

/* Constructor for sphere. You will implement this -> */
Sphere::Sphere(int id, int matIndex, int cIndex, float R)
    : Shape(id, matIndex)
{
	
    this->cIndex = cIndex;
    this->radius = R;

}
Vector3f Sphere::getCoord(int index)const {
    return pScene->vertices[index-1]; 
}   
/* Sphere-ray intersection routine. You will implement this -> 
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc. 
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Sphere::intersect(const Ray & ray) const
{   
    VectorOperators oops;
    Vector3f d = ray.direction;
    Vector3f o = ray.origin;
    Vector3f c = this->getCoord(this->cIndex);
    Vector3f o_c = oops.subtraction(o,c);
    float d_dot_o = oops.dotProduct(d,o);
    float d_dot_o_c = oops.dotProduct(d,o_c);
    float d_dot_d = oops.dotProduct(d,d);
    float o_c_dot_o_c = oops.dotProduct(o_c,o_c);
    float r_2 = pow(this->radius,2.0);
    float d_dot_o_c_sqr = pow(d_dot_o_c,2.0);
    float disc = d_dot_o_c_sqr - d_dot_d * (o_c_dot_o_c - r_2);
    ReturnVal ret;
    ret.distance = -1;
    const float T_MIN = 0.0f;
    
    if(disc == 0) {
        float t1 = (-1)*(d_dot_o_c)/(d_dot_d);
        if(t1 < pScene->intTestEps){
            return ret;
        }
    }
    else if(disc < 0) {
        return ret;
    }
    else {
        float t1 = ((-1)*d_dot_o_c + sqrt(disc))/(d_dot_d);
        float t2 = ((-1)*d_dot_o_c- sqrt(disc))/(d_dot_d);
        float t = t1 < t2 ? t1 : t2; //take the closest one
        if(t >=  pScene->intTestEps ){ //CHANGED
            ret.intersection = ray.getPoint(t);
            ret.normal= oops.normalize(oops.subtraction(ret.intersection ,c));
            ret.distance = oops.length(oops.subtraction(ret.intersection, ray.origin));
            if (oops.dotProduct(ret.normal, ray.direction) > 0) {
                ret.normal = oops.multWithConst(ret.normal, -1);
            }
            return ret;    		
        }
        else if (t < pScene->intTestEps){
            return ret;
        }
        else{
        	return ret;
        }
    }

}


Triangle::Triangle(void)
{}

/* Constructor for triangle. You will implement this -> */
Triangle::Triangle(int id, int matIndex, int p1Index, int p2Index, int p3Index)
    : Shape(id, matIndex)
{
	
    this->p1Index = p1Index;
    this->p2Index = p2Index;
    this->p3Index = p3Index;
}

/* Triangle-ray intersection routine. You will implement this -> 
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc. 
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */

Vector3f Triangle::getCoord(int index)const {
    return pScene->vertices[index-1]; 
}  

ReturnVal Triangle::intersect(const Ray & ray) const
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
    VectorOperators oops;
    ReturnVal result = ReturnVal();
    result.distance = -1;
    Vector3f d = ray.direction;
    Vector3f o = ray.origin;
    Vector3f a = this->getCoord(this ->p1Index);
    Vector3f b = this->getCoord(this ->p2Index);
    Vector3f c = this->getCoord(this ->p3Index);

    float ab_x = a.x - b.x;
    float ab_y = a.y - b.y;
    float ab_z = a.z - b.z;

    float ac_x = a.x - c.x;
    float ac_y = a.y - c.y;
    float ac_z = a.z - c.z;    

    float eihf = ac_y*d.z - ac_z*d.y;
    float gfdi = d.x*ac_z - ac_x*d.z;
    float dheg = ac_x*d.y -ac_y*d.x;
    float area  = ab_x*(eihf) + ab_y*(gfdi) + ab_z*(dheg);

    float ao_x = a.x - o.x;
    float ao_y = a.y - o.y;
    float ao_z = a.z - o.z;

    float g1 = ao_y*d.z - ao_z*d.y;
    float g2 = d.x*ao_z - ao_x*d.z;
    float g3 = ao_x*d.y - ao_y*d.x;

    float gamma = (ab_x*g1 + ab_y*g2 + ab_z*g3)/area;
    

    if (gamma < 0 || gamma > 1) {
        return result;
    }

    float t1 = ac_y*ao_z - ao_y*ac_z;
    float t2 = ac_z*ao_x - ac_x*ao_z;
    float t3 = ac_x*ao_y - ac_y*ao_x;

    float t = (ab_x*t1 + ab_y*t2 + ab_z*t3)/area;  
    if (t < pScene->intTestEps ) {
      return result;
    }    

    float beta = (ao_x*(eihf) + ao_y*(gfdi) + ao_z*(dheg))/area;    
    if (beta < 0 || beta > (1 - gamma)) {
        return result;
    }

    result.intersection = ray.getPoint(t);
    result.normal = oops.normalize(oops.crossProduct(oops.subtraction(a,b),oops.subtraction(c,b)));
    result.distance = oops.length(oops.subtraction(result.intersection, ray.origin));
    if (oops.dotProduct(result.normal, ray.direction) > 0) {
        result.normal = oops.multWithConst(result.normal, -1);
    }

    return result;
    
}

Mesh::Mesh()
{}

/* Constructor for mesh. You will implement this. */
Mesh::Mesh(int id, int matIndex, const vector<Triangle>& faces)
    : Shape(id, matIndex)
{
     this->faces = faces;
     
}

/* Mesh-ray intersection routine. You will implement this -> 
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc. 
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Mesh::intersect(const Ray & ray) const
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
    //std::cout<<this->faces.size()<<std::endl;
	ReturnVal last;
    last.distance = INT8_MAX;
	for(int i=0; i<(this->faces).size(); i++){
		Triangle tri = (this->faces)[i];
		ReturnVal ret; 
		ret = tri.intersect(ray);
		if(ret.distance != -1 and last.distance > ret.distance ){
            //std::cout<<"--"<<endl;	
            VectorOperators oops;
			last.intersection = ret.intersection;
			last.normal = ret.normal;
            last.distance = ret.distance;
            	
		}

	}
    if(last.distance == INT8_MAX) last.distance = -1; 
	return last;
}
