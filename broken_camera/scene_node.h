#ifndef SCENE_NODE_H_
#define SCENE_NODE_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "camera.h"
#include <vector>

namespace game {

    // Class that manages one object in a scene 
    class SceneNode {

        public:

            struct light_t {
                SceneNode* node;
                glm::vec3 light_color;
                float spec_power;
            };

            // Create scene node from given resources
            SceneNode(const std::string name, const Resource *geometry, const Resource *material, const Resource* texture = NULL, const Resource* normal_map = NULL);

            // Destructor
            ~SceneNode();
            
            // Get name of node
            const std::string GetName(void) const;

            // Get node attributes
            glm::vec3 GetPosition(void) const;
            glm::quat GetOrientation(void) const;
            glm::vec3 GetScale(void) const;
            glm::mat4 GetTransf();

            // Set node attributes
            void SetPosition(glm::vec3 position);
            void SetOrientation(glm::quat orientation);
            void SetScale(glm::vec3 scale);
            void SceneNode::SetParent(SceneNode* p);
            inline std::vector<SceneNode*> GetChildren() { return children_; }
            inline void SetOrbiting() { orbiting_ = true; }
            inline void SetJointPos(glm::vec3 p) { joint_pos_ = p; }
            inline void SetBlending(bool b) { blending_ = b; }
            inline void SetOrbitAxis(glm::vec3 a) { orbit_axis_ = a; }
            inline void SetOrbitSpeed(float s) { orbit_speed_ = s; }
            
            // Perform transformations on node
            void Translate(glm::vec3 trans);
            void Rotate(glm::quat rot);
            void Scale(glm::vec3 scale);

            // Draw the node according to scene parameters in 'camera'
            // variable
            virtual void Draw(Camera *camera);

            // Update the node
            virtual void Update(float);

            // OpenGL variables
            GLenum GetMode(void) const;
            GLuint GetArrayBuffer(void) const;
            GLuint GetElementArrayBuffer(void) const;
            GLsizei GetSize(void) const;
            GLuint GetMaterial(void) const;

            inline float GetRadius() { return radius_; }
            inline bool GetCollidable() { return collidable_; }
            inline std::string GetType() { return type_; }
            void SceneNode::Orbit(double d);

        protected:
            std::string name_; // Name of the scene node
            GLuint array_buffer_; // References to geometry: vertex and array buffers
            GLuint element_array_buffer_;
            GLenum mode_; // Type of geometry
            GLsizei size_; // Number of primitives in geometry
            GLuint material_; // Reference to shader program
            GLuint texture_;
            GLuint normal_map_;
            bool blending_;
            
            std::string type_ = "NoneType";
            float radius_ = 1.0f;
            bool collidable_ = false;
            std::vector<SceneNode*> children_;  // child nodes vector
            SceneNode* parent_;
            
            glm::vec3 position_; // Position of node
            glm::quat orientation_; // Orientation of node
            glm::vec3 joint_pos_;
            glm::vec3 scale_; // Scale of node
            // Set matrices that transform the node in a shader program
            void SetupShader(GLuint program);

            glm::vec3 orbit_axis_ = glm::vec3(1, 0, 0); // Orbit Axis
            bool orbiting_;     // whether obj is orbiting
            float orbit_angle_;  // current Orbit angle
            float orbit_speed_;

    }; // class SceneNode

} // namespace game

#endif // SCENE_NODE_H_
