#ifndef GAME_H_
#define GAME_H_

#include <exception>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

#include "orb.h"
#include "scene_graph.h"
#include "resource_manager.h"
#include "Player.h"
#include "asteroid.h"
#include "spaceship.h"
#include "terrain.h"
#include "tree.h"
#include "light.h"
#include "Ui.h"

namespace game {

    // Exception type for the game
    class GameException: public std::exception
    {
        private:
            std::string message_;
        public:
            GameException(std::string message) : message_(message) {};
            virtual const char* what() const throw() { return message_.c_str(); };
            virtual ~GameException() throw() {};
    };

    // Game application
    class Game {

        enum game_state_t { won, lost, inProgress , init, dead};

        public:
            // Constructor and destructor
            Game(void);
            ~Game();
            // Call Init() before calling any other method
            void Init(void); 
            // Set up resources for the game
            void SetupResources(void);
            // Set up initial scene
            void SetupScene(void);
            // Run the game: keep the application active
            void MainLoop(void); 


        private:
            // GLFW window
            GLFWwindow* window_;

            // Scene graph containing all nodes to render
            SceneGraph scene_;
            Terrain* terrain_;

            // Resources available to the game
            ResourceManager resman_;

            // Player abstraction
            Player player_;
            Camera camera_;
            int orbs_left_;
            Light* l;

            //hud
            Ui* gui_;

            // current game state
            game_state_t game_state_;

            // Methods to initialize the game
            void InitWindow(void);
            void InitView(void);
            void InitEventHandlers(void);
 
            // Methods to handle events
            static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
            static void ResizeCallback(GLFWwindow* window, int width, int height);

            void createTerrain(const char* file_name, glm::vec3);
            Orb* createOrbInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name);
            SceneNode* CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name = std::string(""), std::string normal_name = std::string(""));
            Light* CreateLightInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name);
            
            // handle Player-Scene node collisions
            void HandleCollisions();
            void CreateTrees();
            SceneNode* makePalmTree(int treeNum, glm::vec3 pos);

            //DebugMode
            void DebugCameraMovement();
            bool debugCamera_ = false;
            float debugMoveSpeed_ = 5.0f;
            glm::vec2 lastFrameMousePosition_ = glm::vec2(0.0, 0.0);

            //placing stuff/procedural generation
            void PlaceObject(SceneNode*, float x, float offSetY, float z);
            void CreateWorld();
            void createObeliskZone();
            void createVillage();
            void createOasis();
            void createSandNadoZone();
            void generateTerrainFeatures(float x, float z);

    }; // class Game

} // namespace game

#endif // GAME_H_

