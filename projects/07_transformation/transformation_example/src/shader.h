#ifndef __SHADER_H__
#define __SHADER_H__

#include "common.h"

CLASS_PTR(Shader);
class Shader
{
public:
    static ShaderUPtr CreateFromFile(const std::string &filename,
                                     GLenum shaderType);
    // Shader shader = new Shader(); 에러. 생성자가 Private이라 접근 불가
    // ShaderUPtr shader = Shader::CreateFromFile("shader/simple.vs", GL_VERTEX_SHADER); 방식으로만 쉐이더 생성 가능.
    ~Shader();
    uint32_t Get() const { return m_shader; } // Get()은 있는데 Set()는 없는 이유: shader 오브젝트의 생성 관리는 Shader 내부에서만 관리

private:
    Shader() {}                                                    // 생성자가 private인 이유: CreateFromFile() 함수 외에 다른 방식의 Shader 인스턴스 생성을 막기 위해서
    bool LoadFile(const std::string &filename, GLenum shaderType); // LoadFile()이 bool을 리턴하는 이유: 생성에 실패할 경우 false를 리턴하기 위해서
    uint32_t m_shader{0};
};

#endif // __SHADER_H__