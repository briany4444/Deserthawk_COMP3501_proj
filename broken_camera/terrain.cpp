#include "terrain.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <SOIL/SOIL.h>
#include "path_config.h"

namespace game {

    Terrain::Terrain(const std::string name, const Resource* geometry, const Resource* material, const Resource* texture, const Resource* heightMap, Camera* cam) : SceneNode(name, geometry, material, texture) {
        // Set texture
        if (heightMap) {
            texHeightMap_ = heightMap->GetResource();
        }
        else {
            texHeightMap_ = 0;
        }

        this->cam = cam;
    }


    Terrain::~Terrain() {
    }

    void Terrain::initGrid() {
        // hard coded to save time
        grid.length = 300;
        grid.width = 300;
        grid.numVertexX = 100;
        grid.numVertexY = 100;
    }


    float Terrain::getHeight(glm::vec2 uv) {

        
        return 0;
    }


 

    float Terrain::findHeight() {
        float camX = cam->GetPosition().x + grid.length / 2;
        float camZ = cam->GetPosition().z + grid.width / 2;

        float u = camX / grid.length;
        float v = camZ / grid.width;

        glBindTexture(GL_TEXTURE_2D, texHeightMap_);

        std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/moon.jpg");

        int width, height, channels;
        width = 1024;
        height = 1024;
        channels = 3;

        // Allocate memory for reading pixels
        GLubyte* pixels = new GLubyte[width * height * 3]; // 3 for RGB

        // Read the pixels from the texture
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

        // Calculate index for RGB
        int index = (camX * width + camZ);

        // Extract RGB values
        float red = pixels[index] / 255.0;

        delete[] pixels;

        return float(red * maxDepth_ - 8.0f);

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

        SetupShader(material_);

        // Texture
        if (texHeightMap_) {
            GLint tex = glGetUniformLocation(material_, "height_map");
            glUniform1i(tex, 1); // Assign the first texture to the map
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texHeightMap_); // First texture we bind
            // Define texture interpolation
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        // Draw geometry
        if (mode_ == GL_POINTS) {
            glDrawArrays(mode_, 0, size_);
        }
        else {
            glDrawElements(mode_, size_, GL_UNSIGNED_INT, 0);
        }

    }


} // namespace game
