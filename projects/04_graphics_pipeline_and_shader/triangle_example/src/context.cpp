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
        0.5f, 0.5f, 0.0f,   // top right 인덱스 : 0
        0.5f, -0.5f, 0.0f,  // bottom right 인덱스 : 1
        -0.5f, -0.5f, 0.0f, // bottom left 인덱스 : 2
        -0.5f, 0.5f, 0.0f,  // top left 인덱스 : 3
    };
    // 정점 인덱스를 담은 array
    uint32_t indices[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };
    // 순서주의; VAO 바인딩 -> VBO 바이딩 -> vertex attribute setting

    // VAO(Vertex Attribute Object) 생성 및 바인딩
    glGenVertexArrays(1, &m_vertexArrayObject); // VAO 생성
    glBindVertexArray(m_vertexArrayObject);     // 지금부터 사용할 VAO 지정

    // VBO(Vertex Buffer Object) 생성 및 바인딩
    glGenBuffers(1, &m_indexBuffer);                                             // 새로운 buffer object를 만든다
    glBindBuffer(GL_ARRAY_BUFFER, m_indexBuffer);                                // 지금부터 사용할 buffer object를 지정한다
                                                                                 // GL_ARRAY_BUFFER : 사용할 buffer object는 vertex data를 저장할 용도임을 알려줌
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, vertices, GL_STATIC_DRAW); // glBufferData : 지정된 buffer에 데이터를 복사한다
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

    // EBO(Element Buffer Object) 생성 및 바인딩
    // EBO는 딱히 Attribute를 설정할 필요는 없음. 왜냐하면 인덱스는 항상 양의 정수 값이기 때문
    glGenBuffers(1, &m_indexBuffer);                                                      // 새로운 buffer object를 만든다
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);                                 // 지금부터 사용할 buffer object를 지정한다
                                                                                          // GL_ELEMENT_ARRAY_BUFFER : 사용할 buffer object는 vertex index data를 저장할 용도임을 알려줌
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 6, indices, GL_STATIC_DRAW); // glBufferData : 지정된 buffer에 데이터를 복사한다
                                                                                          // 인덱스는 양의 정수니까 uint32_t

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
    // 현재 바인딩된 VAO, VBO, EBO를 바탕으로 그리기
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 첫번째 인자는 primitive: 그려낼 기본 primitive 타입,
                                                         // 두번째 인자는 그리고자 하는 EBO 내 index의 개수 6,
                                                         // 세번째 인자는 type: index의 데이터형, indices 배열은 uint32_t[]타입이기때문에 GL_UNSIGNED_INT,
                                                         // 네번째 인자는 pointer/offset: 그리고자 하는 EBO의 첫 데이터로부터의 오프셋
}
