#include "program.h"

ProgramUPtr Program::Create(const std::vector<ShaderPtr> &shaders)
{
    auto program = ProgramUPtr(new Program());
    if (!program->Link(shaders))
        return nullptr;
    return std::move(program);
}

bool Program::Link(const std::vector<ShaderPtr> &shaders)
{
    m_program = glCreateProgram(); // glCreateShader와 같이 u_int32t 타입으로 반환
    for (auto &shader : shaders)
        glAttachShader(m_program, shader->Get()); // glAttachShader는 m_program에 shader id값을 세팅
    /*
        위 for문은 아래for문의 줄임표현
        for(auto itr = shaders.begin(); itr!=shaders.end(); ++itr){
            (*itr)-> Get();
        }
    */

    glLinkProgram(m_program); // vertex shader와 fragment shader가 attatch된 상태에서 그 둘을 링크

    int success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success); // glGetShaderiv와 하는 일이 같음.
                                                         // GL_LINK_STATUS에 대한 정보를 success에 넣어줌.
    if (!success)                                        // 프로그램 링크 실패시
    {
        char infoLog[1024];
        glGetProgramInfoLog(m_program, 1024, nullptr, infoLog); // program에 관한 로그 정보가져오고
        SPDLOG_ERROR("failed to link program: {}", infoLog);    // 출력
        return false;
    }
    return true;
}

Program::~Program()
{
    if (m_program)
    {                               // 프로그램 id값이 초기값 0이 아닌 다른 값이 있으면
        glDeleteProgram(m_program); // 프로그램 삭제
    }
}

void Program::Use() const
{
    glUseProgram(m_program);
}