#include "vertex_layout.h"

VertexLayoutUPtr VertexLayout::Create()
{
    auto vertexLayout = VertexLayoutUPtr(new VertexLayout());
    vertexLayout->Init();
    return std::move(vertexLayout);
}

VertexLayout::~VertexLayout()
{
    if (m_vertexArrayObject)
    {
        glDeleteVertexArrays(1, &m_vertexArrayObject);
    }
}

void VertexLayout::Bind() const
{
    glBindVertexArray(m_vertexArrayObject); // 지금부터 사용할 VAO 지정
}

void VertexLayout::SetAttrib(
    uint32_t attribIndex, int count,
    uint32_t type, bool normalized,
    size_t stride, uint64_t offset) const
{
    glEnableVertexAttribArray(attribIndex); // // 정점 attribute 중 n번째를 사용하도록 설정
    glVertexAttribPointer(attribIndex, count, type, normalized, stride, (const void *)offset);
    // attribIndex : 정점의 n번째 attribute
    // count : 해당 attribute는 몇개의 값으로 구성되어 있는가
    // type : 해당 attribute의 데이터 타입
    // normalized: 0~1사이의 값인가
    // stride: 두 정점간의 간격 (byte 단위)
    // offset: 첫 정점의 헤당 attribute까지의 간격 (byte 단위)
}

void VertexLayout::Init()
{
    glGenVertexArrays(1, &m_vertexArrayObject); // VAO 생성
    Bind();
}