#ifndef __MESH_H__
#define __MESH_H__

#include "common.h"
#include "buffer.h"
#include "vertex_layout.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
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

	void Draw() const;

private:
	Mesh() {}
	void Init(
		const std::vector<Vertex> &vertices,
		const std::vector<uint32_t> &indices,
		uint32_t primitiveType);

	uint32_t m_primitiveType{GL_TRIANGLES};
	VertexLayoutUPtr m_vertexLayout; // VAO는 해당 메쉬를 그리는데만 사용하므로 unique_ptr
	BufferPtr m_vertexBuffer;		 // VBO EBO는 다른 VAO와 연결하여 재사용할 수 있으므로 shared_ptr
	BufferPtr m_indexBuffer;
};

#endif // __MESH_H__