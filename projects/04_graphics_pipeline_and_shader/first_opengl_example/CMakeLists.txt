cmake_minimum_required(VERSION 3.14) # cmake 3.14이상 버전에서만 사용

set(PROJECT_NAME first_opengl_example) # set()함수는 변수 세팅
set(CMAKE_CXX_STANDARTD 17) # CMAKE로 시작하는 변수는 글로별 변수. CMAKE 세팅을 바꿀 수 있다. c++ standard는 17버전을 쓰겠다는 뜻.

set(WINDOW_NAME "First OpenGL Example")
set(WINDOW_WIDTH 960)
set(WINDOW_HEIGHT 540)

project(${PROJECT_NAME}) # 변수사용법 : ${변수명} project 함수 아래로 본격적으로 프로젝트를 세팅한다.
add_executable(${PROJECT_NAME} src/main.cpp) # src/main.cpp를 컴파일해서 실행파일을 만든다.

include(Dependency.cmake)

# 우리 프로젝트에 include / lib 관련 옵션 추가
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

# 환경 변수를 predefined macro로 프로젝트에 추가	
target_compile_definitions(${PROJECT_NAME} PUBLIC
  WINDOW_NAME="${WINDOW_NAME}" # #define WINDOW_NAME "{WINDOW_NAME}"과 같은효과
  WINDOW_WIDTH=${WINDOW_WIDTH}
  WINDOW_HEIGHT=${WINDOW_HEIGHT}
  )

# Dependency들이 먼저 build 될 수 있게 관계 설정
add_dependencies(${PROJECT_NAME} ${DEP_LIST})