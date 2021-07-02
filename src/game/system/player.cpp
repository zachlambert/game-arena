#include "game/system.h"

static void update_player_state(
    component::Transform &transform,
    component::Physics &physics,
    component::Gun &gun,
    const Input &input,
    Camera &camera,
    double dt)
{
    physics.force.x = 0;
    bool right = input.input_down(InputType::MOVE_RIGHT);
    bool left = input.input_down(InputType::MOVE_LEFT);
    if ((right && left) || (!right && !left)) {
        physics.force.x += 40*(0-physics.twist.x);
    } else if (right) {
        physics.force.x += 40*(800-physics.twist.x);
    } else if (left) {
        physics.force.x += 40*(-800-physics.twist.x);
    }

    physics.force.y = 0;
    bool up = input.input_down(InputType::MOVE_UP);
    bool down = input.input_down(InputType::MOVE_DOWN);
    if ((up && down) || (!up && !down)) {
        physics.force.y += 40*(0-physics.twist.y);
    } else if (up) {
        physics.force.y += 40*(800-physics.twist.y);
    } else if (down) {
        physics.force.y += 40*(-800-physics.twist.y);
    }

    glm::vec2 mouse_pos_screen = input.get_mouse_pos();
    glm::vec2 mouse_pos_world = camera.project_point(mouse_pos_screen);
    double goal_orientation = std::atan2(mouse_pos_world.y - transform.pos.y, mouse_pos_world.x - transform.pos.x);
    double error = goal_orientation - transform.orientation;
    if (error > M_PI) error -= 2*M_PI;
    if (error < -M_PI) error += 2*M_PI;

    static double prev_error = 0;
    static double error_sum = 0;
    double d_error = (error - prev_error) / dt;
    error_sum += error * dt;

    prev_error = error;
    physics.force.z = 12000000*error + 500000*d_error;// + 50000*error_sum;

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

void system_player(EntityManager &entity_manager, const Input &input, Camera &camera, double dt)
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
        update_player_state(*transform, *physics, *gun, input, camera, dt);
        update_player_visual(*gun, *sprite, *animation);
    }
};

