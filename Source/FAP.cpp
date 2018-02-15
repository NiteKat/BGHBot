#include "FAP.h"

#define MAX(a, b) (((a) < (b) ? (b) : (a)))

Neolib::FastAPproximation fap;

namespace Neolib {

	FastAPproximation::FastAPproximation() {}

	void FastAPproximation::addUnitPlayer1(FAPUnit fu) { player1.push_back(fu); }

	void FastAPproximation::addIfCombatUnitPlayer1(FAPUnit fu) {
		if (fu.unitType == BWAPI::UnitTypes::Protoss_Interceptor)
			return;
		if (fu.groundDamage || fu.airDamage ||
			fu.unitType == BWAPI::UnitTypes::Terran_Medic)
			addUnitPlayer1(fu);
	}

	void FastAPproximation::addUnitPlayer2(FAPUnit fu) { player2.push_back(fu); }

	void FastAPproximation::addIfCombatUnitPlayer2(FAPUnit fu) {
		if (fu.groundDamage || fu.airDamage ||
			fu.unitType == BWAPI::UnitTypes::Terran_Medic)
			addUnitPlayer2(fu);
	}

	void FastAPproximation::simulate(int nFrames) {
		while (nFrames--) {
			if (!player1.size() || !player2.size())
				break;

			didSomething = false;

			isimulate();

			if (!didSomething)
				break;
		}
	}

	const auto score = [](const FAPUnit &fu) {
		if (fu.health && fu.maxHealth)
			return ((fu.score * fu.health) / (fu.maxHealth * 2)) +
			(fu.unitType == BWAPI::UnitTypes::Terran_Bunker) *
			BWAPI::UnitTypes::Terran_Marine.destroyScore() * 4;
		return 0;
	};

	std::pair<int, int> FastAPproximation::playerScores() const {
		std::pair<int, int> res;

		for (const auto &u : player1)
			res.first += score(u);

		for (const auto &u : player2)
			res.second += score(u);

		return res;
	}

	std::pair<int, int> FastAPproximation::playerScoresUnits() const {
		std::pair<int, int> res;

		for (const auto &u : player1)
			if (!u.unitType.isBuilding())
				res.first += score(u);

		for (const auto &u : player2)
			if (!u.unitType.isBuilding())
				res.second += score(u);

		return res;
	}

	std::pair<int, int> FastAPproximation::playerScoresBuildings() const {
		std::pair<int, int> res;

		for (const auto &u : player1)
			if (u.unitType.isBuilding())
				res.first += score(u);

		for (const auto &u : player2)
			if (u.unitType.isBuilding())
				res.second += score(u);

		return res;
	}

	std::pair<std::vector<FAPUnit> *,
		std::vector<FAPUnit> *>
		FastAPproximation::getState() {
		return{ &player1, &player2 };
	}

	void FastAPproximation::clearState() { player1.clear(), player2.clear(); }

	void FastAPproximation::dealDamage(const FAPUnit &fu,
		int damage,
		BWAPI::DamageType damageType) const {
		damage <<= 8;
		int remainingShields = fu.shields - damage + (fu.shieldArmor << 8);
		if (remainingShields > 0) {
			fu.shields = remainingShields;
			return;
		}
		else if (fu.shields) {
			damage -= fu.shields + (fu.shieldArmor << 8);
			fu.shields = 0;
		}

		if (!damage)
			return;

		damage -= fu.armor << 8;

		if (damageType == BWAPI::DamageTypes::Concussive) {
			if (fu.unitSize == BWAPI::UnitSizeTypes::Large)
				damage = damage / 4;
			else if (fu.unitSize == BWAPI::UnitSizeTypes::Medium)
				damage = damage / 2;
		}
		else if (damageType == BWAPI::DamageTypes::Explosive) {
			if (fu.unitSize == BWAPI::UnitSizeTypes::Small)
				damage = damage / 2;
			else if (fu.unitSize == BWAPI::UnitSizeTypes::Medium)
				damage = (damage * 3) / 4;
		}

		fu.health -= MAX(128, damage);
	}

	int inline FastAPproximation::distButNotReally(
		const FAPUnit &u1,
		const FAPUnit &u2) const {
		return (u1.x - u2.x) * (u1.x - u2.x) + (u1.y - u2.y) * (u1.y - u2.y);
	}

	bool FastAPproximation::isSuicideUnit(BWAPI::UnitType ut) {
		return (ut == BWAPI::UnitTypes::Zerg_Scourge ||
			ut == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine ||
			ut == BWAPI::UnitTypes::Zerg_Infested_Terran ||
			ut == BWAPI::UnitTypes::Protoss_Scarab);
	}

	void FastAPproximation::unitsim(
		const FAPUnit &fu,
		std::vector<FAPUnit> &enemyUnits) {
		if (fu.attackCooldownRemaining) {
			didSomething = true;
			return;
		}

		auto closestEnemy = enemyUnits.end();
		int closestDist;

		for (auto enemyIt = enemyUnits.begin(); enemyIt != enemyUnits.end();
			++enemyIt) {
			if (enemyIt->flying) {
				if (fu.airDamage) {
					int d = distButNotReally(fu, *enemyIt);
					if ((closestEnemy == enemyUnits.end() || d < closestDist) &&
						d >= fu.airMinRange) {
						closestDist = d;
						closestEnemy = enemyIt;
					}
				}
			}
			else {
				if (fu.groundDamage) {
					int d = distButNotReally(fu, *enemyIt);
					if ((closestEnemy == enemyUnits.end() || d < closestDist) &&
						d >= fu.groundMinRange) {
						closestDist = d;
						closestEnemy = enemyIt;
					}
				}
			}
		}

		if (closestEnemy != enemyUnits.end() && sqrt(closestDist) <= fu.speed &&
			!(fu.x == closestEnemy->x && fu.y == closestEnemy->y)) {
			fu.x = closestEnemy->x;
			fu.y = closestEnemy->y;
			closestDist = 0;

			didSomething = true;
		}

		if (closestEnemy != enemyUnits.end() &&
			closestDist <=
			(closestEnemy->flying ? fu.groundMaxRange : fu.airMinRange)) {
			if (closestEnemy->flying)
				dealDamage(*closestEnemy, fu.airDamage, fu.airDamageType),
				fu.attackCooldownRemaining = fu.airCooldown;
			else {
				dealDamage(*closestEnemy, fu.groundDamage, fu.groundDamageType);
				fu.attackCooldownRemaining = fu.groundCooldown;
				if (fu.elevation != -1 && closestEnemy->elevation != -1)
					if (closestEnemy->elevation > fu.elevation)
						fu.attackCooldownRemaining += fu.groundCooldown;
			}

			if (closestEnemy->health < 1) {
				auto temp = *closestEnemy;
				*closestEnemy = enemyUnits.back();
				enemyUnits.pop_back();
				unitDeath(temp, enemyUnits);
			}

			didSomething = true;
			return;
		}
		else if (closestEnemy != enemyUnits.end() && sqrt(closestDist) > fu.speed) {
			int dx = closestEnemy->x - fu.x, dy = closestEnemy->y - fu.y;

			fu.x += (int)(dx * (fu.speed / sqrt(dx * dx + dy * dy)));
			fu.y += (int)(dy * (fu.speed / sqrt(dx * dx + dy * dy)));

			didSomething = true;
			return;
		}
	}

	void FastAPproximation::medicsim(const FAPUnit &fu,
		std::vector<FAPUnit> &friendlyUnits) {
		auto closestHealable = friendlyUnits.end();
		int closestDist;

		for (auto it = friendlyUnits.begin(); it != friendlyUnits.end(); ++it) {
			if (it->isOrganic && it->health < it->maxHealth && !it->didHealThisFrame) {
				int d = distButNotReally(fu, *it);
				if (closestHealable == friendlyUnits.end() || d < closestDist) {
					closestHealable = it;
					closestDist = d;
				}
			}
		}

		if (closestHealable != friendlyUnits.end()) {
			fu.x = closestHealable->x;
			fu.y = closestHealable->y;

			closestHealable->health += 150;

			if (closestHealable->health > closestHealable->maxHealth)
				closestHealable->health = closestHealable->maxHealth;

			closestHealable->didHealThisFrame = true;
		}
	}

	bool FastAPproximation::suicideSim(const FAPUnit &fu,
		std::vector<FAPUnit> &enemyUnits) {
		auto closestEnemy = enemyUnits.end();
		int closestDist;

		for (auto enemyIt = enemyUnits.begin(); enemyIt != enemyUnits.end();
			++enemyIt) {
			if (enemyIt->flying) {
				if (fu.airDamage) {
					int d = distButNotReally(fu, *enemyIt);
					if ((closestEnemy == enemyUnits.end() || d < closestDist) &&
						d >= fu.airMinRange) {
						closestDist = d;
						closestEnemy = enemyIt;
					}
				}
			}
			else {
				if (fu.groundDamage) {
					int d = distButNotReally(fu, *enemyIt);
					if ((closestEnemy == enemyUnits.end() || d < closestDist) &&
						d >= fu.groundMinRange) {
						closestDist = d;
						closestEnemy = enemyIt;
					}
				}
			}
		}

		if (closestEnemy != enemyUnits.end() && sqrt(closestDist) <= fu.speed) {
			if (closestEnemy->flying)
				dealDamage(*closestEnemy, fu.airDamage, fu.airDamageType);
			else
				dealDamage(*closestEnemy, fu.groundDamage, fu.groundDamageType);

			if (closestEnemy->health < 1) {
				auto temp = *closestEnemy;
				*closestEnemy = enemyUnits.back();
				enemyUnits.pop_back();
				unitDeath(temp, enemyUnits);
			}

			didSomething = true;
			return true;
		}
		else if (closestEnemy != enemyUnits.end() && sqrt(closestDist) > fu.speed) {
			int dx = closestEnemy->x - fu.x, dy = closestEnemy->y - fu.y;

			fu.x += (int)(dx * (fu.speed / sqrt(dx * dx + dy * dy)));
			fu.y += (int)(dy * (fu.speed / sqrt(dx * dx + dy * dy)));

			didSomething = true;
		}

		return false;
	}

	void FastAPproximation::isimulate() {
		for (auto fu = player1.begin(); fu != player1.end();) {
			if (isSuicideUnit(fu->unitType)) {
				bool result = suicideSim(*fu, player2);
				if (result)
					fu = player1.erase(fu);
				else
					++fu;
			}
			else {
				if (fu->unitType == BWAPI::UnitTypes::Terran_Medic)
					medicsim(*fu, player1);
				else
					unitsim(*fu, player2);
				++fu;
			}
		}

		for (auto fu = player2.begin(); fu != player2.end();) {
			if (isSuicideUnit(fu->unitType)) {
				bool result = suicideSim(*fu, player1);
				if (result)
					fu = player2.erase(fu);
				else
					++fu;
			}
			else {
				if (fu->unitType == BWAPI::UnitTypes::Terran_Medic)
					medicsim(*fu, player2);
				else
					unitsim(*fu, player1);
				++fu;
			}
		}

		for (auto &fu : player1) {
			if (fu.attackCooldownRemaining)
				--fu.attackCooldownRemaining;
			if (fu.didHealThisFrame)
				fu.didHealThisFrame = false;
			if (fu.unitType.getRace() == BWAPI::Races::Zerg) {
				if (fu.health < fu.maxHealth)
					fu.health += 4;
				if (fu.health > fu.maxHealth)
					fu.health = fu.maxHealth;
			}
			else if (fu.unitType.getRace() == BWAPI::Races::Protoss) {
				if (fu.shields < fu.maxShields)
					fu.shields += 7;
				if (fu.shields > fu.maxShields)
					fu.shields = fu.maxShields;
			}
		}

		for (auto &fu : player2) {
			if (fu.attackCooldownRemaining)
				--fu.attackCooldownRemaining;
			if (fu.didHealThisFrame)
				fu.didHealThisFrame = false;
			if (fu.unitType.getRace() == BWAPI::Races::Zerg) {
				if (fu.health < fu.maxHealth)
					fu.health += 4;
				if (fu.health > fu.maxHealth)
					fu.health = fu.maxHealth;
			}
			else if (fu.unitType.getRace() == BWAPI::Races::Protoss) {
				if (fu.shields < fu.maxShields)
					fu.shields += 7;
				if (fu.shields > fu.maxShields)
					fu.shields = fu.maxShields;
			}
		}
	}

	void FastAPproximation::unitDeath(const FAPUnit &fu,
		std::vector<FAPUnit> &itsFriendlies) {
		if (fu.unitType == BWAPI::UnitTypes::Terran_Bunker) {
			convertToUnitType(fu, BWAPI::UnitTypes::Terran_Marine);

			for (unsigned i = 0; i < 4; ++i)
				itsFriendlies.push_back(fu);
		}
	}

	void FastAPproximation::convertToUnitType(const FAPUnit &fu,
		BWAPI::UnitType ut) {
		Object ed(fu);
		FAPUnit funew(ed);
		funew.attackCooldownRemaining = fu.attackCooldownRemaining;
		funew.elevation = fu.elevation;

		fu.operator=(funew);
	}

	FAPUnit::FAPUnit(BWAPI::Unit u) : FAPUnit(Object(u)) {}

	FAPUnit::FAPUnit(Object ed)
		: x(ed.getCurrentPosition().x), y(ed.getCurrentPosition().y),

		speed(ed.getPlayer()->topSpeed(ed.getType())),

		health(ed.getCurrentHitPoints()), maxHealth(ed.getType().maxHitPoints()),

		shields(ed.getCurrentShields()),
		shieldArmor(ed.getPlayer()->getUpgradeLevel(
		BWAPI::UpgradeTypes::Protoss_Plasma_Shields)),
		maxShields(ed.getType().maxShields()),
		armor(ed.getPlayer()->armor(ed.getType())), flying(ed.getType().isFlyer()),

		groundDamage(ed.getPlayer()->damage(ed.getType().groundWeapon())),
		groundCooldown(ed.getType().groundWeapon().damageFactor() &&
		ed.getType().maxGroundHits()
		? ed.getPlayer()->weaponDamageCooldown(ed.getType()) /
		(ed.getType().groundWeapon().damageFactor() *
		ed.getType().maxGroundHits())
		: 0),
		groundMaxRange(ed.getPlayer()->weaponMaxRange(ed.getType().groundWeapon())),
		groundMinRange(ed.getType().groundWeapon().minRange()),
		groundDamageType(ed.getType().groundWeapon().damageType()),

		airDamage(ed.getPlayer()->damage(ed.getType().airWeapon())),
		airCooldown(ed.getType().airWeapon().damageFactor() &&
		ed.getType().maxAirHits()
		? ed.getType().airWeapon().damageCooldown() /
		(ed.getType().airWeapon().damageFactor() *
		ed.getType().maxAirHits())
		: 0),
		airMaxRange(ed.getPlayer()->weaponMaxRange(ed.getType().airWeapon())),
		airMinRange(ed.getType().airWeapon().minRange()),
		airDamageType(ed.getType().airWeapon().damageType()),

		unitType(ed.getType()), isOrganic(ed.getType().isOrganic()),
		score(ed.getType().destroyScore()), player(ed.getPlayer()) {

		static int nextId = 0;
		id = nextId++;

		if (ed.getType() == BWAPI::UnitTypes::Protoss_Carrier)
		{
			groundDamage = ed.getPlayer()->damage(
				BWAPI::UnitTypes::Protoss_Interceptor.groundWeapon());

			if (ed.getUnit() && ed.getUnit()->isVisible()) {
				auto interceptorCount = ed.getUnit()->getInterceptorCount();
				if (interceptorCount) {
					groundCooldown = (int)round(37.0f / interceptorCount);
				}
				else {
					groundDamage = 0;
					groundCooldown = 5;
				}
			}
			else {
				if (ed.getPlayer()) {
					groundCooldown =
						(int)round(37.0f / (ed.getPlayer()->getUpgradeLevel(
						BWAPI::UpgradeTypes::Carrier_Capacity)
						? 8
						: 4));
				}
				else {
					groundCooldown = (int)round(37.0f / 8);
				}
			}

			groundDamageType =
				BWAPI::UnitTypes::Protoss_Interceptor.groundWeapon().damageType();
			groundMaxRange = 32 * 8;

			airDamage = groundDamage;
			airDamageType = groundDamageType;
			airCooldown = groundCooldown;
			airMaxRange = groundMaxRange;
		}
		else if (ed.getType() == BWAPI::UnitTypes::Terran_Bunker)
		{
			groundDamage = ed.getPlayer()->damage(BWAPI::WeaponTypes::Gauss_Rifle);
			groundCooldown =
				BWAPI::UnitTypes::Terran_Marine.groundWeapon().damageCooldown() / 4;
			groundMaxRange = ed.getPlayer()->weaponMaxRange(
				BWAPI::UnitTypes::Terran_Marine.groundWeapon()) +
				32;

			airDamage = groundDamage;
			airCooldown = groundCooldown;
			airMaxRange = groundMaxRange;
		}
		else if (ed.getType() == BWAPI::UnitTypes::Protoss_Reaver)
		{
			groundDamage = ed.getPlayer()->damage(BWAPI::WeaponTypes::Scarab);
		}

		if (ed.getUnit() && ed.getUnit()->isStimmed()) {
			groundCooldown /= 2;
			airCooldown /= 2;
		}

		if (ed.getUnit() && ed.getUnit()->isVisible() && !ed.getUnit()->isFlying())
			elevation = BWAPI::Broodwar->getGroundHeight(ed.getUnit()->getTilePosition());

		groundMaxRange *= groundMaxRange;
		groundMinRange *= groundMinRange;
		airMaxRange *= airMaxRange;
		airMinRange *= airMinRange;

		health <<= 8;
		maxHealth <<= 8;
		shields <<= 8;
		maxShields <<= 8;
	}

	const FAPUnit &FAPUnit::
		operator=(const FAPUnit &other) const {
		x = other.x, y = other.y;
		health = other.health, maxHealth = other.maxHealth;
		shields = other.shields, maxShields = other.maxShields;
		speed = other.speed, armor = other.armor, flying = other.flying,
			unitSize = other.unitSize;
		groundDamage = other.groundDamage, groundCooldown = other.groundCooldown,
			groundMaxRange = other.groundMaxRange, groundMinRange = other.groundMinRange,
			groundDamageType = other.groundDamageType;
		airDamage = other.airDamage, airCooldown = other.airCooldown,
			airMaxRange = other.airMaxRange, airMinRange = other.airMinRange,
			airDamageType = other.airDamageType;
		score = other.score;
		attackCooldownRemaining = other.attackCooldownRemaining;
		unitType = other.unitType;
		isOrganic = other.isOrganic;
		didHealThisFrame = other.didHealThisFrame;
		elevation = other.elevation;
		player = other.player;

		return *this;
	}

	bool FAPUnit::operator<(const FAPUnit &other) const {
		return id < other.id;
	}

} // namespace Neolib