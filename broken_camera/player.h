#ifndef PLAYER_H_
#define PLAYER_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "scene_node.h"


namespace game {

    // Abstraction of a Player
    class Player { // should extend sceneNode

        public:
            Player(void);
            ~Player();
            
            //shape
            void SetShape(SceneNode* shape);
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

            void MoveForward();
            void MoveBackward();
            void MoveStop();

            // Get relative attributes of Player
            glm::vec3 GetForward(void) const;
            glm::vec3 GetSide(void) const;
            glm::vec3 GetUp(void) const;

            // Perform relative transformations of Player
            void Pitch(float angle);
            void Yaw(float angle);
            void Roll(float angle);

            void Init(glm::vec3 position, glm::vec3 look_at, glm::vec3 up);

            inline float GetRadius() { return radius_; }
            void AddMaxSpeed(float);
            void Update(double);

        private:
            //geometry
            SceneNode* shape_;
            
            //attributes
            glm::vec3 position_; // Position of Player
            glm::quat orientation_; // Orientation of Player
            glm::vec3 forward_; // Initial forward vector
            glm::vec3 side_; // Initial side vector
            
            double speed_ = 0.0f;  // Current Speed
            float accel_magnitude = 4.0f;
            float radius_ = 1.0f;
            float max_speed_ = 10;

            

    }; // class Player

} // namespace game

#endif // PLAYER_H_
