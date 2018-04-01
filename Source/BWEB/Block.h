#pragma once
#include "BWEB.h"

namespace BWEB
{
#undef small

	//using namespace BWAPI;
	using namespace std;

	class Block
	{
		int w, h;
		BWAPI::TilePosition t;
		set<BWAPI::TilePosition> small, medium, large;

	public:
		Block() { };
		Block(int, int, BWAPI::TilePosition);
		int width() const { return w; }
		int height() const { return h; }

		// Returns the top left tile position of this block
		BWAPI::TilePosition Location() const { return t; }

		// Returns the const set of BWAPI::TilePositions that belong to 2x2 (small) buildings
		set<BWAPI::TilePosition> SmallTiles() const { return small; }

		// Returns the const set of BWAPI::TilePositions that belong to 3x2 (medium) buildings
		set<BWAPI::TilePosition> MediumTiles() const { return medium; }

		// Returns the const set of BWAPI::TilePositions that belong to 4x3 (large) buildings
		set<BWAPI::TilePosition> LargeTiles() const { return large; }
		
		void insertSmall(BWAPI::TilePosition here) { small.insert(here); }
		void insertMedium(BWAPI::TilePosition here) { medium.insert(here); }
		void insertLarge(BWAPI::TilePosition here) { large.insert(here); }		
	};
}
