#ifndef SKELETON_H
#define SKELETON_H

#include <assimp/scene.h>
#include <vector>
#include <QVector3D>
#include <QMatrix4x4>
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
    Skeleton();
    ~Skeleton();
    float simil(uint vertexInd, Triangle t);
    bool parseSkelFile(const std::string& file);
    std::vector<QVector3D> getSkelLines();
    uint getNumberBones() {return edges.size();}

    const std::vector<QVector3D> &getArticulations() const { return articulations; }
    const std::vector<Bone> &getEdges() const { return edges; }

    void parseWeights(const std::string &fileName, size_t meshVertexCount);

    void rotateBone(const size_t boneIndex, float angle, const QVector3D &axis);

    float **getWeights() { return weights; }
    unsigned **getBoneIndices() { return boneInd; }

private:
    float **weights;
    uint **boneInd;

    std::vector<QVector3D> articulations;
    std::vector<std::vector<size_t>> children;
    std::vector<Bone> edges;
    std::vector<Relation> relations;
    std::vector<QMatrix4x4> transformations;
};

#endif // SKELETON_H
