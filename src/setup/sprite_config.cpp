#include "setup/sprite_config.h"

void load_spritesheets(std::vector<SpritesheetConfig> &spritesheets)
{
    SpritesheetConfig spritesheet;
    SpriteConfig sprite;

    // ----- SPRITESHEET 1 -----

    spritesheet.diffuse_texture_file_name = "spritesheet1.png";
    spritesheet.sprites.clear();

    sprite.id = SpriteId::HUMAN_STILL;
    sprite.pos = {0, 0};
    sprite.size = {256, 256};
    sprite.offset = {0, 0};
    sprite.collision_polygon = {
        {-50, 0},
        {-30, 70},
        {0, 100},
        {30, 70},
        {50, 0},
        {30, -70},
        {0, -100},
        {-30, -70}
    };
    spritesheet.sprites.push_back(sprite);

    sprite.id = SpriteId::HUMAN_WALK_0;
    sprite.pos = {256, 0};
    sprite.size = {256, 256};
    sprite.offset = {0, 0};
    sprite.collision_polygon = {
        {-50, 0},
        {-30, 70},
        {0, 100},
        {50, 104},
        {104, 57},
        {50, 40},
        {40, -70},
        {48, -110},
        {-40, -104}
    };
    spritesheet.sprites.push_back(sprite);

    sprite.id = SpriteId::HUMAN_WALK_1;
    sprite.pos = {512, 0};
    sprite.size = {256, 256};
    sprite.offset = {0, 0};
    sprite.collision_polygon = {
        {-50, 0},
        {-40, 104},
        {48, 110},
        {40, 70},
        {50, -40},
        {104, -57},
        {50, -104},
        {0, -100},
        {-30, -70}
    };
    spritesheet.sprites.push_back(sprite);

    sprite.id = SpriteId::HUMAN_GUN;
    sprite.pos = {0, 256};
    sprite.size = {512, 256};
    sprite.offset = {-128, 0};
    sprite.collision_polygon = {
        {-50, 0},
        {-20, 90},
        {0, 100},
        {10, 95},
        {150, 0},
        {10, -95},
        {0, -100},
        {-30, -70}
    };
    spritesheet.sprites.push_back(sprite);

    sprite.id = SpriteId::GUN;
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
