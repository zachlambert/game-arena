#include "game/system.h"

void update_entity(component::Transform &transform, component::Sprite &sprite)
{
    sprite.model = glm::translate(glm::vec3(transform.pos.x, transform.pos.y, (double)sprite.depth))
        * glm::rotate((float)transform.orientation, glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(transform.scale.x, transform.scale.y, 1));
}

void system_render_sprite(EntityManager &entity_manager)
{
    component::Transform *transform;
    component::Sprite *sprite;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::RENDER_SPRITE)) continue;
        transform = entity_manager.get_transform_component(i, 0);
        sprite = entity_manager.get_sprite_component(i, 0);
        update_entity(*transform, *sprite);
    }
};

