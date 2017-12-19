BGH Bot Version 1.11a
This is a Broodwar AI developed in C++ using Visual Studio Community 2013, BWAPI, and the BWEM library. This bot is designed for normal money maps and BGH for all races, except Zerg which is only designed for BGH.

Version History:
1.11a
Changes:
-Fixed some nullptr crashes.

1.11
Changes:
-Added mineral locking
-Added upgrades for default Terran build.
-Made tweaks to Macro for Terran default build.
-Added ability for bot to build from any base if there are no workers left in the main.

1.10b
Changes:
-Added Bunker support.
-Added repair script for bunker.
-Fixed bug related to getGroundDistance.

1.10a
Changes:
-Fixed expansion logic so that units won't clutter the ramp to the natural.
-Fixed a crash involving the military manager.
-Fixed bug that caused tank count to be incorrect.

1.10
Changes:
-Modified Terran Default build to include tanks.

1.09a
Changes:
-Fixed some crashes.
-Rebuilt Protoss building placement.
-Adjusted PvP 4 Gate Goon build to be more robust.

1.09
Changes:
-Fixed some crashes related to BWEM areas.
-Added pvp build.
-Added logic regarding army strength both globally, objectively, and locally for all races.

1.08
Changes:
-Fixed infinite loop that could happen when an SCV building a command center was replaced, then the new SCV was chosen to build a structure.

1.07g
Changes:
-Added logic for determining which chokepoint to defend when expanding. This is based on a rough estimate of "ground distance," that could be applied to other concepts the bot is doing.

1.07f
Changes:
-Fixed bugs that would cause crashes.

1.07e
Changes:
-Bug Fixes
-Added code so workers will attack other workers that are attacking them.

1.07d
Changes:
-Fixed bugs pertaining to trying to build in fog of war.

1.07c
Changes:
-Bug fixes.

1.07b
Changes:
-Bug fixes.

1.07a
Changes:
-Bug fixes.

1.07
Changes:
-Fixed bugs.
-Modified some macro logic to accomodate expanding better.
-Added expansion logic for Protoss.

1.06a
Changes:
-Added logic for Terran to send SCVs to incomplete buildings that don't have an SCV building them.

1.06
Changes:
-Added logic to support expanding for Terran.
-Modified Worker logic to support multiple bases.
-Fixed search logic so units fight enemy units they encounter.

1.05a
Changes:
-Fixed slowdown for air units over large cliff areas.

1.05
Changes:
-May have unknown changes, as there was a break in the project
-Added timers for various functions.

1.04
Changes:
-Revampped how military is handled. Added "Objective" where units are assigned to objectives and micro based on their objective.
-Modified detector logic.

1.03
Changes:
-Added High Templar to Protoss army compisition.

Bug fixes:
-Fixed crash related to detectors.

1.02
Changes:
-Added a 30 second timeout for build orders to workers. If the requested structure is not placed after 30 seconds, the assigned worker will give up.
-Added a check for building locations to prevent them from being over one area away.

1.01
Changes:
-Added new build order for Terran. If bot is outnumberd (counting allies), it will always use the new build order; otherwise, it has a 50% chance of using the new build order.
-Modified build placement logic for Terran Supply Depots.

1.0
-Added README.md file, considering this commit to be version 1.0.