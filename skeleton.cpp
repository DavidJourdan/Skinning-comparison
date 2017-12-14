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
    vector<vector<float>> vectorWeight(numVertices);
    vector<vector<uint>> vectorBone(numVertices);

    for(uint i = 0; i < numBones; i++) {
        aiBone *b = bones[i];
        for(uint j = 0; j < b->mNumWeights; j++) {
            vectorWeight[b->mWeights[j].mVertexId].push_back(b->mWeights[j].mWeight);
            vectorBone[b->mWeights[j].mVertexId].push_back(i);
        }
    }

    weights = new float*[numVertices];
    boneInd = new uint*[numVertices];
    for(uint i = 0; i < numVertices; i++) {
        weights[i] = vectorWeight[i].data();
        boneInd[i] = vectorBone[i].data();
    }
}

Skeleton::Skeleton(const string &skelFile, const string &weightFile, size_t meshVertexCount) {
    parseSkelFile(skelFile);
    weights = new float*[meshVertexCount];
    boneInd = new uint*[meshVertexCount];   

    parseWeights(weightFile, meshVertexCount);
}

Skeleton::Skeleton(){}

Skeleton::~Skeleton() {
    //delete [] weights;
}

float Skeleton::simil(uint vertexInd, Triangle t) {
    uint i = 0;
    uint a = 0, b = 0, c = 0;
    vector<float> triWeights;
    vector<uint> triInds;
    while(weights[vertexInd][i] > 0) { // weight arrays end with a negative number
        uint bone = boneInd[vertexInd][i];
        float w = 0.0;

        // get matching bone indices (if either vertex or triangle doesn't have a weight 
        // on a given bone, we don't compute it)
        while(boneInd[t.a][a] < bone && weights[t.a][a] > 0) // bone indices are sorted 
            ++a;
        if(boneInd[t.a][a] == bone) {
            w += weights[t.a][a];
        }

        while(boneInd[t.b][b] < bone && weights[t.b][b] > 0)
            ++b;
        if(boneInd[t.b][b] == bone) {
            w += weights[t.b][b];
        }

        while(boneInd[t.c][c] < bone && weights[t.c][c] > 0)
            ++c;
        if(boneInd[t.c][c] == bone) {
            w += weights[t.c][c];
        }

        if(w > 0) {
            w = w/3.; // barycenter of weights
            triWeights.push_back(w);
            triInds.push_back(i); // list of bone indices with non-zero weights for vertex and triangle
        }

        ++i;
    }

    float sum = 0.0;
    for(uint j = 0; j < triInds.size(); j++) {
        float factor = 0.0;
        float w_j = weights[vertexInd][triInds[j]];
        for(uint k = 0; k < triInds.size(); k++)
            if(k != j) {
                float w_k = weights[vertexInd][triInds[k]];
                factor += w_k * triWeights[k] *
                    exp(- pow(w_j*triWeights[k] - w_k*triWeights[j], 2) / SIGMA_2);
            }
        sum += w_j * triWeights[j] * factor;
    }

    return sum;
}

bool Skeleton::parseSkelFile(const std::string &file)
{
    // std::stof has to interpret dots as a decimal-point character
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
    vector<Bone> tempEdges;
    tempEdges.reserve(num - 1);

    //siblings[bone.parent] is the list of bones indices sharing the same parent than bone (including bone itself)
    vector<vector<uint>> siblings(num);

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
    }

    std::getline(f, s); // read number of edges (bones)
    index = s.find_first_of(" ");
    num = std::stoi(s.substr(index, s.size() - index));

    boneNb = num;

    for(unsigned int i = 0 ; i < num ; i++) // read edges
    {
        std::getline(f, s);
        uint m, c;
        int i1 = s.find_first_of(" ")+1;
        m = std::stoi(s.substr(0, i1));
        c = std::stoi(s.substr(i1, s.size() - i1));

        Bone b = Bone(true, m, c);
        tempEdges.push_back(b);

        siblings[m].push_back(i);
    }

    std::getline(f, s); // read number of relations
    index = s.find_first_of(" ");
    num = std::stoi(s.substr(index, s.size() - index));

    for(unsigned int i = 0 ; i < num ; i++) // read relations
    {
        std::getline(f, s);
        uint m, c;
        int i1 = s.find_first_of(" ")+1;
        c = std::stoi(s.substr(0, i1));
        m = std::stoi(s.substr(i1, s.size() - i1));

        Bone b = Bone(false, m, c);
        tempEdges.push_back(b);

        siblings[m].push_back(boneNb + i);
    }

    // write the edges array at once, for better data locality
    edges.reserve(tempEdges.size());
    try{
        for(uint i = 0; i < tempEdges.size(); i++) {
            Bone t = tempEdges[i];
            vector<uint> successors = siblings[tempEdges[i].child];
            edges.push_back(Bone(t.edge, t.parent, t.child, successors.size()));
            for(uint j = 0; j < successors.size(); j++){
                edges[i].successors[j] = successors[j];
            }
        }
    } catch(bad_alloc& ba) {
            std::cerr << "bad_alloc caught: " << ba.what() << '\n';
    }

    //no need for the rest of the data
    f.close();

    return true;
}

std::vector<QVector3D> Skeleton::getSkelLines() {
    vector<QVector3D> lines;

    for(Bone b : edges) {
        QVector3D p = articulations[b.parent];
        lines.push_back(p);

        QVector3D c = articulations[b.child];
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

    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    uint vertexIndex = 0;
    for (std::string line; std::getline(file, line); ++vertexIndex) {
        std::stringstream lineStream { line };
        size_t weightCount;
        lineStream >> weightCount;

        weights[vertexIndex] = new float[weightCount+1];
        boneInd[vertexIndex] = new uint[weightCount];

        uint bone, i = 0;
        for (float w; lineStream >> bone >> w;) {
            weights[vertexIndex][i] = w;
            boneInd[vertexIndex][i] = bone;
            i++;
        }
        weights[vertexIndex][i] = -1;
    }
}

void Skeleton::rotateBone(const size_t boneIndex, float angle, const QVector3D &axis)
{
    uint mIndex = edges[boneIndex].parent;
 
    QMatrix4x4 transform { };
 
    transform.translate(articulations[mIndex]);
    transform.rotate(angle, axis);
    transform.translate(-articulations[mIndex]);
 
    // breadth-first search in the skeleton tree
    std::vector<uint> stack;
    stack.push_back(boneIndex);
 
    while (!stack.empty()) {
        uint curBoneI = stack.back();
        stack.pop_back();
        transformations[curBoneI] = transform * transformations[curBoneI];
        uint c = edges[curBoneI].child;
        articulations[c] = transform * articulations[c];

        for (uint i = 0; i < edges[curBoneI].childNb; i++) {
            stack.push_back(edges[curBoneI].successors[i]);
        }
    }
}

