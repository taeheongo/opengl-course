#include "common.h"
#include <fstream>
#include <sstream>

std::optional<std::string> LoadTextFile(const std::string &filename)
{
    std::ifstream fin(filename);
    if (!fin.is_open()) // file을 읽는데 실패하면
    {
        SPDLOG_ERROR("failed to open file: {}", filename);
        return {}; // optional 타입일때 {}를 쓰면 아무것도 아닌 값을 리턴
    }
    std::stringstream text;
    text << fin.rdbuf();
    return text.str(); // string 타입으로 반환
}