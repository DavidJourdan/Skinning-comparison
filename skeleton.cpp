#include "skeleton.h"
#include <math.h>
#define SIGMA_2 0.01

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

Skeleton::Skeleton(const std::string &skelFile, const std::string &weightFile)
{
    weights = new float[1];
    parseSkelFile(skelFile);
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

void Skeleton::parseSkelFile(const std::string &file)
{
    std::ifstream f;
    f.open(file);
    std::string s;
    std::string temp;

    std::getline(f, s);std::getline(f, s); //first two lines unuseful

    uint numArt = 0;
    std::getline(f, s); // third line : number of articulations
    int index = s.find_first_of(" ")+1;
    numArt = std::stoi(s.substr(index, s.size() - index));

    std::cout << numArt << std::endl;
    articulations.reserve(numArt);

    for(int i = 0 ; i < numArt ; i++)
    {

    }
}
