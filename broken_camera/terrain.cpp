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

        return abs(getTerrainY(pos) - pos[1]);
    }

    float Terrain::getTerrainY(glm::vec3 pos) {
        float u, v;
        u = (pos[0] - position_[0] + (terrain_width_ / 2)) / terrain_width_;
        v = (pos[2] - position_[2] + (terrain_length_ / 2)) / terrain_length_;

        int row = floor(v * heightmap_.height_);
        int col = floor(u * heightmap_.width_);
        int index = row * heightmap_.width_ * 3 + col * 3;

        float height = (heightmap_.hmap[index] / 255.0) * heightmap_.max_height;
        
        return position_[1] + height;
    }



} // namespace game
