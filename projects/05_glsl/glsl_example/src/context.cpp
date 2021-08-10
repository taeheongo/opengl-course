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
    // vertex attribute을 설정하기 전에 VBO가 바인딩 되어있을 것

    // VAO(Vertex Attribute Object) 생성 및 바인딩
    m_vertexLayout = VertexLayout::Create();

    // VBO(Vertex Buffer Object) 생성, 바인딩, 정점 데이터 추가
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 12);

    // vertex attribute setting
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    // EBO(Element Buffer Object) 생성, 바인딩, 인덱스 데이터 추가
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);

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

    auto loc = glGetUniformLocation(m_program->Get(), "color"); // glGetUniformLocation()을 사용하여 program object로부터 uniform handle을 얻는다
    m_program->Use();                                           // program이 바인딩 된 상태에서
    glUniform4f(loc, 1.0f, 1.0f, 0.0f, 1.0f);                   // glUniform...()를 사용하여 값을 입력한다

    glClearColor(0.0f, 0.1f, 0.2f, 0.0f); // 화면을 지울 색상 지정을 컬러버퍼에 설정.

    return true;
}

void Context::Render()
{
    glClear(GL_COLOR_BUFFER_BIT); // 컬러버퍼 안에 있는 색상으로 화면을 지움.

    static float time = 0.0f; // static이기때문에 함수가 종료되어도 유지된다.
                              // main함수 while루프에서 Render함수를 60분의 1초마다 호출하기때문에 60분의1초마다 time값이 60분의 1씩증가함.
    float t = sinf(time) * 0.5f + 0.5f;

    auto loc = glGetUniformLocation(m_program->Get(), "color");
    m_program->Use();
    glUniform4f(loc, t * t, 2.0f * t * (1.0f - t), (1.0f - t) * (1.0f - t), 1.0f);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    time += 0.016f;

    // m_program->Use();
    // // 현재 바인딩된 VAO, VBO, EBO를 바탕으로 그리기
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 첫번째 인자는 primitive: 그려낼 기본 primitive 타입,
    //                                                      // 두번째 인자는 그리고자 하는 EBO 내 index의 개수 6,
    //                                                      // 세번째 인자는 type: index의 데이터형, indices 배열은 uint32_t[]타입이기때문에 GL_UNSIGNED_INT,
    //                                                      // 네번째 인자는 pointer/offset: 그리고자 하는 EBO의 첫 데이터로부터의 오프셋
}
