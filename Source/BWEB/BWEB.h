#pragma once
#pragma warning(disable : 4351)
#include <set>

#include <BWAPI.h>
#include "..\BWEM 1.4.1\src\bwem.h"
#include "Station.h"
#include "Block.h"
#include "Wall.h"

namespace BWEB
{
	//using namespace BWAPI;
	using namespace std;

	class Block;
	class Wall;
	class Station;
	class Map
	{
	private:

		vector<Station> stations;
		vector<Wall> walls;
		vector<Block> blocks;
		
		// Blocks
		void findStartBlock(), findHiddenTechBlock();
		bool canAddBlock(BWAPI::TilePosition, int, int, bool);
		void insertTinyBlock(BWAPI::TilePosition, bool, bool);
		void insertSmallBlock(BWAPI::TilePosition, bool, bool);
		void insertMediumBlock(BWAPI::TilePosition, bool, bool);
		void insertLargeBlock(BWAPI::TilePosition, bool, bool);		
		void insertStartBlock(BWAPI::TilePosition, bool, bool);
		void insertTechBlock(BWAPI::TilePosition, bool, bool);

		// Walls
		bool isWallTight(BWAPI::UnitType, BWAPI::TilePosition);	
		bool isPoweringWall(BWAPI::TilePosition);				
		bool iteratePieces();
		bool checkPiece(BWAPI::TilePosition);
		bool testPiece(BWAPI::TilePosition);
		bool placePiece(BWAPI::TilePosition);
		bool identicalPiece(BWAPI::TilePosition, BWAPI::UnitType, BWAPI::TilePosition, BWAPI::UnitType);
		void findCurrentHole();
		void addWallDefenses(const vector<BWAPI::UnitType>& type, Wall& wall);			
		int reserveGrid[256][256] ={};		

		double bestWallScore = 0.0, closest = DBL_MAX;
		BWAPI::TilePosition currentHole, startTile, endTile;
		vector<BWAPI::TilePosition> currentPath;		
		vector<BWAPI::UnitType>::iterator typeIterator;		
		map<BWAPI::TilePosition, BWAPI::UnitType> bestWall;
		map<BWAPI::TilePosition, BWAPI::UnitType> currentWall;

		void setStartTile(), setEndTile(), resetStartEndTiles();

		// Information that is passed in
		vector<BWAPI::UnitType> buildings;
		const BWEM::ChokePoint * choke;
		const BWEM::Area * area;
		BWAPI::UnitType tight;
		bool reservePath;

		// BWAPI::TilePosition grid of what has been visited for wall placement
		struct VisitGrid
		{
			int location[256][256] ={};
		};
		map<BWAPI::UnitType, VisitGrid> visited;
		bool parentSame, currentSame;
		double currentPathSize;		
		
		// Map
		void findMain(), findMainChoke(), findNatural(), findNaturalChoke();
		BWAPI::Position mainPosition, naturalPosition;
		BWAPI::TilePosition mainTile, naturalTile;		
		const BWEM::Area * naturalArea;
		const BWEM::Area * mainArea;
		const BWEM::ChokePoint * naturalChoke;
		const BWEM::ChokePoint * mainChoke;
		set<BWAPI::TilePosition> usedTiles;
		void addOverlap(BWAPI::TilePosition, int, int);
		bool isPlaceable(BWAPI::UnitType, BWAPI::TilePosition);

		// Stations
		void findStations();
		set<BWAPI::TilePosition>& stationDefenses(BWAPI::TilePosition, bool, bool);
		set<BWAPI::TilePosition> returnValues;

		// General
		static Map* BWEBInstance;

	public:
		void draw(), onStart(), onUnitDiscover(BWAPI::Unit), onUnitDestroy(BWAPI::Unit), onUnitMorph(BWAPI::Unit);
		static Map &Instance();
		int overlapGrid[256][256] ={};
		int testGrid[256][256] ={};

		/// This is just put here so AStar can use it for now
		BWAPI::UnitType overlapsCurrentWall(BWAPI::TilePosition tile, int width = 1, int height = 1);

		bool overlapsBlocks(BWAPI::TilePosition);
		bool overlapsStations(BWAPI::TilePosition);
		bool overlapsNeutrals(BWAPI::TilePosition);
		bool overlapsMining(BWAPI::TilePosition);
		bool overlapsWalls(BWAPI::TilePosition);
		bool overlapsAnything(BWAPI::TilePosition here, int width = 1, int height = 1, bool ignoreBlocks = false);
		bool isWalkable(BWAPI::TilePosition);
		int tilesWithinArea(BWEM::Area const *, BWAPI::TilePosition here, int width = 1, int height = 1);

		/// <summary> Returns the closest buildable BWAPI::TilePosition for any type of structure </summary>
		/// <param name="type"> The BWAPI::UnitType of the structure you want to build. </summary>
		/// <param name="tile"> The BWAPI::TilePosition you want to build closest to. </summary>
		BWAPI::TilePosition getBuildPosition(BWAPI::UnitType type, BWAPI::TilePosition tile = BWAPI::Broodwar->self()->getStartLocation());

		/// <summary> Returns the closest buildable BWAPI::TilePosition for a defensive structure </summary>
		/// <param name="type"> The BWAPI::UnitType of the structure you want to build. </summary>
		/// <param name="tile"> The BWAPI::TilePosition you want to build closest to. </summary>
		BWAPI::TilePosition getDefBuildPosition(BWAPI::UnitType type, BWAPI::TilePosition tile = BWAPI::Broodwar->self()->getStartLocation());

		template <class PositionType>
		/// <summary> Returns the estimated ground distance from one Position type to another Position type.</summary>
		/// <param name="first"> The first Position. </param>
		/// <param name="second"> The second Position. </param>
		double getGroundDistance(PositionType first, PositionType second);

		/// <summary> <para> Returns a pointer to a BWEB::Wall if it has been created in the given BWEM::Area and BWEM::ChokePoint. </para>
		/// <para> Note: If you only pass a BWEM::Area or a BWEM::ChokePoint (not both), it will imply and pick a BWEB::Wall that exists within that Area or blocks that BWEM::ChokePoint. </para></summary>
		/// <param name="area"> The BWEM::Area that the BWEB::Wall resides in </param>
		/// <param name="choke"> The BWEM::Chokepoint that the BWEB::Wall blocks </param>
		Wall* getWall(BWEM::Area const* area = nullptr, BWEM::ChokePoint const* choke = nullptr);

		// TODO: Add this
		Station* getStation(BWEM::Area const* area);

		/// <summary> Returns the BWEM::Area of the natural expansion </summary>
		const BWEM::Area * getNaturalArea() { return naturalArea; }

		/// <summary> Returns the BWEM::Area of the main </summary>
		const BWEM::Area * getMainArea() { return mainArea; }

		/// <summary> Returns the BWEM::Chokepoint of the natural </summary>
		const BWEM::ChokePoint * getNaturalChoke() { return naturalChoke; }

		/// <summary> Returns the BWEM::Chokepoint of the main </summary>
		const BWEM::ChokePoint * getMainChoke() { return mainChoke; }

		/// <summary> Returns a vector containing every BWEB::Wall. </summary>
		vector<Wall> getWalls() const { return walls; }

		/// <summary> Returns a vector containing every BWEB::Block </summary>
		vector<Block> Blocks() const { return blocks; }

		/// <summary> Returns a vector containing every BWEB::Station </summary>
		vector<Station> Stations() const { return stations; }

		/// <summary> Returns the closest BWEB::Station to the given BWAPI::TilePosition. </summary>
		const Station* getClosestStation(BWAPI::TilePosition) const;

		/// <summary> Returns the closest BWEB::Wall to the given BWAPI::TilePosition. </summary>
		const Wall* getClosestWall(BWAPI::TilePosition) const;

		/// <summary> Returns the closest BWEB::Block to the given BWAPI::TilePosition. </summary>
		const Block* getClosestBlock(BWAPI::TilePosition) const;

		// Returns the BWAPI::TilePosition of the natural expansion
		BWAPI::TilePosition getNatural() { return naturalTile; }

		// Returns the set of used BWAPI::TilePositions
		set<BWAPI::TilePosition>& getUsedTiles() { return usedTiles; }

		/// <summary> <para> Given a vector of BWAPI::UnitTypes, an Area and a Chokepoint, finds an optimal wall placement, returns true if a valid BWEB::Wall was created. </para>
		/// <para> Note: Highly recommend that only Terran walls attempt to be walled tight, as most Protoss and Zerg wallins have gaps to allow your units through.</para>
		/// <para> BWEB makes tight walls very differently from non tight walls and will only create a tight wall if it is completely tight. </para></summary>
		/// <param name="buildings"> A Vector of BWAPI::UnitTypes that you want the BWEB::Wall to consist of. </param>
		/// <param name="area"> The BWEM::Area that you want the BWEB::Wall to be contained within. </param>
		/// <param name="choke"> The BWEM::Chokepoint that you want the BWEB::Wall to block. </param>
		/// <param name="tight"> (Optional) Decides whether this BWEB::Wall intends to be walled around a specific BWAPI::UnitType. </param>
		/// <param name="defenses"> A Vector of BWAPI::UnitTypes that you want the BWEB::Wall to have defenses consisting of. </param>
		/// <param name="reservePath"> Optional parameter to ensure that a path of BWAPI::TilePositions is reserved and not built on. </param>
		void createWall(vector<BWAPI::UnitType>& buildings, const BWEM::Area * area, const BWEM::ChokePoint * choke, BWAPI::UnitType tight = BWAPI::UnitTypes::None, const vector<BWAPI::UnitType>& defenses ={}, bool reservePath = false);

		/// <summary> Adds a BWAPI::UnitType to a currently existing BWEB::Wall. </summary>
		/// <param name="type"> The BWAPI::UnitType you want to place at the BWEB::Wall. </param>
		/// <param name="area"> The BWEB::Wall you want to add to. </param>
		/// <param name="tight"> (Optional) Decides whether this addition to the BWEB::Wall intends to be walled around a specific BWAPI::UnitType. Defaults to none. </param>
		void addToWall(BWAPI::UnitType type, Wall& wall, BWAPI::UnitType tight = BWAPI::UnitTypes::None);

		/// <summary> Erases any blocks at the specified BWAPI::TilePosition. </summary>
		/// <param name="here"> The BWAPI::TilePosition that you want to delete any BWEB::Block that exists here. </summary>
		void eraseBlock(BWAPI::TilePosition here);

		/// <summary> Initializes the building of every BWEB::Block on the map, call it only once per game. </summary>
		void findBlocks();
	};
}