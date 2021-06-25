#ifndef WORLD_ENTITY_H
#define WORLD_ENTITY_H

#include <array>
#include <bitset>
#include "world/components.h"
#include "world/systems.h"

constexpr int MAX_COMPONENTS_PER_ENTITY = 16;
constexpr int MAX_ENTITIES = 1024;
constexpr int MAX_COMPONENTS = MAX_COMPONENTS_PER_ENTITY*MAX_ENTITIES;
constexpr int NUM_SYSTEMS = 32;

typedef std::bitset<NUM_SYSTEMS> Signature;

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
    int entity_create(int num_components);
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

    void update(const Input &input, Camera &camera, double dt);

private:
    template <typename T>
    T &get_component(int entity_id, int offset, Buffer<T, MAX_COMPONENTS> &buffer);
    template <typename T>
    const T &get_component(int entity_id, int offset, const Buffer<T, MAX_COMPONENTS> &buffer)const;
    template <typename T>
    void add_component(int entity_id, int offset, Buffer<T, MAX_COMPONENTS> &buffer, ComponentType component_type, T component);
    template <typename T>
    void remove_component(Buffer<T, MAX_COMPONENTS> &buffer, int index);

    Buffer<Entity, MAX_ENTITIES> entities;
    Buffer<Entity, MAX_ENTITIES> free;
    Buffer<ComponentReference, MAX_COMPONENTS> component_references;

    Buffer<component::Transform, MAX_COMPONENTS> transform;
    Buffer<component::Physics, MAX_COMPONENTS> physics;
    Buffer<component::VisualStatic, MAX_COMPONENTS> visual_static;
};

int EntityManager::entity_create(int num_components)
{
    Entity entity;
    entity.start = -1;
    entity.count = num_components;

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

void EntityManager::entity_remove(int entity_id)
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
T &EntityManager::get_component(int entity_id, int offset, Buffer<T, MAX_COMPONENTS> &buffer)
{
    return buffer[component_references[entities[entity_id].start + offset].index];
}

template <typename T>
const T &EntityManager::get_component(int entity_id, int offset, const Buffer<T, MAX_COMPONENTS> &buffer)const
{
    return buffer[component_references[entities[entity_id].start + offset].index];
}

template <typename T>
void EntityManager::add_component(int entity_id, int offset, Buffer<T, MAX_COMPONENTS> &buffer, ComponentType component_type, T component)
{
    buffer.append(component);
    component_references[entities[entity_id].start + offset].index = buffer.tail-1;
    component_references[entities[entity_id].start + offset].type = component_type;
}

template <typename T>
void EntityManager::remove_component(Buffer<T, MAX_COMPONENTS> &buffer, int index)
{
    buffer.remove(index);
    if (index != 0) {
        // When removing a component, it will move the end component to the empty
        // slot.
        // Need to update the index stored in the component reference.
        component_references[buffer[index].ref_id].index = index;
    }
}

void EntityManager::update(const Input &input, Camera &camera, double dt)
{
    for (int i = 0; i < entities.tail; i++) {
        const Entity &entity = entities[i];

        if (entity.signature.test((std::size_t)SystemType::ENEMY)) {
            // System = ENEMY
            // Physics: 1
            system_enemy(get_component(i, 1, physics));
        }

        if (entity.signature.test((std::size_t)SystemType::PLAYER)) {
            // System = PLAYER
            // Transform: 0
            // Physics: 1
            system_player(
                get_component(i, 0, transform),
                get_component(i, 1, physics),
                input, camera
            );
        }

        if (entity.signature.test((std::size_t)SystemType::PHYSICS)) {
            // System = PHYSICS
            // Transform: 0
            // Physics: 1
            system_physics(
                get_component(i, 0, transform),
                get_component(i, 1, physics),
                dt
            );
        }

        if (entity.signature.test((std::size_t)SystemType::PHYSICS)) {
            // System = VISUAL_STATIC
            // Transform: 0
            // VisualStatic: 2
            system_physics(
                get_component(i, 0, transform),
                get_component(i, 2, physics),
                dt
            );
        }

        // TODO: Add an entity type (another enum)
        // Have the offsets depend on entity type.
        // The entity is responsible for defining the component layout.
    }
}


#endif
