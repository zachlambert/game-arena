#include "game/system.h"

static void update_entity(component::Transform &transform, component::Polygon &polygon)
{
    polygon.model = glm::translate(glm::vec3(transform.pos.x, transform.pos.y, (double)polygon.depth))
        * glm::rotate((float)transform.orientation, glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(transform.scale.x, transform.scale.y, 1));
}

void system_render_polygon(EntityManager &entity_manager)
{
    component::Transform *transform;
    component::Polygon *polygon;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::RENDER_POLYGON)) continue;
        transform = entity_manager.get_transform_component(i, 0);
        polygon = entity_manager.get_polygon_component(i, 0);
        update_entity(*transform, *polygon);
    }
}
