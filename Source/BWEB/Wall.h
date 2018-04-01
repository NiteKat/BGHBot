#pragma once
#include "BWEB.h"

namespace BWEB
{	
	//using namespace BWAPI;
	using namespace std;

	class Wall
	{
		BWAPI::TilePosition door;
		BWAPI::Position centroid;
		set<BWAPI::TilePosition> defenses, small, medium, large;
		const BWEM::Area * area;
		const BWEM::ChokePoint * choke;
		
	public:
		Wall(const BWEM::Area *, const BWEM::ChokePoint *);
		void insertDefense(BWAPI::TilePosition here) { defenses.insert(here); }
		void setWallDoor(BWAPI::TilePosition here) { door = here; }
		void insertSegment(BWAPI::TilePosition, BWAPI::UnitType);
		void setCentroid(BWAPI::Position here) { centroid = here; }

		const BWEM::ChokePoint * getChokePoint() const { return choke; }
		const BWEM::Area * getArea() const { return area; }
		
		// Returns the defense locations associated with this Wall
		set<BWAPI::TilePosition> getDefenses() const { return defenses; }

		// Returns the BWAPI::TilePosition belonging to the position where a melee unit should stand to fill the gap of the wall
		BWAPI::TilePosition getDoor() const { return door; }

		// Returns the BWAPI::TilePosition belonging to the centroid of the wall pieces
		BWAPI::Position getCentroid() const { return centroid; }

		// Returns the BWAPI::TilePosition belonging to large UnitType buildings
		set<BWAPI::TilePosition> largeTiles() const { return large; }

		// Returns the BWAPI::TilePosition belonging to medium UnitType buildings
		set<BWAPI::TilePosition> mediumTiles() const { return medium; }

		// Returns the BWAPI::TilePosition belonging to small UnitType buildings
		set<BWAPI::TilePosition> smallTiles() const { return small; }
	};	
}
