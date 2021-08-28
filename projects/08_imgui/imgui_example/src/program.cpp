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

void Program::SetUniform(const std::string &name, int value) const
{
    // glGetUniformLocation() 함수로 shader 내의 sampler2D uniform 핸들을 얻어옴
    // glUniform1i() 함수로 sampler2D uniform에 텍스처 슬롯 인덱스를 입력
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1i(loc, value);
}

void Program::SetUniform(const std::string &name,
                         const glm::mat4 &value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    // 첫 번째 인자는 vertexshader의 transform 변수의 handle
    // 두 번째 인자는 행렬 개수
    // transpose(전치)의 여부
    // transform은 16개(4*4)의 value를 저장하고 있는 배열을 가지고 있음. glm::value_ptr은 그 배열의 첫 원소의 주소값을 의미.
}