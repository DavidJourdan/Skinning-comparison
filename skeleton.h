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

typedef struct{
    uint mother;
    uint child;
}Relation;

class Skeleton
{
public:
    Skeleton(uint, uint, aiBone**);
    Skeleton(const std::string& file);//, const std::string& weightFile);
    Skeleton();
    ~Skeleton();
    inline float* weightsAt(uint vertex) { return weights + (vertex*nbBones);}
    float simil(uint vertexInd, Triangle t);
    bool parseSkelFile(const std::string& file);
    std::vector<QVector3D> getSkelLines();
    uint getNumberBones() {return edges.size();}

    void parseWeights(const std::string &fileName, size_t meshVertexCount);

private:
    uint nbBones;
    uint nbVertices;
    float *weights;

    std::vector<QVector3D> articulations;
    std::vector<Bone> edges;
    std::vector<Relation> relations;
};

#endif // SKELETON_H
