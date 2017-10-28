#include "skeleton.h"


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

Skeleton::Skeleton(){}

Skeleton::~Skeleton() {
    delete [] weights;
}

float Skeleton::simil(int vertexInd, int faceInd) {
    
}