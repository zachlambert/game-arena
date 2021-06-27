#include "setup/sprite_config.h"

void load_spritesheets(std::vector<SpritesheetConfig> spritesheets)
{
    SpritesheetConfig spritesheet;
    SpriteConfig sprite;

    // ----- SPRITESHEET 1 -----

    spritesheet.diffuse_texture_file_name = "spritesheet1.png";
    spritesheet.sprites.clear();

    sprite.name = "human_1_still";
    sprite.pos = {0, 0};
    sprite.size = {256, 256};
    sprite.offset = {0, 0};
    sprite.collision_polygon = {
        {-64, 0},
        {0, 128},
        {64, 0},
        {0, -128}
    };
    spritesheet.sprites.push_back(sprite);

    sprite.name = "human_1_walk_a";
    sprite.pos = {256, 0};
    sprite.size = {256, 256};
    sprite.offset = {0, 0};
    sprite.collision_polygon = {
        {-64, 0},
        {0, 128},
        {64, 0},
        {0, -128}
    };
    spritesheet.sprites.push_back(sprite);

    sprite.name = "human_1_walk_b";
    sprite.pos = {768, 0};
    sprite.size = {256, 256};
    sprite.offset = {0, 0};
    sprite.collision_polygon = {
        {-64, 0},
        {0, 128},
        {64, 0},
        {0, -128}
    };
    spritesheet.sprites.push_back(sprite);

    sprite.name = "human_1_gun";
    sprite.pos = {0, 256};
    sprite.size = {512, 256};
    sprite.offset = {-128, 0};
    sprite.collision_polygon = {
        {-64, 0},
        {0, 128},
        {192, 0},
        {0, -128}
    };
    spritesheet.sprites.push_back(sprite);

    sprite.name = "gun";
    sprite.pos = {512, 256};
    sprite.size = {256, 256};
    sprite.offset = {0, 0};
    sprite.collision_polygon = {
        {-32, -32},
        {-32, 32},
        {64, 32},
        {64, -32}
    };
    spritesheet.sprites.push_back(sprite);

    spritesheets.push_back(spritesheet);


    // ----- SPRITESHEET 2 -----

    spritesheet.diffuse_texture_file_name = "spritesheet2.png";
    spritesheet.sprites.clear();
    
    // etc

}
