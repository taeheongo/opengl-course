#ifndef __MESH_H__
#define __MESH_H__

#include "common.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "program.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

CLASS_PTR(Material);
class Material
{
public:
	static MaterialUPtr Create()
	{
		return MaterialUPtr(new Material());
	}
	TexturePtr diffuse;
	TexturePtr specular;
	float shininess{32.0f};

	void SetToProgram(const Program *program) const; // program에서 사용하는 material의 diffuse, specular, shiniess를 uniform설정 및 바인딩

private:
	Material() {}
};

CLASS_PTR(Mesh);
class Mesh
{
public:
	static MeshUPtr Create(
		const std::vector<Vertex> &vertices,
		const std::vector<uint32_t> &indices,
		uint32_t primitiveType); // vertices, indices를 인자로 받아서 m_vertexLayout에 맞게 상자생성
	static MeshUPtr CreateBox(); // 정적인 vertices indices로 m_vertexLayout에 맞게 상자 생성

	const VertexLayout *GetVertexLayout() const { return m_vertexLayout.get(); }
	BufferPtr GetVertexBuffer() const { return m_vertexBuffer; }
	BufferPtr GetIndexBuffer() const { return m_indexBuffer; }

	void SetMaterial(MaterialPtr material) { m_material = material; }
	MaterialPtr GetMaterial() const { return m_material; }

	void Draw(const Program *program) const;

private:
	Mesh() {}
	void Init(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, uint32_t primitiveType);

	uint32_t m_primitiveType{GL_TRIANGLES};

	VertexLayoutUPtr m_vertexLayout; // VAO는 해당 메쉬를 그리는데만 사용하므로 unique_ptr
	BufferPtr m_vertexBuffer;		 // VBO EBO는 다른 VAO와 연결하여 재사용할 수 있으므로 shared_ptr
	BufferPtr m_indexBuffer;

	MaterialPtr m_material; // unique_ptr이 아니라 shadred_ptr을 쓰는 이유는 하나의 material을 여러 mesh에서 공유할 수 있게 하기 위해
							// 소유권을 공유.
};

#endif // __MESH_H__