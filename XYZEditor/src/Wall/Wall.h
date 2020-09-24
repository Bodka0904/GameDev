#pragma once

#include <XYZ.h>

enum class WallConnectionType
{
	CORNER_CORNER,
	SIDE_CORNER
};

struct WallEnd
{
	WallEnd()
		:
		Position(0)
	{
		ConnectedWalls[0] = -1;
		ConnectedWalls[1] = -1;
	}
	glm::vec3 Position;
	int ConnectedWalls[2];
};

struct Wall
{
	Wall(int indexInBuffer, float thickness, float height);

	float CalculateLength() const;
	glm::vec3 CalculateDirrection() const;

	WallEnd End[2];
	int IndexInBuffer;
	float Thickness;
	float Height;
};