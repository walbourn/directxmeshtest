//-------------------------------------------------------------------------------------
// optimize.cpp
//  
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#include "directxtest.h"

#include "directxmesh.h"

#include "TestHelpers.h"
#include "TestGeometry.h"
#include "WaveFrontReader.h"

using namespace DirectX;
using namespace TestGeometry;

// Face-mapped cube
static const uint32_t s_fmCubeAdj[3 * 12] =
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


//-------------------------------------------------------------------------------------
// AttributeSort
bool Test15()
{
    bool success = true;

    {
        static const uint32_t attr[] = { 5, 2, 4, 2, 0, 4, 0, 5, 2, 3, 1, 1 };

        std::vector<uint32_t> attributes, attributesId, attributesZero;
        for( uint32_t j = 0; j < 12; ++j )
        {
            attributes.push_back( attr[ j ] );
            attributesId.push_back( j );
            attributesZero.push_back( 0 );
        }

        static const uint32_t s_attributes[] = { 0, 0, 1, 1, 2, 2, 2, 3, 4, 4, 5, 5 }; 
        static const uint32_t s_attributesId[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
        static const uint32_t s_attributesZero[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

        static const uint32_t s_faceRemap[] = { 4, 6, 10, 11, 1, 3, 8, 9, 2, 5, 0, 7 };
        static const uint32_t s_faceRemapId[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 12 ] );
        memset( remap.get(), 0xff, sizeof(uint32_t) * 12 );

        HRESULT hr = AttributeSort( 12, attributes.data(), remap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: Attribute sort failed (%08X)\n", hr );
        }
        else if ( memcmp( attributes.data(), s_attributes, sizeof(s_attributes) ) != 0 )
        {
            success = false;
            printe("ERROR: Attribute sort failed attributes\n" );
            for(size_t j=0; j < 12; ++j)
                print("%u\n", attributes[j]);
        }
        else if ( !IsValidFaceRemap( g_cubeIndices16, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: Attribute sort failed, remap invalid\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_faceRemap, sizeof(s_faceRemap) ) != 0 )
        {
            success = false;
            printe("ERROR: Attribute sort failed remap\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }

        // identity
        memset( remap.get(), 0xff, sizeof(uint32_t) * 12 );

        hr = AttributeSort( 12, attributesId.data(), remap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: Attribute sort identity failed (%08X)\n", hr );
        }
        else if ( memcmp( attributesId.data(), s_attributesId, sizeof(s_attributesId) ) != 0 )
        {
            success = false;
            printe("ERROR: Attribute sort identity failed attributes\n" );
            for(size_t j=0; j < 12; ++j)
                print("%u\n", attributesId[j]);
        }
        else if ( memcmp( remap.get(), s_faceRemapId, sizeof(s_faceRemapId) ) != 0 )
        {
            success = false;
            printe("ERROR: Attribute sort identity failed\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }

        // zero
        memset( remap.get(), 0xff, sizeof(uint32_t) * 12 );

        hr = AttributeSort( 12, attributesZero.data(), remap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: Attribute sort zero failed (%08X)\n", hr );
        }
        else if ( memcmp( attributesZero.data(), s_attributesZero, sizeof(s_attributesZero) ) != 0 )
        {
            success = false;
            printe("ERROR: Attribute sort zero failed attributes\n" );
            for(size_t j=0; j < 12; ++j)
                print("%u\n", attributesZero[j]);
        }
        else if ( memcmp( remap.get(), s_faceRemapId, sizeof(s_faceRemapId) ) != 0 )
        {
            success = false;
            printe("ERROR: Attribute sort zero failed\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = AttributeSort( UINT32_MAX, attributesZero.data(), remap.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: AttributeSort expected failure for 32-max value faces (%08X)\n", hr );
            success = false;
        }
        #pragma warning(pop)
    }

    return success;
}


//-------------------------------------------------------------------------------------
// OptimizeFaces
bool Test16()
{
    bool success = true;

    // 16-bit fmcube
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 12*3 ] );
        memset( remap.get(), 0xff, sizeof(uint32_t) * 12 * 3 );

        static const uint32_t s_reverse[] = { 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

        HRESULT hr = OptimizeFaces( g_fmCubeIndices16, 12, s_fmCubeAdj, remap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) fmcube failed (%08X)\n", hr );
        }
        else if ( !IsValidFaceRemap( g_fmCubeIndices16, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) fmcube failed remap invalid\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_reverse, sizeof(s_reverse) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) fmcube failed\n" );
            for( size_t j = 0; j < 12; ++j )
            {
                print("\t%Iu -> %u\n", j, remap[j]);
            }
        }

        // striporder
        memset( remap.get(), 0xff, sizeof(uint32_t) * 12 * 3 );

        static const uint32_t s_strip[] = { 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

        hr = OptimizeFaces( g_fmCubeIndices16, 12, s_fmCubeAdj, remap.get(), OPTFACES_V_STRIPORDER );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) fmcube failed [striporder] (%08X)\n", hr );
        }
        else if ( !IsValidFaceRemap( g_fmCubeIndices16, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) fmcube failed [striporder] remap invalid [striporder]\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_strip, sizeof(s_strip) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) fmcube failed [striporder]\n" );
            for( size_t j = 0; j < 12; ++j )
            {
                print("\t%Iu -> %u\n", j, remap[j]);
            }
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = OptimizeFaces( g_fmCubeIndices16, UINT32_MAX, s_fmCubeAdj, remap.get(), OPTFACES_V_STRIPORDER );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: OptimizeFaces(16) expected failure for 32-max value faces (%08X)\n", hr );
            success = false;
        }
        #pragma warning(pop)
    }

    // 32-bit fmcube
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 12*3 ] );
        memset( remap.get(), 0xff, sizeof(uint32_t) * 12 * 3 );

        static const uint32_t s_reverse[] = { 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

        HRESULT hr = OptimizeFaces( g_fmCubeIndices32, 12, s_fmCubeAdj, remap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) fmcube failed (%08X)\n", hr );
        }
        else if ( !IsValidFaceRemap( g_fmCubeIndices32, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) fmcube failed remap invalid\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_reverse, sizeof(s_reverse) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) fmcube failed\n" );
            for( size_t j = 0; j < 12; ++j )
            {
                print("\t%Iu -> %u\n", j, remap[j]);
            }
        }

        // striporder
        memset( remap.get(), 0xff, sizeof(uint32_t) * 12 * 3 );

        static const uint32_t s_strip[] = { 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

        hr = OptimizeFaces( g_fmCubeIndices32, 12, s_fmCubeAdj, remap.get(), OPTFACES_V_STRIPORDER );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) fmcube failed [striporder] (%08X)\n", hr );
        }
        else if ( !IsValidFaceRemap( g_fmCubeIndices32, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) fmcube failed [striporder] remap invalid [striporder]\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_strip, sizeof(s_strip) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) fmcube failed [striporder]\n" );
            for( size_t j = 0; j < 12; ++j )
            {
                print("\t%Iu -> %u\n", j, remap[j]);
            }
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = OptimizeFaces( g_fmCubeIndices32, UINT32_MAX, s_fmCubeAdj, remap.get(), OPTFACES_V_STRIPORDER );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: OptimizeFaces(32) expected failure for 32-max value faces (%08X)\n", hr );
            success = false;
        }
        #pragma warning(pop)
    }

    // Unused (16)
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 12*3 ] );
        memset( remap.get(), 0xff, sizeof(uint32_t) * 12 * 3 );

        static const uint16_t s_unusedIB[ 12*3 ] =
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

        static const uint32_t s_unusedAdj[3 * 12] =
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

        static const uint32_t s_vcremap[] = { 7, 10, 9, 8, 0, 1, 4, 5, 11, 3, 2, uint32_t(-1) };

#ifdef _DEBUG
        std::wstring msgs;
        if ( FAILED( Validate( s_unusedIB, 12, 8, s_unusedAdj, VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs ) ) )
        {
            success = false;
            printe("\nERROR: OptimizeFaces(16) test data failed validation:\n%S\n", msgs.c_str() );
        }
#endif

        HRESULT hr = OptimizeFaces( s_unusedIB, 12, s_unusedAdj, remap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused failed (%08X)\n", hr );
        }
        else if ( !IsValidFaceRemap( s_unusedIB, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused failed remap invalid\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_vcremap, sizeof(s_vcremap) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused failed\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }

        // striporder
        memset( remap.get(), 0xff, sizeof(uint32_t) * 12 * 3 );

        static const uint32_t s_soremap[] = { 7, 8, 0, 1, 4, 5, 2, 3, 9, 10, 11,  uint32_t(-1) };

        hr = OptimizeFaces( s_unusedIB, 12, s_unusedAdj, remap.get(), OPTFACES_V_STRIPORDER );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused [striporder] failed (%08X)\n", hr );
        }
        else if ( !IsValidFaceRemap( s_unusedIB, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused [striporder] failed remap invalid\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_soremap, sizeof(s_soremap) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused [striporder] failed\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
    }

    // Unused (32)
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 12*3 ] );
        memset( remap.get(), 0xff, sizeof(uint32_t) * 12 * 3 );

        static const uint32_t s_unusedIB[ 12*3 ] =
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

        static const uint32_t s_unusedAdj[3 * 12] =
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

        static const uint32_t s_vcremap[] = { 7, 10, 9, 8, 0, 1, 4, 5, 11, 3, 2, uint32_t(-1) };

#ifdef _DEBUG
        std::wstring msgs;
        if ( FAILED( Validate( s_unusedIB, 12, 8, s_unusedAdj, VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs ) ) )
        {
            success = false;
            printe("\nERROR: OptimizeFaces(32) test data failed validation:\n%S\n", msgs.c_str() );
        }
#endif

        HRESULT hr = OptimizeFaces( s_unusedIB, 12, s_unusedAdj, remap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused failed (%08X)\n", hr );
        }
        else if ( !IsValidFaceRemap( s_unusedIB, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused failed remap invalid\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_vcremap, sizeof(s_vcremap) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused failed\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }

        // striporder
        memset( remap.get(), 0xff, sizeof(uint32_t) * 12 * 3 );

        static const uint32_t s_soremap[] = { 7, 8, 0, 1, 4, 5, 2, 3, 9, 10, 11,  uint32_t(-1) };

        hr = OptimizeFaces( s_unusedIB, 12, s_unusedAdj, remap.get(), OPTFACES_V_STRIPORDER );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused [striporder] failed (%08X)\n", hr );
        }
        else if ( !IsValidFaceRemap( s_unusedIB, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused [striporder] failed remap invalid\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_soremap, sizeof(s_soremap) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused [striporder] failed\n" );
            for(size_t j=0; j < 12; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// OptimizeVertices
bool Test17()
{
    bool success = true;

    // 16-bit cube
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 8 ] );
        memset( remap.get(), 0xff, sizeof(uint32_t) * 8 );

        static const uint32_t s_remap[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

        HRESULT hr = OptimizeVertices( g_cubeIndices16, 12, 8, remap.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(16) cube failed (%08X)\n", hr );
            success = false;
        }
        else if ( !IsValidVertexRemap( g_cubeIndices16, 12, remap.get(), 8 ) )
        {
            printe("ERROR: OptimizeVertices(16) cube failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_remap, sizeof(s_remap) ) != 0 )
        {
            printe("ERROR: OptimizeVertices(16) cube failed\n" );
            success = false;
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = OptimizeVertices( g_cubeIndices16, 12, D3D11_16BIT_INDEX_STRIP_CUT_VALUE, remap.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: OptimizeVertices(16) expected failure for strip cut value (%08X)\n", hr );
            success = false;
        }

        hr = OptimizeVertices( g_cubeIndices16, 12, UINT32_MAX, remap.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: OptimizeVertices(16) expected failure for 32-max value verts (%08X)\n", hr );
            success = false;
        }

        hr = OptimizeVertices( g_cubeIndices16, UINT32_MAX, 8, remap.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: OptimizeVertices(16) expected failure for 32-max value faces (%08X)\n", hr );
            success = false;
        }

        hr = OptimizeVertices( g_cubeIndices16, 12, 2, remap.get() );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: OptimizeVertices(16) expected failure for bad vert count (%08X)\n", hr );
            success = false;
        }
        #pragma warning(pop)
    }

    // 16-bit fmcube
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 24 ] );
        memset( remap.get(), 0xff, sizeof(uint32_t) * 24 );

        const static uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, 13, 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23 };

        HRESULT hr = OptimizeVertices( g_fmCubeIndices16, 12, 24, remap.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(16) fmcube failed (%08X)\n", hr );
            success = false;
        }
        else if ( !IsValidVertexRemap( g_fmCubeIndices16, 12, remap.get(), 24 ) )
        {
            printe("ERROR: OptimizeVertices(16) fmcube failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 24; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_remap, sizeof(s_remap) ) != 0 )
        {
            printe("ERROR: OptimizeVertices(16) fmcube failed\n" );
            success = false;
        }
    }

    // 32-bit cube
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 8 ] );
        memset( remap.get(), 0xff, sizeof(uint32_t) * 8 );

        static const uint32_t s_remap[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

        HRESULT hr = OptimizeVertices( g_cubeIndices32, 12, 8, remap.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(32) cube failed (%08X)\n", hr );
            success = false;
        }
        else if ( !IsValidVertexRemap( g_cubeIndices32, 12, remap.get(), 8 ) )
        {
            printe("ERROR: OptimizeVertices(32) cube failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_remap, sizeof(s_remap) ) != 0 )
        {
            printe("ERROR: OptimizeVertices(32) cube failed\n" );
            success = false;
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = OptimizeVertices( g_cubeIndices32, 12, D3D11_32BIT_INDEX_STRIP_CUT_VALUE, remap.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: OptimizeVertices(32) expected failure for strip cut value (%08X)\n", hr );
            success = false;
        }

        hr = OptimizeVertices( g_cubeIndices32, 12, UINT32_MAX, remap.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: OptimizeVertices(32) expected failure for 32-max value verts (%08X)\n", hr );
            success = false;
        }

        hr = OptimizeVertices( g_cubeIndices32, UINT32_MAX, 8, remap.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: OptimizeVertices(32) expected failure for 32-max value faces (%08X)\n", hr );
            success = false;
        }

        hr = OptimizeVertices( g_cubeIndices32, 12, 2, remap.get() );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: OptimizeVertices(32) expected failure for bad vert count (%08X)\n", hr );
            success = false;
        }
        #pragma warning(pop)
    }

    // 32-bit fmcube
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 24 ] );
        memset( remap.get(), 0xff, sizeof(uint32_t) * 24 );

        const static uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, 13, 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23 };

        HRESULT hr = OptimizeVertices( g_fmCubeIndices32, 12, 24, remap.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(32) fmcube failed (%08X)\n", hr );
            success = false;
        }
        else if ( !IsValidVertexRemap( g_fmCubeIndices32, 12, remap.get(), 24 ) )
        {
            printe("ERROR: OptimizeVertices(32) fmcube failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 24; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_remap, sizeof(s_remap) ) != 0 )
        {
            printe("ERROR: OptimizeVertices(32) fmcube failed\n" );
            success = false;
        }
    }

    // Unused 16
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 8 ] );
        memset( remap.get(), 0xff, sizeof(uint32_t) * 8 );

        // Faces
        static const uint16_t s_unusedIB[ 12*3 ] =
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

#ifdef _DEBUG
        std::wstring msgs;
        if ( FAILED( Validate( s_unusedIB, 12, 8, nullptr, VALIDATE_UNUSED, &msgs ) ) )
        {
            success = false;
            printe("\nERROR: OptimizeVertices(16) test data failed validation:\n%S\n", msgs.c_str() );
        }
#endif

        static const uint32_t s_uforder[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

        HRESULT hr = OptimizeVertices( s_unusedIB, 12, 8, remap.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(16) unused faces failed (%08X)\n", hr );
            success = false;
        }
        else if ( !IsValidVertexRemap( s_unusedIB, 12, remap.get(), 8 ) )
        {
            printe("ERROR: OptimizeVertices(16) unused faces failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_uforder, sizeof(s_uforder) ) != 0 )
        {
            printe("ERROR: OptimizeVertices(16) unused faces failed\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }

        // Vertices
        memset( remap.get(), 0xff, sizeof(uint32_t) * 8 );
        static const uint16_t s_unusedVerts[ 7*3 ] =
        {
            0, 1, 2,
            0, 3, 1,
            0, 4, 3,
            3, 6, 1,
            3, 4, 6,
            2, 6, 7,
            0, 2, 7,
        };

        static const uint32_t s_uvorder[] = { 0, 1, 2, 3, 4, 6, 7, uint32_t(-1) } ;

        hr = OptimizeVertices( s_unusedVerts, 7, 8, remap.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(16) unused verts failed (%08X)\n", hr );
            success = false;
        }
        else if ( !IsValidVertexRemap( s_unusedVerts, 7, remap.get(), 8 ) )
        {
            printe("ERROR: OptimizeVertices(16) unused verts failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_uvorder, sizeof(s_uvorder) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeVertices(16) unused verts failed\n" );
            for(size_t j=0; j < 8; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
    }

    // Unused 32
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 8 ] );
        memset( remap.get(), 0xff, sizeof(uint32_t) * 8 );

        // Faces
        static const uint32_t s_unusedIB[ 12*3 ] =
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

#ifdef _DEBUG
        std::wstring msgs;
        if ( FAILED( Validate( s_unusedIB, 12, 8, nullptr, VALIDATE_UNUSED, &msgs ) ) )
        {
            success = false;
            printe("\nERROR: OptimizeVertices(32) test data failed validation:\n%S\n", msgs.c_str() );
        }
#endif

        static const uint32_t s_uforder[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

        HRESULT hr = OptimizeVertices( s_unusedIB, 12, 8, remap.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(32) unused faces failed (%08X)\n", hr );
            success = false;
        }
        else if ( !IsValidVertexRemap( s_unusedIB, 12, remap.get(), 8 ) )
        {
            printe("ERROR: OptimizeVertices(32) unused faces failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_uforder, sizeof(s_uforder) ) != 0 )
        {
            printe("ERROR: OptimizeVertices(32) unused faces failed\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }

        // Vertices
        memset( remap.get(), 0xff, sizeof(uint32_t) * 8 );
        static const uint32_t s_unusedVerts[ 7*3 ] =
        {
            0, 1, 2,
            0, 3, 1,
            0, 4, 3,
            3, 6, 1,
            3, 4, 6,
            2, 6, 7,
            0, 2, 7,
        };

        static const uint32_t s_uvorder[] = { 0, 1, 2, 3, 4, 6, 7, uint32_t(-1) } ;

        hr = OptimizeVertices( s_unusedVerts, 7, 8, remap.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(32) unused verts failed (%08X)\n", hr );
            success = false;
        }
        else if ( !IsValidVertexRemap( s_unusedVerts, 7, remap.get(), 8 ) )
        {
            printe("ERROR: OptimizeVertices(32) unused verts failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_uvorder, sizeof(s_uvorder) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeVertices(32) unused verts failed\n" );
            for(size_t j=0; j < 8; ++j)
                print("%Iu -> %u\n", j, remap[j]);
        }
    }

    return success;
}
