#include "Camera.h"
#include <iostream>
using namespace std;
Camera::Camera(int id,                      // Id of the camera
               const char* imageName,       // Name of the output PPM file 
               const Vector3f& pos,         // Camera position
               const Vector3f& gaze,        // Camera gaze direction
               const Vector3f& up,          // Camera up direction
               const ImagePlane& imgPlane):id(id),pos(pos),gaze(gaze),up(up),imgPlane(imgPlane){
	
      for(int i =0 ;i<32;i++) {
           this->imageName[i] = imageName[i];
      }

}

/* Takes coordinate of an image pixel as row and col, and
 * returns the ray going through that pixel. 
 */
Ray Camera::getPrimaryRay(int col, int row) const
{
      VectorOperators oops;
      Ray ray;
      ray.origin = this->pos;
      Vector3f gaze = oops.normalize(this->gaze);
      Vector3f uVec = oops.crossProduct(this->up,oops.multWithConst(gaze,-1)); //w.o -1
      uVec = oops.normalize(uVec);
      Vector3f new_up = oops.crossProduct(uVec,gaze);
      new_up = oops.normalize(new_up);
      Vector3f mVec = oops.addition(this->pos,oops.multWithConst(gaze,this->imgPlane.distance));
      Vector3f qVec = oops.addition(oops.addition(mVec,oops.multWithConst(uVec,this->imgPlane.left)),oops.multWithConst(new_up,this->imgPlane.top)); 

      float s_u = (col + 0.5f)*((this->imgPlane.right)-(this->imgPlane.left)) / this->imgPlane.nx;
      float s_v = (row + 0.5f)*((this->imgPlane.top)-this->imgPlane.bottom) / this->imgPlane.ny;
      Vector3f s = oops.addition(qVec,(oops.subtraction(oops.multWithConst(uVec,s_u),oops.multWithConst(new_up,s_v)))); 
      ray.direction = oops.subtraction(s,this->pos);

      return ray;
}

