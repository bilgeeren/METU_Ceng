#include "Light.h"
#include <cstdio>
#include <cmath>
#include "operators.h"
/* Constructor. Implemented for you. */
PointLight::PointLight(const Vector3f & position, const Vector3f & intensity)
    : position(position), intensity(intensity) { }

// Compute the contribution of light at point p using the
// inverse square law formula
Vector3f PointLight::computeLightContribution(const Vector3f& p)
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
	VectorOperators oops;
	//float distance = sqrt( pow((p.x - this->position.x),2.0) + pow((p.y - this->position.y),2.0) + pow((p.z - this->position.z),2.0) );
	float distance = oops.length(oops.subtraction(p,this->position));
	Vector3f irradience = oops.divWithConst((this->intensity),pow(distance,2.0));
	return irradience;
}
