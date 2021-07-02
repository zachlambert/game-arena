#include "game/system.h"

static void update_polygon(
    const component::Transform &player_transform,
    component::Polygon &polygon,
    const CollisionManager &collision_manager,
    const Camera &camera)
{
    if (polygon.allocated == false) {
        // For now, just assigning a fixed size polygon
        polygon.vertices.resize(128);
        polygon.indices.resize(256);
        polygon.colors.resize(128);
        for (int i = 0; i < 128; i++) {
            polygon.colors[i] = {0.5, 0.5, 0.5, 1};
        }
        polygon.model = glm::translate(glm::vec3(1, 1, (double)polygon.depth));
    }

    polygon.dirty = true;

    double fov = 150.0 * (M_PI/180);
    double angle = player_transform.orientation + fov/2;

    polygon.vertices[0] = camera.project_point({camera.screen_width/2, camera.screen_height/2});

    int i = 0;
    glm::vec2 screen_pos;
    while (i <= 5) {
        screen_pos.x = camera.screen_width/2 + cos(angle) * 2*camera.screen_width;
        screen_pos.y = camera.screen_height/2 - sin(angle) * 2*camera.screen_width;
        polygon.vertices[i+1] = camera.project_point(screen_pos);
        angle += (2*M_PI - fov)/5;
        i++;
    }
    for (int i = 0; i < 5; i++) {
        polygon.indices[3*i] = 0;
        polygon.indices[3*i+1] = i+1;
        polygon.indices[3*i+2] = i+2;
    }

    // Now add cast shadows for terrain edges
    BoundingBox view_box;
    glm::vec2 bot_left = camera.project_point({0, camera.screen_height});
    glm::vec2 top_right = camera.project_point({camera.screen_width, 0});
    view_box.left = bot_left.x;
    view_box.bot = bot_left.y;
    view_box.right = top_right.x;
    view_box.top = top_right.y;

    std::vector<const Edge*> edges;
    collision_manager.get_occlusion_edges(
        view_box,
        player_transform.orientation-fov/2,
        fov,
        edges
    );

    int vertex_index = 7;
    int index_index = 15;
    glm::vec2 dir_a, dir_b;
    glm::vec2 centre(0.5*(view_box.left+view_box.right), 0.5*(view_box.bot+view_box.top));
    for (const auto &edge: edges) {
        polygon.vertices[vertex_index] = edge->a;
        polygon.vertices[vertex_index+1] = edge->b;
        dir_a = edge->a - centre;
        dir_a /= hypot(dir_a.x, dir_a.y);
        dir_b = edge->b - centre;
        dir_b /= hypot(dir_a.x, dir_a.y);
        polygon.vertices[vertex_index+2] = centre + 2*(float)(view_box.right-view_box.left)*dir_b;
        polygon.vertices[vertex_index+3] = centre + 2*(float)(view_box.right-view_box.left)*dir_a;

        polygon.indices[index_index] = vertex_index;
        polygon.indices[index_index+1] = vertex_index+1;
        polygon.indices[index_index+2] = vertex_index+2;
        polygon.indices[index_index+3] = vertex_index;
        polygon.indices[index_index+4] = vertex_index+2;
        polygon.indices[index_index+5] = vertex_index+3;

        vertex_index += 4;
        index_index += 6;
    }
    polygon.element_count = index_index;
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
