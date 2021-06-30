#include "render/collision_renderer.h"
#include <iostream>
#include <stack>

void CollisionRenderer::load_spritesheets(const std::string &base_dir, const std::vector<SpritesheetConfig> spritesheets)
{
    for (const auto &spritesheet: spritesheets) {
        for (const auto &sprite_config: spritesheet.sprites) {
            SpritePolygon sprite_polygon;
            sprite_polygon.vertices_offset = static_vertices.size();
            sprite_polygon.vertex_count = sprite_config.collision_polygon.size();

            CollisionVertex vertex;
            vertex.color = {1, 0, 0, 1};
            vertex.position.z = 0;
            for (std::size_t i = 0; i < sprite_config.collision_polygon.size(); i++) {
                vertex.position.x = sprite_config.collision_polygon[i].x;
                vertex.position.y = sprite_config.collision_polygon[i].y;
                static_vertices.push_back(vertex);
            }

            double max_dist = hypot(sprite_config.collision_polygon[0].x,
                                sprite_config.collision_polygon[0].y);
            double dist;
            for (std::size_t i = 1; i < sprite_config.collision_polygon.size(); i++) {
                dist = hypot(sprite_config.collision_polygon[i].x,
                             sprite_config.collision_polygon[i].y);
                if (dist > max_dist) max_dist = dist;
            }

            sprite_polygon.box_vertices_offset = static_vertices.size();
            vertex.color = {1, 0, 1, 1};
            vertex.position.x = -max_dist;
            vertex.position.y = -max_dist;
            static_vertices.push_back(vertex);
            vertex.position.y = max_dist;
            static_vertices.push_back(vertex);
            vertex.position.x = max_dist;
            static_vertices.push_back(vertex);
            vertex.position.y = -max_dist;
            static_vertices.push_back(vertex);

            if ((int)sprite_config.id >= sprite_polygons.size()) {
                sprite_polygons.resize((int)sprite_config.id+1);
            }
            sprite_polygons[(int)sprite_config.id] = sprite_polygon;
        }
    }
}

void CollisionRenderer::load_terrain_collision_data(const CollisionManager &collision_manager)
{
    terrain_vertex_count = 0;
    terrain_vertices_offset = static_vertices.size();
    std::stack<Octree*> nodes;
    nodes.push(collision_manager.root);

    CollisionVertex vertex;
    Octree *current;
    while (!nodes.empty()) {
        current = nodes.top();
        nodes.pop();
        if (current == nullptr) continue;

        vertex.position.z = 2;
        vertex.color = {0, 0, 1, 1};
        vertex.position.x = current->box.left;
        vertex.position.y = current->box.bot;
        static_vertices.push_back(vertex);
        vertex.position.y = current->box.top;
        static_vertices.push_back(vertex);
        static_vertices.push_back(vertex);
        vertex.position.x = current->box.right;
        static_vertices.push_back(vertex);
        static_vertices.push_back(vertex);
        vertex.position.y = current->box.bot;
        static_vertices.push_back(vertex);
        static_vertices.push_back(vertex);
        vertex.position.x = current->box.left;
        static_vertices.push_back(vertex);

        terrain_vertex_count += 8;

        vertex.color = {0, 0, 0, 1};
        vertex.position.x = current->box.left;
        vertex.position.y = current->centre.y;
        static_vertices.push_back(vertex);
        vertex.position.x = current->box.right;
        static_vertices.push_back(vertex);

        vertex.position.x = current->centre.x;
        vertex.position.y = current->box.top;
        static_vertices.push_back(vertex);
        vertex.position.y = current->box.bot;
        static_vertices.push_back(vertex);

        terrain_vertex_count += 4;

        vertex.color = {1, 0, 0, 1};
        vertex.position.z = 1;
        for (const auto &edge: current->edges) {
            vertex.position.x = edge.box.left;
            vertex.position.y = edge.box.bot;
            static_vertices.push_back(vertex);
            vertex.position.y = edge.box.top;
            static_vertices.push_back(vertex);
            static_vertices.push_back(vertex);
            vertex.position.x = edge.box.right;
            static_vertices.push_back(vertex);
            static_vertices.push_back(vertex);
            vertex.position.y = edge.box.bot;
            static_vertices.push_back(vertex);
            static_vertices.push_back(vertex);
            vertex.position.x = edge.box.left;
            static_vertices.push_back(vertex);
            terrain_vertex_count += 8;
        }

        nodes.push(current->nodes[0]);
        nodes.push(current->nodes[1]);
        nodes.push(current->nodes[2]);
        nodes.push(current->nodes[3]);
    }
}

void CollisionRenderer::initialise(unsigned int program_id_1, unsigned int program_id_2)
{
    this->program_id_1 = program_id_1;
    v_loc_1 = glGetUniformLocation(program_id_1, "V");

    this->program_id_2 = program_id_2;
    v_loc_2 = glGetUniformLocation(program_id_2, "V");
    m_loc_2 = glGetUniformLocation(program_id_2, "M");

    glGenVertexArrays(1, &static_VAO);
    glGenBuffers(1, &static_VBO);
    glGenBuffers(1, &static_EBO);

    glBindVertexArray(static_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, static_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_vertices.size() * sizeof(CollisionVertex),
        &static_vertices[0],
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_indices.size() * sizeof(unsigned short),
        &static_indices[0],
        GL_STATIC_DRAW
    );

    // 5. Define the vertex buffer attributes

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(CollisionVertex),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 4, GL_FLOAT, GL_FALSE, sizeof(CollisionVertex),
        (void*)offsetof(CollisionVertex, color)
    );
    glEnableVertexAttribArray(1);
}

void CollisionRenderer::enable_terrain(const glm::mat4 &view)
{
    glUseProgram(program_id_1);
    glBindVertexArray(static_VAO);
    glUniformMatrix4fv(v_loc_1, 1, GL_FALSE, &view[0][0]);
}

void CollisionRenderer::render_terrain()
{
    glDrawArrays(
        GL_LINES, terrain_vertices_offset, terrain_vertex_count
    );
}

void CollisionRenderer::enable_sprite_polygons(const glm::mat4 &view)
{
    glUseProgram(program_id_2);
    glBindVertexArray(static_VAO);
    glUniformMatrix4fv(v_loc_2, 1, GL_FALSE, &view[0][0]);
}

void CollisionRenderer::render_sprite_polygon(const component::Sprite &sprite)
{
    const SpritePolygon &sprite_polygon = sprite_polygons[(int)sprite.sprite_id];

    glUniformMatrix4fv(m_loc_2, 1, GL_FALSE, &sprite.model[0][0]);

    glDrawArrays(
        GL_LINE_LOOP,
        sprite_polygon.vertices_offset,
        sprite_polygon.vertex_count
    );

    glUniformMatrix4fv(m_loc_2, 1, GL_FALSE, &sprite.model_unrotated[0][0]);

    glDrawArrays(
        GL_LINE_LOOP,
        sprite_polygon.box_vertices_offset,
        4
    );
}

