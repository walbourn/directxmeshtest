//-------------------------------------------------------------------------------------
// valid.cpp
//  
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#include "directxtest.h"

#include "directxmesh.h"

#include "TestHelpers.h"
#include "TestGeometry.h"

using namespace DirectX;
using namespace TestGeometry;

namespace
{
    // Cube
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

    const uint16_t g_degenCubeIndices16[12 * 3] =
    {
        0, 1, 2,
        0, 3, 1,
        0, 4, 3,
        0, 5, 4,
        3, 6, 1,
        3, 3, 6,
        2, 1, 6,
        2, 6, 7,
        0, 2, 7,
        5, 7, 5,
        5, 7, 6,
        5, 6, 4,
    };

    const uint16_t g_degenCubeIndices32[12 * 3] =
    {
        0, 1, 2,
        0, 3, 1,
        0, 4, 3,
        0, 5, 4,
        3, 6, 1,
        3, 3, 6,
        2, 1, 6,
        2, 6, 7,
        0, 2, 7,
        5, 7, 5,
        5, 7, 6,
        5, 6, 4,
    };

    // Face-mapped cube
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

    // Tetrahedron
    const uint32_t s_tetraAdj[3 * 4] =
    {
        3, uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), 0
    };

    // Bowtie
    const uint32_t s_bowtieAdj[3 * 2] =
    {
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
    };

    // Backface
    const uint32_t s_backfaceAdj[3 * 2] =
    {
        1, 1, 1,
        0, 0, 0
    };

    // Invalid
    const uint16_t s_badIndices16[3] =
    {
        435, 20, 1,
    };

    const uint32_t s_badIndices32[3] =
    {
        435, 20, 1,
    };

    // Bad neighbors
    const uint32_t s_badNeighborAdj1[3 * 12] =
    {
        1, 6, 8,
        2, 4, 0,
        3, 5, 1,
        9, 11, 2,
        5, 6, 1,
        2, 790, 4,
        0, 4, 7,
        6, 10, 8,
        0, 7, 9,
        8, 10, 3,
        9, 7, 11,
        10, 5, 3
    };

    const uint32_t s_badNeighborAdj2[3 * 12] =
    {
        1, 6, 8,
        2, 4, 6,
        3, 5, 1,
        9, 11, 2,
        5, 6, 1,
        2, 11, 4,
        uint32_t(-1), 4, 7,
        6, 10, 8,
        0, 7, 9,
        8, 10, 3,
        9, 7, 11,
        10, 5, 3
    };

    // Unused
    const uint16_t s_unusedIndices16[12 * 3] =
    {
        0, 1, 2,
        0, 3, 1,
        0, 4, 3,
        0, 5, 4,
        3, 6, uint16_t(-1),
        3, 4, 6,
        2, 1, 6,
        2, 6, 7,
        0, 2, 7,
        0, 7, 5,
        5, 7, 6,
        5, 6, 4,
    };

    const uint32_t s_unusedIndices32[12 * 3] =
    {
        0, 1, 2,
        0, 3, 1,
        0, 4, 3,
        0, 5, 4,
        3, 6, uint32_t(-1),
        3, 4, 6,
        2, 1, 6,
        2, 6, 7,
        0, 2, 7,
        0, 7, 5,
        5, 7, 6,
        5, 6, 4,
    };

    const uint32_t s_unusedAdj[3 * 12] =
    {
        1, uint32_t(-1), 8,
        2, 4, 0,
        3, 5, 1,
        9, 11, 2,
        5, uint32_t(-1), 1,
        2, 11, 4,
        uint32_t(-1), uint32_t(-1), 7,
        6, 10, 8,
        0, 7, 9,
        8, 10, 3,
        9, 7, 11,
        10, 5, 3
    };

    const uint16_t s_unusedIndices16_2[12 * 3] =
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

    const uint32_t s_unusedIndices32_2[12 * 3] =
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

    const uint32_t s_unusedAdj_2[3 * 12] =
    {
        1, 6, 8,
        2, 4, 0,
        3, 5, 1,
        9, 11, 2,
        5, 6, 1,
        2, 11, 4,
        uint32_t(-1), uint32_t(-1), uint32_t(-1),
        6, 10, 8,
        0, 7, 9,
        8, 10, 3,
        9, 7, 11,
        10, 5, 3
    };

    const uint32_t s_unusedAdj_3[3 * 12] =
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

    const uint16_t s_unusedIndices16_3[12 * 3] =
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

    const uint32_t s_unusedIndices32_3[12 * 3] =
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

    const uint16_t s_unusedIndices16_4[12 * 3] =
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

    const uint32_t s_unusedIndices32_4[12 * 3] =
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
// Validate (no adjacency)
bool Test06()
{
    bool success = true;

    // 16-bit
    {
        auto ib = CreateIndexBuffer<uint16_t>( 1023, IB_IDENTITY );

        std::wstring msgs;
        HRESULT hr = Validate( ib.get(), 341, 1023, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) identity failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        ib = CreateIndexBuffer<uint16_t>( 1023, IB_REVERSE );

        hr = Validate( ib.get(), 341, 1023, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) reverse failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Test geometry
        hr = Validate( g_cubeIndices16, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) cube failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_boxIndices16, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) box failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_fmCubeIndices16, 12, 24, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) fmcube failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_tetraIndices16, 4, 4, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) tetra failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Wrong vert count
        hr = Validate( g_fmCubeIndices16, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(16) fmcube [wrong vert] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Degenerate
        hr = Validate( g_degenCubeIndices16, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) degencube failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_cubeIndices16, 12, 8, nullptr, VALIDATE_DEGENERATE, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) cube [degen] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_degenCubeIndices16, 12, 8, nullptr, VALIDATE_DEGENERATE, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(16) degencube [degen] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Unused
        hr = Validate( s_unusedIndices16, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) [ignore unused] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices16_2, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) [ignore unused 2] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate(s_unusedIndices16_3, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs);
        if (FAILED(hr))
        {
            printe("ERROR: Validate(16) [ignore unused 3] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str());
            success = false;
        }

        hr = Validate(s_unusedIndices16_4, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs);
        if (FAILED(hr))
        {
            printe("ERROR: Validate(16) [ignore unused 4] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str());
            success = false;
        }

        hr = Validate( s_unusedIndices16, 12, 8, nullptr, VALIDATE_UNUSED, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(16) [unused] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices16_2, 12, 8, nullptr, VALIDATE_UNUSED, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) [unused 2] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate(s_unusedIndices16_3, 12, 8, nullptr, VALIDATE_UNUSED, &msgs);
        if (FAILED(hr))
        {
            printe("ERROR: Validate(16) [unused 3] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str());
            success = false;
        }

        hr = Validate(s_unusedIndices16_4, 12, 8, nullptr, VALIDATE_UNUSED, &msgs);
        if (FAILED(hr))
        {
            printe("ERROR: Validate(16) [unused 4] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str());
            success = false;
        }

        // Failure cases
        hr = Validate( s_badIndices16, 1, 20, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(16) [bad indices] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = Validate( g_cubeIndices16, 12, D3D11_16BIT_INDEX_STRIP_CUT_VALUE, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: Validate(16) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = Validate( g_cubeIndices16, 12, UINT32_MAX, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: Validate(16) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = Validate( g_cubeIndices16, UINT32_MAX, 8, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: Validate(16) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // 32-bit
    {
        auto ib = CreateIndexBuffer<uint32_t>( 1023, IB_IDENTITY );

        std::wstring msgs;
        HRESULT hr = Validate( ib.get(), 341, 1023, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) identity failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        ib = CreateIndexBuffer<uint32_t>( 1023, IB_REVERSE );

        hr = Validate( ib.get(), 341, 1023, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) reverse failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Test geometry
        hr = Validate( g_cubeIndices32, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) cube failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_boxIndices32, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) box failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_fmCubeIndices32, 12, 24, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) fmcube failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_tetraIndices32, 4, 4, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) tetra failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Wrong vert count
        hr = Validate( g_fmCubeIndices32, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(32) fmcube [wrong vert] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Degenerate
        hr = Validate( g_degenCubeIndices32, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) degencube failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_cubeIndices32, 12, 8, nullptr, VALIDATE_DEGENERATE, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) cube [degen] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_degenCubeIndices32, 12, 8, nullptr, VALIDATE_DEGENERATE, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(32) degencube [degen] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Unused
        hr = Validate( s_unusedIndices32, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) [ignore unused] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices32_2, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) [ignore unused 2] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate(s_unusedIndices32_3, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs);
        if (FAILED(hr))
        {
            printe("ERROR: Validate(32) [ignore unused 3] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str());
            success = false;
        }

        hr = Validate(s_unusedIndices32_4, 12, 8, nullptr, VALIDATE_DEFAULT, &msgs);
        if (FAILED(hr))
        {
            printe("ERROR: Validate(32) [ignore unused 4] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str());
            success = false;
        }

        hr = Validate( s_unusedIndices32, 12, 8, nullptr, VALIDATE_UNUSED, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(32) [unused] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices32_2, 12, 8, nullptr, VALIDATE_UNUSED, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) [unused 2] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate(s_unusedIndices32_3, 12, 8, nullptr, VALIDATE_UNUSED, &msgs);
        if (FAILED(hr))
        {
            printe("ERROR: Validate(32) [unused 3] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str());
            success = false;
        }

        hr = Validate(s_unusedIndices32_4, 12, 8, nullptr, VALIDATE_UNUSED, &msgs);
        if (FAILED(hr))
        {
            printe("ERROR: Validate(32) [unused 4] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str());
            success = false;
        }

        // Failure cases
        hr = Validate( s_badIndices32, 1, 20, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(32) [bad indices] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = Validate( g_cubeIndices32, 12, D3D11_32BIT_INDEX_STRIP_CUT_VALUE, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: Validate(32) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = Validate( g_cubeIndices32, 12, UINT32_MAX, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: Validate(32) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = Validate( g_cubeIndices32, UINT32_MAX, 8, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: Validate(32) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    return success;
}


//-------------------------------------------------------------------------------------
// Validate (with Adjacency)
bool Test09()
{
    bool success = true;

    // 16-bit
    {
        std::wstring msgs;

        // Test geometry
        HRESULT hr = Validate( g_cubeIndices16, 12, 8, s_cubeAdj, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) cube adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_boxIndices16, 12, 8, s_cubeAdj, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) box adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_fmCubeIndices16, 12, 24, s_fmCubeAdj, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) fmcube adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_tetraIndices16, 4, 4, s_tetraAdj, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) tetra adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_bowtieIndices16, 2, 5, s_bowtieAdj, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) bowtie adj [default] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_backfaceIndices16, 2, 3, s_backfaceAdj, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) backface adj [default] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Wrong vert count
        hr = Validate( g_fmCubeIndices16, 12, 8, s_fmCubeAdj, VALIDATE_DEFAULT, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(16) fmcube adj [wrong vert] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Wrong face count
        hr = Validate( g_fmCubeIndices16, 4, 24, s_fmCubeAdj, VALIDATE_DEFAULT, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(16) fmcube adj [wrong face] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Back-facing
        hr = Validate( g_cubeIndices16, 12, 8, s_cubeAdj, VALIDATE_BACKFACING, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) cube adj [bface] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_tetraIndices16, 4, 4, s_tetraAdj, VALIDATE_BACKFACING, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) tetra adj [bface] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_backfaceIndices16, 2, 3, s_backfaceAdj, VALIDATE_BACKFACING, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(16) backface adj [bface] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_bowtieIndices16, 2, 5, s_bowtieAdj, VALIDATE_BACKFACING, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) bowtie adj [bface] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Bow-ties
        hr = Validate( g_cubeIndices16, 12, 8, s_cubeAdj, VALIDATE_BOWTIES, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) cube adj [bowties] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_tetraIndices16, 4, 4, s_tetraAdj, VALIDATE_BOWTIES, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(16) tetra adj [bowties] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_bowtieIndices16, 2, 5, s_bowtieAdj, VALIDATE_BOWTIES, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(16) bowtie adj [bowties] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Unused
        hr = Validate( s_unusedIndices16, 12, 8, s_unusedAdj, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) [ignore unused] adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices16_2, 12, 8, s_unusedAdj_2, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) [ignore unused 2] adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices16_2, 12, 8, s_unusedAdj_3, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) [ignore unused 3] adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices16, 12, 8, s_unusedAdj, VALIDATE_UNUSED, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(16) [unused] adj didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices16_2, 12, 8, s_unusedAdj_2, VALIDATE_UNUSED, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) [unused 2B] adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices16_2, 12, 8, s_unusedAdj_3, VALIDATE_UNUSED, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) [unused 3] adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices16_2, 12, 8, s_unusedAdj_2, VALIDATE_ASYMMETRIC_ADJ, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(16) [unused 2C] adj didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Failure cases
        hr = Validate( g_cubeIndices16, 12, 8, s_badNeighborAdj1, VALIDATE_DEFAULT, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(16) [bad neighbhor] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_cubeIndices16, 12, 8, s_badNeighborAdj2, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(16) [bad neighbhor 2] adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_cubeIndices16, 12, 8, s_badNeighborAdj2, VALIDATE_ASYMMETRIC_ADJ, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(16) [asymmetric neighbhor] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }
    }

    // 32-bit
    {
        std::wstring msgs;

        // Test geometry
        HRESULT hr = Validate( g_cubeIndices32, 12, 8, s_cubeAdj, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) cube adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_boxIndices32, 12, 8, s_cubeAdj, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) box adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_fmCubeIndices32, 12, 24, s_fmCubeAdj, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) fmcube adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_tetraIndices32, 4, 4, s_tetraAdj, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) tetra adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_bowtieIndices32, 2, 5, s_bowtieAdj, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) bowtie adj [default] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_backfaceIndices32, 2, 3, s_backfaceAdj, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) backface adj [default] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Wrong vert count
        hr = Validate( g_fmCubeIndices32, 12, 8, s_fmCubeAdj, VALIDATE_DEFAULT, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(32) fmcube adj [wrong vert] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Wrong face count
        hr = Validate( g_fmCubeIndices32, 4, 24, s_fmCubeAdj, VALIDATE_DEFAULT, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(32) fmcube adj [wrong face] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Back-facing
        hr = Validate( g_cubeIndices32, 12, 8, s_cubeAdj, VALIDATE_BACKFACING, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) cube adj [bface] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_tetraIndices32, 4, 4, s_tetraAdj, VALIDATE_BACKFACING, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) tetra adj [bface] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_backfaceIndices32, 2, 3, s_backfaceAdj, VALIDATE_BACKFACING, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(32) backface adj [bface] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_bowtieIndices32, 2, 5, s_bowtieAdj, VALIDATE_BACKFACING, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) bowtie adj [bface] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Bow-ties
        hr = Validate( g_cubeIndices32, 12, 8, s_cubeAdj, VALIDATE_BOWTIES, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) cube adj [bowties] failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_tetraIndices32, 4, 4, s_tetraAdj, VALIDATE_BOWTIES, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(32) tetra adj [bowties] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_bowtieIndices32, 2, 5, s_bowtieAdj, VALIDATE_BOWTIES, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(32) bowtie adj [bowties] didn't fail as expected(%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Unused
        hr = Validate( s_unusedIndices32, 12, 8, s_unusedAdj, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) [ignore unused] adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices32_2, 12, 8, s_unusedAdj_2, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) [ignore unused 2] adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices32_2, 12, 8, s_unusedAdj_3, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) [ignore unused 3] adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices32, 12, 8, s_unusedAdj, VALIDATE_UNUSED, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(32) [unused] adj didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices32_2, 12, 8, s_unusedAdj_2, VALIDATE_UNUSED, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) [unused 2B] adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices32_2, 12, 8, s_unusedAdj_3, VALIDATE_UNUSED, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) [unused 3] adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( s_unusedIndices32_2, 12, 8, s_unusedAdj_2, VALIDATE_ASYMMETRIC_ADJ, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(32) [unused 2C] adj didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        // Failure cases
        hr = Validate( g_cubeIndices32, 12, 8, s_badNeighborAdj1, VALIDATE_DEFAULT, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(32) [bad neighbhor] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_cubeIndices32, 12, 8, s_badNeighborAdj2, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            printe("ERROR: Validate(32) [bad neighbhor 2] adj failed (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }

        hr = Validate( g_cubeIndices32, 12, 8, s_badNeighborAdj2, VALIDATE_ASYMMETRIC_ADJ, &msgs );
        if ( hr != E_FAIL )
        {
            printe("ERROR: Validate(32) [asymmetric neighbhor] didn't fail as expected (%08X)\n%S\n", static_cast<unsigned int>(hr), msgs.c_str() );
            success = false;
        }
    }

    return success;
}
