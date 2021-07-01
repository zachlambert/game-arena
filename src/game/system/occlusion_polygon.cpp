#include "game/system.h"

static void update_polygon(
    const component::Transform &player_transform,
    component::Polygon &polygon,
    const CollisionManager &collision_manager,
    const Camera &camera)
{
    double fov = 150.0 * (M_PI/180);
    double angle = player_transform.orientation + fov/2;

    if (polygon.vertices.size() < 7) polygon.vertices.resize(7);
    if (polygon.indices.size() < 15) polygon.indices.resize(15);
    polygon.vertices[0] = camera.project_point({camera.screen_width/2, camera.screen_height/2});

    int i = 0;
    glm::vec2 screen_pos;
    while (i <= 5) {
        screen_pos.x = camera.screen_width/2 + cos(angle) * 2*camera.screen_width;
        screen_pos.y = camera.screen_height/2 - sin(angle) * 2*camera.screen_width;
        polygon.vertices[i+1] = camera.project_point(screen_pos);
        angle += (2*M_PI - fov)/4;
        i++;
    }
    for (int i = 0; i < 5; i++) {
        polygon.indices[3*i] = 0;
        polygon.indices[3*i+1] = i+1;
        polygon.indices[3*i+2] = i+2;
    }
    polygon.dirty = true;

    polygon.vertices.resize(16);
    polygon.indices.resize(16);
    polygon.element_count = 14;

    polygon.colors.resize(7);
    glm::vec4 color(0.5, 0.5, 0.5, 1);
    for (int i = 0; i < 7; i++) polygon.colors[i] = color;

    polygon.model = glm::translate(glm::vec3(1, 1, (double)polygon.depth));
}

void system_occlusion_polygon(EntityManager &entity_manager, const CollisionManager &collision_manager, const Camera &camera)
{
    const component::Transform *player_transform;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::PLAYER)) continue;
        player_transform = entity_manager.get_transform_component(i, 0);
        break;
    }

    component::Polygon *polygon;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::OCCLUSION_POLYGON)) continue;
        polygon = entity_manager.get_polygon_component(i, 0);
        update_polygon(*player_transform, *polygon, collision_manager, camera);
    }
}
