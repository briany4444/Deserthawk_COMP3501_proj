#ifndef PLAYER_H_
#define PLAYER_H_

#define GLEW_STATIC
#define INIT_MAX_SPEED 1.9
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


namespace game {

    // Abstraction of a Player
    class Player { // should extend sceneNode

        public:
            Player(void);
            ~Player();
 
            // Get global Player attributes
            glm::vec3 GetPosition(void) const;
            glm::quat GetOrientation(void) const;

            // Set global Player attributes
            void SetPosition(glm::vec3 position);
            void SetOrientation(glm::quat orientation);
            
            // Perform global transformations of Player
            void Translate(glm::vec3 trans);
            void Rotate(glm::quat rot);

            void Accelerate(float);
            void Decelerate(float);

            // Get relative attributes of Player
            glm::vec3 GetForward(void) const;
            glm::vec3 GetSide(void) const;
            glm::vec3 GetUp(void) const;

            // Perform relative transformations of Player
            void Pitch(float angle);
            void Yaw(float angle);
            void Roll(float angle);

            // Set the view from Player parameters: initial position of Player,
            // point looking at, and up vector
            // Resets the current orientation and position of the Player
            void SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up);
            // Set projection from frustum parameters: field-of-view,
            // near and far planes, and width and height of viewport
            void SetProjection(GLfloat fov, GLfloat near, GLfloat far, GLfloat w, GLfloat h);
            // Set all Player-related variables in shader program
            void SetupShader(GLuint program);

            inline float GetRadius() { return radius_; }
            void AddMaxSpeed(float);
            void Update(double);

        private:
            glm::vec3 position_; // Position of Player
            glm::quat orientation_; // Orientation of Player
            glm::vec3 forward_; // Initial forward vector
            glm::vec3 side_; // Initial side vector
            glm::mat4 view_matrix_; // View matrix
            glm::mat4 projection_matrix_; // Projection matrix
            double speed_ = 0.0f;  // Current Speed
            float accel_magnitude = 4.0f;
            float radius_ = 1.0f;
            float max_speed_ = INIT_MAX_SPEED;

            // Create view matrix from current Player parameters
            void SetupViewMatrix(void);

    }; // class Player

} // namespace game

#endif // PLAYER_H_
