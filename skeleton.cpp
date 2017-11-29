#include "skeleton.h"
#include <math.h>
#define SIGMA_2 0.01

using namespace std;

Skeleton::Skeleton(uint numBones, uint numVertices, aiBone** bones): nbBones(numBones), nbVertices(numVertices) {
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

Skeleton::Skeleton(const std::string &file)//, const std::string &weightFile)
{
    weights = new float[1];

    std::string weightFile = file + "skel1.weights";
    std::string skelFile = file + "skel1.skeleton";

    if(!parseSkelFile(skelFile))
    {
        weightFile = file + "skel.weights";
        skelFile = file + "skel.skeleton";
        parseSkelFile(skelFile);
    }

}

Skeleton::Skeleton(){}

Skeleton::~Skeleton() {
    //delete [] weights;
}

float Skeleton::simil(uint vertexInd, Triangle t) {
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

    for(unsigned int i = 0 ; i < num ; i++) // read articulations' positions
    {
        std::getline(f, s);
        float x, y, z;
        int i1 = s.find_first_of(" ")+1, i2 = s.find_last_of(" ")+1;
        x = std::stof(s.substr(0, i1));
        y = std::stof(s.substr(i1, i2-i1));
        z = std::stof(s.substr(i2, s.size()-i2));

        articulations.push_back(QVector3D(x, y, z));
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
        m = std::stoi(s.substr(0, i1));
        c = std::stoi(s.substr(i1, s.size() - i1));

        Relation r; r.child=c;r.mother=m;
        relations.push_back(r);
    }

    //no need for the rest of the data
    f.close();

    return true;
}

std::vector<QVector3D> Skeleton::getSkelLines() {
    vector<QVector3D> lines;
    for(Bone b : edges) {
        lines.push_back(articulations[b.mother]);
        lines.push_back(articulations[b.child]);
    }
    for(Relation r : relations) {
        lines.push_back(articulations[r.mother]);
        lines.push_back(articulations[r.child]);
    }
    return lines;
}

void Skeleton::parseWeights(const string &fileName)
{
    ifstream file { fileName };
    size_t vertexCount;
    file >> std::string { } >> vertexCount;

    if (vertexCount != nbVertices) {
        std::cerr << "Weight file does not match mesh.\n";
        std::exit(EXIT_FAILURE);
    }

    size_t vertexIndex = 0;
    for (std::string line; std::getline(file, line); ++vertexIndex) {
        std::stringstream lineStream { line };
        size_t weightCount;
        lineStream >> weightCount;

        {
            size_t index;
            for (float w; lineStream >> index >> w;) {
                weights[vertexIndex * nbBones + index] += w;
            }
        }
    }
}

