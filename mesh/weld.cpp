//-------------------------------------------------------------------------------------
// weld.cpp
//  
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#include "directxtest.h"
#include "TestHelpers.h"
#include "TestGeometry.h"
#include "ShapesGenerator.h"

#include "directxmesh.h"

#include "TestHelpers.h"
#include "TestGeometry.h"

using namespace DirectX;
using namespace TestGeometry;

namespace
{
    // Cube
    const uint32_t s_cubePointReps[8] =
    {
        0, 1, 2, 3, 4, 5, 6, 7,
    };

    // Face-mapped cube
    const uint32_t s_fmCubePointReps[24] =
    {
        0, 1, 2, 3, 4, 5,
        6, 7, 7, 4, 0, 3,
        6, 5, 1, 2, 4, 5,
        1, 0, 7, 6, 2, 3,
    };

    const uint32_t s_fmCubePointRepsEps[24] =
    {
        19, 18, 22,  3, 16, 17,
        12, 7,   7, 16, 19,  3,
        12, 17, 18, 22, 16, 17,
        18, 19,  7, 12, 22,  3,
    };
}

//-------------------------------------------------------------------------------------
bool Test26()
{
    bool success = false;

    // TODO -

    return success;
}