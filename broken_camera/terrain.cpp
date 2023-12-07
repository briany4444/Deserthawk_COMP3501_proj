#include "terrain.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <SOIL/SOIL.h>
#include "path_config.h"

namespace game {

    Terrain::Terrain(const std::string name, const Resource* geometry, const Resource* material, const Resource* texture, const Resource* nMap, HeightMap h, float l, float w) : SceneNode(name, geometry, material, texture) {
        
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

        float u, v;
        u = float((pos[0] + (terrain_width_ / 2.0)) / terrain_width_);
        v = float((pos[2] + (terrain_length_ / 2.0)) / terrain_length_);

        int row = floor(v * heightmap_.height_);
        int col = floor(u * heightmap_.width_);

        float height = heightmap_.hmap[row * heightmap_.width_ * 3 + col * 3] / 255.0;

        float diff = abs((position_.y + height) - pos.y);

        return diff;
    }


    void Terrain::Draw(Camera* camera)
    {
        // Select proper material (shader program)
        glUseProgram(material_);

        // Set geometry to draw
        glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_);

        // Set globals for camera
        camera->SetupShader(material_);



        // Set world matrix and other shader input variables
        SetupShader(material_);

        // Draw geometry
        if (mode_ == GL_POINTS) {
            glDrawArrays(mode_, 0, size_);
        }
        else {
            glDrawElements(mode_, size_, GL_UNSIGNED_INT, 0);
        }

    }


} // namespace game
