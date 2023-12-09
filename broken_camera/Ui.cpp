#include "Ui.h"


game::Ui::Ui(const std::string name, const Resource* wallGeometry, const Resource* material, const Resource* texture) : SceneNode(name, wallGeometry, material, texture) {
	num_collected_ = 0;
	//height_ = 600;
	//width_ = 800;
}

game::Ui::~Ui()
{
}

void game::Ui::IncrementCollected(void) {
	++num_collected_;
	if (num_collected_ > NUM_OBJECTIVES) {
		num_collected_ = NUM_OBJECTIVES;
	}
}

void game::Ui::Draw(Camera* camera) {
	
	SceneNode::Draw(camera);


}

void game::Ui::SetupShader(GLuint program) {

	// Set attributes for shaders
	GLint vertex_att = glGetAttribLocation(program, "vertex");
	glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(vertex_att);

	GLint normal_att = glGetAttribLocation(program, "normal");
	glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(normal_att);

	GLint color_att = glGetAttribLocation(program, "color");
	glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(color_att);

	GLint tex_att = glGetAttribLocation(program, "uv");
	glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(9 * sizeof(GLfloat)));
	glEnableVertexAttribArray(tex_att);

	// World transformation
	glm::mat4 scaling = glm::scale(glm::mat4(1.0), scale_);
	glm::mat4 rotation = glm::mat4_cast(orientation_);
	//glm::vec3 pos = glm::vec3( position_.x + parent_->GetPosition().x, position_.y + parent_->GetPosition().y, position_.z + parent_->GetPosition().z);
	glm::mat4 translation = glm::translate(glm::mat4(1.0), position_);
	glm::mat4 transf = translation * rotation * scaling;

	// Set projection matrix in shader
	GLint projection_mat = glGetUniformLocation(program, "projection_mat");
	glUniformMatrix4fv(projection_mat, 1, GL_FALSE, glm::value_ptr(glm::ortho(-0.5f, 0.5f, 0.5f, -0.5f)));
	
	GLint world_mat = glGetUniformLocation(program, "world_mat");
	glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));

	// Timer
	GLint timer_var = glGetUniformLocation(program, "timer");
	double current_time = glfwGetTime();
	glUniform1f(timer_var, (float)current_time);
}
/*
void game::Ui::setOrthographicProjection() {

	// switch to projection mode
	glMatrixMode(GL_PROJECTION);

	// save previous matrix which contains the
	//settings for the perspective projection
	glPushMatrix();

	// reset matrix
	glLoadIdentity();

	// set a 2D orthographic projection
	//glOrtho();
	glOrtho(0.0f, width_, height_, 0.0f, 0.0f, 1.0f);
	//gluOrtho2D(0, 1, 1, 0);

	// switch back to modelview mode
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	/*glPushAttrib(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
}

void game::Ui::restorePerspectiveProjection() {
	/*glPopMatrix();
	glPopAttrib();*

	glMatrixMode(GL_PROJECTION);
	// restore previous projection matrix
	glPopMatrix();

	// get back to modelview mode
	glMatrixMode(GL_MODELVIEW);

	glPopMatrix(); 
}*/