#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene_node.h"
#include "resource.h"
#include "player.h"

namespace game {

    // Class that manages all the objects in a scene
    class SceneGraph {

        private:
            // Background color
            glm::vec3 background_color_;

            // Scene nodes to render
            std::vector<SceneNode *> node_;
            std::vector<SceneNode*> collidable_nodes_;

            void RemoveNode(std::string);

        public:
            // Constructor and destructor
            SceneGraph(void);
            ~SceneGraph();

            // Background color
            void SetBackgroundColor(glm::vec3 color);
            glm::vec3 GetBackgroundColor(void) const;
            
            // Create a scene node from the specified resources
            SceneNode *CreateNode(std::string node_name, Resource *geometry, Resource *material);
            // Add an already-created node
            void AddNode(SceneNode *node);
            // Find a scene node with a specific name
            SceneNode *GetNode(std::string node_name) const;
            void RemoveCollidable(std::string node_name);
            // Get node const iterator
            std::vector<SceneNode *>::const_iterator begin() const;
            std::vector<SceneNode *>::const_iterator end() const;

            inline std::vector<SceneNode *> GetGraph() { return node_; }
            inline std::vector<SceneNode *> GetCollidables() { return collidable_nodes_; }

            // Draw the entire scene
            void Draw(Player *player);

            // Update entire scene
            void Update(float);

    }; // class SceneGraph

} // namespace game

#endif // SCENE_GRAPH_H_
