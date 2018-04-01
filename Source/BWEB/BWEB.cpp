#include "BWEB.h"

// TODO:
// Restructure - NEW CRITICAL
// Wall improvements - NEW HIGH
// Performance improvements - NEW MEDIUM
// Block improvements - NEW MEDIUM
// Dynamic block addition (insert BWAPI::UnitTypes, get block) - NEW MEDIUM
// Improve logic for mirroring blocks - NEW LOW
// Code cleanup - NEW LOW
// Limit number of building sizes per area - ONGOING LOW

// Completed Changes:
// Re-add Doors to Walls - COMPLETE
// Improve "Overlap functions", they are probably fairly expensive - COMPLETE
// Changed Terran stations to reserve room for CC addons - COMPLETE
// Reduce number of tiles searched in wall tight search to only buildable tiles - COMPLETE
// Final check on wall tight walls needs to check for tightness against terrain - NFG


namespace BWEB
{
	void Map::onStart()
	{
		findMain();
		findNatural();
		findMainChoke();
		findNaturalChoke();
		findStations();

		for (auto &unit : BWAPI::Broodwar->neutral()->getUnits())
			addOverlap(unit->getTilePosition(), unit->getType().tileWidth(), unit->getType().tileHeight());
	}

	void Map::onUnitDiscover(BWAPI::Unit unit)
	{
		if (!unit || !unit->exists() || !unit->getType().isBuilding() || unit->isFlying()) return;
		if (unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser) return;

		BWAPI::TilePosition tile(unit->getTilePosition());
		BWAPI::UnitType type(unit->getType());

		for (int x = tile.x; x < tile.x + type.tileWidth(); x++)
		{
			for (int y = tile.y; y < tile.y + type.tileHeight(); y++)
			{
				BWAPI::TilePosition t(x, y);
				if (!t.isValid()) continue;
				usedTiles.insert(t);
			}
		}
	}

	void Map::onUnitMorph(BWAPI::Unit unit)
	{
		onUnitDiscover(unit);
	}

	void Map::onUnitDestroy(BWAPI::Unit unit)
	{
		if (!unit || !unit->getType().isBuilding() || unit->isFlying()) return;

		BWAPI::TilePosition tile(unit->getTilePosition());
		BWAPI::UnitType type(unit->getType());

		for (int x = tile.x; x < tile.x + type.tileWidth(); x++)
		{
			for (int y = tile.y; y < tile.y + type.tileHeight(); y++)
			{
				BWAPI::TilePosition t(x, y);
				if (!t.isValid()) continue;
				usedTiles.erase(t);
			}
		}
	}

	void Map::findMain()
	{
		mainTile = BWAPI::Broodwar->self()->getStartLocation();
		mainPosition = (BWAPI::Position)mainTile + BWAPI::Position(64, 48);
		mainArea = BWEM::Map::Instance().GetArea(mainTile);
	}

	void Map::findNatural()
	{
		double distBest = DBL_MAX;
		for (auto& area : BWEM::Map::Instance().Areas())
		{
			for (auto& base : area.Bases())
			{
				if (base.Starting() || base.Geysers().size() == 0 || area.AccessibleNeighbours().size() == 0) continue;
				double dist = getGroundDistance(base.Center(), mainPosition);
				if (dist < distBest)
				{
					distBest = dist;
					naturalArea = base.GetArea();
					naturalTile = base.Location();
					naturalPosition = (BWAPI::Position)naturalTile + BWAPI::Position(64, 48);
				}
			}
		}
	}

	void Map::findMainChoke()
	{
		double distBest = DBL_MAX;
		for (auto& choke : naturalArea->ChokePoints())
		{
			double dist = getGroundDistance(BWAPI::Position(choke->Center()), mainPosition);
			if (choke && dist < distBest)
				mainChoke = choke, distBest = dist;
		}
	}

	void Map::findNaturalChoke()
	{
		// Exception for maps with a natural behind the main such as Crossing Fields
		if (getGroundDistance(mainPosition, BWEM::Map::Instance().Center()) < getGroundDistance(BWAPI::Position(naturalTile), BWEM::Map::Instance().Center()))
		{
			naturalChoke = mainChoke;
			return;
		}

		// Find area that shares the choke we need to defend
		double distBest = DBL_MAX;
		const BWEM::Area* second = nullptr;
		for (auto& area : naturalArea->AccessibleNeighbours())
		{
			BWAPI::WalkPosition center = area->Top();
			double dist = BWAPI::Position(center).getDistance(BWEM::Map::Instance().Center());
			if (center.isValid() && dist < distBest)
				second = area, distBest = dist;
		}

		// Find second choke based on the connected area
		distBest = DBL_MAX;
		for (auto& choke : naturalArea->ChokePoints())
		{
			if (choke->Center() == mainChoke->Center()) continue;
			if (choke->Blocked() || choke->Geometry().size() <= 3) continue;
			if (choke->GetAreas().first != second && choke->GetAreas().second != second) continue;
			double dist = BWAPI::Position(choke->Center()).getDistance(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()));
			if (dist < distBest)
				naturalChoke = choke, distBest = dist;
		}
	}

	void Map::draw()
	{
		for (auto& block : blocks)
		{
			for (auto& tile : block.SmallTiles())
				BWAPI::Broodwar->drawBoxMap(BWAPI::Position(tile), BWAPI::Position(tile) + BWAPI::Position(65, 65), BWAPI::Broodwar->self()->getColor());
			for (auto& tile : block.MediumTiles())
				BWAPI::Broodwar->drawBoxMap(BWAPI::Position(tile), BWAPI::Position(tile) + BWAPI::Position(97, 65), BWAPI::Broodwar->self()->getColor());
			for (auto& tile : block.LargeTiles())
				BWAPI::Broodwar->drawBoxMap(BWAPI::Position(tile), BWAPI::Position(tile) + BWAPI::Position(129, 97), BWAPI::Broodwar->self()->getColor());
		}

		for (auto& station : stations)
		{
			for (auto& tile : station.DefenseLocations())
				BWAPI::Broodwar->drawBoxMap(BWAPI::Position(tile), BWAPI::Position(tile) + BWAPI::Position(65, 65), BWAPI::Broodwar->self()->getColor());
			BWAPI::Broodwar->drawBoxMap(BWAPI::Position(station.BWEMBase()->Location()), BWAPI::Position(station.BWEMBase()->Location()) + BWAPI::Position(129, 97), BWAPI::Broodwar->self()->getColor());
		}

		for (auto& wall : walls)
		{			
			for (auto& tile : wall.smallTiles())
				BWAPI::Broodwar->drawBoxMap(BWAPI::Position(tile), BWAPI::Position(tile) + BWAPI::Position(65, 65), BWAPI::Broodwar->self()->getColor());
			for (auto& tile : wall.mediumTiles())
				BWAPI::Broodwar->drawBoxMap(BWAPI::Position(tile), BWAPI::Position(tile) + BWAPI::Position(97, 65), BWAPI::Broodwar->self()->getColor());
			for (auto& tile : wall.largeTiles())
				BWAPI::Broodwar->drawBoxMap(BWAPI::Position(tile), BWAPI::Position(tile) + BWAPI::Position(129, 97), BWAPI::Broodwar->self()->getColor());
			for (auto& tile : wall.getDefenses())
				BWAPI::Broodwar->drawBoxMap(BWAPI::Position(tile), BWAPI::Position(tile) + BWAPI::Position(65, 65), BWAPI::Broodwar->self()->getColor());
			BWAPI::Broodwar->drawBoxMap(BWAPI::Position(wall.getDoor()), BWAPI::Position(wall.getDoor()) + BWAPI::Position(33, 33), BWAPI::Broodwar->self()->getColor(), true);
		}

		//for (int x = 0; x < BWAPI::Broodwar->mapWidth()*4; x++)
		//{
		//	for (int y = 0; y < BWAPI::Broodwar->mapHeight()*4; y++)
		//	{
		//		WalkPosition t(x, y);
		//		if (!BWAPI::Broodwar->isWalkable(t))
		//			BWAPI::Broodwar->drawBoxMap(Position(t), Position(t) + Position(9, 9), BWAPI::Broodwar->self()->getColor(), true);
		//	}
		//}

		//BWAPI::Broodwar->drawCircleMap(Position(startTile), 8, Colors::Green, true);
		//BWAPI::Broodwar->drawCircleMap(Position(endTile), 8, Colors::Orange, true);
		//BWAPI::Broodwar->drawCircleMap(naturalPosition, 8, Colors::Red, true);
		////BWAPI::Broodwar->drawCircleMap(Position(mainChoke->Center()), 8, Colors::Green, true);
		//BWAPI::Broodwar->drawCircleMap(Position(naturalChoke->Center()), 8, Colors::Yellow, true);
	}

	template <class PositionType>
	double Map::getGroundDistance(PositionType start, PositionType end)
	{
		double dist = 0.0;
		if (!start.isValid() || !end.isValid() || !BWEM::Map::Instance().GetArea(BWAPI::WalkPosition(start)) || !BWEM::Map::Instance().GetArea(BWAPI::WalkPosition(end)))
			return DBL_MAX;

		for (auto& cpp : BWEM::Map::Instance().GetPath(start, end))
		{
			auto center = BWAPI::Position{ cpp->Center() };
			dist += start.getDistance(center);
			start = center;
		}

		return dist += start.getDistance(end);
	}

	BWAPI::TilePosition Map::getBuildPosition(BWAPI::UnitType type, BWAPI::TilePosition searchCenter)
	{
		double distBest = DBL_MAX;
		BWAPI::TilePosition tileBest = BWAPI::TilePositions::Invalid;

		// Search through each block to find the closest block and valid position
		for (auto& block : blocks)
		{
			set<BWAPI::TilePosition> placements;
			if (type.tileWidth() == 4) placements = block.LargeTiles();
			else if (type.tileWidth() == 3) placements = block.MediumTiles();
			else placements = block.SmallTiles();

			for (auto& tile : placements)
			{
				double distToPos = tile.getDistance(searchCenter);
				if (distToPos < distBest && isPlaceable(type, tile))
					distBest = distToPos, tileBest = tile;
			}
		}
		return tileBest;
	}

	bool Map::isPlaceable(BWAPI::UnitType type, BWAPI::TilePosition location)
	{
		// Placeable is valid if buildable and not overlapping neutrals
		// Note: Must check neutrals due to the terrain below them technically being buildable
		int creepCheck = type.requiresCreep() ? 1 : 0;
		for (int x = location.x; x < location.x + type.tileWidth(); x++)
		{
			for (int y = location.y; y < location.y + type.tileHeight() + creepCheck; y++)
			{
				BWAPI::TilePosition tile(x, y);
				if (!tile.isValid() || !BWAPI::Broodwar->isBuildable(tile)) return false;
				if (reserveGrid[x][y] > 0 || overlapGrid[x][y] > 0) return false;
				if (usedTiles.find(tile) != usedTiles.end()) return false;
				if (type.isResourceDepot() && !BWAPI::Broodwar->canBuildHere(tile, type)) return false;				
			}
		}
		return true;
	}

	void Map::addOverlap(BWAPI::TilePosition t, int w, int h)
	{
		for (int x = t.x; x < t.x + w; x++)
		{
			for (int y = t.y; y < t.y + h; y++)
			{
				overlapGrid[x][y] = 1;
			}
		}
	}

	Map* Map::BWEBInstance = nullptr;

	Map & Map::Instance()
	{
		if (!BWEBInstance) BWEBInstance = new Map();
		return *BWEBInstance;
	}
}