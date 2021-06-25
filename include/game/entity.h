#ifndef WORLD_ENTITY_H
#define WORLD_ENTITY_H

#include <array>
#include <bitset>
#include "game/components.h"

#include <iostream>

constexpr int MAX_COMPONENTS_PER_ENTITY = 16;
constexpr int MAX_ENTITIES = 1024;
constexpr int MAX_COMPONENTS = MAX_COMPONENTS_PER_ENTITY*MAX_ENTITIES;

constexpr int NUM_SYSTEMS = 32;
typedef std::bitset<NUM_SYSTEMS> Signature;
enum class SystemType {
    PHYSICS,
    RENDER_BASE,
    PLAYER,
    ENEMY,
    RENDER_GUN_RAY,
    GUNSHOT_SOURCE,
    GUNSHOT_TARGET
};

struct Entity {
    // Block of component references
    int start;
    int count;
    Signature signature;
    bool to_remove;
};

template <typename T, int N>
struct Buffer {
    std::array<T, N> data;
    int tail;

    T &operator[](int index){ return data[index]; }
    const T &operator[](int index)const{ return data[index]; }

    bool append(T element) {
        if (tail == N) return false;
        data[tail] = element;
        tail++;
        return true;
    }
    bool reserve(int num) {
        if (tail + num >= N) return false;
        tail+=num;
        return true;
    }
    void remove(int index) {
        tail--;
        if (index != tail) {
            data[index] = data[tail];
        }
    }

    Buffer(): tail(0) {}
};

class EntityManager {
public:
    Buffer<Entity, MAX_ENTITIES> entities;
    Buffer<component::Transform, MAX_COMPONENTS> transform;
    Buffer<component::Physics, MAX_COMPONENTS> physics;
    Buffer<component::VisualStatic, MAX_COMPONENTS> visual_static;
    Buffer<component::Gun, MAX_COMPONENTS> gun;

    int entity_create(int num_components, Signature signature);
    void entity_remove(int entity_id);
    void entity_add_transform(int entity_id, int offset, component::Transform component) {
        add_component(entity_id, offset, transform, ComponentType::TRANSFORM, component);
    }
    void entity_add_physics(int entity_id, int offset, component::Physics component) {
        add_component(entity_id, offset, physics, ComponentType::PHYSICS, component);
    }
    void entity_add_visual_static(int entity_id, int offset, component::VisualStatic component) {
        add_component(entity_id, offset, visual_static, ComponentType::VISUAL_STATIC, component);
    }
    void entity_add_gun(int entity_id, int offset, component::Gun component) {
        add_component(entity_id, offset, gun, ComponentType::GUN, component);
    }

    component::Transform* get_transform_component(int entity_id, int index=0) {
        return get_component(entity_id, index, ComponentType::TRANSFORM, transform);
    }
    component::Physics* get_physics_component(int entity_id, int index=0) {
        return get_component(entity_id, index, ComponentType::PHYSICS, physics);
    }
    component::VisualStatic* get_visual_static_component(int entity_id, int index=0) {
        return get_component(entity_id, index, ComponentType::VISUAL_STATIC, visual_static);
    }
    component::Gun* get_gun_component(int entity_id, int index=0) {
        return get_component(entity_id, index, ComponentType::GUN, gun);
    }

    bool entity_supports_system(int entity_id, SystemType type){ return entities[entity_id].signature.test((std::size_t)type); }

    // TODO: Temporarily public for debugging
    Buffer<ComponentReference, MAX_COMPONENTS> component_references;
private:
    template <typename T>
    T* get_component(int entity_id, int index, ComponentType type, Buffer<T, MAX_COMPONENTS> &buffer);
    template <typename T>
    void add_component(int entity_id, int offset, Buffer<T, MAX_COMPONENTS> &buffer, ComponentType component_type, T component);
    template <typename T>
    void remove_component(Buffer<T, MAX_COMPONENTS> &buffer, int index);

    Buffer<Entity, MAX_ENTITIES> free;

    class SystemManager;
    friend SystemManager;
    class System;
    friend System;
};

template <typename T>
inline T* EntityManager::get_component(int entity_id, int index, ComponentType type, Buffer<T, MAX_COMPONENTS> &buffer)
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
inline void EntityManager::add_component(int entity_id, int offset, Buffer<T, MAX_COMPONENTS> &buffer, ComponentType component_type, T component)
{
    buffer.append(component);
    component_references[entities[entity_id].start + offset].index = buffer.tail-1;
    component_references[entities[entity_id].start + offset].type = component_type;
}

template <typename T>
inline void EntityManager::remove_component(Buffer<T, MAX_COMPONENTS> &buffer, int index)
{
    buffer.remove(index);
    if (index != 0) {
        // When removing a component, it will move the end component to the empty
        // slot.
        // Need to update the index stored in the component reference.
        component_references[buffer[index].ref_id].index = index;
    }
}


#endif
