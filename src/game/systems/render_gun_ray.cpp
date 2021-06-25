#include "game/systems.h"

void update_entity(component::Transform &transform, component::Gun &gun, component::VisualStatic &visual_static, const Camera &camera, double dt)
{
    if (gun.fire_event) {
        gun.fire_event = false;
        gun.fire_visual_on = true;
        visual_static.render_index = gun.mesh_index_fired;
        gun.fire_visual_timer = 0;
    }

    if (gun.fire_visual_on) {
        gun.fire_visual_timer += dt;
        if (gun.fire_visual_timer > gun.fire_visual_timeout) {
            gun.fire_visual_on = false;
            visual_static.render_index = gun.mesh_index_aiming;
        }
    }

    double gradient =  0.02 * (1 - gun.focus*0.8);
    double scale_x = 2000/camera.zoom;
    double scale_y = gradient * scale_x;
    visual_static.model = glm::translate(glm::vec3(transform.pos.x, transform.pos.y, (double)visual_static.depth))
        * glm::rotate((float)transform.orientation, glm::vec3(0, 0, 1))
        * glm::translate(glm::vec3(gun.origin_offset, 0, 0))
        * glm::scale(glm::vec3(scale_x, scale_y, 1));
}

void system_render_gun_ray(EntityManager &entity_manager, const Camera &camera, double dt)
{
    component::Transform *transform;
    component::Gun *gun;
    component::VisualStatic *visual;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::RENDER_GUN_RAY)) continue;
        transform = entity_manager.get_transform_component(i, 0);
        gun = entity_manager.get_gun_component(i, 0);
        visual = entity_manager.get_visual_static_component(i, 1);
        update_entity(*transform, *gun, *visual, camera, dt);
    }
}
