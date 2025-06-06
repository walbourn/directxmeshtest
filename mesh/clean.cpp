//-------------------------------------------------------------------------------------
// clean.cpp
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#include "directxtest.h"

#include "DirectXMesh.h"

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

    // Asymmetric adj
    const uint32_t s_badNeighborAdj[3 * 12] =
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
}

//-------------------------------------------------------------------------------------
// Clean
bool Test14()
{
    bool success = true;

    // 16-bit cube
    {
        std::unique_ptr<uint16_t[]> indices( new uint16_t[ 12 * 3 ] );
        memcpy( indices.get(), g_cubeIndices16, sizeof(g_cubeIndices16) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 12 * 3 ] );
        memcpy( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) );

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 12, 8, adj.get(), nullptr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) cube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(16) cube failed unexpected dups\n" );
            success = false;
        }
        else if ( memcmp( indices.get(), g_cubeIndices16, sizeof(g_cubeIndices16) ) != 0 )
        {
            printe("ERROR: Clean(16) cube failed\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 8, adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) cube validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }

        // invalid args
        hr = Clean( indices.get(), 12, UINT16_MAX /*D3D11_16BIT_INDEX_STRIP_CUT_VALUE*/, adj.get(), nullptr, dups );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: Clean(16) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = Clean( indices.get(), 12, UINT32_MAX, adj.get(), nullptr, dups );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: Clean(16) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = Clean( indices.get(), UINT32_MAX, 8, adj.get(), nullptr, dups );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: Clean(16) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = Clean( indices.get(), 12, 2, adj.get(), nullptr, dups );
        if ( hr != E_FAIL )
        {
            printe("\nERROR: Clean(16) expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        // Asymmetric neighbor
        static const uint32_t s_badNeighborCleanAdj[3 * 12] =
        {
            uint32_t(-1), uint32_t(-1), 8,
            2, 4, uint32_t(-1),
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

        memcpy( indices.get(), g_cubeIndices16, sizeof(g_cubeIndices16) );
        memcpy( adj.get(), s_badNeighborAdj, sizeof(s_badNeighborAdj) );
        hr = Clean( indices.get(), 12, 8, adj.get(), nullptr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) cube [asymmetric] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(16) cube [asymmetric] failed unexpected dups\n" );
            success = false;
        }
        else if ( memcmp( indices.get(), g_cubeIndices16, sizeof(g_cubeIndices16) ) != 0 )
        {
            printe("ERROR: Clean(16) cube [asymmetric] failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_badNeighborCleanAdj, sizeof(s_badNeighborCleanAdj) ) != 0 )
        {
            printe("ERROR: Clean(16) cube [asymmetric] adj failed\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 8, adj.get(), VALIDATE_BACKFACING | VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) cube [asymmetric] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }

        // Unused
        static const uint16_t s_unusedClean[ 12*3 ] =
        {
            0, 1, 2,
            0, 3, 1,
            0, 4, 3,
            0, 5, 4,
            uint16_t(-1), uint16_t(-1), uint16_t(-1),
            3, 4, 6,
            2, 1, 6,
            2, 6, 7,
            0, 2, 7,
            0, 7, 5,
            5, 7, 6,
            5, 6, 4,
        };

        static const uint32_t s_unusedCleanAdj[3 * 12] =
        {
            1, uint32_t(-1), 8,
            2, uint32_t(-1), 0,
            3, 5, 1,
            9, 11, 2,
            uint32_t(-1), uint32_t(-1), uint32_t(-1),
            2, 11, uint32_t(-1),
            uint32_t(-1), uint32_t(-1), 7,
            6, 10, 8,
            0, 7, 9,
            8, 10, 3,
            9, 7, 11,
            10, 5, 3
        };

        memcpy( indices.get(), s_unusedIndices16, sizeof(s_unusedIndices16) );
        memcpy( adj.get(), s_unusedAdj, sizeof(s_unusedAdj) );
        hr = Clean( indices.get(), 12, 8, adj.get(), nullptr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) cube [unused] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(16) cube [unused] failed unexpected dups\n" );
            success = false;
        }
        else if ( memcmp( indices.get(), s_unusedClean, sizeof(s_unusedClean) ) != 0 )
        {
            printe("ERROR: Clean(16) cube [unused] failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_unusedCleanAdj, sizeof(s_unusedCleanAdj) ) != 0 )
        {
            printe("ERROR: Clean(16) cube [unused] adj failed\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 8, adj.get(), VALIDATE_BACKFACING | VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) cube [unused] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }

        }
    }

    // 16-bit fmcube
    {
        std::unique_ptr<uint16_t[]> indices( new uint16_t[ 12 * 3 ] );
        memcpy( indices.get(), g_fmCubeIndices16, sizeof(g_fmCubeIndices16) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 12 * 3 ] );
        memcpy( adj.get(), s_fmCubeAdj, sizeof(s_fmCubeAdj) );

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 12, 24, adj.get(), nullptr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(16) fmcube failed unexpected dups\n" );
            success = false;
        }
        else if ( memcmp( indices.get(), g_fmCubeIndices16, sizeof(g_fmCubeIndices16) ) != 0 )
        {
            printe("ERROR: Clean(16) fmcube failed\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 24, adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) fmcube validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }
    }

    // 16-bit tetra
    {
        std::unique_ptr<uint16_t[]> indices( new uint16_t[ 4 * 3 ] );
        memcpy( indices.get(), g_tetraIndices16, sizeof(g_tetraIndices16) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 4 * 3 ] );
        memcpy( adj.get(), s_tetraAdj, sizeof(s_tetraAdj) );

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 4, 4, adj.get(), nullptr, dups, false );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) tetra failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( indices.get(), g_tetraIndices16, sizeof(g_tetraIndices16) ) != 0 )
        {
            printe("ERROR: Clean(16) tetra failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_tetraAdj, sizeof(s_tetraAdj) ) != 0 )
        {
            printe("ERROR: Clean(16) tetra failed (2)\n" );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(16) tetra failed no dups expected %zu\n", dups.size() );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 4, 4, adj.get(), VALIDATE_BACKFACING, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) tetra validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
            else
            {
                hr = Validate( indices.get(), 4, 4, adj.get(), VALIDATE_BOWTIES, &msgs );
                if ( hr != E_FAIL )
                {
                    printe("ERROR: Clean(16) tetra validation (2) unexpected result (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                    success = false;
                }
            }
        }

        // Bowties
        static const uint16_t s_cleanIndices[] =
            {
                0, 1, 2,
                4, 5, 6,
                7, 8, 3,
                0, 9, 1,
            };

        static const uint32_t s_cleanAdj[] =
            {
                3, uint32_t(-1), uint32_t(-1),
                uint32_t(-1), uint32_t(-1), uint32_t(-1),
                uint32_t(-1), uint32_t(-1), uint32_t(-1),
                uint32_t(-1), uint32_t(-1), 0
            };

        static const uint32_t s_cleanDups[] =
            {
                1, 2, 0, 2, 0, 3,
            };

        memcpy( indices.get(), g_tetraIndices16, sizeof(g_tetraIndices16) );
        memcpy( adj.get(), s_tetraAdj, sizeof(s_tetraAdj) );

        hr = Clean( indices.get(), 4, 4, adj.get(), nullptr, dups, true );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) tetra [bowties] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( indices.get(), s_cleanIndices, sizeof(s_cleanIndices) ) != 0 )
        {
            printe("ERROR: Clean(16) tetra [bowties] failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_cleanAdj, sizeof(s_cleanAdj) ) != 0 )
        {
            printe("ERROR: Clean(16) tetra [bowties] failed (2)\n" );
            success = false;
        }
        else if ( dups.size() != std::size(s_cleanDups) )
        {
            printe("ERROR: Clean(16) tetra [bowties] failed unexpected number of dups %zu\n", dups.size() );
            success = false;
        }
        else if ( memcmp( dups.data(), s_cleanDups, sizeof(s_cleanDups) ) != 0 )
        {
            printe("ERROR: Clean(16) tetra [bowties] failed (3)\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 4, 4 + dups.size(), adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) tetra [bowties] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
            else
            {
                hr = Validate( indices.get(), 4, 4, adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
                if ( hr != E_FAIL )
                {
                    printe("ERROR: Clean(16) tetra [bowties] expected a validate failure (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                    success = false;
                }
            }
        }
    }

    // 16-bit backfacing
    {
        static const uint32_t s_cleanAdj[] =
            {
                uint32_t(-1), uint32_t(-1), uint32_t(-1),
                uint32_t(-1), uint32_t(-1), uint32_t(-1),
            };

        std::unique_ptr<uint16_t[]> indices( new uint16_t[ 2 * 3 ] );
        memcpy( indices.get(), g_backfaceIndices16, sizeof(g_backfaceIndices16) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 2 * 3 ] );
        memcpy( adj.get(), s_backfaceAdj, sizeof(s_backfaceAdj) );

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 2, 3, adj.get(), nullptr, dups, false );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) bface failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( indices.get(), g_backfaceIndices16, sizeof(g_backfaceIndices16) ) != 0 )
        {
            printe("ERROR: Clean(16) bface failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_cleanAdj, sizeof(s_cleanAdj) ) != 0 )
        {
            printe("ERROR: Clean(16) bface failed (2)\n" );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(16) bface failed expected no dups %zu\n", dups.size() );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 2, 3, adj.get(), VALIDATE_BACKFACING, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) bface validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
            else
            {
                hr = Validate( indices.get(), 2, 3, adj.get(), VALIDATE_BOWTIES, &msgs );
                if ( hr != E_FAIL )
                {
                    printe("ERROR: Clean(16) bface validation (2) expected to fail (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                    success = false;
                }
            }
        }

        // Bowties
        static const uint16_t s_cleanIndices[] =
            {
                0, 1, 2,
                3, 4, 5
            };

        static const uint32_t s_cleanDups[] =
            {
                2, 1, 0
            };

        memcpy( indices.get(), g_backfaceIndices16, sizeof(g_backfaceIndices16) );
        memcpy( adj.get(), s_backfaceAdj, sizeof(s_backfaceAdj) );

        hr = Clean( indices.get(), 2, 3, adj.get(), nullptr, dups, true );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) bface [bowties] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( indices.get(), s_cleanIndices, sizeof(s_cleanIndices) ) != 0 )
        {
            printe("ERROR: Clean(16) bface [bowties] failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_cleanAdj, sizeof(s_cleanAdj) ) != 0 )
        {
            printe("ERROR: Clean(16) bface [bowties] failed (2)\n" );
            success = false;
        }
        else if ( dups.size() != std::size(s_cleanDups) )
        {
            printe("ERROR: Clean(16) bface [bowties] failed unexpected number of dups %zu\n", dups.size() );
            success = false;
        }
        else if ( memcmp( dups.data(), s_cleanDups, sizeof(s_cleanDups) ) != 0 )
        {
            printe("ERROR: Clean(16) bface [bowties] failed (3)\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 2, 3 + dups.size(), adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) bface [bowties] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
            else
            {
                hr = Validate( indices.get(), 2, 3, adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
                if ( hr != E_FAIL )
                {
                    printe("ERROR: Clean(16) bface [bowties] expected a validate failure (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                    success = false;
                }
            }
        }
    }

    // 16-bit bowtie
    {
        std::unique_ptr<uint16_t[]> indices( new uint16_t[ 2 * 3 ] );
        memcpy( indices.get(), g_bowtieIndices16, sizeof(g_bowtieIndices16) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 2 * 3 ] );
        memcpy( adj.get(), s_bowtieAdj, sizeof(s_bowtieAdj) );

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 2, 5, adj.get(), nullptr, dups, false );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) bowtie failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( indices.get(), g_bowtieIndices16, sizeof(g_bowtieIndices16) ) != 0 )
        {
            printe("ERROR: Clean(16) bowtie failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_bowtieAdj, sizeof(s_bowtieAdj) ) != 0 )
        {
            printe("ERROR: Clean(16) bowtie failed (2)\n" );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(16) bowtie failed expected no dups %zu\n", dups.size() );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 2, 5, adj.get(), VALIDATE_BACKFACING, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) bowtie validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
            else
            {
                hr = Validate( indices.get(), 2, 5, adj.get(), VALIDATE_BOWTIES, &msgs );
                if ( hr != E_FAIL )
                {
                    printe("ERROR: Clean(16) bowtie validation (2) expected failure (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                    success = false;
                }
            }
        }

        // Bowties
        static const uint16_t s_cleanIndices[] =
            {
                0, 1, 2,
                5, 4, 3,
            };

        static const uint32_t s_cleanAdj[] =
            {
                uint32_t(-1), uint32_t(-1), uint32_t(-1),
                uint32_t(-1), uint32_t(-1), uint32_t(-1),
            };

        static const uint32_t s_cleanDups[] =
            {
                2,
            };

        memcpy( indices.get(), g_bowtieIndices16, sizeof(g_bowtieIndices16) );
        memcpy( adj.get(), s_bowtieAdj, sizeof(s_bowtieAdj) );

        hr = Clean( indices.get(), 2, 5, adj.get(), nullptr, dups, true );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) bowtie [bowties] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( indices.get(), s_cleanIndices, sizeof(s_cleanIndices) ) != 0 )
        {
            printe("ERROR: Clean(16) bowtie [bowties] failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_cleanAdj, sizeof(s_cleanAdj) ) != 0 )
        {
            printe("ERROR: Clean(16) bowtie [bowties] failed (2)\n" );
            success = false;
        }
        else if ( dups.size() != std::size(s_cleanDups) )
        {
            printe("ERROR: Clean(16) bowtie [bowties] failed unexpected number of dups %zu\n", dups.size() );
            success = false;
        }
        else if ( memcmp( dups.data(), s_cleanDups, sizeof(s_cleanDups) ) != 0 )
        {
            printe("ERROR: Clean(16) bowtie [bowties] failed (3)\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 2, 5 + dups.size(), adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) bowtie [bowties] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
            else
            {
                hr = Validate( indices.get(), 2, 5, adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
                if ( hr != E_FAIL )
                {
                    printe("ERROR: Clean(16) bowtie [bowties] expected a validate failure (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                    success = false;
                }
            }
        }
    }

    // 32-bit cube
    {
        std::unique_ptr<uint32_t[]> indices( new uint32_t[ 12 * 3 ] );
        memcpy( indices.get(), g_cubeIndices32, sizeof(g_cubeIndices32) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 12 * 3 ] );
        memcpy( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) );

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 12, 8, adj.get(), nullptr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) cube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(32) cube failed unexpected dups\n" );
            success = false;
        }
        else if ( memcmp( indices.get(), g_cubeIndices32, sizeof(g_cubeIndices32) ) != 0 )
        {
            printe("ERROR: Clean(32) cube failed\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 8, adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(32) cube validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }

        // invalid args
        hr = Clean( indices.get(), 12, UINT32_MAX /*D3D11_32BIT_INDEX_STRIP_CUT_VALUE*/, adj.get(), nullptr, dups );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: Clean(32) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = Clean( indices.get(), 12, UINT32_MAX, adj.get(), nullptr, dups );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: Clean(32) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = Clean( indices.get(), UINT32_MAX, 8, adj.get(), nullptr, dups );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: Clean(32) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = Clean( indices.get(), 12, 2, adj.get(), nullptr, dups );
        if ( hr != E_FAIL )
        {
            printe("\nERROR: Clean(32) expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        // Asymmetric neighbor
        static const uint32_t s_badNeighborCleanAdj[3 * 12] =
        {
            uint32_t(-1), uint32_t(-1), 8,
            2, 4, uint32_t(-1),
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

        memcpy( indices.get(), g_cubeIndices32, sizeof(g_cubeIndices32) );
        memcpy( adj.get(), s_badNeighborAdj, sizeof(s_badNeighborAdj) );
        hr = Clean( indices.get(), 12, 8, adj.get(), nullptr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) cube [asymmetric] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(32) cube [asymmetric] failed unexpected dups\n" );
            success = false;
        }
        else if ( memcmp( indices.get(), g_cubeIndices32, sizeof(g_cubeIndices32) ) != 0 )
        {
            printe("ERROR: Clean(32) cube [asymmetric] failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_badNeighborCleanAdj, sizeof(s_badNeighborCleanAdj) ) != 0 )
        {
            printe("ERROR: Clean(32) cube [asymmetric] adj failed\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 8, adj.get(), VALIDATE_BACKFACING | VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(32) cube [asymmetric] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }

        // Unused
        static const uint32_t s_unusedClean[ 12*3 ] =
        {
            0, 1, 2,
            0, 3, 1,
            0, 4, 3,
            0, 5, 4,
            uint32_t(-1), uint32_t(-1), uint32_t(-1),
            3, 4, 6,
            2, 1, 6,
            2, 6, 7,
            0, 2, 7,
            0, 7, 5,
            5, 7, 6,
            5, 6, 4,
        };

        static const uint32_t s_unusedCleanAdj[3 * 12] =
        {
            1, uint32_t(-1), 8,
            2, uint32_t(-1), 0,
            3, 5, 1,
            9, 11, 2,
            uint32_t(-1), uint32_t(-1), uint32_t(-1),
            2, 11, uint32_t(-1),
            uint32_t(-1), uint32_t(-1), 7,
            6, 10, 8,
            0, 7, 9,
            8, 10, 3,
            9, 7, 11,
            10, 5, 3
        };

        memcpy( indices.get(), s_unusedIndices32, sizeof(s_unusedIndices32) );
        memcpy( adj.get(), s_unusedAdj, sizeof(s_unusedAdj) );
        hr = Clean( indices.get(), 12, 8, adj.get(), nullptr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) cube [unused] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(32) cube [unused] failed unexpected dups\n" );
            success = false;
        }
        else if ( memcmp( indices.get(), s_unusedClean, sizeof(s_unusedClean) ) != 0 )
        {
            printe("ERROR: Clean(32) cube [unused] failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_unusedCleanAdj, sizeof(s_unusedCleanAdj) ) != 0 )
        {
            printe("ERROR: Clean(32) cube [unused] adj failed\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 8, adj.get(), VALIDATE_BACKFACING | VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(32) cube [unused] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }

        }
    }

    // 32-bit fmcube
    {
        std::unique_ptr<uint32_t[]> indices( new uint32_t[ 12 * 3 ] );
        memcpy( indices.get(), g_fmCubeIndices32, sizeof(g_fmCubeIndices32) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 12 * 3 ] );
        memcpy( adj.get(), s_fmCubeAdj, sizeof(s_fmCubeAdj) );

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 12, 24, adj.get(), nullptr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(32) fmcube failed unexpected dups\n" );
            success = false;
        }
        else if ( memcmp( indices.get(), g_fmCubeIndices32, sizeof(g_fmCubeIndices32) ) != 0 )
        {
            printe("ERROR: Clean(32) fmcube failed\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 24, adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(32) fmcube validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }
    }

    // 32-bit tetra
    {
        std::unique_ptr<uint32_t[]> indices( new uint32_t[ 4 * 3 ] );
        memcpy( indices.get(), g_tetraIndices32, sizeof(g_tetraIndices32) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 4 * 3 ] );
        memcpy( adj.get(), s_tetraAdj, sizeof(s_tetraAdj) );

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 4, 4, adj.get(), nullptr, dups, false );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) tetra failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( indices.get(), g_tetraIndices32, sizeof(g_tetraIndices32) ) != 0 )
        {
            printe("ERROR: Clean(32) tetra failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_tetraAdj, sizeof(s_tetraAdj) ) != 0 )
        {
            printe("ERROR: Clean(32) tetra failed (2)\n" );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(32) tetra failed no dups expected %zu\n", dups.size() );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 4, 4, adj.get(), VALIDATE_BACKFACING, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(32) tetra validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
            else
            {
                hr = Validate( indices.get(), 4, 4, adj.get(), VALIDATE_BOWTIES, &msgs );
                if ( hr != E_FAIL )
                {
                    printe("ERROR: Clean(32) tetra validation (2) unexpected result (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                    success = false;
                }
            }
        }

        // Bowties
        static const uint32_t s_cleanIndices[] =
            {
                0, 1, 2,
                4, 5, 6,
                7, 8, 3,
                0, 9, 1,
            };

        static const uint32_t s_cleanAdj[] =
            {
                3, uint32_t(-1), uint32_t(-1),
                uint32_t(-1), uint32_t(-1), uint32_t(-1),
                uint32_t(-1), uint32_t(-1), uint32_t(-1),
                uint32_t(-1), uint32_t(-1), 0
            };

        static const uint32_t s_cleanDups[] =
            {
                1, 2, 0, 2, 0, 3,
            };

        memcpy( indices.get(), g_tetraIndices32, sizeof(g_tetraIndices32) );
        memcpy( adj.get(), s_tetraAdj, sizeof(s_tetraAdj) );

        hr = Clean( indices.get(), 4, 4, adj.get(), nullptr, dups, true );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) tetra [bowties] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( indices.get(), s_cleanIndices, sizeof(s_cleanIndices) ) != 0 )
        {
            printe("ERROR: Clean(32) tetra [bowties] failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_cleanAdj, sizeof(s_cleanAdj) ) != 0 )
        {
            printe("ERROR: Clean(32) tetra [bowties] failed (2)\n" );
            success = false;
        }
        else if ( dups.size() != std::size(s_cleanDups) )
        {
            printe("ERROR: Clean(32) tetra [bowties] failed unexpected number of dups %zu\n", dups.size() );
            success = false;
        }
        else if ( memcmp( dups.data(), s_cleanDups, sizeof(s_cleanDups) ) != 0 )
        {
            printe("ERROR: Clean(32) tetra [bowties] failed (3)\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 4, 4 + dups.size(), adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(32) tetra [bowties] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
            else
            {
                hr = Validate( indices.get(), 4, 4, adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
                if ( hr != E_FAIL )
                {
                    printe("ERROR: Clean(32) tetra [bowties] expected a validate failure (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                    success = false;
                }
            }
        }
    }

    // 32-bit backfacing
    {
        static const uint32_t s_cleanAdj[] =
            {
                uint32_t(-1), uint32_t(-1), uint32_t(-1),
                uint32_t(-1), uint32_t(-1), uint32_t(-1),
            };

        std::unique_ptr<uint32_t[]> indices( new uint32_t[ 2 * 3 ] );
        memcpy( indices.get(), g_backfaceIndices32, sizeof(g_backfaceIndices32) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 2 * 3 ] );
        memcpy( adj.get(), s_backfaceAdj, sizeof(s_backfaceAdj) );

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 2, 3, adj.get(), nullptr, dups, false );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) bface failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( indices.get(), g_backfaceIndices32, sizeof(g_backfaceIndices32) ) != 0 )
        {
            printe("ERROR: Clean(32) bface failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_cleanAdj, sizeof(s_cleanAdj) ) != 0 )
        {
            printe("ERROR: Clean(32) bface failed (2)\n" );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(32) bface failed expected no dups %zu\n", dups.size() );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 2, 3, adj.get(), VALIDATE_BACKFACING, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(32) bface validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
            else
            {
                hr = Validate( indices.get(), 2, 3, adj.get(), VALIDATE_BOWTIES, &msgs );
                if ( hr != E_FAIL )
                {
                    printe("ERROR: Clean(32) bface validation (2) expected to fail (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                    success = false;
                }
            }
        }

        // Bowties
        static const uint32_t s_cleanIndices[] =
            {
                0, 1, 2,
                3, 4, 5
            };

        static const uint32_t s_cleanDups[] =
            {
                2, 1, 0
            };

        memcpy( indices.get(), g_backfaceIndices32, sizeof(g_backfaceIndices32) );
        memcpy( adj.get(), s_backfaceAdj, sizeof(s_backfaceAdj) );

        hr = Clean( indices.get(), 2, 3, adj.get(), nullptr, dups, true );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) bface [bowties] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( indices.get(), s_cleanIndices, sizeof(s_cleanIndices) ) != 0 )
        {
            printe("ERROR: Clean(32) bface [bowties] failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_cleanAdj, sizeof(s_cleanAdj) ) != 0 )
        {
            printe("ERROR: Clean(32) bface [bowties] failed (2)\n" );
            success = false;
        }
        else if ( dups.size() != std::size(s_cleanDups) )
        {
            printe("ERROR: Clean(32) bface [bowties] failed unexpected number of dups %zu\n", dups.size() );
            success = false;
        }
        else if ( memcmp( dups.data(), s_cleanDups, sizeof(s_cleanDups) ) != 0 )
        {
            printe("ERROR: Clean(32) bface [bowties] failed (3)\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 2, 3 + dups.size(), adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(32) bface [bowties] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
            else
            {
                hr = Validate( indices.get(), 2, 3, adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
                if ( hr != E_FAIL )
                {
                    printe("ERROR: Clean(32) bface [bowties] expected a validate failure (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                    success = false;
                }
            }
        }
    }

    // 32-bit bowtie
    {
        std::unique_ptr<uint32_t[]> indices( new uint32_t[ 2 * 3 ] );
        memcpy( indices.get(), g_bowtieIndices32, sizeof(g_bowtieIndices32) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 2 * 3 ] );
        memcpy( adj.get(), s_bowtieAdj, sizeof(s_bowtieAdj) );

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 2, 5, adj.get(), nullptr, dups, false );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) bowtie failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( indices.get(), g_bowtieIndices32, sizeof(g_bowtieIndices32) ) != 0 )
        {
            printe("ERROR: Clean(32) bowtie failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_bowtieAdj, sizeof(s_bowtieAdj) ) != 0 )
        {
            printe("ERROR: Clean(32) bowtie failed (2)\n" );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(32) bowtie failed expected no dups %zu\n", dups.size() );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 2, 5, adj.get(), VALIDATE_BACKFACING , &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(32) bowtie validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
            else
            {
                hr = Validate( indices.get(), 2, 5, adj.get(), VALIDATE_BOWTIES, &msgs );
                if ( hr != E_FAIL )
                {
                    printe("ERROR: Clean(32) bowtie validation (2) expected failure (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                    success = false;
                }
            }
        }

        // Bowties
        static const uint32_t s_cleanIndices[] =
            {
                0, 1, 2,
                5, 4, 3,
            };

        static const uint32_t s_cleanAdj[] =
            {
                uint32_t(-1), uint32_t(-1), uint32_t(-1),
                uint32_t(-1), uint32_t(-1), uint32_t(-1),
            };

        static const uint32_t s_cleanDups[] =
            {
                2,
            };

        memcpy( indices.get(), g_bowtieIndices32, sizeof(g_bowtieIndices32) );
        memcpy( adj.get(), s_bowtieAdj, sizeof(s_bowtieAdj) );

        hr = Clean( indices.get(), 2, 5, adj.get(), nullptr, dups, true );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) bowtie [bowties] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( indices.get(), s_cleanIndices, sizeof(s_cleanIndices) ) != 0 )
        {
            printe("ERROR: Clean(32) bowtie [bowties] failed\n" );
            success = false;
        }
        else if ( memcmp( adj.get(), s_cleanAdj, sizeof(s_cleanAdj) ) != 0 )
        {
            printe("ERROR: Clean(32) bowtie [bowties] failed (2)\n" );
            success = false;
        }
        else if ( dups.size() != std::size(s_cleanDups) )
        {
            printe("ERROR: Clean(32) bowtie [bowties] failed unexpected number of dups %zu\n", dups.size() );
            success = false;
        }
        else if ( memcmp( dups.data(), s_cleanDups, sizeof(s_cleanDups) ) != 0 )
        {
            printe("ERROR: Clean(32) bowtie [bowties] failed (3)\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 2, 5 + dups.size(), adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(32) bowtie [bowties] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
            else
            {
                hr = Validate( indices.get(), 2, 5, adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
                if ( hr != E_FAIL )
                {
                    printe("ERROR: Clean(32) bowtie [bowties] expected a validate failure (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                    success = false;
                }
            }
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// Clean (attributes)
bool Test21()
{
    bool success = true;

    // 16-bit cube same attribute
    {
        std::unique_ptr<uint16_t[]> indices( new uint16_t[ 12 * 3 ] );
        memcpy( indices.get(), g_cubeIndices16, sizeof(g_cubeIndices16) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 12 * 3 ] );
        memcpy( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) );

        static const uint32_t attr[ 12 ] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 12, 8, adj.get(), attr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) cube identity failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(16) cube identity failed unexpected dups\n" );
            success = false;
        }
        else if ( memcmp( indices.get(), g_cubeIndices16, sizeof(g_cubeIndices16) ) != 0 )
        {
            printe("ERROR: Clean(16) cube identity failed\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 8, adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) cube identity validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }

        // null adj
        hr = Clean( indices.get(), 12, 8, nullptr, attr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) cube [null adj] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(16) cube [null adj] failed unexpected dups\n" );
            success = false;
        }
        else if ( memcmp( indices.get(), g_cubeIndices16, sizeof(g_cubeIndices16) ) != 0 )
        {
            printe("ERROR: Clean(16) cube [null adj] failed\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 8, nullptr, VALIDATE_DEFAULT, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) cube [null adj] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }
    }

    // 16-bit cube attr
    {
        std::unique_ptr<uint16_t[]> indices( new uint16_t[ 12 * 3 ] );
        memcpy( indices.get(), g_cubeIndices16, sizeof(g_cubeIndices16) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 12 * 3 ] );
        memcpy( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) );

        static const uint32_t attr[ 12 ] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5 };

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 12, 8, adj.get(), attr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) cube attr failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( dups.size() != 16 )
        {
            printe("ERROR: Clean(16) cube attr failed unexpected number of dups %zu\n", dups.size() );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 8 + dups.size(), adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) cube attr validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }

        // null adj
        memcpy( indices.get(), g_cubeIndices16, sizeof(g_cubeIndices16) );
        memcpy( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) );

        hr = Clean( indices.get(), 12, 8, nullptr, attr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) cube attr [null adj] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( dups.size() != 16 )
        {
            printe("ERROR: Clean(16) cube attr [null adj] failed unexpected number of dups %zu\n", dups.size() );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 8 + dups.size(), nullptr, VALIDATE_DEFAULT, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) cube attr [null adj] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }
    }

    // 16-bit fmcube attr
    {
        std::unique_ptr<uint16_t[]> indices( new uint16_t[ 12 * 3 ] );
        memcpy( indices.get(), g_fmCubeIndices16, sizeof(g_fmCubeIndices16) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 12 * 3 ] );
        memcpy( adj.get(), s_fmCubeAdj, sizeof(s_fmCubeAdj) );

        static const uint32_t attr[ 12 ] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5 };

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 12, 24, adj.get(), attr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) fmcube attr failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(16) fmcube attr failed unexpected dups\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 24, adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) fmcube attr validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }

        // null adj
        memcpy( indices.get(), g_fmCubeIndices16, sizeof(g_fmCubeIndices16) );
        memcpy( adj.get(), s_fmCubeAdj, sizeof(s_fmCubeAdj) );

        hr = Clean( indices.get(), 12, 24, nullptr, attr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(16) fmcube attr [null adj] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(16) fmcube attr [null adj] failed unexpected dups\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 24, nullptr, VALIDATE_DEFAULT, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) fmcube attr [null adj] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }
    }

    // 32-bit cube same attribute
    {
        std::unique_ptr<uint32_t[]> indices( new uint32_t[ 12 * 3 ] );
        memcpy( indices.get(), g_cubeIndices32, sizeof(g_cubeIndices32) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 12 * 3 ] );
        memcpy( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) );

        static const uint32_t attr[ 12 ] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 12, 8, adj.get(), attr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) cube identity failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(32) cube identity failed unexpected dups\n" );
            success = false;
        }
        else if ( memcmp( indices.get(), g_cubeIndices32, sizeof(g_cubeIndices32) ) != 0 )
        {
            printe("ERROR: Clean(32) cube identity failed\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 8, adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(32) cube identity validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }

        // null adj
        hr = Clean( indices.get(), 12, 8, nullptr, attr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) cube identity [null adj] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(32) cube identity [null adj] failed unexpected dups\n" );
            success = false;
        }
        else if ( memcmp( indices.get(), g_cubeIndices32, sizeof(g_cubeIndices32) ) != 0 )
        {
            printe("ERROR: Clean(32) cube identity [null adj] failed\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 8, nullptr, VALIDATE_DEFAULT, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(32) cube identity [null adj] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }
    }

    // 32-bit cube attr
    {
        std::unique_ptr<uint32_t[]> indices( new uint32_t[ 12 * 3 ] );
        memcpy( indices.get(), g_cubeIndices32, sizeof(g_cubeIndices32) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 12 * 3 ] );
        memcpy( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) );

        static const uint32_t attr[ 12 ] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5 };

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 12, 8, adj.get(), attr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) cube attr failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( dups.size() != 16 )
        {
            printe("ERROR: Clean(32) cube attr failed unexpected number of dups %zu\n", dups.size() );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 8 + dups.size(), adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(16) cube attr validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }

        // null adj
        memcpy( indices.get(), g_cubeIndices32, sizeof(g_cubeIndices32) );
        memcpy( adj.get(), s_cubeAdj, sizeof(s_cubeAdj) );

        hr = Clean( indices.get(), 12, 8, nullptr, attr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) cube attr [null adj] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( dups.size() != 16 )
        {
            printe("ERROR: Clean(32) cube attr [null adj] failed unexpected number of dups %zu\n", dups.size() );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 8 + dups.size(), nullptr, VALIDATE_DEFAULT, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(32) cube attr [null adj] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }
    }

    // 32-bit fmcube attr
    {
        std::unique_ptr<uint32_t[]> indices( new uint32_t[ 12 * 3 ] );
        memcpy( indices.get(), g_fmCubeIndices32, sizeof(g_fmCubeIndices32) );

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ 12 * 3 ] );
        memcpy( adj.get(), s_fmCubeAdj, sizeof(s_fmCubeAdj) );

        static const uint32_t attr[ 12 ] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5 };

        std::vector<uint32_t> dups;
        HRESULT hr = Clean( indices.get(), 12, 24, adj.get(), attr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) fmcube attr failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(32) fmcube attr failed unexpected dups\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 24, adj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(32) fmcube attr validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }

        // null adj
        memcpy( indices.get(), g_fmCubeIndices32, sizeof(g_fmCubeIndices32) );
        memcpy( adj.get(), s_fmCubeAdj, sizeof(s_fmCubeAdj) );

        hr = Clean( indices.get(), 12, 24, nullptr, attr, dups );
        if ( FAILED(hr) )
        {
            printe("ERROR: Clean(32) fmcube attr [null adj] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !dups.empty() )
        {
            printe("ERROR: Clean(32) fmcube attr [null adj] failed unexpected dups\n" );
            success = false;
        }
        else
        {
            std::wstring msgs;
            hr = Validate( indices.get(), 12, 24, nullptr, VALIDATE_DEFAULT, &msgs );
            if ( FAILED(hr) )
            {
                printe("ERROR: Clean(32) fmcube attr [null adj] validation failed (%08X)\n%ls\n", static_cast<unsigned int>(hr), msgs.c_str() );
                success = false;
            }
        }
    }

    return success;
}

