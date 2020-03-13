//-------------------------------------------------------------------------------------
// remap.cpp
//  
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#include "directxtest.h"
#include "TestHelpers.h"
#include "TestGeometry.h"

#include "directxmesh.h"

#include <algorithm>
#include <random>

using namespace DirectX;

#pragma warning(disable:6262) // test code doesn't need conservative stack size

//-------------------------------------------------------------------------------------
// FinalizeIB
bool Test03()
{
    bool success = true;

    std::random_device rd;
    std::default_random_engine rng(rd());

    // Identity (16)
    {
        auto srcib = CreateIndexBuffer<uint16_t>( 1023, IB_IDENTITY );

        uint32_t remap[1024];
        for( uint32_t j = 0; j < 1024; ++j )
            remap[j] = j;

        auto destib = CreateIndexBuffer<uint16_t>( 1023, IB_ZERO );

        HRESULT hr = FinalizeIB( srcib.get(), 341, remap, 1024, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(16) identity failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestIBCorrect( destib.get(), 1023, IB_IDENTITY ) )
        {
            printe("ERROR: FinalizeIB(16) identity failed\n" );
            success = false;
        }
        else
        {	
            for( size_t j = 0; j < 1023; ++j )
            {
                if ( destib[ j ] != remap[ srcib[ j ] ] )
                {
                    printe("ERROR: FinalizeIB(16) identity failed (2)\n" );
                    success = false;
                    break;
                }
            }
        }

        hr = FinalizeIB( srcib.get(), 341, remap, 1024 );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(16) identity [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestIBCorrect( srcib.get(), 1023, IB_IDENTITY ) )
        {
            printe("ERROR: FinalizeIB(16) identity [in-place] failed]\n" );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 1023; ++j )
            {
                if ( destib[ j ] != remap[ srcib[ j ] ] )
                {
                    printe("ERROR: FinalizeIB(16) identity [in-place] failed (2)\n" );
                    success = false;
                    break;
                }
            }
        }
    }

    // Reverse (16)
    {
        auto srcib = CreateIndexBuffer<uint16_t>( 1023, IB_IDENTITY );

        uint32_t remap[1024];
        for( uint32_t j = 0; j < 1024; ++j )
            remap[j] = 1023 - j;

        auto destib = CreateIndexBuffer<uint16_t>( 1023, IB_ZERO );

        HRESULT hr = FinalizeIB( srcib.get(), 341, remap, 1024, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(16) reverse failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {	
            for( size_t j = 0; j < 1023; ++j )
            {
                if ( destib[ j ] != remap[ srcib[ j ] ] )
                {
                    printe("ERROR: FinalizeIB(16) reverse failed\n" );
                    success = false;
                    break;
                }
            }
        }

        auto srcib2 = CreateIndexBuffer<uint16_t>( 1023, IB_IDENTITY );

        hr = FinalizeIB( srcib.get(), 341, remap, 1024 );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(16) reverse [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 1023; ++j )
            {
                if ( srcib[ j ] != remap[ srcib2[ j ] ] )
                {
                    printe("ERROR: FinalizeIB(16) reverse [in-place] failed\n" );
                    success = false;
                    break;
                }
            }
        }
    }

    // Shuffle (16)
    for( size_t retry = 0; retry < 10; ++retry )
    {
        auto srcib = CreateIndexBuffer<uint16_t>( 1023, IB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 1024 );
        for( uint32_t j = 0; j < 1024; ++j )
            remap.push_back( j );

        std::shuffle(std::begin(remap), std::end(remap), rng);

        std::unique_ptr<uint32_t[]> inverseRemap(new uint32_t[1024]);
        for (uint32_t j = 0; j < 1024; ++j)
        {
            inverseRemap[remap[j]] = j;
        }

        auto destib = CreateIndexBuffer<uint16_t>( 1023, IB_ZERO );

        HRESULT hr = FinalizeIB( srcib.get(), 341, remap.data(), 1024, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(16) shuffle failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 1023; ++j )
            {
                if ( destib[ j ] != inverseRemap[ srcib[ j ] ] )
                {
                    printe("ERROR: FinalizeIB(16) shuffle failed\n" );
                    success = false;
                    break;
                }
            }
        }

        auto srcib2 = CreateIndexBuffer<uint16_t>( 1023, IB_IDENTITY );

        hr = FinalizeIB( srcib.get(), 341, remap.data(), 1024 );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(16) shuffle [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 1023; ++j )
            {
                if ( srcib[ j ] != inverseRemap[ srcib2[ j ] ] )
                {
                    printe("ERROR: FinalizeIB(16) shuffle [in-place] failed\n" );
                    success = false;
                    break;
                }
            }
        }
    }

    // Face-mapped Cube (16)
    {
        using namespace TestGeometry;

        std::unique_ptr<uint16_t[]> destib( new uint16_t[ 12 * 3 ] );
        memset( destib.get(), 0xff, sizeof(uint16_t) * 12 * 3 );

        const static uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, 13, 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23 };

        static const uint16_t s_sorted[] =
        {
            0, 1, 2,
            3, 1, 0,
            4, 5, 6,
            7, 5, 4,
            8, 9, 10,
            11, 9, 8,
            12, 13, 14,
            15, 13, 12,
            16, 17, 18,
            19, 17, 16,
            20, 21, 22,
            23, 21, 20,
        };

        HRESULT hr = FinalizeIB( g_fmCubeIndices16, 12, s_remap, 24, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(16) fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: FinalizeIB(16) fmcube failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                printe("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );  
            }
        }

        memcpy( destib.get(), g_fmCubeIndices16, sizeof(g_fmCubeIndices16) );

        hr = FinalizeIB( destib.get(), 12, s_remap, 24 );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(16) fmcube [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: FinalizeIB(16) fmcube [in-place] failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                printe("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );  
            }
        }        

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = FinalizeIB( g_fmCubeIndices16, 12, nullptr, 24, destib.get() );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: FinalizeIB(16) nullptr remap expected failure\n" );
            success = false;
        }

        hr = FinalizeIB( g_fmCubeIndices16, 12, s_remap, D3D11_16BIT_INDEX_STRIP_CUT_VALUE, destib.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeIB(16) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeIB( g_fmCubeIndices16, 12, s_remap, UINT32_MAX, destib.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeIB(16) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeIB( g_fmCubeIndices16, UINT32_MAX, s_remap, 24, destib.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: FinalizeIB(16) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeIB( g_fmCubeIndices16, 12, s_remap, 2, destib.get() );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: FinalizeIB(16) expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        memcpy( destib.get(), g_fmCubeIndices16, sizeof(g_fmCubeIndices16) );

        hr = FinalizeIB( destib.get(), 12, nullptr, 24 );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: FinalizeIB(16) [in-place] nullptr remap expected failure\n" );
            success = false;
        }

        hr = FinalizeIB( destib.get(), 12, s_remap, D3D11_16BIT_INDEX_STRIP_CUT_VALUE );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeIB(16) [in-place] expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeIB( destib.get(), 12, s_remap, UINT32_MAX );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeIB(16) [in-place] expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeIB( destib.get(), UINT32_MAX, s_remap, 24 );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: FinalizeIB(16) [in-place] expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeIB( destib.get(), 12, s_remap, 2 );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: FinalizeIB(16) [in-place] expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // Identity (32)
    {
        auto srcib = CreateIndexBuffer<uint32_t>( 1023, IB_IDENTITY );

        uint32_t remap[1024];
        for( uint32_t j = 0; j < 1024; ++j )
            remap[j] = j;

        auto destib = CreateIndexBuffer<uint32_t>( 1023, IB_ZERO );

        HRESULT hr = FinalizeIB( srcib.get(), 341, remap, 1024, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(32) identity failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestIBCorrect( destib.get(), 1023, IB_IDENTITY ) )
        {
            printe("ERROR: FinalizeIB(32) identity failed\n" );
            success = false;
        }
        else
        {	
            for( size_t j = 0; j < 1023; ++j )
            {
                if ( destib[ j ] != remap[ srcib[ j ] ] )
                {
                    printe("ERROR: FinalizeIB(32) identity failed (2)\n" );
                    success = false;
                    break;
                }
            }
        }

        hr = FinalizeIB( srcib.get(), 341, remap, 1024 );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(32) identity [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestIBCorrect( srcib.get(), 1023, IB_IDENTITY ) )
        {
            printe("ERROR: FinalizeIB(32) identity [in-place] failed]\n" );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 1023; ++j )
            {
                if ( destib[ j ] != remap[ srcib[ j ] ] )
                {
                    printe("ERROR: FinalizeIB(32) identity [in-place] failed (2)\n" );
                    success = false;
                    break;
                }
            }
        }
    }

    // Reverse (32)
    {
        auto srcib = CreateIndexBuffer<uint32_t>( 1023, IB_IDENTITY );

        uint32_t remap[1024];
        for( uint32_t j = 0; j < 1024; ++j )
            remap[j] = 1023 - j;

        uint32_t inverseRemap[1024];
        for (uint32_t j = 0; j < 1024; ++j)
        {
            inverseRemap[remap[j]] = j;
        }

        auto destib = CreateIndexBuffer<uint32_t>( 1023, IB_ZERO );

        HRESULT hr = FinalizeIB( srcib.get(), 341, remap, 1024, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(32) reverse failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {	
            for( size_t j = 0; j < 1023; ++j )
            {
                if ( destib[ j ] != inverseRemap[ srcib[ j ] ] )
                {
                    printe("ERROR: FinalizeIB(32) reverse failed\n" );
                    success = false;
                    break;
                }
            }
        }

        auto srcib2 = CreateIndexBuffer<uint32_t>( 1023, IB_IDENTITY );

        hr = FinalizeIB( srcib.get(), 341, remap, 1024 );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(32) reverse [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 1023; ++j )
            {
                if ( srcib[ j ] != inverseRemap[ srcib2[ j ] ] )
                {
                    printe("ERROR: FinalizeIB(32) reverse [in-place] failed\n" );
                    success = false;
                    break;
                }
            }
        }
    }

    // Shuffle (32)
    for( size_t retry = 0; retry < 10; ++retry )
    {
        auto srcib = CreateIndexBuffer<uint32_t>( 1023, IB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 1024 );
        for( uint32_t j = 0; j < 1024; ++j )
            remap.push_back( j );

        std::shuffle(std::begin(remap), std::end(remap), rng);

        std::unique_ptr<uint32_t[]> inverseRemap(new uint32_t[1024]);
        for (uint32_t j = 0; j < 1024; ++j)
        {
            inverseRemap[remap[j]] = j;
        }

        auto destib = CreateIndexBuffer<uint32_t>( 1023, IB_ZERO );

        HRESULT hr = FinalizeIB( srcib.get(), 341, remap.data(), 1024, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(32) shuffle failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {	
            for( size_t j = 0; j < 1023; ++j )
            {
                if ( destib[ j ] != inverseRemap[ srcib[ j ] ] )
                {
                    printe("ERROR: FinalizeIB(32) shuffle failed\n" );
                    success = false;
                    break;
                }
            }
        }

        auto srcib2 = CreateIndexBuffer<uint32_t>( 1023, IB_IDENTITY );

        hr = FinalizeIB( srcib.get(), 341, remap.data(), 1024 );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(32) shuffle [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 1023; ++j )
            {
                if ( srcib[ j ] != inverseRemap[ srcib2[ j ] ] )
                {
                    printe("ERROR: FinalizeIB(32) shuffle [in-place] failed\n" );
                    success = false;
                    break;
                }
            }
        }
    }

    // Face-mapped Cube (32)
    {
        using namespace TestGeometry;

        std::unique_ptr<uint32_t[]> destib( new uint32_t[ 12 * 3 ] );
        memset( destib.get(), 0xff, sizeof(uint32_t) * 12 * 3 );

        const static uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, 13, 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23 };

        static const uint32_t s_sorted[] =
        {
            0, 1, 2,
            3, 1, 0,
            4, 5, 6,
            7, 5, 4,
            8, 9, 10,
            11, 9, 8,
            12, 13, 14,
            15, 13, 12,
            16, 17, 18,
            19, 17, 16,
            20, 21, 22,
            23, 21, 20,
        };

        HRESULT hr = FinalizeIB( g_fmCubeIndices32, 12, s_remap, 24, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(32) fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: FinalizeIB(32) fmcube failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                printe("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );  
            }
        }

        memcpy( destib.get(), g_fmCubeIndices32, sizeof(g_fmCubeIndices32) );

        hr = FinalizeIB( destib.get(), 12, s_remap, 24 );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(32) fmcube [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: FinalizeIB(32) fmcube [in-place] failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                printe("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );  
            }
        }        

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = FinalizeIB( g_fmCubeIndices32, 12, nullptr, 24, destib.get() );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: FinalizeIB(32) nullptr remap expected failure\n" );
            success = false;
        }

        hr = FinalizeIB( g_fmCubeIndices32, 12, s_remap, D3D11_32BIT_INDEX_STRIP_CUT_VALUE, destib.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeIB(32) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeIB( g_fmCubeIndices32, 12, s_remap, UINT32_MAX, destib.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeIB(32) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeIB( g_fmCubeIndices32, UINT32_MAX, s_remap, 24, destib.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: FinalizeIB(32) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeIB( g_fmCubeIndices32, 12, s_remap, 2, destib.get() );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: FinalizeIB(32) expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        memcpy( destib.get(), g_fmCubeIndices32, sizeof(g_fmCubeIndices32) );

        hr = FinalizeIB( destib.get(), 12, nullptr, 24 );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: FinalizeIB(32) [in-place] nullptr remap expected failure\n" );
            success = false;
        }

        hr = FinalizeIB( destib.get(), 12, s_remap, D3D11_32BIT_INDEX_STRIP_CUT_VALUE );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeIB(32) [in-place] expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeIB( destib.get(), 12, s_remap, UINT32_MAX );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeIB(32) [in-place] expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeIB( destib.get(), UINT32_MAX, s_remap, 24 );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: FinalizeIB(32) [in-place] expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeIB( destib.get(), 12, s_remap, 2 );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: FinalizeIB(32) [in-place] expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // Unused (16)
    {
        using namespace TestGeometry;

        static const uint16_t s_unused[ 12*3 ] =
            {
                3,1,0,
                2,1,3,

                6,4,5,
                7,4,6,

                11,9,8,
                uint16_t(-1),uint16_t(-1),uint16_t(-1),

                14,12,13,
                15,12,14,

                19,17,16,
                18,17,19,

                22,20,21,
                23,20,22
            };

#ifdef _DEBUG
        std::wstring msgs;
        if ( FAILED( Validate( s_unused, 12, 24, nullptr, VALIDATE_UNUSED, &msgs ) ) )
        {
            success = false;
            printe("\nERROR: test data failed validation:\n%S\n", msgs.c_str() );
        }
#endif

        std::unique_ptr<uint16_t[]> destib( new uint16_t[ 12 * 3 ] );
        memset( destib.get(), 0xff, sizeof(uint16_t) * 12 * 3 );

        const static uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, uint32_t(-1), 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23 };

        static const uint16_t s_sorted[] =
        {
            0, 1, 2,
            3, 1, 0,
            4, 5, 6,
            7, 5, 4,
            8, 9, 10,
            uint16_t(-1), uint16_t(-1), uint16_t(-1),
            12, 13, 13,
            15, 13, 12,
            16, 17, 18,
            19, 17, 16,
            20, 21, 22,
            23, 21, 20,
        };

        HRESULT hr = FinalizeIB( s_unused, 12, s_remap, 24, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(16) unused failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: FinalizeIB(16) unused failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                printe("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );  
            }
        }

        memcpy( destib.get(), s_unused, sizeof(s_unused) );

        hr = FinalizeIB( destib.get(), 12, s_remap, 24 );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(16) unused [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: FinalizeIB(16) unused [in-place] failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                printe("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );  
            }
        }        
    }

    // Unused (32)
    {
        using namespace TestGeometry;

        static const uint32_t s_unused[ 12*3 ] =
            {
                3,1,0,
                2,1,3,

                6,4,5,
                7,4,6,

                11,9,8,
                uint32_t(-1),uint32_t(-1),uint32_t(-1),

                14,12,13,
                15,12,14,

                19,17,16,
                18,17,19,

                22,20,21,
                23,20,22
            };

#ifdef _DEBUG
        std::wstring msgs;
        if ( FAILED( Validate( s_unused, 12, 24, nullptr, VALIDATE_UNUSED, &msgs ) ) )
        {
            success = false;
            printe("\nERROR: test data failed validation:\n%S\n", msgs.c_str() );
        }
#endif

        std::unique_ptr<uint32_t[]> destib( new uint32_t[ 12 * 3 ] );
        memset( destib.get(), 0xff, sizeof(uint32_t) * 12 * 3 );

        const static uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, uint32_t(-1), 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23 };

        static const uint32_t s_sorted[] =
        {
            0, 1, 2,
            3, 1, 0,
            4, 5, 6,
            7, 5, 4,
            8, 9, 10,
            uint32_t(-1), uint32_t(-1), uint32_t(-1),
            12, 13, 13,
            15, 13, 12,
            16, 17, 18,
            19, 17, 16,
            20, 21, 22,
            23, 21, 20,
        };

        HRESULT hr = FinalizeIB( s_unused, 12, s_remap, 24, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(32) unused failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: FinalizeIB(32) unused failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                printe("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );  
            }
        }

        memcpy( destib.get(), s_unused, sizeof(s_unused) );

        hr = FinalizeIB( destib.get(), 12, s_remap, 24 );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeIB(32) unused [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: FinalizeIB(32) unused [in-place] failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                printe("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );  
            }
        }        
    }

    return success;
}


//-------------------------------------------------------------------------------------
// FinalizeVB (no duplicates)
bool Test04()
{
    bool success = true;

    std::random_device rd;
    std::default_random_engine rng(rd());

    // Identity (32)
    {
        auto srcvb = CreateVertexBuffer32( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( j );

        auto destvb = CreateVertexBuffer( 32, 65535 );

        HRESULT hr = FinalizeVB( srcvb.get(), 32, 65535, nullptr, 0, remap.data(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(32) identity failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestVBCorrect32( destvb.get(), 65535, VB_IDENTITY ) )
        {
            printe("ERROR: FinalizeVB(32) identity failed\n" );
            success = false;
        }
        else
        {
            auto ptr = destvb.get();
            for( size_t j = 0; j < 65535; ++j )
            {
                if ( !IsTestVBCorrect32( ptr, DWORD( j ) ) )
                {
                    printe("ERROR: FinalizeVB(32) identity failed (2)\n" );
                    success = false;
                    break;
                }
                ptr += 32;
            }
        }

        hr = FinalizeVB( srcvb.get(), 32, 65535, remap.data() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(32) identity [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestVBCorrect32( srcvb.get(), 65535, VB_IDENTITY ) )
        {
            printe("ERROR: FinalizeVB(32) identity [in-place] failed\n" );
            success = false;
        }
        else
        {
            auto ptr = srcvb.get();
            for( size_t j = 0; j < 65535; ++j )
            {
                if ( !IsTestVBCorrect32( ptr, DWORD( j ) ) )
                {
                    printe("ERROR: FinalizeVB(32) identity [in-place] failed (2)\n" );
                    success = false;
                    break;
                }
                ptr += 32;
            }
        }
    }

    // Reverse (32)
    {
        auto srcvb = CreateVertexBuffer32( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( 65534 - j );

        auto destvb = CreateVertexBuffer( 32, 65535 );

        HRESULT hr = FinalizeVB( srcvb.get(), 32, 65535, nullptr, 0, remap.data(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(32) reverse failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestVBCorrect32( destvb.get(), 65535, VB_REVERSE ) )
        {
            printe("ERROR: FinalizeVB(32) reverse failed\n" );
            success = false;
        }

        hr = FinalizeVB( srcvb.get(), 32, 65535, remap.data() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(32) reverse [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestVBCorrect32( srcvb.get(), 65535, VB_REVERSE ) )
        {
            printe("ERROR: FinalizeVB(32) reverse [in-place] failed\n" );
            success = false;
        }
    }

    // Shuffle (32)
    for( size_t retry = 0; retry < 10; ++retry )
    {
        auto srcvb = CreateVertexBuffer32( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( j );

        std::shuffle(std::begin(remap), std::end(remap), rng);

        std::unique_ptr<uint32_t[]> inverseRemap(new uint32_t[65535]);
        for (uint32_t j = 0; j < 65535; ++j)
        {
            inverseRemap[remap[j]] = j;
        }

        auto destvb = CreateVertexBuffer( 32, 65535 );

        HRESULT hr = FinalizeVB( srcvb.get(), 32, 65535, nullptr, 0, remap.data(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(32) shuffle failed (%08X)\n", static_cast<unsigned int>(hr) );
            printe("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 65535; ++j )
            {
                auto ptr = destvb.get() + 32* inverseRemap[j];
                if ( !IsTestVBCorrect32( ptr, DWORD( j ) ) )
                {
                    printe("ERROR: FinalizeVB(32) shuffle failed\n" );
                    printe("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
                    success = false;
                    break;
                }
            }
        }

        hr = FinalizeVB( srcvb.get(), 32, 65535, remap.data() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(32) shuffle [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            printe("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 65535; ++j )
            {
                auto ptr = srcvb.get() + 32* inverseRemap[j];
                if ( !IsTestVBCorrect32( ptr, DWORD( j ) ) )
                {
                    printe("ERROR: FinalizeVB(32) shuffle [in-place] failed\n" );
                    printe("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
                    success = false;
                    break;
                }
            }
        }
    }

    // Identity (16)
    {
        auto srcvb = CreateVertexBuffer16( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( j );

        auto destvb = CreateVertexBuffer( 16, 65535 );

        HRESULT hr = FinalizeVB( srcvb.get(), 16, 65535, nullptr, 0, remap.data(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(16) identity failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestVBCorrect16( destvb.get(), 65535, VB_IDENTITY ) )
        {
            printe("ERROR: FinalizeVB(16) identity failed\n" );
            success = false;
        }
        else
        {
            auto ptr = destvb.get();
            for( size_t j = 0; j < 65535; ++j )
            {
                if ( !IsTestVBCorrect16( ptr, DWORD(j) ) )
                {
                    printe("ERROR: FinalizeVB(16) identity failed (2)\n" );
                    success = false;
                    break;
                }
                ptr += 16;
            }
        }

        hr = FinalizeVB( srcvb.get(), 16, 65535, remap.data() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(16) identity [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestVBCorrect16( srcvb.get(), 65535, VB_IDENTITY ) )
        {
            printe("ERROR: FinalizeVB(16) identity [in-place] failed\n" );
            success = false;
        }
        else
        {
            auto ptr = srcvb.get();
            for( size_t j = 0; j < 65535; ++j )
            {
                if ( !IsTestVBCorrect16( ptr, DWORD(j) ) )
                {
                    printe("ERROR: FinalizeVB(16) identity [in-place] failed (2)\n" );
                    success = false;
                    break;
                }
                ptr += 16;
            }
        }
    }

    // Reverse (16)
    {
        auto srcvb = CreateVertexBuffer16( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( 65534 - j );

        auto destvb = CreateVertexBuffer( 16, 65535 );

        HRESULT hr = FinalizeVB( srcvb.get(), 16, 65535, nullptr, 0, remap.data(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(16) reverse failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestVBCorrect16( destvb.get(), 65535, VB_REVERSE ) )
        {
            printe("ERROR: FinalizeVB(16) reverse failed\n" );
            success = false;
        }

        hr = FinalizeVB( srcvb.get(), 16, 65535, remap.data() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(16) reverse [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestVBCorrect16( srcvb.get(), 65535, VB_REVERSE ) )
        {
            printe("ERROR: FinalizeVB(16) reverse [in-place] failed\n" );
            success = false;
        }
    }

    // Shuffle (16)
    for( size_t retry = 0; retry < 10; ++retry )
    {
        auto srcvb = CreateVertexBuffer16( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 );
        for( uint32_t j = 0; j < 65535; ++j )
            remap.push_back( j );

        std::shuffle(std::begin(remap), std::end(remap), rng);

        std::unique_ptr<uint32_t[]> inverseRemap(new uint32_t[65535]);
        for (uint32_t j = 0; j < 65535; ++j)
        {
            inverseRemap[remap[j]] = j;
        }

        auto destvb = CreateVertexBuffer( 16, 65535 );

        HRESULT hr = FinalizeVB( srcvb.get(), 16, 65535, nullptr, 0, remap.data(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(16) shuffle failed (%08X)\n", static_cast<unsigned int>(hr) );
            printe("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 65535; ++j )
            {
                auto ptr = destvb.get() + 16* inverseRemap[j];
                if ( !IsTestVBCorrect16( ptr, DWORD(j) ) )
                {
                    printe("ERROR: FinalizeVB(16) shuffle failed\n" );
                    printe("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
                    success = false;
                    break;
                }
            }
        }

        hr = FinalizeVB( srcvb.get(), 16, 65535, remap.data() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(16) shuffle [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            printe("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 65535; ++j )
            {
                auto ptr = srcvb.get() + 16* inverseRemap[j];
                if ( !IsTestVBCorrect16( ptr, DWORD(j) ) )
                {
                    printe("ERROR: FinalizeVB(16) shuffle [in-place] failed\n" );
                    printe("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[65532], remap[65533], remap[65534] );
                    success = false;
                    break;
                }
            }
        }
    }

    // Face-mapped Cube (4)
    {
        using namespace TestGeometry;

        std::unique_ptr<uint32_t[]> srcvb( new uint32_t[ 24 ] );
        for( uint32_t j=0; j < 24; ++j)
            srcvb[ j ] = j;

        std::unique_ptr<uint32_t[]> destvb( new uint32_t[ 24 ] );
        memset( destvb.get(), 0xff, sizeof(uint32_t) * 24 );

        const static uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, 13, 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23 };

        HRESULT hr = FinalizeVB( srcvb.get(), sizeof(uint32_t), 24, nullptr, 0, s_remap, destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destvb.get(), s_remap, sizeof(s_remap) ) != 0 )
        {
            printe("ERROR: FinalizeVB fmcube failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                printe("\t%zu: %u .. %u\n", j, destvb[ j ], s_remap[ j ] );
            }
        }

        // in place remap
        for( uint32_t j=0; j < 24; ++j)
            srcvb[ j ] = j;

        hr = FinalizeVB( srcvb.get(), sizeof(uint32_t), 24, s_remap );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB fmcube [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( srcvb.get(), s_remap, sizeof(s_remap) ) != 0 )
        {
            printe("ERROR: FinalizeVB fmcube [in-place] failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                printe("\t%zu: %u .. %u\n", j, srcvb[ j ], s_remap[ j ] );
            }
        }        

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = FinalizeVB( srcvb.get(), sizeof(uint32_t), 24, nullptr, 0, nullptr, destvb.get() );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: FinalizeVB nullptr remap expected failure\n" );
            success = false;
        }

        hr = FinalizeVB( srcvb.get(), UINT32_MAX, 24, nullptr, 0, s_remap, destvb.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeVB expected failure for bad stride value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeVB( srcvb.get(), sizeof(uint32_t), D3D11_32BIT_INDEX_STRIP_CUT_VALUE, nullptr, 0, s_remap, destvb.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeVB expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeVB( srcvb.get(), sizeof(uint32_t), UINT32_MAX, nullptr, 0, s_remap, destvb.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeVB expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        for( uint32_t j=0; j < 24; ++j)
            srcvb[ j ] = j;

        hr = FinalizeVB( srcvb.get(), sizeof(uint32_t), 24, nullptr );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: FinalizeVB [in-place] nullptr remap expected failure\n" );
            success = false;
        }

        hr = FinalizeVB( srcvb.get(), UINT32_MAX, 24, s_remap );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeVB [in-place] expected failure for bad stride value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeVB( srcvb.get(), sizeof(uint32_t), D3D11_32BIT_INDEX_STRIP_CUT_VALUE, s_remap );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeVB [in-place] expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = FinalizeVB( srcvb.get(), sizeof(uint32_t), UINT32_MAX, s_remap );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: FinalizeVB [in-place] expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // Unused (4)
    {
        using namespace TestGeometry;

        std::unique_ptr<uint32_t[]> srcvb( new uint32_t[ 24 ] );
        for( uint32_t j=0; j < 24; ++j)
            srcvb[ j ] = j;

        std::unique_ptr<uint32_t[]> destvb( new uint32_t[ 24 ] );
        memset( destvb.get(), 0, sizeof(uint32_t) * 24 );

        const static uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, uint32_t(-1), 10,
                                            14, 12, 13, 15,
                                            19, uint32_t(-1), 16, 18,
                                            22, 20, 21, 23 };

        const static uint32_t s_sorted[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 0, 10,
                                            14, 12, 13, 15,
                                            19, 0, 16, 18,
                                            22, 20, 21, 23 };

        HRESULT hr = FinalizeVB( srcvb.get(), sizeof(uint32_t), 24, nullptr, 0, s_remap, destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(4) unused failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destvb.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: FinalizeVB(4) unused failed\n" );
            success = false;
            for( size_t j = 0; j < 24; ++j )
            {
                printe("\t%zu: %u .. %u\n", j, destvb[ j ], s_sorted[ j ] );
            }
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// FinalizeVB (with duplicates)
bool Test05()
{
    bool success = true;

    std::random_device rd;
    std::default_random_engine rng(rd());

    // Identity (32)
    {
        auto srcvb = CreateVertexBuffer32( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 + 256 );
        for( uint32_t j = 0; j < (65535 + 256); ++j )
            remap.push_back( j );

        std::vector<uint32_t> dups;
        dups.reserve( 256 );
        for( uint32_t j = 0; j < 256; ++j )
            dups.push_back( j );

        auto destvb = CreateVertexBuffer( 32, 65535 + 256 );

        HRESULT hr = FinalizeVB( srcvb.get(), 32, 65535, dups.data(), dups.size(), remap.data(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(32) dups identity [remap] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            auto ptr = destvb.get();
            for( size_t j = 0; j < 65535; ++j )
            {
                if ( !IsTestVBCorrect32( ptr, DWORD(j) ) )
                {
                    printe("ERROR: FinalizeVB(32) dups identity [remap] failed\n" );
                    success = false;
                    break;
                }
                ptr += 32;
            }

            for( size_t j = 0; j < 256; ++j )
            {
                if ( !IsTestVBCorrect32( ptr, DWORD(j) ) )
                {
                    printe("ERROR: FinalizeVB(32) dups identity [remap] failed (2)\n" );
                    success = false;
                    break;
                }
                ptr += 32;
            }
        }

        memset( destvb.get(), 0, sizeof(32) * ( 65535 + 256 ) );

        hr = FinalizeVB( srcvb.get(), 32, 65535, dups.data(), dups.size(), nullptr, destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(32) dups identity failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            auto ptr = destvb.get();
            for( size_t j = 0; j < 65535; ++j )
            {
                if ( !IsTestVBCorrect32( ptr, DWORD(j) ) )
                {
                    printe("ERROR: FinalizeVB(32) dups identity failed\n" );
                    success = false;
                    break;
                }
                ptr += 32;
            }

            for( size_t j = 0; j < 256; ++j )
            {
                if ( !IsTestVBCorrect32( ptr, DWORD(j) ) )
                {
                    printe("ERROR: FinalizeVB(32) dups identity failed (2)\n" );
                    success = false;
                    break;
                }
                ptr += 32;
            }
        }
    }

    // Reverse (32)
    {
        auto srcvb = CreateVertexBuffer32( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 + 256 );
        for( uint32_t j = 0; j < (65535 + 256); ++j )
            remap.push_back( (65535 + 255) - j );
 
        std::vector<uint32_t> dups;
        dups.reserve( 256 );
        for( uint32_t j = 0; j < 256; ++j )
            dups.push_back( j );

        auto destvb = CreateVertexBuffer( 32, 65535 + 256 );

        HRESULT hr = FinalizeVB( srcvb.get(), 32, 65535, dups.data(), dups.size(), remap.data(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(32) dups reverse [remap] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            auto ptr = destvb.get();

            for( size_t j = 0; j < 256; ++j )
            {
                if ( !IsTestVBCorrect32( ptr, DWORD( 255 - j ) ) )
                {
                    printe("ERROR: FinalizeVB(32) dups reverse [remap] failed\n" );
                    success = false;
                    break;
                }
                ptr += 32;
            }

            for( size_t j = 256; j < (65535 + 256); ++j )
            {
                if ( !IsTestVBCorrect32( ptr, DWORD( (65535 + 255) - j ) ) )
                {
                    printe("ERROR: FinalizeVB(32) dups reverse [remap] failed (2)\n" );
                    success = false;
                    break;
                }
                ptr += 32;
            }
        }
    }

    // Shuffle (32)
    for( size_t retry = 0; retry < 10; ++retry )
    {
        auto srcvb = CreateVertexBuffer32( 65535, VB_IDENTITY );

        std::vector<uint32_t> remap;
        remap.reserve( 65535 + 256 );
        for( uint32_t j = 0; j < (65535 + 256); ++j )
            remap.push_back( j );
 
        std::shuffle(std::begin(remap), std::end(remap), rng);

        std::unique_ptr<uint32_t[]> inverseRemap(new uint32_t[65535 + 256]);
        for (uint32_t j = 0; j < (65535 + 256); ++j)
        {
            inverseRemap[remap[j]] = j;
        }

        std::vector<uint32_t> dups;
        dups.reserve( 256 );
        for( uint32_t j = 0; j < 256; ++j )
            dups.push_back( j );

        auto destvb = CreateVertexBuffer( 32, 65535 + 256 );

        HRESULT hr = FinalizeVB( srcvb.get(), 32, 65535, dups.data(), dups.size(), remap.data(), destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(32) dups shuffle [remap] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 65535; ++j )
            {
                auto ptr = destvb.get() + 32* inverseRemap[j];
                if ( !IsTestVBCorrect32( ptr, DWORD(j) ) )
                {
                    printe("ERROR: FinalizeVB(32) dups shuffle [remap] failed\n" );
                    success = false;
                    break;
                }
                ptr += 32;
            }

            for( size_t j = 65536; j < (65535 + 256); ++j )
            {
                auto ptr = destvb.get() + 32* inverseRemap[j];
                if ( !IsTestVBCorrect32( ptr, DWORD( j - 65535 ) ) )
                {
                    printe("ERROR: FinalizeVB(32) dups shuffle [remap] failed (2)\n" );
                    success = false;
                    break;
                }
                ptr += 32;
            }
        }
    }

    // Face-mapped Cube (4)
    {
        using namespace TestGeometry;

        std::unique_ptr<uint32_t[]> srcvb( new uint32_t[ 24 ] );
        for( uint32_t j=0; j < 24; ++j)
            srcvb[ j ] = j;

        std::unique_ptr<uint32_t[]> destvb( new uint32_t[ 28 ] );
        memset( destvb.get(), 0xff, sizeof(uint32_t) * 28 );

        std::vector<uint32_t> dups;
        dups.reserve( 4 );
        for( uint32_t j = 0; j < 4; ++j )
            dups.push_back( j );

        const static uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, 13, 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23,
                                            27, 26, 25, 24 };

        const static uint32_t s_sorted[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, 13, 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23,
                                             3,  2,  1,  0 };

        HRESULT hr = FinalizeVB( srcvb.get(), sizeof(uint32_t), 24, dups.data(), dups.size(), s_remap, destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(4) dups fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destvb.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: FinalizeVB(4) dups fmcube failed\n" );
            success = false;
            for( size_t j = 0; j < 28; ++j )
            {
                printe("\t%zu: %u .. %u\n", j, destvb[ j ], s_sorted[ j ] );
            }
        }
    }

    // Unused (4)
    {
        using namespace TestGeometry;

        std::unique_ptr<uint32_t[]> srcvb( new uint32_t[ 24 ] );
        for( uint32_t j=0; j < 24; ++j)
            srcvb[ j ] = j;

        std::unique_ptr<uint32_t[]> destvb( new uint32_t[ 28 ] );
        memset( destvb.get(), 0, sizeof(uint32_t) * 28 );

        std::vector<uint32_t> dups;
        dups.reserve( 4 );
        for( uint32_t j = 0; j < 4; ++j )
            dups.push_back( j );

        const static uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, uint32_t(-1), 10,
                                            14, 12, 13, 15,
                                            19, uint32_t(-1), 16, 18,
                                            22, 20, 21, 23,
                                            27, 26, uint32_t(-1), 24 };

        const static uint32_t s_sorted[] = { 3, 1, 0, 2,
                                             6, 4, 5, 7,
                                             11, 9, 0, 10,
                                             14, 12, 13, 15,
                                             19, 0, 16, 18,
                                             22, 20, 21, 23,
                                             3,  2,  0,  0 };

        HRESULT hr = FinalizeVB( srcvb.get(), sizeof(uint32_t), 24, dups.data(), dups.size(), s_remap, destvb.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: FinalizeVB(4) dups unused failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destvb.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: FinalizeVB(4) dups unused failed\n" );
            success = false;
            for( size_t j = 0; j < 28; ++j )
            {
                printe("\t%zu: %u .. %u\n", j, destvb[ j ], s_sorted[ j ] );  
            }
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// ReorderIB
bool Test18()
{
    bool success = true;

    std::random_device rd;
    std::default_random_engine rng(rd());

    // Identity (16)
    {
        auto srcib = CreateIndexBuffer<uint16_t>( 1023, IB_IDENTITY );

        uint32_t remap[341];
        for( uint32_t j = 0; j < 341; ++j )
            remap[j] = j;

        auto destib = CreateIndexBuffer<uint16_t>( 1023, IB_ZERO );

        HRESULT hr = ReorderIB( srcib.get(), 341, remap, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(16) identity failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestIBCorrect( destib.get(), 1023, IB_IDENTITY ) )
        {
            printe("ERROR: ReorderIB(16) identity failed\n" );
            success = false;
        }
        else
        {	
            for( size_t j = 0; j < 341; ++j )
            {
                uint32_t f = remap[j];

                if ( destib[ j*3 ] != ( f*3 )
                    || destib[ j*3 + 1 ] != ( f*3 + 1 )
                    || destib[ j*3 + 2 ] != ( f*3 + 2 ) )
                {
                    printe("ERROR: ReorderIB(16) identity failed (2)\n" );
                    success = false;
                    break;
                }
            }
        }

        hr = ReorderIB( srcib.get(), 341, remap );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(16) identity [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestIBCorrect( srcib.get(), 1023, IB_IDENTITY ) )
        {
            printe("ERROR: ReorderIB(16) identity [in-place] failed]\n" );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 341; ++j )
            {
                uint32_t f = remap[j];

                if ( srcib[ j*3 ] != ( f*3 )
                    || srcib[ j*3 + 1 ] != ( f*3 + 1 )
                    || srcib[ j*3 + 2 ] != ( f*3 + 2 ) )
                {
                    printe("ERROR: ReorderIB(16) identity [in-place] failed (2)\n" );
                    success = false;
                    break;
                }
            }
        }
    }

    // Reverse (16)
    {
        auto srcib = CreateIndexBuffer<uint16_t>( 1023, IB_IDENTITY );

        uint32_t remap[341];
        for( uint32_t j = 0; j < 341; ++j )
            remap[j] = (340 - j);

        auto destib = CreateIndexBuffer<uint16_t>( 1023, IB_ZERO );

        HRESULT hr = ReorderIB( srcib.get(), 341, remap, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(16) reverse failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestIBCorrect( destib.get(), 1023, IB_REVERSE ) )
        {
            printe("ERROR: ReorderIB(16) reverse\n" );
            success = false;
        }

        hr = ReorderIB( srcib.get(), 341, remap );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(16) reverse [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestIBCorrect( srcib.get(), 1023, IB_REVERSE ) )
        {
            printe("ERROR: ReorderIB(16) reverse [in-place] failed\n" );
            success = false;
        }
    }

    // Shuffle (16)
    for( size_t retry = 0; retry < 10; ++retry )
    {
        auto srcib = CreateIndexBuffer<uint16_t>( 1023, IB_IDENTITY );

        uint32_t remap[341];
        for( uint32_t j = 0; j < 341; ++j )
            remap[j] = j;

        std::shuffle(std::begin(remap), std::end(remap), rng);

        auto destib = CreateIndexBuffer<uint16_t>( 1023, IB_ZERO );

        HRESULT hr = ReorderIB( srcib.get(), 341, remap, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(16) shuffle failed (%08X)\n", static_cast<unsigned int>(hr) );
            print("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[338], remap[339], remap[340] );
            success = false;
        }
        else
        {
            bool matches = true;

            for( size_t j = 0; j < 341; ++j )
            {
                uint32_t f = remap[j];

                if ( destib[ j*3 ] != ( f*3 )
                    || destib[ j*3 + 1 ] != ( f*3 + 1 )
                    || destib[ j*3 + 2 ] != ( f*3 + 2 ) )
                {
                    matches = false;
                    break;
                }
            }

            if ( !matches )
            {
                printe("ERROR: ReorderIB(16) shuffle failed\n" );
                print("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[338], remap[339], remap[340] );
                success = false;
            }
        }

        hr = ReorderIB( srcib.get(), 341, remap );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(16) shuffle [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            print("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[338], remap[339], remap[340] );
            success = false;
        }
        else
        {
            bool matches = true;

            for( size_t j = 0; j < 341; ++j )
            {
                uint32_t f = remap[j];

                if ( srcib[ j*3 ] != ( f*3 )
                    || srcib[ j*3 + 1 ] != ( f*3 + 1 )
                    || srcib[ j*3 + 2 ] != ( f*3 + 2 ) )
                {
                    matches = false;
                    break;
                }
            }

            if ( !matches )
            {
                printe("ERROR: ReorderIB(16) shuffle [in-place] failed\n" );
                print("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[338], remap[339], remap[340] );
                success = false;
            }
        }
    }

    // Cube (16)
    {
        using namespace TestGeometry;

        std::unique_ptr<uint16_t[]> destib( new uint16_t[ 12 * 3 ] );
        memset( destib.get(), 0xff, sizeof(uint16_t) * 12 * 3 );

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

        HRESULT hr = ReorderIB( g_cubeIndices16, 12, s_faceRemap, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(16) cube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: ReorderIB(16) cube failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );  
            }
        }

        // in-place
        memcpy( destib.get(), g_cubeIndices16, sizeof(g_cubeIndices16) );

        hr = ReorderIB( destib.get(), 12, s_faceRemap );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(16) cube [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: ReorderIB(16) cube [in-place] failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );  
            }
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = ReorderIB( g_cubeIndices16, 12, nullptr, destib.get() );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: ReorderIB(16) nullptr remap expected failure\n" );
            success = false;
        }

        hr = ReorderIB( g_cubeIndices16, UINT32_MAX, s_faceRemap, destib.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: ReorderIB(16) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        memcpy( destib.get(), g_cubeIndices16, sizeof(g_cubeIndices16) );

        hr = ReorderIB( destib.get(), 12, nullptr );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: ReorderIB(16) [in-place] nullptr remap expected failure\n" );
            success = false;
        }

        hr = ReorderIB( destib.get(), UINT32_MAX, s_faceRemap );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: ReorderIB(16) [in-place] expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // Identity (32)
    {
        auto srcib = CreateIndexBuffer<uint32_t>( 1023, IB_IDENTITY );

        uint32_t remap[341];
        for( uint32_t j = 0; j < 341; ++j )
            remap[j] = j;

        auto destib = CreateIndexBuffer<uint32_t>( 1023, IB_ZERO );

        HRESULT hr = ReorderIB( srcib.get(), 341, remap, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(32) identity failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestIBCorrect( destib.get(), 1023, IB_IDENTITY ) )
        {
            printe("ERROR: ReorderIB(32) identity failed\n" );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 341; ++j )
            {
                uint32_t f = remap[j];

                if ( destib[ j*3 ] != ( f*3 )
                    || destib[ j*3 + 1 ] != ( f*3 + 1 )
                    || destib[ j*3 + 2 ] != ( f*3 + 2 ) )
                {
                    printe("ERROR: ReorderIB(32) identity failed (2)\n" );
                    success = false;
                    break;
                }
            }
        }

        hr = ReorderIB( srcib.get(), 341, remap );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(32) identity [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestIBCorrect( srcib.get(), 1023, IB_IDENTITY ) )
        {
            printe("ERROR: ReorderIB(32) identity [in-place] failed]\n" );
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 341; ++j )
            {
                uint32_t f = remap[j];

                if ( srcib[ j*3 ] != ( f*3 )
                    || srcib[ j*3 + 1 ] != ( f*3 + 1 )
                    || srcib[ j*3 + 2 ] != ( f*3 + 2 ) )
                {
                    printe("ERROR: ReorderIB(32) identity [in-place] failed (2)\n" );
                    success = false;
                    break;
                }
            }
        }
    }

    // Reverse (32)
    {
        auto srcib = CreateIndexBuffer<uint32_t>( 1023, IB_IDENTITY );

        uint32_t remap[341];
        for( uint32_t j = 0; j < 341; ++j )
            remap[j] = (340 - j);

        auto destib = CreateIndexBuffer<uint32_t>( 1023, IB_ZERO );

        HRESULT hr = ReorderIB( srcib.get(), 341, remap, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(32) reverse failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestIBCorrect( destib.get(), 1023, IB_REVERSE ) )
        {
            printe("ERROR: ReorderIB(32) reverse\n" );
            success = false;
        }

        hr = ReorderIB( srcib.get(), 341, remap );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(32) reverse [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsTestIBCorrect( srcib.get(), 1023, IB_REVERSE ) )
        {
            printe("ERROR: ReorderIB(32) reverse [in-place] failed\n" );
            success = false;
        }
    }

    // Shuffle (32)
    for( size_t retry = 0; retry < 10; ++retry )
    {
        auto srcib = CreateIndexBuffer<uint32_t>( 1023, IB_IDENTITY );

        uint32_t remap[341];
        for( uint32_t j = 0; j < 341; ++j )
            remap[j] = j;

        std::shuffle(std::begin(remap), std::end(remap), rng);

        auto destib = CreateIndexBuffer<uint32_t>( 1023, IB_ZERO );

        HRESULT hr = ReorderIB( srcib.get(), 341, remap, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(32) shuffle failed (%08X)\n", static_cast<unsigned int>(hr) );
            print("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[338], remap[339], remap[340] );
            success = false;
        }
        else
        {
            bool matches = true;

            for( size_t j = 0; j < 341; ++j )
            {
                uint32_t f = remap[j];

                if ( destib[ j*3 ] != ( f*3 )
                    || destib[ j*3 + 1 ] != ( f*3 + 1 )
                    || destib[ j*3 + 2 ] != ( f*3 + 2 ) )
                {
                    matches = false;
                    break;
                }
            }

            if ( !matches )
            {
                printe("ERROR: ReorderIB(32) shuffle failed\n" );
                print("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[338], remap[339], remap[340] );
                success = false;
            }
        }

        hr = ReorderIB( srcib.get(), 341, remap );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(32) shuffle [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            print("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[338], remap[339], remap[340] );
            success = false;
        }
        else
        {
            bool matches = true;

            for( size_t j = 0; j < 341; ++j )
            {
                uint32_t f = remap[j];

                if ( srcib[ j*3 ] != ( f*3 )
                    || srcib[ j*3 + 1 ] != ( f*3 + 1 )
                    || srcib[ j*3 + 2 ] != ( f*3 + 2 ) )
                {
                    matches = false;
                    break;
                }
            }

            if ( !matches )
            {
                printe("ERROR: ReorderIB(32) shuffle [in-place] failed\n" );
                print("\t[%zu] %u %u %u .. %u %u %u\n", retry, remap[0], remap[1], remap[2], remap[338], remap[339], remap[340] );
                success = false;
            }
        }
    }

    // Cube (32)
    {
        using namespace TestGeometry;

        std::unique_ptr<uint32_t[]> destib( new uint32_t[ 12 * 3 ] );
        memset( destib.get(), 0xff, sizeof(uint32_t) * 12 * 3 );

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

        HRESULT hr = ReorderIB( g_cubeIndices32, 12, s_faceRemap, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(32) cube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: ReorderIB(32) cube failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );  
            }
        }

        // in-place
        memcpy( destib.get(), g_cubeIndices32, sizeof(g_cubeIndices32) );

        hr = ReorderIB( destib.get(), 12, s_faceRemap );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(32) cube [in-place] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: ReorderIB(32) cube [in-place] failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );  
            }
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = ReorderIB( g_cubeIndices32, 12, nullptr, destib.get() );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: ReorderIB(32) nullptr remap expected failure\n" );
            success = false;
        }

        hr = ReorderIB( g_cubeIndices32, UINT32_MAX, s_faceRemap, destib.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: ReorderIB(32) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        memcpy( destib.get(), g_cubeIndices32, sizeof(g_cubeIndices32) );

        hr = ReorderIB( destib.get(), 12, nullptr );
        if ( SUCCEEDED(hr) )
        {
            printe("\nERROR: ReorderIB(32) [in-place] nullptr remap expected failure\n" );
            success = false;
        }

        hr = ReorderIB( destib.get(), UINT32_MAX, s_faceRemap );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: ReorderIB(32) [in-place] expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // Unused faces (16)
    {
        using namespace TestGeometry;

        std::unique_ptr<uint16_t[]> destib( new uint16_t[ 12 * 3 ] );
        memset( destib.get(), 0xff, sizeof(uint16_t) * 12 * 3 );

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

        HRESULT hr = ReorderIB( g_cubeIndices16, 12, s_faceRemap, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(16) unused faces failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: ReorderIB(16) unused faces ib failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );  
            }
        }
    }

    // Unused faces (32)
    {
        using namespace TestGeometry;

        std::unique_ptr<uint32_t[]> destib( new uint32_t[ 12 * 3 ] );
        memset( destib.get(), 0xff, sizeof(uint32_t) * 12 * 3 );

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

        HRESULT hr = ReorderIB( g_cubeIndices32, 12, s_faceRemap, destib.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ReorderIB(32) unused faces failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( memcmp( destib.get(), s_sorted, sizeof(s_sorted) ) != 0 )
        {
            printe("ERROR: ReorderIB(32) unused faces ib failed\n" );
            success = false;
            for( size_t j = 0; j < (12*3); j += 3 )
            {
                print("\t%zu: %u %u %u\n", j, destib[ j ], destib[ j + 1 ], destib[ j + 2 ] );  
            }
        }
    }

    return success;
}
