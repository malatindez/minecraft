#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H
#define pow2to63		9223372036854775808
#define pow2263			pow2to63
#define PI			    3.1415926535897932384626433
#define TWOPI           6.2831853071795864769252866
#include <cmath>
#include  <stdint.h>
#include <string.h>
#include <utility>
//https://gist.github.com/eevee/26f547457522755cb1fb8739d0ea89a1

class PerlinNoise {
	const uint64_t seed = 0;
	uint64_t bufSeed = 0;
	uint8_t dimension;
	uint8_t octaves;
	double scaleFactor;
	void sessionRand() {
		bufSeed = seed;
	}
	uint64_t getRandomInteger() {
		bufSeed = bufSeed * 6364136223846793005 + 1442695040888963407;
		return bufSeed;
	}
	double getRandomDouble() {
		double x = getRandomInteger();
		double y = getRandomInteger();
		return x - (y / pow2263);
	}
	double getRandomDouble1() {
		double y = getRandomInteger();
		return 1 - (y / pow2263);
	}
#define GAUSS_NONE 0xffff
	double gauss_next = GAUSS_NONE;
	double gaussDouble(double mu, double sigma) {
		double z = gauss_next;
		gauss_next = GAUSS_NONE;
		if (z == GAUSS_NONE) {
			double x2pi = getRandomDouble1() * TWOPI;
			double g2rad = sqrt(-2.0 * log(1.0 - getRandomDouble1()));
			z = cos(x2pi) * g2rad;
			gauss_next = sin(x2pi) * g2rad;
		}
		return mu + z * sigma;
	}
	PerlinNoise(uint64_t Seed, uint8_t dimension, uint8_t octaves) : seed(Seed) {
		this->dimension = dimension;
		this->octaves = octaves;
		this->scaleFactor = std::pow(dimension, -0.5) * 2;
	}
	int64_t** CartesianProduct(int64_t** grid) {
		int64_t** data = new int64_t * [(uint32_t(1) << (dimension - 1))];
		for (uint32_t i = 0; i < (uint32_t(1) << (dimension - 1)); i++) {
			data[i] = new int64_t[dimension - 1];
		}
		for (uint16_t i = 0; i < dimension - 1; i++) {
			for (uint32_t j = 0; j < (uint32_t(1) << (dimension - 1)); j++) {
				data[j][i] = grid[i][(j / (1 << i)) % 2];
			}
		}
		return data;
	}
	double GetPlainNoise(int64_t* pBuf) {
		int64_t** grid = new int64_t*[dimension - 1];
		for (uint16_t i = 0; i < dimension - 1; i++) {
			grid[i] = new int64_t[2];
			grid[i][0] = pBuf[i];
			grid[i][1] = pBuf[i] + 1;
		}
		uint32_t cProductSize = uint32_t(1 )<< (dimension);
		int64_t** cProduct = CartesianProduct(grid);


		for (uint16_t i = 0; i < dimension - 1; i++) {
			delete[] grid[i];
		}
		for (uint32_t i = 0; i < cProductSize; i++) {
			delete[] cProduct[i];
		}
		delete[] cProduct;
		delete[] grid;
	}
	// size of points is dimension - 1
	void CalculateAtPoint(int64_t *points) { // result is y coordinate
		double returnValue = 0;
		for (uint16_t octave = 0; octave < octaves; octave++) {
			double o = uint64_t(1) << octave;
			int64_t* pBuf = new int64_t[dimension - 1];
			memcpy(pBuf, points, sizeof(int64_t) * (dimension - 1));
			for (uint16_t i = 0; i < dimension - 1; i++) {
				pBuf[i] *= o;
			}
			returnValue += GetPlainNoise(pBuf) / o;
			delete[] pBuf;
		}
		returnValue /= double(2) - double(1) / (uint64_t(1) << (octaves - 1));
		// 2 - 2 ** (1 - octaves)
	}
};
#undef GAUSS_NONE
#undef pow2263
#undef pow2to63
#endif