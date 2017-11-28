#ifndef SKELETON_H
#define SKELETON_H

#include <assimp/scene.h>
#include <vector>
#include <QVector3D>
#include <iostream>
#include <fstream>

struct Triangle {
    uint a;
    uint b;
    uint c;
};

typedef struct{
    uint mother;
    uint child;
}Bone;

class Skeleton
{
public:
    Skeleton(uint, uint, aiBone**);
    Skeleton(const std::string& skelFile, const std::string& weightFile);
    Skeleton();
    ~Skeleton();
    inline float* weightsAt(uint vertex) { return weights + (vertex*nbBones);}
    float simil(uint vertexInd, Triangle t);
    void parseSkelFile(const std::string& file);

private:
    uint nbBones;
    uint nbVertices;
    float *weights;

    std::vector<QVector3D> articulations;
    std::vector<Bone> edges;
};

#endif // SKELETON_H
