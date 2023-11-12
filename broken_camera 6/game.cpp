#include <iostream>
#include <time.h>
#include <sstream>

#include "game.h"
#include "path_config.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Main window settings
const std::string window_title_g = "A2 Race";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

// Viewport and Player settings
float Player_near_clip_distance_g = 0.01;
float Player_far_clip_distance_g = 1000.0;
float Player_fov_g = 60.0; // Field-of-view of Player (degrees)
const glm::vec3 viewport_background_color_g(0, 0.0, 0.0);
glm::vec3 Player_position_g(0.0, 0.0, 800.0);
glm::vec3 Player_look_at_g(0.0, 0.0, 0.0);
glm::vec3 Player_up_g(0.0, 1.0, 0.0);
static double last_time = 0;

// Materials 
const std::string material_directory_g = MATERIAL_DIRECTORY;


Game::Game(void){

    // Don't do work in the constructor, leave it for the Init() function
}


void Game::Init(void){

    // Run all initialization steps
    InitWindow();
    InitView();
    InitEventHandlers();

    // Set variables
    game_state_ = inProgress;
}

       
void Game::InitWindow(void){

    // Initialize the window management library (GLFW)
    if (!glfwInit()){
        throw(GameException(std::string("Could not initialize the GLFW library")));
    }

    // Create a window and its OpenGL context
    if (window_full_screen_g){
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), glfwGetPrimaryMonitor(), NULL);
    } else {
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
    }
    if (!window_){
        glfwTerminate();
        throw(GameException(std::string("Could not create window")));
    }

    // Make the window's context the current 
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK){
        throw(GameException(std::string("Could not initialize the GLEW library: ")+std::string((const char *) glewGetErrorString(err))));
    }
}


void Game::InitView(void){

    // Set up z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set viewport
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, width, height);

    // Set up Player
    // Set current view
    Player_.SetView(Player_position_g, Player_look_at_g, Player_up_g);
    // Set projection
    Player_.SetProjection(Player_fov_g, Player_near_clip_distance_g, Player_far_clip_distance_g, width, height);
}


void Game::InitEventHandlers(void){

    // Set event callbacks
    glfwSetKeyCallback(window_, KeyCallback);
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Set pointer to game object, so that callbacks can access it
    glfwSetWindowUserPointer(window_, (void *) this);
}


void Game::SetupResources(void){

    // Create a simple object to represent the asteroids
    resman_.CreateCone("Asteroid", 2.0, 1.0, 10, 10);
    resman_.CreateTorus("Target", 1);
    resman_.CreateCone("Beacon", 2, 2, 30, 30);
    resman_.CreateCylinder("Enemy");
    resman_.CreateSphere("Powerup");


    // Load material to be applied to asteroids
    std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/material");
    resman_.LoadResource(Material, "ObjectMaterial", filename.c_str());
}


void Game::SetupScene(void){

    // Set background color for the scene
    scene_.SetBackgroundColor(viewport_background_color_g);

    // init Racetrack
    std::vector<Beacon*> beacons = racetrack_.InitBeacons(resman_);
    for (int i = 0; i < beacons.size(); i++) {
        scene_.AddNode(beacons[i]);
    }

    CreateShips();
    CreatePowerups();
    CreateAsteroidField();
}


void Game::MainLoop(void){

    // Loop while the user did not close the window
    while (!glfwWindowShouldClose(window_)){

        // Animate the scene
        if (game_state_ == inProgress){
            double current_time = glfwGetTime();
            if ((current_time - last_time) > 0.05){
                scene_.Update(current_time - last_time);
                Player_.Update(current_time - last_time);
                last_time = current_time;
            }
            HandleCollisions();
        }

        // Draw the scene
        scene_.Draw(&Player_);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);

        // Update other events like input handling
        glfwPollEvents();
    }
}


void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

    // Get user data with a pointer to the game class
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;

    // Quit game if 'q' is pressed
    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    if (game->game_state_ == inProgress) {
        // View control
        float rot_factor(glm::pi<float>() * 3 / 180); // amount the ship turns per keypress
        float trans_factor = 1.0; // amount the ship steps forward per keypress
        if (key == GLFW_KEY_UP) {
            game->Player_.Pitch(rot_factor);
        }
        if (key == GLFW_KEY_DOWN) {
            game->Player_.Pitch(-rot_factor);
        }
        if (key == GLFW_KEY_LEFT) {
            game->Player_.Yaw(rot_factor);
        }
        if (key == GLFW_KEY_RIGHT) {
            game->Player_.Yaw(-rot_factor);
        }
        if (key == GLFW_KEY_Z) {
            game->Player_.Roll(-rot_factor);
        }
        if (key == GLFW_KEY_X) {
            game->Player_.Roll(rot_factor);
        }
        if (key == GLFW_KEY_W) {
            game->Player_.Accelerate(glfwGetTime() - last_time);
        }
        if (key == GLFW_KEY_S) {
            game->Player_.Decelerate(glfwGetTime() - last_time);
        }
    }

 
}


void Game::ResizeCallback(GLFWwindow* window, int width, int height){

    // Set up viewport and Player projection based on new window size
    glViewport(0, 0, width, height);
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;
    game->Player_.SetProjection(Player_fov_g, Player_near_clip_distance_g, Player_far_clip_distance_g, width, height);
}


Game::~Game(){
    
    glfwTerminate();
}

void Game::CreateShips() {
    // Create a number of ship instances
    for (int i = 0; i < 4; i++) {
        // Create instance name
        std::stringstream ss;
        ss << i;
        std::string index = ss.str();
        std::string name = "Enemy" + index;

        // Create ship instance
        Spaceship* ship = CreateShipInstance(name, "Enemy", "ObjectMaterial");
        ship->SetPosition(glm::vec3(-3, 0, 800 - (i+1) * 40));
        ship->SetScale(glm::vec3(0.5, 0.5, 0.5));
        ship->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>() * ((float)rand() / RAND_MAX), glm::vec3(((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX)))));
    }
}

Spaceship* Game::CreateShipInstance(std::string entity_name, std::string object_name, std::string material_name) {
    // Get resources
    Resource* geom = resman_.GetResource(object_name);
    if (!geom) {
        throw(GameException(std::string("Could not find resource \"") + object_name + std::string("\"")));
    }

    Resource* mat = resman_.GetResource(material_name);
    if (!mat) {
        throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
    }

    // Create ship
    Spaceship* ship = new Spaceship(&Player_, entity_name, geom, mat);
    scene_.AddNode(ship);
    return ship;
}


Asteroid *Game::CreateAsteroidInstance(std::string entity_name, std::string object_name, std::string material_name){

    // Get resources
    Resource *geom = resman_.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = resman_.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }

    // Create asteroid instance
    Asteroid *ast = new Asteroid(entity_name, geom, mat);
    scene_.AddNode(ast);
    return ast;
}


void Game::CreateAsteroidField(int num_asteroids){

    // Create a number of asteroid instances
    for (int i = 0; i < num_asteroids; i++){
        // Create instance name
        std::stringstream ss;
        ss << i;
        std::string index = ss.str();
        std::string name = "AsteroidInstance" + index;

        // Create asteroid instance
        Asteroid *ast = CreateAsteroidInstance(name, "Asteroid", "ObjectMaterial");

        // Set attributes of asteroid: random position, orientation, and
        // angular momentum
        ast->SetPosition(glm::vec3(-300.0 + 600.0*((float) rand() / RAND_MAX), -300.0 + 600.0*((float) rand() / RAND_MAX), 600.0*((float) rand() / RAND_MAX)));
        ast->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
        ast->SetAngM(glm::normalize(glm::angleAxis(0.05f*glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
    }
}

Powerup* Game::CreatePowerupInstance(std::string entity_name, std::string object_name, std::string material_name) {

    // Get resources
    Resource* geom = resman_.GetResource(object_name);
    if (!geom) {
        throw(GameException(std::string("Could not find resource \"") + object_name + std::string("\"")));
    }

    Resource* mat = resman_.GetResource(material_name);
    if (!mat) {
        throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
    }

    // Create Powerup instance
    Powerup* pu = new Powerup(entity_name, geom, mat);
    scene_.AddNode(pu);
    return pu;
}

void Game::CreatePowerups() {

    // init the target Powerup
    Powerup* close_pu = CreatePowerupInstance("Powerup0", "Powerup", "ObjectMaterial");
    close_pu->SetPosition(glm::vec3(0, 0, 795));
    close_pu->SetScale(glm::vec3(0.5, 0.5, 0.5));
    close_pu->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>() * ((float)rand() / RAND_MAX), glm::vec3(((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX)))));

    for (int i = 0; i < 4; i++) {

        // Create instance name
        std::stringstream ss;
        ss << i+1;
        std::string index = ss.str();
        std::string name = "Powerup" + index;

        // Create asteroid instance
        Powerup* pu = CreatePowerupInstance(name, "Powerup", "ObjectMaterial");
        pu->SetScale(glm::vec3(0.75, 0.75, 0.75));
        pu->SetPosition(glm::vec3 (0, 0, 800 - (i+1) * 40));
        pu->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>() * ((float)rand() / RAND_MAX), glm::vec3(((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX)))));
    }
}


void Game::HandleCollisions() {

    std::vector<SceneNode*> collidables = scene_.GetCollidables();
    for (int i = 0; i < collidables.size(); ) {
        SceneNode* curr_node = collidables[i];
        float node_dist = glm::length(curr_node->GetPosition() - Player_.GetPosition());

        // if collision occurred
        if (Player_.GetRadius() + curr_node->GetRadius() > node_dist) {

            // handles Player - Beacon collision
            if (curr_node->GetName() == "targetBeacon") {
                scene_.RemoveCollidable(racetrack_.GetNextName());
                std::cout << "You reached a beacon!" << std::endl;
                if (racetrack_.UpdateRaceTrack()) {
                    std::cout << "You reached all beacons in the correct order - You Won!" << std::endl;
                    game_state_ = won;
                    break;
                }
                continue;

            // handles Player - Enemy Collision
            } else if (curr_node->GetType() == "Spaceship") {
                game_state_ = lost;
                std::cout << "Collided with the enemy cylinder-ships - You lost!" << std::endl;
                break;

            // handles Player - Power Up Collision
            } else if (curr_node->GetType() == "Powerup") {
                Player_.AddMaxSpeed(4.0f);
                std::cout << "You collected a powerup! Max speed increased by 4 units" << std::endl;
                scene_.RemoveCollidable(curr_node->GetName());
            }
        }
        i++;
    }

}

} // namespace game
