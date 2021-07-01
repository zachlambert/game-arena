#include "setup/entity_builder.h"

void build_player(EntityManager &entity_manager, PlayerConfig config)
{
    Signature signature;
    signature.set((size_t)SystemType::PLAYER);
    signature.set((size_t)SystemType::PHYSICS);
    signature.set((size_t)SystemType::RENDER_SPRITE);
    signature.set((size_t)SystemType::GUN);
    signature.set((size_t)SystemType::GUNSHOT_TARGET);
    signature.set((size_t)SystemType::COLLISION);
    signature.set((size_t)SystemType::COLLISION_PHYSICS);

    // Component 0 = Transform
    component::Transform transform;
    transform.pos = config.start_pos;
    transform.orientation = config.start_orientation;

    // Component 1 = Physics
    component::Physics physics;

    // Component 2 = Sprite
    component::Sprite sprite;
    sprite.depth = 4;
    sprite.sprite_id = config.sprite_id;

    // Component 3 = Gun
    component::Gun gun;

    // Component 4 = Gun ray polygon
    component::Polygon gun_ray_polygon;
    gun_ray_polygon.depth = 5;
    // TODO

    // Component 5 = Hitbox
    component::Hitbox hitbox;

    int id = entity_manager.entity_create(6, signature);
    entity_manager.entity_add_transform(id, 0, transform);
    entity_manager.entity_add_physics(id, 1, physics);
    entity_manager.entity_add_sprite(id, 2, sprite);
    entity_manager.entity_add_gun(id, 3, gun);
    entity_manager.entity_add_polygon(id, 4, gun_ray_polygon);
    entity_manager.entity_add_hitbox(id, 5, hitbox);
}

void build_enemy(EntityManager &entity_manager, EnemyConfig config)
{
    Signature signature;
    signature.set((size_t)SystemType::ENEMY);
    signature.set((size_t)SystemType::PHYSICS);
    signature.set((size_t)SystemType::RENDER_SPRITE);
    signature.set((size_t)SystemType::GUNSHOT_TARGET);
    signature.set((size_t)SystemType::COLLISION);
    signature.set((size_t)SystemType::COLLISION_PHYSICS);

    // 3 Components

    // Component 0 = Transform
    component::Transform transform;
    transform.pos = config.start_pos;
    transform.orientation = config.start_orientation;

    // Component 1 = Physics
    component::Physics physics;

    // Component 2 = Visual static
    component::Sprite sprite;
    sprite.depth = 5;
    sprite.sprite_id = config.sprite_id;

    // Component 3 = Hitbox
    component::Hitbox hitbox;

    int id = entity_manager.entity_create(4, signature);
    entity_manager.entity_add_transform(id, 0, transform);
    entity_manager.entity_add_physics(id, 1, physics);
    entity_manager.entity_add_sprite(id, 2, sprite);
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
