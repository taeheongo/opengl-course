#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

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

    // glfw 루프 실행, 윈도우 close 버튼을 누르면 정상 종료
    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window)) // glfwWindows가 닫혀야하나요? = false. 앞에 ! 있으니 true. 무한루프가 됨.
    {
        glfwPollEvents(); //  60분의 1초 단위마다 키보드 이벤트, 마우스 이벤트 등등 window와 관련된 이벤트 수집.
    }

    glfwTerminate(); // glflw라이브러리 종료
    return 0;        // The program '[18228] cmake_project_example.exe' has exited with code 0 (0x0). 마지막에 0이 여기서 return한 0
}