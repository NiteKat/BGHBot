#include "BWEB.h"

namespace BWEB
{
	bool Map::overlapsStations(BWAPI::TilePosition here)
	{
		for (auto& station : BWEB::Map::Instance().Stations())
		{
			BWAPI::TilePosition tile = station.BWEMBase()->Location();
			if (here.x >= tile.x && here.x < tile.x + 4 && here.y >= tile.y && here.y < tile.y + 3) return true;
			for (auto& defense : station.DefenseLocations())
				if (here.x >= defense.x && here.x < defense.x + 2 && here.y >= defense.y && here.y < defense.y + 2) return true;
		}
		return false;
	}

	bool Map::overlapsBlocks(BWAPI::TilePosition here)
	{
		for (auto& block : BWEB::Map::Instance().Blocks())
		{
			if (here.x >= block.Location().x && here.x < block.Location().x + block.width() && here.y >= block.Location().y && here.y < block.Location().y + block.height()) return true;
		}
		return false;
	}

	bool Map::overlapsMining(BWAPI::TilePosition here)
	{
		for (auto& station : BWEB::Map::Instance().Stations())
			if (here.getDistance(BWAPI::TilePosition(station.ResourceCentroid())) < 5) return true;
		return false;
	}

	bool Map::overlapsNeutrals(BWAPI::TilePosition here)
	{
		for (auto& m : BWEM::Map::Instance().Minerals())
		{
			BWAPI::TilePosition tile = m->TopLeft();
			if (here.x >= tile.x && here.x < tile.x + 2 && here.y >= tile.y && here.y < tile.y + 1) return true;
		}

		for (auto& g : BWEM::Map::Instance().Geysers())
		{
			BWAPI::TilePosition tile = g->TopLeft();
			if (here.x >= tile.x && here.x < tile.x + 4 && here.y >= tile.y && here.y < tile.y + 2) return true;
		}

		for (auto& n : BWEM::Map::Instance().StaticBuildings())
		{
			BWAPI::TilePosition tile = n->TopLeft();
			if (here.x >= tile.x && here.x < tile.x + n->Type().tileWidth() && here.y >= tile.y && here.y < tile.y + n->Type().tileHeight()) return true;
		}

		for (auto& n : BWAPI::Broodwar->neutral()->getUnits())
		{
			BWAPI::TilePosition tile = n->getInitialTilePosition();
			if (here.x >= tile.x && here.x < tile.x + n->getType().tileWidth() && here.y >= tile.y && here.y < tile.y + n->getType().tileHeight()) return true;
		}
		return false;
	}

	bool Map::overlapsWalls(BWAPI::TilePosition here)
	{
		int x = here.x;
		int y = here.y;

		for (auto& wall : BWEB::Map::Instance().getWalls())
		{
			for (auto tile : wall.smallTiles())			
				if (x >= tile.x && x < tile.x + 2 && y >= tile.y && y < tile.y + 2) return true;
			for (auto tile : wall.mediumTiles())
				if (x >= tile.x && x < tile.x + 3 && y >= tile.y && y < tile.y + 2) return true;
			for (auto tile : wall.largeTiles())
				if (x >= tile.x && x < tile.x + 4 && y >= tile.y && y < tile.y + 3) return true;

			for (auto& defense : wall.getDefenses())			
				if (here.x >= defense.x && here.x < defense.x + 2 && here.y >= defense.y && here.y < defense.y + 2) return true;			
		}
		return false;
	}

	bool Map::overlapsAnything(BWAPI::TilePosition here, int width, int height, bool ignoreBlocks)
	{
		for (int i = here.x; i < here.x + width; i++)
		{
			for (int j = here.y; j < here.y + height; j++)
			{
				if (!BWAPI::TilePosition(i, j).isValid()) continue;
				if (overlapGrid[i][j] > 0) return true;
			}
		}
		return false;
	}

	bool Map::isWalkable(BWAPI::TilePosition here)
	{
		BWAPI::WalkPosition start = BWAPI::WalkPosition(here);
		for (int x = start.x; x < start.x + 4; x++)
		{
			for (int y = start.y; y < start.y + 4; y++)
			{
				if (!BWAPI::WalkPosition(x, y).isValid()) return false;
				if (!BWAPI::Broodwar->isWalkable(BWAPI::WalkPosition(x, y))) return false;
			}
		}
		return true;
	}

	int Map::tilesWithinArea(BWEM::Area const * area, BWAPI::TilePosition here, int width, int height)
	{
		int cnt = 0;
		for (int x = here.x; x < here.x + width; x++)
		{
			for (int y = here.y; y < here.y + height; y++)
			{
				BWAPI::TilePosition t(x, y);
				if (!t.isValid()) return false;
				if (BWEM::Map::Instance().GetArea(t) == area || !BWEM::Map::Instance().GetArea(t))
					cnt++;
			}
		}
		return cnt;
	}
}