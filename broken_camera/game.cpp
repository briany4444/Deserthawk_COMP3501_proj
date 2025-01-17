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
const std::string window_title_g = "Final Project";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

// Viewport and Player settings
float camera_near_clip_distance_g = 0.01;
float camera_far_clip_distance_g = 1000.0;
float camera_fov_g = 60.0; // Field-of-view of Player (degrees)
const glm::vec3 viewport_background_color_g(0, 0.0, 0.0);
glm::vec3 camera_position_g(glm::vec3(-370, 40, 420));
glm::vec3 camera_look_at_g(-51,67,800);
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

    
    // set loading screen
    LoadScreen();
    glClearColor(viewport_background_color_g[0], viewport_background_color_g[1], viewport_background_color_g[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    resman_.CreateWall("SimpleWall"); //UI and images

    std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/SS_textured_material");
    resman_.LoadResource(Material, "PlainTexMaterial", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/loading.png");
    resman_.LoadResource(Texture, "Loading", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/shaders/pond");
    resman_.LoadResource(Material, "PondMat", filename.c_str());

    Ui* a = new Ui("LoadingScreen", resman_.GetResource("SimpleWall"), resman_.GetResource("PlainTexMaterial"), resman_.GetResource("Loading"));
    a->Draw(&camera_);
    
    // Push buffer drawn in the background onto the display
    glfwSwapBuffers(window_);
}


void Game::InitEventHandlers(void){

    // Set event callbacks
    glfwSetKeyCallback(window_, KeyCallback);
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Set pointer to game object, so that callbacks can access it
    glfwSetWindowUserPointer(window_, (void *) this);
}


void Game::SetupResources(void){

    // Setup drawing to texture
    scene_.SetupDrawToTexture();

    // Create a simple object to represent the tree
    {
        resman_.CreateCone("SimpleObject", 2.0, 1.0, 10, 10);
        resman_.CreateCylinder("tree", 20, 2, 100, 100);
        resman_.CreateCylinder("branch", 5.0, 0.5, 100, 100);
        resman_.CreateCone("thorn", 0.5, 0.2, 90, 90);
    }

    resman_.CreateSphere("lightMesh", 0.5, 30, 30);
    resman_.CreateCubeInverted("SkyBox");
    resman_.CreateTorus("Ring", 1);
    resman_.CreateSphere("Orb");
    resman_.CreateSphereParticles("SphereParticles", 250);
    resman_.CreateSphereParticles("SParticle1000", 1000);
    resman_.CreateWall("SimpleWall"); //UI and images


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

        
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/random_textured_material");
        resman_.LoadResource(Material, "RandomTexMaterial", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/normal_map");
        resman_.LoadResource(Material, "TerrainMat", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/screen_space");
        resman_.LoadResource(Material, "ScreenSpaceMaterial", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/shaders/cubemap_material");
        resman_.LoadResource(Material, "SkyboxMaterial", filename.c_str());

        /// Particle Systems ///

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/Sand-nato");
        resman_.LoadResource(Material, "PS-SandTornatoMaterial", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/Fire");
        resman_.LoadResource(Material, "PS-Fire", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/firefly_particle");
        resman_.LoadResource(Material, "PS-FirFlyMaterial", filename.c_str());
       
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

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/T5NormalMap.png");
        resman_.LoadResource(Texture, "Texture2", filename.c_str());
   

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/SkyBoxCubeMap.png");
        resman_.LoadResource(Texture, "CubeMap", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Sun.png");
        resman_.LoadResource(Texture, "RedStar", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Orb_Texture.png");
        resman_.LoadResource(Texture, "OrbTexture", filename.c_str());
    }

 

    /*Dylans Game Objects*/ if (true)
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

        //OtherTree
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/models/Tree_Trunk.obj");
        resman_.LoadResource(Mesh, "TreeTrunkMesh", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Tree_Trunk_Texture.png");
        resman_.LoadResource(Texture, "TreeTrunkTexture", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Tree_Trunk_Normal.png");
        resman_.LoadResource(Texture, "TreeTrunkNormal", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/models/Tree_Branch_1.obj");
        resman_.LoadResource(Mesh, "TreeBranches1Mesh", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/models/Tree_Branch_2.obj");
        resman_.LoadResource(Mesh, "TreeBranches2Mesh", filename.c_str());
        filename = std::string(MATERIAL_DIRECTORY) + std::string("/models/Tree_Branch_3.obj");
        resman_.LoadResource(Mesh, "TreeBranches3Mesh", filename.c_str());

    }
    

}


void Game::SetupScene(void) {

    std::srand(time(NULL));

    // Set background color for the scene
    scene_.SetBackgroundColor(viewport_background_color_g);
    
    //player
    SceneNode* playerShape = new SceneNode("PlayerShape", resman_.GetResource("Orb"), resman_.GetResource("ObjectMaterial"));
    player_.SetShape(playerShape);
    player_.SetPosition(glm::vec3(-370, 40, 420));
    scene_.AddNode(playerShape);
    
    // Create global light source
    {
        l = CreateLightInstance("light", "lightMesh", "TextureNormalMaterial", "RedStar");
        l->SetPosition(glm::vec3(-900, 1200, 1800));
        l->SetJointPos(glm::vec3(0, 0, 100));
        l->SetOrbiting();
        l->SetOrbitSpeed(-.3);
        l->SetOrbitAxis(glm::vec3(.5, 0, .5));
        l->SetScale(glm::vec3(25, 25, 25));
    }

    // create world   
    CreateWorld();
    
    //skybox init
    {
        SceneNode* skyBox = new SceneNode("SkyBox", resman_.GetResource("SkyBox"), resman_.GetResource("SkyboxMaterial"), resman_.GetResource("CubeMap"));
        skyBox->Scale(glm::vec3(camera_far_clip_distance_g*1.1)); // same dist as far cliping plane from the center of the box
        scene_.skyBox_ = skyBox;
    }

    StartScreen();

    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

                // updates game objects
                camera_.UpdateLightInfo(l->GetTransf() * glm::vec4(l->GetPosition(), 1.0), l->GetLightCol(), l->GetSpecPwr());
                scene_.Update(delta_time);
                player_.Update(delta_time);
                scene_.skyBox_->SetPosition(player_.GetPosition());
                
                // updates player
                camera_.Update(player_.GetOrientation(), player_.GetForward(), player_.GetSide(), player_.GetPosition());
                DebugCameraMovement();

                float angle = glm::mod(current_time, 5.0);
                if (angle > 2.5) angle = 0.002;
                else angle = -0.002;

                // update dead tree
                for each (SceneNode * part in deadTreeParts)
                {
                    part->Rotate(glm::angleAxis(angle, glm::vec3(1.0, 0.0, 0.0)));
                }

                // triggers watch tower behvaiour
                watchTowerBehaviour(angle);

                
                last_time = current_time;
            }
            HandleCollisions();
        }
        else if (game_state_ == init) {
            glfwPollEvents();
            continue;
        }
        else if (game_state_ == lost) {
            //print end screen 
            if (glfwGetTime() - last_time > 15) {
                return;
            }
            continue;
        }

        // updates when player has won
        else if (game_state_ == won) { 
            //show win screen
            std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/WinScreen.png");
            resman_.LoadResource(Texture, "Winner", filename.c_str());
            gui_ = new Ui("win", resman_.GetResource("SimpleWall"), resman_.GetResource("PlainTexMaterial"), resman_.GetResource("Winner"));
            gui_->Draw(&camera_);
            glfwSwapBuffers(window_);
            game_state_ = lost;
            continue;
        }

        // handles updates when player is alive
        if (game_state_ != dead) { 
            // Draw to the scene
            scene_.Draw(&camera_);

            // turns off alpha blending for particle systems and UI
            scene_.AlphaBlending(true);
            scene_.Draw(&camera_,SceneGraph::EFFECTS, false); 
            gui_->Draw(&camera_);   
            scene_.AlphaBlending(false);
        }
        else {
            // Draw the scene to a texture
            scene_.DrawToTexture(&camera_);
            
            // Process the texture with a screen-space effect and display
            // the texture
            float death_duration = scene_.DisplayTexture(resman_.GetResource("ScreenSpaceMaterial")->GetResource());
            if (death_duration >= 10) {
                game_state_ = lost;
                
                //update display to game over screen
                std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/GameOver.png");
                resman_.LoadResource(Texture, "GameOver", filename.c_str());
                gui_ = new Ui("LossScreen", resman_.GetResource("SimpleWall"), resman_.GetResource("PlainTexMaterial"), resman_.GetResource("GameOver"));
                gui_->Draw(&camera_);
                glfwSwapBuffers(window_);
                continue;
            }
        }
            
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

    // handles game-in-progress keypresses
    if (game->game_state_ == inProgress) {
        if (key == GLFW_KEY_Y) {
            game->l->Translate(glm::vec3(0.0, 2.0, 0.0));
        }
        if (key == GLFW_KEY_H) {
            game->l->Translate(glm::vec3(0.0, -2.0, 0.0));
        }
        if (key == GLFW_KEY_C) {
            float x = game->camera_.GetPosition().x;
            float y = game->camera_.GetPosition().y;
            float z = game->camera_.GetPosition().z;
            std::cout << "x:" << x << std::endl;
            std::cout << "y:" << y << std::endl;
            std::cout << "z:" << z << std::endl;
            std::cout << "orb_positions.push_back(glm::vec3(" << x << "," << y << "," << z << "));" << std::endl;
        }
    }
    // handles start-up key-strokes
    else if (game->game_state_ == init && key == GLFW_KEY_SPACE) {
        game->game_state_ = inProgress;  
        game->gui_ = new Ui("Hud", game->resman_.GetResource("SimpleWall"), game->resman_.GetResource("GuiMaterial"), game->resman_.GetResource("NoiseTex"));
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


// movement function for debugging
void Game::DebugCameraMovement()
{
    double* mouseX = new double(0);
    double* mouseY = new double(0);

    glfwGetCursorPos(window_, mouseX, mouseY);

    glm::vec2 mousePosition = glm::vec2(*mouseX, *mouseY);

    glm::vec2 mouseSlide = mousePosition - lastFrameMousePosition_;

    lastFrameMousePosition_ = mousePosition;

    player_.Yaw(-mouseSlide.x / 1000.0f);
    player_.Pitch(-mouseSlide.y / 1000.0f); //Honestly 1000 just seemed like a good number to control the turn speed using mouse controls

    // A : Strafe Left
    if (glfwGetKey(window_, GLFW_KEY_A)) {
        //camera_.SetPosition(camera_.GetPosition() + -camera_.GetSide() * debugMoveSpeed_);
        player_.SetPosition(player_.GetPosition() + -player_.GetSide() * debugMoveSpeed_);
    }

    // D : Strafe Right
    if (glfwGetKey(window_, GLFW_KEY_D)) {
        //camera_.SetPosition(camera_.GetPosition() + camera_.GetSide() * debugMoveSpeed_);
        player_.SetPosition(player_.GetPosition() + player_.GetSide() * debugMoveSpeed_);
    }

    // W : Go Foraward
    if (glfwGetKey(window_, GLFW_KEY_W)) {
        //camera_.SetPosition(camera_.GetPosition() + camera_.GetForward() * debugMoveSpeed_);
        player_.SetPosition(player_.GetPosition() + player_.GetForward() * debugMoveSpeed_);
    }

    // S : Go Backward
    if (glfwGetKey(window_, GLFW_KEY_S)) {
        //camera_.SetPosition(camera_.GetPosition() + -camera_.GetForward() * debugMoveSpeed_);
        player_.SetPosition(player_.GetPosition() + -player_.GetForward() * debugMoveSpeed_);

    }

    // Space : Acsend
    if (glfwGetKey(window_, GLFW_KEY_SPACE)) {
        //camera_.SetPosition(camera_.GetPosition() + camera_.GetUp() * debugMoveSpeed_);
        player_.SetPosition(player_.GetPosition() + glm::vec3(0.0,1.0,0.0) * debugMoveSpeed_);
    }

    // Shift : Descend
    if (glfwGetKey(window_, GLFW_KEY_LEFT_SHIFT)) {
        //camera_.SetPosition(camera_.GetPosition() + -camera_.GetUp() * debugMoveSpeed_);
        player_.SetPosition(player_.GetPosition() + -glm::vec3(0.0, 1.0, 0.0) * debugMoveSpeed_);
    }


}


// creates a scene node instance
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

void Game::HandleCollisions() {

    // terrain collisions
    if (terrain_->getDistToGround(player_.GetPosition()) - player_.GetRadius() < 0) {
        game_state_ = dead;
        return;
    }

    // world Object collisions
    std::vector<SceneNode*> collidables = scene_.GetCollidables();
    for (int i = 0; i < collidables.size(); ) {
        SceneNode* curr_node = collidables[i];
        float node_dist = glm::length(curr_node->GetPosition() - player_.GetPosition());
        if (player_.GetRadius() + curr_node->GetRadius() > node_dist) {

            // handles Player - Orb Up Collision
            if (curr_node->GetType() == "Orb") {
                orbs_left_ -= 1;
                gui_->IncrementCollected();
                if (orbs_left_ == 0) {
                    std::cout << "You Have WON!" << std::endl;
                    game_state_ = won;
                }
                std::cout << "You collected an Orb!" << std::endl;
                scene_.RemoveCollidable(curr_node->GetName());
            }
        }
        i++;
    }
}
 



// creates the orb and rings
Orb* Game::createOrbInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name) {

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

    Orb* orb = new Orb(entity_name, geom, mat, tex);
    orb->SetScale(glm::vec3(10, 10, 10));
    scene_.AddNode(orb);
    orbs_left_++;

    orb->AddChild("Ring", resman_.GetResource("Ring"), resman_.GetResource("RandomTexMaterial"), resman_.GetResource("OrbTexture"));
    orb->AddChild("Ring2", resman_.GetResource("Ring"), resman_.GetResource("RandomTexMaterial"), resman_.GetResource("OrbTexture"));
    orb->AddChild("Ring3", resman_.GetResource("Ring"), resman_.GetResource("RandomTexMaterial"), resman_.GetResource("OrbTexture"));

    // scales the orbs
    std::vector<SceneNode*> children = orb->GetChildren();
    for (int i = 0; i < children.size(); ++i) {
        children[i]->SetScale(glm::vec3(10, 10, 10));
    }

    return orb;

}


// generates the terrain model
void Game::createTerrain(const char* file_name, glm::vec3 pos) {

    HeightMap heightMap;
    heightMap.max_height = 90;
    
    // loads the texture
    std::string f_name = std::string(MATERIAL_DIRECTORY) + std::string(file_name);
    heightMap.hmap = SOIL_load_image(f_name.c_str(), &heightMap.width_, &heightMap.height_, 0, SOIL_LOAD_RGB);

    // creates terrain geometry
    float terrain_l = 1100;
    float terrain_w = 1100;
    resman_.CreatePlane("terrain", terrain_l, terrain_w, 300, 300, heightMap);

    // adds to scene
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
    Light* light = new Light( 800.0f, glm::vec3(1, 0, 0), entity_name, geom, mat, tex);
    scene_.AddNode(light);

    return light;

}

// places an object on the terrain given and x and y
void Game::PlaceObject(SceneNode* obj, float x, float offSetY, float z) {
    float y = terrain_->getTerrainY(glm::vec3(x, 0, z));
    y += offSetY;
    obj->SetPosition(glm::vec3(x, y, z));
}

void Game::createObeliskZone() {
    // place obelisk with 4 watch towers around it

    //Obelisk
    game::SceneNode* obelisk = CreateInstance("Obelisk", "ObeliskMesh", "TextureNormalMaterial", "ObeliskTexture", "ObeliskNormal");
    obelisk->SetScale(glm::vec3(30, 30, 30));
    float x = -48.5;
    float z = 800;
    PlaceObject(obelisk, x, 8, z);

    //Watch Towers   
    game::SceneNode* newWatchTower = CreateInstance("WatchTower1", "WatchTowerBaseMesh", "TextureNormalMaterial", "WatchTowerBaseTexture", "WatchTowerBaseNormal");
    newWatchTower->SetScale(glm::vec3(8, 8, 8));
    PlaceObject(newWatchTower, x + 125, 8, z + 125);

    game::SceneNode* watchEye = CreateInstance("WatchEye1", "WatchEyeMesh", "TextureNormalMaterial", "WatchEyeTexture", "WatchEyeNormal");
    watchEye->SetParent(newWatchTower);
    watchEye->SetScale(glm::vec3(12, 12, 12));
    watchEye->Translate(glm::vec3(0, 90, 0));

    newWatchTower = CreateInstance("WatchTower2", "WatchTowerBaseMesh", "TextureNormalMaterial", "WatchTowerBaseTexture", "WatchTowerBaseNormal");
    newWatchTower->SetScale(glm::vec3(8, 8, 8));
    newWatchTower->Rotate(glm::angleAxis(glm::pi<float>(), glm::vec3(0, 1, 0)));
    PlaceObject(newWatchTower, x - 125, 8, z - 125);

    watchEye = CreateInstance("WatchEye2", "WatchEyeMesh", "TextureNormalMaterial", "WatchEyeTexture", "WatchEyeNormal");
    watchEye->SetParent(newWatchTower);
    watchEye->SetScale(glm::vec3(12, 12, 12));
    watchEye->Translate(glm::vec3(0, 90, 0));

    newWatchTower = CreateInstance("WatchTower3", "WatchTowerBaseMesh", "TextureNormalMaterial", "WatchTowerBaseTexture", "WatchTowerBaseNormal");
    newWatchTower->SetScale(glm::vec3(8, 8, 8));
    PlaceObject(newWatchTower, x + 125, 8, z - 125);

    watchEye = CreateInstance("WatchEye3", "WatchEyeMesh", "TextureNormalMaterial", "WatchEyeTexture", "WatchEyeNormal");
    watchEye->SetParent(newWatchTower);
    watchEye->SetScale(glm::vec3(12, 12, 12));
    watchEye->Translate(glm::vec3(0, 90, 0));

    newWatchTower = CreateInstance("WatchTower4", "WatchTowerBaseMesh", "TextureNormalMaterial", "WatchTowerBaseTexture", "WatchTowerBaseNormal");
    newWatchTower->SetScale(glm::vec3(8, 8, 8));
    newWatchTower->Rotate(glm::angleAxis(glm::pi<float>(), glm::vec3(0, 1, 0)));
    PlaceObject(newWatchTower, x - 125, 8, z + 125);

    watchEye = CreateInstance("WatchEye4", "WatchEyeMesh", "TextureNormalMaterial", "WatchEyeTexture", "WatchEyeNormal");
    watchEye->SetParent(newWatchTower);
    watchEye->SetScale(glm::vec3(12, 12, 12));
    watchEye->Translate(glm::vec3(0, 90, 0));    
}


// creates the village area
void Game::createVillage() {
    // pace several houses with some dead bushes and a spiky tree thing
    std::vector<glm::vec3> x_z_positions;
    x_z_positions.push_back(glm::vec3(-388, -0.5, 1153));
    x_z_positions.push_back(glm::vec3(-485, -0.5, 1088));
    x_z_positions.push_back(glm::vec3(-281, -0.5, 1272));
    x_z_positions.push_back(glm::vec3(-454, -0.5, 1272));
    x_z_positions.push_back(glm::vec3(-494, -0.6, 1180));
    x_z_positions.push_back(glm::vec3(-370, -0.6, 1287));
    x_z_positions.push_back(glm::vec3(-292, -0.6, 1149));
    x_z_positions.push_back(glm::vec3(-379, -2.5, 1027));

    //Huts
    game::SceneNode* hut;
    for (int i = 0; i < x_z_positions.size(); ++i) {
        hut = CreateInstance("Hut" + i, "Hut1Mesh", "TextureNormalMaterial", "Hut1Texture", "Hut1Normal");
        hut->SetScale(glm::vec3(8, 8, 8));
        glm::vec3 hutPos = x_z_positions[i];
        PlaceObject(hut, hutPos.x, hutPos.y, hutPos.z);
    }
      
    // spiky tree
    game::SceneNode* newTree = CreateInstance("VillageTree", "TreeMesh", "TextureNormalMaterial", "TreeTexture", "TreeNormal");
    newTree->SetScale(glm::vec3(8, 8, 8));
    glm::vec3 TreePos = glm::vec3(-312, 0, 1075);
    PlaceObject(newTree, TreePos.x, TreePos.y, TreePos.z);
    

    // dead bushes
    std::vector<glm::vec3> bush_positions;
    bush_positions.push_back(glm::vec3(-481, -0.5, 918));
    bush_positions.push_back(glm::vec3(-463, -0.5, 969));
    bush_positions.push_back(glm::vec3(-487, -0.5, 994));
    bush_positions.push_back(glm::vec3(-443, -0.5, 1036));
    bush_positions.push_back(glm::vec3(-425, -0.5, 1088));
    bush_positions.push_back(glm::vec3(-432, -0.5, 1164));
    bush_positions.push_back(glm::vec3(-466, -0.5, 1239));
    bush_positions.push_back(glm::vec3(-432, -0.5, 1164));
    bush_positions.push_back(glm::vec3(-386, -0.5, 1223));
    bush_positions.push_back(glm::vec3(-329, -0.5, 1268));
    bush_positions.push_back(glm::vec3(-198, -0.5, 1251));
    bush_positions.push_back(glm::vec3(-276, -0.5, 1212));

    // instantiates the bushes
    game::SceneNode* bush;
    for (int j = 0; j < bush_positions.size(); ++j)
    {
       bush = CreateInstance("DryShrub" + j, "DryShrubMesh", "TextureNormalMaterial", "DryShrubMeshTexture", "DryShrubMeshNormal");
       bush->SetScale(glm::vec3(8, 8, 8));
       glm::vec3 BushPos = bush_positions[j];
       PlaceObject(bush, BushPos.x, BushPos.y, BushPos.z);
    }
}

// puts components together to create the palm tree
SceneNode* Game::makePalmTree(int treeNum, glm::vec3 pos) {
    //Palm Tree
    game::SceneNode* palmTreeTrunk = CreateInstance("PalmTreeTrunk" + treeNum, "PalmTreeTrunkMesh", "TextureNormalMaterial", "PalmTreeTrunkTexture", "PalmTreeNormal");
    palmTreeTrunk->SetScale(glm::vec3(8,8,8));
    game::SceneNode* palmTreeHead = CreateInstance("PalmTreeHead" + treeNum, "PalmTreeHeadMesh", "TextureNormalMaterial", "PalmTreeHeadTexture", "PalmTreeNormal");
    palmTreeHead->SetScale(glm::vec3(8, 8, 8));
    palmTreeHead->SetParent(palmTreeTrunk);

    // goes through and add leaves to the trees
    for (int i = 0; i < 14; i++)
    {
        std::string name = treeNum + "Leaf" + i;
        game::SceneNode* newLeaf = CreateInstance(name, "PalmTreeLeafMesh", "TextureNormalMaterial", "PalmTreeLeafTexture", "PalmTreeNormal");
        newLeaf->SetScale(glm::vec3(8, 8, 8));
        newLeaf->Translate(glm::vec3(0.0, 48.0, 0.0));
        if (i > 7) newLeaf->Rotate(glm::angleAxis(0.5f + 0.04f * (float)i, glm::vec3(1, 0, 0)));
        newLeaf->Rotate(glm::angleAxis((float)i, glm::vec3(0, 1, 0)));
        newLeaf->SetParent(palmTreeHead);
    }

    return palmTreeTrunk;
}

// creates the hierarchical dead trees
SceneNode* Game::makeDeadTree(int treeNum, glm::vec3 pos)
{
    //SwayingTree
    game::SceneNode* treeTrunk = CreateInstance("TreeTrunk" + treeNum, "TreeTrunkMesh", "TextureNormalMaterial", "TreeTrunkTexture", "TreeTrunkNormal");
    treeTrunk->SetScale(glm::vec3(5, 5, 5));
    PlaceObject(treeTrunk, -180.0f, 8.0f, 750.0f);
    deadTreeParts.push_back(treeTrunk);

    // the branches moving locally to trunk
    game::SceneNode* treeBranch1 = CreateInstance("TreeBranches1" + treeNum, "TreeBranches1Mesh", "TextureNormalMaterial", "TreeTrunkTexture", "TreeTrunkNormal");
    treeBranch1->SetScale(glm::vec3(5, 5, 5));
    treeBranch1->SetParent(treeTrunk);
    treeBranch1->Translate(glm::vec3(0.0f, 50.0f, 0.0f));
    deadTreeParts.push_back(treeBranch1);

    game::SceneNode* treeBranch2 = CreateInstance("TreeBranches2" + treeNum, "TreeBranches2Mesh", "TextureNormalMaterial", "TreeTrunkTexture", "TreeTrunkNormal");
    treeBranch2->SetScale(glm::vec3(5, 5, 5));
    treeBranch2->SetParent(treeBranch1);
    treeBranch2->Translate(glm::vec3(0.0f, 0.0f, 0.0f));
    deadTreeParts.push_back(treeBranch2);

    game::SceneNode* treeBranch3 = CreateInstance("TreeBranches3" + treeNum, "TreeBranches3Mesh", "TextureNormalMaterial", "TreeTrunkTexture", "TreeTrunkNormal");
    treeBranch3->SetScale(glm::vec3(5, 5, 5));
    treeBranch3->SetParent(treeBranch2);
    treeBranch3->Translate(glm::vec3(0.0f, 0.0f, 0.0f));
    deadTreeParts.push_back(treeBranch3);

    return treeTrunk;
}

// function that triggers watch tower behaviour when player gets close
void Game::watchTowerBehaviour(float angle)
{

    glm::vec3 direction = glm::normalize(camera_.GetPosition() - scene_.GetNode("WatchTower1")->GetPosition());
    glm::vec3 rotationAxis = glm::normalize(glm::cross(direction, scene_.GetNode("WatchEye1")->GetForward()));
    float dotP = glm::dot(direction, glm::normalize(scene_.GetNode("WatchTower1")->GetForward()));
    float ang = glm::acos(dotP);

    if ((glm::distance(camera_.GetPosition(), scene_.GetNode("WatchTower1")->GetPosition())) > 200.0f) scene_.GetNode("WatchEye1")->Rotate(glm::angleAxis(angle * 8.0f, glm::vec3(0.0, 1.0, 0.0)));
    else scene_.GetNode("WatchEye1")->SetOrientation(glm::normalize(glm::angleAxis(ang, rotationAxis)));

    direction = glm::normalize(camera_.GetPosition() - scene_.GetNode("WatchTower2")->GetPosition());
    rotationAxis = glm::normalize(glm::cross(direction, scene_.GetNode("WatchEye2")->GetForward()));
    dotP = glm::dot(direction, glm::normalize(scene_.GetNode("WatchTower2")->GetForward()));
    ang = glm::acos(dotP);

    if ((glm::distance(camera_.GetPosition(), scene_.GetNode("WatchTower2")->GetPosition())) > 200.0f) scene_.GetNode("WatchEye2")->Rotate(glm::angleAxis(angle * 8.0f, glm::vec3(0.0, 1.0, 0.0)));
    else scene_.GetNode("WatchEye2")->SetOrientation(glm::normalize(glm::angleAxis(ang, rotationAxis)));

    direction = glm::normalize(camera_.GetPosition() - scene_.GetNode("WatchTower3")->GetPosition());
    rotationAxis = glm::normalize(glm::cross(direction, scene_.GetNode("WatchEye3")->GetForward()));
    dotP = glm::dot(direction, glm::normalize(scene_.GetNode("WatchTower3")->GetForward()));
    ang = glm::acos(dotP);

    if ((glm::distance(camera_.GetPosition(), scene_.GetNode("WatchTower3")->GetPosition())) > 200.0f) scene_.GetNode("WatchEye3")->Rotate(glm::angleAxis(angle * 8.0f, glm::vec3(0.0, 1.0, 0.0)));
    else scene_.GetNode("WatchEye3")->SetOrientation(glm::normalize(glm::angleAxis(ang, rotationAxis)));

    direction = glm::normalize(camera_.GetPosition() - scene_.GetNode("WatchTower4")->GetPosition());
    rotationAxis = glm::normalize(glm::cross(direction, scene_.GetNode("WatchEye4")->GetForward()));
    dotP = glm::dot(direction, glm::normalize(scene_.GetNode("WatchTower4")->GetForward()));
    ang = glm::acos(dotP);

    if ((glm::distance(camera_.GetPosition(), scene_.GetNode("WatchTower4")->GetPosition())) > 200.0f) scene_.GetNode("WatchEye4")->Rotate(glm::angleAxis(angle * 8.0f, glm::vec3(0.0, 1.0, 0.0)));
    else scene_.GetNode("WatchEye4")->SetOrientation(glm::normalize(glm::angleAxis(ang, rotationAxis)));
}


// creates the oasis area
void Game::createOasis() {
    // place pond with palm trees, flowers, and fireflies

    //palm trees
    std::vector<glm::vec3> pTree_positions;
    pTree_positions.push_back(glm::vec3(117, 0, 1288));
    pTree_positions.push_back(glm::vec3(249, 0, 1299));
    pTree_positions.push_back(glm::vec3(326, 0, 1283));
    pTree_positions.push_back(glm::vec3(413, 0, 1246));
    pTree_positions.push_back(glm::vec3(453, 0, 1196));
    pTree_positions.push_back(glm::vec3(480, 0, 1047));
    pTree_positions.push_back(glm::vec3(473, 0, 994));
    pTree_positions.push_back(glm::vec3(433, 0, 962));
    pTree_positions.push_back(glm::vec3(483, 0, 1120));
    pTree_positions.push_back(glm::vec3(172, 0, 1292));

    
    game::SceneNode* pTree;
    for (int i = 0; i < pTree_positions.size(); ++i) {
        pTree = makePalmTree(i, pTree_positions[i]);
        glm::vec3 pTreePos = pTree_positions[i];
        PlaceObject(pTree, pTreePos.x, pTreePos.y, pTreePos.z);
    }

    // flowers
    std::vector<glm::vec3> flower_positions;
    flower_positions.push_back(glm::vec3(446, 0, 1232));
    flower_positions.push_back(glm::vec3(468.885, 0, 1164.15));
    flower_positions.push_back(glm::vec3(479.159, 0, 1095.66));
    flower_positions.push_back(glm::vec3(464.649, 0, 1032.86));    
    flower_positions.push_back(glm::vec3(372.454, 0, 1241.44));   
    flower_positions.push_back(glm::vec3(304.66, 0, 1277.98));
    flower_positions.push_back(glm::vec3(225.744, 0, 1285.89));
    flower_positions.push_back(glm::vec3(419.017, 0, 989.477));

    game::SceneNode* oasisPlant;
    for (int j = 0; j < flower_positions.size(); ++j) {
        oasisPlant = CreateInstance("OasisPlant", "OasisPlantMesh", "TextureNormalMaterial", "OasisPlantTexture", "OasisPlantNormal");
        glm::vec3 FlowerPos = flower_positions[j];
        oasisPlant->SetScale(glm::vec3(18, 18, 18));
        oasisPlant->Rotate(glm::angleAxis(3 * glm::pi<float>() / 4, glm::vec3(0, 1, 0)));
        PlaceObject(oasisPlant, FlowerPos.x, FlowerPos.y, FlowerPos.z);
    }   

    { //pond
        SceneNode* s = CreateInstance("Pond", "SimpleWall", "PondMat", "Texture1");
        s->SetPosition(glm::vec3(290, 4, 1080));
        s->Rotate(glm::quat(1, glm::vec3(1, 0, 0)));
        s->Rotate(glm::quat(.5, glm::vec3(0, 0, 01)));
        s->SetScale(glm::vec3(315));
    }
}


// creates the area where the tornados are
void Game::createSandNadoZone() {

    game::SceneNode* sand = new SceneNode("sandNato", resman_.GetResource("SParticle1000"), resman_.GetResource("PS-SandTornatoMaterial"), resman_.GetResource("SandParticle"));
    sand->SetPosition(glm::vec3(337, 30, 463));
    sand->SetScale(glm::vec3(50));
    scene_.AddNode(sand, SceneGraph::EFFECTS);

}


// creates the fire by the obelisk
void Game::createfires() {
    // place the fire around the obilisk nados 
    game::SceneNode* fire = new SceneNode("Fire1", resman_.GetResource("SParticle1000"), resman_.GetResource("PS-Fire"));
    fire->SetPosition(glm::vec3(-74, 0, 776));
    fire->SetScale(glm::vec3(5));
    scene_.AddNode(fire, SceneGraph::EFFECTS);

    fire = new SceneNode("Fire2", resman_.GetResource("SParticle1000"), resman_.GetResource("PS-Fire"));
    fire->SetPosition(glm::vec3(-74, 0, 830));
    fire->SetScale(glm::vec3(5));
    scene_.AddNode(fire, SceneGraph::EFFECTS);

    fire = new SceneNode("Fire3", resman_.GetResource("SParticle1000"), resman_.GetResource("PS-Fire"));
    fire->SetPosition(glm::vec3(-15, 0, 830));
    fire->SetScale(glm::vec3(5));
    scene_.AddNode(fire, SceneGraph::EFFECTS);

    fire = new SceneNode("Fire4", resman_.GetResource("SParticle1000"), resman_.GetResource("PS-Fire"));
    fire->SetPosition(glm::vec3(-15, 0, 776));
    fire->SetScale(glm::vec3(5));
    scene_.AddNode(fire, SceneGraph::EFFECTS);
}    

// creates the dead tree area 
void Game::createDeadTreeArea()
{
    std::vector<glm::vec3> pTree_positions;
    pTree_positions.push_back(glm::vec3(387, -0.5, 407));
    pTree_positions.push_back(glm::vec3(373, -0.5, 468));
    pTree_positions.push_back(glm::vec3(286, -0.5, 478));
    pTree_positions.push_back(glm::vec3(226, -0.5, 428));
    pTree_positions.push_back(glm::vec3(256, -0.5, 353));
    pTree_positions.push_back(glm::vec3(167, -0.5, 354));
    pTree_positions.push_back(glm::vec3(224, -0.5, 480));
    pTree_positions.push_back(glm::vec3(227, -0.5, 484));
    pTree_positions.push_back(glm::vec3(295, -0.5, 558));
    pTree_positions.push_back(glm::vec3(419, -0.5, 638));

    game::SceneNode* pTree;
    for (int i = 0; i < pTree_positions.size(); ++i) {
        pTree = makeDeadTree(i, pTree_positions[i]);
        glm::vec3 pTreePos = pTree_positions[i];
        PlaceObject(pTree, pTreePos.x, pTreePos.y, pTreePos.z);
    }
}


// the function that procedurally generates tumbleweeds and trees
void Game::generateTerrainFeatures(float x, float z) {
    // generate tumbleweeds and bushes around the specified position. 
    glm::vec3 topLeft(x, 0, z);

    int featureDensity = 7;
    float distBPoints = 20;
    int numFails = 0; // number of attempts it fails in a row

    int gridSide = 10;
    std::vector<std::vector<bool>> grid(gridSide, std::vector<bool>(gridSide, false));

    for (int i = featureDensity; i > 0; --i) {
        int col = rand() % gridSide;
        int row = rand() % gridSide;

        game::SceneNode* bush;
        game::SceneNode* newTumbleweed;
        if (!grid[row][col]) {
            int choice = rand() % 2;
            if (choice) {
                bush = CreateInstance(row + col + "DryShrub" + i, "DryShrubMesh", "TextureNormalMaterial", "DryShrubMeshTexture", "DryShrubMeshNormal");
                bush->SetScale(glm::vec3(8, 8, 8));
                PlaceObject(bush, x - distBPoints * row, -0.5, z + distBPoints * col);
            }
            else {
                newTumbleweed = CreateInstance(col + row + "Tumbleweed" + i, "TumbleweedMesh", "TextureNormalMaterial", "TumbleweedTexture", "TumbleweedNormal");
                newTumbleweed->SetScale(glm::vec3(18, 18, 18));
                PlaceObject(newTumbleweed, x - distBPoints * row, 2, z + distBPoints * col);
            }           

            numFails = 0;
            grid[row][col] = true;
        }
        else {
            i++;
            numFails++;
            if (numFails > 50) {
                break;
            }
        }
    }
}

void Game::LoadScreen()
{
    glClearColor(viewport_background_color_g[0], viewport_background_color_g[1], viewport_background_color_g[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window_);

    glClearColor(viewport_background_color_g[0], viewport_background_color_g[1], viewport_background_color_g[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    resman_.CreateWall("SWall"); //UI and images

    std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/SS_textured_material");
    resman_.LoadResource(Material, "PlainTexMaterial", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/loading.png");
    resman_.LoadResource(Texture, "Loading", filename.c_str());

    Ui* a = new Ui("LoadingScreen", resman_.GetResource("SWall"), resman_.GetResource("PlainTexMaterial"), resman_.GetResource("Loading"));
    a->Draw(&camera_);

    // Push buffer drawn in the background onto the display
    glfwSwapBuffers(window_);
}

// This is the start screen function, which loads the screen
void Game::StartScreen()
{
    glClearColor(viewport_background_color_g[0], viewport_background_color_g[1], viewport_background_color_g[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/start.png");
    resman_.LoadResource(Texture, "StartScreen", filename.c_str());

    Ui* a = new Ui("StartScreen", resman_.GetResource("SimpleWall"), resman_.GetResource("PlainTexMaterial"), resman_.GetResource("StartScreen"));
    a->Draw(&camera_);
    
    // Push buffer drawn in the background onto the display
    glfwSwapBuffers(window_);
}

// High level function that generates all aspects of the world
void Game::CreateWorld() {
    orbs_left_ = 0;
    createTerrain("/textures/T5.png", glm::vec3(0, -30, 790));
    createObeliskZone();
    createVillage();
    createOasis();
    createSandNadoZone();
    createDeadTreeArea();
    createfires();

    // list of points to specify grid points
    // for the procedural generation.
    std::vector<glm::vec3> positions;
    positions.push_back(glm::vec3(-273.46, 0, 463.368));
    positions.push_back(glm::vec3(-96.6231, 0, 486.843));
    positions.push_back(glm::vec3(142.756, 0, 491.982));
    positions.push_back(glm::vec3(300.191, 0, 717.446));
    positions.push_back(glm::vec3(-23.5003, 0, 1081.79));
    positions.push_back(glm::vec3(-287.054, 0, 795.625));

    for (int i = 0; i < positions.size(); ++i) {
        generateTerrainFeatures(positions[i].x, positions[i].z);
    }

    // places orbs
    std::vector<glm::vec3> orb_positions;
    orb_positions.push_back(glm::vec3(68.062, 64.7692, 808.75));
    orb_positions.push_back(glm::vec3(-382.8, 46.6105, 1091.44));
    orb_positions.push_back(glm::vec3(318.562, 71.4704, 1121.69));
    orb_positions.push_back(glm::vec3(341.851, 68.7377, 467.923));

    Orb* orb;
    for (int j = 0; j < orb_positions.size(); ++j) {
        std::stringstream ss;
        ss << j;
        std::string index = ss.str();
        std::string name = "Orb" + index;
        orb = createOrbInstance(name, "Orb", "RandomTexMaterial", "OrbTexture");
        orb->SetPosition(glm::vec3(orb_positions[j].x, orb_positions[j].y, orb_positions[j].z));
    }
}


} // namespace game
