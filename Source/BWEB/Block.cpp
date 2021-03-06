#include "Block.h"
#include <chrono>

namespace BWEB
{
	void Map::findStartBlock()
	{
		bool h, v;
		h = v = false;

		BWAPI::TilePosition tileBest;
		double distBest = DBL_MAX;
		for (int x = mainTile.x - 8; x <= mainTile.x + 5; x++)
		{
			for (int y = mainTile.y - 5; y <= mainTile.y + 4; y++)
			{
				BWAPI::TilePosition tile = BWAPI::TilePosition(x, y);
				if (!tile.isValid()) continue;
				BWAPI::Position blockCenter = BWAPI::Position(tile) + BWAPI::Position(128, 80);
				double dist = blockCenter.getDistance(mainPosition) + blockCenter.getDistance(BWAPI::Position(mainChoke->Center()));
				if (dist < distBest && ((BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss && canAddBlock(tile, 8, 5, true)) || (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran && canAddBlock(tile, 6, 5, true))))
				{
					tileBest = tile;
					distBest = dist;

					if (blockCenter.x < mainPosition.x) h = true;
					if (blockCenter.y < mainPosition.y) v = true;
				}


			}
		}

		if (tileBest.isValid())
			insertStartBlock(tileBest, h, v);
	}

	void Map::findHiddenTechBlock()
	{
		double distBest = 0.0;
		BWAPI::TilePosition best;
		for (int x = mainTile.x - 30; x <= mainTile.x + 30; x++)
		{
			for (int y = mainTile.y - 30; y <= mainTile.y + 30; y++)
			{
				BWAPI::TilePosition tile = BWAPI::TilePosition(x, y);
				if (!tile.isValid() || BWEM::Map::Instance().GetArea(tile) != mainArea) continue;
				BWAPI::Position blockCenter = BWAPI::Position(tile) + BWAPI::Position(80, 64);
				double dist = blockCenter.getDistance(BWAPI::Position(mainChoke->Center()));
				if (dist > distBest && canAddBlock(tile, 5, 4, true))
				{
					best = tile;
					distBest = dist;
				}
			}
		}
		insertTechBlock(best, false, false);
	}

	void Map::findBlocks()
	{
		chrono::steady_clock::time_point start;
		start = chrono::high_resolution_clock::now();
		findStartBlock();
		map<const BWEM::Area *, int> typePerArea;

		// Iterate every tile
		for (int y = 0; y <= BWAPI::Broodwar->mapHeight(); y++)
		{
			for (int x = 0; x <= BWAPI::Broodwar->mapWidth(); x++)
			{
				BWAPI::TilePosition tile(x, y);
				if (!tile.isValid()) continue;

				const BWEM::Area * area = BWEM::Map::Instance().GetArea(tile);
				if (!area) continue;

				// Check if we should mirror our blocks - TODO: Improve the decisions for these
				bool mirrorHorizontal = false, mirrorVertical = false;
				if (BWEM::Map::Instance().Center().x > mainPosition.x) mirrorHorizontal = true;
				if (BWEM::Map::Instance().Center().y > mainPosition.y) mirrorVertical = true;

				if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
				{
					if (canAddBlock(tile, 10, 6, false) && typePerArea[area] < 12)
					{
						typePerArea[area] += 4;
						insertLargeBlock(tile, mirrorHorizontal, mirrorVertical);
						x+=9;
					}
					else if (canAddBlock(tile, 6, 8, false) && typePerArea[area] < 12)
					{
						typePerArea[area] += 2;
						insertMediumBlock(tile, mirrorHorizontal, mirrorVertical);
						x+=5;
					}
					else if (canAddBlock(tile, 4, 5, false))
					{
						typePerArea[area] += 1;
						insertSmallBlock(tile, mirrorHorizontal, mirrorVertical);
						x+=3;
					}
					else if (canAddBlock(tile, 5, 2, false))
					{
						insertTinyBlock(tile, mirrorHorizontal, mirrorVertical);
						x+=4;
					}
				}
				else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
				{
					if (canAddBlock(tile, 8, 6, false))
					{
						x+=7;
						insertLargeBlock(tile, mirrorHorizontal, mirrorVertical);
					}

					else if (canAddBlock(tile, 7, 6, false))
					{
						x+=6;
						insertMediumBlock(tile, mirrorHorizontal, mirrorVertical);
					}

					else if (canAddBlock(tile, 6, 6, false))
					{
						x+=5;
						insertSmallBlock(tile, mirrorHorizontal, mirrorVertical);
					}
					else if (canAddBlock(tile, 6, 4, false))
					{
						x+=5;
						insertTinyBlock(tile, mirrorHorizontal, mirrorVertical);
					}
				}
			}
		}

		double dur = std::chrono::duration <double, std::milli>(std::chrono::high_resolution_clock::now() - start).count();
		BWAPI::Broodwar << "Block time:" << dur << endl;
	}

	bool Map::canAddBlock(BWAPI::TilePosition here, int width, int height, bool lowReq)
	{
		// Check 4 corners before checking the rest
		BWAPI::TilePosition one(here.x, here.y);
		BWAPI::TilePosition two(here.x + width - 1, here.y);
		BWAPI::TilePosition three(here.x, here.y + height - 1);
		BWAPI::TilePosition four(here.x + width - 1, here.y + height - 1);

		if (!one.isValid() || !two.isValid() || !three.isValid() || !four.isValid()) return false;
		if (!BWEM::Map::Instance().GetTile(one).Buildable() || overlapsAnything(one)) return false;
		if (!BWEM::Map::Instance().GetTile(two).Buildable() || overlapsAnything(two)) return false;
		if (!BWEM::Map::Instance().GetTile(three).Buildable() || overlapsAnything(three)) return false;
		if (!BWEM::Map::Instance().GetTile(four).Buildable() || overlapsAnything(four)) return false;

		int offset = lowReq ? 0 : 1;
		// Check if a block of specified size would overlap any bases, resources or other blocks
		for (int x = here.x - offset; x < here.x + width + offset; x++)
		{
			for (int y = here.y - offset; y < here.y + height + offset; y++)
			{
				BWAPI::TilePosition tile(x, y);
				if (tile == one || tile == two || tile == three || tile == four) continue;
				if (!tile.isValid()) return false;
				if (!BWEM::Map::Instance().GetTile(BWAPI::TilePosition(x, y)).Buildable()) return false;
				if (overlapGrid[x][y] > 0) return false;
			}
		}
		return true;
	}

	void Map::insertTinyBlock(BWAPI::TilePosition here, bool mirrorHorizontal, bool mirrorVertical)
	{
		if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
		{
			Block newBlock(5, 2, here);
			addOverlap(here, 5, 2);
			newBlock.insertSmall(here);
			newBlock.insertMedium(here + BWAPI::TilePosition(2, 0));
			blocks.push_back(newBlock);
		}
		else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
		{
			Block newBlock(6, 4, here);
			addOverlap(here, 6, 4);
			newBlock.insertMedium(here);
			newBlock.insertMedium(here + BWAPI::TilePosition(0, 2));
			newBlock.insertMedium(here + BWAPI::TilePosition(3, 0));
			newBlock.insertMedium(here + BWAPI::TilePosition(3, 2));
			blocks.push_back(newBlock);
		}
	}

	void Map::insertSmallBlock(BWAPI::TilePosition here, bool mirrorHorizontal, bool mirrorVertical)
	{
		if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
		{
			Block newBlock(4, 5, here);
			addOverlap(here, 4, 5);
			if (mirrorVertical)
			{
				newBlock.insertSmall(here);
				newBlock.insertSmall(here + BWAPI::TilePosition(2, 0));
				newBlock.insertLarge(here + BWAPI::TilePosition(0, 2));
			}
			else
			{
				newBlock.insertSmall(here + BWAPI::TilePosition(0, 3));
				newBlock.insertSmall(here + BWAPI::TilePosition(2, 3));
				newBlock.insertLarge(here);
			}
			blocks.push_back(newBlock);

		}
		else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
		{
			Block newBlock(6, 6, here);
			addOverlap(here, 6, 6);
			newBlock.insertSmall(here + BWAPI::TilePosition(4, 1));
			newBlock.insertSmall(here + BWAPI::TilePosition(4, 4));
			newBlock.insertLarge(here);
			newBlock.insertLarge(here + BWAPI::TilePosition(0, 3));
			blocks.push_back(newBlock);
		}

	}

	void Map::insertMediumBlock(BWAPI::TilePosition here, bool mirrorHorizontal, bool mirrorVertical)
	{
		if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
		{
			Block newBlock(6, 8, here);
			addOverlap(here, 6, 8);
			if (mirrorHorizontal)
			{
				if (mirrorVertical)
				{
					newBlock.insertSmall(here + BWAPI::TilePosition(0, 2));
					newBlock.insertSmall(here + BWAPI::TilePosition(0, 4));
					newBlock.insertSmall(here + BWAPI::TilePosition(0, 6));
					newBlock.insertMedium(here);
					newBlock.insertMedium(here + BWAPI::TilePosition(3, 0));
					newBlock.insertLarge(here + BWAPI::TilePosition(2, 2));
					newBlock.insertLarge(here + BWAPI::TilePosition(2, 5));
				}
				else
				{
					newBlock.insertSmall(here);
					newBlock.insertSmall(here + BWAPI::TilePosition(0, 2));
					newBlock.insertSmall(here + BWAPI::TilePosition(0, 4));
					newBlock.insertMedium(here + BWAPI::TilePosition(0, 6));
					newBlock.insertMedium(here + BWAPI::TilePosition(3, 6));
					newBlock.insertLarge(here + BWAPI::TilePosition(2, 0));
					newBlock.insertLarge(here + BWAPI::TilePosition(2, 3));
				}
			}
			else
			{
				if (mirrorVertical)
				{
					newBlock.insertSmall(here + BWAPI::TilePosition(4, 2));
					newBlock.insertSmall(here + BWAPI::TilePosition(4, 4));
					newBlock.insertSmall(here + BWAPI::TilePosition(4, 6));
					newBlock.insertMedium(here);
					newBlock.insertMedium(here + BWAPI::TilePosition(3, 0));
					newBlock.insertLarge(here + BWAPI::TilePosition(0, 2));
					newBlock.insertLarge(here + BWAPI::TilePosition(0, 5));
				}
				else
				{
					newBlock.insertSmall(here + BWAPI::TilePosition(4, 0));
					newBlock.insertSmall(here + BWAPI::TilePosition(4, 2));
					newBlock.insertSmall(here + BWAPI::TilePosition(4, 4));
					newBlock.insertMedium(here + BWAPI::TilePosition(0, 6));
					newBlock.insertMedium(here + BWAPI::TilePosition(3, 6));
					newBlock.insertLarge(here);
					newBlock.insertLarge(here + BWAPI::TilePosition(0, 3));
				}
			}
			blocks.push_back(newBlock);
		}
		else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
		{
			Block newBlock(7, 6, here);
			addOverlap(here, 7, 6);
			newBlock.insertMedium(here);
			newBlock.insertMedium(here + BWAPI::TilePosition(0, 2));
			newBlock.insertMedium(here + BWAPI::TilePosition(0, 4));
			newBlock.insertLarge(here + BWAPI::TilePosition(3, 0));
			newBlock.insertLarge(here + BWAPI::TilePosition(3, 3));
			blocks.push_back(newBlock);
		}
	}

	void Map::insertLargeBlock(BWAPI::TilePosition here, bool mirrorHorizontal, bool mirrorVertical)
	{
		if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
		{
			Block newBlock(10, 6, here);
			addOverlap(here, 10, 6);
			newBlock.insertLarge(here);
			newBlock.insertLarge(here + BWAPI::TilePosition(0, 3));
			newBlock.insertLarge(here + BWAPI::TilePosition(6, 0));
			newBlock.insertLarge(here + BWAPI::TilePosition(6, 3));
			newBlock.insertSmall(here + BWAPI::TilePosition(4, 0));
			newBlock.insertSmall(here + BWAPI::TilePosition(4, 2));
			newBlock.insertSmall(here + BWAPI::TilePosition(4, 4));
			blocks.push_back(newBlock);
		}
		else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
		{
			Block newBlock(8, 6, here);
			addOverlap(here, 8, 6);
			newBlock.insertLarge(here);
			newBlock.insertLarge(here + BWAPI::TilePosition(0, 3));
			newBlock.insertLarge(here + BWAPI::TilePosition(4, 0));
			newBlock.insertLarge(here + BWAPI::TilePosition(4, 3));
			blocks.push_back(newBlock);
		}
	}

	void Map::insertStartBlock(BWAPI::TilePosition here, bool mirrorHorizontal, bool mirrorVertical)
	{
		// TODO -- mirroring
		if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
		{
			if (mirrorHorizontal)
			{
				if (mirrorVertical)
				{
					Block newBlock(8, 5, here);
					addOverlap(here, 8, 5);
					newBlock.insertLarge(here);
					newBlock.insertLarge(here + BWAPI::TilePosition(4, 0));
					newBlock.insertSmall(here + BWAPI::TilePosition(6, 3));
					newBlock.insertMedium(here + BWAPI::TilePosition(0, 3));
					newBlock.insertMedium(here + BWAPI::TilePosition(3, 3));
					blocks.push_back(newBlock);
				}
				else
				{
					Block newBlock(8, 5, here);
					addOverlap(here, 8, 5);
					newBlock.insertLarge(here + BWAPI::TilePosition(0, 2));
					newBlock.insertLarge(here + BWAPI::TilePosition(4, 2));
					newBlock.insertSmall(here + BWAPI::TilePosition(6, 0));
					newBlock.insertMedium(here + BWAPI::TilePosition(0, 0));
					newBlock.insertMedium(here + BWAPI::TilePosition(3, 0));
					blocks.push_back(newBlock);
				}
			}
			else
			{
				if (mirrorVertical)
				{
					Block newBlock(8, 5, here);
					addOverlap(here, 8, 5);
					newBlock.insertLarge(here);
					newBlock.insertLarge(here + BWAPI::TilePosition(4, 0));
					newBlock.insertSmall(here + BWAPI::TilePosition(0, 3));
					newBlock.insertMedium(here + BWAPI::TilePosition(2, 3));
					newBlock.insertMedium(here + BWAPI::TilePosition(5, 3));
					blocks.push_back(newBlock);
				}
				else
				{
					Block newBlock(8, 5, here);
					addOverlap(here, 8, 5);
					newBlock.insertLarge(here + BWAPI::TilePosition(0, 2));
					newBlock.insertLarge(here + BWAPI::TilePosition(4, 2));
					newBlock.insertSmall(here + BWAPI::TilePosition(0, 0));
					newBlock.insertMedium(here + BWAPI::TilePosition(2, 0));
					newBlock.insertMedium(here + BWAPI::TilePosition(5, 0));
					blocks.push_back(newBlock);
				}
			}
		}
		else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran)
		{
			Block newBlock(6, 5, here);
			addOverlap(here, 6, 5);
			newBlock.insertLarge(here);
			newBlock.insertSmall(here + BWAPI::TilePosition(4, 1));
			newBlock.insertMedium(here + BWAPI::TilePosition(0, 3));
			newBlock.insertMedium(here + BWAPI::TilePosition(3, 3));
			blocks.push_back(newBlock);
		}
	}

	void Map::insertTechBlock(BWAPI::TilePosition here, bool mirrorHorizontal, bool mirrorVertical)
	{
		if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
		{
			Block newBlock(5, 4, here);
			addOverlap(here, 5, 4);
			newBlock.insertSmall(here);
			newBlock.insertSmall(here + BWAPI::TilePosition(0, 2));
			newBlock.insertMedium(here + BWAPI::TilePosition(2, 0));
			newBlock.insertMedium(here + BWAPI::TilePosition(2, 2));
			blocks.push_back(newBlock);
		}
	}

	void Map::eraseBlock(BWAPI::TilePosition here)
	{
		for (auto it = blocks.begin(); it != blocks.end(); it++)
		{
			auto&  block = *it;
			if (here.x >= block.Location().x && here.x < block.Location().x + block.width() && here.y >= block.Location().y && here.y < block.Location().y + block.height())
			{
				blocks.erase(it);
				// Remove overlap
				return;
			}
		}
	}

	const Block* Map::getClosestBlock(BWAPI::TilePosition here) const
	{
		double distBest = DBL_MAX;
		const Block* bestBlock = nullptr;
		for (auto& block : blocks)
		{
			BWAPI::TilePosition tile = block.Location() + BWAPI::TilePosition(block.width() / 2, block.height() / 2);
			double dist = here.getDistance(tile);

			if (dist < distBest)
			{
				distBest = dist;
				bestBlock = &block;
			}
		}
		return bestBlock;
	}

	Block::Block(int width, int height, BWAPI::TilePosition tile)
	{
		w = width, h = height, t = tile;
	}

}