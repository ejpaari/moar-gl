#include "model.h"

#include <iostream>
#include <algorithm>

namespace moar
{

Model::Model()
{
}

Model::~Model()
{
}

const std::vector<std::unique_ptr<Mesh> >& Model::getMeshes() const
{
    return meshes;
}

void Model::addMesh(std::unique_ptr<Mesh> mesh)
{    
    meshes.push_back(std::move(mesh));
}

} // moar
