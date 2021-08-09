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

    uint32_t m_vertexBuffer;      // VBO를 담아둘 uint32_t형 멤버 변수
                                  // vertex buffer object(VBO) : 정점 데이터를 담은 버퍼 오브젝트
                                  // 정점에 대한 다양한 데이터를 GPU가 접근 가능한 메모리에 저장해둔다
                                  // (position, normal, tangent, color, texture coordinate...)
    uint32_t m_vertexArrayObject; // VAO를 담아둘 uint32_t형 멤버 변수
                                  // vertex array object(VAO) : 정점 데이터의 구조를 알려주는 오브젝트
                                  // 각 정점은 몇 byte로 구성되었는가? 두 정점은 몇 byte만큼 떨어져 있는가? 정점의 0번째 데이터는 어떤 사이즈의 데이터가 몇개 있는 형태인가?
    uint32_t m_indexBuffer;       // EBO를 담아둘 uint32_t형 멤버 변수
                                  // 정점의 인덱스를 저장할 수 있는 버퍼, 인덱스 버퍼라고도 부름
                                  // indexed drawing
};

#endif // __CONTEXT_H__