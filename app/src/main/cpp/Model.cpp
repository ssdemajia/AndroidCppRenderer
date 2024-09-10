//
// Created by ss on 2024/8/25.
//
#include "Model.h"
#include "AndroidOut.h"
#include <filesystem>
#include <stddef.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>

std::shared_ptr<FModel> FModel::LoadAsset(AAssetManager *assetManager, const std::string & InModelPath) {
    auto pModelAsset = AAssetManager_open(
            assetManager,
            InModelPath.c_str(),
            AASSET_MODE_BUFFER);
    const void* pBuffer = AAsset_getBuffer(pModelAsset);
    size_t Length = AAsset_getLength(pModelAsset);
    std::shared_ptr<FModel> Model = std::make_shared<FModel>();
    Model->Load(pBuffer, Length);
    Model->GenerateVAO();
    return Model;
}

void FModel::Load(const void *InBuffer, size_t InLength) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFileFromMemory(InBuffer, InLength, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        aout << "[ERROR] Assimp " << importer.GetErrorString() << std::endl;
        return;
    }
//    mModelDir = std::filesystem::path(InModelPath).parent_path();
//    mFileName = std::filesystem::path(InModelPath).filename().string();
    ProcessNode(scene->mRootNode, scene);

    aout << "load assimp model" << std::endl;
}

void FModel::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        auto* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

void FModel::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    Mesh smesh;
    int vertexCount = 0;
    for (auto i = 0; i < mesh->mNumVertices; i++)
    {
        FVertex vertex;
        if (mesh->mVertices)
        {
            aiVector3D& v = mesh->mVertices[i];
            vertex.pos = glm::vec3(v.x, v.y, v.z);
        }

        if (mesh->mNormals)
        {
            aiVector3D& n = mesh->mNormals[i];
            vertex.normal = glm::vec3(n.x, n.y, n.z);
        }

        if (mesh->mTextureCoords[0])
        {
            aiVector3D& t = mesh->mTextureCoords[0][i];
            vertex.uv0 = glm::vec2(t.x, t.y);   // 有可能出现3个分量得情况
        }

        if (mesh->mTangents)
        {
            aiVector3D& b = mesh->mTangents[i];
            vertex.tangent = glm::vec3(b.x, b.y, b.z);
        }
        vertices.push_back(vertex);
        vertexCount++;
    }
    smesh.vertexCount = vertexCount;

    smesh.indexOffset = indices.size();
    int indexCount = 0;
    for (auto i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace& face = mesh->mFaces[i];
        for (auto j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
            indexCount++;
        }
    }
    smesh.indexCount = indexCount;

    if (mesh->mMaterialIndex < 0)
    {
        aout << "Mesh haven't got a material\n";
        return;
    }
    mMeshes.push_back(smesh);
}

void FModel::GenerateVAO()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint vbo;
    glGenBuffers(1, &vbo);
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(FVertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, uv0));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, tangent));
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(FVertex), (void*)offsetof(FVertex, color));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}


void FModel::Draw()
{
    glBindVertexArray(vao);
    for (int i = 0; i < mMeshes.size(); i++)
    {
        auto& mesh = mMeshes[i];
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, (void*)(mesh.indexOffset * sizeof(unsigned int)));
    }
    glBindVertexArray(0);
}

void FMeshPrimitive::draw(const unsigned int readTex1, const unsigned int readTex2, const unsigned int readTex3)
{
    glBindVertexArray(VAO);
    if (readTex1 != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, readTex1);
    }

    if (readTex2 != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, readTex2);
    }

    if (readTex3 != 0) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, readTex3);
    }

    glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

void FQuad::draw(const unsigned int readTex1, const unsigned int readTex2, const unsigned int readTex3)
{
    glDisable(GL_DEPTH_TEST);
    FMeshPrimitive::draw(readTex1, readTex2, readTex3);
}

void FQuad::setup()
{
    const float quadVertices[] = {
            //positions //texCoordinates
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
    };

    //OpenGL postprocessing quad setup
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //Bind Vertex Array Object and VBO in correct order
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //VBO initialization
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    //Quad position pointer initialization in attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    //Quad texcoords pointer initialization in attribute array
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}