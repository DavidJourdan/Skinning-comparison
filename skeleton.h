#ifndef SKELETON_H
#define SKELETON_H

#include <assimp/scene.h>
#include <vector>

class Skeleton
{
public:
    Skeleton(uint, uint, aiBone**);
    Skeleton();
    ~Skeleton();
    inline float* weightsAt(uint vertex) { return weights + (vertex*nbBones);}
    float simil(int vertexInd, int faceInd);
private:
    int nbBones;
    int nbVertices;
    float *weights;

};

#endif // SKELETON_H
