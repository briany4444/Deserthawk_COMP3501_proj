#include "Ui.h"


game::Ui::Ui(const std::string name, const Resource* wallGeometry, const Resource* material) : SceneNode(name, wallGeometry, material) {
	num_collected_ = 0;
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
