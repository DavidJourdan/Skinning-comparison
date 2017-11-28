#ifndef SKELETON_H
#define SKELETON_H

#include <assimp/scene.h>
#include <vector>

struct Triangle {
    uint a;
    uint b;
    uint c;
};

class Skeleton
{
public:
    Skeleton(uint, uint, aiBone**);
    Skeleton();
    ~Skeleton();
    inline float* weightsAt(uint vertex) { return weights + (vertex*nbBones);}
    float simil(uint vertexInd, Triangle t);
private:
    uint nbBones;
    uint nbVertices;
    float *weights;

};

#endif // SKELETON_H
