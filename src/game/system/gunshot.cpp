#include "game/system.h"

struct Gunshot {
    glm::vec2 origin;
    glm::vec2 fire_point;
    double focus;
    int entity_id;
};

void check_for_gunshot(const component::Transform &transform, const component::Gun &gun, int entity_id, std::vector<Gunshot> &gunshots)
{
    if (!gun.fire_event) return;
    Gunshot gunshot;
    gunshot.origin = transform.pos +
        glm::vec2(gun.origin_offset * cos(transform.orientation),
                  gun.origin_offset * sin(transform.orientation));
    gunshot.fire_point = gun.fire_point;
    gunshot.focus = gun.focus;
    gunshot.entity_id = entity_id;
    gunshots.push_back(gunshot);
}

bool check_for_gunshot_hit(component::Transform &transform, int entity_id, const std::vector<Gunshot> &gunshots)
{
    // TODO: In future, subtract health. For now, just mark for removal.
    for (const auto &gunshot: gunshots) {
        if (gunshot.entity_id == entity_id) continue;
        glm::vec2 disp = gunshot.fire_point - transform.pos;
        if (std::hypot(disp.x, disp.y) < 200) return true;
    }
    return false;
}

void system_gunshot(EntityManager &entity_manager)
{
    std::vector<Gunshot> gunshots;
    component::Transform *transform;
    component::Gun *gun;
    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::GUNSHOT_SOURCE)) continue;
        transform = entity_manager.get_transform_component(i, 0);
        gun = entity_manager.get_gun_component(i, 0);
        check_for_gunshot(*transform, *gun, i, gunshots);
    }

    for (int i = 0; i < entity_manager.entities.tail; i++) {
        if (!entity_manager.entity_supports_system(i, SystemType::GUNSHOT_TARGET)) continue;
        transform = entity_manager.get_transform_component(i, 0);
        entity_manager.entities[i].to_remove |= check_for_gunshot_hit(*transform, i, gunshots);
    }
}
