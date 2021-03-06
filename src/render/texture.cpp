#include "render/texture.h"

#include <iostream>

#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture load_texture(const std::string &texture_path)
{
    Texture texture;
    texture.id = 0; // Default parameters if fails to load texture
    texture.width = 1;
    texture.height = 1;

    unsigned char *data = stbi_load(texture_path.c_str(), &texture.width, &texture.height, &texture.num_channels, 0);

    if (!data) {
        std::cout << "Failed to load " << texture_path << std::endl;
        return texture;
    } else {
        glGenTextures(1, &texture.id);
        glBindTexture(GL_TEXTURE_2D, texture.id);
        switch (texture.num_channels) {
            case 1: // depth (eg: specular or normal map)
                glTexImage2D(
                    GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                    texture.width, texture.height, 0, GL_DEPTH_COMPONENT,
                    GL_UNSIGNED_BYTE, data);
                break;
            case 3: // rgb
                glTexImage2D(
                    GL_TEXTURE_2D, 0, GL_RGB,
                    texture.width, texture.height, 0, GL_RGB,
                    GL_UNSIGNED_BYTE, data);
                break;
            case 4: // rgba
                glTexImage2D(
                    GL_TEXTURE_2D, 0, GL_RGBA,
                    texture.width, texture.height, 0, GL_RGBA,
                    GL_UNSIGNED_BYTE, data);
                break;
            default:
                std::cout << "Unsupported numer of channels: " << texture.num_channels << std::endl;
                return texture;
        }
        glGenerateMipmap(GL_TEXTURE_2D);
        // Avoid repeating images. Only works if tex_coords are allowed
        // outside the [0, 1] range, which isn't allowed when using
        // normalised values (which is more efficient)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    return texture;
}
