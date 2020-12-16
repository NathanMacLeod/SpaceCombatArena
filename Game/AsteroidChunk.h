#pragma once

#include "Asteroid.h"
#include <unordered_map>
#include <string>

class AsteroidChunk {
public:
	AsteroidChunk(SpaceMinerGame* game, int x, int y, int z);
	
	static void setUniverseProperties(double chunkSize, double loadRange, double asteroidDensity);
	static void loadChunk(SpaceMinerGame* game, int x, int y, int z);
	static void unloadChunk(SpaceMinerGame* game, int x, int y, int z);
	static bool chunkActive(int x, int y, int z, AsteroidChunk** out = nullptr);
	static void unloadAllChunks(SpaceMinerGame* game);
	static void loadChunksAtPos(SpaceMinerGame* game, int currX, int currY, int currZ, int prevX = 0, int prevY = 0, int prevZ = 0, bool reload = false);
	static double getChunkSize();
	static double getLoadRange();

private:

	std::vector<Asteroid*> asteroids;
	int x, y, z;
	uint32_t chunkVal;

	//static stuff
	static uint32_t seed;
	static double chunkSize;
	static double asteroidDensity; //average asteroids per unit of chunk volume
	static double chunkLoadRange;

	static std::unordered_map<int, AsteroidChunk*> activeChunks;

	static uint32_t getCoordVal(int x, int y, int z);

	static uint32_t state;
	static void sLehmer(uint32_t seed) {
		state = seed;
	}
	//from https://en.wikipedia.org/wiki/Lehmer_random_number_generator
	static uint32_t lehmerRand() {
		uint64_t product = ((uint64_t) state) * 48271;
		uint32_t x = (product & 0x7fffffff) + (product >> 31);

		x = (x & 0x7fffffff) + (x >> 31);
		return state = x;
	}
	static float lehmerFloat() {
		return 2.0 * (double)lehmerRand() / UINT32_MAX;
	}
};