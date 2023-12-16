#include "terrain.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <SOIL/SOIL.h>
#include "path_config.h"

namespace game {

    Terrain::Terrain(const std::string name, const Resource* geometry, const Resource* material, const Resource* texture, const Resource* nMap, HeightMap h, float l, float w) : SceneNode(name, geometry, material, texture, nMap) {
        
        if (nMap != NULL) {
            normalMap_ = nMap->GetResource();
        }

        heightmap_ = h;
        terrain_length_ = l;
        terrain_width_ = w;
    }


    Terrain::~Terrain() {
    }


    float Terrain::getDistToGround(glm::vec3 pos){

        return abs(getTerrainY(pos) - pos.y);
    }

    float Terrain::getTerrainY(glm::vec3 pos) {
        float u, v;
        u = (pos[0] - position_.x + (terrain_width_ / 2));
        v = pos[2] - position_.z + (terrain_length_ / 2);

        int row = floor(v);
        int col = floor(u);
        int index = row * heightmap_.width_ * 3 + col * 3;

        float height = (heightmap_.hmap[index] / 255.0) * heightmap_.max_height;
        
        return position_.y + height;
    }



} // namespace game
