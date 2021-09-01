# ExternalProject 관련 명령어 셋 추가
include(ExternalProject)

# Dependency 관련 변수 설정
set(DEP_INSTALL_DIR ${PROJECT_BINARY_DIR}/install)
set(DEP_INCLUDE_DIR ${DEP_INSTALL_DIR}/include)
set(DEP_LIB_DIR ${DEP_INSTALL_DIR}/lib)

# spdlog: fast logger library
ExternalProject_Add(
    dep-spdlog
    GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
    GIT_TAG "v1.x"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
    TEST_COMMAND ""
)
# Dependency 리스트 및 라이브러리 파일 리스트 추가
set(DEP_LIST ${DEP_LIST} dep-spdlog)
set(DEP_LIBS ${DEP_LIBS} spdlog$<$<CONFIG:Debug>:d>)

# glfw
ExternalProject_Add(
    dep_glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "3.3.2"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
        -DGLFW_BUILD_EXAMPLES=OFF
        -DGLFW_BUILD_TESTS=OFF
        -DGLFW_BUILD_DOCS=OFF
    TEST_COMMAND ""
    )
set(DEP_LIST ${DEP_LIST} dep_glfw)
set(DEP_LIBS ${DEP_LIBS} glfw3)

# glad
ExternalProject_Add(
    dep_glad
    GIT_REPOSITORY "https://github.com/Dav1dde/glad"
    GIT_TAG "v0.1.34"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
        -DGLAD_INSTALL=ON
    TEST_COMMAND ""
    )
set(DEP_LIST ${DEP_LIST} dep_glad)
set(DEP_LIBS ${DEP_LIBS} glad)

# stb
ExternalProject_Add(
    dep_stb
    GIT_REPOSITORY "https://github.com/nothings/stb"
    GIT_TAG "master"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    TEST_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_BINARY_DIR}/dep_stb-prefix/src/dep_stb/stb_image.h
        ${DEP_INSTALL_DIR}/include/stb/stb_image.h
    )
set(DEP_LIST ${DEP_LIST} dep_stb)

# glm
ExternalProject_Add(
  dep_glm
  GIT_REPOSITORY "https://github.com/g-truc/glm"
  GIT_TAG "0.9.9.8"
  GIT_SHALLOW 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  TEST_COMMAND ""
  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_BINARY_DIR}/dep_glm-prefix/src/dep_glm/glm
        ${DEP_INSTALL_DIR}/include/glm
  )
set(DEP_LIST ${DEP_LIST} dep_glm)

# imgui
add_library(imgui
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
    ) # 헤더파일을 포함하지 않는 이유는 헤더가 안 바뀔 것이기 때문에 cpp파일만 컴파일.
    # add_library를 통해서 외부프로젝트가아닌 내부 프로젝트를 추가하는 경우 컴파일하면 imguil.lib파일이 install/lib이 아닌 Debug폴더안에 추가된다.

target_include_directories(imgui PRIVATE ${DEP_INCLUDE_DIR}) # imgui 타켓의 include 디렉토리는 ${DEP_INCLUDE_DIR} 이다.
add_dependencies(imgui ${DEP_LIST}) # DEP_LIST가 먼저 컴파일되고 난후 imgui컴파일.
set(DEP_INCLUDE_DIR ${DEP_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgui) # DEP_INCLUDE_DIR에 ${CMAKE_CURRENT_SOURCE_DIR}/imgui 경로 추가
set(DEP_LIST ${DEP_LIST} imgui)
set(DEP_LIBS ${DEP_LIBS} imgui)

# assimp
ExternalProject_Add(
  dep_assimp
  GIT_REPOSITORY "https://github.com/assimp/assimp"
  GIT_TAG "v5.0.1"
  GIT_SHALLOW 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
      -DBUILD_SHARED_LIBS=OFF
      -DASSIMP_BUILD_ASSIMP_TOOLS=OFF
      -DASSIMP_BUILD_TESTS=OFF
      -DASSIMP_INJECT_DEBUG_POSTFIX=OFF
      -DASSIMP_BUILD_ZLIB=ON
  TEST_COMMAND ""
  )
set(DEP_LIST ${DEP_LIST} dep_assimp)
set(DEP_LIBS ${DEP_LIBS}
  assimp-vc142-mt$<$<CONFIG:Debug>:d>
  zlibstatic$<$<CONFIG:Debug>:d>
  IrrXML$<$<CONFIG:Debug>:d>
  )