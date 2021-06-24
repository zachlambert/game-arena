#include "render/vertex.h"

Vertex::Vertex(glm::vec3 position, glm::vec2 tex_coords)
{
    // Position straightforward
    this->position = position;
    // Tex coords: Need to convert to normalised ushort
    // If outside the [0, 1] range, clamp coordinates
    this->tex_coords[0] = static_cast<unsigned short>(
        glm::clamp(tex_coords.x, 0.0f, 1.0f) * (float)0xFFFF);
    this->tex_coords[1] = static_cast<unsigned short>(
        glm::clamp(tex_coords.y, 0.0f, 1.0f) * (float)0xFFFF);
}

