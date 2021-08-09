#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"

CLASS_PTR(Context)
class Context
{
public:
    static ContextUPtr Create();
    void Render();

private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;

    uint32_t m_vertexBuffer; // VBO를 담아둘 uint32_t형 멤버 변수
                             // vertex buffer object(VBO) : 정점 데이터를 담은 버퍼 오브젝트
                             // 정점에 대한 다양한 데이터를 GPU가 접근 가능한 메모리에 저장해둔다
                             // (position, normal, tangent, color, texture coordinate...)
    uint32_t m_vertexArrayObject;
};

#endif // __CONTEXT_H__