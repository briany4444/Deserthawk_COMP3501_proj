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
    glClearColor(viewport_background_color_g[0], viewport_background_color_g[1], viewport_background_color_g[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    resman_.CreateWall("SimpleWall"); //UI and images
    std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/textured_material");
    resman_.LoadResource(Material, "PlainTexMaterial", filename.c_str());
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/loading.png");
    resman_.LoadResource(Texture, "Loading", filename.c_str());
    gui_ = new Ui("LoadingScreen", resman_.GetResource("SimpleWall"), resman_.GetResource("PlainTexMaterial"), resman_.GetResource("Loading"));
    gui_->Draw(&camera_);
    std::cout << "loading..." << std::endl;
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

    resman_.CreateCubeInverted("SkyBox");

    

  
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

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/startScreen.png");
        resman_.LoadResource(Texture, "StartScreen", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/GameOver.png");
        resman_.LoadResource(Texture, "GameOver", filename.c_str());

        filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/SkyBoxCubeMap.png");
        resman_.LoadResource(Texture, "CubeMap", filename.c_str());

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
    
    //player
    SceneNode* playerShape = new SceneNode("PlayerShape", resman_.GetResource("Powerup"), resman_.GetResource("ObjectMaterial"));
    player_.SetShape(playerShape);
    player_.SetPosition(glm::vec3(-370, 40, 420));
    scene_.AddNode(playerShape);
    

    // Create global light source
    {
        l = CreateLightInstance("light", "lightMesh", "RandomTexMaterial", "Texture1");
        l->SetPosition(glm::vec3(0, 200, 500));
        l->SetJointPos(glm::vec3(0, 0, 100));
        //l->SetOrbiting();
        //l->SetOrbitSpeed(1.0);
        l->SetOrbitAxis(glm::vec3(0, 1, 0));
        l->SetScale(glm::vec3(5, 5, 5));
    }

    // create world   
    CreateWorld();

    {
    //sky
    SceneNode* skyBox = new SceneNode("SkyBox", resman_.GetResource("SkyBox"), resman_.GetResource("SkyboxMaterial"), resman_.GetResource("CubeMap"));
    skyBox->Scale(glm::vec3(camera_far_clip_distance_g)); // same dist as far cliping plane from the center of the box
    scene_.skyBox_ = skyBox;
    
    }

    /*Dylans Game Objects*/ if (true) //this line is here so that this large section of code can be collasped
    {
        int num_instances = 5; // For loops       

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



    ////// PARTICLE SYSTEMS ////// 
    {
    



    }

    //game_state_ = inProgress;
    // exit loading screen into start screen
    glClearColor(viewport_background_color_g[0], viewport_background_color_g[1], viewport_background_color_g[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    delete gui_;
    gui_ = new Ui("StartScreen", resman_.GetResource("SimpleWall"), resman_.GetResource("PlainTexMaterial"), resman_.GetResource("StartScreen"));
    gui_->Draw(&camera_);
    std::cout << "start" << std::endl;
    // Push buffer drawn in the background onto the display
    glfwSwapBuffers(window_);
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

                camera_.UpdateLightInfo(l->GetTransf() * glm::vec4(l->GetPosition(), 1.0), l->GetLightCol(), l->GetSpecPwr());
                scene_.Update(delta_time);
                player_.Update(delta_time);
                scene_.skyBox_->SetPosition(player_.GetPosition());

                if (!debugCamera_)
                {
                    camera_.Update(player_.GetOrientation(), player_.GetForward(), player_.GetSide(),
                    player_.GetPosition());
                }
                else
                {
                    DebugCameraMovement();
                }
                
                last_time = current_time;
            }
            HandleCollisions();
        }
        else if (game_state_ == init) {
            //in start screen, spin-wait for space input
           // std::cout << "in init..." << std::endl;

            // Update other events like input handling
            glfwPollEvents();
            continue;
        }
        else if (game_state_ == lost) {
            //print end screen 
            if (glfwGetTime() - last_time > 10) {
                return;
            }
            continue;
        }
        else if (game_state_ == won) { 
            //show win screen
        }


        if (true) { // if player NOT dead
            // Draw to the scene
            scene_.Draw(&camera_);

            scene_.AlphaBlending(true);
            //scene_.Draw(&camera_,SceneGraph::EFFECTS, false);
            gui_->Draw(&camera_);
            scene_.AlphaBlending(false);
        }
        else {
            // Draw the scene to a texture
            scene_.DrawToTexture(&camera_);
            // Process the texture with a screen-space effect and display
            // the texture
            float death_duration = scene_.DisplayTexture(resman_.GetResource("ScreenSpaceMaterial")->GetResource());
            if (death_duration >= 5) {
                game_state_ = lost;
                //update display to game over screen
                delete gui_;
                gui_ = new Ui("LossScreen", resman_.GetResource("SimpleWall"), resman_.GetResource("PlainTexMaterial"), resman_.GetResource("GameOver"));
                gui_->Draw(&camera_);
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
        if (key == GLFW_KEY_C) {
            float x = game->camera_.GetPosition().x;
            float y = game->camera_.GetPosition().y;
            float z = game->camera_.GetPosition().z;
            std::cout << "x:" << x << std::endl;
            std::cout << "y:" << y << std::endl;
            std::cout << "z:" << z << std::endl;
            std::cout << "flower_positions.push_back(glm::vec3(" << x << "," << 0 << "," << z << "));" << std::endl;
        }
    }
    else if (game->game_state_ == init && key == GLFW_KEY_SPACE) {
        game->game_state_ = inProgress;

        //init game gui
        delete game->gui_;
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

    if (terrain_->getDistToGround(player_.GetPosition()) - player_.GetRadius() < 0) {
        game_state_ = lost;
        return;
    }

    std::vector<SceneNode*> collidables = scene_.GetCollidables();
    for (int i = 0; i < collidables.size(); ) {
        SceneNode* curr_node = collidables[i];
        float node_dist = glm::length(curr_node->GetPosition() - player_.GetPosition());
    }
  

}

void Game::createTerrain(const char* file_name, glm::vec3 pos) {

    HeightMap heightMap;
    heightMap.max_height = 90;
    
    std::string f_name = std::string(MATERIAL_DIRECTORY) + std::string(file_name);
    heightMap.hmap = SOIL_load_image(f_name.c_str(), &heightMap.width_, &heightMap.height_, 0, SOIL_LOAD_RGB);



    float terrain_l = 1100;
    float terrain_w = 1100;
    resman_.CreatePlane("terrain", terrain_l, terrain_w, 300, 300, heightMap);

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


    game::SceneNode* bush;
    for (int j = 0; j < bush_positions.size(); ++j)
    {
       bush = CreateInstance("DryShrub" + j, "DryShrubMesh", "TextureNormalMaterial", "DryShrubMeshTexture", "DryShrubMeshNormal");
       bush->SetScale(glm::vec3(8, 8, 8));
       glm::vec3 BushPos = bush_positions[j];
       PlaceObject(bush, BushPos.x, BushPos.y, BushPos.z);
    }
}

SceneNode* Game::makePalmTree(int treeNum, glm::vec3 pos) {
    //Palm Tree
    game::SceneNode* palmTreeTrunk = CreateInstance("PalmTreeTrunk" + treeNum, "PalmTreeTrunkMesh", "TextureNormalMaterial", "PalmTreeTrunkTexture", "PalmTreeNormal");
    palmTreeTrunk->SetScale(glm::vec3(8,8,8));
    game::SceneNode* palmTreeHead = CreateInstance("PalmTreeHead" + treeNum, "PalmTreeHeadMesh", "TextureNormalMaterial", "PalmTreeHeadTexture", "PalmTreeNormal");
    palmTreeHead->SetScale(glm::vec3(8, 8, 8));
    palmTreeHead->SetParent(palmTreeTrunk);

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
}

void Game::createSandNadoZone() {
    // place several sand nados 

}

void Game::generateTerrainFeatures(float x, float z) {
    // generate rocks and bushes around the specified position. 
}

void Game::CreateWorld() {
    createTerrain("/textures/T5.png", glm::vec3(0, -30, 790));
    createObeliskZone();
    createVillage();
    createOasis();
    createSandNadoZone();

}


} // namespace game
