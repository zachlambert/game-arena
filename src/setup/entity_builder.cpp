#include "setup/entity_builder.h"

void build_player(EntityManager &entity_manager, PlayerConfig config)
{
    Signature signature;
    signature.set((size_t)SystemType::PLAYER);
    signature.set((size_t)SystemType::PHYSICS);
    signature.set((size_t)SystemType::RENDER_BASE);
    signature.set((size_t)SystemType::RENDER_GUN_RAY);
    signature.set((size_t)SystemType::GUNSHOT_SOURCE);
    signature.set((size_t)SystemType::GUNSHOT_TARGET);
    signature.set((size_t)SystemType::COLLISION);

    // Component 0 = Transform
    component::Transform transform;
    transform.pos = config.start_pos;
    transform.orientation = config.start_orientation;

    // Component 1 = Physics
    component::Physics physics;

    // Component 2 = Visual static
    component::VisualStatic visual_static;
    visual_static.depth = 4;
    visual_static.render_index = (int)config.sprite_id;
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

    // Component 5 = Hitbox
    component::Hitbox hitbox;
    hitbox.initialised = false;

    int id = entity_manager.entity_create(6, signature);
    entity_manager.entity_add_transform(id, 0, transform);
    entity_manager.entity_add_physics(id, 1, physics);
    entity_manager.entity_add_visual_static(id, 2, visual_static);
    entity_manager.entity_add_gun(id, 3, gun);
    entity_manager.entity_add_visual_static(id, 4, gun_visual_static);
    entity_manager.entity_add_hitbox(id, 5, hitbox);
}

void build_enemy(EntityManager &entity_manager, const CollisionManager &collision_manager, EnemyConfig config)
{
    Signature signature;
    signature.set((size_t)SystemType::ENEMY);
    signature.set((size_t)SystemType::PHYSICS);
    signature.set((size_t)SystemType::RENDER_BASE);
    signature.set((size_t)SystemType::GUNSHOT_TARGET);
    signature.set((size_t)SystemType::COLLISION);

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
    visual_static.render_index = (int)config.sprite_id;
    visual_static.type = component::VisualStatic::Type::SPRITE;

    // Component 3 = Hitbox
    component::Hitbox hitbox;
    hitbox.initialised = false;

    int id = entity_manager.entity_create(4, signature);
    entity_manager.entity_add_transform(id, 0, transform);
    entity_manager.entity_add_physics(id, 1, physics);
    entity_manager.entity_add_visual_static(id, 2, visual_static);
    entity_manager.entity_add_hitbox(id, 3, hitbox);
}

void build_enemy_spawner(EntityManager &entity_manager, EnemySpawnerConfig config)
{
    Signature signature;
    signature.set((size_t)SystemType::ENEMY_SPAWNER);

    // 1 Component

    // Component 0 = Enemy spawner
    component::EnemySpawner enemy_spawner;
    enemy_spawner.pos = config.pos;
    enemy_spawner.spawn_timeout = config.spawn_timeout;

    int id = entity_manager.entity_create(1, signature);
    entity_manager.entity_add_enemy_spawner(id, 0, enemy_spawner);
}
