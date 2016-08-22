//-------------------------------------------------------------------------------------
// normals.cpp
//  
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#include "directxtest.h"

#include "directxmesh.h"

#include "TestHelpers.h"
#include "TestGeometry.h"

using namespace DirectX;
using namespace TestGeometry;

//-------------------------------------------------------------------------------------
// ComputeNormals
bool Test11()
{
    bool success = true;

    // Cube
    static const XMFLOAT3 s_cubeNormalsAngle[8] =
        {
            XMFLOAT3( -0.577350f, -0.577350f, -0.577350f ),
            XMFLOAT3( 0.577350f, 0.577350f, -0.577350f ),
            XMFLOAT3( 0.577350f, -0.577350f, -0.577350f ),
            XMFLOAT3( -0.577350f, 0.577350f, -0.577350f ),
            XMFLOAT3( -0.577350f, 0.577350f, 0.577350f ),
            XMFLOAT3( -0.577350f, -0.577350f, 0.577350f ),
            XMFLOAT3( 0.577350f, 0.577350f, 0.577350f ),
            XMFLOAT3( 0.577350f, -0.577350f, 0.577350f ),
        };

    static const XMFLOAT3 s_cubeNormalsEqual[8] =
        {
            XMFLOAT3(  -0.577350f, -0.577350f, -0.577350f ),
            XMFLOAT3( 0.408248f, 0.408248f, -0.816497f ),
            XMFLOAT3( 0.816496f, -0.408248f, -0.408248f ),
            XMFLOAT3( -0.408248f, 0.816496f, -0.408248f ),
            XMFLOAT3( -0.816496f, 0.408248f, 0.408248f ),
            XMFLOAT3( -0.408248f, -0.408248f, 0.816497f ),
            XMFLOAT3( 0.577350f, 0.577350f, 0.577350f ),
            XMFLOAT3( 0.408248f, -0.816496f, 0.408248f ),
        };

    static const XMFLOAT3 s_cubeNormalsAngleCW[8] =
        {
            XMFLOAT3( 0.577350f, 0.577350f, 0.577350f ),
            XMFLOAT3( -0.577350f, -0.577350f, 0.577350f ),
            XMFLOAT3( -0.577350f, 0.577350f, 0.577350f ),
            XMFLOAT3( 0.577350f, -0.577350f, 0.577350f ),
            XMFLOAT3( 0.577350f, -0.577350f, -0.577350f ),
            XMFLOAT3( 0.577350f, 0.577350f, -0.577350f ),
            XMFLOAT3( -0.577350f, -0.577350f, -0.577350f ),
            XMFLOAT3( -0.577350f, 0.577350f, -0.577350f ),
        };

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

    static const XMFLOAT3 s_fmCubeNormalsCW[24] =
        {
            XMFLOAT3( 0.f, -1.f, 0.f ),
            XMFLOAT3( 0.f, -1.f, 0.f ),
            XMFLOAT3( 0.f, -1.f, 0.f ),
            XMFLOAT3( 0.f, -1.f, 0.f ),
            XMFLOAT3( 0.f, 1.f, 0.f ),
            XMFLOAT3( 0.f, 1.f, 0.f ),
            XMFLOAT3( 0.f, 1.f, 0.f ),
            XMFLOAT3( 0.f, 1.f, 0.f ),
            XMFLOAT3( 1.f, 0.f, 0.f ),
            XMFLOAT3( 1.f, 0.f, 0.f ),
            XMFLOAT3( 1.f, 0.f, 0.f ),
            XMFLOAT3( 1.f, 0.f, 0.f ),
            XMFLOAT3( -1.f, 0.f, 0.f ),
            XMFLOAT3( -1.f, 0.f, 0.f ),
            XMFLOAT3( -1.f, 0.f, 0.f ),
            XMFLOAT3( -1.f, 0.f, 0.f ),
            XMFLOAT3( 0.f, 0.f, 1.f ),
            XMFLOAT3( 0.f, 0.f, 1.f ),
            XMFLOAT3( 0.f, 0.f, 1.f ),
            XMFLOAT3( 0.f, 0.f, 1.f ),
            XMFLOAT3( 0.f, 0.f, -1.f ),
            XMFLOAT3( 0.f, 0.f, -1.f ),
            XMFLOAT3( 0.f, 0.f, -1.f ),
            XMFLOAT3( 0.f, 0.f, -1.f ),
        };

    // Box (aka cuboid or rectangular parallelepiped)
    static const XMFLOAT3 s_boxNormalsArea[8] =
        {
            XMFLOAT3( -0.218218f, -0.436436f, -0.872872f ),
            XMFLOAT3( 0.120386f, 0.240772f, -0.963087f ),
            XMFLOAT3( 0.408248f, -0.408248f, -0.816497f ),
            XMFLOAT3( -0.174078f, 0.696310f, -0.696310f ),
            XMFLOAT3( -0.408248f, 0.408248f, 0.816497f ),
            XMFLOAT3( -0.120386f, -0.240772f, 0.963087f ),
            XMFLOAT3( 0.218218f, 0.436436f, 0.872872f ),
            XMFLOAT3( 0.174078f, -0.696310f, 0.696310f ),
        };

    // Tetrahedron
    static const XMFLOAT3 s_tetraNormalsAngle[4] =
        {
            XMFLOAT3( -0.408248f, 0.408248f, -0.816497f ),
            XMFLOAT3( -0.447214f, 0.000000f, -0.894427f ),
            XMFLOAT3( 0.000000f, 0.447214f, -0.894427f ),
            XMFLOAT3( -0.707107f, 0.707107f, 0.000000f ),
        };

    static const XMFLOAT3 s_tetraNormalsEqual[4] =
        {
            XMFLOAT3( -0.408248f, 0.408248f, -0.816497f ),
            XMFLOAT3( -0.447214f, 0.000000f, -0.894427f ),
            XMFLOAT3( 0.000000f, 0.447214f, -0.894427f ),
            XMFLOAT3( -0.707107f, 0.707107f, 0.000000f ),
        };

    static const XMFLOAT3 s_tetraNormalsAngleCW[4] =
        {
            XMFLOAT3( 0.408248f, -0.408248f, 0.816497f ),
            XMFLOAT3( 0.447214f, 0.000000f, 0.894427f ),
            XMFLOAT3( 0.000000f, -0.447214f, 0.894427f ),
            XMFLOAT3( 0.707107f, -0.707107f, 0.000000f ),
        };

    // 16-bit (cube)
    {
        std::unique_ptr<XMFLOAT3[]> normals( new XMFLOAT3[ 8 ] );

        // by angle
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        HRESULT hr = ComputeNormals( g_cubeIndices16, 12, g_cubeVerts, 8, CNORM_DEFAULT, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) cube [angle] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_cubeNormalsAngle, 8 ) )
        {
            printe("ERROR: ComputeNormals(16) cube [angle] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_cubeNormalsAngle[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // equal weight
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( g_cubeIndices16, 12, g_cubeVerts, 8, CNORM_WEIGHT_EQUAL, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) cube [equal] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_cubeNormalsEqual, 8 ) )
        {
            printe("ERROR: ComputeNormals(16) cube [equal] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_cubeNormalsEqual[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // by area (for uniform cube, same as equal weight)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( g_cubeIndices16, 12, g_cubeVerts, 8, CNORM_WEIGHT_BY_AREA, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) cube [area] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_cubeNormalsEqual, 8 ) )
        {
            printe("ERROR: ComputeNormals(16) cube [area] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_cubeNormalsEqual[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // wind cw flag
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( g_cubeIndices16, 12, g_cubeVerts, 8, CNORM_WIND_CW, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) cube [cw] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_cubeNormalsAngleCW, 8 ) )
        {
            printe("ERROR: ComputeNormals(16) cube [cw] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_cubeNormalsAngleCW[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = ComputeNormals( g_cubeIndices16, 12, g_cubeVerts, D3D11_16BIT_INDEX_STRIP_CUT_VALUE, CNORM_DEFAULT, normals.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ComputeNormals(16) expected failure for strip cut value (%08X)\n", hr );
            success = false;
        }

        hr = ComputeNormals( g_cubeIndices16, 12, g_cubeVerts, UINT32_MAX, CNORM_DEFAULT, normals.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ComputeNormals(16) expected failure for 32-max value verts (%08X)\n", hr );
            success = false;
        }

        hr = ComputeNormals( g_cubeIndices16, UINT32_MAX, g_cubeVerts, 8, CNORM_DEFAULT, normals.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: ComputeNormals(16) expected failure for 32-max value faces (%08X)\n", hr );
            success = false;
        }

        hr = ComputeNormals( g_cubeIndices16, 12, g_cubeVerts, 2, CNORM_DEFAULT, normals.get() );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: ComputeNormals(16) expected failure for bad vert count (%08X)\n", hr );
            success = false;
        }
        #pragma warning(pop)
    }

    // 16-bit (face-mapped cube)
    {
        std::unique_ptr<XMFLOAT3[]> normals( new XMFLOAT3[ 24 ] );

        // by angle (for face-mapped cube, results in face normals)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        HRESULT hr = ComputeNormals( g_fmCubeIndices16, 12, g_fmCubeVerts, 24, CNORM_DEFAULT, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) fmcube [angle] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_fmCubeNormals, 24 ) )
        {
            printe("ERROR: ComputeNormals(16) fmcube [angle] failed compare of results\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeNormals[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // equal weight (for face-mapped cube, results in face normals)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        hr = ComputeNormals( g_fmCubeIndices16, 12, g_fmCubeVerts, 24, CNORM_WEIGHT_EQUAL, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) fmcube [equal] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_fmCubeNormals, 24 ) )
        {
            printe("ERROR: ComputeNormals(16) fmcube [equal] failed compare of results\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeNormals[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // by area (for face-mapped cube, results in face normals)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        hr = ComputeNormals( g_fmCubeIndices16, 12, g_fmCubeVerts, 24, CNORM_WEIGHT_BY_AREA, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) fmcube [area] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_fmCubeNormals, 24 ) )
        {
            printe("ERROR: ComputeNormals(16) fmcube [area] failed compare of results\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeNormals[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // wind cw flag (for face-mapped cube, results in face normals)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        hr = ComputeNormals( g_fmCubeIndices16, 12, g_fmCubeVerts, 24, CNORM_WIND_CW, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) fmcube [cw] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_fmCubeNormalsCW, 24 ) )
        {
            printe("ERROR: ComputeNormals(16) fmcube [cw] failed compare of results\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeNormalsCW[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }
    }

    // 16-bit (box)
    {
        std::unique_ptr<XMFLOAT3[]> normals( new XMFLOAT3[ 8 ] );

        // by angle (same as cube)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        HRESULT hr = ComputeNormals( g_boxIndices16, 12, g_boxVerts, 8, CNORM_DEFAULT, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) box [angle] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_cubeNormalsAngle, 8 ) )
        {
            printe("ERROR: ComputeNormals(16) box [angle] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_cubeNormalsAngle[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // equal weight (same as cube)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( g_boxIndices16, 12, g_boxVerts, 8, CNORM_WEIGHT_EQUAL, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) box [equal] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_cubeNormalsEqual, 8 ) )
        {
            printe("ERROR: ComputeNormals(16) box [equal] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_cubeNormalsEqual[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // by area
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( g_boxIndices16, 12, g_boxVerts, 8, CNORM_WEIGHT_BY_AREA, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) box [area] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_boxNormalsArea, 8 ) )
        {
            printe("ERROR: ComputeNormals(16) box [area] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_boxNormalsArea[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // wind cw flag (same as cube)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( g_boxIndices16, 12, g_boxVerts, 8, CNORM_WIND_CW, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) box [cw] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_cubeNormalsAngleCW, 8 ) )
        {
            printe("ERROR: ComputeNormals(16) box [cw] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_cubeNormalsAngleCW[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }
    }

    // 16-bit (tetra)
    {
        std::unique_ptr<XMFLOAT3[]> normals( new XMFLOAT3[ 4 ] );

        // by angle
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 4 );

        HRESULT hr = ComputeNormals( g_tetraIndices16, 4, g_tetraVerts, 4, CNORM_DEFAULT, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) tetra [angle] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_tetraNormalsAngle, 4 ) )
        {
            printe("ERROR: ComputeNormals(16) tetra [angle] failed compare of results\n" );
            for( size_t j = 0; j < 4; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_tetraNormalsAngle[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // equal weight
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 4 );

        hr = ComputeNormals( g_tetraIndices16, 4, g_tetraVerts, 4, CNORM_WEIGHT_EQUAL, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) tetra [equal] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_tetraNormalsEqual, 4 ) )
        {
            printe("ERROR: ComputeNormals(16) tetra [equal] failed compare of results\n" );
            for( size_t j = 0; j < 4; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_tetraNormalsEqual[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // by area (for uniform tetrahedron, same as equal weight)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 4 );

        hr = ComputeNormals( g_tetraIndices16, 4, g_tetraVerts, 4, CNORM_WEIGHT_BY_AREA, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) tetra [area] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_tetraNormalsEqual, 4 ) )
        {
            printe("ERROR: ComputeNormals(16) tetra [area] failed compare of results\n" );
            for( size_t j = 0; j < 4; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_tetraNormalsEqual[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // wind cw flag
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 4 );

        hr = ComputeNormals( g_tetraIndices16, 4, g_tetraVerts, 4, CNORM_WIND_CW, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) tetra [cw] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_tetraNormalsAngleCW, 4 ) )
        {
            printe("ERROR: ComputeNormals(16) tetra [cw] failed compare of results\n" );
            for( size_t j = 0; j < 4; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_tetraNormalsAngleCW[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }
    }

    // 32-bit (cube)
    {
        std::unique_ptr<XMFLOAT3[]> normals( new XMFLOAT3[ 8 ] );

        // by angle
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        HRESULT hr = ComputeNormals( g_cubeIndices32, 12, g_cubeVerts, 8, CNORM_DEFAULT, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) cube failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_cubeNormalsAngle, 8 ) )
        {
            printe("ERROR: ComputeNormals(32) cube [angle] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_cubeNormalsAngle[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // equal weight
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( g_cubeIndices32, 12, g_cubeVerts, 8, CNORM_WEIGHT_EQUAL, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) cube [equal] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_cubeNormalsEqual, 8 ) )
        {
            printe("ERROR: ComputeNormals(32) cube [equal] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_cubeNormalsEqual[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // by area (for uniform cube, same as equal weight)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( g_cubeIndices32, 12, g_cubeVerts, 8, CNORM_WEIGHT_BY_AREA, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) cube [area] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_cubeNormalsEqual, 8 ) )
        {
            printe("ERROR: ComputeNormals(32) cube [area] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_cubeNormalsEqual[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // wind cw flag
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( g_cubeIndices32, 12, g_cubeVerts, 8, CNORM_WIND_CW, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) cube [cw] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_cubeNormalsAngleCW, 8 ) )
        {
            printe("ERROR: ComputeNormals(32) cube [cw] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_cubeNormalsAngleCW[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385)
        hr = ComputeNormals( g_cubeIndices32, 12, g_cubeVerts, D3D11_32BIT_INDEX_STRIP_CUT_VALUE, CNORM_DEFAULT, normals.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ComputeNormals(32) expected failure for strip cut value (%08X)\n", hr );
            success = false;
        }

        hr = ComputeNormals( g_cubeIndices32, 12, g_cubeVerts, UINT32_MAX, CNORM_DEFAULT, normals.get() );
        if ( hr != E_INVALIDARG )
        {
            printe("\nERROR: ComputeNormals(32) expected failure for 32-max value verts (%08X)\n", hr );
            success = false;
        }

        hr = ComputeNormals( g_cubeIndices32, UINT32_MAX, g_cubeVerts, 8, CNORM_DEFAULT, normals.get() );
        if ( hr != HRESULT_FROM_WIN32( ERROR_ARITHMETIC_OVERFLOW ) )
        {
            printe("\nERROR: ComputeNormals(32) expected failure for 32-max value faces (%08X)\n", hr );
            success = false;
        }

        hr = ComputeNormals( g_cubeIndices32, 12, g_cubeVerts, 2, CNORM_DEFAULT, normals.get() );
        if ( hr != E_UNEXPECTED )
        {
            printe("\nERROR: ComputeNormals(32) expected failure for bad vert count (%08X)\n", hr );
            success = false;
        }
        #pragma warning(pop)
    }

    // 32-bit (face-mapped cube)
    {
        std::unique_ptr<XMFLOAT3[]> normals( new XMFLOAT3[ 24 ] );

        // by angle (for face-mapped cube, results in face normals)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        HRESULT hr = ComputeNormals( g_fmCubeIndices32, 12, g_fmCubeVerts, 24, CNORM_DEFAULT, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) fmcube [angle] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_fmCubeNormals, 24 ) )
        {
            printe("ERROR: ComputeNormals(32) fmcube [angle] failed compare of results\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeNormals[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // equal weight (for face-mapped cube, results in face normals)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        hr = ComputeNormals( g_fmCubeIndices32, 12, g_fmCubeVerts, 24, CNORM_WEIGHT_EQUAL, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) fmcube [equal] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_fmCubeNormals, 24 ) )
        {
            printe("ERROR: ComputeNormals(32) fmcube [equal] failed compare of results\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeNormals[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // by area (for face-mapped cube, results in face normals)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        hr = ComputeNormals( g_fmCubeIndices32, 12, g_fmCubeVerts, 24, CNORM_WEIGHT_BY_AREA, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) fmcube [area] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_fmCubeNormals, 24 ) )
        {
            printe("ERROR: ComputeNormals(32) fmcube [area] failed compare of results\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeNormals[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // wind cw flag (for face-mapped cube, results in face normals)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 24 );

        hr = ComputeNormals( g_fmCubeIndices32, 12, g_fmCubeVerts, 24, CNORM_WIND_CW, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) fmcube [cw] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_fmCubeNormalsCW, 24 ) )
        {
            printe("ERROR: ComputeNormals(32) fmcube [cw] failed compare of results\n" );
            for( size_t j = 0; j < 24; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_fmCubeNormalsCW[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }
    }

    // 32-bit (box)
    {
        std::unique_ptr<XMFLOAT3[]> normals( new XMFLOAT3[ 8 ] );

        // by angle (same as cube)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        HRESULT hr = ComputeNormals( g_boxIndices32, 12, g_boxVerts, 8, CNORM_DEFAULT, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) box failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_cubeNormalsAngle, 8 ) )
        {
            printe("ERROR: ComputeNormals(32) box [angle] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_cubeNormalsAngle[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // equal weight (same as cube)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( g_boxIndices32, 12, g_boxVerts, 8, CNORM_WEIGHT_EQUAL, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) box [equal] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_cubeNormalsEqual, 8 ) )
        {
            printe("ERROR: ComputeNormals(32) box [equal] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_cubeNormalsEqual[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // by area
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( g_boxIndices32, 12, g_boxVerts, 8, CNORM_WEIGHT_BY_AREA, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) box [area] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_boxNormalsArea, 8 ) )
        {
            printe("ERROR: ComputeNormals(32) box [area] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_boxNormalsArea[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // wind cw flag (same as cube)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( g_boxIndices32, 12, g_boxVerts, 8, CNORM_WIND_CW, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) box [cw] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_cubeNormalsAngleCW, 8 ) )
        {
            printe("ERROR: ComputeNormals(32) box [cw] failed compare of results\n" );
            for( size_t j = 0; j < 8; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_cubeNormalsAngleCW[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }
    }

    // 32-bit (tetra)
    {
        std::unique_ptr<XMFLOAT3[]> normals( new XMFLOAT3[ 4 ] );

        // by angle
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 4 );

        HRESULT hr = ComputeNormals( g_tetraIndices32, 4, g_tetraVerts, 4, CNORM_DEFAULT, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) tetra [angle] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_tetraNormalsAngle, 4 ) )
        {
            printe("ERROR: ComputeNormals(32) tetra [angle] failed compare of results\n" );
            for( size_t j = 0; j < 4; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_tetraNormalsAngle[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // equal weight
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 4 );

        hr = ComputeNormals( g_tetraIndices32, 4, g_tetraVerts, 4, CNORM_WEIGHT_EQUAL, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) tetra [equal] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_tetraNormalsEqual, 4 ) )
        {
            printe("ERROR: ComputeNormals(32) tetra [equal] failed compare of results\n" );
            for( size_t j = 0; j < 4; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_tetraNormalsEqual[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // by area (for uniform tetrahedron, same as equal weight)
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 4 );

        hr = ComputeNormals( g_tetraIndices32, 4, g_tetraVerts, 4, CNORM_WEIGHT_BY_AREA, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) tetra [area] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_tetraNormalsEqual, 4 ) )
        {
            printe("ERROR: ComputeNormals(32) tetra [area] failed compare of results\n" );
            for( size_t j = 0; j < 4; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_tetraNormalsEqual[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }

        // wind cw flag
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 4 );

        hr = ComputeNormals( g_tetraIndices32, 4, g_tetraVerts, 4, CNORM_WIND_CW, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) tetra [cw] failed (%08X)\n", hr );
            success = false;
        }
        else if ( !CompareArray( normals.get(), s_tetraNormalsAngleCW, 4 ) )
        {
            printe("ERROR: ComputeNormals(32) tetra [cw] failed compare of results\n" );
            for( size_t j = 0; j < 4; ++j )
            {
                XMVECTOR v = XMLoadFloat3( &normals[j] );
                XMVECTOR r = XMLoadFloat3( &s_tetraNormalsAngleCW[j] );
                print( "%Iu: %f %f %f ... %f %f %f\n", j, XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r) );
            }
            success = false;
        }
    }

    // Unused (16)
    {
        std::unique_ptr<XMFLOAT3[]> normals( new XMFLOAT3[ 8 ] );

        static const uint16_t s_unused[ 12*3 ] =
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
        if ( FAILED( Validate( s_unused, 12, 8, nullptr, VALIDATE_UNUSED, &msgs ) ) )
        {
            success = false;            
            printe("\nERROR: ComputeNormals(16) test data failed validation:\n%S\n", msgs.c_str() );
        }
#endif

        // by angle
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        HRESULT hr = ComputeNormals( s_unused, 12, g_cubeVerts, 8, CNORM_DEFAULT, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) unused [angle] failed (%08X)\n", hr );
            success = false;
        }

        // equal weight
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( s_unused, 12, g_cubeVerts, 8, CNORM_WEIGHT_EQUAL, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) unused [equal] failed (%08X)\n", hr );
            success = false;
        }

        // by area
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( s_unused, 12, g_cubeVerts, 8, CNORM_WEIGHT_BY_AREA, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(16) unused [area] failed (%08X)\n", hr );
            success = false;
        }
    }

    // Unused (32)
    {
        std::unique_ptr<XMFLOAT3[]> normals( new XMFLOAT3[ 8 ] );

        static const uint32_t s_unused[ 12*3 ] =
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
        if ( FAILED( Validate( s_unused, 12, 8, nullptr, VALIDATE_UNUSED, &msgs ) ) )
        {
            success = false;            
            printe("\nERROR: ComputeNormals(32) test data failed validation:\n%S\n", msgs.c_str() );
        }
#endif

        // by angle
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        HRESULT hr = ComputeNormals( s_unused, 12, g_cubeVerts, 8, CNORM_DEFAULT, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) unused [angle] failed (%08X)\n", hr );
            success = false;
        }

        // equal weight
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( s_unused, 12, g_cubeVerts, 8, CNORM_WEIGHT_EQUAL, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) unused [equal] failed (%08X)\n", hr );
            success = false;
        }

        // by area
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * 8 );

        hr = ComputeNormals( s_unused, 12, g_cubeVerts, 8, CNORM_WEIGHT_BY_AREA, normals.get() );
        if ( FAILED(hr) )
        {
            printe("ERROR: ComputeNormals(32) unused [area] failed (%08X)\n", hr );
            success = false;
        }
    }

    return success;
}
