#ifndef SKELETON_H
#define SKELETON_H

#include <assimp/scene.h>
#include <vector>

class Skeleton
{
public:
    Skeleton(uint, uint, aiBone**);
    Skeleton();

private:
    int nbBones;
    int nbVertices;
    float *weights;

};

#endif // SKELETON_H
