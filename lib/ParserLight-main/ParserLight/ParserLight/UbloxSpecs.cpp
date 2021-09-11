#include "stdafx.h"
#include "UbloxSpecs.h"
#include <iostream>


UbloxSpecs::UbloxSpecs()
{
}


UbloxSpecs::~UbloxSpecs()
{
}

void UbloxSpecs::test_specs()
{
	/* Consitency check of variable size. One never knows... */
	if (sizeof(ub_u1) != 1){ std::cerr << "WARNING: ub_u1 has wrong size!\n"; };
	if (sizeof(ub_u2) != 2){ std::cerr << "WARNING: ub_u2 has wrong size!\n"; };
	if (sizeof(ub_u4) != 4){ std::cerr << "WARNING: ub_u4 has wrong size!\n"; };
	if (sizeof(ub_i1) != 1){ std::cerr << "WARNING: ub_i1 has wrong size!\n"; };
	if (sizeof(ub_i2) != 2){ std::cerr << "WARNING: ub_i2 has wrong size!\n"; };
	if (sizeof(ub_i4) != 4){ std::cerr << "WARNING: ub_i4 has wrong size!\n"; };
	if (sizeof(ub_r4) != 4){ std::cerr << "WARNING: ub_r4 has wrong size!\n"; };
	if (sizeof(ub_r8) != 8){ std::cerr << "WARNING: ub_r8 has wrong size!\n"; };
}