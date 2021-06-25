#ifndef WORLD_SYSTEMS_H
#define WORLD_SYSTEMS_H

#include <array>
#include <iostream>
#include <random>

#include "window/input.h"
#include "world/components.h"
#include "world/camera.h"
#include "world/entity.h"

// Represent systems by classes instead of just functions, in case a system
// needs to store some state.

class SystemPhysics {

    void update(EntityManager &entity_manager, double dt) {
        component::Transform *transform;
        component::Physics *physics;
        for (int i = 0; i < entity_manager.entities.tail; i++) {
            transform = nullptr;
            physics = nullptr;
            Entity &entity = entity_manager.entities[i];
            for (int j = entity.start; j < entity.start+entity.count; j++) {
                if (transform==nullptr && entity_manager.component_references[j].type == ComponentType::TRANSFORM) {
                    transform = &entity_manager.transform[entity_manager.component_references[j].index];
                    continue;
                }
                if (physics==nullptr && entity_manager.component_references[j].type == ComponentType::PHYSICS) {
                    physics = &entity_manager.physics[entity_manager.component_references[j].index];
                    continue;
                }
            }
            if (transform!=nullptr && physics!=nullptr) {
                update_entity(*transform, *physics, dt);
            }
        }
    };

    void update_entity(component::Transform &transform, component::Physics &physics, double dt) {
        transform.pos.x += physics.twist.x * dt;
        transform.pos.y += physics.twist.y * dt;
        transform.orientation += physics.twist.z * dt;
        if (transform.orientation < -M_PI) transform.orientation += 2*M_PI;
        if (transform.orientation > M_PI) transform.orientation -= 2*M_PI;
    }
};

void system_render_static(component::Transform transform, component::VisualStatic visual_static)
{
    visual_static.model = glm::translate(glm::vec3(transform.pos.x, transform.pos.y, (double)visual_static.depth))
        * glm::rotate((float)transform.orientation, glm::vec3(0, 0, 1));
}

void system_physics(component::Transform transform, component::Physics physics, double dt)
{
}

void system_player(component::Transform &transform, component::Physics &physics, const Input &input, Camera &camera)
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

void system_enemy(component::Physics &physics)
{
    static std::default_random_engine generator;
    static std::normal_distribution<double> distribution(0, 1);
    physics.twist.x = physics.twist.x*0.99 + distribution(generator)*25;
    physics.twist.y = physics.twist.y*0.99 + distribution(generator)*25;
    physics.twist.z = physics.twist.z*0.99 + distribution(generator)*0.5;
}

#endif
