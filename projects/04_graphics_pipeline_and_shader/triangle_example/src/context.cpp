#include "context.h"

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init()
{
    // 정점 데이터를 담은 array
    float vertices[] = {
        // first triangle
        0.5f, 0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f, // bottom right
        -0.5f, 0.5f, 0.0f, // top left
        // second triangle
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f   // top left
    };

    // 순서주의; VAO 바인딩 -> VBO 바이딩 -> vertex attribute setting

    // VAO(Vertex Attribute Object) 바인딩
    glGenVertexArrays(1, &m_vertexArrayObject); // VAO 생성
    glBindVertexArray(m_vertexArrayObject);     // 지금부터 사용할 VAO 지정

    // VBO(Vertex Buffer Object) 바인딩
    glGenBuffers(1, &m_vertexBuffer);                                            // 새로운 buffer object를 만든다
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);                               // 지금부터 사용할 buffer object를 지정한다
                                                                                 // GL_ARRAY_BUFFER : 사용할 buffer object는 vertex data를 저장할 용도임을 알려줌
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18, vertices, GL_STATIC_DRAW); // 지정된 buffer에 데이터를 복사한다
                                                                                 // 용도는 "STATIC | DYNAMIC | STREAM", "DRAW | COPY | READ"의 조합
                                                                                 // 버퍼의 데이터를 바꾸지 않을거라면 STATIC, 바꾼다면 DYNAMIC
                                                                                 // STREAM은 버퍼를 한번 세팅하고 바로 버릴 예정일 경우
                                                                                 // 용도에 맞는 flag를 지정해야 효율이 올라감
    // vertex attribute setting
    glEnableVertexAttribArray(0);                                          // 정점 attribute 중 n번째를 사용하도록 설정
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0); // n: 정점의 n번째 attribute
                                                                           // size: 해당 attribute는 몇개의 값으로 구성되어 있는가?
                                                                           // type: 해당 attribute의 데이터 타입
                                                                           // normalized: 0~1사이의 값인가
                                                                           // stride: 두 정점간의 간격 (byte 단위)
                                                                           // offset: 첫 정점의 헤당 attribute까지의 간격 (byte 단위)

    // shader 생성
    // Shader 인스턴스가 unique_ptr에서 shared_ptr로 변환되었음을 유의
    ShaderPtr vertShader = Shader::CreateFromFile("./shader/simple.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/simple.fs", GL_FRAGMENT_SHADER);
    /* 
        앞에 ShaderPtr로 타입을 명시해줘야 unique_ptr이 shadred_ptr로 바뀐다.
        auto로 쓰면 CreateFromFile의 반환타이빈 unique_ptr이 그대로 쓰인다. 
        auto vertShader = Shader::CreateFromFile("./shader/simple.vs", GL_VERTEX_SHADER);
    */
    if (!vertShader || !fragShader)
        return false;

    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    // program 생성
    m_program = Program::Create({fragShader, vertShader});

    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    glClearColor(0.0f, 0.1f, 0.2f, 0.0f); // 화면을 지울 색상 지정을 컬러버퍼에 설정.

    return true;
}

void Context::Render()
{
    glClear(GL_COLOR_BUFFER_BIT); // 컬러버퍼 안에 있는 색상으로 화면을 지움.

    glUseProgram(m_program->Get());
    glDrawArrays(GL_TRIANGLES, 0, 6); // glDrawArray(primitive, offset, count) : 현재 설정된 program, VBO, VAO로 그림을 그린다
                                      // primitive: 그리고자 하는 primitive 타입
                                      // offset: 그리고자 하는 첫 정점의 index
                                      // count: 그리려는 정점의 총 개수
}
