//-------------------------------------------------------------------------------------
// optimize.cpp
//  
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#include "directxtest.h"

#include "DirectXMesh.h"

#include "ShapesGenerator.h"
#include "TestHelpers.h"
#include "TestGeometry.h"
#include "WaveFrontReader.h"

using namespace DirectX;
using namespace TestGeometry;

namespace
{
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
}

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
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 12 );

        HRESULT hr = AttributeSort( 12, attributes.data(), remap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: Attribute sort failed (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else if ( memcmp( attributes.data(), s_attributes, sizeof(s_attributes) ) != 0 )
        {
            success = false;
            printe("ERROR: Attribute sort failed attributes\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu: %u .. %u\n", j, attributes[j], s_attributes[j]);
        }
        else if ( !IsValidFaceRemap( g_cubeIndices16, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: Attribute sort failed, remap invalid\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_faceRemap, sizeof(s_faceRemap) ) != 0 )
        {
            success = false;
            printe("ERROR: Attribute sort failed remap\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_faceRemap[j]);
        }

        // identity
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 12 );

        hr = AttributeSort( 12, attributesId.data(), remap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: Attribute sort identity failed (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else if ( memcmp( attributesId.data(), s_attributesId, sizeof(s_attributesId) ) != 0 )
        {
            success = false;
            printe("ERROR: Attribute sort identity failed attributes\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu: %u .. %u\n", j, attributesId[j], s_attributesId[j]);
        }
        else if ( memcmp( remap.get(), s_faceRemapId, sizeof(s_faceRemapId) ) != 0 )
        {
            success = false;
            printe("ERROR: Attribute sort identity failed\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_faceRemapId[j]);
        }

        // zero
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 12 );

        hr = AttributeSort( 12, attributesZero.data(), remap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: Attribute sort zero failed (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else if ( memcmp( attributesZero.data(), s_attributesZero, sizeof(s_attributesZero) ) != 0 )
        {
            success = false;
            printe("ERROR: Attribute sort zero failed attributes\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu: %u .. %u\n", j, attributesZero[j], s_attributesZero[j]);
        }
        else if ( memcmp( remap.get(), s_faceRemapId, sizeof(s_faceRemapId) ) != 0 )
        {
            success = false;
            printe("ERROR: Attribute sort zero failed\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_faceRemapId[j]);
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = AttributeSort( UINT32_MAX, attributesZero.data(), remap.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: AttributeSort expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
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
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3 );

        static const uint32_t s_reverse[] = { 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

        HRESULT hr = OptimizeFaces( g_fmCubeIndices16, 12, s_fmCubeAdj, remap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else if ( !IsValidFaceRemap( g_fmCubeIndices16, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) fmcube failed remap invalid\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_reverse, sizeof(s_reverse) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) fmcube failed\n" );
            for( size_t j = 0; j < 12; ++j )
            {
                print("\t%zu -> %u .. %u\n", j, remap[j], s_reverse[j]);
            }
        }

        // striporder
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3 );

        static const uint32_t s_strip[] = { 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

        hr = OptimizeFaces( g_fmCubeIndices16, 12, s_fmCubeAdj, remap.get(), OPTFACES_V_STRIPORDER );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) fmcube failed [striporder] (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else if ( !IsValidFaceRemap( g_fmCubeIndices16, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) fmcube failed [striporder] remap invalid [striporder]\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_strip, sizeof(s_strip) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) fmcube failed [striporder]\n" );
            for( size_t j = 0; j < 12; ++j )
            {
                print("\t%zu -> %u .. %u\n", j, remap[j], s_strip[j]);
            }
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = OptimizeFaces( g_fmCubeIndices16, UINT32_MAX, s_fmCubeAdj, remap.get(), OPTFACES_V_STRIPORDER );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: OptimizeFaces(16) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // 32-bit fmcube
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 12*3 ] );
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3 );

        static const uint32_t s_reverse[] = { 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

        HRESULT hr = OptimizeFaces( g_fmCubeIndices32, 12, s_fmCubeAdj, remap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else if ( !IsValidFaceRemap( g_fmCubeIndices32, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) fmcube failed remap invalid\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_reverse, sizeof(s_reverse) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) fmcube failed\n" );
            for( size_t j = 0; j < 12; ++j )
            {
                print("\t%zu -> %u .. %u\n", j, remap[j], s_reverse[j]);
            }
        }

        // striporder
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3 );

        static const uint32_t s_strip[] = { 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

        hr = OptimizeFaces( g_fmCubeIndices32, 12, s_fmCubeAdj, remap.get(), OPTFACES_V_STRIPORDER );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) fmcube failed [striporder] (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else if ( !IsValidFaceRemap( g_fmCubeIndices32, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) fmcube failed [striporder] remap invalid [striporder]\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_strip, sizeof(s_strip) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) fmcube failed [striporder]\n" );
            for( size_t j = 0; j < 12; ++j )
            {
                print("\t%zu -> %u .. %u\n", j, remap[j], s_strip[j]);
            }
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = OptimizeFaces( g_fmCubeIndices32, UINT32_MAX, s_fmCubeAdj, remap.get(), OPTFACES_V_STRIPORDER );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: OptimizeFaces(32) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // Unused (16)
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 12*3 ] );
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3 );

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

        static const uint16_t s_unusedIB_1st[12 * 3] =
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

        static const uint32_t s_unusedAdj_1st[3 * 12] =
        {
            uint32_t(-1), uint32_t(-1), uint32_t(-1),
            2, 4, uint32_t(-1),
            3, 5, 1,
            9, 11, 2,
            5, 6, 1,
            2, 11, 4,
            uint32_t(-1), 4, 7,
            6, 10, 8,
            uint32_t(-1), 7, 9,
            8, 10, 3,
            9, 7, 11,
            10, 5, 3
        };

        static const uint16_t s_unusedIB_all[12 * 3] =
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

        static const uint32_t s_unusedAdj_all[3 * 12] =
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

        static const uint32_t s_vcremap[] = { 7, 10, 9, 8, 0, 1, 4, 5, 11, 3, 2, uint32_t(-1) };

#ifdef _DEBUG
        std::wstring msgs;
        if ( FAILED( Validate( s_unusedIB, 12, 8, s_unusedAdj, VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs ) ) )
        {
            success = false;
            printe("\nERROR: OptimizeFaces(16) test data failed validation:\n%S\n", msgs.c_str() );
        }
        if (FAILED(Validate(s_unusedIB_1st, 12, 8, s_unusedAdj_1st, VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs)))
        {
            success = false;
            printe("\nERROR: OptimizeFaces(16) test data 1st failed validation:\n%S\n", msgs.c_str());
        }
        if (FAILED(Validate(s_unusedIB_all, 12, 8, s_unusedAdj_all, VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs)))
        {
            success = false;
            printe("\nERROR: OptimizeFaces(16) test data all failed validation:\n%S\n", msgs.c_str());
        }
#endif

        HRESULT hr = OptimizeFaces( s_unusedIB, 12, s_unusedAdj, remap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused failed (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else if ( !IsValidFaceRemap( s_unusedIB, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused failed remap invalid\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_vcremap, sizeof(s_vcremap) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused failed\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_vcremap[j]);
        }

        memset(remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3);

        hr = OptimizeFaces(s_unusedIB_1st, 12, s_unusedAdj_1st, remap.get());
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused 1st failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(s_unusedIB_1st, remap.get(), 12))
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused 1st failed remap invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }

        memset(remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3);

        hr = OptimizeFaces(s_unusedIB_all , 12, s_unusedAdj_all, remap.get());
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused all failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(s_unusedIB_all, remap.get(), 12))
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused all failed remap invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }

        // striporder
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3 );

        static const uint32_t s_soremap[] = { 7, 8, 0, 1, 4, 5, 2, 3, 9, 10, 11,  uint32_t(-1) };

        hr = OptimizeFaces( s_unusedIB, 12, s_unusedAdj, remap.get(), OPTFACES_V_STRIPORDER );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused [striporder] failed (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else if ( !IsValidFaceRemap( s_unusedIB, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused [striporder] failed remap invalid\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_soremap, sizeof(s_soremap) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(16) unused [striporder] failed\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_soremap[j]);
        }
    }

    // Unused (32)
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 12*3 ] );
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3 );

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

        static const uint32_t s_unusedIB_1st[12 * 3] =
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

        static const uint32_t s_unusedAdj_1st[3 * 12] =
        {
            uint32_t(-1), uint32_t(-1), uint32_t(-1),
            2, 4, uint32_t(-1),
            3, 5, 1,
            9, 11, 2,
            5, 6, 1,
            2, 11, 4,
            uint32_t(-1), 4, 7,
            6, 10, 8,
            uint32_t(-1), 7, 9,
            8, 10, 3,
            9, 7, 11,
            10, 5, 3
        };

        static const uint32_t s_unusedIB_all[12 * 3] =
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

        static const uint32_t s_unusedAdj_all[3 * 12] =
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

        static const uint32_t s_vcremap[] = { 7, 10, 9, 8, 0, 1, 4, 5, 11, 3, 2, uint32_t(-1) };

#ifdef _DEBUG
        std::wstring msgs;
        if ( FAILED( Validate( s_unusedIB, 12, 8, s_unusedAdj, VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs ) ) )
        {
            success = false;
            printe("\nERROR: OptimizeFaces(32) test data failed validation:\n%S\n", msgs.c_str() );
        }
        if (FAILED(Validate(s_unusedIB_1st, 12, 8, s_unusedAdj_1st, VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs)))
        {
            success = false;
            printe("\nERROR: OptimizeFaces(32) test data 1st failed validation:\n%S\n", msgs.c_str());
        }
        if (FAILED(Validate(s_unusedIB_all, 12, 8, s_unusedAdj_all, VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs)))
        {
            success = false;
            printe("\nERROR: OptimizeFaces(32) test data all failed validation:\n%S\n", msgs.c_str());
        }
#endif

        HRESULT hr = OptimizeFaces( s_unusedIB, 12, s_unusedAdj, remap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused failed (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else if ( !IsValidFaceRemap( s_unusedIB, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused failed remap invalid\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_vcremap, sizeof(s_vcremap) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused failed\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_vcremap[j]);
        }

        memset(remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3);

        hr = OptimizeFaces(s_unusedIB_1st, 12, s_unusedAdj_1st, remap.get());
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused 1st failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(s_unusedIB_1st, remap.get(), 12))
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused 1st failed remap invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }

        memset(remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3);

        hr = OptimizeFaces(s_unusedIB_all, 12, s_unusedAdj_all, remap.get());
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused all failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(s_unusedIB_all, remap.get(), 12))
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused all failed remap invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }

        // striporder
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3 );

        static const uint32_t s_soremap[] = { 7, 8, 0, 1, 4, 5, 2, 3, 9, 10, 11,  uint32_t(-1) };

        hr = OptimizeFaces( s_unusedIB, 12, s_unusedAdj, remap.get(), OPTFACES_V_STRIPORDER );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused [striporder] failed (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else if ( !IsValidFaceRemap( s_unusedIB, remap.get(), 12 ) )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused [striporder] failed remap invalid\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_soremap, sizeof(s_soremap) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeFaces(32) unused [striporder] failed\n" );
            for(size_t j=0; j < 12; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_soremap[j]);
        }
    }

    // 16-bit torus
    {
        std::vector<uint16_t> indices;
        std::vector<ShapesGenerator<uint16_t>::Vertex> vertices;
        ShapesGenerator<uint16_t>::CreateTorus(indices, vertices, 1.f, 0.333f, 32, false);

        size_t nFaces = indices.size() / 3;

        float acmrOrig, atvrOrig;
        ComputeVertexCacheMissRate(indices.data(), nFaces, vertices.size(), OPTFACES_V_DEFAULT, acmrOrig, atvrOrig);

        std::unique_ptr<uint32_t[]> remap(new uint32_t[nFaces]);
        memset(remap.get(), 0xff, sizeof(uint32_t) * nFaces);

        float acmrDef = 0.f, atvrDef = 0.f;

        std::unique_ptr<uint32_t[]> adj(new uint32_t[3 * nFaces]);
        memset(adj.get(), 0xcd, sizeof(uint32_t) * 3 * nFaces);

        std::unique_ptr<XMFLOAT3[]> positions(new XMFLOAT3[vertices.size()]);
        {
            size_t j = 0;
            for (const auto& it : vertices)
            {
                positions[j] = it.position;
                ++j;
            }
        }

        HRESULT hr = GenerateAdjacencyAndPointReps(indices.data(), nFaces, positions.get(), vertices.size(), 0.f, nullptr, adj.get());
        if (FAILED(hr))
        {
            printe("\nERROR: OptimizeFaces(16) torus adjacency failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else
        {
            hr = OptimizeFaces(indices.data(), nFaces, adj.get(), remap.get());
            if (FAILED(hr))
            {
                success = false;
                printe("ERROR: OptimizeFaces(16) torus failed (%08X)\n", static_cast<unsigned int>(hr));
            }
            else if (!IsValidFaceRemap(indices.data(), remap.get(), nFaces))
            {
                success = false;
                printe("ERROR: OptimizeFaces(16) torus failed remap invalid\n");
                for (size_t j = 0; j < nFaces; ++j)
                    print("%zu -> %u\n", j, remap[j]);
            }
            else
            {
                std::vector<uint16_t> reorderedIB(indices.cbegin(), indices.cend());

                hr = ReorderIB(reorderedIB.data(), nFaces, remap.get());
                if (FAILED(hr))
                {
                    success = false;
                    printe("ERROR: OptimizeFaces(16) torus reorder failed (%08X)\n", static_cast<unsigned int>(hr));
                }
                else
                {
                    ComputeVertexCacheMissRate(reorderedIB.data(), nFaces, vertices.size(), OPTFACES_V_DEFAULT, acmrDef, atvrDef);

                    if (acmrDef > acmrOrig
                        || atvrDef> atvrOrig)
                    {
                        success = false;
                        printe("ERROR: OptimizeFaces(16) torus failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmrDef, acmrOrig, atvrDef, atvrOrig);
                    }
                }
            }

            // striporder
            hr = OptimizeFaces(indices.data(), nFaces, adj.get(), remap.get(), OPTFACES_V_STRIPORDER);
            if (FAILED(hr))
            {
                success = false;
                printe("ERROR: OptimizeFaces(16) torus strip failed (%08X)\n", static_cast<unsigned int>(hr));
            }
            else if (!IsValidFaceRemap(indices.data(), remap.get(), nFaces))
            {
                success = false;
                printe("ERROR: OptimizeFaces(16) torus strip failed remap invalid\n");
                for (size_t j = 0; j < nFaces; ++j)
                    print("%zu -> %u\n", j, remap[j]);
            }
            else
            {
                std::vector<uint16_t> reorderedIB(indices.cbegin(), indices.cend());

                hr = ReorderIB(reorderedIB.data(), nFaces, remap.get());
                if (FAILED(hr))
                {
                    success = false;
                    printe("ERROR: OptimizeFaces(16) torus strip reorder failed (%08X)\n", static_cast<unsigned int>(hr));
                }
                else
                {
                    float acmr, atvr;
                    ComputeVertexCacheMissRate(reorderedIB.data(), nFaces, vertices.size(), OPTFACES_V_DEFAULT, acmr, atvr);

                    if (acmr > acmrOrig
                        || atvr > atvrOrig)
                    {
                        success = false;
                        printe("ERROR: OptimizeFaces(16) torus strip failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, acmrOrig, atvr, atvrOrig);
                    }

                    if (acmr < acmrDef
                        || atvr < atvrDef)
                    {
                        success = false;
                        printe("ERROR: OptimizeFaces(16) torus strip vs default failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, acmrDef, atvr, atvrDef);
                    }
                }
            }

            // intel cache
            hr = OptimizeFaces(indices.data(), nFaces, adj.get(), remap.get(), 24, 20);
            if (FAILED(hr))
            {
                success = false;
                printe("ERROR: OptimizeFaces(16) torus intel failed (%08X)\n", static_cast<unsigned int>(hr));
            }
            else if (!IsValidFaceRemap(indices.data(), remap.get(), nFaces))
            {
                success = false;
                printe("ERROR: OptimizeFaces(16) torus intel failed remap invalid\n");
                for (size_t j = 0; j < nFaces; ++j)
                    print("%zu -> %u\n", j, remap[j]);
            }
            else
            {
                std::vector<uint16_t> reorderedIB(indices.cbegin(), indices.cend());

                hr = ReorderIB(reorderedIB.data(), nFaces, remap.get());
                if (FAILED(hr))
                {
                    success = false;
                    printe("ERROR: OptimizeFaces(16) torus intel reorder failed (%08X)\n", static_cast<unsigned int>(hr));
                }
                else
                {
                    float acmr, atvr;
                    ComputeVertexCacheMissRate(reorderedIB.data(), nFaces, vertices.size(), OPTFACES_V_DEFAULT, acmr, atvr);

                    if (acmr > acmrOrig
                        || atvr > atvrOrig)
                    {
                        success = false;
                        printe("ERROR: OptimizeFaces(16) torus intel failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, acmrOrig, atvr, atvrOrig);
                    }

                    if (acmr > acmrDef
                        || atvr > atvrDef)
                    {
                        success = false;
                        printe("ERROR: OptimizeFaces(16) torus intel vs default failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, acmrDef, atvr, atvrDef);
                    }
                }
            }
        }
    }

    // 32-bit torus
    {
        std::vector<uint32_t> indices;
        std::vector<ShapesGenerator<uint32_t>::Vertex> vertices;
        ShapesGenerator<uint32_t>::CreateTorus(indices, vertices, 1.f, 0.333f, 32, false);

        size_t nFaces = indices.size() / 3;

        float acmrOrig, atvrOrig;
        ComputeVertexCacheMissRate(indices.data(), nFaces, vertices.size(), OPTFACES_V_DEFAULT, acmrOrig, atvrOrig);

        std::unique_ptr<uint32_t[]> remap(new uint32_t[nFaces]);
        memset(remap.get(), 0xcd, sizeof(uint32_t) * nFaces);

        float acmrDef = 0.f;
        float atvrDef = 0.f;

        std::unique_ptr<uint32_t[]> adj(new uint32_t[3 * nFaces]);
        memset(adj.get(), 0xcd, sizeof(uint32_t) * 3 * nFaces);

        std::unique_ptr<XMFLOAT3[]> positions(new XMFLOAT3[vertices.size()]);
        {
            size_t j = 0;
            for (const auto& it : vertices)
            {
                positions[j] = it.position;
                ++j;
            }
        }

        HRESULT hr = GenerateAdjacencyAndPointReps(indices.data(), nFaces, positions.get(), vertices.size(), 0.f, nullptr, adj.get());
        if (FAILED(hr))
        {
            printe("\nERROR: OptimizeFaces(32) torus adjacency failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else
        {
            hr = OptimizeFaces(indices.data(), nFaces, adj.get(), remap.get());
            if (FAILED(hr))
            {
                success = false;
                printe("ERROR: OptimizeFaces(32) torus failed (%08X)\n", static_cast<unsigned int>(hr));
            }
            else if (!IsValidFaceRemap(indices.data(), remap.get(), nFaces))
            {
                success = false;
                printe("ERROR: OptimizeFaces(32) torus failed remap invalid\n");
                for (size_t j = 0; j < nFaces; ++j)
                    print("%zu -> %u\n", j, remap[j]);
            }
            else
            {
                std::vector<uint32_t> reorderedIB(indices.cbegin(), indices.cend());

                hr = ReorderIB(reorderedIB.data(), nFaces, remap.get());
                if (FAILED(hr))
                {
                    success = false;
                    printe("ERROR: OptimizeFaces(32) torus reorder failed (%08X)\n", static_cast<unsigned int>(hr));
                }
                else
                {
                    ComputeVertexCacheMissRate(reorderedIB.data(), nFaces, vertices.size(), OPTFACES_V_DEFAULT, acmrDef, atvrDef);

                    if (acmrDef > acmrOrig
                        || atvrDef> atvrOrig)
                    {
                        success = false;
                        printe("ERROR: OptimizeFaces(32) torus failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmrDef, acmrOrig, atvrDef, atvrOrig);
                    }
                }
            }

            // striporder
            hr = OptimizeFaces(indices.data(), nFaces, adj.get(), remap.get(), OPTFACES_V_STRIPORDER);
            if (FAILED(hr))
            {
                success = false;
                printe("ERROR: OptimizeFaces(32) torus strip failed (%08X)\n", static_cast<unsigned int>(hr));
            }
            else if (!IsValidFaceRemap(indices.data(), remap.get(), nFaces))
            {
                success = false;
                printe("ERROR: OptimizeFaces(32) torus strip failed remap invalid\n");
                for (size_t j = 0; j < nFaces; ++j)
                    print("%zu -> %u\n", j, remap[j]);
            }
            else
            {
                std::vector<uint32_t> reorderedIB(indices.cbegin(), indices.cend());

                hr = ReorderIB(reorderedIB.data(), nFaces, remap.get());
                if (FAILED(hr))
                {
                    success = false;
                    printe("ERROR: OptimizeFaces(32) torus strip reorder failed (%08X)\n", static_cast<unsigned int>(hr));
                }
                else
                {
                    float acmr, atvr;
                    ComputeVertexCacheMissRate(reorderedIB.data(), nFaces, vertices.size(), OPTFACES_V_DEFAULT, acmr, atvr);

                    if (acmr > acmrOrig
                        || atvr > atvrOrig)
                    {
                        success = false;
                        printe("ERROR: OptimizeFaces(32) torus strip failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, acmrOrig, atvr, atvrOrig);
                    }

                    if (acmr < acmrDef
                        || atvr < atvrDef)
                    {
                        success = false;
                        printe("ERROR: OptimizeFaces(32) torus strip vs default failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, acmrDef, atvr, atvrDef);
                    }
                }
            }

            // intel cache
            hr = OptimizeFaces(indices.data(), nFaces, adj.get(), remap.get(), 24, 20);
            if (FAILED(hr))
            {
                success = false;
                printe("ERROR: OptimizeFaces(32) torus intel failed (%08X)\n", static_cast<unsigned int>(hr));
            }
            else if (!IsValidFaceRemap(indices.data(), remap.get(), nFaces))
            {
                success = false;
                printe("ERROR: OptimizeFaces(32) torus intel failed remap invalid\n");
                for (size_t j = 0; j < nFaces; ++j)
                    print("%zu -> %u\n", j, remap[j]);
            }
            else
            {
                std::vector<uint32_t> reorderedIB(indices.cbegin(), indices.cend());

                hr = ReorderIB(reorderedIB.data(), nFaces, remap.get());
                if (FAILED(hr))
                {
                    success = false;
                    printe("ERROR: OptimizeFaces(32) torus intel reorder failed (%08X)\n", static_cast<unsigned int>(hr));
                }
                else
                {
                    float acmr, atvr;
                    ComputeVertexCacheMissRate(reorderedIB.data(), nFaces, vertices.size(), OPTFACES_V_DEFAULT, acmr, atvr);

                    if (acmr > acmrOrig
                        || atvr > atvrOrig)
                    {
                        success = false;
                        printe("ERROR: OptimizeFaces(32) torus intel failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, acmrOrig, atvr, atvrOrig);
                    }

                    if (acmr > acmrDef
                        || atvr > atvrDef)
                    {
                        success = false;
                        printe("ERROR: OptimizeFaces(32) torus intel vs default failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, acmrDef, atvr, atvrDef);
                    }
                }
            }
        }
    }

    return success;
}

//-------------------------------------------------------------------------------------
// OptimizeFacesLRU
bool Test25()
{
    bool success = true;

    // 16-bit fmcube
    {
        std::unique_ptr<uint32_t[]> remap(new uint32_t[12 * 3]);
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3);

        static const uint32_t s_vcremap[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

        HRESULT hr = OptimizeFacesLRU(g_fmCubeIndices16, 12, remap.get());
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) fmcube failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(g_fmCubeIndices16, remap.get(), 12))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) fmcube failed remap invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if (memcmp(remap.get(), s_vcremap, sizeof(s_vcremap)) != 0)
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) fmcube failed\n");
            for (size_t j = 0; j < 12; ++j)
            {
                print("\t%zu -> %u .. %u\n", j, remap[j], s_vcremap[j]);
            }
        }

        // vertex cache size
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3);

        hr = OptimizeFacesLRU(g_fmCubeIndices16, 12, remap.get(), 4);
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) fmcube lru4 failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(g_fmCubeIndices16, remap.get(), 12))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) fmcube lru4 failed remap invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if (memcmp(remap.get(), s_vcremap, sizeof(s_vcremap)) != 0)
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) fmcube lru4 failed\n");
            for (size_t j = 0; j < 12; ++j)
            {
                print("\t%zu -> %u .. %u\n", j, remap[j], s_vcremap[j]);
            }
        }

        // invalid args
#pragma warning(push)
#pragma warning(disable:6385)
        hr = OptimizeFacesLRU(g_fmCubeIndices16, UINT32_MAX, remap.get());
        if (hr != HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW))
        {
            printe("\nERROR: OptimizeFacesLRU(16) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = OptimizeFacesLRU(g_fmCubeIndices16, 12, remap.get(), 128);
        if (hr != E_INVALIDARG)
        {
            printe("\nERROR: OptimizeFacesLRU(16) expected failure for too large a cache size (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
#pragma warning(pop)
    }

    // 32-bit fmcube
    {
        std::unique_ptr<uint32_t[]> remap(new uint32_t[12 * 3]);
        memset(remap.get(), 0xff, sizeof(uint32_t) * 12 * 3);

        static const uint32_t s_vcremap[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

        HRESULT hr = OptimizeFacesLRU(g_fmCubeIndices32, 12, remap.get());
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) fmcube failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(g_fmCubeIndices32, remap.get(), 12))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) fmcube failed remap invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if (memcmp(remap.get(), s_vcremap, sizeof(s_vcremap)) != 0)
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) fmcube failed\n");
            for (size_t j = 0; j < 12; ++j)
            {
                print("\t%zu -> %u .. %u\n", j, remap[j], s_vcremap[j]);
            }
        }

        // vertex cache size
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3);

        hr = OptimizeFacesLRU(g_fmCubeIndices32, 12, remap.get(), 4);
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) fmcube lru4 failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(g_fmCubeIndices32, remap.get(), 12))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) fmcube lru4 failed remap invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if (memcmp(remap.get(), s_vcremap, sizeof(s_vcremap)) != 0)
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) fmcube lru4 failed\n");
            for (size_t j = 0; j < 12; ++j)
            {
                print("\t%zu -> %u .. %u\n", j, remap[j], s_vcremap[j]);
            }
        }

        // invalid args
#pragma warning(push)
#pragma warning(disable:6385)
        hr = OptimizeFacesLRU(g_fmCubeIndices32, UINT32_MAX, remap.get());
        if (hr != HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW))
        {
            printe("\nERROR: OptimizeFacesLRU(32) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = OptimizeFacesLRU(g_fmCubeIndices32, 12, remap.get(), 128);
        if (hr != E_INVALIDARG)
        {
            printe("\nERROR: OptimizeFacesLRU(32) expected failure for too large a cache size (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
#pragma warning(pop)
    }

    // Unused (16)
    {
        std::unique_ptr<uint32_t[]> remap(new uint32_t[12 * 3]);
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3);

        static const uint16_t s_unusedIB[12 * 3] =
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

        static const uint16_t s_unusedIB_1st[12 * 3] =
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

        static const uint16_t s_unusedIB_all[12 * 3] =
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

        static const uint32_t s_vcremap[] = { 0, 1, 4, 8, 7, 2, 5, 9, 3, 10, 11, uint32_t(-1) };

#ifdef _DEBUG
        std::wstring msgs;
        if (FAILED(Validate(s_unusedIB, 12, 8, nullptr, VALIDATE_UNUSED, &msgs)))
        {
            success = false;
            printe("\nERROR: OptimizeFacesLRU(16) test data failed validation:\n%S\n", msgs.c_str());
        }
        if (FAILED(Validate(s_unusedIB_1st, 12, 8, nullptr, VALIDATE_UNUSED, &msgs)))
        {
            success = false;
            printe("\nERROR: OptimizeFacesLRU(16) test data 1st failed validation:\n%S\n", msgs.c_str());
        }
        if (FAILED(Validate(s_unusedIB_all, 12, 8, nullptr, VALIDATE_UNUSED, &msgs)))
        {
            success = false;
            printe("\nERROR: OptimizeFacesLRU(16) test data all failed validation:\n%S\n", msgs.c_str());
        }
#endif

        HRESULT hr = OptimizeFacesLRU(s_unusedIB, 12, remap.get());
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) unused failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(s_unusedIB, remap.get(), 12))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) unused failed remap invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if (memcmp(remap.get(), s_vcremap, sizeof(s_vcremap)) != 0)
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) unused failed\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_vcremap[j]);
        }

        memset(remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3);

        hr = OptimizeFacesLRU(s_unusedIB_1st, 12, remap.get());
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) unused 1st failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(s_unusedIB_1st, remap.get(), 12))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) unused 1st failed remap invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }

        memset(remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3);

        hr = OptimizeFacesLRU(s_unusedIB_all, 12, remap.get());
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) unused all failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(s_unusedIB_all, remap.get(), 12))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) unused all failed remap invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
    }

    // Unused (32)
    {
        std::unique_ptr<uint32_t[]> remap(new uint32_t[12 * 3]);
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3);

        static const uint32_t s_unusedIB[12 * 3] =
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

        static const uint32_t s_unusedIB_1st[12 * 3] =
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

        static const uint32_t s_unusedIB_all[12 * 3] =
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

        static const uint32_t s_vcremap[] = { 0, 1, 4, 8, 7, 2, 5, 9, 3, 10, 11, uint32_t(-1) };

#ifdef _DEBUG
        std::wstring msgs;
        if (FAILED(Validate(s_unusedIB, 12, 8, nullptr, VALIDATE_UNUSED, &msgs)))
        {
            success = false;
            printe("\nERROR: OptimizeFacesLRU(32) test data failed validation:\n%S\n", msgs.c_str());
        }
        if (FAILED(Validate(s_unusedIB_1st, 12, 8, nullptr, VALIDATE_UNUSED, &msgs)))
        {
            success = false;
            printe("\nERROR: OptimizeFacesLRU(32) test data 1st failed validation:\n%S\n", msgs.c_str());
        }
        if (FAILED(Validate(s_unusedIB_all, 12, 8, nullptr, VALIDATE_UNUSED, &msgs)))
        {
            success = false;
            printe("\nERROR: OptimizeFacesLRU(32) test data all failed validation:\n%S\n", msgs.c_str());
        }
#endif

        HRESULT hr = OptimizeFacesLRU(s_unusedIB, 12, remap.get());
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) unused failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(s_unusedIB, remap.get(), 12))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) unused failed remap invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if (memcmp(remap.get(), s_vcremap, sizeof(s_vcremap)) != 0)
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) unused failed\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_vcremap[j]);
        }

        memset(remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3);

        hr = OptimizeFacesLRU(s_unusedIB_1st, 12, remap.get());
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) unused 1st failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(s_unusedIB_1st, remap.get(), 12))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) unused 1st failed remap invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }

        memset(remap.get(), 0xcd, sizeof(uint32_t) * 12 * 3);

        hr = OptimizeFacesLRU(s_unusedIB_all, 12, remap.get());
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) unused all failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(s_unusedIB_all, remap.get(), 12))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) unused all failed remap invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
    }

    // 16-bit torus
    {
        std::vector<uint16_t> indices;
        std::vector<ShapesGenerator<uint16_t>::Vertex> vertices;
        ShapesGenerator<uint16_t>::CreateTorus(indices, vertices, 1.f, 0.333f, 32, false);

        size_t nFaces = indices.size() / 3;

        float acmrOrig, atvrOrig;
        ComputeVertexCacheMissRate(indices.data(), nFaces, vertices.size(), OPTFACES_V_DEFAULT, acmrOrig, atvrOrig);

        std::unique_ptr<uint32_t[]> remap(new uint32_t[nFaces]);
        memset(remap.get(), 0xcd, sizeof(uint32_t) * nFaces);

        float acmr32 = 0.f;
        float atvr32 = 0.f;

        HRESULT hr = OptimizeFacesLRU(indices.data(), nFaces, remap.get());
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) torus failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(indices.data(), remap.get(), nFaces))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) torus failed remap invalid\n");
            for (size_t j = 0; j < nFaces; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else
        {
            std::vector<uint16_t> reorderedIB(indices.cbegin(), indices.cend());

            hr = ReorderIB(reorderedIB.data(), nFaces, remap.get());
            if (FAILED(hr))
            {
                success = false;
                printe("ERROR: OptimizeFacesLRU(16) torus reorder failed (%08X)\n", static_cast<unsigned int>(hr));
            }
            else
            {
                ComputeVertexCacheMissRate(reorderedIB.data(), nFaces, vertices.size(), OPTFACES_V_DEFAULT, acmr32, atvr32);

                if (acmr32 > acmrOrig
                    || atvr32 > atvrOrig)
                {
                    success = false;
                    printe("ERROR: OptimizeFacesLRU(16) torus failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmr32, acmrOrig, atvr32, atvrOrig);
                }
            }
        }

        // vertex cache size
        memset(remap.get(), 0xcd, sizeof(uint32_t) * nFaces);

        hr = OptimizeFacesLRU(indices.data(), nFaces, remap.get(), 4);
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) torus lru4 failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(indices.data(), remap.get(), nFaces))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(16) torus lru4 failed remap invalid\n");
            for (size_t j = 0; j < nFaces; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else
        {
            std::vector<uint16_t> reorderedIB(indices.cbegin(), indices.cend());

            hr = ReorderIB(reorderedIB.data(), nFaces, remap.get());
            if (FAILED(hr))
            {
                success = false;
                printe("ERROR: OptimizeFacesLRU(16) torus lru4 reorder failed (%08X)\n", static_cast<unsigned int>(hr));
            }
            else
            {
                float acmr, atvr;
                ComputeVertexCacheMissRate(reorderedIB.data(), nFaces, vertices.size(), OPTFACES_V_DEFAULT, acmr, atvr);

                if (acmr > acmrOrig
                    || atvr > atvrOrig)
                {
                    success = false;
                    printe("ERROR: OptimizeFacesLRU(16) torus lru4 failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, acmrOrig, atvr, atvrOrig);
                }

                if (acmr < acmr32
                    || atvr < atvr32)
                {
                    success = false;
                    printe("ERROR: OptimizeFacesLRU(16) torus lru4 vs lru32 failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, acmr32, atvr, atvr32);
                }
            }
        }
    }

    // 32-bit torus
    {
        std::vector<uint32_t> indices;
        std::vector<ShapesGenerator<uint32_t>::Vertex> vertices;
        ShapesGenerator<uint32_t>::CreateTorus(indices, vertices, 1.f, 0.333f, 32, false);

        size_t nFaces = indices.size() / 3;

        float acmrOrig, atvrOrig;
        ComputeVertexCacheMissRate(indices.data(), nFaces, vertices.size(), OPTFACES_V_DEFAULT, acmrOrig, atvrOrig);

        std::unique_ptr<uint32_t[]> remap(new uint32_t[nFaces]);
        memset(remap.get(), 0xcd, sizeof(uint32_t) * nFaces);

        float acmr32 = 0.f;
        float atvr32 = 0.f;

        HRESULT hr = OptimizeFacesLRU(indices.data(), nFaces, remap.get());
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) torus failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(indices.data(), remap.get(), nFaces))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) torus failed remap invalid\n");
            for (size_t j = 0; j < nFaces; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else
        {
            std::vector<uint32_t> reorderedIB(indices.cbegin(), indices.cend());

            hr = ReorderIB(reorderedIB.data(), nFaces, remap.get());
            if (FAILED(hr))
            {
                success = false;
                printe("ERROR: OptimizeFacesLRU(32) torus reorder failed (%08X)\n", static_cast<unsigned int>(hr));
            }
            else
            {
                ComputeVertexCacheMissRate(reorderedIB.data(), nFaces, vertices.size(), OPTFACES_V_DEFAULT, acmr32, atvr32);

                if (acmr32 > acmrOrig
                    || atvr32 > atvrOrig)
                {
                    success = false;
                    printe("ERROR: OptimizeFacesLRU(32) torus failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmr32, acmrOrig, atvr32, atvrOrig);
                }
            }
        }

        // vertex cache size
        memset(remap.get(), 0xcd, sizeof(uint32_t) * nFaces);

        hr = OptimizeFacesLRU(indices.data(), nFaces, remap.get(), 4);
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) torus lru4 failed (%08X)\n", static_cast<unsigned int>(hr));
        }
        else if (!IsValidFaceRemap(indices.data(), remap.get(), nFaces))
        {
            success = false;
            printe("ERROR: OptimizeFacesLRU(32) torus lru4 failed remap invalid\n");
            for (size_t j = 0; j < nFaces; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else
        {
            std::vector<uint32_t> reorderedIB(indices.cbegin(), indices.cend());

            hr = ReorderIB(reorderedIB.data(), nFaces, remap.get());
            if (FAILED(hr))
            {
                success = false;
                printe("ERROR: OptimizeFacesLRU(32) torus lru4 reorder failed (%08X)\n", static_cast<unsigned int>(hr));
            }
            else
            {
                float acmr, atvr;
                ComputeVertexCacheMissRate(reorderedIB.data(), nFaces, vertices.size(), OPTFACES_V_DEFAULT, acmr, atvr);

                if (acmr > acmrOrig
                    || atvr > atvrOrig)
                {
                    success = false;
                    printe("ERROR: OptimizeFacesLRU(32) torus lru4 failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, acmrOrig, atvr, atvrOrig);
                }

                if (acmr < acmr32
                    || atvr < atvr32)
                {
                    success = false;
                    printe("ERROR: OptimizeFacesLRU(32) torus lru4 vs lru32 failed ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, acmr32, atvr, atvr32);
                }
            }
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
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 8 );

        static const uint32_t s_remap[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

        HRESULT hr = OptimizeVertices( g_cubeIndices16, 12, 8, remap.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(16) cube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidVertexRemap( g_cubeIndices16, 12, remap.get(), 8 ) )
        {
            printe("ERROR: OptimizeVertices(16) cube failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_remap, sizeof(s_remap) ) != 0 )
        {
            printe("ERROR: OptimizeVertices(16) cube failed\n" );
            success = false;
            for (size_t j = 0; j < 8; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_remap[j]);
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = OptimizeVertices( g_cubeIndices16, 12, UINT16_MAX /*D3D11_16BIT_INDEX_STRIP_CUT_VALUE*/, remap.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: OptimizeVertices(16) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = OptimizeVertices( g_cubeIndices16, 12, UINT32_MAX, remap.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: OptimizeVertices(16) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = OptimizeVertices( g_cubeIndices16, UINT32_MAX, 8, remap.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: OptimizeVertices(16) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = OptimizeVertices( g_cubeIndices16, 12, 2, remap.get() );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: OptimizeVertices(16) expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // 16-bit fmcube
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 24 ] );
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 24 );

        const static uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, 13, 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23 };

        HRESULT hr = OptimizeVertices( g_fmCubeIndices16, 12, 24, remap.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(16) fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidVertexRemap( g_fmCubeIndices16, 12, remap.get(), 24 ) )
        {
            printe("ERROR: OptimizeVertices(16) fmcube failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 24; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_remap, sizeof(s_remap) ) != 0 )
        {
            printe("ERROR: OptimizeVertices(16) fmcube failed\n" );
            success = false;
            for (size_t j = 0; j < 24; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_remap[j]);
        }
    }

    // 32-bit cube
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 8 ] );
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 8 );

        static const uint32_t s_remap[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

        HRESULT hr = OptimizeVertices( g_cubeIndices32, 12, 8, remap.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(32) cube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidVertexRemap( g_cubeIndices32, 12, remap.get(), 8 ) )
        {
            printe("ERROR: OptimizeVertices(32) cube failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_remap, sizeof(s_remap) ) != 0 )
        {
            printe("ERROR: OptimizeVertices(32) cube failed\n" );
            success = false;
            for (size_t j = 0; j < 8; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_remap[j]);
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = OptimizeVertices( g_cubeIndices32, 12, UINT32_MAX /*D3D11_32BIT_INDEX_STRIP_CUT_VALUE*/, remap.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: OptimizeVertices(32) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = OptimizeVertices( g_cubeIndices32, 12, UINT32_MAX, remap.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: OptimizeVertices(32) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = OptimizeVertices( g_cubeIndices32, UINT32_MAX, 8, remap.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: OptimizeVertices(32) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = OptimizeVertices( g_cubeIndices32, 12, 2, remap.get() );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: OptimizeVertices(32) expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // 32-bit fmcube
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 24 ] );
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 24 );

        const static uint32_t s_remap[] = { 3, 1, 0, 2,
                                            6, 4, 5, 7,
                                            11, 9, 8, 10,
                                            14, 12, 13, 15,
                                            19, 17, 16, 18,
                                            22, 20, 21, 23 };

        HRESULT hr = OptimizeVertices( g_fmCubeIndices32, 12, 24, remap.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(32) fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidVertexRemap( g_fmCubeIndices32, 12, remap.get(), 24 ) )
        {
            printe("ERROR: OptimizeVertices(32) fmcube failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 24; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_remap, sizeof(s_remap) ) != 0 )
        {
            printe("ERROR: OptimizeVertices(32) fmcube failed\n" );
            success = false;
            for (size_t j = 0; j < 24; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_remap[j]);
        }
    }

    // Unused 16
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 8 ] );
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 8 );

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

        size_t trailingUnused = 0xff;
        HRESULT hr = OptimizeVertices( s_unusedIB, 12, 8, remap.get(), &trailingUnused );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(16) unused faces failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidVertexRemap( s_unusedIB, 12, remap.get(), 8 ) )
        {
            printe("ERROR: OptimizeVertices(16) unused faces failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_uforder, sizeof(s_uforder) ) != 0 )
        {
            printe("ERROR: OptimizeVertices(16) unused faces failed\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_uforder[j]);
        }
        else if (trailingUnused != 0)
        {
            printe("ERROR: OptimizeVertices(16) unused faces produced expected number of trailing unsed (%zu .. 0)\n", trailingUnused);
            success = false;
        }

        // Vertices
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 8 );
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

        hr = OptimizeVertices( s_unusedVerts, 7, 8, remap.get(), &trailingUnused );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(16) unused verts failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidVertexRemap( s_unusedVerts, 7, remap.get(), 8 ) )
        {
            printe("ERROR: OptimizeVertices(16) unused verts failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_uvorder, sizeof(s_uvorder) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeVertices(16) unused verts failed\n" );
            for(size_t j=0; j < 8; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_uvorder[j]);
        }
        else if (trailingUnused != 1)
        {
            printe("ERROR: OptimizeVertices(16) unused faces produced expected number of trailing unsed (%zu .. 1)\n", trailingUnused);
            success = false;
        }
    }

    // Unused 32
    {
        std::unique_ptr<uint32_t[]> remap( new uint32_t[ 8 ] );
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 8 );

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

        size_t trailingUnused = 0xff;
        HRESULT hr = OptimizeVertices( s_unusedIB, 12, 8, remap.get(), &trailingUnused );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(32) unused faces failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidVertexRemap( s_unusedIB, 12, remap.get(), 8 ) )
        {
            printe("ERROR: OptimizeVertices(32) unused faces failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_uforder, sizeof(s_uforder) ) != 0 )
        {
            printe("ERROR: OptimizeVertices(32) unused faces failed\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_uforder[j]);
        }
        else if (trailingUnused != 0)
        {
            printe("ERROR: OptimizeVertices(32) unused faces produced expected number of trailing unsed (%zu .. 0)\n", trailingUnused);
            success = false;
        }
        // Vertices
        memset( remap.get(), 0xcd, sizeof(uint32_t) * 8 );
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

        hr = OptimizeVertices( s_unusedVerts, 7, 8, remap.get(), &trailingUnused );
        if ( FAILED(hr) )
        {
            printe("ERROR: OptimizeVertices(32) unused verts failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !IsValidVertexRemap( s_unusedVerts, 7, remap.get(), 8 ) )
        {
            printe("ERROR: OptimizeVertices(32) unused verts failed remap invalid\n" );
            success = false;
            for(size_t j=0; j < 8; ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else if ( memcmp( remap.get(), s_uvorder, sizeof(s_uvorder) ) != 0 )
        {
            success = false;
            printe("ERROR: OptimizeVertices(32) unused verts failed\n" );
            for(size_t j=0; j < 8; ++j)
                print("%zu -> %u .. %u\n", j, remap[j], s_uvorder[j]);
        }
        else if (trailingUnused != 1)
        {
            printe("ERROR: OptimizeVertices(32) unused faces produced expected number of trailing unsed (%zu .. 1)\n", trailingUnused);
            success = false;
        }
    }

    // 16-bit torus
    {
        std::vector<uint16_t> indices;
        std::vector<ShapesGenerator<uint16_t>::Vertex> vertices;
        ShapesGenerator<uint16_t>::CreateTorus(indices, vertices, 1.f, 0.333f, 32, false);

        std::unique_ptr<uint32_t[]> remap( new uint32_t[ vertices.size() ] );
        memset( remap.get(), 0xcd, sizeof(uint32_t) * vertices.size() );

        size_t nFaces = indices.size() / 3;

        HRESULT hr = OptimizeVertices(indices.data(), nFaces, vertices.size(), remap.get());
        if (FAILED(hr))
        {
            printe("ERROR: OptimizeVertices(16) torus failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!IsValidVertexRemap(indices.data(), nFaces, remap.get(), vertices.size()))
        {
            printe("ERROR: OptimizeVertices(16) torus failed remap invalid\n");
            success = false;
            for (size_t j = 0; j < vertices.size(); ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else
        {
            std::unique_ptr<uint16_t[]> newIndices(new uint16_t[nFaces * 3]);
            hr = FinalizeIB(indices.data(), nFaces, remap.get(), vertices.size(), newIndices.get());
            if (FAILED(hr))
            {
                printe("ERROR: OptimizeVertices(16) torus failed finalize IB (%08X)\n", static_cast<unsigned int>(hr));
                success = false;
            }
            else if (memcmp(indices.data(), newIndices.get(), sizeof(uint16_t) * nFaces *3) == 0)
            {
                printe("ERROR: OptimizeVertices(16) torus failed to change order of vertices\n");
                success = false;
            }
            else
            {
                hr = FinalizeVB(vertices.data(), sizeof(ShapesGenerator<uint16_t>::Vertex), vertices.size(), remap.get());
                if (FAILED(hr))
                {
                    printe("ERROR: OptimizeVertices(16) torus failed finalize VB (%08X)\n", static_cast<unsigned int>(hr));
                    success = false;
                }
            }
        }
    }

    // 32-bit torus
    {
        std::vector<uint32_t> indices;
        std::vector<ShapesGenerator<uint32_t>::Vertex> vertices;
        ShapesGenerator<uint32_t>::CreateTorus(indices, vertices, 1.f, 0.333f, 32, false);

        std::unique_ptr<uint32_t[]> remap(new uint32_t[vertices.size()]);
        memset(remap.get(), 0xcd, sizeof(uint32_t) * vertices.size());

        size_t nFaces = indices.size() / 3;

        HRESULT hr = OptimizeVertices(indices.data(), nFaces, vertices.size(), remap.get());
        if (FAILED(hr))
        {
            printe("ERROR: OptimizeVertices(32) torus failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!IsValidVertexRemap(indices.data(), nFaces, remap.get(), vertices.size()))
        {
            printe("ERROR: OptimizeVertices(32) torus failed remap invalid\n");
            success = false;
            for (size_t j = 0; j < vertices.size(); ++j)
                print("%zu -> %u\n", j, remap[j]);
        }
        else
        {
            std::unique_ptr<uint32_t[]> newIndices(new uint32_t[nFaces * 3]);
            hr = FinalizeIB(indices.data(), nFaces, remap.get(), vertices.size(), newIndices.get());
            if (FAILED(hr))
            {
                printe("ERROR: OptimizeVertices(32) torus failed finalize IB (%08X)\n", static_cast<unsigned int>(hr));
                success = false;
            }
            else if (memcmp(indices.data(), newIndices.get(), sizeof(uint32_t) * nFaces * 3) == 0)
            {
                printe("ERROR: OptimizeVertices(32) torus failed to change order of vertices\n");
                success = false;
            }
            else
            {
                hr = FinalizeVB(vertices.data(), sizeof(ShapesGenerator<uint32_t>::Vertex), vertices.size(), remap.get());
                if (FAILED(hr))
                {
                    printe("ERROR: OptimizeVertices(32) torus failed finalize VB (%08X)\n", static_cast<unsigned int>(hr));
                    success = false;
                }
            }
        }
    }

    return success;
}
