#ifndef TERRAIN_H_
#define TERRAIN_H_

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "scene_node.h"

namespace game {

    // Abstraction of an asteroid
    class Terrain : public SceneNode {


        typedef struct
        {
            float length, width;
            int numVertexX, numVertexY;
        } gridInfo;

    public:
        // Create asteroid from given resources
        Terrain(const std::string name, const Resource* geometry, const Resource* material, const Resource* texture, const Resource* heightMap, Camera* cam);

        // Destructor
        ~Terrain();

        void Draw(Camera* camera);

        void initGrid();
        float getHeight(glm::vec2);
        float findHeight();

    private:
        // Angular momentum of asteroid
        glm::quat angm_;
        float maxDepth_ = 20;
        GLuint texHeightMap_; // Reference to texture resource
        gridInfo grid;

        Camera* cam;


    }; // class Asteroid

} // namespace game

#endif // TERRAIN_H_
