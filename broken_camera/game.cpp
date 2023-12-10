#include <iostream>
#include <time.h>
#include <sstream>


#include "game.h"
#include "path_config.h"
#include <string>

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Main window settings
const std::string window_title_g = "A2 Race";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

// Viewport and Player settings
float camera_near_clip_distance_g = 0.01;
float camera_far_clip_distance_g = 1000.0;
float camera_fov_g = 60.0; // Field-of-view of Player (degrees)
const glm::vec3 viewport_background_color_g(0, 0.0, 0.0);
glm::vec3 camera_position_g(0.0, 0.0, 800.0);
glm::vec3 camera_look_at_g(0.0, 0.0, 0.0);
glm::vec3 camera_up_g(0.0, 1.0, 0.0);
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
    game_state_ = init;
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

    //init player
    player_ = Player();
    player_.Init(camera_position_g, camera_look_at_g, camera_up_g);

    // Set up camera
    camera_ = Camera();
    // Set current view
    camera_.SetView(camera_position_g, camera_look_at_g, camera_up_g);
    // Set projection
    camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);

    
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
    resman_.CreateSphereParticles("SphereParticles", 250);

    // Create a simple object to represent the tree
    {
    resman_.CreateCone("SimpleObject", 2.0, 1.0, 10, 10);
    resman_.CreateCylinder("tree", 20, 2, 100, 100);
    resman_.CreateCylinder("branch", 5.0, 0.5, 100, 100);
    resman_.CreateCone("thorn", 0.5, 0.2, 90, 90);
    }

    resman_.CreateSphere("lightMesh", 0.5, 30, 30);

    resman_.CreateWall("SimpleWall"); //UI

  
    std::string filename;

    ////// MATERIALS //////
    {
        
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/material");
        resman_.LoadResource(Material, "ObjectMaterial", filename.c_str());

        // Load material to be applied to gui
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/ui");
        resman_.LoadResource(Material, "GuiMaterial", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/texture_and_normal");
        resman_.LoadResource(Material, "TextureNormalMaterial", filename.c_str());

        
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textured_material");
        resman_.LoadResource(Material, "RandomTexMaterial", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/normal_map");
        resman_.LoadResource(Material, "TerrainMat", filename.c_str());


        /// Particle Systems ///

        // filename = std::string(MATERIAL_DIRECTORY) + std::string("/Sand-nato");
        // resman_.LoadResource(Material, "PS-SandTornatoMaterial", filename.c_str());

        // filename = std::string(MATERIAL_DIRECTORY) + std::string("/firefly_particle");
        // resman_.LoadResource(Material, "PS-FirFlyMaterial", filename.c_str());

        
    }

    ////// TEXTURES ////// 
    {
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/moon.jpg");
        resman_.LoadResource(Texture, "MoonTex", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/sparkle.png");
        resman_.LoadResource(Texture, "sparkle", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/SandParticle.png");
        resman_.LoadResource(Texture, "SandParticle", filename.c_str());


        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/rocky.png");
        resman_.LoadResource(Texture, "Texture1", filename.c_str());
        
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/noise.png");
        resman_.LoadResource(Texture, "NoiseTex", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/sandy_with_artificial_shadows.png");
         resman_.LoadResource(Texture, "TextureMaterial", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/normal_map2.png");
        resman_.LoadResource(Texture, "Texture2", filename.c_str());

    }

    

    /*Dylans Game Objects*/ if (true ) //this line is here so that this large section of code can be collasped
    {
        //Obelisk
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/models/Obelisk.obj");
        resman_.LoadResource(Mesh, "ObeliskMesh", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Obelisk_Texture.png");
        resman_.LoadResource(Texture, "ObeliskTexture", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Obelisk_Normal.png");
        resman_.LoadResource(Texture, "ObeliskNormal", filename.c_str());

        //Watch Tower
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/models/Watch_Tower.obj");
        resman_.LoadResource(Mesh, "WatchTowerBaseMesh", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Watch_Tower_Texture.png");
        resman_.LoadResource(Texture, "WatchTowerBaseTexture", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Watch_Tower_Normal.png");
        resman_.LoadResource(Texture, "WatchTowerBaseNormal", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/models/Watch_Eye.obj");
        resman_.LoadResource(Mesh, "WatchEyeMesh", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Watch_Eye_Texture.png");
        resman_.LoadResource(Texture, "WatchEyeTexture", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Watch_Eye_Normal.png");
        resman_.LoadResource(Texture, "WatchEyeNormal", filename.c_str());

        //Palm Tree
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/models/Palm_tree_trunk.obj");
        resman_.LoadResource(Mesh, "PalmTreeTrunkMesh", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Palm_tree_trunk_Texture.png");
        resman_.LoadResource(Texture, "PalmTreeTrunkTexture", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Palm_tree_Normal.png");
        resman_.LoadResource(Texture, "PalmTreeNormal", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/models/Palm_tree_head.obj");
        resman_.LoadResource(Mesh, "PalmTreeHeadMesh", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Palm_tree_head_Texture.png");
        resman_.LoadResource(Texture, "PalmTreeHeadTexture", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/models/Palm_tree_leaf.obj");
        resman_.LoadResource(Mesh, "PalmTreeLeafMesh", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Palm_tree_leaf_Texture.png");
        resman_.LoadResource(Texture, "PalmTreeLeafTexture", filename.c_str());

        //Dry Shrub
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/models/Dry_shrub.obj");
        resman_.LoadResource(Mesh, "DryShrubMesh", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Dry_shrub_Texture.png");
        resman_.LoadResource(Texture, "DryShrubMeshTexture", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Dry_shrub_Normal.png");
        resman_.LoadResource(Texture, "DryShrubMeshNormal", filename.c_str());

        //Tree
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/models/Tree.obj");
        resman_.LoadResource(Mesh, "TreeMesh", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Tree_Texture.png");
        resman_.LoadResource(Texture, "TreeTexture", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Tree_Normal.png");
        resman_.LoadResource(Texture, "TreeNormal", filename.c_str());

        //Hut 1
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/models/Hut_1.obj");
        resman_.LoadResource(Mesh, "Hut1Mesh", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Hut_1_Texture.png");
        resman_.LoadResource(Texture, "Hut1Texture", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Hut_1_Normal.png");
        resman_.LoadResource(Texture, "Hut1Normal", filename.c_str());

        //Oasis Plant
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/models/Oasis_plant.obj");
        resman_.LoadResource(Mesh, "OasisPlantMesh", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Oasis_plant_Texture.png");
        resman_.LoadResource(Texture, "OasisPlantTexture", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Oasis_plant_Normal.png");
        resman_.LoadResource(Texture, "OasisPlantNormal", filename.c_str());

        //Tumbleweed
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/models/Tumbleweed.obj");
        resman_.LoadResource(Mesh, "TumbleweedMesh", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Dry_shrub_Texture.png");
        resman_.LoadResource(Texture, "TumbleweedTexture", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Dry_shrub_Normal.png");
        resman_.LoadResource(Texture, "TumbleweedNormal", filename.c_str());
    }
    

}


void Game::SetupScene(void) {

    std::srand(time(NULL));

    // Set background color for the scene
    scene_.SetBackgroundColor(viewport_background_color_g);
    effects_.SetBackgroundColor(viewport_background_color_g);

    //player
    SceneNode* playerShape = new SceneNode("PlayerShape", resman_.GetResource("Powerup"), resman_.GetResource("ObjectMaterial"));
    player_.SetShape(playerShape);
    scene_.AddNode(playerShape);
    //gui
    //gui_ = new Ui("Hud", resman_.GetResource("SimpleWall"), resman_.GetResource("GuiMaterial"));

    // Create global light source
    {
        l = CreateLightInstance("light", "lightMesh", "RandomTexMaterial", "Texture1");
        l->SetPosition(glm::vec3(0, 0, 800));
        l->SetJointPos(glm::vec3(0, 0, 10));
        l->SetOrbiting();
        l->SetOrbitSpeed(0.5);
        l->SetOrbitAxis(glm::vec3(0, 1, 0));
    }

    // terrain
    {
        createTerrain("/textures/Moon.jpg", glm::vec3(0, -30, 790));
        CreateTrees();
    }

    /*Dylans Game Objects*/ if (true) //this line is here so that this large section of code can be collasped
    {
        int num_instances = 5; // For loops

        //Obelisk
        game::SceneNode* obelisk = CreateInstance("Obelisk", "ObeliskMesh", "TextureNormalMaterial", "ObeliskTexture", "ObeliskNormal");
        obelisk->Translate(glm::vec3(0.0, -18.0, 800.0));

        //Watch Towers
        num_instances = 5;
        for (int i = 0; i < num_instances; i++)
        {
            std::stringstream ss;
            ss << i;
            std::string index = ss.str();

            std::string name = "WatchTowerInstance" + index;
            game::SceneNode* newWatchTower = CreateInstance(name, "WatchTowerBaseMesh", "TextureNormalMaterial", "WatchTowerBaseTexture", "WatchTowerBaseNormal");
            newWatchTower->Translate(glm::vec3(0.0 + i * 5.0f, -18.0, 775.0));

            name = "WatchEyeInstance" + index;
            game::SceneNode* watchEye = CreateInstance("WatchEye", "WatchEyeMesh", "TextureNormalMaterial", "WatchEyeTexture", "WatchEyeNormal");
            watchEye->Translate(glm::vec3(0.0 + i * 5.0f, -8.0, 775.0));
        }


        //Palm Tree
        game::SceneNode* palmTreeTrunk = CreateInstance("PalmTreeTrunk", "PalmTreeTrunkMesh", "TextureNormalMaterial", "PalmTreeTrunkTexture", "PalmTreeNormal");
        palmTreeTrunk->Translate(glm::vec3(0.0, -18.0, 760.0));
        game::SceneNode* palmTreeHead = CreateInstance("PalmTreeHead", "PalmTreeHeadMesh", "TextureNormalMaterial", "PalmTreeHeadTexture", "PalmTreeNormal");
        palmTreeHead->SetParent(palmTreeTrunk);
        for (int i = 0; i < 14; i++)
        {
            std::stringstream ss;
            ss << i;
            std::string index = ss.str();

            std::string name = "Leaf" + index;
            game::SceneNode* newLeaf = CreateInstance(name, "PalmTreeLeafMesh", "TextureNormalMaterial", "PalmTreeLeafTexture", "PalmTreeNormal");
            newLeaf->Translate(glm::vec3(0.0, 6.0, 0.0));
            if (i > 7) newLeaf->Rotate(glm::angleAxis(0.5f + 0.04f * (float)i, glm::vec3(1, 0, 0)));
            newLeaf->Rotate(glm::angleAxis((float)i,glm::vec3(0,1,0)));
            newLeaf->SetParent(palmTreeHead);
            
        }

        //Dry Shrubs
        num_instances = 5;
        for (int i = 0; i < num_instances; i++)
        {
            std::stringstream ss;
            ss << i;
            std::string index = ss.str();

            std::string name = "DryShrub" + index;
            game::SceneNode* newDryShrub = CreateInstance("DryShrub", "DryShrubMesh", "TextureNormalMaterial", "DryShrubMeshTexture", "DryShrubMeshNormal");
            newDryShrub->Translate(glm::vec3(0.0 + i * 5.0f, -18.0, 750.0));
        }

        //Trees
        num_instances = 5;
        for (int i = 0; i < num_instances; i++)
        {
            std::stringstream ss;
            ss << i;
            std::string index = ss.str();

            std::string name = "Tree" + index;
            game::SceneNode* newTree = CreateInstance("Tree", "TreeMesh", "TextureNormalMaterial", "TreeTexture", "TreeNormal");
            newTree->Translate(glm::vec3(0.0 + i * 5.0f, -18.0, 740.0));
        }

        //Hut1
        game::SceneNode* hut1 = CreateInstance("Hut1", "Hut1Mesh", "TextureNormalMaterial", "Hut1Texture", "Hut1Normal");
        hut1->Translate(glm::vec3(0.0, -18.0, 730.0));

        //Oasis Plant
        game::SceneNode* oasisPlant = CreateInstance("OasisPlant", "OasisPlantMesh", "TextureNormalMaterial", "OasisPlantTexture", "OasisPlantNormal");
        oasisPlant->Translate(glm::vec3(0.0, -18.0, 720.0));

        //Tumbleweeds
        num_instances = 5;
        for (int i = 0; i < num_instances; i++)
        {
            std::stringstream ss;
            ss << i;
            std::string index = ss.str();

            std::string name = "Tumbleweed" + index;
            game::SceneNode* newTumbleweed = CreateInstance("Tumbleweed", "TumbleweedMesh", "TextureNormalMaterial", "TumbleweedTexture", "TumbleweedNormal");
            newTumbleweed->Translate(glm::vec3(0.0 + i * 5.0f, -18.0, 710.0));
        }


    }


    //gui
    gui_ = new Ui("Hud", resman_.GetResource("SimpleWall"), resman_.GetResource("GuiMaterial"), resman_.GetResource("NoiseTex"));

    ////// PARTICLE SYSTEMS ////// 
    {
    



    }

    game_state_ = inProgress;

}


void Game::MainLoop(void){

    // Loop while the user did not close the window
    while (!glfwWindowShouldClose(window_)){

        // Animate the scene
        if (game_state_ == inProgress){
            double current_time = glfwGetTime();
            double delta_time = current_time - last_time;
            if ((delta_time) > 0.05){
                glEnable(GL_CULL_FACE);
                scene_.Update(delta_time);
                player_.Update(delta_time);
                if (!debugCamera_)
                {
                    camera_.Update(player_.GetOrientation(), player_.GetForward(), player_.GetSide(),
                        player_.GetPosition());
                }
                else
                {
                    DebugCameraMovement();
                }

                camera_.UpdateLightInfo(l->GetTransf() * glm::vec4(l->GetPosition(), 1.0), l->GetLightCol(), l->GetSpecPwr());
                
                last_time = current_time;
            }
            HandleCollisions();
        }

        // Draw the scene
        scene_.Draw(&camera_);

        effects_.AlphaBlending(true);
        //effects_.Draw(&camera_, false);
        //gui_->Draw(&camera_);
        //effects_.Draw(&camera_);
       // gui_->Draw(&camera_);
        effects_.AlphaBlending(false);

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
            game->player_.Pitch(rot_factor);
        }
        if (key == GLFW_KEY_DOWN) {
            game->player_.Pitch(-rot_factor);
        }
        if (key == GLFW_KEY_LEFT) {
            game->player_.Yaw(rot_factor);
        }
        if (key == GLFW_KEY_RIGHT) {
            game->player_.Yaw(-rot_factor);
        }
        if (key == GLFW_KEY_Z) {
            game->player_.Roll(-rot_factor);
        }
        if (key == GLFW_KEY_X) {
            game->player_.Roll(rot_factor);
        }
        if (key == GLFW_KEY_W) {
            game->player_.Accelerate(glfwGetTime() - last_time);
        }
        if (key == GLFW_KEY_S) {
            game->player_.Decelerate(glfwGetTime() - last_time);
        }
        if (key == GLFW_KEY_B && action == GLFW_PRESS) {
            if (game->debugCamera_)
            {
                game->debugCamera_ = false;
                glfwSetInputMode(game->window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else
            {
                game->debugCamera_ = true;
                glfwSetInputMode(game->window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
        }
        if (key == GLFW_KEY_Y) {
            game->l->Translate(glm::vec3(0.0, 2.0, 0.0));
        }
        if (key == GLFW_KEY_H) {
            game->l->Translate(glm::vec3(0.0, -2.0, 0.0));
        }
    }

 
}


void Game::ResizeCallback(GLFWwindow* window, int width, int height){

    // Set up viewport and Player projection based on new window size
    glViewport(0, 0, width, height);
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;
    game->camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}


Game::~Game(){
    
    glfwTerminate();
}



// creates tree instance in the scene
void Game::CreateTrees() {

    // Get resources
    Resource* geom = resman_.GetResource("tree");
    Resource* mat = resman_.GetResource("RandomTexMaterial");
    Resource* branch_geom = resman_.GetResource("branch");
    Resource* thorn_geom = resman_.GetResource("thorn");

    // creates tree instance and updates attributes
    Tree* tree = new Tree("tree", geom, mat, 20, 1, thorn_geom, resman_.GetResource("MoonTex"));
    tree->SetPosition(glm::vec3(0, -5, 790));
    scene_.AddNode(tree);
    tree->createBranches(branch_geom, mat, 4);

    // only make main branches orbit/sway
    std::vector<SceneNode*> main_branches = tree->GetChildren();
    for (int i = 0; i < main_branches.size(); i++) {
        main_branches[i]->SetOrbiting();
    }
}

void Game::DebugCameraMovement()
{
    double* mouseX = new double(0);
    double* mouseY = new double(0);

    glfwGetCursorPos(window_, mouseX, mouseY);

    glm::vec2 mousePosition = glm::vec2(*mouseX, *mouseY);

    glm::vec2 mouseSlide = mousePosition - lastFrameMousePosition_;

    lastFrameMousePosition_ = mousePosition;

    camera_.Yaw(-mouseSlide.x / 1000.0f);
    camera_.Pitch(-mouseSlide.y / 1000.0f); //Honestly 1000 just seemed like a good number to control the turn speed using mouse controls

    // A : Strafe Left
    if (glfwGetKey(window_, GLFW_KEY_A)) {
        camera_.SetPosition(camera_.GetPosition() + -camera_.GetSide() * debugMoveSpeed_);
    }

    // D : Strafe Right
    if (glfwGetKey(window_, GLFW_KEY_D)) {
        camera_.SetPosition(camera_.GetPosition() + camera_.GetSide() * debugMoveSpeed_);
    }

    // W : Go Foraward
    if (glfwGetKey(window_, GLFW_KEY_W)) {
        camera_.SetPosition(camera_.GetPosition() + camera_.GetForward() * debugMoveSpeed_);
    }

    // S : Go Backward
    if (glfwGetKey(window_, GLFW_KEY_S)) {
        camera_.SetPosition(camera_.GetPosition() + -camera_.GetForward() * debugMoveSpeed_);
    }

    // Space : Acsend
    if (glfwGetKey(window_, GLFW_KEY_SPACE)) {
        camera_.SetPosition(camera_.GetPosition() + camera_.GetUp() * debugMoveSpeed_);
    }

    // Shift : Descend
    if (glfwGetKey(window_, GLFW_KEY_LEFT_SHIFT)) {
        camera_.SetPosition(camera_.GetPosition() + -camera_.GetUp() * debugMoveSpeed_);
    }
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
    Spaceship* ship = new Spaceship(&player_, entity_name, geom, mat);
    scene_.AddNode(ship);
    return ship;
}

SceneNode* Game::CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, std::string normal_name) {

    Resource* geom = resman_.GetResource(object_name);
    if (!geom) {
        throw(GameException(std::string("Could not find resource \"") + object_name + std::string("\"")));
    }

    Resource* mat = resman_.GetResource(material_name);
    if (!mat) {
        throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
    }

    Resource* tex = NULL;
    if (texture_name != "") {
        tex = resman_.GetResource(texture_name);
        if (!tex) {
            throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
        }
    }

    Resource* norm = NULL;
    if (normal_name != "") {
        norm = resman_.GetResource(normal_name);
        if (!norm) {
            throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
        }
    }

    SceneNode* scn = scene_.CreateNode(entity_name, geom, mat, tex);
    return scn;
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
        float node_dist = glm::length(curr_node->GetPosition() - player_.GetPosition());

        // if collision occurred
        if (player_.GetRadius() + curr_node->GetRadius() > node_dist) {
            std::cout << "c" << std::endl;
            // handles Player - Beacon collision
            if (curr_node->GetName() == "targetBeacon") {
                scene_.RemoveCollidable(racetrack_.GetNextName());
                std::cout << "You reached a beacon!" << std::endl;
                if (racetrack_.UpdateRaceTrack()) {
                    std::cout << "You reached all beacons in the correct order - You Won!" << std::endl;
                    //game_state_ = won;
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
                player_.AddMaxSpeed(4.0f);
                std::cout << "You collected a powerup! Max speed increased by 4 units" << std::endl;
                scene_.RemoveCollidable(curr_node->GetName());
            }
        }
        i++;
    }

}

void Game::createTerrain(const char* file_name, glm::vec3 pos) {

    HeightMap heightMap;
    heightMap.max_height = 10;

    std::string f_name = std::string(MATERIAL_DIRECTORY) + std::string(file_name);
    heightMap.hmap = SOIL_load_image(f_name.c_str(), &heightMap.width_, &heightMap.height_, 0, SOIL_LOAD_RGB);

    float terrain_l = 300;
    float terrain_w = 300;
    resman_.CreatePlane("terrain", terrain_l, terrain_w, terrain_l, terrain_w, heightMap);

    Terrain* t = new Terrain("terrain", resman_.GetResource("terrain"), resman_.GetResource("TerrainMat"), resman_.GetResource("Texture1"), resman_.GetResource("Texture2"), heightMap, terrain_l, terrain_w);
    t->SetPosition(pos);
    scene_.AddNode(t);
    terrain_ = t;

}



// Creates the light instance and the object in space associated with the light
Light* Game::CreateLightInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name) {

    Resource* geom = resman_.GetResource(object_name);
    Resource* mat = resman_.GetResource(material_name);
    Resource* tex = NULL;
    if (texture_name != "") {
        tex = resman_.GetResource(texture_name);
    }

    // creates light object and adds it to the scenegraph to be rendered
    Light* light = new Light(800.0f, glm::vec3(1, 0, 0), entity_name, geom, mat, tex);
    scene_.AddNode(light);

    return light;

}




} // namespace game
