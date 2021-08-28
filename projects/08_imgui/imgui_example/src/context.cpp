#include "context.h"
#include "image.h"
#include <imgui.h> // common.h에 include하면 대부분의 코드는 common.h를 사용하기때문에 모든 파일에서 imgui 사용가능.
                   // context.h에 include하면 main.cpp와 context.cpp에서 imgui 사용가능.
                   // context.cpp에 include하면 context.cpp에서 사용가능. context.cpp에서만 사용할거기때문에 여기에 include.

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

    return true;
}

void Context::Render()
{
    if (ImGui::Begin("ui window")) // begin ~ end사이의 코드가 imgui 윈도우 내용, my first ImGui window가 제목.
                                   // 윈도우를 접으면 ImGui::Begin()의 값이 false가 되고 if문 안의 내용이 실행되지 않는다.
    {
        if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor)))
        {
            glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
        }
        ImGui::Separator();
        // 드래그 ui를 통해서 카메라 위치, 회전방향 제어.
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5, -89.0f, 89.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera"))
        {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }
    }
    ImGui::End();

    // 깊이 테스트 (Depth test) : 어떤 픽셀의 값을 업데이트 하기 전, 현재 그리려는 픽셀의 z값과 깊이 버퍼에 저장된 해당 위치의 z값을 비교해 봄.
    //                           비교 결과 현재 그리려는 픽셀이 이전에 그려진 픽셀보다 뒤에 있을 경우 픽셀을 그리지 않음

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 각 픽셀의 컬러 값을 저장하는 버퍼 외에, 해당 픽셀의 깊이값 (z축값)을 저장.
                                                        // OpenGL의 Depth Buffer 초기값은 1. 1이 가장 뒤에 있고, 0이 가장 앞을 의미 (왼손 좌표계)

    glEnable(GL_DEPTH_TEST); // 깊이 테스트를 켠다.
                             // glDepthFunc()을 이용하여 깊이 테스트 통과 조건을 변경할 수 있음. 깊이 테스트 통과 조건의 기본값은 GL_LESS.
                             // depth가 작은 값을 화면에 그림

    // m_cameraFront = vec4(0, 0, -1, 0)를 y축중심 m_cameraYaw, x축중심 m_cameraPitch만큼 회전한 방향벡터.
    m_cameraFront =
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    auto view = glm::lookAt(
        m_cameraPos,                                                                                         // 카메라 위치 EYE
        m_cameraPos + m_cameraFront,                                                                         // EYE + n = AT
        m_cameraUp);                                                                                         // UP
    auto projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.01f, 30.0f); // (fovy, aspect, near, far) far를 크게해주면 잘리는것을 막을 수 있음.

    // 여러개의 회전하는 큐브
    // 큐브마다 translate해줄 값을 cubePositions에 저장.
    std::vector<glm::vec3> cubePositions = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };

    for (size_t i = 0; i < cubePositions.size(); i++)
    {
        auto &pos = cubePositions[i];
        auto model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::rotate(model, glm::radians((float)glfwGetTime() * 120.0f + 20.0f * (float)i), glm::vec3(1.0f, 0.5f, 0.0f));
        auto transform = projection * view * model;

        m_program->SetUniform("transform", transform);

        // 현재 바인딩된 VAO, VBO, EBO를 바탕으로 그리기
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // 첫번째 인자는 primitive: 그려낼 기본 primitive 타입,
                                                              // 두번째 인자는 그리고자 하는 EBO 내 index의 개수 6,
                                                              // 세번째 인자는 type: index의 데이터형, indices 배열은 uint32_t[]타입이기때문에 GL_UNSIGNED_INT,
                                                              // 네번째 인자는 pointer/offset: 그리고자 하는 EBO의 첫 데이터로부터의 오프셋
    }
}

void Context::ProcessInput(GLFWwindow *window)
{
    if (!m_cameraControl)
        return;

    const float cameraSpeed = 0.05f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront)); // u벡터(카메라 기저에서의 x축)
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight)); //  v벡터(카메라 좌표계에서의 y축)
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;
}

void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y)
{
    if (!m_cameraControl)
        return;

    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.8f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;   // 도리각 (x변화량 * 회전속도조절계수)만큼의 각
    m_cameraPitch -= deltaPos.y * cameraRotSpeed; // 끄덕각 (y변화량 * 회전속도조절계수)만큼의 각

    // 0 ~ 360도 사이로 조정.
    if (m_cameraYaw < 0.0f)
        m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f)
        m_cameraYaw -= 360.0f;

    // -90 ~ 90도 사이로 조정.
    if (m_cameraPitch > 89.0f)
        m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f)
        m_cameraPitch = -89.0f;

    m_prevMousePos = pos; // 이전좌표 갱신
}

void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            // 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_cameraControl = false;
        }
    }
}