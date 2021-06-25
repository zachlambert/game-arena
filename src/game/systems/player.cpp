#include "game/systems.h"

void update_entity(
    component::Transform &transform,
    component::Physics &physics,
    component::Gun &gun,
    const Input &input,
    Camera &camera)
{
    physics.twist.x = 0;
    if (input.input_down(InputType::MOVE_RIGHT)) {
        physics.twist.x += 800;
    }
    if (input.input_down(InputType::MOVE_LEFT)) {
        physics.twist.x -= 800;
    }

    physics.twist.y = 0;
    if (input.input_down(InputType::MOVE_UP)) {
        physics.twist.y += 800;
    }
    if (input.input_down(InputType::MOVE_DOWN)) {
        physics.twist.y -= 800;
    }

    glm::vec2 mouse_pos_screen = input.get_mouse_pos();
    glm::vec2 mouse_pos_world = camera.project_point(mouse_pos_screen);
    transform.orientation = std::atan2(mouse_pos_world.y - transform.pos.y, mouse_pos_world.x - transform.pos.x);

    if (input.query_input_state(InputType::CLICK_LEFT) == InputState::PRESSED)
    {
        gun.fire_event = true;
        gun.fire_point = mouse_pos_world;
    }
}

void system_player(EntityManager &entity_manager, const Input &input, Camera &camera)
{
    component::Transform *transform;
    component::Physics *physics;
    component::Gun *gun;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::PLAYER)) continue;
        Entity &entity = entity_manager.entities[i];
        transform = entity_manager.get_transform_component(i, 0);
        physics = entity_manager.get_physics_component(i, 0);
        gun = entity_manager.get_gun_component(i, 0);
        update_entity(*transform, *physics, *gun, input, camera);
    }
};

