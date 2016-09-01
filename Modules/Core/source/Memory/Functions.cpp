// Functions.cpp

#include <cstdlib>
#include "../../include/Core/Memory/Functions.h"

void* sge::malloc(std::size_t size)
{
	return std::malloc(size);
}

void sge::free(void* buff)
{
	std::free(buff);
}
