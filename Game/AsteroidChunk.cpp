#include "AsteroidChunk.h"
#include "SpaceMinerGame.h"

double AsteroidChunk::chunkSize;
double AsteroidChunk::asteroidDensity; //average asteroids per unit of chunk volume
double AsteroidChunk::chunkLoadRange;
uint32_t AsteroidChunk::seed;

std::unordered_map<int, AsteroidChunk*> AsteroidChunk::activeChunks;

uint32_t AsteroidChunk::state;

AsteroidChunk::AsteroidChunk(SpaceMinerGame* game, int x, int y, int z) {
	this->x = x;
	this->y = y;
	this->z = z;
	
	chunkVal = getCoordVal(x, y, z);
	sLehmer(chunkVal);
	double meanAst = chunkSize * chunkSize * chunkSize * asteroidDensity;
	
	lehmerFloat();
	float u = lehmerFloat();
	int nAsteroids = 0;
	double eVal = pow(2.718, -meanAst);
	double k = 1;
	double l = eVal;
	while (l < u) {
		nAsteroids++;
		k *= meanAst / nAsteroids;
		l += eVal * k;
	}
	//int nAsteroids = meanAst;

	//printf("x: %d, y: %d, z: %d, uVal: %f, nAsteroids: %d\n", x, y, z, u, nAsteroids);

	for (int i = 0; i < nAsteroids; i++) {
		double xPos = x * chunkSize + chunkSize * lehmerFloat() - chunkSize / 2.0;
		double yPos = y * chunkSize + chunkSize * lehmerFloat() - chunkSize / 2.0;
		double zPos = z * chunkSize + chunkSize * lehmerFloat() - chunkSize / 2.0;
		
		double size = Asteroid::pickAsteroidSize(lehmerFloat());

		Asteroid* ast = new Asteroid(Vector3D(xPos, yPos, zPos), size);
		game->addAsteroid(ast);
		asteroids.push_back(ast);
	}
}

void AsteroidChunk::unloadAllChunks(SpaceMinerGame* game) {
	for (std::unordered_map<int, AsteroidChunk*>::iterator itr = activeChunks.begin(); itr != activeChunks.end(); itr++) {
		AsteroidChunk* chunk = itr->second;
		if (chunk != nullptr) {
			activeChunks[itr->first] = nullptr;
			for (Asteroid* ast : chunk->asteroids) {
				ast->markUnloaded();
			}
			delete chunk;
		}
	}
	activeChunks.clear();
}

double AsteroidChunk::getChunkSize() {
	return chunkSize;
}

double AsteroidChunk::getLoadRange() {
	return chunkLoadRange;
}

void AsteroidChunk::loadChunk(SpaceMinerGame* game, int x, int y, int z) {
	if (!chunkActive(x, y, z)) {
		activeChunks[getCoordVal(x, y, z)] = new AsteroidChunk(game, x, y, z);
	}
}

void AsteroidChunk::unloadChunk(SpaceMinerGame* game, int x, int y, int z) {
	AsteroidChunk* chunk = nullptr;
	if (chunkActive(x, y, z, &chunk)) {
		uint32_t v = getCoordVal(x, y, z);
		activeChunks[v] = nullptr;

		for (Asteroid* ast : chunk->asteroids) {
			ast->markUnloaded();
		}
		delete chunk;
		activeChunks.erase(v);
	}
}

bool AsteroidChunk::chunkActive(int x, int y, int z, AsteroidChunk** out) {
	uint32_t v = getCoordVal(x, y, z);
	std::unordered_map<int, AsteroidChunk*>::const_iterator found = activeChunks.find(v);
	if (found == activeChunks.end() || found->second == nullptr) {
		return false;
	}
	else {
		if (out != nullptr) {
			*out = found->second;
		}
		return true;
	}
}

uint32_t AsteroidChunk::getCoordVal(int x, int y, int z) {
	return (uint32_t)1 + seed + ((x & 0x3ff) << 22) + ((y & 0x7ff) << 11) + (z & 0x7ff);
}

void AsteroidChunk::setUniverseProperties(double chunkSize, double loadRange, double asteroidDensity) {
	AsteroidChunk::chunkSize = chunkSize;
	AsteroidChunk::asteroidDensity = asteroidDensity;
	AsteroidChunk::chunkLoadRange = loadRange;
	AsteroidChunk::seed = rand();
}

void AsteroidChunk::loadChunksAtPos(SpaceMinerGame* game, int currX, int currY, int currZ, int prevX, int prevY, int prevZ, bool reload) {
	int dim = getLoadRange() / getChunkSize();

	//load in all new chunks (loadChunk checks if chunk already loaded first);
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			for (int k = 0; k < dim; k++) {
				int x = i - dim / 2;
				int y = j - dim / 2;
				int z = k - dim / 2;
				if (x * x + y * y + z * z < dim * dim / 4) {
					AsteroidChunk::loadChunk(game, currX + x, currY + y, currZ + z);
				}
			}
		}
	}

	//remove all chunks in vicinity previous position
	if (reload) {
		for (int i = 0; i < dim; i++) {
			for (int j = 0; j < dim; j++) {
				for (int k = 0; k < dim; k++) {
					int x = prevX + i - dim / 2;
					int y = prevY + j - dim / 2;
					int z = prevZ + k - dim / 2;
					if ((x - currX) * (x - currX) + (y - currY) * (y - currY) + (z - currZ) * (z - currZ) > dim * dim / 4) {
						AsteroidChunk::unloadChunk(game, x, y, z);
					}
				}
			}
		}
	}
}