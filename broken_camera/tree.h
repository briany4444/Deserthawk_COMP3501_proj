#ifndef TREE_H_
#define TREE_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "scene_node.h"
#include "thorn.h"

namespace game {

    enum sway_State_t { clockwise, counterclockwise };

    // Abstraction of an tree
    class Tree : public SceneNode {

    public:
        // Create tree from given resources
        Tree(const std::string name, const Resource* geometry, const Resource* material, float, float, const Resource*, const Resource*);

        inline float GetLength() { return length_; }
        // Destructor
        ~Tree();
        void Update(float) override;

        void createBranches(const Resource* geometry, const Resource* material, int);
        void createThorns(const Resource*, const Resource*, int);

    private:
        float length_;
        float radius_;
        const Resource* thorn_geom;
        sway_State_t curr_state_t;

    }; // class Tree

} // namespace game

#endif // TREE_H_
