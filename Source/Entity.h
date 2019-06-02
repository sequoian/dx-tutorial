#pragma once

#include <stdint.h>

const unsigned int entityIndexBits = 32;
const unsigned int entityGenerationBits = 32;

const unsigned int entityIndexMask = (1 << entityIndexBits) - 1;
const unsigned int entityGenerationMask = (1 << entityGenerationBits) - 1;

struct Entity
{
	uint64_t id;

	uint32_t index() const { return id & entityIndexMask; }
	uint32_t generation() const { return (id >> entityIndexBits) & entityGenerationMask; }
};