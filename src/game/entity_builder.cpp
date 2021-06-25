#include "game/entity_builder.h"

void build_player(EntityManager &entity_manager, PlayerConfig config)
{
    Signature signature;
    signature.set((size_t)SystemType::PLAYER);
    signature.set((size_t)SystemType::PHYSICS);
    signature.set((size_t)SystemType::RENDER_BASE);
    signature.set((size_t)SystemType::RENDER_GUN_RAY);

    // 5 Components

    // Component 0 = Transform
    component::Transform transform;
    transform.pos = config.start_pos;
    transform.orientation = config.start_orientation;

    // Component 1 = Physics
    component::Physics physics;

    // Component 2 = Visual static
    component::VisualStatic visual_static;
    visual_static.depth = 4;
    visual_static.render_index = config.sprite_index;
    visual_static.type = component::VisualStatic::Type::SPRITE;

    // Component 3 = Gun
    component::Gun gun;
    gun.mesh_index_aiming = config.gun_ray_mesh_index_aiming;
    gun.mesh_index_fired = config.gun_ray_mesh_index_fired;

    // Component 4 = Gun visual static
    component::VisualStatic gun_visual_static;
    gun_visual_static.depth = 5;
    gun_visual_static.render_index = gun.mesh_index_aiming;
    gun_visual_static.type = component::VisualStatic::Type::MESH;

    int id = entity_manager.entity_create(5, signature);
    entity_manager.entity_add_transform(id, 0, transform);
    entity_manager.entity_add_physics(id, 1, physics);
    entity_manager.entity_add_visual_static(id, 2, visual_static);
    entity_manager.entity_add_gun(id, 3, gun);
    entity_manager.entity_add_visual_static(id, 4, gun_visual_static);
}

void build_enemy(EntityManager &entity_manager, EnemyConfig config)
{
    Signature signature;
    signature.set((size_t)SystemType::ENEMY);
    signature.set((size_t)SystemType::PHYSICS);
    signature.set((size_t)SystemType::RENDER_BASE);

    // 3 Components

    // Component 0 = Transform
    component::Transform transform;
    transform.pos = config.start_pos;
    transform.orientation = config.start_orientation;

    // Component 1 = Physics
    component::Physics physics;

    // Component 2 = Visual static
    component::VisualStatic visual_static;
    visual_static.depth = 5;
    visual_static.render_index = config.sprite_index;
    visual_static.type = component::VisualStatic::Type::SPRITE;

    int id = entity_manager.entity_create(3, signature);
    entity_manager.entity_add_transform(id, 0, transform);
    entity_manager.entity_add_physics(id, 1, physics);
    entity_manager.entity_add_visual_static(id, 2, visual_static);
}

void build_terrain(EntityManager &entity_manager, TerrainConfig config)
{
    Signature signature;
    signature.set((size_t)SystemType::RENDER_BASE);

    // 2 Components

    // Component 0 = Transform
    component::Transform transform;
    transform.pos = config.pos;
    transform.orientation = config.orientation;

    // Component 1 = Visual static
    component::VisualStatic visual_static;
    visual_static.depth = 8;
    visual_static.render_index = config.mesh_index;
    visual_static.type = component::VisualStatic::Type::MESH;

    int id = entity_manager.entity_create(2, signature);
    entity_manager.entity_add_transform(id, 0, transform);
    entity_manager.entity_add_visual_static(id, 1, visual_static);
}
