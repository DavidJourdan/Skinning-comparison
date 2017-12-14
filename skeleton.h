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
    bool edge; // false if it's a PARENT_RELATION
    uint parent; // index of parent joint position
    uint child; // index of child joint position
    size_t childNb;
    uint *successors; //array of Bone indices
    Bone(bool edge, uint parInd, uint childInd, uint childNb = 0): edge(edge), parent(parInd), 
        child(childInd), childNb(childNb) {
            successors = (childNb != 0) ? new uint[childNb] : nullptr;
    }
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
    uint getNumberBones() {return boneNb;}

    const std::vector<QVector3D> &getArticulations() const { return articulations; }
    const std::vector<Bone> &getEdges() const { return edges; }

    void parseWeights(const std::string &fileName, size_t meshVertexCount);

    void rotateBone(const size_t boneIndex, float angle, const QVector3D &axis);

private:
    float **weights;
    uint **boneInd;
    uint boneNb;

    std::vector<QVector3D> articulations;
    std::vector<Bone> edges;
    std::vector<QMatrix4x4> transformations;
};

#endif // SKELETON_H
