#include "common.h"
#include "context.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// 윈도우의 프레임버퍼 크기가 변경되었을 때 호출하기 위한 콜백 정의
void OnFramebufferSizeChange(GLFWwindow *window, int width, int height) // 윈도우의 크기가 병경되었을 때마다 윈도우의 새로운 width와 height를 받아온다.
{
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    auto context = (Context *)glfwGetWindowUserPointer(window); // glfwGetWindowUserPointer()의 반환타입이 void*라서 형변환.
    context->Reshape(width, height);
}

// 키보드 입력 콜백 정의
void OnKeyEvent(GLFWwindow *window,
                int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods); // imgui key 콜백호출, 숫자를 키보드로 변경가능.

    SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
                key, scancode,
                action == GLFW_PRESS ? "Pressed" : action == GLFW_RELEASE ? "Released"
                                               : action == GLFW_REPEAT    ? "Repeat"
                                                                          : "Unknown",
                mods & GLFW_MOD_CONTROL ? "C" : "-",
                mods & GLFW_MOD_SHIFT ? "S" : "-",
                mods & GLFW_MOD_ALT ? "A" : "-");
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) // Escape 키 누르면 프로그램 종료
    {
        glfwSetWindowShouldClose(window, true);
    }
}

// 마우스 커서 콜백함수 정의
void OnCursorPos(GLFWwindow *window, double x, double y)
{
    auto context = (Context *)glfwGetWindowUserPointer(window);
    context->MouseMove(x, y);
}

// 마우스 클릭 콜백함수 정의
void OnMouseButton(GLFWwindow *window, int button, int action, int modifier)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier); // imgui mouse button 콜백 호출

    auto context = (Context *)glfwGetWindowUserPointer(window);
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    context->MouseButton(button, action, x, y);
}

void OnCharEvent(GLFWwindow *window, unsigned int ch)
{
    ImGui_ImplGlfw_CharCallback(window, ch);
}

void OnScroll(GLFWwindow *window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset); // imgui 스크롤 콜백 호출.
}

int main(int argc, const char **argv)
{
    SPDLOG_INFO("start program");

    // glfw 라이브러리 초기화, 실패하면 에러 출력후 종료
    SPDLOG_INFO("Initialize glfw");
    if (!glfwInit()) // glfw 라이브러리 초기화를 실패하면
    {
        const char *description = nullptr;
        glfwGetError(&description);                                 // 에러사유를 string으로 얻어와서
        SPDLOG_ERROR("failed to initialize glfw: {}", description); // 에러사유를 출력
        // fmt 라이브러리가 {} 자리에 description에 맞는 타입을 알아서 적용해줌.
        // printf("failed to initialize glfw: %s", description); 와 동일.
        return -1;
    }

    // GLFW 윈도우를 생성하기 전 만들기 희망하는 OpenGL 버전 추가
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw 윈도우 생성, 실패하면 에러 출력후 종료
    SPDLOG_INFO("Create glfw window");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME,
                                   nullptr, nullptr);
    // auto를 사용하게 되면 자동으로 glfwCreateWindow()의 반환타입인 GLFWwindow* 타입으로 적용됨. (c++ 11부터지원)
    // GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr); 와 동일.
    if (!window) // window에 실패하면 0 성공하면 GLFWwindow*가 들어있음.
    {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return -1;
    }

    // 윈도우 생성 후 함께 생성된 OpenGL Context를 사용
    glfwMakeContextCurrent(window);

    // glad를 활용한 OpenGL 함수 로딩
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return -1;
    }

    // 그래픽카드에 내장되어있는 opengl함수를 load하는데 성공하면 이 다음부터는 opengl 사용가능.

    auto glVersion = glGetString(GL_VERSION);             // glGetString()은 opengl 정보를 가져오는 함수.
    SPDLOG_INFO("OpenGL context version: {}", glVersion); // 출력 예) OpenGL context version: 3.3.0 NVIDIA 471.11
    // NVIDIA의 opengl 함수를 가져왔다는 뜻.

    // imgui 초기화 코드
    auto imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContext);
    ImGui_ImplGlfw_InitForOpenGL(window, false); // 첫 번째 인자는 *GLFWwindow, 두번째 인자는 callback 세팅여부. 콜백을 직접 설정할거라 false.
    ImGui_ImplOpenGL3_Init();                    // opengl을 사용해서 gui를 만들기때문에 imgui가 opengl3를 사용가능하게 함.
    ImGui_ImplOpenGL3_CreateFontsTexture();      // 사용하려고하는 font를 로딩해서 텍스쳐를 만듬. 텍스쳐 안쪽에 있는 글자를 오려서 화면에 렌더링.
    ImGui_ImplOpenGL3_CreateDeviceObjects();     // device는 texture나 shader를 의미.

    auto context = Context::Create();
    if (!context) // context 초기화 실패하면 nullptr
    {
        SPDLOG_ERROR("failed to create context");
        glfwTerminate();
        return -1;
    }

    glfwSetWindowUserPointer(window, context.get()); // user pointer를 통해서 context.get()을 통해서 context의 포인터값을 저장.

    // 정의한 콜백을 윈도우에 등록
    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);    // 윈도우 생성 직후에는 프레임버퍼 변경 이벤트가 발생하지 않으므로 첫 호출은 수동으로 함
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange); // glfwPollEvents()에서 이벤트를 수집하고, 그 이벤트에 해당하는 콜백을 세팅해놓으면 이벤트마다 실행됨.
    glfwSetKeyCallback(window, OnKeyEvent);
    glfwSetCharCallback(window, OnCharEvent);
    glfwSetCursorPosCallback(window, OnCursorPos);
    glfwSetMouseButtonCallback(window, OnMouseButton);
    glfwSetScrollCallback(window, OnScroll);

    // glfw 루프 실행, 윈도우 close 버튼을 누르면 정상 종료
    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window)) // glfwWindows가 닫혀야하나요? = false. 앞에 ! 있으니 true. 무한루프가 됨.
    {
        glfwPollEvents();          // 60분의 1초 단위마다 키보드 이벤트, 마우스 이벤트 등등 window와 관련된 이벤트 수집.
        ImGui_ImplGlfw_NewFrame(); // 새로운 프레임 생성. imgui는 매 프레임마다 화면을 그리기때문에 새로운 프레임을 생성해야함.
        // ImGui_ImplGlfw_NewFrame()가 GLFWwindow로부터 화면 크기 및 마우스 상태 등을 업데이트
        // 따라서 별도의 callback 연결 없이 마우스 인터렉션이 가능(창 크기변경, 드래그 등.)
        ImGui::NewFrame(); // 새로운 프레임이다라는 것을 imgui에게 알려줌.
        // context render
        context->ProcessInput(window);
        context->Render();
        // imgui render
        ImGui::Render();                                        // 실제로 그릴 리스트를 종합.
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // context에서 render한 프레임위에 imgui가 그림.

        glfwSwapBuffers(window); // front buffer와 back buffer가 있는데 back buffer에 먼저 그림을 그리고 front buffer와 바꿔치기(swap)을 하는 것.
                                 // 그림이 그려지는 과정이 보여지지 않게 하기 위함. 더블 버퍼링이라고함
    }
    // context 메모리 해제하는 2가지 방법
    context.reset();
    // context = nullptr // 소유권이 없어지는 시점에서 메모리 해제

    // imgui init의 역순으로 메모리해제
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);

    glfwTerminate(); // glflw라이브러리 종료

    return 0; // The program '[18228] cmake_project_example.exe' has exited with code 0 (0x0). 마지막에 0이 여기서 return한 0
}
