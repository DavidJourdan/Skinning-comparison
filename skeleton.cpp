#include "skeleton.h"
#include <math.h>
#include <locale>
#include <sstream>
#define SIGMA_2 0.01

using namespace std;

// TODO: implement bone hierarchy loading.
// This is difficult because Assimp doesn't really have a structure for that,
// we need to look for bone.mName in the node hierarchy
Skeleton::Skeleton(uint numBones, uint numVertices, aiBone** bones) {
    weights = new float[numBones*numVertices];
    for(uint i = 0; i < numBones*numVertices; i++)
        weights[i] = 0.0;
    for(uint i = 0; i < numBones; i++) {
        aiBone *b = bones[i];
        for(uint j = 0; j < b->mNumWeights; j++) {
            weights[numBones * b->mWeights[j].mVertexId + i] = b->mWeights[j].mWeight;
        }
    }
}

Skeleton::Skeleton(const string &skelFile, const string &weightFile, size_t meshVertexCount) {
    parseSkelFile(skelFile);
    size_t numBones = edges.size();
    weights = new float[numBones*meshVertexCount];

    // important step here: will cause a segfault if not all the weights are initialized
    for(uint i = 0; i < numBones*meshVertexCount; i++)
        weights[i] = 0.0;

    parseWeights(weightFile, meshVertexCount);
}

Skeleton::Skeleton(){}

Skeleton::~Skeleton() {
    //delete [] weights;
}

float Skeleton::simil(uint vertexInd, Triangle t) {
    size_t nbBones = edges.size();
    float *vertWeight = weightsAt(vertexInd);
    float *triWeight = new float[nbBones];
    for(uint i = 0; i < nbBones; i++)
        triWeight[i] = (weightsAt(t.a)[i] + weightsAt(t.b)[i] + weightsAt(t.c)[i]);
    float sum = 0.0;
    for(uint j = 0; j < nbBones; j++) {
        float term = 0.0;
        for(uint k = 0; k < nbBones; k++)
            if(k != j) {
                term += vertWeight[k]*triWeight[k]*
                    exp(pow(vertWeight[j]*triWeight[k]-vertWeight[k]*triWeight[j], 2)/SIGMA_2);
            }
        sum += vertWeight[j]*triWeight[j]*term;
    }
    return sum;
}

bool Skeleton::parseSkelFile(const std::string &file)
{
    // std::stof has to inpterpret dots as a decimal-point character
    std::locale::global(std::locale("POSIX"));

    std::ifstream f;
    f.open(file);
    if(!f.is_open())
        return false;

    std::string s;

    std::getline(f, s);std::getline(f, s); //first two lines useless

    uint num = 0;
    std::getline(f, s); // third line : number of articulations
    int index = s.find_first_of(" ");
    num = std::stoi(s.substr(index, s.size() - index));

    articulations.reserve(num);

    children.reserve(num);

    transformations = std::vector<QMatrix4x4>(num);

    for(unsigned int i = 0 ; i < num ; i++) // read articulations' positions
    {
        std::getline(f, s);
        float x, y, z;
        int i1 = s.find_first_of(" ")+1, i2 = s.find_last_of(" ")+1;
        x = std::stof(s.substr(0, i1));
        y = std::stof(s.substr(i1, i2-i1));
        z = std::stof(s.substr(i2, s.size()-i2));

        articulations.push_back(QVector3D(x, y, z));
        children.push_back(std::vector<size_t>());
    }

    std::getline(f, s); // read number of edges (bones)
    index = s.find_first_of(" ");
    num = std::stoi(s.substr(index, s.size() - index));

    edges.reserve(num);

    for(unsigned int i = 0 ; i < num ; i++) // read edges
    {
        std::getline(f, s);
        uint m, c;
        int i1 = s.find_first_of(" ")+1;
        m = std::stoi(s.substr(0, i1));
        c = std::stoi(s.substr(i1, s.size() - i1));

        Bone b; b.child=c;b.mother=m;
        edges.push_back(b);

        children[m].push_back(c);
    }

    std::getline(f, s); // read number of relations
    index = s.find_first_of(" ");
    num = std::stoi(s.substr(index, s.size() - index));

    relations.reserve(num);

    for(unsigned int i = 0 ; i < num ; i++) // read relations
    {
        std::getline(f, s);
        uint m, c;
        int i1 = s.find_first_of(" ")+1;
        c = std::stoi(s.substr(0, i1));
        m = std::stoi(s.substr(i1, s.size() - i1));

        Relation r; r.child=c;r.mother=m;
        relations.push_back(r);

        children[m].push_back(c);
    }

    //no need for the rest of the data
    f.close();

    return true;
}

std::vector<QVector3D> Skeleton::getSkelLines() {
    vector<QVector3D> lines;

    for(Bone b : edges) {
        auto m = articulations[b.mother];
        lines.push_back(m);

        auto c = articulations[b.child];
        lines.push_back(c);
    }

    for(Relation r : relations) {
        auto m = articulations[r.mother];
        lines.push_back(m);

        auto c = articulations[r.child];
        lines.push_back(c);
    }
    return lines;
}

void Skeleton::parseWeights(const string &fileName, size_t meshVertexCount)
{
    ifstream file { fileName };
    size_t vertexCount;
    std::string magic;
    file >> magic >> vertexCount;

    if (vertexCount != meshVertexCount) {
        std::cerr << "Weight file does not match mesh.\n";
        std::exit(EXIT_FAILURE);
    }

    weights = new float[vertexCount * edges.size()];


    for(uint i = 0; i < vertexCount * edges.size(); i++)
        weights[i] = 0.0;

    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    size_t vertexIndex = 0;
    for (std::string line; std::getline(file, line); ++vertexIndex) {
        std::stringstream lineStream { line };
        size_t weightCount;
        lineStream >> weightCount;

        size_t index;
        for (float w; lineStream >> index >> w;) {
            weights[vertexIndex * edges.size() + index] += w;
        }
    }
}

void Skeleton::rotateBone(const size_t boneIndex, float angle, const QVector3D &axis)
{
    uint mIndex = edges[boneIndex].mother;
 
    QMatrix4x4 transform { };
 
    transform.translate(articulations[mIndex]);
    transform.rotate(angle, axis);
    transform.translate(-articulations[mIndex]);
 
    transformations[mIndex] = transform * transformations[mIndex];
    std::vector<size_t> stack;
    uint cIndex = edges[boneIndex].child;
    stack.push_back(cIndex);
 
    while (!stack.empty()) {
        mIndex = stack.back();
        stack.pop_back();
        transformations[mIndex] = transform * transformations[mIndex];
        articulations[mIndex] = transform * articulations[mIndex];
        for (uint c : children[mIndex]) {
            stack.push_back(c);
        }
    }
}

