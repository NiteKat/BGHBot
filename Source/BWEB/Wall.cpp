#include "Wall.h"
#include "AStar.h"
#include <tuple>
#include <chrono>

namespace BWEB
{
	Wall::Wall(const BWEM::Area * a, const BWEM::ChokePoint * c)
	{
		area = a;
		choke = c;
		door = BWAPI::TilePositions::Invalid;
	}

	void Map::createWall(vector<BWAPI::UnitType>& _buildings, const BWEM::Area * _area, const BWEM::ChokePoint * _choke, BWAPI::UnitType _tight, const vector<BWAPI::UnitType>& _defenses, bool _reservePath)
	{
		if (!_area || !_choke || _buildings.empty())
			return;

		// Start a clock to time walls
		chrono::high_resolution_clock clock;
		chrono::steady_clock::time_point start;
		start = chrono::high_resolution_clock::now();

		// I got sick of passing the parameters everywhere, sue me
		buildings = _buildings, area = _area, choke = _choke, tight = _tight, reservePath = _reservePath;

		// Create a new wall object
		Wall newWall(area, choke);

		// Setup pathing parameters
		resetStartEndTiles();
		setStartTile();
		setEndTile();
		
		// Iterate pieces, try to find best location
		if (iteratePieces())
		{
			for (auto& placement : bestWall)
			{
				newWall.insertSegment(placement.first, placement.second);
				addOverlap(placement.first, placement.second.tileWidth(), placement.second.tileHeight());
			}
			currentWall = bestWall;
		}

		findCurrentHole();
		if (true)
		{
			for (auto& tile : currentPath)
			{
				reserveGrid[tile.x][tile.y] = 1;
				if (!BWEM::Map::Instance().GetArea(tile))
					newWall.setWallDoor(tile);
			}
		}

		// Set the Walls centroid
		BWAPI::Position centroid;
		for (auto piece : currentWall)
			centroid += (BWAPI::Position)piece.first + BWAPI::Position(piece.second.tileWidth() * 16, piece.second.tileHeight() * 16);
		newWall.setCentroid(centroid / currentWall.size());

		// Add wall defenses if requested
		if (!_defenses.empty())
			addWallDefenses(_defenses, newWall);

		// Push wall into the vector
		walls.push_back(newWall);

		// Print time
		double dur = chrono::duration <double, milli>(chrono::high_resolution_clock::now() - start).count();
		BWAPI::Broodwar << "Wall time: " << dur << endl;
		return;
	}

	bool Map::iteratePieces()
	{
		// Sort functionality for Pylons by Hannes
		if (find(buildings.begin(), buildings.end(), BWAPI::UnitTypes::Protoss_Pylon) != buildings.end())
		{
			sort(buildings.begin(), buildings.end(), [](BWAPI::UnitType l, BWAPI::UnitType r){ return (l == BWAPI::UnitTypes::Protoss_Pylon) < (r == BWAPI::UnitTypes::Protoss_Pylon); }); // Moves pylons to end
			sort(buildings.begin(), find(buildings.begin(), buildings.end(), BWAPI::UnitTypes::Protoss_Pylon)); // Sorts everything before pylons
		}
		else
			sort(buildings.begin(), buildings.end());

		do {
			currentWall.clear();
			typeIterator = buildings.begin();
			checkPiece((BWAPI::TilePosition)choke->Center());
		} while (next_permutation(buildings.begin(), find(buildings.begin(), buildings.end(), BWAPI::UnitTypes::Protoss_Pylon)));

		return !bestWall.empty();
	}

	bool Map::checkPiece(BWAPI::TilePosition start)
	{
		BWAPI::UnitType parentType = overlapsCurrentWall(start);
		BWAPI::UnitType currentType = (*typeIterator);
		BWAPI::TilePosition parentSize;
		BWAPI::TilePosition currentSize;
		int tightnessFactor = tight == BWAPI::UnitTypes::None ? 32 : min(tight.width(), tight.height());

		if (currentType.isValid())
			currentSize = ((*typeIterator).tileSize());

		// If we have a previous piece, only iterate the pieces around it
		if (parentType != BWAPI::UnitTypes::None && currentType != BWAPI::UnitTypes::Protoss_Pylon)
		{
			parentSize = (parentType.tileSize());

			int parentRight = (parentSize.x * 16) - parentType.dimensionRight() - 1;
			int currentLeft = (currentSize.x * 16) - (*typeIterator).dimensionLeft();
			int parentLeft = (parentSize.x * 16) - parentType.dimensionLeft();
			int currentRight = (currentSize.x * 16) - (*typeIterator).dimensionRight() - 1;

			// Left edge and right edge
			if (parentRight + currentLeft < tightnessFactor || parentLeft + currentRight < tightnessFactor)
			{
				int xLeft = start.x - currentSize.x;
				int xRight = start.x + parentSize.x;
				for (int y = 1 + start.y - currentSize.y; y < start.y + parentSize.y; y++)
				{
					BWAPI::TilePosition left(xLeft, y); BWAPI::TilePosition right(xRight, y);

					if (visited[currentType].location[left.x][left.y] != 2)
					{
						if (identicalPiece(start, parentType, left, currentType) || (parentRight + currentLeft < tightnessFactor && testPiece(left)))
							placePiece(left);
					}
					if (visited[currentType].location[right.x][right.y] != 2)
					{
						if (identicalPiece(start, parentType, right, currentType) || (parentLeft + currentRight < tightnessFactor && testPiece(right)))
							placePiece(right);
					}
				}
			}

			// Top and bottom edge
			int parentTop = (parentSize.y * 16) - parentType.dimensionUp();
			int currentBottom = (currentSize.y * 16) - (*typeIterator).dimensionDown() - 1;
			int parentBottom = (parentSize.y * 16) - parentType.dimensionDown() - 1;
			int currentTop = (currentSize.y * 16) - (*typeIterator).dimensionUp();

			if (parentTop + currentBottom < tightnessFactor || parentBottom + currentTop < tightnessFactor)
			{
				int yTop = start.y - currentSize.y;
				int yBottom = start.y + parentSize.y;
				for (int x = 1 + start.x - currentSize.x; x < start.x + parentSize.x; x++)
				{
					BWAPI::TilePosition top(x, yTop); BWAPI::TilePosition bot(x, yBottom);
					if (visited[currentType].location[top.x][top.y] != 2)
					{
						if (identicalPiece(start, parentType, top, currentType) || (parentTop + currentBottom < tightnessFactor && testPiece(top)))
							placePiece(top);
					}
					if (visited[currentType].location[bot.x][bot.y] != 2)
					{
						if (identicalPiece(start, parentType, bot, currentType) || (parentBottom + currentTop < tightnessFactor && testPiece(bot)))
							placePiece(bot);
					}
				}
			}
		}
		// Otherwise we need to start the choke center
		else
		{
			int width = (choke->Pos(choke->end1).getDistance(choke->Pos(choke->end2)))/4;
			for (int x = start.x - width; x < start.x + width; x++)
			{
				for (int y = start.y - width; y < start.y + width; y++)
				{
					BWAPI::TilePosition t(x, y);
					parentSame = false, currentSame = false;
					if ((isWallTight((*typeIterator), t) || currentType == BWAPI::UnitTypes::Protoss_Pylon) && testPiece(t))
						placePiece(t);
				}
			}
		}
		return true;
	}

	bool Map::identicalPiece(BWAPI::TilePosition parentTile, BWAPI::UnitType parentType, BWAPI::TilePosition currentTile, BWAPI::UnitType currentType)
	{
		// Want to store that it is physically possible to build this piece here so we don't waste time checking
		parentSame = false, currentSame = false;
		if (parentType != BWAPI::UnitTypes::None)
		{
			for (auto& node : visited)
			{
				auto& v = node.second;
				if (node.first == parentType && v.location[parentTile.x][parentTile.y] == 1)
					parentSame = true;
				if (node.first == currentType && v.location[currentTile.x][currentTile.y] == 1)
					currentSame = true;
				if (parentSame && currentSame)
				{
					if (overlapsCurrentWall(currentTile, currentType.tileWidth(), currentType.tileHeight()) == BWAPI::UnitTypes::None)
						return true;
					else
						return false;
				}
			}
		}
		return false;
	}

	bool Map::testPiece(BWAPI::TilePosition t)
	{
		// If this is not a valid type, not a valid tile, overlaps the current wall, overlaps anything, isn't within the area passed in, isn't placeable or isn't wall tight
		if ((*typeIterator) == BWAPI::UnitTypes::Protoss_Pylon && !isPoweringWall(t)) return false;
		if (!(*typeIterator).isValid() || !t.isValid() || overlapsCurrentWall(t, (*typeIterator).tileWidth(), (*typeIterator).tileHeight()) != BWAPI::UnitTypes::None) return false;

		// If we can't place here, regardless of what's currently placed, set as impossible to place
		if (overlapsAnything(t, (*typeIterator).tileWidth(), (*typeIterator).tileHeight(), true) || !isPlaceable(*typeIterator, t) || tilesWithinArea(area, t, (*typeIterator).tileWidth(), (*typeIterator).tileHeight()) == 0)
		{
			visited[(*typeIterator)].location[t.x][t.y] = 2;
			return false;
		}
		return true;
	}

	bool Map::placePiece(BWAPI::TilePosition t)
	{
		// If we haven't tried to place one here, set visited
		if (!currentSame)
			visited[(*typeIterator)].location[t.x][t.y] = 1;

		currentWall[t] = *typeIterator, typeIterator++;

		// If we have placed all pieces
		if (typeIterator == buildings.end())
		{
			if (currentWall.size() == buildings.size())
			{
				findCurrentHole();
				double dist = 1.0;
				for (auto& piece : currentWall)
				{
					if (piece.second == BWAPI::UnitTypes::Protoss_Pylon || piece.second.getRace() == BWAPI::Races::Zerg)
						dist += piece.first.getDistance(naturalTile);
					else
						dist += piece.first.getDistance((BWAPI::TilePosition)choke->Center());
				}

				double score = currentPathSize / dist;
				if (score > bestWallScore && (!reservePath || currentHole != BWAPI::TilePositions::None))
				{
					bestWall = currentWall;
					bestWallScore = score;
				}
			}
		}

		// Else check for another
		else
			checkPiece(t);

		// Erase current tile and reduce iterator
		currentWall.erase(t);
		typeIterator--;
		return true;
	}

	void Map::findCurrentHole()
	{
		if (overlapsCurrentWall(startTile) != BWAPI::UnitTypes::None)
			setStartTile();
		if (overlapsCurrentWall(endTile) != BWAPI::UnitTypes::None)
			setEndTile();

		// Reset hole and get a new path
		currentHole = BWAPI::TilePositions::None;
		currentPath = BWEB::AStar().findPath(startTile, endTile, true);
		currentPathSize = (double)currentPath.size();

		// Quick check to see if the path contains our end point
		if (find(currentPath.begin(), currentPath.end(), endTile) == currentPath.end())
		{
			currentHole = BWAPI::TilePositions::None;
			currentPathSize = DBL_MAX;
			return;
		}

		// Otherwise iterate all tiles and locate the hole
		for (auto& tile : currentPath)
		{
			double closestGeo = DBL_MAX;
			for (auto& geo : choke->Geometry())
			{
				if (overlapsCurrentWall(tile) == BWAPI::UnitTypes::None && BWAPI::TilePosition(geo) == tile && tile.getDistance(startTile) < closestGeo)
					currentHole = tile, closestGeo = tile.getDistance(startTile);
			}
		}
		resetStartEndTiles();
	}

	BWAPI::UnitType Map::overlapsCurrentWall(BWAPI::TilePosition here, int width, int height)
	{
		for (int x = here.x; x < here.x + width; x++)
		{
			for (int y = here.y; y < here.y + height; y++)
			{
				for (auto& placement : currentWall)
				{
					BWAPI::TilePosition tile = placement.first;
					if (x >= tile.x && x < tile.x + placement.second.tileWidth() && y >= tile.y && y < tile.y + placement.second.tileHeight())
						return placement.second;
				}
			}
		}
		return BWAPI::UnitTypes::None;
	}

	void Map::addToWall(BWAPI::UnitType building, Wall& wall, BWAPI::UnitType tight)
	{
		double distance = DBL_MAX;
		BWAPI::TilePosition tileBest = BWAPI::TilePositions::Invalid;
		BWAPI::TilePosition start(wall.getCentroid());

		if (wall.getDoor().isValid())
			start = BWAPI::TilePosition(wall.getDoor());

		for (int x = start.x - 6; x <= start.x + 6; x++)
		{
			for (int y = start.y - 6; y <= start.y + 6; y++)
			{
				BWAPI::TilePosition t(x, y);
				BWAPI::Position p(t);

				if (!BWAPI::TilePosition(x, y).isValid()) continue;
				if (overlapsAnything(BWAPI::TilePosition(x, y), building.tileWidth(), building.tileHeight(), true) || overlapsWalls(t)) continue;
				if (!isPlaceable(building, BWAPI::TilePosition(x, y)) || overlapsCurrentWall(BWAPI::TilePosition(x, y), building.tileWidth(), building.tileHeight()) != BWAPI::UnitTypes::None) continue;

				BWAPI::Position center = BWAPI::Position(t) + BWAPI::Position(32, 32);
				BWAPI::Position hold = BWAPI::Position(start);

				double dist = center.getDistance(hold);

				if (BWEM::Map::Instance().GetArea(BWAPI::TilePosition(center)) != wall.getArea()) continue;
				if (p.getDistance(BWAPI::Position(endTile)) < wall.getCentroid().getDistance(BWAPI::Position(endTile))) continue;

				if (dist < distance)
					tileBest = BWAPI::TilePosition(x, y), distance = dist;
			}
		}

		if (tileBest.isValid())
		{
			currentWall[tileBest] = building;
			wall.insertDefense(tileBest);
			addOverlap(tileBest, building.tileWidth(), building.tileHeight());
		}
	}

	void Map::addWallDefenses(const vector<BWAPI::UnitType>& types, Wall& wall)
	{
		for (auto& building : types)
			addToWall(building, wall, tight);
	}

	bool Map::isPoweringWall(BWAPI::TilePosition here)
	{
		for (auto& piece : currentWall)
		{
			BWAPI::TilePosition tile(piece.first);
			BWAPI::UnitType type(piece.second);
			if (type.tileWidth() == 4)
			{
				bool powersThis = false;
				if (tile.y - here.y == -5 || tile.y - here.y == 4)
				{
					if (tile.x - here.x >= -4 && tile.x - here.x <= 1) powersThis = true;
				}
				if (tile.y - here.y == -4 || tile.y - here.y == 3)
				{
					if (tile.x - here.x >= -7 && tile.x - here.x <= 4) powersThis = true;
				}
				if (tile.y - here.y == -3 || tile.y - here.y == 2)
				{
					if (tile.x - here.x >= -8 && tile.x - here.x <= 5) powersThis = true;
				}
				if (tile.y - here.y >= -2 && tile.y - here.y <= 1)
				{
					if (tile.x - here.x >= -8 && tile.x - here.x <= 6) powersThis = true;
				}
				if (!powersThis) return false;
			}
			else
			{
				bool powersThis = false;
				if (tile.y - here.y == 4)
				{
					if (tile.x - here.x >= -3 && tile.x - here.x <= 2) powersThis = true;
				}
				if (tile.y - here.y == -4 || tile.y - here.y == 3)
				{
					if (tile.x - here.x >= -6 && tile.x - here.x <= 5) powersThis = true;
				}
				if (tile.y - here.y >= -3 && tile.y - here.y <= 2)
				{
					if (tile.x - here.x >= -7 && tile.x - here.x <= 6) powersThis = true;
				}
				if (!powersThis) return false;
			}
		}
		return true;
	}

	void Wall::insertSegment(BWAPI::TilePosition here, BWAPI::UnitType building)
	{
		if (building.tileWidth() >= 4) large.insert(here);
		else if (building.tileWidth() >= 3) medium.insert(here);
		else small.insert(here);
	}

	const Wall * Map::getClosestWall(BWAPI::TilePosition here) const
	{
		double distBest = DBL_MAX;
		const Wall * bestWall = nullptr;
		for (auto& wall : walls)
		{
			double dist = here.getDistance((BWAPI::TilePosition)wall.getChokePoint()->Center());

			if (dist < distBest)
			{
				distBest = dist;
				bestWall = &wall;
			}
		}
		return bestWall;
	}

	Wall* Map::getWall(const BWEM::Area * area, const BWEM::ChokePoint * choke)
	{
		if (!area && !choke) return nullptr;

		for (auto& wall : walls)
		{
			if ((!area || wall.getArea() == area) && (!choke || wall.getChokePoint() == choke))
				return &wall;
		}
		return nullptr;
	}

	bool Map::isWallTight(BWAPI::UnitType building, BWAPI::TilePosition here)
	{
		bool L, R, T, B;
		L = R = T = B = false;
		int height = building.tileHeight() * 4;
		int width = building.tileWidth() * 4;
		int htSize = building.tileHeight() * 16;
		int wtSize = building.tileWidth() * 16;

		if (tight != BWAPI::UnitTypes::None)
		{
			if (htSize - building.dimensionDown() - 1 < 16)
				B = true;
			if (htSize - building.dimensionUp() < 16)
				T = true;
			if (wtSize - building.dimensionLeft() < 16)
				L = true;
			if (wtSize - building.dimensionRight() - 1 < 16)
				R = true;
		}
		else
		{
			L = R = T = B = true;
		}

		BWAPI::WalkPosition right = BWAPI::WalkPosition(here) + BWAPI::WalkPosition(width, 0);
		BWAPI::WalkPosition left = BWAPI::WalkPosition(here) - BWAPI::WalkPosition(1, 0);
		BWAPI::WalkPosition top = BWAPI::WalkPosition(here) - BWAPI::WalkPosition(0, 1);
		BWAPI::WalkPosition bottom = BWAPI::WalkPosition(here) + BWAPI::WalkPosition(0, height);

		for (int y = right.y - 1; y < right.y + height + 1; y++)
		{
			int x = right.x;
			BWAPI::WalkPosition w(x, y);
			BWAPI::TilePosition t(w);
			if (R && (!w.isValid() || !BWAPI::Broodwar->isWalkable(w) || overlapGrid[x][y] > 0)) return true;
		}

		for (int y = left.y - 1; y < left.y + height + 1; y++)
		{
			int x = left.x;
			BWAPI::WalkPosition w(x, y);
			BWAPI::TilePosition t(w);
			if (L && (!w.isValid() || !BWAPI::Broodwar->isWalkable(w) || overlapGrid[x][y] > 0)) return true;
		}

		for (int x = top.x - 1; x < top.x + width + 1; x++)
		{
			int y = top.y;
			BWAPI::WalkPosition w(x, y);
			BWAPI::TilePosition t(w);
			if (T && (!w.isValid() || !BWAPI::Broodwar->isWalkable(w) || overlapGrid[x][y] > 0)) return true;
			
		}

		for (int x = bottom.x - 1; x < bottom.x + width + 1; x++)
		{
			int y = bottom.y;
			BWAPI::WalkPosition w(x, y);
			BWAPI::TilePosition t(w);
			if (B && (!w.isValid() || !BWAPI::Broodwar->isWalkable(w) || overlapGrid[x][y] > 0)) return true;
		}
		return false;
	}

	void Map::setStartTile()
	{
		double distBest = DBL_MAX;
		if (!BWEM::Map::Instance().GetArea(startTile) || !isWalkable(startTile))
		{
			for (int x = startTile.x - 2; x < startTile.x + 2; x++)
			{
				for (int y = startTile.y - 2; y < startTile.y + 2; y++)
				{
					BWAPI::TilePosition t(x, y);
					double dist = t.getDistance(endTile);
					if (overlapsCurrentWall(t) != BWAPI::UnitTypes::None) continue;
					if (BWEM::Map::Instance().GetArea(t) == area && dist < distBest)
						startTile = BWAPI::TilePosition(x, y), distBest = dist;
				}
			}
		}
	}

	void Map::setEndTile()
	{
		double distBest = 0.0;
		if (!BWEM::Map::Instance().GetArea(endTile) || !isWalkable(endTile))
		{
			for (int x = endTile.x - 2; x < endTile.x + 2; x++)
			{
				for (int y = endTile.y - 2; y < endTile.y + 2; y++)
				{
					BWAPI::TilePosition t(x, y);
					double dist = t.getDistance(startTile);
					if (overlapsCurrentWall(t) != BWAPI::UnitTypes::None) continue;
					if (BWEM::Map::Instance().GetArea(t) && dist > distBest)
						endTile = BWAPI::TilePosition(x, y), distBest = dist;
				}
			}
		}
	}

	void Map::resetStartEndTiles()
	{
		const BWEM::Area* thirdArea = (choke->GetAreas().first != area) ? choke->GetAreas().first : choke->GetAreas().second;

		// Finding start and end points of our pathing
		BWAPI::Position mc = (BWAPI::Position)mainChoke->Center();
		BWAPI::Position nc = (BWAPI::Position)naturalChoke->Center();
		if (area == naturalArea)
		{
			double slope1 = double(nc.x - naturalPosition.x) / 32.0;
			double slope2 = double(nc.y - naturalPosition.y) / 32.0;

			startTile = (BWAPI::TilePosition(mainChoke->Center()) + BWAPI::TilePosition(mainChoke->Center()) + BWAPI::TilePosition(mainChoke->Center()) + naturalTile) / 4;
			endTile = BWAPI::TilePosition(nc + BWAPI::Position(32.0 * slope1, 32.0 * slope2));
		}
		else if (area == mainArea)
		{
			startTile = (BWAPI::TilePosition(mainChoke->Center()) + mainTile) / 2;
			endTile = (BWAPI::TilePosition(mainChoke->Center()) + naturalTile) / 2;
		}
		else
		{
			startTile = (BWAPI::TilePosition)area->Top();
			endTile = (BWAPI::TilePosition)thirdArea->Top();
		}
	}
}