cmake_minimum_required(VERSION 3.13) # cmake 3.14이상 버전에서만 사용

set(PROJECT_NAME imgui_example) # set()함수는 변수 세팅
set(CMAKE_CXX_STANDARD 17) # CMAKE로 시작하는 변수는 글로별 변수. CMAKE 세팅을 바꿀 수 있다. c++ standard는 17버전을 쓰겠다는 뜻.

set(WINDOW_NAME "Imgui Example")
set(WINDOW_WIDTH 960)
set(WINDOW_HEIGHT 540)

project(${PROJECT_NAME}) # 변수사용법 : ${변수명} project 함수 아래로 본격적으로 프로젝트를 세팅한다.
add_executable(${PROJECT_NAME}
  src/main.cpp
  src/common.cpp src/common.h
  src/shader.cpp src/shader.h
  src/program.cpp src/program.h
  src/context.cpp src/context.h
  src/buffer.cpp src/buffer.h
  src/vertex_layout.cpp src/vertex_layout.h
  src/image.cpp src/image.h
  src/texture.cpp src/texture.h
  ) # cpp파일들을 컴파일해서 실행파일을 만든다. 
    # 컴파일은 cpp파일만 하는 것이지만 헤더파일까지 포함시키는 이유는 해더파일이 변경되었을 때 컴파일을 다시 시키기위하여

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


if (MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC /wd4819)
endif()
