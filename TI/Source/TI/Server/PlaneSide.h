#pragma once

struct OrientationInfo
{
	unsigned int sideAxis = 0;
	unsigned int heightAxis = 1;
	unsigned int frontAxis = 2;
	bool positive = true;

	bool operator==(const OrientationInfo& other) const
	{
		return sideAxis == other.sideAxis &&
			heightAxis == other.heightAxis &&
			frontAxis == other.frontAxis &&
			positive == other.positive;
	}

	bool operator!=(const OrientationInfo& other) const
	{
		return !(*this == other);
	}
};

namespace Orientations
{
	static const OrientationInfo TOP	{ 0, 1, 2, true };
	static const OrientationInfo RIGHT	{ 1, 0, 2, true };
	static const OrientationInfo FRONT	{ 1, 2, 0, true };
	static const OrientationInfo BOTTOM { 0, 1, 2, false };
	static const OrientationInfo LEFT	{ 1, 0, 2, false };
	static const OrientationInfo BACK	{ 1, 2, 0, false };
}
