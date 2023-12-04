#ifndef UI_H_
#define UI_H_

#define NUM_OBJECTIVES 4

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <GLh>

#include "scene_node.h"

namespace game {

    // Class that manages one object in a scene 
    class Ui : public SceneNode{

    public:
        
        Ui(const std::string name, const Resource* wallGeometry, const Resource* material, Camera* camera);
        ~Ui();

        //update the number of collected objectives
        void IncrementCollected(void);


        void Draw(Camera* camera);

    protected:
        //void SetupShader(GLuint program);
        

    private:
        int num_collected_;
        //Camera* parent_;
        void setOrthographicProjection();
        void restorePerspectiveProjection();
    

    }; // class Ui

} // namespace game

#endif // UI_H_
