#pragma once

// Returns a random value between 0 and 1
double Random();

// Returns a random value between min and max
double RandomRange(double min, double max);

// Seeds the generator with the current time
void SeedRandom();

// Seeds the generator with seed
void SeedRandom(unsigned long seed);