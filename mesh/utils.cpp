//-------------------------------------------------------------------------------------
// utils.cpp
//  
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#include "directxtest.h"

#include "DirectXMeshP.h"

#include "TestHelpers.h"
#include "TestGeometry.h"
#include "ShapesGenerator.h"

using namespace DirectX;

namespace
{
    const UINT DXGI_START = 1;
    const UINT DXGI_END = 190; // as of DXGI 1.3

    const float g_Epsilon = 0.0001f;
}

//-------------------------------------------------------------------------------------
// IsValidVB
// IsValidIB
// BytesPerElement
bool Test01()
{
    static const DXGI_FORMAT ibFormats[] =
    {
        DXGI_FORMAT_R16_UINT, DXGI_FORMAT_R32_UINT,
    };
    static const DXGI_FORMAT vbElements[] =
    {
        // No typeless, BC, packed, depth, video, XR 2 bias, sRGB, RGBE, alpha-only, or 1-bit is valid for VB decls
        DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_UINT, DXGI_FORMAT_R32G32B32A32_SINT,
        DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32_UINT, DXGI_FORMAT_R32G32B32_SINT,
        DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UINT, DXGI_FORMAT_R16G16B16A16_SNORM, DXGI_FORMAT_R16G16B16A16_SINT,
        DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_UINT, DXGI_FORMAT_R32G32_SINT,
        DXGI_FORMAT_R10G10B10A2_UNORM, DXGI_FORMAT_R10G10B10A2_UINT,
        DXGI_FORMAT_R11G11B10_FLOAT,
        DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UINT, DXGI_FORMAT_R8G8B8A8_SNORM, DXGI_FORMAT_R8G8B8A8_SINT,
        DXGI_FORMAT_R16G16_FLOAT, DXGI_FORMAT_R16G16_UNORM, DXGI_FORMAT_R16G16_UINT, DXGI_FORMAT_R16G16_SNORM, DXGI_FORMAT_R16G16_SINT,
        DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_UINT, DXGI_FORMAT_R32_SINT,
        DXGI_FORMAT_R8G8_UNORM, DXGI_FORMAT_R8G8_UINT, DXGI_FORMAT_R8G8_SNORM, DXGI_FORMAT_R8G8_SINT,
        DXGI_FORMAT_R16_FLOAT, DXGI_FORMAT_R16_UNORM, DXGI_FORMAT_R16_UINT, DXGI_FORMAT_R16_SNORM, DXGI_FORMAT_R16_SINT,
        DXGI_FORMAT_R8_UNORM, DXGI_FORMAT_R8_UINT, DXGI_FORMAT_R8_SNORM, DXGI_FORMAT_R8_SINT,
        DXGI_FORMAT_B5G6R5_UNORM, DXGI_FORMAT_B5G5R5A1_UNORM,
        DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_B8G8R8X8_UNORM,
        DXGI_FORMAT_B4G4R4A4_UNORM,
        XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM,
    };

    bool success = true;

    for (UINT f = DXGI_START; f <= DXGI_END; ++f )
    {
        auto itib = std::find( std::begin(ibFormats), std::end(ibFormats), (DXGI_FORMAT)f );
        bool isib = (itib != std::end(ibFormats) );

        auto itvb = std::find( std::begin(vbElements), std::end(vbElements), (DXGI_FORMAT)f );
        bool isvb = (itvb != std::end(vbElements) );

        if ( IsValidIB( (DXGI_FORMAT)f ) != isib )
        {
            printe( "ERROR: IsValidIB failed on DXGI Format %u\n", f );
            success = false;
        }

        if ( IsValidVB( (DXGI_FORMAT)f ) != isvb )
        {
            printe( "ERROR: IsValidVB failed on DXGI Format %u\n", f );
            success = false;
        }

        if ( BytesPerElement( (DXGI_FORMAT)f ) == 0 && (isvb || isib) )
        {
            printe( "ERROR: BytesPerElement failed on DXGI Format %u\n", f );
            success = false;
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// TestHelpers (internal)
bool Test02()
{
    bool success = true;

    // IB helpers
    {
        auto ib = CreateIndexBuffer<uint16_t>( 1023, IB_IDENTITY );
        if ( !ib || !IsTestIBCorrect( ib.get(), 1023, IB_IDENTITY) )
        {
            printe("ERROR: CreateIndexBuffer(16) identity failed\n" );
            success = false;
        }

        ib = CreateIndexBuffer<uint16_t>( 1023, IB_REVERSE );
        if ( !ib || !IsTestIBCorrect( ib.get(), 1023, IB_REVERSE) )
        {
            printe("ERROR: CreateIndexBuffer(16) reverse failed\n" );
            success = false;
        }

        ib = CreateIndexBuffer<uint16_t>( 1023, IB_ZERO );
        if ( !ib || !IsTestIBCorrect( ib.get(), 1023, IB_ZERO) )
        {
            printe("ERROR: CreateIndexBuffer(16) zero failed\n" );
            success = false;
        }
    }

    {
        auto ib = CreateIndexBuffer<uint32_t>( 1023, IB_IDENTITY );
        if ( !ib || !IsTestIBCorrect( ib.get(), 1023, IB_IDENTITY) )
        {
            printe("ERROR: CreateIndexBuffer(32) identity failed\n" );
            success = false;
        }

        ib = CreateIndexBuffer<uint32_t>( 1023, IB_REVERSE );
        if ( !ib || !IsTestIBCorrect( ib.get(), 1023, IB_REVERSE) )
        {
            printe("ERROR: CreateIndexBuffer(32) reverse failed\n" );
            success = false;
        }

        ib = CreateIndexBuffer<uint32_t>( 1023, IB_ZERO );
        if ( !ib || !IsTestIBCorrect( ib.get(), 1023, IB_ZERO) )
        {
            printe("ERROR: CreateIndexBuffer(32) zero failed\n" );
            success = false;
        }
    }

    // VB helpers
    {
        auto vb = CreateVertexBuffer( 32, 65535 );
        if ( !vb )
        {
            printe("ERROR: CreateVertexBuffer failed\n" );
            success = false;
        }
           
        vb = CreateVertexBuffer32( 65535, VB_IDENTITY );
        if ( !vb || !IsTestVBCorrect32( vb.get(), 65535, VB_IDENTITY ) )
        {
            printe("ERROR: CreateVertexBuffer32 identity failed\n" );
            success = false;
        }

        vb = CreateVertexBuffer32( 65535, VB_REVERSE );
        if ( !vb || !IsTestVBCorrect32( vb.get(), 65535, VB_REVERSE ) )
        {
            printe("ERROR: CreateVertexBuffer32 reverse failed\n" );
            success = false;
        }

        vb = CreateVertexBuffer32( 65535, VB_ZERO );
        if ( !vb || !IsTestVBCorrect32( vb.get(), 65535, VB_ZERO ) )
        {
            printe("ERROR: CreateVertexBuffer32 zero failed\n" );
            success = false;
        }

        vb = CreateVertexBuffer16( 65535, VB_IDENTITY );
        if ( !vb || !IsTestVBCorrect16( vb.get(), 65535, VB_IDENTITY ) )
        {
            printe("ERROR: CreateVertexBuffer16 identity failed\n" );
            success = false;
        }

        vb = CreateVertexBuffer16( 65535, VB_REVERSE );
        if ( !vb || !IsTestVBCorrect16( vb.get(), 65535, VB_REVERSE ) )
        {
            printe("ERROR: CreateVertexBuffer16 reverse failed\n" );
            success = false;
        }

        vb = CreateVertexBuffer16( 65535, VB_ZERO );
        if ( !vb || !IsTestVBCorrect16( vb.get(), 65535, VB_ZERO ) )
        {
            printe("ERROR: CreateVertexBuffer16 zero failed\n" );
            success = false;
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// ComputeVertexCacheMissRate
bool Test22()
{
    using namespace TestGeometry;

    bool success = true;

    // 16-bit fmcube
    {
        float acmr, atvr;
        ComputeVertexCacheMissRate( g_fmCubeIndices16, 12, 24, OPTFACES_V_DEFAULT, acmr, atvr );

        if ( fabsf(acmr - 2.f) > g_Epsilon
             || fabsf(atvr - 1.f) > g_Epsilon )
        {
            printe("ERROR: ComputeVertexCacheMissRate(16) fmcube ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, 2.f, atvr, 1.f );
            success = false;
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385)
        ComputeVertexCacheMissRate( g_fmCubeIndices16, 12, D3D11_16BIT_INDEX_STRIP_CUT_VALUE, OPTFACES_V_DEFAULT, acmr, atvr );
        if( acmr != -1.f || atvr != -1.f )
        {
            printe("\nERROR: ComputeVertexCacheMissRate(16) expected failure for strip cut value (ACMR %f, ATVR %f)\n", acmr, atvr );
            success = false;
        }

        ComputeVertexCacheMissRate( g_fmCubeIndices16, 12, UINT32_MAX, OPTFACES_V_DEFAULT, acmr, atvr );
        if( acmr != -1.f || atvr != -1.f )
        {
            printe("\nERROR: ComputeVertexCacheMissRate(16) expected failure for 32-max value verts (ACMR %f, ATVR %f)\n", acmr, atvr );
            success = false;
        }

        ComputeVertexCacheMissRate( g_fmCubeIndices16, UINT32_MAX, 24, OPTFACES_V_DEFAULT, acmr, atvr );
        if( acmr != -1.f || atvr != -1.f )
        {
            printe("\nERROR: ComputeVertexCacheMissRate(16) expected failure for 32-max value faces (ACMR %f, ATVR %f)\n", acmr, atvr );
            success = false;
        }
        #pragma warning(pop)
    }

    // 16-bit sphere
    {
        std::vector<uint16_t> indices;
        std::vector<ShapesGenerator<uint16_t>::Vertex> vertices;
        ShapesGenerator<uint16_t>::CreateSphere( indices, vertices, 1.f, 16, false );

        float acmr, atvr;
        ComputeVertexCacheMissRate( indices.data(), indices.size() / 3, vertices.size(), OPTFACES_V_DEFAULT, acmr, atvr );

        if ( fabsf(acmr - 1.016098f) > g_Epsilon
             || fabsf(atvr - 1.912656f) > g_Epsilon )
        {
            printe("ERROR: ComputeVertexCacheMissRate(16) sphere ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, 1.016098f, atvr, 1.912656f );
            success = false;
        }
    }

    // 16-bit cylinder
    {
        std::vector<uint16_t> indices;
        std::vector<ShapesGenerator<uint16_t>::Vertex> vertices;
        ShapesGenerator<uint16_t>::CreateCylinder( indices, vertices, 1.f, 1.f, 32, false );

        float acmr, atvr;
        ComputeVertexCacheMissRate( indices.data(), indices.size() / 3, vertices.size(), OPTFACES_V_DEFAULT, acmr, atvr );

        if ( fabsf(acmr - 1.079365f) > g_Epsilon
             || fabsf(atvr - 1.046154f) > g_Epsilon )
        {
            printe("ERROR: ComputeVertexCacheMissRate(16) cylinder ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, 1.079365f, atvr, 1.046154f );
            success = false;
        }
    }

    // 16-bit torus
    {
        std::vector<uint16_t> indices;
        std::vector<ShapesGenerator<uint16_t>::Vertex> vertices;
        ShapesGenerator<uint16_t>::CreateTorus( indices, vertices, 1.f, 0.333f, 32, false );

        float acmr, atvr;
        ComputeVertexCacheMissRate( indices.data(), indices.size() / 3, vertices.size(), OPTFACES_V_DEFAULT, acmr, atvr );

        if ( fabsf(acmr - 1.015611f) > g_Epsilon
             || fabsf(atvr - 2.031221f) > g_Epsilon )
        {
            printe("ERROR: ComputeVertexCacheMissRate(16) torus ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, 1.015611f, atvr, 2.031221f );
            success = false;
        }
    }

    // 32-bit fmcube
    {
        float acmr, atvr;
        ComputeVertexCacheMissRate( g_fmCubeIndices32, 12, 24, OPTFACES_V_DEFAULT, acmr, atvr );

        if ( fabsf(acmr - 2.f) > g_Epsilon
             || fabsf(atvr - 1.f) > g_Epsilon )
        {
            printe("ERROR: ComputeVertexCacheMissRate(32) fmcube ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, 2.f, atvr, 1.f );
            success = false;
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385 6387)  
        ComputeVertexCacheMissRate( g_fmCubeIndices32, 12, D3D11_32BIT_INDEX_STRIP_CUT_VALUE, OPTFACES_V_DEFAULT, acmr, atvr );
        if( acmr != -1.f || atvr != -1.f )
        {
            printe("\nERROR: ComputeVertexCacheMissRate(32) expected failure for strip cut value (ACMR %f, ATVR %f)\n", acmr, atvr );
            success = false;
        }

        ComputeVertexCacheMissRate( g_fmCubeIndices32, 12, UINT32_MAX, OPTFACES_V_DEFAULT, acmr, atvr );
        if( acmr != -1.f || atvr != -1.f )
        {
            printe("\nERROR: ComputeVertexCacheMissRate(32) expected failure for 32-max value verts (ACMR %f, ATVR %f)\n", acmr, atvr );
            success = false;
        }

        ComputeVertexCacheMissRate( g_fmCubeIndices32, UINT32_MAX, 24, OPTFACES_V_DEFAULT, acmr, atvr );
        if( acmr != -1.f || atvr != -1.f )
        {
            printe("\nERROR: ComputeVertexCacheMissRate(32) expected failure for 32-max value faces (ACMR %f, ATVR %f)\n", acmr, atvr );
            success = false;
        }
        #pragma warning(pop)
    }

    // 32-bit sphere
    {
        std::vector<uint32_t> indices;
        std::vector<ShapesGenerator<uint32_t>::Vertex> vertices;
        ShapesGenerator<uint32_t>::CreateSphere( indices, vertices, 1.f, 16, false );

        float acmr, atvr;
        ComputeVertexCacheMissRate( indices.data(), indices.size() / 3, vertices.size(), OPTFACES_V_DEFAULT, acmr, atvr );

        if ( fabsf(acmr - 1.016098f) > g_Epsilon
             || fabsf(atvr - 1.912656f) > g_Epsilon )
        {
            printe("ERROR: ComputeVertexCacheMissRate(32) sphere ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, 1.016098f, atvr, 1.912656f );
            success = false;
        }
    }

    // 32-bit cylinder
    {
        std::vector<uint32_t> indices;
        std::vector<ShapesGenerator<uint32_t>::Vertex> vertices;
        ShapesGenerator<uint32_t>::CreateCylinder( indices, vertices, 1.f, 1.f, 32, false );

        float acmr, atvr;
        ComputeVertexCacheMissRate( indices.data(), indices.size() / 3, vertices.size(), OPTFACES_V_DEFAULT, acmr, atvr );

        if ( fabsf(acmr - 1.079365f) > g_Epsilon
             || fabsf(atvr - 1.046154f) > g_Epsilon )
        {
            printe("ERROR: ComputeVertexCacheMissRate(32) cylinder ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, 1.079365f, atvr, 1.046154f );
            success = false;
        }
    }

    // 32-bit torus
    {
        std::vector<uint32_t> indices;
        std::vector<ShapesGenerator<uint32_t>::Vertex> vertices;
        ShapesGenerator<uint32_t>::CreateTorus( indices, vertices, 1.f, 0.333f, 32, false );

        float acmr, atvr;
        ComputeVertexCacheMissRate( indices.data(), indices.size() / 3, vertices.size(), OPTFACES_V_DEFAULT, acmr, atvr );

        if ( fabsf(acmr - 1.015611f) > g_Epsilon
             || fabsf(atvr - 2.031221f) > g_Epsilon )
        {
            printe("ERROR: ComputeVertexCacheMissRate(32) torus ACMR: %f .. %f, ATVR: %f .. %f\n", acmr, 1.015611f, atvr, 2.031221f );
            success = false;
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// ComputeSubsets
bool Test24()
{
    bool success = true;

    // empty
    {
        auto subsets = ComputeSubsets( nullptr, 0 );
        if ( !subsets.empty() )
        {
            printe("ERROR: ComputeSubsets empty failed count %zu .. 0\n", subsets.size() );
            success = false;
        }
    }

    // null
    {
        static const std::pair<size_t,size_t> s_result[] = {
            std::pair<size_t,size_t>( 0, 12 ),
        };

        auto subsets = ComputeSubsets( nullptr, 12 );
        if ( subsets.size() != 1 )
        {
            printe("ERROR: ComputeSubsets null failed count %zu .. 1\n", subsets.size() );
            success = false;
        }
        else if ( memcmp( s_result, subsets.data(), sizeof(s_result) ) != 0 )
        {
            for( auto it = subsets.cbegin(); it != subsets.cend(); ++it )
            {
                printe("ERROR: ComputeSubsets null failed %zu,%zu\n", it->first, it->second );
            }
            success = false;
        }
    }

    // identity
    {
        static const uint32_t s_attr[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

        static const std::pair<size_t,size_t> s_result[] = {
            std::pair<size_t,size_t>( 0, 1 ),
            std::pair<size_t,size_t>( 1, 1 ),
            std::pair<size_t,size_t>( 2, 1 ),
            std::pair<size_t,size_t>( 3, 1 ),
            std::pair<size_t,size_t>( 4, 1 ),
            std::pair<size_t,size_t>( 5, 1 ),
            std::pair<size_t,size_t>( 6, 1 ),
            std::pair<size_t,size_t>( 7, 1 ),
            std::pair<size_t,size_t>( 8, 1 ),
            std::pair<size_t,size_t>( 9, 1 ),
            std::pair<size_t,size_t>( 10, 1 ),
            std::pair<size_t,size_t>( 11, 1 ),
        };

        auto subsets = ComputeSubsets( s_attr, _countof(s_attr) );
        if ( subsets.size() != 12 )
        {
            printe("ERROR: ComputeSubsets identity failed count %zu .. 12\n", subsets.size() );
            success = false;
        }
        else if ( memcmp( s_result, subsets.data(), sizeof(s_result) ) != 0 )
        {
            for( auto it = subsets.cbegin(); it != subsets.cend(); ++it )
            {
                printe("ERROR: ComputeSubsets identity failed %zu,%zu\n", it->first, it->second );
            }
            success = false;
        }
    }

    // zero
    {
        static const uint32_t s_attr[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

        static const std::pair<size_t,size_t> s_result[] = {
            std::pair<size_t,size_t>( 0, 12 ),
        };

        auto subsets = ComputeSubsets( s_attr, _countof(s_attr) );
        if ( subsets.size() != 1 )
        {
            printe("ERROR: ComputeSubsets zero failed count %zu .. 1\n", subsets.size() );
            success = false;
        }
        else if ( memcmp( s_result, subsets.data(), sizeof(s_result) ) != 0 )
        {
            for( auto it = subsets.cbegin(); it != subsets.cend(); ++it )
            {
                printe("ERROR: ComputeSubsets zero failed %zu,%zu\n", it->first, it->second );
            }
            success = false;
        }
    }

    // unsorted with duplicates
    {
        static const uint32_t s_attr[] = { 5, 2, 4, 2, 0, 4, 0, 5, 2, 3, 1, 1 };

        static const std::pair<size_t,size_t> s_result[] = {
            std::pair<size_t,size_t>( 0, 1 ),
            std::pair<size_t,size_t>( 1, 1 ),
            std::pair<size_t,size_t>( 2, 1 ),
            std::pair<size_t,size_t>( 3, 1 ),
            std::pair<size_t,size_t>( 4, 1 ),
            std::pair<size_t,size_t>( 5, 1 ),
            std::pair<size_t,size_t>( 6, 1 ),
            std::pair<size_t,size_t>( 7, 1 ),
            std::pair<size_t,size_t>( 8, 1 ),
            std::pair<size_t,size_t>( 9, 1 ),
            std::pair<size_t,size_t>( 10, 2 ),
        };

        auto subsets = ComputeSubsets( s_attr, _countof(s_attr) );
        if ( subsets.size() != 11 )
        {
            printe("ERROR: ComputeSubsets unsorted failed count %zu .. 11\n", subsets.size() );
            success = false;
        }
        else if ( memcmp( s_result, subsets.data(), sizeof(s_result) ) != 0 )
        {
            for( auto it = subsets.cbegin(); it != subsets.cend(); ++it )
            {
                printe("ERROR: ComputeSubsets unsorted failed %zu,%zu\n", it->first, it->second );
            }
            success = false;
        }
    }

    // sorted
    {
        static const uint32_t s_attr[] = { 0, 0, 1, 1, 2, 2, 2, 3, 4, 4, 5, 5 }; 

        static const std::pair<size_t,size_t> s_result[] = {
            std::pair<size_t,size_t>( 0, 2 ),
            std::pair<size_t,size_t>( 2, 2 ),
            std::pair<size_t,size_t>( 4, 3 ),
            std::pair<size_t,size_t>( 7, 1 ),
            std::pair<size_t,size_t>( 8, 2 ),
            std::pair<size_t,size_t>( 10, 2 ),
        };

        auto subsets = ComputeSubsets( s_attr, _countof(s_attr) );
        if ( subsets.size() != 6 )
        {
            printe("ERROR: ComputeSubsets sorted failed count %zu .. 6\n", subsets.size() );
            success = false;
        }
        else if ( memcmp( s_result, subsets.data(), sizeof(s_result) ) != 0 )
        {
            for( auto it = subsets.cbegin(); it != subsets.cend(); ++it )
            {
                printe("ERROR: ComputeSubsets sorted failed %zu,%zu\n", it->first, it->second );
            }
            success = false;
        }
    }

    // sorted (2)
    {
        static const uint32_t s_attr[] = { 3, 3, 3, 6, 7, 7, 7, 12, 12, 20, 20, 20 }; 

        static const std::pair<size_t,size_t> s_result[] = {
            std::pair<size_t,size_t>( 0, 3 ),
            std::pair<size_t,size_t>( 3, 1 ),
            std::pair<size_t,size_t>( 4, 3 ),
            std::pair<size_t,size_t>( 7, 2 ),
            std::pair<size_t,size_t>( 9, 3 ),
        };

        auto subsets = ComputeSubsets( s_attr, _countof(s_attr) );
        if ( subsets.size() != 5 )
        {
            printe("ERROR: ComputeSubsets sorted (2) failed count %zu .. 5\n", subsets.size() );
            success = false;
        }
        else if ( memcmp( s_result, subsets.data(), sizeof(s_result) ) != 0 )
        {
            for( auto it = subsets.cbegin(); it != subsets.cend(); ++it )
            {
                printe("ERROR: ComputeSubsets sorted (2) failed %zu,%zu\n", it->first, it->second );
            }
            success = false;
        }
    }

    return success;
}
