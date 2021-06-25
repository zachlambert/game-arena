#ifndef WORLD_ENTITY_H
#define WORLD_ENTITY_H

#include <array>
#include <bitset>
#include "game/components.h"

constexpr int MAX_COMPONENTS_PER_ENTITY = 16;
constexpr int MAX_ENTITIES = 1024;
constexpr int MAX_COMPONENTS = MAX_COMPONENTS_PER_ENTITY*MAX_ENTITIES;

constexpr int NUM_SYSTEMS = 32;
typedef std::bitset<NUM_SYSTEMS> Signature;
enum class SystemType {
    PHYSICS,
    RENDER_STATIC,
    PLAYER,
    ENEMY
};

struct Entity {
    // Block of component references
    int start;
    int count;
    Signature signature;
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

    component::Transform* get_transform_component(int entity_id, int index=0);
    component::Physics* get_physics_component(int entity_id, int index=0);
    component::VisualStatic* get_visual_static_component(int entity_id, int index=0);

    bool entity_supports_system(int entity_id, SystemType type){ return entities[entity_id].signature.test((std::size_t)type); }

private:
    template <typename T>
    T* get_component(int entity_id, int index, ComponentType type, Buffer<T, MAX_COMPONENTS> &buffer);
    template <typename T>
    void add_component(int entity_id, int offset, Buffer<T, MAX_COMPONENTS> &buffer, ComponentType component_type, T component);
    template <typename T>
    void remove_component(Buffer<T, MAX_COMPONENTS> &buffer, int index);

    Buffer<Entity, MAX_ENTITIES> free;
    Buffer<ComponentReference, MAX_COMPONENTS> component_references;

    class SystemManager;
    friend SystemManager;
    class System;
    friend System;
};

inline component::Transform* EntityManager::get_transform_component(int entity_id, int index)
{
    return get_component(entity_id, index, ComponentType::TRANSFORM, transform);
}

inline component::Physics* EntityManager::get_physics_component(int entity_id, int index)
{
    return get_component(entity_id, index, ComponentType::PHYSICS, physics);
}

inline component::VisualStatic* EntityManager::get_visual_static_component(int entity_id, int index)
{
    return get_component(entity_id, index, ComponentType::VISUAL_STATIC, visual_static);
}

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

inline int EntityManager::entity_create(int num_components, Signature signature)
{
    Entity entity;
    entity.start = -1;
    entity.count = num_components;
    entity.signature = signature;

    // STEP 1: Find space for new components, either in a free slot or at
    // the end.
    if (free.tail != 0) {
        // Choose the smallest available slot.
        int min_index = -1;
        int min_excess = MAX_COMPONENTS;
        int excess;
        for (int i = free.tail-1; i >= 0; i--) {
            if (free[i].count >= num_components) {
                excess = free[i].count - num_components;
                if (excess < min_excess) {
                    min_excess = excess;
                    min_index = i;
                }
            }
        }
        if (min_index != -1) {
            entity.start = free[min_index].start;
            free[min_index].count -= min_excess;
            free[min_index].start += entity.count;
            if (free[min_index].count == 0) {
                free.remove(min_index);
            }
        }
    }

    if (entity.start == -1) {
        entity.start = entities.tail;
        component_references.reserve(entity.count);
    }

    entities.append(entity);
    return entities.tail-1;
}

inline void EntityManager::entity_remove(int entity_id)
{
    const Entity &entity = entities[entity_id];

    // ----- 1. Update free slots ------

    // Combine on the left if possible, and remove merged free slot
    Entity combination = entity;
    for (int i = 0; i < free.tail; i++) {
        if (free[i].start+free[i].count == combination.start) {
            combination.start = free[i].start;
            combination.count += free[i].count;
            free.remove(i);
            break;
        }
    }
    // Combine on the right if possible, and remove merged free slot
    for (int i = 0; i < free.tail; i++) {
        if (combination.start+combination.count == free[i].start) {
            combination.count += free[i].count;
            free.remove(i);
            break;
        }
    }
    // Now add the new free slot to the end of the buffer
    free.append(combination);

    // ----- 2. Remove component references used by entity ------
    // Can still access the component references, they aren't removed just marked as
    // available for use by new entities
    for (int i = entity.start; i < entity.start + entity.count; i++) {
        switch (component_references[i].type) {
            case ComponentType::TRANSFORM:
                transform.remove(component_references[i].index);
                break;
            case ComponentType::PHYSICS:
                physics.remove(component_references[i].index);
                break;
            case ComponentType::VISUAL_STATIC:
                visual_static.remove(component_references[i].index);
                break;
            case ComponentType::HUMAN:
                // TODO
                break;
            case ComponentType::ENEMY:
                // TODO
                break;
        }
    }

    // ----- 3. Remove the entity struct itself -----
    entities.remove(entity_id);
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
