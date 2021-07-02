#include "game/system.h"

static void update_player_state(
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
    double goal_orientation = std::atan2(mouse_pos_world.y - transform.pos.y, mouse_pos_world.x - transform.pos.x);
    physics.twist.z = goal_orientation - transform.orientation;
    if (physics.twist.z > M_PI) physics.twist.z -= 2*M_PI;
    if (physics.twist.z < -M_PI) physics.twist.z += 2*M_PI;
    physics.twist.z *= 50; // Kp gain

    if (input.query_input_state(InputType::TOGGLE_GUN) == InputState::PRESSED) {
        gun.gun_out = !gun.gun_out;
    }

    if (gun.gun_out && input.query_input_state(InputType::CLICK_LEFT) == InputState::PRESSED)
    {
        gun.fire_event = true;
        gun.fire_point = mouse_pos_world;
    }
}

static void update_player_visual(
    const component::Gun &gun,
    component::Sprite &sprite,
    component::Animation &animation)
{
    if (!gun.gun_out && !animation.started) {
        animation.started = true;
        animation.id = AnimationId::HUMAN_WALK;
        animation.index = -1;
        animation.frame_time = 0.25;
        animation.loop = true;
        return;
    }
    if (gun.gun_out && sprite.sprite_id != SpriteId::HUMAN_GUN) {
        animation.started = false;
        std::cout << "Stopping animation" << std::endl;
        sprite.sprite_id = SpriteId::HUMAN_GUN;
    }
}

void system_player(EntityManager &entity_manager, const Input &input, Camera &camera)
{
    component::Transform *transform;
    component::Physics *physics;
    component::Gun *gun;
    component::Sprite *sprite;
    component::Animation *animation;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::PLAYER)) continue;
        Entity &entity = entity_manager.entities[i];
        transform = entity_manager.get_transform_component(i, 0);
        physics = entity_manager.get_physics_component(i, 0);
        gun = entity_manager.get_gun_component(i, 0);
        sprite = entity_manager.get_sprite_component(i, 0);
        animation = entity_manager.get_animation_component(i, 0);
        update_player_state(*transform, *physics, *gun, input, camera);
        update_player_visual(*gun, *sprite, *animation);
    }
};

