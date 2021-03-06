#include "game/entity.h"

void EntityManager::remove_entities()
{
    int i = 0;
    while (i < entities.tail) {
        if (entities[i].to_remove) {
            entity_remove(i);
        } else {
            i++;
        }
    }
}

int EntityManager::entity_create(int num_components, Signature signature)
{
    Entity entity;
    entity.start = -1;
    entity.count = num_components;
    entity.signature = signature;
    entity.to_remove = false;

    // STEP 1: Find space for new components, either in a free slot or at
    // the end.
    if (free.tail != 0) {
        // Choose the smallest available slot.
        int min_index = -1;
        int min_excess = entities.tail;
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
            free[min_index].count -= entity.count;
            free[min_index].start += entity.count;
            if (free[min_index].count == 0) {
                free.remove(min_index);
            }
        }
    }

    if (entity.start == -1) {
        entity.start = component_references.tail;
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
        int comp_index = component_references[i].index;
        switch (component_references[i].type) {
            case ComponentType::TRANSFORM:
                remove_component(transform, comp_index);
                break;
            case ComponentType::SPRITE:
                remove_component(sprite, comp_index);
                break;
            case ComponentType::POLYGON:
                remove_component(polygon, comp_index);
                break;
            case ComponentType::PHYSICS:
                remove_component(physics, comp_index);
                break;
            case ComponentType::GUN:
                remove_component(gun, comp_index);
                break;
            case ComponentType::ENEMY_SPAWNER:
                remove_component(enemy_spawner, comp_index);
                break;
            case ComponentType::HITBOX:
                remove_component(hitbox, comp_index);
                break;
            case ComponentType::ANIMATION:
                remove_component(animation, comp_index);
                break;
        }
    }

    // ----- 3. Remove the entity struct itself -----
    entities.remove(entity_id);
}

