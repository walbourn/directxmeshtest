//-------------------------------------------------------------------------------------
// adjacency.cpp
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#include "directxtest.h"

#include "DirectXMesh.h"

#include <algorithm>
#include <random>

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

    const uint32_t s_cubeAdj[3 * 12] =
    {
        1, 6, 8,
        2, 4, 0,
        3, 5, 1,
        9, 11, 2,
        5, 6, 1,
        2, 11, 4,
        0, 4, 7,
        6, 10, 8,
        0, 7, 9,
        8, 10, 3,
        9, 7, 11,
        10, 5, 3
    };

    const uint16_t s_cubeGSAdj16[3 * 12 * 2] =
    {
        0, 3, 1, 6, 2, 7,
        0, 4, 3, 6, 1, 2,
        0, 5, 4, 6, 3, 1,
        0, 7, 5, 6, 4, 3,
        3, 4, 6, 2, 1, 0,
        3, 0, 4, 5, 6, 1,
        2, 0, 1, 3, 6, 7,
        2, 1, 6, 5, 7, 0,
        0, 1, 2, 6, 7, 5,
        0, 2, 7, 6, 5, 4,
        5, 0, 7, 2, 6, 4,
        5, 7, 6, 3, 4, 0
    };

    const uint32_t s_cubeGSAdj32[3 * 12 * 2] =
    {
        0, 3, 1, 6, 2, 7,
        0, 4, 3, 6, 1, 2,
        0, 5, 4, 6, 3, 1,
        0, 7, 5, 6, 4, 3,
        3, 4, 6, 2, 1, 0,
        3, 0, 4, 5, 6, 1,
        2, 0, 1, 3, 6, 7,
        2, 1, 6, 5, 7, 0,
        0, 1, 2, 6, 7, 5,
        0, 2, 7, 6, 5, 4,
        5, 0, 7, 2, 6, 4,
        5, 7, 6, 3, 4, 0
    };

    // Face-mapped cube
    const uint32_t s_fmCubePointReps[24] =
    {
        0, 1, 2, 3, 4, 5,
        6, 7, 7, 4, 0, 3,
        6, 5, 1, 2, 4, 5,
        1, 0, 7, 6, 2, 3,
    };

    const uint32_t s_fmCubeAdj[3 * 12] =
    {
        1, 9, 5,
        7, 0, 11,
        3, 8, 6,
        4, 2, 10,
        5, 3, 11,
        8, 4, 0,
        7, 2, 9,
        10, 6, 1,
        9, 2, 5,
        6, 8, 0,
        11, 3, 7,
        4, 10, 1
    };

    const uint32_t s_fmCubePointRepsEps[24] =
    {
        19, 18, 22,  3, 16, 17,
        12, 7,   7, 16, 19,  3,
        12, 17, 18, 22, 16, 17,
        18, 19,  7, 12, 22,  3,
    };

    const uint16_t s_fmCubeGSAdj16[3 * 12 * 2] =
    {
        3, 2, 1, 17, 0, 9,
        2, 12, 1, 0, 3, 20,
        6, 7, 4, 19, 5, 14,
        7, 11, 4, 5, 6, 22,
        11, 10, 9, 6, 8, 22,
        10, 17, 9, 8, 11, 1,
        14, 15, 12, 4, 13, 19,
        15, 20, 12, 13, 14, 3,
        19, 18, 17, 6, 16, 11,
        18, 12, 17, 16, 19, 3,
        22, 23, 20, 4, 21, 14,
        23, 9, 20, 21, 22, 1
    };

    const uint32_t s_fmCubeGSAdj32[3 * 12 * 2] =
    {
        3, 2, 1, 17, 0, 9,
        2, 12, 1, 0, 3, 20,
        6, 7, 4, 19, 5, 14,
        7, 11, 4, 5, 6, 22,
        11, 10, 9, 6, 8, 22,
        10, 17, 9, 8, 11, 1,
        14, 15, 12, 4, 13, 19,
        15, 20, 12, 13, 14, 3,
        19, 18, 17, 6, 16, 11,
        18, 12, 17, 16, 19, 3,
        22, 23, 20, 4, 21, 14,
        23, 9, 20, 21, 22, 1
    };

    // Tetrahedron
    const uint32_t s_tetraPointReps[4] =
    {
        0, 1, 2, 3,
    };

    const uint32_t s_tetraAdj[3 * 4] =
    {
        3, uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), 0
    };

    const uint16_t s_tetraGSAdj16[3 * 4 * 2] =
    {
        0, 3, 1, 0, 2, 1,
        1, 0, 2, 1, 0, 2,
        2, 3, 0, 2, 3, 0,
        0, 1, 3, 0, 1, 2
    };

    const uint32_t s_tetraGSAdj32[3 * 4 * 2] =
    {
        0, 3, 1, 0, 2, 1,
        1, 0, 2, 1, 0, 2,
        2, 3, 0, 2, 3, 0,
        0, 1, 3, 0, 1, 2
    };

    // Unused
    const uint16_t s_unused16[12 * 3] =
    {
        0, 1, 2,
        0, 3, 1,
        0, 4, 3,
        0, 5, 4,
        3, 6, 1,
        3, 4, 6,
        uint16_t(-1), uint16_t(-1), uint16_t(-1),
        2, 6, 7,
        0, 2, 7,
        0, 7, 5,
        5, 7, 6,
        5, 6, 4,
    };

    const uint32_t s_unused32[12 * 3] =
    {
        0, 1, 2,
        0, 3, 1,
        0, 4, 3,
        0, 5, 4,
        3, 6, 1,
        3, 4, 6,
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        2, 6, 7,
        0, 2, 7,
        0, 7, 5,
        5, 7, 6,
        5, 6, 4,
    };

    const uint32_t s_unusedPR[8] =
    {
        0, 1, 2, 3, 4, uint32_t(-1), 6, 7,
    };

    const uint32_t s_unusedAdj[3 * 12] =
    {
        1, uint32_t(-1), 8,
        2, 4, 0,
        3, 5, 1,
        9, 11, 2,
        5, uint32_t(-1), 1,
        2, 11, 4,
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), 10, 8,
        0, 7, 9,
        8, 10, 3,
        9, 7, 11,
        10, 5, 3
    };

    const uint16_t s_unused16_1st[12 * 3] =
    {
        uint16_t(-1), uint16_t(-1), uint16_t(-1),
        0, 3, 1,
        0, 4, 3,
        0, 5, 4,
        3, 6, 1,
        3, 4, 6,
        2, 1, 6,
        2, 6, 7,
        0, 2, 7,
        0, 7, 5,
        5, 7, 6,
        5, 6, 4,
    };

    const uint32_t s_unused32_1st[12 * 3] =
    {
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        0, 3, 1,
        0, 4, 3,
        0, 5, 4,
        3, 6, 1,
        3, 4, 6,
        2, 1, 6,
        2, 6, 7,
        0, 2, 7,
        0, 7, 5,
        5, 7, 6,
        5, 6, 4,
    };

    const uint16_t s_unused16_all[12 * 3] =
    {
        uint16_t(-1), uint16_t(-1), uint16_t(-1),
        uint16_t(-1), uint16_t(-1), uint16_t(-1),
        uint16_t(-1), uint16_t(-1), uint16_t(-1),
        uint16_t(-1), uint16_t(-1), uint16_t(-1),
        uint16_t(-1), uint16_t(-1), uint16_t(-1),
        uint16_t(-1), uint16_t(-1), uint16_t(-1),
        uint16_t(-1), uint16_t(-1), uint16_t(-1),
        uint16_t(-1), uint16_t(-1), uint16_t(-1),
        uint16_t(-1), uint16_t(-1), uint16_t(-1),
        uint16_t(-1), uint16_t(-1), uint16_t(-1),
        uint16_t(-1), uint16_t(-1), uint16_t(-1),
        uint16_t(-1), uint16_t(-1), uint16_t(-1),
    };

    const uint32_t s_unused32_all[12 * 3] =
    {
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
    };
}

//-------------------------------------------------------------------------------------
// GenerateAdjacencyAndPointReps (point reps)
bool Test07()
{
    bool success = true;

    // 16-bit pointreps (cube)
    {
        std::unique_ptr<uint32_t[]> preps( new uint32_t[ 8 ] );
        memset( preps.get(), 0xff, sizeof(uint32_t) * 8 );

        HRESULT hr = GenerateAdjacencyAndPointReps( g_cubeIndices16, 12, g_cubeVerts, 8, 0.f, preps.get(), nullptr );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidPointReps( preps.get(), 8 ) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [exact] invalid pointRep\n" );
            success = false;
        }
        else if ( memcmp( preps.get(), s_cubePointReps, sizeof(s_cubePointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [exact] failed\n" );
            success = false;
            for( size_t j = 0; j < 8; ++j )
            {
                if ( preps[ j ] != s_cubePointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_cubePointReps[j] );
                }
            }
        }

        // epsilon is same as exact for cube with no overlapping verts
        hr = GenerateAdjacencyAndPointReps( g_cubeIndices16, 12, g_cubeVerts, 8, 0.0001f, preps.get(), nullptr );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [epsilon] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidPointReps( preps.get(), 8 ) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [epsilon] invalid pointRep\n" );
            success = false;
        }
        else if ( memcmp( preps.get(), s_cubePointReps, sizeof(s_cubePointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [epsilon] failed\n" );
            success = false;
            for( size_t j = 0; j < 8; ++j )
            {
                if ( preps[ j ] != s_cubePointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_cubePointReps[j] );
                }
            }
        }

        // unused
        hr = GenerateAdjacencyAndPointReps(s_unused16_1st, 12, g_cubeVerts, 8, 0.f, preps.get(), nullptr);
        if (FAILED(hr))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [unused 1st] failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!IsValidPointReps(preps.get(), 8))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [unused 1st] invalid pointRep\n");
            success = false;
        }

        hr = GenerateAdjacencyAndPointReps(s_unused16_all, 12, g_cubeVerts, 8, 0.f, preps.get(), nullptr);
        if (FAILED(hr))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [unused all] failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!IsValidPointReps(preps.get(), 8))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [unused all] invalid pointRep\n");
            success = false;
        }

        // invalid args
        hr = GenerateAdjacencyAndPointReps(static_cast<const uint16_t*>(nullptr), 0, nullptr, 0, 0.f, nullptr, nullptr);
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) expected failure for 0 count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateAdjacencyAndPointReps(g_cubeIndices16, 12, g_cubeVerts, 8, 0.f, nullptr, nullptr);
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) expected failure for null result (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = GenerateAdjacencyAndPointReps(static_cast<const uint16_t*>(nullptr), 23, nullptr, 52, 0.f, nullptr, nullptr);
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) expected failure for null parameters (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateAdjacencyAndPointReps( g_cubeIndices16, 12, g_cubeVerts, UINT16_MAX /*D3D11_16BIT_INDEX_STRIP_CUT_VALUE*/, 0.f, preps.get(), nullptr );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateAdjacencyAndPointReps( g_cubeIndices16, 12, g_cubeVerts, UINT32_MAX, 0.f, preps.get(), nullptr );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateAdjacencyAndPointReps( g_cubeIndices16, UINT32_MAX, g_cubeVerts, 8, 0.f, preps.get(), nullptr );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateAdjacencyAndPointReps( g_cubeIndices16, 12, g_cubeVerts, 2, 0.f, preps.get(), nullptr );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // 16-bit pointreps (face-mapped cube)
    {
        std::unique_ptr<uint32_t[]> preps( new uint32_t[ 24 ] );
        memset( preps.get(), 0xff, sizeof(uint32_t) * 24 );

        HRESULT hr = GenerateAdjacencyAndPointReps( g_fmCubeIndices16, 12, g_fmCubeVerts, 24, 0.f, preps.get(), nullptr );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) fmcube [exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidPointReps( preps.get(), 24 ) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) fmcube [exact] invalid pointRep\n" );
            success = false;
        }
        else if ( memcmp( preps.get(), s_fmCubePointReps, sizeof(s_fmCubePointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) fmcube [exact] failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                if ( preps[ j ] != s_fmCubePointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_fmCubePointReps[j] );
                }
            }
        }

        // epsilon
        hr = GenerateAdjacencyAndPointReps( g_fmCubeIndices16, 12, g_fmCubeVerts, 24, 0.0001f, preps.get(), nullptr );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) fmcube [epsilon] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidPointReps( preps.get(), 24 ) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) fmcube [epsilon] invalid pointRep\n" );
            success = false;
        }
        else  if ( memcmp( preps.get(), s_fmCubePointRepsEps, sizeof(s_fmCubePointRepsEps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) fmcube [epsilon] failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                if ( preps[ j ] != s_fmCubePointRepsEps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_fmCubePointRepsEps[j] );
                }
            }
        }
    }

    // 16-bit pointreps (tetra)
    {
        std::unique_ptr<uint32_t[]> preps( new uint32_t[ 4 ] );
        memset( preps.get(), 0xff, sizeof(uint32_t) * 4 );

        HRESULT hr = GenerateAdjacencyAndPointReps( g_tetraIndices16, 4, g_tetraVerts, 4, 0.f, preps.get(), nullptr );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) tetra [exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidPointReps( preps.get(), 4 ) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) tetra [exact] invalid pointRep\n" );
            success = false;
        }
        else if ( memcmp( preps.get(), s_tetraPointReps, sizeof(s_tetraPointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) tetra [exact] failed\n" );
            success = false;
            for( size_t j = 0; j < 4; ++j )
            {
                if ( preps[ j ] != s_tetraPointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_tetraPointReps[j] );
                }
            }
        }

        // epsilon is same as exact for tetra with no overlapping verts
        hr = GenerateAdjacencyAndPointReps( g_tetraIndices16, 4, g_tetraVerts, 4, 0.0001f, preps.get(), nullptr );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) tetra [epsilon] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidPointReps( preps.get(), 4 ) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) tetra [epsilon] invalid pointRep\n" );
            success = false;
        }
        else if ( memcmp( preps.get(), s_tetraPointReps, sizeof(s_tetraPointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) tetra [epsilon] failed\n" );
            success = false;
            for( size_t j = 0; j < 4; ++j )
            {
                if ( preps[ j ] != s_tetraPointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_tetraPointReps[j] );
                }
            }
        }
    }

    // 32-bit pointreps (cube)
    {
        std::unique_ptr<uint32_t[]> preps( new uint32_t[ 8 ] );
        memset( preps.get(), 0xff, sizeof(uint32_t) * 8 );

        HRESULT hr = GenerateAdjacencyAndPointReps( g_cubeIndices32, 12, g_cubeVerts, 8, 0.f, preps.get(), nullptr );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidPointReps( preps.get(), 8 ) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [exact] invalid pointRep\n" );
            success = false;
        }
        else if ( memcmp( preps.get(), s_cubePointReps, sizeof(s_cubePointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [exact] failed\n" );
            success = false;
            for( size_t j = 0; j < 8; ++j )
            {
                if ( preps[ j ] != s_cubePointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_cubePointReps[j] );
                }
            }
        }

        // epsilon is same as exact for cube with no overlapping verts
        hr = GenerateAdjacencyAndPointReps( g_cubeIndices32, 12, g_cubeVerts, 8, 0.0001f, preps.get(), nullptr );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [epsilon] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidPointReps( preps.get(), 8 ) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [epsilon] invalid pointRep\n" );
            success = false;
        }
        else if ( memcmp( preps.get(), s_cubePointReps, sizeof(s_cubePointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [epsilon] failed\n" );
            success = false;
            for( size_t j = 0; j < 8; ++j )
            {
                if ( preps[ j ] != s_cubePointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_cubePointReps[j] );
                }
            }
        }

        // unused
        hr = GenerateAdjacencyAndPointReps(s_unused32_1st, 12, g_cubeVerts, 8, 0.f, preps.get(), nullptr);
        if (FAILED(hr))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [unused 1st] failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!IsValidPointReps(preps.get(), 8))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [unused 1st] invalid pointRep\n");
            success = false;
        }

        hr = GenerateAdjacencyAndPointReps(s_unused32_all, 12, g_cubeVerts, 8, 0.f, preps.get(), nullptr);
        if (FAILED(hr))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [unused all] failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!IsValidPointReps(preps.get(), 8))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [unused all] invalid pointRep\n");
            success = false;
        }

        // invalid args
        hr = GenerateAdjacencyAndPointReps(static_cast<const uint32_t*>(nullptr), 0, nullptr, 0, 0.f, nullptr, nullptr);
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) expected failure for 0 count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateAdjacencyAndPointReps(g_cubeIndices32, 12, g_cubeVerts, 8, 0.f, nullptr, nullptr);
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) expected failure for null result (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = GenerateAdjacencyAndPointReps(static_cast<const uint32_t*>(nullptr), 23, nullptr, 52, 0.f, nullptr, nullptr);
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) expected failure for null parameters (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateAdjacencyAndPointReps( g_cubeIndices32, 12, g_cubeVerts, UINT32_MAX /*D3D11_32BIT_INDEX_STRIP_CUT_VALUE*/, 0.f, preps.get(), nullptr );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateAdjacencyAndPointReps( g_cubeIndices32, 12, g_cubeVerts, UINT32_MAX, 0.f, preps.get(), nullptr );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateAdjacencyAndPointReps( g_cubeIndices32, UINT32_MAX, g_cubeVerts, 8, 0.f, preps.get(), nullptr );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateAdjacencyAndPointReps( g_cubeIndices32, 12, g_cubeVerts, 2, 0.f, preps.get(), nullptr );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // 32-bit pointreps (face-mapped cube)
    {
        std::unique_ptr<uint32_t[]> preps( new uint32_t[ 24 ] );
        memset( preps.get(), 0xff, sizeof(uint32_t) * 24 );

        HRESULT hr = GenerateAdjacencyAndPointReps( g_fmCubeIndices32, 12, g_fmCubeVerts, 24, 0.f, preps.get(), nullptr );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) fmcube [exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidPointReps( preps.get(), 24 ) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) fmcube [exact] invalid pointRep\n" );
            success = false;
        }
        else if ( memcmp( preps.get(), s_fmCubePointReps, sizeof(s_fmCubePointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) fmcube [exact] failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                if ( preps[ j ] != s_fmCubePointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_fmCubePointReps[j] );
                }
            }
        }

        // epsilon
        hr = GenerateAdjacencyAndPointReps( g_fmCubeIndices32, 12, g_fmCubeVerts, 24, 0.0001f, preps.get(), nullptr );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) fmcube [epsilon] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidPointReps( preps.get(), 24 ) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) fmcube [epsilon] invalid pointRep\n" );
            success = false;
        }
        else if ( memcmp( preps.get(), s_fmCubePointRepsEps, sizeof(s_fmCubePointRepsEps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) fmcube [epsilon] failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                if ( preps[ j ] != s_fmCubePointRepsEps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_fmCubePointRepsEps[j] );
                }
            }
        }
    }

    // 32-bit pointreps (tetra)
    {
        std::unique_ptr<uint32_t[]> preps( new uint32_t[ 4 ] );
        memset( preps.get(), 0xff, sizeof(uint32_t) * 4 );

        HRESULT hr = GenerateAdjacencyAndPointReps( g_tetraIndices32, 4, g_tetraVerts, 4, 0.f, preps.get(), nullptr );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) tetra [exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidPointReps( preps.get(), 4 ) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) tetra [exact] invalid pointRep\n" );
            success = false;
        }
        else if ( memcmp( preps.get(), s_tetraPointReps, sizeof(s_tetraPointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) tetra [exact] failed\n" );
            success = false;
            for( size_t j = 0; j < 4; ++j )
            {
                if ( preps[ j ] != s_tetraPointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_tetraPointReps[j] );
                }
            }
        }

        // epsilon is same as exact for tetra with no overlapping verts
        hr = GenerateAdjacencyAndPointReps( g_tetraIndices32, 4, g_tetraVerts, 4, 0.0001f, preps.get(), nullptr );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) tetra [epsilon] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidPointReps( preps.get(), 4 ) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) tetra [epsilon] invalid pointRep\n" );
            success = false;
        }
        else if ( memcmp( preps.get(), s_tetraPointReps, sizeof(s_tetraPointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) tetra [epsilon] failed\n" );
            success = false;
            for( size_t j = 0; j < 4; ++j )
            {
                if ( preps[ j ] != s_tetraPointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_tetraPointReps[j] );
                }
            }
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// GenerateAdjacencyAndPointReps (adjacency)
bool Test08()
{
    bool success = true;

    // 16-bit adjacency (cube)
    {
        std::unique_ptr<uint32_t[]> preps( new uint32_t[ 8 ] );
        memset( preps.get(), 0xff, sizeof(uint32_t) * 8 );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 3 * 12 ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) * 3 * 12 );

        HRESULT hr = GenerateAdjacencyAndPointReps( g_cubeIndices16, 12, g_cubeVerts, 8, 0.f, preps.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( preps.get(), s_cubePointReps, sizeof(s_cubePointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [exact] preps failed\n" );
            success = false;
            for( size_t j = 0; j < 8; ++j )
            {
                if ( preps[ j ] != s_cubePointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_cubePointReps[j] );
                }
            }
        }
        else if ( memcmp( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [exact] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_cubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_cubeAdj[j] );
                }
            }
        }

        // nullptr preps
        hr = GenerateAdjacencyAndPointReps( g_cubeIndices16, 12, g_cubeVerts, 8, 0.f, nullptr, adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [nullpr exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [nullpr exact] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_cubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_cubeAdj[j] );
                }
            }
        }

        // unused
        hr = GenerateAdjacencyAndPointReps(s_unused16_1st, 12, g_cubeVerts, 8, 0.f, preps.get(), adj.get());
        if (FAILED(hr))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [unused 1st] failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!IsValidPointReps(preps.get(), 8))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [unused 1st] invalid pointRep\n");
            success = false;
        }
        else
        {
            std::wstring msgs;
            if (FAILED(Validate(s_unused16_1st, 12, 8, adj.get(), VALIDATE_UNUSED, &msgs)))
            {
                printe("ERROR: GenerateAdjacencyAndPointReps(16) cube [unused 1st] validate failed\n%ls\n", msgs.c_str());
                success = false;
            }
        }

        hr = GenerateAdjacencyAndPointReps(s_unused16_all, 12, g_cubeVerts, 8, 0.f, preps.get(), adj.get());
        if (FAILED(hr))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [unused all] failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!IsValidPointReps(preps.get(), 8))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [unused all] invalid pointRep\n");
            success = false;
        }
        else
        {
            std::wstring msgs;
            if (FAILED(Validate(s_unused16_all, 12, 8, adj.get(), VALIDATE_UNUSED, &msgs)))
            {
                printe("ERROR: GenerateAdjacencyAndPointReps(16) cube [unused all] validate failed\n%ls\n", msgs.c_str());
                success = false;
            }
        }

        // epsilon is same as exact for cube with no overlapping verts
        hr = GenerateAdjacencyAndPointReps( g_cubeIndices16, 12, g_cubeVerts, 8, 0.0001f, preps.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [epsilon] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( preps.get(), s_cubePointReps, sizeof(s_cubePointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [epsilon] preps failed\n" );
            success = false;
            for( size_t j = 0; j < 8; ++j )
            {
                if ( preps[ j ] != s_cubePointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_cubePointReps[j] );
                }
            }
        }
        else if ( memcmp( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) cube [epsilon] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_cubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_cubeAdj[j] );
                }
            }
        }
    }

    // 16-bit adjacency (face-mapped cube)
    {
        std::unique_ptr<uint32_t[]> preps( new uint32_t[ 24 ] );
        memset( preps.get(), 0xff, sizeof(uint32_t) * 24 );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 3 * 12 ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) * 3 * 12 );

        HRESULT hr = GenerateAdjacencyAndPointReps( g_fmCubeIndices16, 12, g_fmCubeVerts, 24, 0.f, preps.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) fmcube [exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( preps.get(), s_fmCubePointReps, sizeof(s_fmCubePointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) fmcube [exact] preps failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                if ( preps[ j ] != s_fmCubePointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_fmCubePointReps[j] );
                }
            }
        }
        else if ( memcmp( adj.get(), s_fmCubeAdj, sizeof(s_fmCubeAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) fmcube [exact] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_fmCubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_fmCubeAdj[j] );
                }
            }
        }

        // nullptr preps
        hr = GenerateAdjacencyAndPointReps( g_fmCubeIndices16, 12, g_fmCubeVerts, 24, 0.f, nullptr, adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) fmcube [nullpr exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( adj.get(), s_fmCubeAdj, sizeof(s_fmCubeAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) fmcube [nullpr exact] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_fmCubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_fmCubeAdj[j] );
                }
            }
        }

        // epsilon picks different pointreps, but has same adjancecy
        hr = GenerateAdjacencyAndPointReps( g_fmCubeIndices16, 12, g_fmCubeVerts, 24, 0.0001f, preps.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) fmcube [epsilon] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( preps.get(), s_fmCubePointRepsEps, sizeof(s_fmCubePointRepsEps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) fmcube [epsilon] preps failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                if ( preps[ j ] != s_fmCubePointRepsEps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_fmCubePointRepsEps[j] );
                }
            }
        }
        else if ( memcmp( adj.get(), s_fmCubeAdj, sizeof(s_fmCubeAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) fmcube [epsilon] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_fmCubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_fmCubeAdj[j] );
                }
            }
        }
    }

    // 16-bit adjacency (tetra)
    {
        std::unique_ptr<uint32_t[]> preps( new uint32_t[ 4 ] );
        memset( preps.get(), 0xff, sizeof(uint32_t) * 4 );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 3 * 4 ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) * 3 * 4 );

        HRESULT hr = GenerateAdjacencyAndPointReps( g_tetraIndices16, 4, g_tetraVerts, 4, 0.f, preps.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) tetra [exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( preps.get(), s_tetraPointReps, sizeof(s_tetraPointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) tetra [exact] preps failed\n" );
            success = false;
            for( size_t j = 0; j < 4; ++j )
            {
                if ( preps[ j ] != s_tetraPointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_tetraPointReps[j] );
                }
            }
        }
        else if ( memcmp( adj.get(), s_tetraAdj, sizeof(s_tetraAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) tetra [exact] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 4); ++j )
            {
                if ( adj[ j ] != s_tetraAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_tetraAdj[j] );
                }
            }
        }

        // nullptr preps
        hr = GenerateAdjacencyAndPointReps( g_tetraIndices16, 4, g_tetraVerts, 4, 0.f, nullptr, adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) tetra [nullpr exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( adj.get(), s_tetraAdj, sizeof(s_tetraAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) tetra [nullpr exact] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 4); ++j )
            {
                if ( adj[ j ] != s_tetraAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_tetraAdj[j] );
                }
            }
        }

        // epsilon is same as exact for tetra with no overlapping verts
        hr = GenerateAdjacencyAndPointReps( g_tetraIndices16, 4, g_tetraVerts, 4, 0.0001f, preps.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) tetra [epsilon] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( preps.get(), s_tetraPointReps, sizeof(s_tetraPointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) tetra [epsilon] preps failed\n" );
            success = false;
            for( size_t j = 0; j < 4; ++j )
            {
                if ( preps[ j ] != s_tetraPointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_tetraPointReps[j] );
                }
            }
        }
        else if ( memcmp( adj.get(), s_tetraAdj, sizeof(s_tetraAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(16) tetra [epsilon] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 4); ++j )
            {
                if ( adj[ j ] != s_tetraAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_tetraAdj[j] );
                }
            }
        }
    }

    // 32-bit adjacency (cube)
    {
        std::unique_ptr<uint32_t[]> preps( new uint32_t[ 8 ] );
        memset( preps.get(), 0xff, sizeof(uint32_t) * 8 );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 3 * 12 ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) * 3 * 12 );

        HRESULT hr = GenerateAdjacencyAndPointReps( g_cubeIndices32, 12, g_cubeVerts, 8, 0.f, preps.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( preps.get(), s_cubePointReps, sizeof(s_cubePointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [exact] preps failed\n" );
            success = false;
            for( size_t j = 0; j < 8; ++j )
            {
                if ( preps[ j ] != s_cubePointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_cubePointReps[j] );
                }
            }
        }
        else if ( memcmp( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [exact] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_cubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_cubeAdj[j] );
                }
            }
        }

        // nullptr preps
        hr = GenerateAdjacencyAndPointReps( g_cubeIndices32, 12, g_cubeVerts, 8, 0.f, nullptr, adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [nullpr exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [nullpr exact] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_cubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_cubeAdj[j] );
                }
            }
        }

        // unused
        hr = GenerateAdjacencyAndPointReps(s_unused32_1st, 12, g_cubeVerts, 8, 0.f, preps.get(), adj.get());
        if (FAILED(hr))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [unused 1st] failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!IsValidPointReps(preps.get(), 8))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [unused 1st] invalid pointRep\n");
            success = false;
        }
        else
        {
            std::wstring msgs;
            if (FAILED(Validate(s_unused32_1st, 12, 8, adj.get(), VALIDATE_UNUSED, &msgs)))
            {
                printe("ERROR: GenerateAdjacencyAndPointReps(32) cube [unused 1st] validate failed\n%ls\n", msgs.c_str());
                success = false;
            }
        }

        hr = GenerateAdjacencyAndPointReps(s_unused32_all, 12, g_cubeVerts, 8, 0.f, preps.get(), adj.get());
        if (FAILED(hr))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [unused all] failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!IsValidPointReps(preps.get(), 8))
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [unused all] invalid pointRep\n");
            success = false;
        }
        else
        {
            std::wstring msgs;
            if (FAILED(Validate(s_unused32_all, 12, 8, adj.get(), VALIDATE_UNUSED, &msgs)))
            {
                printe("ERROR: GenerateAdjacencyAndPointReps(32) cube [unused all] validate failed\n%ls\n", msgs.c_str());
                success = false;
            }
        }

        // epsilon is same as exact for cube with no overlapping verts
        hr = GenerateAdjacencyAndPointReps( g_cubeIndices32, 12, g_cubeVerts, 8, 0.0001f, preps.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [epsilon] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( preps.get(), s_cubePointReps, sizeof(s_cubePointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [epsilon] preps failed\n" );
            success = false;
            for( size_t j = 0; j < 8; ++j )
            {
                if ( preps[ j ] != s_cubePointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_cubePointReps[j] );
                }
            }
        }
        else if ( memcmp( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) cube [epsilon] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_cubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_cubeAdj[j] );
                }
            }
        }
    }

    // 32-bit adjacency (face-mapped cube)
    {
        std::unique_ptr<uint32_t[]> preps( new uint32_t[ 24 ] );
        memset( preps.get(), 0xff, sizeof(uint32_t) * 24 );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 3 * 12 ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) * 3 * 12 );

        HRESULT hr = GenerateAdjacencyAndPointReps( g_fmCubeIndices32, 12, g_fmCubeVerts, 24, 0.f, preps.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) fmcube [exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( preps.get(), s_fmCubePointReps, sizeof(s_fmCubePointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) fmcube [exact] preps failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                if ( preps[ j ] != s_fmCubePointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_fmCubePointReps[j] );
                }
            }
        }
        else if ( memcmp( adj.get(), s_fmCubeAdj, sizeof(s_fmCubeAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) fmcube [exact] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_fmCubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_fmCubeAdj[j] );
                }
            }
        }

        // nullptr preps
        hr = GenerateAdjacencyAndPointReps( g_fmCubeIndices32, 12, g_fmCubeVerts, 24, 0.f, nullptr, adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) fmcube [nullpr exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( adj.get(), s_fmCubeAdj, sizeof(s_fmCubeAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) fmcube [nullpr exact] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_fmCubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_fmCubeAdj[j] );
                }
            }
        }

        // epsilon picks different pointreps, but has same adjancecy
        hr = GenerateAdjacencyAndPointReps( g_fmCubeIndices32, 12, g_fmCubeVerts, 24, 0.0001f, preps.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) fmcube [epsilon] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( preps.get(), s_fmCubePointRepsEps, sizeof(s_fmCubePointRepsEps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) fmcube [epsilon] preps failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                if ( preps[ j ] != s_fmCubePointRepsEps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_fmCubePointRepsEps[j] );
                }
            }
        }
        else if ( memcmp( adj.get(), s_fmCubeAdj, sizeof(s_fmCubeAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) fmcube [epsilon] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_fmCubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_fmCubeAdj[j] );
                }
            }
        }
    }

    // 32-bit adjacency (tetra)
    {
        std::unique_ptr<uint32_t[]> preps( new uint32_t[ 4 ] );
        memset( preps.get(), 0xff, sizeof(uint32_t) * 4 );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 3 * 4 ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) * 3 * 4 );

        HRESULT hr = GenerateAdjacencyAndPointReps( g_tetraIndices32, 4, g_tetraVerts, 4, 0.f, preps.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) tetra [exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( preps.get(), s_tetraPointReps, sizeof(s_tetraPointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) tetra [exact] preps failed\n" );
            success = false;
            for( size_t j = 0; j < 4; ++j )
            {
                if ( preps[ j ] != s_tetraPointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_tetraPointReps[j] );
                }
            }
        }
        else if ( memcmp( adj.get(), s_tetraAdj, sizeof(s_tetraAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) tetra [exact] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 4); ++j )
            {
                if ( adj[ j ] != s_tetraAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_tetraAdj[j] );
                }
            }
        }

        // nullptr preps
        hr = GenerateAdjacencyAndPointReps( g_tetraIndices32, 4, g_tetraVerts, 4, 0.f, nullptr, adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) tetra [nullpr exact] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( adj.get(), s_tetraAdj, sizeof(s_tetraAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) tetra [nullpr exact] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 4); ++j )
            {
                if ( adj[ j ] != s_tetraAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_tetraAdj[j] );
                }
            }
        }

        // epsilon is same as exact for tetra with no overlapping verts
        hr = GenerateAdjacencyAndPointReps( g_tetraIndices32, 4, g_tetraVerts, 4, 0.0001f, preps.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) tetra [epsilon] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( preps.get(), s_tetraPointReps, sizeof(s_tetraPointReps) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) tetra [epsilon] preps failed\n" );
            success = false;
            for( size_t j = 0; j < 4; ++j )
            {
                if ( preps[ j ] != s_tetraPointReps[j] )
                {
                    print("%zu: %u ... %u\n", j, preps[j], s_tetraPointReps[j] );
                }
            }
        }
        else if ( memcmp( adj.get(), s_tetraAdj, sizeof(s_tetraAdj) ) != 0 )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps(32) tetra [epsilon] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 4); ++j )
            {
                if ( adj[ j ] != s_tetraAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_tetraAdj[j] );
                }
            }
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// GenerateGSAdjacency
bool Test10()
{
    bool success = true;

    // 16-bit (cube)
    {
        std::unique_ptr<uint16_t[]> gsadj( new uint16_t[ 12 * 6 ] );
        memset( gsadj.get(), 0xff, sizeof(uint16_t) * 12 * 6 );

        HRESULT hr = GenerateGSAdjacency( g_cubeIndices16, 12, s_cubePointReps, s_cubeAdj, 8, gsadj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateGSAdjacency(16) cube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( gsadj.get(), s_cubeGSAdj16, sizeof(s_cubeGSAdj16) ) != 0 )
        {
            printe("\nERROR: GenerateGSAdjacency(16) cube failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12 * 2); ++j )
            {
                if ( gsadj[ j ] != s_cubeGSAdj16[j] )
                {
                    print("%zu: %d ... %d\n", j, gsadj[j], s_cubeGSAdj16[j] );
                }
            }
        }

        // invalid args
        hr = GenerateGSAdjacency(static_cast<const uint16_t *>(nullptr), 0, nullptr, nullptr, 0, nullptr);
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateGSAdjacency(16) expected failure for 0 count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = GenerateGSAdjacency(static_cast<const uint16_t *>(nullptr), 23, nullptr, nullptr, 52, nullptr);
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateGSAdjacency(16) expected failure for null parameter (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateGSAdjacency( g_cubeIndices16, 12, s_cubePointReps, s_cubeAdj, UINT16_MAX /*D3D11_16BIT_INDEX_STRIP_CUT_VALUE*/, gsadj.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateGSAdjacency(16) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateGSAdjacency( g_cubeIndices16, 12, s_cubePointReps, s_cubeAdj, UINT32_MAX, gsadj.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateGSAdjacency(16) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateGSAdjacency( g_cubeIndices16, UINT32_MAX, s_cubePointReps, s_cubeAdj, 8, gsadj.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: GenerateGSAdjacency(16) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateGSAdjacency( g_cubeIndices16, 12, s_cubePointReps, s_cubeAdj, 2, gsadj.get() );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: GenerateGSAdjacency(16) expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        memcpy(gsadj.get(), g_cubeIndices16, sizeof(g_cubeIndices16));
        hr = GenerateGSAdjacency( gsadj.get(), 12, s_cubePointReps, s_cubeAdj, 2, gsadj.get() );
        if ( hr != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED) )
        {
            printe("\nERROR: GenerateGSAdjacency(16) expected failure for in-place (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // 16-bit (face-mapped cube)
    {
        std::unique_ptr<uint16_t[]> gsadj( new uint16_t[ 12 * 6 ] );
        memset( gsadj.get(), 0xff, sizeof(uint16_t) * 12 * 6 );

        HRESULT hr = GenerateGSAdjacency( g_fmCubeIndices16, 12, s_fmCubePointReps, s_fmCubeAdj, 24, gsadj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateGSAdjacency(16) fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( gsadj.get(), s_fmCubeGSAdj16, sizeof(s_fmCubeGSAdj16) ) != 0 )
        {
            printe("\nERROR: GenerateGSAdjacency(16) fmcube failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12 * 2); ++j )
            {
                if ( gsadj[ j ] != s_fmCubeGSAdj16[j] )
                {
                    print("%zu: %d ... %d\n", j, gsadj[j], s_fmCubeGSAdj16[j] );
                }
            }
        }
    }

    // 16-bit (tetra)
    {
        std::unique_ptr<uint16_t[]> gsadj( new uint16_t[ 4 * 6 ] );
        memset( gsadj.get(), 0xff, sizeof(uint16_t) * 4 * 6 );

        HRESULT hr = GenerateGSAdjacency( g_tetraIndices16, 4, s_tetraPointReps, s_tetraAdj, 4, gsadj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateGSAdjacency(16) tetra failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( gsadj.get(), s_tetraGSAdj16, sizeof(s_tetraGSAdj16) ) != 0 )
        {
            printe("\nERROR: GenerateGSAdjacency(16) tetra failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 4 * 2); ++j )
            {
                if ( gsadj[ j ] != s_tetraGSAdj16[j] )
                {
                    print("%zu: %d ... %d\n", j, gsadj[j], s_tetraGSAdj16[j] );
                }
            }
        }
    }

    // 32-bit (cube)
    {
        std::unique_ptr<uint32_t[]> gsadj( new uint32_t[ 12 * 6 ] );
        memset( gsadj.get(), 0xff, sizeof(uint32_t) * 12 * 6 );

        HRESULT hr = GenerateGSAdjacency( g_cubeIndices32, 12, s_cubePointReps, s_cubeAdj, 8, gsadj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateGSAdjacency(32) cube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( gsadj.get(), s_cubeGSAdj32, sizeof(s_cubeGSAdj32) ) != 0 )
        {
            printe("\nERROR: GenerateGSAdjacency(32) cube failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12 * 2); ++j )
            {
                if ( gsadj[ j ] != s_cubeGSAdj32[j] )
                {
                    print("%zu: %u ... %u\n", j, gsadj[j], s_cubeGSAdj32[j] );
                }
            }
        }

        // invalid args
        hr = GenerateGSAdjacency(static_cast<const uint32_t *>(nullptr), 0, nullptr, nullptr, 0, nullptr);
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateGSAdjacency(32) expected failure for 0 count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = GenerateGSAdjacency(static_cast<const uint32_t *>(nullptr), 23, nullptr, nullptr, 52, nullptr);
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateGSAdjacency(32) expected failure for null parameter (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateGSAdjacency( g_cubeIndices32, 12, s_cubePointReps, s_cubeAdj, UINT32_MAX /*D3D11_32BIT_INDEX_STRIP_CUT_VALUE*/, gsadj.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateGSAdjacency(32) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateGSAdjacency( g_cubeIndices32, 12, s_cubePointReps, s_cubeAdj, UINT32_MAX, gsadj.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: GenerateGSAdjacency(32) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateGSAdjacency( g_cubeIndices32, UINT32_MAX, s_cubePointReps, s_cubeAdj, 8, gsadj.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: GenerateGSAdjacency(32) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = GenerateGSAdjacency( g_cubeIndices32, 12, s_cubePointReps, s_cubeAdj, 2, gsadj.get() );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: GenerateGSAdjacency(32) expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        memcpy(gsadj.get(), g_cubeIndices32, sizeof(g_cubeIndices32));
        hr = GenerateGSAdjacency( gsadj.get(), 12, s_cubePointReps, s_cubeAdj, 2, gsadj.get() );
        if ( hr != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED) )
        {
            printe("\nERROR: GenerateGSAdjacency(32) expected failure for in-place (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // 32-bit (face-mapped cube)
    {
        std::unique_ptr<uint32_t[]> gsadj( new uint32_t[ 12 * 6 ] );
        memset( gsadj.get(), 0xff, sizeof(uint32_t) * 12 * 6 );

        HRESULT hr = GenerateGSAdjacency( g_fmCubeIndices32, 12, s_fmCubePointReps, s_fmCubeAdj, 24, gsadj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateGSAdjacency(32) fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( gsadj.get(), s_fmCubeGSAdj32, sizeof(s_fmCubeGSAdj32) ) != 0 )
        {
            printe("\nERROR: GenerateGSAdjacency(32) fmcube failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12 * 2); ++j )
            {
                if ( gsadj[ j ] != s_fmCubeGSAdj32[j] )
                {
                    print("%zu: %u ... %u\n", j, gsadj[j], s_fmCubeGSAdj32[j] );
                }
            }
        }
    }

    // 32-bit (tetra)
    {
        std::unique_ptr<uint32_t[]> gsadj( new uint32_t[ 4 * 6 ] );
        memset( gsadj.get(), 0xff, sizeof(uint32_t) * 4 * 6 );

        HRESULT hr = GenerateGSAdjacency( g_tetraIndices32, 4, s_tetraPointReps, s_tetraAdj, 4, gsadj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateGSAdjacency(32) tetra failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( gsadj.get(), s_tetraGSAdj32, sizeof(s_tetraGSAdj32) ) != 0 )
        {
            printe("\nERROR: GenerateGSAdjacency(32) tetra failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 4 * 2); ++j )
            {
                if ( gsadj[ j ] != s_tetraGSAdj32[j] )
                {
                    print("%zu: %u ... %u\n", j, gsadj[j], s_tetraGSAdj32[j] );
                }
            }
        }
    }


    // Unused (16)
    {
        std::unique_ptr<uint16_t[]> gsadj( new uint16_t[ 12 * 6 ] );
        memset( gsadj.get(), 0xff, sizeof(uint16_t) * 12 * 6 );

        static const uint16_t s_unusedGSAdj[3 * 12 * 2] =
        {
            0, 3, 1, 0, 2, 7,
            0, 4, 3, 6, 1, 2,
            0, 5, 4, 6, 3, 1,
            0, 7, 5, 6, 4, 3,
            3, 4, 6, 3, 1, 0,
            3, 0, 4, 5, 6, 1,
            uint16_t(-1), uint16_t(-1), uint16_t(-1), uint16_t(-1), uint16_t(-1), uint16_t(-1),
            2, 7, 6, 5, 7, 0,
            0, 1, 2, 6, 7, 5,
            0, 2, 7, 6, 5, 4,
            5, 0, 7, 2, 6, 4,
            5, 7, 6, 3, 4, 0
        };

#ifdef _DEBUG
        std::wstring msgs;
        if ( FAILED( Validate( s_unused16, 12, 8, s_unusedAdj, VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs ) ) )
        {
            success = false;
            printe("\nERROR: GenerateGSAdjacency(16) test data failed validation:\n%ls\n", msgs.c_str() );
        }
#endif

        HRESULT hr = GenerateGSAdjacency( s_unused16, 12, s_unusedPR, s_unusedAdj, 8, gsadj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateGSAdjacency(16) unused failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( gsadj.get(), s_unusedGSAdj, sizeof(s_unusedGSAdj) ) != 0 )
        {
            printe("\nERROR: GenerateGSAdjacency(16) unused failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12 * 2); ++j )
            {
                if ( gsadj[ j ] != s_unusedGSAdj[j] )
                {
                    print("%zu: %d ... %d\n", j, gsadj[j], s_unusedGSAdj[j] );
                }
            }
        }
    }

    // Unused (32)
    {
        std::unique_ptr<uint32_t[]> gsadj( new uint32_t[ 12 * 6 ] );
        memset( gsadj.get(), 0xff, sizeof(uint32_t) * 12 * 6 );

        static const uint32_t s_unusedGSAdj[3 * 12 * 2] =
        {
            0, 3, 1, 0, 2, 7,
            0, 4, 3, 6, 1, 2,
            0, 5, 4, 6, 3, 1,
            0, 7, 5, 6, 4, 3,
            3, 4, 6, 3, 1, 0,
            3, 0, 4, 5, 6, 1,
            uint32_t(-1), uint32_t(-1), uint32_t(-1), uint32_t(-1), uint32_t(-1), uint32_t(-1),
            2, 7, 6, 5, 7, 0,
            0, 1, 2, 6, 7, 5,
            0, 2, 7, 6, 5, 4,
            5, 0, 7, 2, 6, 4,
            5, 7, 6, 3, 4, 0
        };

#ifdef _DEBUG
        std::wstring msgs;
        if ( FAILED( Validate( s_unused32, 12, 8, s_unusedAdj, VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs ) ) )
        {
            success = false;
            printe("\nERROR: GenerateGSAdjacency(32) test data failed validation:\n%ls\n", msgs.c_str() );
        }
#endif

        HRESULT hr = GenerateGSAdjacency( s_unused32, 12, s_unusedPR, s_unusedAdj, 8, gsadj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: GenerateGSAdjacency(32) unused failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( gsadj.get(), s_unusedGSAdj, sizeof(s_unusedGSAdj) ) != 0 )
        {
            printe("\nERROR: GenerateGSAdjacency(32) unused failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12 * 2); ++j )
            {
                if ( gsadj[ j ] != s_unusedGSAdj[j] )
                {
                    print("%zu: %u ... %u\n", j, gsadj[j], s_unusedGSAdj[j] );
                }
            }
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// ConvertPointRepsToAdjacency
bool Test12()
{
    bool success = true;

    // 16-bit (cube)
    {
        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 3 * 12 ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) * 3 * 12 );

        HRESULT hr = ConvertPointRepsToAdjacency( g_cubeIndices16, 12, g_cubeVerts, 8, s_cubePointReps, adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(16) cube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) ) != 0 )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(16) cube failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_cubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_cubeAdj[j] );
                }
            }
        }

        // identity preps
        hr = ConvertPointRepsToAdjacency( g_cubeIndices16, 12, g_cubeVerts, 8, nullptr, adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(16) cube [identity] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) ) != 0 )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(16) cube [identity] failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_cubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_cubeAdj[j] );
                }
            }
        }

        // invalid args
        hr = ConvertPointRepsToAdjacency(static_cast<const uint16_t*>(nullptr), 0, nullptr, 0, nullptr, nullptr);
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(16) expected failure for 0 count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = ConvertPointRepsToAdjacency(static_cast<const uint16_t*>(nullptr), 23, nullptr, 52, nullptr, nullptr);
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(16) expected failure for null parameter (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ConvertPointRepsToAdjacency( g_cubeIndices16, 12, g_cubeVerts, UINT16_MAX /*D3D11_16BIT_INDEX_STRIP_CUT_VALUE*/, nullptr, adj.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(16) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ConvertPointRepsToAdjacency( g_cubeIndices16, 12, g_cubeVerts, UINT32_MAX, nullptr, adj.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(16) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ConvertPointRepsToAdjacency( g_cubeIndices16, UINT32_MAX, g_cubeVerts, 8, nullptr, adj.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(16) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ConvertPointRepsToAdjacency( g_cubeIndices16, 12, g_cubeVerts, 2, nullptr, adj.get() );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(16) expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // 16-bit (fmcube)
    {
        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 3 * 12 ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) * 3 * 12 );

        HRESULT hr = ConvertPointRepsToAdjacency( g_fmCubeIndices16, 12, g_fmCubeVerts, 24, s_fmCubePointReps, adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(16) fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( adj.get(), s_fmCubeAdj, sizeof(s_fmCubeAdj) ) != 0 )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(16) fmcube adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_fmCubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_fmCubeAdj[j] );
                }
            }
        }
    }

    // 32-bit (cube)
    {
        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 3 * 12 ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) * 3 * 12 );

        HRESULT hr = ConvertPointRepsToAdjacency( g_cubeIndices32, 12, g_cubeVerts, 8, s_cubePointReps, adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(32) cube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) ) != 0 )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(32) cube failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_cubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_cubeAdj[j] );
                }
            }
        }

        // identity preps
        hr = ConvertPointRepsToAdjacency( g_cubeIndices32, 12, g_cubeVerts, 8, nullptr, adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(32) cube [identity] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) ) != 0 )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(32) cube [identity] failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_cubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_cubeAdj[j] );
                }
            }
        }

        // invalid args
        hr = ConvertPointRepsToAdjacency(static_cast<const uint32_t*>(nullptr), 0, nullptr, 0, nullptr, nullptr);
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(32) expected failure for 0 count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = ConvertPointRepsToAdjacency(static_cast<const uint32_t*>(nullptr), 23, nullptr, 52, nullptr, nullptr);
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(32) expected failure for null parameter (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ConvertPointRepsToAdjacency( g_cubeIndices32, 12, g_cubeVerts, UINT32_MAX /*D3D11_32BIT_INDEX_STRIP_CUT_VALUE*/, nullptr, adj.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(32) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ConvertPointRepsToAdjacency( g_cubeIndices32, 12, g_cubeVerts, UINT32_MAX, nullptr, adj.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(32) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ConvertPointRepsToAdjacency( g_cubeIndices32, UINT32_MAX, g_cubeVerts, 8, nullptr, adj.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(32) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ConvertPointRepsToAdjacency( g_cubeIndices32, 12, g_cubeVerts, 2, nullptr, adj.get() );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(32) expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // 32-bit (fmcube)
    {
        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 3 * 12 ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) * 3 * 12 );

        HRESULT hr = ConvertPointRepsToAdjacency( g_fmCubeIndices32, 12, g_fmCubeVerts, 24, s_fmCubePointReps, adj.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(32) fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( adj.get(), s_fmCubeAdj, sizeof(s_fmCubeAdj) ) != 0 )
        {
            printe("\nERROR: ConvertPointRepsToAdjacency(32) fmcube adj failed\n" );
            success = false;
            for( size_t j = 0; j < (3 * 12); ++j )
            {
                if ( adj[ j ] != s_fmCubeAdj[j] )
                {
                    print("%zu (%zu/%zu): %u ... %u\n", j, (j/3), (j % 3), adj[j], s_fmCubeAdj[j] );
                }
            }
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// ReorderIBAndAdjacency
bool Test23()
{
    bool success = true;

    // Cube (16)
    {
        std::unique_ptr<uint16_t[]> destib( new uint16_t[ 12 * 3 ] );
        memset( destib.get(), 0xff, sizeof(uint16_t) * 12 * 3 );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 3 * 12 ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) * 3 * 12 );

        static const uint32_t s_faceRemap[] = { 4, 6, 10, 11, 1, 3, 8, 9, 2, 5, 0, 7 };

        static const uint16_t s_sorted[] =
        {
            3, 6, 1,
            2, 1, 6,
            5, 7, 6,
            5, 6, 4,
            0, 3, 1,
            0, 5, 4,
            0, 2, 7,
            0, 7, 5,
            0, 4, 3,
            3, 4, 6,
            0, 1, 2,
            2, 6, 7
        };

        static const uint32_t s_sortedAdj[] =
        {
            5, 6, 1,
            0, 4, 7,
            9, 7, 11,
            10, 5, 3,
            2, 4, 0,
            9, 11, 2,
            0, 7, 9,
            8, 10, 3,
            3, 5, 1,
            2, 11, 4,
            1, 6, 8,
            6, 10, 8,
        };

        HRESULT hr = ReorderIBAndAdjacency( g_cubeIndices16, 12, s_cubeAdj, s_faceRemap, destib.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIBAndAdjacency(16) cube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: ReorderIBAndAdjacency(16) cube ib failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %d %d %d\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );
            }
        }
        else if ( memcmp( adj.get(), s_sortedAdj, sizeof(s_sortedAdj) ) != 0 )
        {
            printe("ERROR: ReorderIBAndAdjacency(16) cube adj failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %u %u %u\n", j, adj[ j ], adj[ j + 1 ], adj[ j + 2 ] );
            }
        }

        // in-place
        memcpy( destib.get(), g_cubeIndices16, sizeof(g_cubeIndices16) );
        memcpy( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) );

        hr = ReorderIBAndAdjacency( destib.get(), 12, adj.get(), s_faceRemap );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIBAndAdjacency(16) cube [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: ReorderIBAndAdjacency(16) cube [in-place] ib failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %d %d %d\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );
            }
        }
        else if ( memcmp( adj.get(), s_sortedAdj, sizeof(s_sortedAdj) ) != 0 )
        {
            printe("ERROR: ReorderIBAndAdjacency(16) cube [in-place] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %u %u %u\n", j, adj[ j ], adj[ j + 1 ], adj[ j + 2 ] );
            }
        }

        // invalid args
        hr = ReorderIBAndAdjacency(static_cast<const uint16_t*>(nullptr), 0, nullptr, nullptr, nullptr, nullptr);
        if (hr != E_INVALIDARG)
        {
            printe("\nERROR: ReorderIBAndAdjacency(16) expected to fail for 0 count (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = ReorderIBAndAdjacency(static_cast<const uint16_t*>(nullptr), 23, nullptr, nullptr, nullptr, nullptr);
        if (hr != E_INVALIDARG)
        {
            printe("\nERROR: ReorderIBAndAdjacency(16) expected to fail for null parameter (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = ReorderIBAndAdjacency( destib.get(), 12, s_cubeAdj, s_faceRemap, destib.get(), adj.get() );
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
        {
            printe("\nERROR: ReorderIBAndAdjacency(16) expected to fail for inib==out (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = ReorderIBAndAdjacency( g_cubeIndices16, 12, adj.get(), s_faceRemap, destib.get(), adj.get() );
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
        {
            printe("\nERROR: ReorderIBAndAdjacency(16) expected to fail for inadj==out (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = ReorderIBAndAdjacency( g_cubeIndices16, 12, s_cubeAdj, nullptr, destib.get(), adj.get() );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: ReorderIBAndAdjacency(16) nullptr remap expected failure\n" );
            success = false;
        }

        hr = ReorderIBAndAdjacency( g_cubeIndices16, UINT32_MAX, s_cubeAdj, s_faceRemap, destib.get(), adj.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: ReorderIBAndAdjacency(16) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        memcpy( destib.get(), g_cubeIndices16, sizeof(g_cubeIndices16) );
        memcpy( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) );

        hr = ReorderIBAndAdjacency( destib.get(), 12, adj.get(), nullptr );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: ReorderIBAndAdjacency(16) [in-place] nullptr remap expected failure\n" );
            success = false;
        }

        hr = ReorderIBAndAdjacency( destib.get(), UINT32_MAX, adj.get(), s_faceRemap );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: ReorderIBAndAdjacency(16) [in-place] expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // Cube (32)
    {
        std::unique_ptr<uint32_t[]> destib( new uint32_t[ 12 * 3 ] );
        memset( destib.get(), 0xff, sizeof(uint32_t) * 12 * 3 );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 3 * 12 ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) * 3 * 12 );

        static const uint32_t s_faceRemap[] = { 4, 6, 10, 11, 1, 3, 8, 9, 2, 5, 0, 7 };

        static const uint32_t s_sorted[] =
        {
            3, 6, 1,
            2, 1, 6,
            5, 7, 6,
            5, 6, 4,
            0, 3, 1,
            0, 5, 4,
            0, 2, 7,
            0, 7, 5,
            0, 4, 3,
            3, 4, 6,
            0, 1, 2,
            2, 6, 7
        };

        static const uint32_t s_sortedAdj[] =
        {
            5, 6, 1,
            0, 4, 7,
            9, 7, 11,
            10, 5, 3,
            2, 4, 0,
            9, 11, 2,
            0, 7, 9,
            8, 10, 3,
            3, 5, 1,
            2, 11, 4,
            1, 6, 8,
            6, 10, 8,
        };

        HRESULT hr = ReorderIBAndAdjacency( g_cubeIndices32, 12, s_cubeAdj, s_faceRemap, destib.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIBAndAdjacency(32) cube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: ReorderIBAndAdjacency(32) cube ib failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );
            }
        }
        else if ( memcmp( adj.get(), s_sortedAdj, sizeof(s_sortedAdj) ) != 0 )
        {
            printe("ERROR: ReorderIBAndAdjacency(32) cube adj failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %u %u %u\n", j, adj[ j ], adj[ j + 1 ], adj[ j + 2 ] );
            }
        }

        // in-place
        memcpy( destib.get(), g_cubeIndices32, sizeof(g_cubeIndices32) );
        memcpy( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) );

        hr = ReorderIBAndAdjacency( destib.get(), 12, adj.get(), s_faceRemap );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIBAndAdjacency(32) cube [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: ReorderIBAndAdjacency(32) cube [in-place] ib failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );
            }
        }
        else if ( memcmp( adj.get(), s_sortedAdj, sizeof(s_sortedAdj) ) != 0 )
        {
            printe("ERROR: ReorderIBAndAdjacency(32) cube [in-place] adj failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %u %u %u\n", j, adj[ j ], adj[ j + 1 ], adj[ j + 2 ] );
            }
        }

        // invalid args
       hr = ReorderIBAndAdjacency(static_cast<const uint32_t*>(nullptr), 0, nullptr, nullptr, nullptr, nullptr);
        if (hr != E_INVALIDARG)
        {
            printe("\nERROR: ReorderIBAndAdjacency(32) expected to fail for 0 count (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = ReorderIBAndAdjacency(static_cast<const uint32_t*>(nullptr), 23, nullptr, nullptr, nullptr, nullptr);
        if (hr != E_INVALIDARG)
        {
            printe("\nERROR: ReorderIBAndAdjacency(32) expected to fail for null parameter (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = ReorderIBAndAdjacency( destib.get(), 12, s_cubeAdj, s_faceRemap, destib.get(), adj.get() );
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
        {
            printe("\nERROR: ReorderIBAndAdjacency(32) expected to fail for inib==out (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = ReorderIBAndAdjacency( g_cubeIndices32, 12, adj.get(), s_faceRemap, destib.get(), adj.get() );
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
        {
            printe("\nERROR: ReorderIBAndAdjacency(32) expected to fail for inadj==out (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = ReorderIBAndAdjacency( g_cubeIndices32, 12, s_cubeAdj, nullptr, destib.get(), adj.get() );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: ReorderIBAndAdjacency(32) nullptr remap expected failure\n" );
            success = false;
        }

        hr = ReorderIBAndAdjacency( g_cubeIndices32, UINT32_MAX, s_cubeAdj, s_faceRemap, destib.get(), adj.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: ReorderIBAndAdjacency(32) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        memcpy( destib.get(), g_cubeIndices32, sizeof(g_cubeIndices32) );
        memcpy( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) );

        hr = ReorderIBAndAdjacency( destib.get(), 12, adj.get(), nullptr );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: ReorderIBAndAdjacency(32) [in-place] nullptr remap expected failure\n" );
            success = false;
        }

        hr = ReorderIBAndAdjacency( destib.get(), UINT32_MAX, adj.get(), s_faceRemap );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: ReorderIBAndAdjacency(32) [in-place] expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // Unused (16)
    {
        std::unique_ptr<uint16_t[]> destib( new uint16_t[ 12 * 3 ] );
        memset( destib.get(), 0xff, sizeof(uint16_t) * 12 * 3 );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 3 * 12 ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) * 3 * 12 );

        static const uint32_t s_faceRemap[] = { 4, 6, 10, uint32_t(-1), 1, 3, 8, 9, 2, 5, 0, 7 };

        static const uint16_t s_sorted[] =
        {
            3, 6, 1,
            2, 1, 6,
            5, 7, 6,
            uint16_t(-1), uint16_t(-1), uint16_t(-1),
            0, 3, 1,
            0, 5, 4,
            0, 2, 7,
            0, 7, 5,
            0, 4, 3,
            3, 4, 6,
            0, 1, 2,
            2, 6, 7
        };

        static const uint32_t s_sortedAdj[] =
        {
            5, 6, 1,
            0, 4, 7,
            9, 7, 11,
            uint32_t(-1), uint32_t(-1), uint32_t(-1),
            2, 4, 0,
            9, 11, 2,
            0, 7, 9,
            8, 10, 3,
            3, 5, 1,
            2, 11, 4,
            1, 6, 8,
            6, 10, 8,
        };

        HRESULT hr = ReorderIBAndAdjacency( g_cubeIndices16, 12, s_cubeAdj, s_faceRemap, destib.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIBAndAdjacency(16) unused faces failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: ReorderIBAndAdjacency(16) unused faces ib failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %d %d %d\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );
            }
        }
        else if ( memcmp( adj.get(), s_sortedAdj, sizeof(s_sortedAdj) ) != 0 )
        {
            printe("ERROR: ReorderIBAndAdjacency(16) unused faces adj failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %u %u %u\n", j, adj[ j ], adj[ j + 1 ], adj[ j + 2 ] );
            }
        }
    }

    // Unused (32)
    {
        std::unique_ptr<uint32_t[]> destib( new uint32_t[ 12 * 3 ] );
        memset( destib.get(), 0xff, sizeof(uint32_t) * 12 * 3 );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 3 * 12 ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) * 3 * 12 );

        static const uint32_t s_faceRemap[] = { 4, 6, 10, uint32_t(-1), 1, 3, 8, 9, 2, 5, 0, 7 };

        static const uint32_t s_sorted[] =
        {
            3, 6, 1,
            2, 1, 6,
            5, 7, 6,
            uint32_t(-1), uint32_t(-1), uint32_t(-1),
            0, 3, 1,
            0, 5, 4,
            0, 2, 7,
            0, 7, 5,
            0, 4, 3,
            3, 4, 6,
            0, 1, 2,
            2, 6, 7
        };

        static const uint32_t s_sortedAdj[] =
        {
            5, 6, 1,
            0, 4, 7,
            9, 7, 11,
            uint32_t(-1), uint32_t(-1), uint32_t(-1),
            2, 4, 0,
            9, 11, 2,
            0, 7, 9,
            8, 10, 3,
            3, 5, 1,
            2, 11, 4,
            1, 6, 8,
            6, 10, 8,
        };

        HRESULT hr = ReorderIBAndAdjacency( g_cubeIndices32, 12, s_cubeAdj, s_faceRemap, destib.get(), adj.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIBAndAdjacency(32) unused faces failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: ReorderIBAndAdjacency(32) unused faces ib failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );
            }
        }
        else if ( memcmp( adj.get(), s_sortedAdj, sizeof(s_sortedAdj) ) != 0 )
        {
            printe("ERROR: ReorderIBAndAdjacency(32) unused faces adj failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %u %u %u\n", j, adj[ j ], adj[ j + 1 ], adj[ j + 2 ] );
            }
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// FinalizeVBAndPointReps (no duplicates)
bool Test19()
{
    bool success = true;

    // Cube identity
    {
        std::unique_ptr<uint32_t[]> preps( new uint32_t[ 8 ] );
        memset( preps.get(), 0xff, sizeof(uint32_t) * 8 );

        std::unique_ptr<uint32_t[]> srcvb( new uint32_t[ 8 ] );
        for( uint32_t j=0; j < 8; ++j)
            srcvb[ j ] = j;

        std::unique_ptr<uint32_t[]> destvb( new uint32_t[ 8 ] );
        memset( destvb.get(), 0xff, sizeof(uint32_t) * 8 );

        std::vector<uint32_t> remap;
        remap.reserve( 8 );
        for( uint32_t j = 0; j < 8; ++j )
            remap.push_back( j );

        HRESULT hr = FinalizeVBAndPointReps( srcvb.get(), sizeof(uint32_t), 8, s_cubePointReps, nullptr, 0, remap.data(),
                                             destvb.get(), preps.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: FinalizeVBAndPointReps cube identity failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 8; ++j )
            {
                if ( preps[ j ] != remap[ s_cubePointReps[ j ] ] )
                {
                    printe("\nERROR: FinalizeVBAndPointReps cube identify pr failed\n" );
                    success = false;
                    break;
                }
                if ( srcvb[ j ] != remap[ destvb[ j ] ] )
                {
                    printe("\nERROR: FinalizeVBAndPointReps cube identify vb failed\n" );
                    success = false;
                    break;
                }
            }
        }

        // in place remap
        for( uint32_t j=0; j < 8; ++j)
            srcvb[ j ] = j;

        memcpy( preps.get(), s_cubePointReps, sizeof(uint32_t) * 8 );

        hr = FinalizeVBAndPointReps( srcvb.get(), sizeof(uint32_t), 8, preps.get(), remap.data() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: FinalizeVBAndPointReps cube identity [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 8; ++j )
            {
                if ( preps[ j ] != remap[ s_cubePointReps[ j ] ] )
                {
                    printe("\nERROR: FinalizePointReps cube identify [in-place] pr failed\n" );
                    success = false;
                    break;
                }
                if ( destvb[ j ] != j )
                {
                    printe("\nERROR: FinalizePointReps cube identify [in-place] vb failed\n" );
                    success = false;
                    break;
                }
            }
        }

        // invalid args
        hr = FinalizeVBAndPointReps(nullptr, 0, 0, nullptr, nullptr, 0, nullptr, nullptr, nullptr);
        if (hr != E_INVALIDARG)
        {
            printe("\nERROR: FinalizeVBAndPointReps expected to fail for 0 count (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = FinalizeVBAndPointReps(nullptr, sizeof(uint32_t), 23, nullptr, nullptr, 52, nullptr, nullptr, nullptr);
        if (hr != E_INVALIDARG)
        {
            printe("\nERROR: FinalizeVBAndPointReps expected to fail for null parameter (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = FinalizeVBAndPointReps(srcvb.get(), sizeof(uint32_t), 8, s_cubePointReps, nullptr, 0, remap.data(), srcvb.get(), preps.get());
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
        {
            printe("\nERROR: FinalizeVBAndPointReps expected to fail for in=out (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = FinalizeVBAndPointReps( srcvb.get(), sizeof(uint32_t), 8, s_cubePointReps, nullptr, 0, nullptr, destvb.get(), preps.get() );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: FinalizeVBAndPointReps nullptr remap expected failure\n" );
            success = false;
        }

        hr = FinalizeVBAndPointReps( srcvb.get(), UINT32_MAX, 8, s_cubePointReps, nullptr, 0, remap.data(), destvb.get(), preps.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeVBAndPointReps expected failure for bad stride value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeVBAndPointReps( srcvb.get(), sizeof(uint32_t), UINT32_MAX /*D3D11_32BIT_INDEX_STRIP_CUT_VALUE*/, s_cubePointReps, nullptr, 0, remap.data(), destvb.get(), preps.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeVBAndPointReps expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeVBAndPointReps( srcvb.get(), sizeof(uint32_t), UINT32_MAX, s_cubePointReps, nullptr, 0, remap.data(), destvb.get(), preps.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeVBAndPointReps expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        for( uint32_t j=0; j < 8; ++j)
            srcvb[ j ] = j;

        memcpy( preps.get(), s_cubePointReps, sizeof(uint32_t) * 8 );

        hr = FinalizeVBAndPointReps( srcvb.get(), sizeof(uint32_t), 8, preps.get(), nullptr );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: FinalizeVBAndPointReps [in-place] nullptr remap expected failure\n" );
            success = false;
        }

        hr = FinalizeVBAndPointReps( srcvb.get(), UINT32_MAX, 8, preps.get(), remap.data() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeVBAndPointReps [in-place] expected failure for bad stride value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeVBAndPointReps( srcvb.get(), sizeof(uint32_t), UINT32_MAX /*D3D11_32BIT_INDEX_STRIP_CUT_VALUE*/, preps.get(), remap.data() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeVBAndPointReps [in-place] expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeVBAndPointReps( srcvb.get(), sizeof(uint32_t), UINT32_MAX, preps.get(), remap.data() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeVBAndPointReps [in-place] expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // Face-mapped Cube
    {
        using namespace TestGeometry;

        std::unique_ptr<uint32_t[]> prout( new uint32_t[ 24 ] );
        memset( prout.get(), 0xff, sizeof(uint32_t) * 24 );

        std::unique_ptr<uint32_t[]> srcvb( new uint32_t[ 24 ] );
        for( uint32_t j=0; j < 24; ++j)
        {
            srcvb[ j ] = j;
        }

        std::unique_ptr<uint32_t[]> destvb( new uint32_t[ 24 ] );
        memset( destvb.get(), 0xff, sizeof(uint32_t) * 24 );

        static const uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, 13, 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23 };

        static const uint32_t s_preps[] = { 0, 1, 2, 3, 4, 5,
                                            6, 7, 0, 5, 7, 2,
                                            1, 4, 6, 3, 2, 6,
                                            5, 1, 3, 7, 4, 0 };

        HRESULT hr = FinalizeVBAndPointReps( srcvb.get(), sizeof(uint32_t), 24, s_fmCubePointReps, nullptr, 0, s_remap, destvb.get(), prout.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVBAndPointReps fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destvb.get(), s_remap, sizeof(s_remap) ) != 0 )
        {
            printe("ERROR: FinalizeVBAndPointReps fmcube vb failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                printe("\t%zu: %u .. %u\n", j, destvb[ j ], s_remap[ j ] );
            }
        }
        else if ( memcmp( prout.get(), s_preps, sizeof(s_preps) ) != 0 )
        {
            printe("ERROR: FinalizeVBAndPointReps fmcube pr failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                printe("\t%zu: %u .. %u\n", j, prout[ j ], s_preps[ j ] );
            }
        }

        // in place remap
        memcpy( prout.get(), s_fmCubePointReps, sizeof(s_fmCubePointReps) );

        for( uint32_t j=0; j < 24; ++j)
        {
            srcvb[ j ] = j;
        }

        hr = FinalizeVBAndPointReps( srcvb.get(), sizeof(uint32_t), 24, prout.get(), s_remap );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVBAndPointReps fmcube [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( srcvb.get(), s_remap, sizeof(s_remap) ) != 0 )
        {
            printe("ERROR: FinalizeVBAndPointReps fmcube [in-place] vb failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                printe("\t%zu: %u .. %u\n", j, srcvb[ j ], s_remap[ j ] );
            }
        }
        else if ( memcmp( prout.get(), s_preps, sizeof(s_preps) ) != 0 )
        {
            printe("ERROR: FinalizeVBAndPointReps fmcube [in-place] pr failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                printe("\t%zu: %u .. %u\n", j, prout[ j ], s_preps[ j ] );
            }
        }
    }

    // Unused
    {
        using namespace TestGeometry;

        std::unique_ptr<uint32_t[]> prout( new uint32_t[ 24 ] );
        memset( prout.get(), 0xff, sizeof(uint32_t) * 24 );

        std::unique_ptr<uint32_t[]> srcvb( new uint32_t[ 24 ] );
        for( uint32_t j=0; j < 24; ++j)
            srcvb[ j ] = j;

        std::unique_ptr<uint32_t[]> destvb( new uint32_t[ 24 ] );
        memset( destvb.get(), 0, sizeof(uint32_t) * 24 );

        static const uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, uint32_t(-1), 10,
                                            14, 12, 13, 15,
                                            19, uint32_t(-1), 16, 18,
                                            22, 20, 21, 23 };

        static const uint32_t s_sorted[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 0, 10,
                                            14, 12, 13, 15,
                                            19, 0, 16, 18,
                                            22, 20, 21, 23 };

        static const uint32_t s_preps[] = { 0, 1, 2, 3, 4, 5,
                                            6, 7, 0, 5, uint32_t(-1), 2,
                                            1, 4, 6, 3, 2, uint32_t(-1),
                                            5, 1, 3, 7, 4, 0 };

        HRESULT hr = FinalizeVBAndPointReps( srcvb.get(), sizeof(uint32_t), 24, s_fmCubePointReps, nullptr, 0, s_remap, destvb.get(), prout.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVBAndPointReps unused failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destvb.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: FinalizeVBAndPointReps unused failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                printe("\t%zu: %u .. %u\n", j, destvb[ j ], s_sorted[ j ] );
            }
        }
        else if ( memcmp( prout.get(), s_preps, sizeof(s_preps) ) != 0 )
        {
            printe("ERROR: FinalizeVBAndPointReps unused pr failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                printe("\t%zu: %u .. %u\n", j, prout[ j ], s_preps[ j ] );
            }
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// FinalizeVBAndPointReps (duplicates)
bool Test20()
{
    bool success = true;

    // cube identity
    {
        std::unique_ptr<uint32_t[]> preps( new uint32_t[ 12 ] );
        memset( preps.get(), 0xff, sizeof(uint32_t) * 12 );

        std::unique_ptr<uint32_t[]> srcvb( new uint32_t[ 8 ] );
        for( uint32_t j=0; j < 8; ++j)
            srcvb[ j ] = j;

        std::unique_ptr<uint32_t[]> destvb( new uint32_t[ 12 ] );
        memset( destvb.get(), 0xff, sizeof(uint32_t) * 12 );

        std::vector<uint32_t> remap;
        remap.reserve( 12 );
        for( uint32_t j = 0; j < 12; ++j )
            remap.push_back( j );

        std::vector<uint32_t> dups;
        dups.reserve( 4 );
        for( uint32_t j = 0; j < 4; ++j )
            dups.push_back( j );

        HRESULT hr = FinalizeVBAndPointReps( srcvb.get(), sizeof(uint32_t), 8, s_cubePointReps, dups.data(), dups.size(), remap.data(),
                                             destvb.get(), preps.get() );
        if ( FAILED(hr) )
        {
            printe("\nERROR: FinalizeVBAndPointReps dups cube identity failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            size_t j = 0;
            for( ; j < 8; ++j )
            {
                if ( preps[ j ] != remap[ s_cubePointReps[ j ] ] )
                {
                    printe("\nERROR: FinalizeVBAndPointReps dups cube identify pr failed\n" );
                    success = false;
                    break;
                }
                if ( srcvb[ j ] != remap[ destvb[ j ] ] )
                {
                    printe("\nERROR: FinalizeVBAndPointReps dups cube identify vb failed\n" );
                    success = false;
                    break;
                }
            }

            for( size_t i = 0; i < 4; ++i, ++j )
            {
                if ( preps[ j ] != remap[ s_cubePointReps[ i ] ] )
                {
                    printe("\nERROR: FinalizeVBAndPointReps dups cube identify pr failed (2)\n" );
                    success = false;
                    break;
                }
                if ( srcvb[ i ] != remap[ destvb[ j ] ] )
                {
                    printe("\nERROR: FinalizeVBAndPointReps dups cube identify vb failed (2)\n" );
                    success = false;
                    break;
                }
            }
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = FinalizeVBAndPointReps(srcvb.get(), sizeof(uint32_t), 8, s_cubePointReps, dups.data(), 0, remap.data(), destvb.get(), preps.get());
        if (hr != E_INVALIDARG)
        {
            printe("\nERROR: FinalizeVBAndPointReps expected to fail for 0 dup count (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = FinalizeVBAndPointReps(srcvb.get(), sizeof(uint32_t), 8, s_cubePointReps, nullptr, dups.size(), remap.data(), destvb.get(), preps.get());
        if (hr != E_INVALIDARG)
        {
            printe("\nERROR: FinalizeVBAndPointReps expected to fail for null dup with dup count (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = FinalizeVBAndPointReps(srcvb.get(), sizeof(uint32_t), INT16_MAX, s_cubePointReps, dups.data(), UINT32_MAX, remap.data(), destvb.get(), preps.get());
        if ( hr != HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW) )
        {
            printe("\nERROR: FinalizeVBAndPointReps dups [arith overflow] expected failure (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // Face-mapped Cube
    {
        using namespace TestGeometry;

        std::unique_ptr<uint32_t[]> prout( new uint32_t[ 28 ] );
        memset( prout.get(), 0xff, sizeof(uint32_t) * 28 );

        std::unique_ptr<uint32_t[]> srcvb( new uint32_t[ 24 ] );
        for( uint32_t j=0; j < 24; ++j)
        {
            srcvb[ j ] = j;
        }

        std::unique_ptr<uint32_t[]> destvb( new uint32_t[ 28 ] );
        memset( destvb.get(), 0xff, sizeof(uint32_t) * 28 );

        static const uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, 13, 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23,
                                            27, 26, 25, 24 };

        static const uint32_t s_sorted[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, 13, 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23,
                                            3, 2, 1, 0 };

        static const uint32_t s_preps[] = { 0, 1, 2, 3, 4, 5,
                                            6, 7, 0, 5, 7, 2,
                                            1, 4, 6, 3, 2, 6,
                                            5, 1, 3, 7, 4, 0,
                                            0, 3, 1, 2 };

        std::vector<uint32_t> dups;
        dups.reserve( 4 );
        for( uint32_t j = 0; j < 4; ++j )
            dups.push_back( j );

        HRESULT hr = FinalizeVBAndPointReps( srcvb.get(), sizeof(uint32_t), 24, s_fmCubePointReps, dups.data(), dups.size(), s_remap, destvb.get(), prout.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVBAndPointReps dups fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destvb.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: FinalizeVBAndPointReps dups fmcube vb failed\n" );
            success = false;
            for( size_t j = 0; j < 28; ++j )
            {
                printe("\t%zu: %u .. %u\n", j, destvb[ j ], s_sorted[ j ] );
            }
        }
        else if ( memcmp( prout.get(), s_preps, sizeof(s_preps) ) != 0 )
        {
            printe("ERROR: FinalizeVBAndPointReps dups fmcube pr failed\n" );
            success = false;
            for( size_t j = 0; j < 28; ++j )
            {
                printe("\t%zu: %u .. %u\n", j, prout[ j ], s_preps[ j ] );
            }
        }
    }

    // Unused
    {
        using namespace TestGeometry;

        std::unique_ptr<uint32_t[]> prout( new uint32_t[ 28 ] );
        memset( prout.get(), 0xff, sizeof(uint32_t) * 28 );

        std::unique_ptr<uint32_t[]> srcvb( new uint32_t[ 24 ] );
        for( uint32_t j=0; j < 24; ++j)
            srcvb[ j ] = j;

        std::unique_ptr<uint32_t[]> destvb( new uint32_t[ 28 ] );
        memset( destvb.get(), 0, sizeof(uint32_t) * 28 );

        static const uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, uint32_t(-1), 10,
                                            14, 12, 13, 15,
                                            19, uint32_t(-1), 16, 18,
                                            22, 20, 21, 23,
                                            27, 26, uint32_t(-1), 24 };

        static const uint32_t s_sorted[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 0, 10,
                                            14, 12, 13, 15,
                                            19, 0, 16, 18,
                                            22, 20, 21, 23,
                                            3, 2, 0, 0 };

        static const uint32_t s_preps[] = { 0, 1, 2, 3, 4, 5,
                                            6, 7, 0, 5, uint32_t(-1), 2,
                                            1, 4, 6, 3, 2, uint32_t(-1),
                                            5, 1, 3, 7, 4, 0,
                                            0, 3, uint32_t(-1), 2 };

        std::vector<uint32_t> dups;
        dups.reserve( 4 );
        for( uint32_t j = 0; j < 4; ++j )
            dups.push_back( j );

        HRESULT hr = FinalizeVBAndPointReps( srcvb.get(), sizeof(uint32_t), 24, s_fmCubePointReps, dups.data(), dups.size(), s_remap, destvb.get(), prout.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVBAndPointReps dups unused failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destvb.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: FinalizeVBAndPointReps dups unused failed\n" );
            success = false;
            for( size_t j = 0; j < 28; ++j )
            {
                printe("\t%zu: %u .. %u\n", j, destvb[ j ], s_sorted[ j ] );
            }
        }
        else if ( memcmp( prout.get(), s_preps, sizeof(s_preps) ) != 0 )
        {
            printe("ERROR: FinalizeVBAndPointReps unused pr failed\n" );
            success = false;
            for( size_t j = 0; j < 28; ++j )
            {
                printe("\t%zu: %u .. %u\n", j, prout[ j ], s_preps[ j ] );
            }
        }
    }

    return success;
}
