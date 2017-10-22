#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <glm/vec3.hpp>

class Mesh
{
public:
    Mesh(const std::string& fileName);

    std::vector<glm::vec3>& getVertices() { return vertices; }

private:
    std::vector<glm::vec3> vertices;
    std::vector<size_t> indices;
};

#endif // MESH_H
