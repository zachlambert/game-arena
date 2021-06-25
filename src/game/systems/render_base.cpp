#include "game/systems.h"

void update_entity(component::Transform &transform, component::VisualStatic &visual_static)
{
    visual_static.model = glm::translate(glm::vec3(transform.pos.x, transform.pos.y, (double)visual_static.depth))
        * glm::rotate((float)transform.orientation, glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(transform.scale.x, transform.scale.y, 1));
}

void system_render_base(EntityManager &entity_manager)
{
    component::Transform *transform;
    component::VisualStatic *visual_static;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::RENDER_BASE)) continue;
        transform = entity_manager.get_transform_component(i, 0);
        visual_static = entity_manager.get_visual_static_component(i, 0);
        update_entity(*transform, *visual_static);
    }
};

