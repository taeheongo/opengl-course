#include "shader.h"

ShaderUPtr Shader::CreateFromFile(const std::string &filename,
                                  GLenum shaderType)
{
    // auto shader = std::unique_ptr<Shader>(new Shader());
    auto shader = ShaderUPtr(new Shader()); // 위와 동일

    if (!shader->LoadFile(filename, shaderType)) // shader파일을 로드하는데 실패하면
        return nullptr;                          // 함수 종료 시 nullptr 리턴, 그리고 shader 포인터 해제
    return std::move(shader);                    // shader 파일을 로드하는데 성공하면 shader 포인터가 가리키는 메모리에 대한 소유권 이전
}

bool Shader::LoadFile(const std::string &filename, GLenum shaderType)
{
    auto result = LoadTextFile(filename); // optional의 값은 있을 수도 없을 수도 었다.
    if (!result.has_value())              // optional의 값이 있는지 체크
        return false;                     // 파일을 못 읽었으니 false 리턴.

    auto &code = result.value(); // 레퍼런스 쓰는이유: string code = result.value(); 를 사용하게 되면 메모리 복사가 이루어짐.
                                 // 메모리를 복사할 필요가없음. 이 함수 종료시까지 result가 존재하기 때문. 반환은 bool 이기때문.
    const char *codePtr = code.c_str();
    int32_t codeLength = (int32_t)code.length(); // int32_t 는 그냥 int. 32비트(4bytes) 정수형.

    // create and compile shader
    m_shader = glCreateShader(shaderType);                                     // glCreateShader()는 정수를 반환
    glShaderSource(m_shader, 1, (const GLchar *const *)&codePtr, &codeLength); // 첫 번째 shader id, 두 번째 인자는 코드의 개수, 3번째인자는 코드 배열, 4번째 인자는 코드 글자수 배열
    glCompileShader(m_shader);                                                 // shader 소스들을 컴파이르

    // check compile error
    int success = 0;
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success); // glGetShaderiv는 shader 정보를 가져온다. 두번째 인자에 shader의 컴파일 상태를 넣어줘서 컴파일 상태에 대한 정보를 가져와서 success에 넣어준다.
                                                          // iv는 integer vector. interger pointer를 의미. 그래서 마지막 인자에 success의 주소값을 넣어줌.
    if (!success)                                         // 실패했으면 success에 그대로 0이 들어있음.
    {
        char infoLog[1024];                                   // 얻어올 정보의 크기 1024
        glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog); // 로그 정보를 가져온다.
        SPDLOG_ERROR("failed to compile shader: \"{}\"", filename);
        SPDLOG_ERROR("reason: {}", infoLog); // 컴파일 쉐이더 이유 출력
        return false;
    }
    return true; // 성공 !
}

Shader::~Shader()
{
    if (m_shader)
    {
        glDeleteShader(m_shader);
    }
}