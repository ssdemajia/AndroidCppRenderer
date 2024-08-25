#ifndef ANDROIDGLINVESTIGATIONS_MODEL_H
#define ANDROIDGLINVESTIGATIONS_MODEL_H

#include <vector>
#include <filesystem>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "TextureAsset.h"

union Vector3 {
    struct {
        float x, y, z;
    };
    float idx[3];
};

union Vector2 {
    struct {
        float x, y;
    };
    struct {
        float u, v;
    };
    float idx[2];
};

struct Vertex {
    constexpr Vertex(const Vector3 &inPosition, const Vector2 &inUV) : position(inPosition),
                                                                       uv(inUV) {}

    Vector3 position;
    Vector2 uv;
};

typedef uint16_t Index;


class Model {
public:
    inline Model(
            std::vector<Vertex> vertices,
            std::vector<Index> indices,
            std::shared_ptr<TextureAsset> spTexture)
            : vertices_(std::move(vertices)),
              indices_(std::move(indices)),
              spTexture_(std::move(spTexture)) {}

    inline const Vertex *getVertexData() const {
        return vertices_.data();
    }

    inline const size_t getIndexCount() const {
        return indices_.size();
    }

    inline const Index *getIndexData() const {
        return indices_.data();
    }

    inline const TextureAsset &getTexture() const {
        return *spTexture_;
    }

private:
    std::vector<Vertex> vertices_;
    std::vector<Index> indices_;
    std::shared_ptr<TextureAsset> spTexture_;
};

struct FVertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv0;
    glm::vec3 tangent;
    glm::vec3 color;
};

struct Mesh
{
    int materialIndex = -1;
    int vertexCount;
    int indexOffset;
    int indexCount;
};

class FModel {
public:
    static std::shared_ptr<FModel> LoadAsset(AAssetManager *assetManager, const std::string &assetPath);

    void Load(const void *InBuffer, size_t InLength);
    void GenerateVAO();
    void Draw();
private:
    void ProcessNode(aiNode* node, const aiScene* scene);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene);
    GLuint vao;
    std::filesystem::path mModelDir;
    std::string mFileName;
    std::vector<Mesh> mMeshes;
    std::vector<uint> indices;
    std::vector<FVertex> vertices;
};


#endif //ANDROIDGLINVESTIGATIONS_MODEL_H