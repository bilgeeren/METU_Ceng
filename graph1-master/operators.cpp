#include "operators.h"


float VectorOperators::dotProduct(Vector3f fst, Vector3f snd)const {
    return (fst.x*snd.x + fst.y*snd.y + fst.z*snd.z);  
}

Vector3f VectorOperators::addition(Vector3f fst, Vector3f snd)const {
    Vector3f vec;
    vec.x = fst.x + snd.x;
    vec.y = fst.y + snd.y;
    vec.z = fst.z + snd.z;
    return vec;
}

Vector3f VectorOperators::subtraction(Vector3f fst, Vector3f snd)const {
    Vector3f vec;
    vec.x = fst.x - snd.x;
    vec.y = fst.y - snd.y;
    vec.z = fst.z - snd.z;
    return vec;
}

Vector3f VectorOperators::crossProduct(Vector3f fst, Vector3f snd)const {
    Vector3f vec;
    vec.x = fst.y*snd.z - fst.z*snd.y;
    vec.y = fst.z*snd.x - fst.x*snd.z;
    vec.z = fst.x*snd.y - fst.y*snd.x;
    return vec;
}

Vector3f VectorOperators::multWithConst(Vector3f fst, float x)const { 
    Vector3f vec;
    vec.x = fst.x*float(x);
    vec.y = fst.y*float(x);
    vec.z = fst.z*float(x);
    return vec;
}

Vector3f VectorOperators::divWithConst(Vector3f fst, float x)const {
    Vector3f vec;
    vec.x = fst.x/float(x);
    vec.y = fst.y/float(x);
    vec.z = fst.z/float(x);
    return vec;
}

Vector3f VectorOperators::normalize(Vector3f fst)const {
    Vector3f vec;
    float length = sqrt(fst.x * fst.x + fst.y * fst.y + fst.z * fst.z);
    vec.x = fst.x/length;
    vec.y = fst.y/length;
    vec.z = fst.z/length;
    return vec;
}

float VectorOperators::length(Vector3f fst)const {
    return sqrt(fst.x * fst.x + fst.y * fst.y + fst.z * fst.z);
}

