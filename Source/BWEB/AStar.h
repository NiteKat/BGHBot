#pragma once
#include "BWEB.h"

namespace BWEB
{
	using uint = unsigned int;

	//using namespace BWAPI;
	using namespace std;

	struct Node
	{
		uint G, H;
		BWAPI::TilePosition coordinates, direction;
		Node *parent;

		Node(BWAPI::TilePosition tile, Node *parent = nullptr);
		uint getScore();
	};

	class AStar
	{
		Node* findNodeOnList(set<Node*>&, BWAPI::TilePosition);
		vector<BWAPI::TilePosition> direction, walls;
		uint directions;
		uint manhattan(BWAPI::TilePosition, BWAPI::TilePosition);

	public:
		AStar();
		vector<BWAPI::TilePosition> findPath(BWAPI::TilePosition, BWAPI::TilePosition, bool);
	};
}
