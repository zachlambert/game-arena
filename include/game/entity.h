#ifndef WORLD_ENTITY_H
#define WORLD_ENTITY_H

#include <array>
#include <bitset>
#include "game/components.h"
#include "data_structures/buffer.h"

#include <iostream>

constexpr int NUM_SYSTEMS = 32;
typedef std::bitset<NUM_SYSTEMS> Signature;
enum class SystemType {
    RENDER_SPRITE,
    RENDER_POLYGON,
    PHYSICS,
    PLAYER,
    ENEMY,
    GUN,
    GUNSHOT_TARGET,
    ENEMY_SPAWNER,
    COLLISION,
    COLLISION_PHYSICS,
    OCCLUSION_POLYGON,
    ANIMATION
};

struct Entity {
    // Block of component references
    int start;
    int count;
    Signature signature;
    bool to_remove;
};

#define CREATE_COMPONENT(Component, name, TYPE, add_func, get_func) \
    Buffer<component::Component> name; \
    void add_func(int entity_id, int offset, component::Component component) {\
        add_component(entity_id, offset, name, ComponentType::TYPE, component);\
    }\
    component::Component* get_func(int entity_id, int index=0) {\
        return get_component(entity_id, index, ComponentType::TYPE, name);\
    }\

class EntityManager {
public:
    Buffer<Entity> entities;
    void remove_entities();
    int entity_create(int num_components, Signature signature);
    void entity_remove(int entity_id);
    bool entity_supports_system(int entity_id, SystemType type){ return entities[entity_id].signature.test((std::size_t)type); }

    CREATE_COMPONENT(Transform, transform, TRANSFORM, entity_add_transform, get_transform_component);
    CREATE_COMPONENT(Sprite, sprite, SPRITE, entity_add_sprite, get_sprite_component);
    CREATE_COMPONENT(Polygon, polygon, POLYGON, entity_add_polygon, get_polygon_component);
    CREATE_COMPONENT(Physics, physics, PHYSICS, entity_add_physics, get_physics_component);
    CREATE_COMPONENT(Gun, gun, GUN, entity_add_gun, get_gun_component);
    CREATE_COMPONENT(EnemySpawner, enemy_spawner, ENEMY_SPAWNER, entity_add_enemy_spawner, get_enemy_spawner_component);
    CREATE_COMPONENT(Hitbox, hitbox, HITBOX, entity_add_hitbox, get_hitbox_component);
    CREATE_COMPONENT(Animation, animation, ANIMATION, entity_add_animation, get_animation_component);

private:
    template <typename T>
    T* get_component(int entity_id, int index, ComponentType type, Buffer<T> &buffer);
    template <typename T>
    void add_component(int entity_id, int offset, Buffer<T> &buffer, ComponentType component_type, T component);
    template <typename T>
    void remove_component(Buffer<T> &buffer, int index);

    Buffer<ComponentReference> component_references;
    Buffer<Entity> free;
};

template <typename T>
inline T* EntityManager::get_component(int entity_id, int index, ComponentType type, Buffer<T> &buffer)
{
    // If index = 0, return the first occurance of the component
    // If index = 1, return the second occurance of the component
    // etc
    // Countdown the number of components encountered
    int countdown = index+1;
    Entity &entity = entities[entity_id];
    for (int i = entity.start; i < entity.start+entity.count; i++) {
        if (component_references[i].type == type) countdown--;
        if (countdown == 0) return &buffer[component_references[i].index];
    }
    return nullptr;
}

template <typename T>
inline void EntityManager::add_component(int entity_id, int offset, Buffer<T> &buffer, ComponentType component_type, T component)
{
    if (entity_id == -1) return;
    int ref_index = entities[entity_id].start + offset;
    component.ref_id = ref_index;
    buffer.append(component);
    component_references[ref_index].index = buffer.tail-1;
    component_references[ref_index].type = component_type;
}

template <typename T>
inline void EntityManager::remove_component(Buffer<T> &buffer, int index)
{
    buffer.remove(index);
    if (buffer.tail != 0) {
        // When removing a component, it will move the end component to the empty
        // slot.
        // Need to update the index stored in the component reference.
        component_references[buffer[index].ref_id].index = index;
    }
}


#endif
