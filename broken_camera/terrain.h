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
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resource.h"
#include "scene_node.h"

namespace game {

    // Abstraction of an asteroid
    class Terrain : public SceneNode {

    public:
        // Create asteroid from given resources
        Terrain(const std::string name, const Resource* geometry, const Resource* material, const Resource* texture, const Resource* normalMap, HeightMap h);

        // Destructor
        ~Terrain();

        void Draw(Camera* camera);

    private:
        GLuint normalMap_;
        HeightMap heightmap_;


    }; // class Asteroid

} // namespace game

#endif // TERRAIN_H_
