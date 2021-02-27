//-------------------------------------------------------------------------------------
// tangentframe.cpp
//  
// Copyright (c) Microsoft Corporation.
//-------------------------------------------------------------------------------------

#include "directxtest.h"

#include "DirectXMesh.h"

#include "TestHelpers.h"
#include "TestGeometry.h"

using namespace DirectX;
using namespace TestGeometry;

namespace
{
    inline bool IsValidCoVector(size_t nVerts, const XMFLOAT3* vectors)
    {
        if (!nVerts || !vectors)
            return false;

        for (size_t j = 0; j < nVerts; ++j)
        {
            XMVECTOR v = XMLoadFloat3(&vectors[j]);
            if (XMVector3IsInfinite(v) || XMVector3IsNaN(v))
            {
                return false;
            }

            if (XMVectorGetX(XMVector3Length(v)) > 1.0001f)
            {
                return false;
            }
        }

        return true;
    }
}

//-------------------------------------------------------------------------------------
// ComputeTangentFrame
bool Test13()
{
    bool success = true;

    // Face-mapped Cube
    static const XMFLOAT3 s_fmCubeNormals[24] =
        {
            XMFLOAT3( 0.f, 1.f, 0.f ),
            XMFLOAT3( 0.f, 1.f, 0.f ),
            XMFLOAT3( 0.f, 1.f, 0.f ),
            XMFLOAT3( 0.f, 1.f, 0.f ),
            XMFLOAT3( 0.f, -1.f, 0.f ),
            XMFLOAT3( 0.f, -1.f, 0.f ),
            XMFLOAT3( 0.f, -1.f, 0.f ),
            XMFLOAT3( 0.f, -1.f, 0.f ),
            XMFLOAT3( -1.f, 0.f, 0.f ),
            XMFLOAT3( -1.f, 0.f, 0.f ),
            XMFLOAT3( -1.f, 0.f, 0.f ),
            XMFLOAT3( -1.f, 0.f, 0.f ),
            XMFLOAT3( 1.f, 0.f, 0.f ),
            XMFLOAT3( 1.f, 0.f, 0.f ),
            XMFLOAT3( 1.f, 0.f, 0.f ),
            XMFLOAT3( 1.f, 0.f, 0.f ),
            XMFLOAT3( 0.f, 0.f, -1.f ),
            XMFLOAT3( 0.f, 0.f, -1.f ),
            XMFLOAT3( 0.f, 0.f, -1.f ),
            XMFLOAT3( 0.f, 0.f, -1.f ),
            XMFLOAT3( 0.f, 0.f, 1.f ),
            XMFLOAT3( 0.f, 0.f, 1.f ),
            XMFLOAT3( 0.f, 0.f, 1.f ),
            XMFLOAT3( 0.f, 0.f, 1.f ),
        };

    static const XMFLOAT3 s_fmCubeTangents[24] = 
    {
            XMFLOAT3( -1.0f, 0.0f, 0.0f ),
            XMFLOAT3( -1.0f, 0.0f, 0.0f ),
            XMFLOAT3( -1.0f, 0.0f, 0.0f ),
            XMFLOAT3( -1.0f, 0.0f, 0.0f ),
            XMFLOAT3( 1.0f, 0.0f, 0.0f ),
            XMFLOAT3( 1.0f, 0.0f, 0.0f ),
            XMFLOAT3( 1.0f, 0.0f, 0.0f ),
            XMFLOAT3( 1.0f, 0.0f, 0.0f ),
            XMFLOAT3( 0.0f, 0.0f, -1.0f ),
            XMFLOAT3( 0.0f, 0.0f, -1.0f ),
            XMFLOAT3( 0.0f, 0.0f, -1.0f ),
            XMFLOAT3( 0.0f, 0.0f, -1.0f ),
            XMFLOAT3( 0.0f, 0.0f, 1.0f ),
            XMFLOAT3( 0.0f, 0.0f, 1.0f ),
            XMFLOAT3( 0.0f, 0.0f, 1.0f ),
            XMFLOAT3( 0.0f, 0.0f, 1.0f ),
            XMFLOAT3( 1.0f, 0.0f, 0.0f ),
            XMFLOAT3( 1.0f, 0.0f, 0.0f ),
            XMFLOAT3( 1.0f, 0.0f, 0.0f ),
            XMFLOAT3( 1.0f, 0.0f, 0.0f ),
            XMFLOAT3( -1.0f, 0.0f, 0.0f ),
            XMFLOAT3( -1.0f, 0.0f, 0.0f ),
            XMFLOAT3( -1.0f, 0.0f, 0.0f ),
            XMFLOAT3( -1.0f, 0.0f, 0.0f ),
    };

    static const XMFLOAT3 s_fmCubeBiTangents[24] = 
    {
            XMFLOAT3( 0.0f, -0.0f, 1.0f ),
            XMFLOAT3( 0.0f, -0.0f, 1.0f ),
            XMFLOAT3( 0.0f, -0.0f, 1.0f ),
            XMFLOAT3( 0.0f, -0.0f, 1.0f ),
            XMFLOAT3( -0.0f, 0.0f, 1.0f ),
            XMFLOAT3( -0.0f, 0.0f, 1.0f ),
            XMFLOAT3( -0.0f, 0.0f, 1.0f ),
            XMFLOAT3( -0.0f, 0.0f, 1.0f ),
            XMFLOAT3( -0.0f, -1.0f, 0.0f ),
            XMFLOAT3( -0.0f, -1.0f, 0.0f ),
            XMFLOAT3( -0.0f, -1.0f, 0.0f ),
            XMFLOAT3( -0.0f, -1.0f, 0.0f ),
            XMFLOAT3( 0.0f, -1.0f, -0.0f ),
            XMFLOAT3( 0.0f, -1.0f, -0.0f ),
            XMFLOAT3( 0.0f, -1.0f, -0.0f ),
            XMFLOAT3( 0.0f, -1.0f, -0.0f ),
            XMFLOAT3( -0.0f, -1.0f, 0.0f ),
            XMFLOAT3( -0.0f, -1.0f, 0.0f ),
            XMFLOAT3( -0.0f, -1.0f, 0.0f ),
            XMFLOAT3( -0.0f, -1.0f, 0.0f ),
            XMFLOAT3( 0.0f, -1.0f, 0.0f ),
            XMFLOAT3( 0.0f, -1.0f, 0.0f ),
            XMFLOAT3( 0.0f, -1.0f, 0.0f ),
            XMFLOAT3( 0.0f, -1.0f, 0.0f ),
    };

    // 16-bit (face-mapped cube)
    {
        std::unique_ptr<XMFLOAT3[]> tangents( new XMFLOAT3[ 24 ] );
        std::unique_ptr<XMFLOAT4[]> tangents4( new XMFLOAT4[ 24 ] );
        std::unique_ptr<XMFLOAT3[]> bitangents( new XMFLOAT3[ 24 ] );

        memset( tangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );
        memset( tangents4.get(), 0xff, sizeof(XMFLOAT4) * 24 );
        memset( bitangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        HRESULT hr = ComputeTangentFrame( g_fmCubeIndices16, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                          tangents.get(), bitangents.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(16) fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !CompareArray( tangents.get(), s_fmCubeTangents, 24 ) )
        {
            printe("ERROR: ComputeTangentFrame(16) fmcube failed compare of tangents\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &tangents[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeTangents[j] );
                print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }
        else if ( !CompareArray( bitangents.get(), s_fmCubeBiTangents, 24 ) )
        {
            printe("ERROR: ComputeTangentFrame(16) fmcube failed compare of bitangents\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &bitangents[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeBiTangents[j] );
                print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        hr = ComputeTangentFrame( g_fmCubeIndices16, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  tangents4.get(), bitangents.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(16) fmcube [tangents4/binormal] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !CompareArray( bitangents.get(), s_fmCubeBiTangents, 24 ) )
        {
            printe("ERROR: ComputeTangentFrame(16) fmcube [tangents4/binormal] failed compare of bitangents\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &bitangents[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeBiTangents[j] );
                print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR t = XMLoadFloat3( reinterpret_cast<const XMFLOAT3*>( &tangents4[j] ) );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeTangents[j] );

                if ( !XMVector3NearEqual( t, r, g_MeshEpsilon ) )
                {
                    print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(t), XMVectorGetY(t), XMVectorGetZ(t), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
                    printe("ERROR: ComputeTangentFrame(16) fmcube [tangents4/binormal] failed compare of tangents\n" );
                    success = false;
                }

                XMVECTOR n = XMLoadFloat3( &s_fmCubeNormals[j] );
                XMVECTOR b = XMVectorScale( XMVector3Cross( n, t ), tangents4[j].w );
                r = XMLoadFloat3( &s_fmCubeBiTangents[j] );
                if ( !XMVector3NearEqual( b, r, g_MeshEpsilon ) )
                {
                    print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(b), XMVectorGetY(b), XMVectorGetZ(b), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
                    printe("ERROR: ComputeTangentFrame(16) fmcube [tangents4/binormal] failed compare of computed bitangents\n" );
                    success = false;
                }
            }
        }

        memset( tangents4.get(), 0xff, sizeof(XMFLOAT4) * 24 );

        hr = ComputeTangentFrame( g_fmCubeIndices16, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  tangents4.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(16) fmcube [tangents4] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            for( size_t j=0; j < 24; ++j)
            {
                XMVECTOR t = XMLoadFloat3( reinterpret_cast<const XMFLOAT3*>( &tangents4[j] ) );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeTangents[j] );

                if ( !XMVector3NearEqual( t, r, g_MeshEpsilon ) )
                {
                    print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(t), XMVectorGetY(t), XMVectorGetZ(t), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
                    printe("ERROR: ComputeTangentFrame(16) fmcube [tangents4] failed compare of tangents\n" );
                    success = false;
                }

                XMVECTOR n = XMLoadFloat3( &s_fmCubeNormals[j] );
                XMVECTOR b = XMVectorScale( XMVector3Cross( n, t ), tangents4[j].w );
                r = XMLoadFloat3( &s_fmCubeBiTangents[j] );
                if ( !XMVector3NearEqual( b, r, g_MeshEpsilon ) )
                {
                    print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(b), XMVectorGetY(b), XMVectorGetZ(b), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
                    printe("ERROR: ComputeTangentFrame(16) fmcube [tangents4] failed compare of computed bitangents\n" );
                    success = false;
                }
            }
        }

        // nullptr outputs
        memset( bitangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        hr = ComputeTangentFrame( g_fmCubeIndices16, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  static_cast<XMFLOAT3*>(nullptr), bitangents.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(16) fmcube [3/binormal] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !CompareArray( bitangents.get(), s_fmCubeBiTangents, 24 ) )
        {
            printe("ERROR: ComputeTangentFrame(16) fmcube [3/binormal] failed compare of bitangents\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &bitangents[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeBiTangents[j] );
                print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        memset( bitangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        hr = ComputeTangentFrame( g_fmCubeIndices16, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  static_cast<XMFLOAT4*>(nullptr), bitangents.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(16) fmcube [4/binormal] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !CompareArray( bitangents.get(), s_fmCubeBiTangents, 24 ) )
        {
            printe("ERROR: ComputeTangentFrame(16) fmcube [4/binormal] failed compare of bitangents\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &bitangents[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeBiTangents[j] );
                print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        memset( tangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        hr = ComputeTangentFrame( g_fmCubeIndices16, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  tangents.get(), nullptr );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(16) fmcube [tangent] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !CompareArray( tangents.get(), s_fmCubeTangents, 24 ) )
        {
            printe("ERROR: ComputeTangentFrame(16) fmcube [tangent] failed compare of tangents\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &tangents[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeTangents[j] );
                print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = ComputeTangentFrame( g_fmCubeIndices16, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, D3D11_16BIT_INDEX_STRIP_CUT_VALUE,
                                  tangents.get(), bitangents.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ComputeTangentFrame(16) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ComputeTangentFrame( g_fmCubeIndices16, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, UINT32_MAX,
                                  tangents.get(), bitangents.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ComputeTangentFrame(16) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ComputeTangentFrame( g_fmCubeIndices16, UINT32_MAX, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  tangents.get(), bitangents.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: ComputeTangentFrame(16) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ComputeTangentFrame( g_fmCubeIndices16, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 2,
                                  tangents.get(), bitangents.get() );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: ComputeTangentFrame(16) expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ComputeTangentFrame( g_fmCubeIndices16, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  static_cast<XMFLOAT3*>(nullptr), nullptr );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ComputeTangentFrame(16) [3] expected failure for both nullptr outputs (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ComputeTangentFrame( g_fmCubeIndices16, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  static_cast<XMFLOAT4*>(nullptr), nullptr );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ComputeTangentFrame(16) [4] expected failure for both nullptr outputs (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // 32-bit (face-mapped cube)
    {
        std::unique_ptr<XMFLOAT3[]> tangents( new XMFLOAT3[ 24 ] );
        std::unique_ptr<XMFLOAT4[]> tangents4( new XMFLOAT4[ 24 ] );
        std::unique_ptr<XMFLOAT3[]> bitangents( new XMFLOAT3[ 24 ] );

        memset( tangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );
        memset( tangents4.get(), 0xff, sizeof(XMFLOAT4) * 24 );
        memset( bitangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        HRESULT hr = ComputeTangentFrame( g_fmCubeIndices32, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                          tangents.get(), bitangents.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(32) fmcube failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !CompareArray( tangents.get(), s_fmCubeTangents, 24 ) )
        {
            printe("ERROR: ComputeTangentFrame(32) fmcube failed compare of tangents\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &tangents[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeTangents[j] );
                print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }
        else if ( !CompareArray( bitangents.get(), s_fmCubeBiTangents, 24 ) )
        {
            printe("ERROR: ComputeTangentFrame(32) fmcube failed compare of bitangents\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &bitangents[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeBiTangents[j] );
                print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }
        
        hr = ComputeTangentFrame( g_fmCubeIndices32, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  tangents4.get(), bitangents.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(32) fmcube [tangents4/binormal] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !CompareArray( bitangents.get(), s_fmCubeBiTangents, 24 ) )
        {
            printe("ERROR: ComputeTangentFrame(32) fmcube [tangents4/binormal] failed compare of bitangents\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &bitangents[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeBiTangents[j] );
                print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }
        else
        {
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR t = XMLoadFloat3( reinterpret_cast<const XMFLOAT3*>( &tangents4[j] ) );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeTangents[j] );

                if ( !XMVector3NearEqual( t, r, g_MeshEpsilon ) )
                {
                    print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(t), XMVectorGetY(t), XMVectorGetZ(t), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
                    printe("ERROR: ComputeTangentFrame(32) fmcube [tangents4/binormal] failed compare of tangents\n" );
                    success = false;
                }

                XMVECTOR n = XMLoadFloat3( &s_fmCubeNormals[j] );
                XMVECTOR b = XMVectorScale( XMVector3Cross( n, t ), tangents4[j].w );
                r = XMLoadFloat3( &s_fmCubeBiTangents[j] );
                if ( !XMVector3NearEqual( b, r, g_MeshEpsilon ) )
                {
                    print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(b), XMVectorGetY(b), XMVectorGetZ(b), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
                    printe("ERROR: ComputeTangentFrame(32) fmcube [tangents4/binormal] failed compare of computed bitangents\n" );
                    success = false;
                }
            }
        }

        memset( tangents4.get(), 0xff, sizeof(XMFLOAT4) * 24 );

        hr = ComputeTangentFrame( g_fmCubeIndices32, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  tangents4.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(32) fmcube [tangents4] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            for( size_t j=0; j < 24; ++j)
            {
                XMVECTOR t = XMLoadFloat3( reinterpret_cast<const XMFLOAT3*>( &tangents4[j] ) );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeTangents[j] );

                if ( !XMVector3NearEqual( t, r, g_MeshEpsilon ) )
                {
                    print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(t), XMVectorGetY(t), XMVectorGetZ(t), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
                    printe("ERROR: ComputeTangentFrame(32) fmcube [tangents4] failed compare of tangents\n" );
                    success = false;
                }

                XMVECTOR n = XMLoadFloat3( &s_fmCubeNormals[j] );
                XMVECTOR b = XMVectorScale( XMVector3Cross( n, t ), tangents4[j].w );
                r = XMLoadFloat3( &s_fmCubeBiTangents[j] );
                if ( !XMVector3NearEqual( b, r, g_MeshEpsilon ) )
                {
                    print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(b), XMVectorGetY(b), XMVectorGetZ(b), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
                    printe("ERROR: ComputeTangentFrame(32) fmcube [tangents4] failed compare of computed bitangents\n" );
                    success = false;
                }
            }
        }

        // nullptr outputs
        memset( bitangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        hr = ComputeTangentFrame( g_fmCubeIndices32, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  static_cast<XMFLOAT3*>(nullptr), bitangents.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(32) fmcube [3/binormal] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !CompareArray( bitangents.get(), s_fmCubeBiTangents, 24 ) )
        {
            printe("ERROR: ComputeTangentFrame(32) fmcube [3/binormal] failed compare of bitangents\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &bitangents[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeBiTangents[j] );
                print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        memset( bitangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        hr = ComputeTangentFrame( g_fmCubeIndices32, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  static_cast<XMFLOAT4*>(nullptr), bitangents.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(32) fmcube [4/binormal] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !CompareArray( bitangents.get(), s_fmCubeBiTangents, 24 ) )
        {
            printe("ERROR: ComputeTangentFrame(32) fmcube [4/binormal] failed compare of bitangents\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &bitangents[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeBiTangents[j] );
                print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        memset( tangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        hr = ComputeTangentFrame( g_fmCubeIndices32, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  tangents.get(), nullptr );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(32) fmcube [tangent] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if ( !CompareArray( tangents.get(), s_fmCubeTangents, 24 ) )
        {
            printe("ERROR: ComputeTangentFrame(32) fmcube [tangent] failed compare of tangents\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &tangents[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeTangents[j] );
                print( "%zu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = ComputeTangentFrame( g_fmCubeIndices32, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, D3D11_32BIT_INDEX_STRIP_CUT_VALUE,
                                  tangents.get(), bitangents.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ComputeTangentFrame(32) expected failure for strip cut value (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ComputeTangentFrame( g_fmCubeIndices32, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, UINT32_MAX,
                                  tangents.get(), bitangents.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ComputeTangentFrame(32) expected failure for 32-max value verts (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ComputeTangentFrame( g_fmCubeIndices32, UINT32_MAX, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  tangents.get(), bitangents.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: ComputeTangentFrame(32) expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ComputeTangentFrame( g_fmCubeIndices32, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 2,
                                  tangents.get(), bitangents.get() );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: ComputeTangentFrame(32) expected failure for bad vert count (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ComputeTangentFrame( g_fmCubeIndices32, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  static_cast<XMFLOAT3*>(nullptr), nullptr );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ComputeTangentFrame(32) [3] expected failure for both nullptr outputs (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }

        hr = ComputeTangentFrame( g_fmCubeIndices32, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                  static_cast<XMFLOAT4*>(nullptr), nullptr );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ComputeTangentFrame(32) [4] expected failure for both nullptr outputs (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        #pragma warning(pop)
    }

    // Unused (16)
    {
        static const uint16_t s_unused[12 * 3] =
        {
            3,1,0,
            2,1,3,

            6,4,5,
            7,4,6,

            11,9,8,
            10,9,11,

            uint16_t(-1),uint16_t(-1),uint16_t(-1),
            15,12,14,

            19,17,16,
            18,17,19,

            22,20,21,
            23,20,22
        };

        static const uint16_t s_unused_1st[12 * 3] =
        {
            uint16_t(-1), uint16_t(-1), uint16_t(-1),
            2,1,3,

            6,4,5,
            7,4,6,

            11,9,8,
            10,9,11,

            14,12,13,
            15,12,14,

            19,17,16,
            18,17,19,

            22,20,21,
            23,20,22
        };

        static const uint16_t s_unused_all[12 * 3] =
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

#ifdef _DEBUG
        std::wstring msgs;
        if (FAILED(Validate(s_unused, 12, 24, nullptr, VALIDATE_UNUSED, &msgs)))
        {
            success = false;
            printe("\nERROR: ComputeTangentFrame(16) test data failed validation:\n%S\n", msgs.c_str());
        }

        if (FAILED(Validate(s_unused_1st, 12, 24, nullptr, VALIDATE_UNUSED, &msgs)))
        {
            success = false;
            printe("\nERROR: ComputeTangentFrame(16) test data 1st failed validation:\n%S\n", msgs.c_str());
        }

        if (FAILED(Validate(s_unused_all, 12, 24, nullptr, VALIDATE_UNUSED, &msgs)))
        {
            success = false;
            printe("\nERROR: ComputeTangentFrame(16) test data all failed validation:\n%S\n", msgs.c_str());
        }
#endif

        std::unique_ptr<XMFLOAT3[]> tangents(new XMFLOAT3[24]);
        std::unique_ptr<XMFLOAT3[]> bitangents(new XMFLOAT3[24]);

        memset(tangents.get(), 0xff, sizeof(XMFLOAT3) * 24);
        memset(bitangents.get(), 0xff, sizeof(XMFLOAT3) * 24);

        HRESULT hr = ComputeTangentFrame(s_unused, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
            tangents.get(), bitangents.get());
        if (FAILED(hr))
        {
            printe("ERROR: ComputeTangentFrame(16) unused failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!IsValidCoVector(24, tangents.get()))
        {
            printe("ERROR: ComputeTangentFrame(16) unused tangents invalid\n");
            success = false;
        }
        else if (!IsValidCoVector(24, bitangents.get()))
        {
            printe("ERROR: ComputeTangentFrame(16) unused bi-tangents invalid\n");
            success = false;
        }

        hr = ComputeTangentFrame(s_unused_1st, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
            tangents.get(), bitangents.get());
        if (FAILED(hr))
        {
            printe("ERROR: ComputeTangentFrame(16) unused 1st failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!IsValidCoVector(24, tangents.get()))
        {
            printe("ERROR: ComputeTangentFrame(16) unused 1st tangents invalid\n");
            success = false;
        }
        else if (!IsValidCoVector(24, bitangents.get()))
        {
            printe("ERROR: ComputeTangentFrame(16) unused 1st bi-tangents invalid\n");
            success = false;
        }

        hr = ComputeTangentFrame(s_unused_all, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
            tangents.get(), bitangents.get());
        if (FAILED(hr))
        {
            printe("ERROR: ComputeTangentFrame(16) unused all failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!IsValidCoVector(24, tangents.get()))
        {
            printe("ERROR: ComputeTangentFrame(16) unused all tangents invalid\n");
            success = false;
        }
        else if (!IsValidCoVector(24, bitangents.get()))
        {
            printe("ERROR: ComputeTangentFrame(16) unused all bi-tangents invalid\n");
            success = false;
        }
    }

    // Unused (32)
    {
        static const uint32_t s_unused[ 12*3 ] =
            {
                3,1,0,
                2,1,3,

                6,4,5,
                7,4,6,

                11,9,8,
                10,9,11,

                uint32_t(-1),uint32_t(-1),uint32_t(-1),
                15,12,14,

                19,17,16,
                18,17,19,

                22,20,21,
                23,20,22
            };

        static const uint32_t s_unused_1st[12 * 3] =
        {
            uint32_t(-1), uint32_t(-1), uint32_t(-1),
            2,1,3,

            6,4,5,
            7,4,6,

            11,9,8,
            10,9,11,

            14,12,13,
            15,12,14,

            19,17,16,
            18,17,19,

            22,20,21,
            23,20,22
        };

        static const uint32_t s_unused_all[12 * 3] =
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
#ifdef _DEBUG
        std::wstring msgs;
        if ( FAILED( Validate( s_unused, 12, 24, nullptr, VALIDATE_UNUSED, &msgs ) ) )
        {
            success = false;
            printe("\nERROR: ComputeTangentFrame(32) test data failed validation:\n%S\n", msgs.c_str() );
        }

        if (FAILED(Validate(s_unused_1st, 12, 24, nullptr, VALIDATE_UNUSED, &msgs)))
        {
            success = false;
            printe("\nERROR: ComputeTangentFrame(32 test data 1st failed validation:\n%S\n", msgs.c_str());
        }

        if (FAILED(Validate(s_unused_all, 12, 24, nullptr, VALIDATE_UNUSED, &msgs)))
        {
            success = false;
            printe("\nERROR: ComputeTangentFrame(32) test data all failed validation:\n%S\n", msgs.c_str());
        }
#endif

        std::unique_ptr<XMFLOAT3[]> tangents( new XMFLOAT3[ 24 ] );
        std::unique_ptr<XMFLOAT3[]> bitangents( new XMFLOAT3[ 24 ] );

        memset( tangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );
        memset( bitangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        HRESULT hr = ComputeTangentFrame( s_unused, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
                                          tangents.get(), bitangents.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(32) unused failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else if (!IsValidCoVector(24, tangents.get()))
        {
            printe("ERROR: ComputeTangentFrame(32) unused tangents invalid\n");
            success = false;
        }
        else if (!IsValidCoVector(24, bitangents.get()))
        {
            printe("ERROR: ComputeTangentFrame(32) unused bi-tangents invalid\n");
            success = false;
        }

        hr = ComputeTangentFrame(s_unused_1st, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
            tangents.get(), bitangents.get());
        if (FAILED(hr))
        {
            printe("ERROR: ComputeTangentFrame(32) unused 1st failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!IsValidCoVector(24, tangents.get()))
        {
            printe("ERROR: ComputeTangentFrame(32) unused 1st tangents invalid\n");
            success = false;
        }
        else if (!IsValidCoVector(24, bitangents.get()))
        {
            printe("ERROR: ComputeTangentFrame(32) unused 1st bi-tangents invalid\n");
            success = false;
        }

        hr = ComputeTangentFrame(s_unused_all, 12, g_fmCubeVerts, s_fmCubeNormals, g_fmCubeUVs, 24,
            tangents.get(), bitangents.get());
        if (FAILED(hr))
        {
            printe("ERROR: ComputeTangentFrame(32) unused all failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!IsValidCoVector(24, tangents.get()))
        {
            printe("ERROR: ComputeTangentFrame(32) unused all tangents invalid\n");
            success = false;
        }
        else if (!IsValidCoVector(24, bitangents.get()))
        {
            printe("ERROR: ComputeTangentFrame(32) unused all bi-tangents invalid\n");
            success = false;
        }
    }

    // degenerate case (zero UVs)
    {
        std::unique_ptr<XMFLOAT3[]> tangents( new XMFLOAT3[ 24 ] );
        std::unique_ptr<XMFLOAT3[]> bitangents( new XMFLOAT3[ 24 ] );

        std::unique_ptr<XMFLOAT2[]> uvs( new XMFLOAT2[ 24 ] );
        memset( uvs.get(), 0, sizeof(XMFLOAT2) * 24 );

        memset( tangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );
        memset( bitangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        HRESULT hr = ComputeTangentFrame( g_fmCubeIndices16, 12, g_fmCubeVerts, s_fmCubeNormals, uvs.get(), 24,
                                          tangents.get(), bitangents.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(16) [degenerate zero UV] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            bool pass = true;
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR t = XMLoadFloat3( &tangents[ j] );
                XMVECTOR b = XMLoadFloat3( &bitangents[ j] );

                XMVECTOR lent = XMVector3Length( t );
                XMVECTOR lenb = XMVector3Length( b );
                if ( XMVector3Less( lent, g_MeshEpsilon ) || XMVector3Less( lenb, g_MeshEpsilon ) )
                {
                    pass = false;
                }
            }

            if ( !pass )
            {
                printe("ERROR: ComputeTangentFrame(16) [degenerate zero UV] failed\n" );
                success = false;
            }
        }
    }

    // degenerate case (zero Us)
    {
        static const XMFLOAT2 s_uvs[ 24 ] = 
            {
                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 1.0f ),
                DirectX::XMFLOAT2( 0.0f, 1.0f ),

                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 1.0f ),
                DirectX::XMFLOAT2( 0.0f, 1.0f ),

                DirectX::XMFLOAT2( 0.0f, 1.0f ),
                DirectX::XMFLOAT2( 0.0f, 1.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),

                DirectX::XMFLOAT2( 0.0f, 1.0f ),
                DirectX::XMFLOAT2( 0.0f, 1.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),

                DirectX::XMFLOAT2( 0.0f, 1.0f ),
                DirectX::XMFLOAT2( 0.0f, 1.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),

                DirectX::XMFLOAT2( 0.0f, 1.0f ),
                DirectX::XMFLOAT2( 0.0f, 1.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),
            };

        std::unique_ptr<XMFLOAT3[]> tangents( new XMFLOAT3[ 24 ] );
        std::unique_ptr<XMFLOAT3[]> bitangents( new XMFLOAT3[ 24 ] );

        memset( tangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );
        memset( bitangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        HRESULT hr = ComputeTangentFrame( g_fmCubeIndices16, 12, g_fmCubeVerts, s_fmCubeNormals, s_uvs, 24,
                                          tangents.get(), bitangents.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(16) [degenerate zero U] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            bool pass = true;
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR t = XMLoadFloat3( &tangents[ j] );
                XMVECTOR b = XMLoadFloat3( &bitangents[ j] );

                XMVECTOR lent = XMVector3Length( t );
                XMVECTOR lenb = XMVector3Length( b );
                if ( XMVector3Less( lent, g_MeshEpsilon ) || XMVector3Less( lenb, g_MeshEpsilon ) )
                {
                    pass = false;
                }
            }

            if ( !pass )
            {
                printe("ERROR: ComputeTangentFrame(16) [degenerate zero U] failed\n" );
                success = false;
            }
        }
    }

    // degenerate case (zero Vs)
    {
        static const XMFLOAT2 s_uvs[ 24 ] = 
            {
                DirectX::XMFLOAT2( 1.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 1.0f, 0.0f ),

                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 1.0f, 0.0f ),
                DirectX::XMFLOAT2( 1.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),

                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 1.0f, 0.0f ),
                DirectX::XMFLOAT2( 1.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),

                DirectX::XMFLOAT2( 1.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 1.0f, 0.0f ),

                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 1.0f, 0.0f ),
                DirectX::XMFLOAT2( 1.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),

                DirectX::XMFLOAT2( 1.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 0.0f, 0.0f ),
                DirectX::XMFLOAT2( 1.0f, 0.0f ),
            };

        std::unique_ptr<XMFLOAT3[]> tangents( new XMFLOAT3[ 24 ] );
        std::unique_ptr<XMFLOAT3[]> bitangents( new XMFLOAT3[ 24 ] );

        memset( tangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );
        memset( bitangents.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        HRESULT hr = ComputeTangentFrame( g_fmCubeIndices16, 12, g_fmCubeVerts, s_fmCubeNormals, s_uvs, 24,
                                          tangents.get(), bitangents.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeTangentFrame(16) [degenerate zero V] failed (%08X)\n", static_cast<unsigned int>(hr) );
            success = false;
        }
        else
        {
            bool pass = true;
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR t = XMLoadFloat3( &tangents[ j] );
                XMVECTOR b = XMLoadFloat3( &bitangents[ j] );

                XMVECTOR lent = XMVector3Length( t );
                XMVECTOR lenb = XMVector3Length( b );
                if ( XMVector3Less( lent, g_MeshEpsilon ) || XMVector3Less( lenb, g_MeshEpsilon ) )
                {
                    pass = false;
                }
            }

            if ( !pass )
            {
                printe("ERROR: ComputeTangentFrame(16) [degenerate zero V] failed\n" );
                success = false;
            }
        }
    }

    return success;
}
