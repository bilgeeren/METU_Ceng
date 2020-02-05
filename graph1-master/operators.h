#include <vector>

#include "defs.h"
#include <cmath>

class VectorOperators {
    public:
        float dotProduct(Vector3f fst, Vector3f snd)const;
        Vector3f crossProduct(Vector3f fst, Vector3f snd)const;
        Vector3f addition(Vector3f fst, Vector3f snd)const;
        Vector3f subtraction(Vector3f fst, Vector3f snd)const;
        Vector3f multWithConst(Vector3f fst, float x)const; //CHANGED TO FLOAT
        Vector3f divWithConst(Vector3f fst, float x)const; //CHANGED TO FLOAT
        Vector3f normalize(Vector3f fst)const;
        float length(Vector3f fst)const;
};