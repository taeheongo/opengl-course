#include "model.h"

ModelUPtr Model::Load(const std::string &filename)
{
    auto model = ModelUPtr(new Model());
    if (!model->LoadByAssimp(filename))
        return nullptr;

    // LoadByAssimp가 끝나면 model을 이루는 m_mashes, m_materials가 다 세팅되어있음.
    return std::move(model);
}

bool Model::LoadByAssimp(const std::string &filename)
{
    Assimp::Importer importer;
    auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_ERROR("failed to load model: {}", filename);
        return false;
    }

    auto dirname = filename.substr(0, filename.find_last_of("/")); // 0 ~ 마지막 "/" 앞까지 substring
    // Lambda expression (https://docs.microsoft.com/ko-kr/cpp/cpp/lambda-expressions-in-cpp?view=msvc-160)
    // capture절의 [&]를 쓰면 해당 클로저 상위 스코프의 모든 값에 접근 가능(dirname).
    auto LoadTexture = [&](aiMaterial *material, aiTextureType type) -> TexturePtr
    {
        if (material->GetTextureCount(type) <= 0)
            return nullptr;

        aiString filepath;
        material->GetTexture(type, 0, &filepath); // type에 맞는 texture의 파일명을 filepath에 저장.

        auto image = Image::Load(fmt::format("{}/{}", dirname, filepath.C_Str()));
        if (!image)
            return nullptr;

        return Texture::CreateFromImage(image.get());
    };

    for (uint32_t i = 0; i < scene->mNumMaterials; i++)
    {
        auto material = scene->mMaterials[i];
        auto glMaterial = Material::Create();

        // material에서 사용되는 difuse 텍스쳐와 specular 텍스쳐를 로드해서 glMaterial의 멤버로 저장.
        glMaterial->diffuse = LoadTexture(material, aiTextureType_DIFFUSE);
        glMaterial->specular = LoadTexture(material, aiTextureType_SPECULAR);

        m_materials.push_back(std::move(glMaterial));
    }

    ProcessNode(scene->mRootNode, scene);
    return true;
}

void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        auto meshIndex = node->mMeshes[i];
        auto mesh = scene->mMeshes[meshIndex];
        ProcessMesh(mesh, scene); // m_meshes에 mesh보관
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

void Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    SPDLOG_INFO("process mesh: {}, #vert: {}, #face: {}",
                mesh->mName.C_Str(), mesh->mNumVertices, mesh->mNumFaces);

    std::vector<Vertex> vertices;
    vertices.resize(mesh->mNumVertices);
    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        auto &v = vertices[i];
        v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        v.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    }

    std::vector<uint32_t> indices;
    indices.resize(mesh->mNumFaces * 3);
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        indices[3 * i] = mesh->mFaces[i].mIndices[0];
        indices[3 * i + 1] = mesh->mFaces[i].mIndices[1];
        indices[3 * i + 2] = mesh->mFaces[i].mIndices[2];
    }

    // mesh생성, mesh에서 사용할 VBO, VAO, EBO가 다 설정.
    auto glMesh = Mesh::Create(vertices, indices, GL_TRIANGLES);

    // mesh에서 사용할 material 설정.
    if (mesh->mMaterialIndex >= 0)
        glMesh->SetMaterial(m_materials[mesh->mMaterialIndex]);

    m_meshes.push_back(std::move(glMesh));
}

void Model::Draw(const Program *program) const
{
    for (auto &mesh : m_meshes)
    {
        mesh->Draw(program);
    }
}