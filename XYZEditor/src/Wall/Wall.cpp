#include "Wall.h"


Wall::Wall(int indexInBuffer, float thickness, float height)
	:
	IndexInBuffer(indexInBuffer), Thickness(thickness), Height(height)
{}

float Wall::CalculateLength() const
{
	glm::vec3 vecTmp = { End[0].Position.x - End[1].Position.x, End[0].Position.y - End[1].Position.y ,End[0].Position.z - End[1].Position.z };
	return sqrt(((double)vecTmp.x * (double)vecTmp.x) + ((double)vecTmp.y * (double)vecTmp.y) + ((double)vecTmp.z * (double)vecTmp.z));
}

glm::vec3 Wall::CalculateDirrection() const
{
	return { End[0].Position.x - End[1].Position.x, End[0].Position.y - End[1].Position.y ,End[0].Position.z - End[1].Position.z };
}
