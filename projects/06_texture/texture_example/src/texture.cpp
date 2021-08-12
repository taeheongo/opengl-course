#include "texture.h"

TextureUPtr Texture::CreateFromImage(const Image *image)
{
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();
    texture->SetTextureFromImage(image);
    return std::move(texture);
}

Texture::~Texture()
{
    if (m_texture)
    {
        glDeleteTextures(1, &m_texture);
    }
}

void Texture::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_texture); // 사용하고자 하는 텍스처 바인딩
}

void Texture::SetFilter(uint32_t minFilter, uint32_t magFilter) const
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::SetWrap(uint32_t sWrap, uint32_t tWrap) const
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}

void Texture::CreateTexture()
{
    glGenTextures(1, &m_texture); // OpenGL texture object 생성
    // bind and set default filter and wrap option
    Bind();
    SetFilter(GL_LINEAR, GL_LINEAR);
    SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Texture::SetTextureFromImage(const Image *image)
{
    GLenum format = GL_RGBA;
    switch (image->GetChannelCount()) // image의 channel수에 따라 format 결정.
    {
    default:
        break;
    case 1:
        format = GL_RED;
        break;
    case 2:
        format = GL_RG;
        break;
    case 3:
        format = GL_RGB;
        break;
    }

    // glTexImage2D(target, level, internalFormat, width, height, border, format, type, data)
    // 바인딩된 텍스처의 크기 픽셀 포맷을 설정하고 GPU에 이미지 데이터를 복사
    // cpu에서의 image의 channel수가 GL_RGB인데 GPU에서의 texture의 format이 GL_RGBA이면 Alpha 채널이 다 255로 들어가게 된다.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 image->GetWidth(), image->GetHeight(), 0,
                 format, GL_UNSIGNED_BYTE,
                 image->GetData());
    // target : GL_TEXTURE_2D
    // level : 0은 기본 이미지 크기, 커지면 커질수록 이미지 크기가 줄어든다.
    // 3,4,5,6번째 인자는 gpu의 texture에 대한 정보:
    //      internalFormat : 채널 타입. GL_RED, GL_RGB, GL_RG, GL_RGBA8
    //      width: 텍스처 / 이미지의 가로 크기,
    //             이미지의 크기는 2^n * 2^n 형태일때 효율이 극대화 됨. ex) 512 * 512, 256* 256, ...,
    //             2^n의 크기가 아닌 이미지크기를 NPOT(Non Power Of Two)라고 함.(gpu의 스펙에따라 이런 크기를 가지는 텍스쳐를 지원하지 않을 수 도 있음.)
    //      height: 텍스처 / 이미지의 세로 크기
    //      border: 텍스처 외곽의 border 크기
    // 7, 8, 9번째 인자는 이미지에 대한 정보:
    //      format: 입력하는 이미지의 픽셀 포맷
    //      type: 입력하는 이미지의 채널별 데이터 타입
    //      data: 이미지 데이터가 기록된 메모리 주소
}
