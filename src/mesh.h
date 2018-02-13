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

    const vector<Vertex> &getVertices() { return vertices; }
    const vector<Triangle> &getTriangles() { return triangles;}
    const std::vector<QVector3D> &getCoRs(void) { return CoRs; }
    uint getBoneSelected() const { return boneSelected; }
    void setBoneSelected(uint i) { boneSelected = i % skeleton.getBones().size(); }

    static Mesh fromOcorFile(const std::string &fileName);

    void rotateBone(float angle, QVector3D axis);

private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    std::vector<QVector3D> CoRs;
    Skeleton skeleton;
    uint boneSelected { 0 };
    float area(Triangle t);
};

#endif // MESH_H
