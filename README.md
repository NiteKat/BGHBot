BGH Bot Version 1.06
This is a Broodwar AI developed in C++ using Visual Studio Community 2013, BWAPI, and the BWEM library. This bot is designed for play on the Big Game Hunters map, and can play all three races in the game.

Version History:
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