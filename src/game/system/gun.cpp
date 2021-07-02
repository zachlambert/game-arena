#include "game/system.h"

struct Gunshot {
    glm::vec2 origin;
    glm::vec2 fire_point;
    double focus;
    int entity_id;
};

static void initialise_gun_ray_polygon(
    component::Polygon &polygon)
{
    polygon.indices = {0, 1, 2, 0, 2, 3, 0, 0}; // last 2 unused
    polygon.element_count = 6;
    polygon.vertices.resize(4);
    polygon.colors.resize(4);
}

static void update_gun_ray(
    const component::Transform &source_transform,
    component::Gun &gun,
    component::Polygon &gun_ray_polygon,
    const Camera &camera)
{
    // Set transform to same as source sprite (but with no scaling, and different depth)
    gun_ray_polygon.model =
        glm::translate(glm::vec3(source_transform.pos.x, source_transform.pos.y, (double)gun_ray_polygon.depth))
        * glm::rotate((float)source_transform.orientation, glm::vec3(0, 0, 1));

    if (gun_ray_polygon.indices.size() == 0) {
        initialise_gun_ray_polygon(gun_ray_polygon);
    }

    // Update polygon when:
    // - Aiming, so it adjusts based on focus
    // - When a shot is fired, such that it stays the same or a bit, and
    //   darkens.
    if (!gun.fire_visual_on || gun.fire_event) {
        double gradient =  0.01 * (1 - gun.focus*0.8);
        double x_dist = 2000/camera.zoom;
        double y_dist = gradient * x_dist;
        static double origin_y = 6;

        gun_ray_polygon.vertices[0] = {gun.origin_offset, -origin_y};
        gun_ray_polygon.vertices[1] = {gun.origin_offset, origin_y};
        gun_ray_polygon.vertices[2] = {gun.origin_offset + x_dist, origin_y + y_dist};
        gun_ray_polygon.vertices[3] = {gun.origin_offset + x_dist, -origin_y - y_dist};

        if (gun.fire_event) {
            gun_ray_polygon.colors[0] = {0, 0, 0, 1};
            gun_ray_polygon.colors[1] = {0, 0, 0, 1};
            gun_ray_polygon.colors[2] = {1, 1, 1, 1};
            gun_ray_polygon.colors[3] = {1, 1, 1, 1};
        } else if (!gun.fire_visual_on) {
            gun_ray_polygon.colors[0] = {0.7, 0.7, 0.7, 1};
            gun_ray_polygon.colors[1] = {0.7, 0.7, 0.7, 1};
            gun_ray_polygon.colors[2] = {1, 1, 1, 1};
            gun_ray_polygon.colors[3] = {1, 1, 1, 1};
        }
        gun_ray_polygon.dirty = true;
    }
}

static void update_gun(
    const component::Transform &source_transform,
    component::Gun &gun,
    int entity_id, // For recording the source of a gunshot
    std::vector<Gunshot> &gunshots,
    double dt)
{
    if (gun.fire_visual_on) {
        gun.fire_visual_timer += dt;
        if (gun.fire_visual_timer > gun.fire_visual_timeout) {
            gun.fire_visual_on = false;
        }
    }

    if (!gun.fire_event) return;

    gun.fire_visual_on = true;
    gun.fire_visual_timer = 0;

    Gunshot gunshot;
    gunshot.origin = source_transform.pos +
        glm::vec2(gun.origin_offset * cos(source_transform.orientation),
                  gun.origin_offset * sin(source_transform.orientation));
    gunshot.fire_point = gun.fire_point;
    gunshot.focus = gun.focus;
    gunshot.entity_id = entity_id;
    gunshots.push_back(gunshot);
}

static bool check_for_gunshot_hit(component::Transform &transform, int entity_id, const std::vector<Gunshot> &gunshots)
{
    // TODO: In future, subtract health. For now, just mark for removal.
    for (const auto &gunshot: gunshots) {
        if (gunshot.entity_id == entity_id) continue;
        glm::vec2 disp = gunshot.fire_point - transform.pos;
        if (std::hypot(disp.x, disp.y) < 200) return true;
    }
    return false;
}

void system_gun(EntityManager &entity_manager, double dt, const Camera &camera)
{
    std::vector<Gunshot> gunshots;
    component::Transform *transform;
    component::Gun *gun;
    component::Polygon *gun_ray_polygon;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::GUN)) continue;
        transform = entity_manager.get_transform_component(i, 0);
        gun = entity_manager.get_gun_component(i, 0);
        gun_ray_polygon = entity_manager.get_polygon_component(i, 0);
        update_gun(*transform, *gun, i, gunshots, dt);
        update_gun_ray(*transform, *gun, *gun_ray_polygon, camera);
        gun->fire_event = false;
    }

    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::GUNSHOT_TARGET)) continue;
        transform = entity_manager.get_transform_component(i, 0);
        entity_manager.entities[i].to_remove |= check_for_gunshot_hit(*transform, i, gunshots);
    }
}
