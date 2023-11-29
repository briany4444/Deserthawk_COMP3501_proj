#include "terrain.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace game {

    Terrain::Terrain(const std::string name, const Resource* geometry, const Resource* material, const Resource* texture) : SceneNode(name, geometry, material, texture) {
    }


    Terrain::~Terrain() {
    }

    void Terrain::SetUpShader(GLuint program)
    {
        //make the matrix
        glm::vec3 heightTrans = glm::vec3(0, -maxDepth_, 0);
        glm::mat4 trans = glm::translate(glm::mat4(1), heightTrans);
        SceneNode::SetupShader(program);

        GLint depth = glGetUniformLocation(program, "MaxDepth");
        glUniform1f(depth, 5);

        GLint height = glGetUniformLocation(program, "heightTranlation");
        glUniformMatrix4fv(height, 1, GL_FALSE, glm::value_ptr(trans));

    }

} // namespace game
