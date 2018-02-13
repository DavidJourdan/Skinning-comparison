#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

using std::vector;
using std::string;

#include<QVector3D>
#include "skeleton.h"
#include "config.h"

struct Vertex
{
    Vertex(QVector3D pos, QVector3D normal) :
        pos { pos }, normal { normal }
    { }

    Vertex() { }

    QVector3D pos;
    QVector3D normal;
};

class Mesh
{
public:
    Mesh(vector<Vertex> vertices,
         vector<Triangle> triangles,
         Skeleton skeleton,
         vector<QVector3D> cors);

    const vector<Vertex> &getVertices() const { return vertices; }
    const vector<Triangle> &getTriangles() const { return triangles; }
    const Skeleton &getSkeleton() const { return skeleton; }
    const vector<QVector3D> &getCoRs() const { return CoRs; }
    uint getBoneSelected() const { return boneSelected; }
    void setBoneSelected(uint i) { boneSelected = i % skeleton.getBones().size(); }

    static Mesh fromOcorFile(const std::string &fileName);

    void rotateBone(float angle, QVector3D axis);

private:
    vector<Vertex> vertices;
    vector<Triangle> triangles;
    Skeleton skeleton;
    vector<QVector3D> CoRs;
    uint boneSelected { 0 };
};

#endif // MESH_H
