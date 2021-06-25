#include "game/systems.h"

#include <iostream>

void SystemPhysics::update(EntityManager &entity_manager, double dt) {
    component::Transform *transform;
    component::Physics *physics;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::PHYSICS)) continue;
        transform = entity_manager.get_transform_component(i, 0);
        physics = entity_manager.get_physics_component(i, 0);
        update_entity(*transform, *physics, dt);
    }
};

void SystemPhysics::update_entity(component::Transform &transform, component::Physics &physics, double dt) {
    transform.pos.x += physics.twist.x * dt;
    transform.pos.y += physics.twist.y * dt;
    transform.orientation += physics.twist.z * dt;
    if (transform.orientation < -M_PI) transform.orientation += 2*M_PI;
    if (transform.orientation > M_PI) transform.orientation -= 2*M_PI;
}


void SystemRenderStatic::update(EntityManager &entity_manager) {
    component::Transform *transform;
    component::VisualStatic *visual_static;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::RENDER_STATIC)) continue;
        transform = entity_manager.get_transform_component(i, 0);
        visual_static = entity_manager.get_visual_static_component(i, 0);
        update_entity(*transform, *visual_static);
    }
};

void SystemRenderStatic::update_entity(component::Transform &transform, component::VisualStatic &visual_static) {
    visual_static.model = glm::translate(glm::vec3(transform.pos.x, transform.pos.y, (double)visual_static.depth))
        * glm::rotate((float)transform.orientation, glm::vec3(0, 0, 1));
}


void SystemPlayer::update(EntityManager &entity_manager, const Input &input, Camera &camera) {
    component::Transform *transform;
    component::Physics *physics;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::PLAYER)) continue;
        Entity &entity = entity_manager.entities[i];
        transform = entity_manager.get_transform_component(i, 0);
        physics = entity_manager.get_physics_component(i, 0);
        update_entity(*transform, *physics, input, camera);
    }
};

void SystemPlayer::update_entity(component::Transform &transform, component::Physics &physics, const Input &input, Camera &camera)
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
        static int count = 0;
        std::cout << "MOUSE PRESSED " << count++ << std::endl;
    }
}


void SystemEnemy::update(EntityManager &entity_manager) {
    component::Transform *transform;
    component::Physics *physics;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::ENEMY)) continue;
        physics = entity_manager.get_physics_component(i, 0);
        update_entity(*physics);
    }
};

void SystemEnemy::update_entity(component::Physics &physics)
{
    static std::default_random_engine generator;
    static std::normal_distribution<double> distribution(0, 1);
    physics.twist.x = physics.twist.x*0.99 + distribution(generator)*25;
    physics.twist.y = physics.twist.y*0.99 + distribution(generator)*25;
    physics.twist.z = physics.twist.z*0.99 + distribution(generator)*0.5;
}
