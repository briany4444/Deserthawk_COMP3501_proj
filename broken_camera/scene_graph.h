#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene_node.h"
#include "resource.h"
#include "camera.h"

// Size of the texture that we will draw
#define FRAME_BUFFER_WIDTH 1024
#define FRAME_BUFFER_HEIGHT 768

namespace game {

    // Class that manages all the objects in a scene
    class SceneGraph {

        private:
            // Background color
            glm::vec3 background_color_;

            // Scene nodes to render
            std::vector<SceneNode *> node_;
            std::vector<SceneNode*> collidable_nodes_;

            //Particle effect
            std::vector<SceneNode*> effects_;


            // Frame buffer for drawing to texture
            GLuint frame_buffer_;
            // Quad vertex array for drawing from texture
            GLuint quad_array_buffer_;
            // Render targets
            GLuint texture_;
            GLuint depth_buffer_;

            double startTime_;

            void RemoveNode(std::string);

        public:

            enum Options
            {
                OBJ, EFFECTS
            };
            // Constructor and destructor
            SceneGraph(void);
            ~SceneGraph();

            // Background color
            void SetBackgroundColor(glm::vec3 color);
            glm::vec3 GetBackgroundColor(void) const;
            
            // Create a scene node from the specified resources
            SceneNode *CreateNode(std::string node_name, Resource *geometry, Resource *material, Resource* texture = NULL, Resource* normal_map = NULL);
            // Add an already-created node
            void AddNode(SceneNode *node, Options x = OBJ);
            // Find a scene node with a specific name
            SceneNode *GetNode(std::string node_name) const;
            void RemoveCollidable(std::string node_name);
            // Get node const iterator
            std::vector<SceneNode *>::const_iterator begin() const;
            std::vector<SceneNode *>::const_iterator end() const;

            inline std::vector<SceneNode *> GetGraph() { return node_; }
            inline std::vector<SceneNode *> GetCollidables() { return collidable_nodes_; }

            //Alpha Blending
            static void AlphaBlending(bool set);

            // Drawing from/to a texture
            // Setup the texture
            void SetupDrawToTexture(void);
            // Draw the scene into a texture
            void DrawToTexture(Camera* camera);
            // Process and draw the texture on the screen
            float DisplayTexture(GLuint program);
            // Save texture to a file in ppm format
            void SaveTexture(char* filename);

            // Draw the entire scene
            void Draw(Camera *camera, Options x = OBJ,  bool first = true);

            // Update entire scene
            void Update(float);

    }; // class SceneGraph

} // namespace game

#endif // SCENE_GRAPH_H_
