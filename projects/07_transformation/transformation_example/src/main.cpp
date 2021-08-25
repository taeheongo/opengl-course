#include "common.h"
#include "context.h"
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

    // glfw 루프 실행, 윈도우 close 버튼을 누르면 정상 종료
    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window)) // glfwWindows가 닫혀야하나요? = false. 앞에 ! 있으니 true. 무한루프가 됨.
    {
        glfwPollEvents(); //  60분의 1초 단위마다 키보드 이벤트, 마우스 이벤트 등등 window와 관련된 이벤트 수집.
        // render
        context->ProcessInput(window);
        context->Render();
        glfwSwapBuffers(window); // front buffer와 back buffer가 있는데 back buffer에 먼저 그림을 그리고 front buffer와 바꿔치기(swap)을 하는 것.
                                 // 그림이 그려지는 과정이 보여지지 않게 하기 위함. 더블 버퍼링이라고함
    }
    // context 메모리 해제하는 2가지 방법
    context.reset();
    // context = nullptr // 소유권이 없어지는 시점에서 메모리 해제

    glfwTerminate(); // glflw라이브러리 종료
    return 0;        // The program '[18228] cmake_project_example.exe' has exited with code 0 (0x0). 마지막에 0이 여기서 return한 0
}