#include "terrain.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <SOIL/SOIL.h>
#include "path_config.h"

namespace game {

    Terrain::Terrain(const std::string name, const Resource* geometry, const Resource* material, const Resource* texture, const Resource* nMap, HeightMap h) : SceneNode(name, geometry, material, texture) {
        
        if (nMap != NULL) {
            normalMap_ = nMap->GetResource();
        }

        heightmap_ = h;
    }


    Terrain::~Terrain() {
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
