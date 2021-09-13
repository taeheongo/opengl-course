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
    m_box = Mesh::CreateBox();

    // program 생성
    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
    if (!m_simpleProgram)
        return false;
    SPDLOG_INFO("program id: {}", m_simpleProgram->Get());

    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");

    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    glClearColor(0.0f, 0.1f, 0.2f, 0.0f); // 화면을 지울 색상 지정을 컬러버퍼에 설정.

    // image 로드
    auto image = Image::Load("./image/container.jpg");
    if (!image)
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());

    m_texture = Texture::CreateFromImage(image.get());

    auto image2 = Image::Load("./image/awesomeface.png");
    if (!image2)
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels", image2->GetWidth(), image2->GetHeight(), image2->GetChannelCount());

    m_texture2 = Texture::CreateFromImage(image2.get());

    TexturePtr darkGrayTexture = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)).get());

    TexturePtr grayTexture = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());

    m_planeMaterial = Material::Create();
    m_planeMaterial->diffuse = Texture::CreateFromImage(Image::Load("./image/marble.jpg").get());
    m_planeMaterial->specular = grayTexture;
    m_planeMaterial->shininess = 128.0f;

    m_box1Material = Material::Create();
    m_box1Material->diffuse = Texture::CreateFromImage(Image::Load("./image/container.jpg").get());
    m_box1Material->specular = darkGrayTexture;
    m_box1Material->shininess = 16.0f;

    m_box2Material = Material::Create();
    m_box2Material->diffuse = Texture::CreateFromImage(Image::Load("./image/container2.png").get());
    m_box2Material->specular = Texture::CreateFromImage(Image::Load("./image/container2_specular.png").get());
    m_box2Material->shininess = 64.0f;

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

        // ImGuiTreeNodeFlags_DefaultOpen를 ImGui::CollapsingHeader의 두번째 인자로 주면 처음에 접혀있지 않고 열려있음.
        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.5f, 0.0f, 180.0f); // max 값이 180.0f가 되면 point light랑 같아짐.
            ImGui::DragFloat("l.distance", &m_light.distance, 0.5f, 0.0f, 3000.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
            ImGui::Checkbox("flash light", &m_flashLightMode);
        }

        ImGui::Checkbox("animation", &m_animation);
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
        m_cameraPos,                                                                                           // 카메라 위치 EYE
        m_cameraPos + m_cameraFront,                                                                           // EYE + n = AT
        m_cameraUp);                                                                                           // UP
    auto projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.05f, 1000.0f); // (fovy, aspect, near, far) far를 크게해주면 잘리는것을 막을 수 있음.

    // model에 대한 uniform변수들 설정.
    glm::vec3 lightPos = m_light.position;
    glm::vec3 lightDir = m_light.direction;

    if (m_flashLightMode)
    {
        lightPos = m_cameraPos;
        lightDir = m_cameraFront;
    }
    else
    {
        // 광원에 대한 uniform변수들 설정.
        auto lightModelTransform = glm::translate(glm::mat4(1.0), m_light.position) * glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
        m_simpleProgram->Use();
        m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
        m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
        m_box->Draw(m_simpleProgram.get());
    }

    m_program->Use();
    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("light.position", lightPos);
    m_program->SetUniform("light.direction", lightDir);
    m_program->SetUniform("light.cutoff", glm::vec2(
                                              cosf(glm::radians(m_light.cutoff[0])),
                                              cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_program->SetUniform("light.ambient", m_light.ambient);
    m_program->SetUniform("light.diffuse", m_light.diffuse);
    m_program->SetUniform("light.specular", m_light.specular);

    auto modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
    auto transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_planeMaterial->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_box1Material->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.749f, 2.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_box2Material->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());
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