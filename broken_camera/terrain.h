#ifndef TERRAIN_H_
#define TERRAIN_H_

#include <string>
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

    public:
        // Create asteroid from given resources
        Terrain(const std::string name, const Resource* geometry, const Resource* material, const Resource* texture);

        // Destructor
        ~Terrain();


    protected:
        void SetUpShader(GLuint program);
    private:
        // Angular momentum of asteroid
        glm::quat angm_;
        float maxDepth_ = 1;


    }; // class Asteroid

} // namespace game

#endif // TERRAIN_H_
