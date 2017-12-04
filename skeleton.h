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

struct Bone {
    uint mother;
    uint child;
};

struct Relation {
    uint mother;
    uint child;
};

class Skeleton
{
public:
    Skeleton(const std::string &weightFile, const std::string &skelFile, size_t meshVertexCount);
    Skeleton(uint, uint, aiBone**);
    Skeleton(const std::string& file);//, const std::string& weightFile);
    Skeleton();
    ~Skeleton();
    inline float* weightsAt(uint vertex) { return weights + (vertex*edges.size());}
    float simil(uint vertexInd, Triangle t);
    bool parseSkelFile(const std::string& file);
    std::vector<QVector3D> getSkelLines();
    uint getNumberBones() {return edges.size();}

    void parseWeights(const std::string &fileName, size_t meshVertexCount);

private:
    uint nbBones;
    float *weights;

    std::vector<QVector3D> articulations;
    std::vector<Bone> edges;
    std::vector<Relation> relations;
};

#endif // SKELETON_H
