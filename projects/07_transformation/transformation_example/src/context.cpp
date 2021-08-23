#include "context.h"
#include "image.h"

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
    // [x, y, z, s, t]
    float vertices[] = {
        -0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        0.0f,
        0.5f,
        -0.5f,
        -0.5f,
        1.0f,
        0.0f,
        0.5f,
        0.5f,
        -0.5f,
        1.0f,
        1.0f,
        -0.5f,
        0.5f,
        -0.5f,
        0.0f,
        1.0f,

        -0.5f,
        -0.5f,
        0.5f,
        0.0f,
        0.0f,
        0.5f,
        -0.5f,
        0.5f,
        1.0f,
        0.0f,
        0.5f,
        0.5f,
        0.5f,
        1.0f,
        1.0f,
        -0.5f,
        0.5f,
        0.5f,
        0.0f,
        1.0f,

        -0.5f,
        0.5f,
        0.5f,
        1.0f,
        0.0f,
        -0.5f,
        0.5f,
        -0.5f,
        1.0f,
        1.0f,
        -0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        1.0f,
        -0.5f,
        -0.5f,
        0.5f,
        0.0f,
        0.0f,

        0.5f,
        0.5f,
        0.5f,
        1.0f,
        0.0f,
        0.5f,
        0.5f,
        -0.5f,
        1.0f,
        1.0f,
        0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        1.0f,
        0.5f,
        -0.5f,
        0.5f,
        0.0f,
        0.0f,

        -0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        1.0f,
        0.5f,
        -0.5f,
        -0.5f,
        1.0f,
        1.0f,
        0.5f,
        -0.5f,
        0.5f,
        1.0f,
        0.0f,
        -0.5f,
        -0.5f,
        0.5f,
        0.0f,
        0.0f,

        -0.5f,
        0.5f,
        -0.5f,
        0.0f,
        1.0f,
        0.5f,
        0.5f,
        -0.5f,
        1.0f,
        1.0f,
        0.5f,
        0.5f,
        0.5f,
        1.0f,
        0.0f,
        -0.5f,
        0.5f,
        0.5f,
        0.0f,
        0.0f,
    };
    // 정점 인덱스를 담은 array
    // 한 면당 2개의 삼각형이 필요하므로 12개의 삼각형이 필요.
    uint32_t indices[] = {
        0,
        2,
        1,
        2,
        0,
        3,
        4,
        5,
        6,
        6,
        7,
        4,
        8,
        9,
        10,
        10,
        11,
        8,
        12,
        14,
        13,
        14,
        12,
        15,
        16,
        17,
        18,
        18,
        19,
        16,
        20,
        22,
        21,
        22,
        20,
        23,
    };
    // 순서주의; VAO 바인딩 -> VBO 바이딩 -> vertex attribute setting
    // vertex attribute을 설정하기 전에 VBO가 바인딩 되어있을 것

    // VAO(Vertex Attribute Object) 생성 및 바인딩
    m_vertexLayout = VertexLayout::Create();

    // VBO(Vertex Buffer Object) 생성, 바인딩, 정점 데이터 추가
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 120);

    // vertex attribute setting
    // m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 3); // 텍스쳐 좌표는 2차원 이므로 두 번째 인자는 2

    // EBO(Element Buffer Object) 생성, 바인딩, 인덱스 데이터 추가
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 36);

    // shader 생성
    // Shader 인스턴스가 unique_ptr에서 shared_ptr로 변환되었음을 유의
    ShaderPtr vertShader = Shader::CreateFromFile("./shader/texture.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/texture.fs", GL_FRAGMENT_SHADER);

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

    // image 로드
    auto image2 = Image::Load("./image/awesomeface.png");
    if (!image2)
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels", image2->GetWidth(), image2->GetHeight(), image2->GetChannelCount());

    auto image = Image::Load("./image/container.jpg");
    if (!image)
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());

    m_texture = Texture::CreateFromImage(image.get());

    m_texture2 = Texture::CreateFromImage(image2.get());

    glActiveTexture(GL_TEXTURE0);                   // glActiveTexture(textureSlot) : 함수로 현재 다루고자 하는 텍스처 슬롯을 선택
    glBindTexture(GL_TEXTURE_2D, m_texture->Get()); // glBindTexture(textureType, textureId) : 함수로 현재 설정중인 텍스처 슬롯에 우리의 텍스처 오브젝트를 바인딩
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

    m_program->Use();
    m_program->SetUniform("tex", 0);
    m_program->SetUniform("tex2", 1);

    // x축중심 -55도 회전
    auto model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // 카메라는 원점으로부터 z축 방향으로 -3만큼 떨어짐
    auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    // 종횡비 4:3, 세로화각 45도의 원근 투영
    auto projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 10.0f);
    auto transform = projection * view * model;

    m_program->SetUniform("transform", transform);

    return true;
}

void Context::Render()
{
    // 큐브를 회전.
    auto projection = glm::perspective(glm::radians(45.0f), (float)640 / (float)480, 0.01f, 10.0f);
    auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    // glfwGetTime()는 glfwInit()함수호출이후 시간이 얼마나 흘렀는가를 초로 반환.
    // (1.0, 0.5, 0.0) 축을 기준으로 1초에 120도씩(3초에 1바퀴) 회전.
    auto model = glm::rotate(glm::mat4(1.0f), glm::radians((float)glfwGetTime() * 120.0f), glm::vec3(1.0f, 0.5f, 0.0f));

    auto transform = projection * view * model;

    m_program->SetUniform("transform", transform);

    // 깊이 테스트 (Depth test) : 어떤 픽셀의 값을 업데이트 하기 전, 현재 그리려는 픽셀의 z값과 깊이 버퍼에 저장된 해당 위치의 z값을 비교해 봄.
    //                           비교 결과 현재 그리려는 픽셀이 이전에 그려진 픽셀보다 뒤에 있을 경우 픽셀을 그리지 않음

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 각 픽셀의 컬러 값을 저장하는 버퍼 외에, 해당 픽셀의 깊이값 (z축값)을 저장.
                                                        // OpenGL의 Depth Buffer 초기값은 1. 1이 가장 뒤에 있고, 0이 가장 앞을 의미 (왼손 좌표계)
    glEnable(GL_DEPTH_TEST);                            // 깊이 테스트를 켠다.
                                                        // glDepthFunc()을 이용하여 깊이 테스트 통과 조건을 변경할 수 있음. 깊이 테스트 통과 조건의 기본값은 GL_LESS.
                                                        // depth가 작은 값을 화면에 그림
    m_program->Use();
    // 현재 바인딩된 VAO, VBO, EBO를 바탕으로 그리기
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // 첫번째 인자는 primitive: 그려낼 기본 primitive 타입,
                                                          // 두번째 인자는 그리고자 하는 EBO 내 index의 개수 6,
                                                          // 세번째 인자는 type: index의 데이터형, indices 배열은 uint32_t[]타입이기때문에 GL_UNSIGNED_INT,
                                                          // 네번째 인자는 pointer/offset: 그리고자 하는 EBO의 첫 데이터로부터의 오프셋
}
