#ifndef __COMMON_H__
#define __COMMON_H__

#include <memory>
#include <string>
#include <optional>
#include <vector>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define CLASS_PTR(klassName)                            \
    class klassName;                                    \
    using klassName##UPtr = std::unique_ptr<klassName>; \
    using klassName##Ptr = std::shared_ptr<klassName>;  \
    using klassName##WPtr = std::weak_ptr<klassName>;

/*
    CLASS_PTR(Shader)를 사용할 경우 아래 4줄을 의미.

    class Shader;                                    W
    using ShaderUPtr = std::unique_ptr<Shader>;  // using은 type aliasing
    using ShaderPtr = std::shared_ptr<Shader>;
    using ShaderWPtr = std::weak_ptr<Shader>;
*/

std::optional<std::string> LoadTextFile(const std::string &filename); // std::string* 타입으로 반환하면 그 포인터는 동적할당된 메모리의 주소값을 가질것이고,
                                                                      // 그 메모리를 해제를 해줘야하는데 해제를 잊어버리면 그 메모리가 누수가됨. 그래서 optional을 사용함.
                                                                      // optional은 값이 들어있으면 꺼내서 쓸 수 있고, 값이 없으면 꺼낼 수 없음.

#endif // __COMMON_H__