#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "common.h"
#include "shader.h"

// vertex shader 단계와 fragment shader 단계를 거치는 하나의 그래픽스 파이프라인을 프로그램이라고한다.
CLASS_PTR(Program)
class Program
{
public:
    static ProgramUPtr Create(const std::vector<ShaderPtr> &shaders); // 링크에 성공하면 Opengl program object를 생성. 실패하면 메모리 할당해제(uniqueptr)
                                                                      // vertex, fragment shader 외에 여러 개의 Shader를 링크할 수도 있게 하기 위해
                                                                      // std::vector<ShaderPtr> 타입으로 인자를 받음.(레퍼런스로 받아서 복사 x)
                                                                      // Shader 인스턴스는 다른 Program 인스턴스를 만드는 데 재사용할 수도 있음.
                                                                      // 따라서 shared pointer를 사용: ShaderPtr(메모리 소유권 공유)

    ~Program();
    uint32_t Get() const { return m_program; }
    void Use() const;

private:
    Program() {}
    bool Link(const std::vector<ShaderPtr> &shaders); // 두 개의 shader를 입력받아서 프로그램을 링크

    uint32_t m_program{0};
};

#endif // __PROGRAM_H__