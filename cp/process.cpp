//-------------------------------------------------------------------------------------
// process.cpp
//  
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#include "directxtest.h"

#include "directxmesh.h"

#include "TestHelpers.h"
#include "WaveFrontReader.h"

using namespace DirectX;

static const float g_Epsilon = 0.0001f;

struct TestMedia
{
    const wchar_t * fname;
    float           stripOrderACMR;
    float           stripOrderATVR;
    float           independentACMR;
    float           independentATVR;
    float           attrStripOrderACMR;
    float           attrStripOrderATVR;
    float           attrIndependentACMR;
    float           attrIndependentATVR;
    size_t          bowtieDups;
    size_t          attrDups;
};

static const TestMedia g_TestMedia[] =
{
    // filename                             | striporder           | independent          | attr-striporder      | attr-independent     | dups
    { MEDIA_PATH L"cup._obj",                 1.050532f, 1.473881f,  0.761170f, 1.067910f,  1.268085f, 1.779104f,  1.002128f, 1.405970f,  0,  0   },
    { MEDIA_PATH L"teapot._obj",              1.013672f, 1.958491f,  0.646484f, 1.249057f,  1.013672f, 1.958491f,  0.646484f, 1.249057f,  0,  0   },
    { MEDIA_PATH L"SuperSimpleRunner._obj",   1.660000f, 1.f,        1.660000f, 1.f,        1.660000f, 1.f,        1.660000f, 1.f,        0,  0   },
    { MEDIA_PATH L"shuttle._obj",             0.878247f, 1.745161f,  0.696429f, 1.383871f,  0.870130f, 1.285372f,  0.798701f, 1.179856f,  0,  107 },
    { MEDIA_PATH L"player_ship_a._obj",       1.102867f, 1.053140f,  1.091062f, 1.041868f,  1.102867f, 1.053140f,  1.091062f, 1.041868f,  0,  0   },
    { MEDIA_PATH L"FSEngineGeo._obj",         1.689465f, 1.013975f,  1.676662f, 1.006291f,  1.689465f, 1.013975f,  1.676662f, 1.006291f,  10, 0   },
    { MEDIA_PATH L"sphere.vbo",               1.001894f, 1.885918f,  0.630682f, 1.187166f,  1.001894f, 1.885918f,  0.630682f, 1.187166f,  0,  0   },
    { MEDIA_PATH L"cylinder.vbo",             1.079365f, 1.046154f,  1.079365f, 1.046154f,  1.079365f, 1.046154f,  1.079365f, 1.046154f,  0,  0   },
    { MEDIA_PATH L"torus.vbo",                1.000918f, 2.001837f,  0.642332f, 1.284665f,  1.000918f, 2.001837f,  0.642332f, 1.284665f,  0,  0   },
    { MEDIA_PATH L"Head_Big_Ears.vbo",        0.815828f, 1.604267f,  0.664554f, 1.306799f,  0.815828f, 1.604267f,  0.664554f, 1.306799f,  0,  0   },
};

//-------------------------------------------------------------------------------------
// MeshBuild
bool Test01()
{
    bool success = true;

    size_t ncount = 0;
    size_t npass = 0;

    for( size_t index=0; index < _countof(g_TestMedia); ++index )
    {
        WCHAR szPath[MAX_PATH];
        ExpandEnvironmentStringsW( g_TestMedia[index].fname, szPath, MAX_PATH );

#ifdef DEBUG
        OutputDebugStringW(szPath);
        OutputDebugStringA("\n");
#endif

        WCHAR ext[_MAX_EXT];
        _wsplitpath_s( szPath, nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT );

        ++ncount;

        std::unique_ptr<WaveFrontReader<uint16_t>> mesh( new WaveFrontReader<uint16_t>() );

        print( "." );

        HRESULT hr;
        if ( _wcsicmp( ext, L".vbo" ) == 0 )
        {
            hr = mesh->LoadVBO( szPath );
        }
        else
        {
            hr = mesh->Load( szPath );
        }

        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed loading mesh data (%08X):\n%S\n", hr, szPath );
            continue;
        }

        size_t nFaces = mesh->indices.size() / 3;
        size_t nVerts = mesh->vertices.size();

#ifdef DEBUG
        char output[ 256 ] = { 0 };
        sprintf_s( output, "INFO: %Iu verts, %Iu faces\n", nVerts, nFaces );
        OutputDebugStringA( output );
#endif

        std::wstring msgs;
        hr = Validate( &mesh->indices.front(), nFaces, nVerts, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed Validate mesh data (%08X):\n%S\n%S\n", hr, szPath, msgs.c_str() );
            continue;
        }

#ifdef DEBUG
        hr = Validate(&mesh->indices.front(), nFaces, nVerts, nullptr, VALIDATE_DEGENERATE, &msgs );
        if ( FAILED(hr) )
        {
            OutputDebugStringW( msgs.c_str() );
        }
#endif

        std::unique_ptr<XMFLOAT3[]> pos( new XMFLOAT3[ nVerts ] );
        for( size_t j = 0; j < nVerts; ++j )
            pos[ j ] = mesh->vertices[ j ].position;

#if _DEBUG
        std::unique_ptr<uint32_t[]> preps( new uint32_t[ nVerts ] );
        memset( preps.get(), 0xff, sizeof(uint32_t) * nVerts );

        hr = GenerateAdjacencyAndPointReps( &mesh->indices.front(), nFaces, pos.get(), nVerts, 0.f, preps.get(), nullptr );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: failed GenerateAdjacencyAndPointReps [preps] (%08X)\n:%S", hr, szPath );
            continue;
        }
        else if ( !IsValidPointReps( preps.get(), nVerts ) )
        {
            printe("\nERROR: GenerateAdjacencyAndPointReps [preps] invalid pointRep\n" );
            success = false;
            continue;
        }

#if 0
printf("POINTREP:\n");
for( size_t j = 0; j < nVerts; ++j )
{
    printf("%Iu -> %u\n", j, preps[j] );
}
#endif

#endif

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ mesh->indices.size() ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) *  mesh->indices.size() );

        hr = GenerateAdjacencyAndPointReps( &mesh->indices.front(), nFaces, pos.get(), nVerts, 0.f, nullptr, adj.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: failed GenerateAdjacencyAndPointReps (%08X)\n:%S", hr, szPath );
            continue;
        }

#if 0
printf("ADJ:\n");
for( size_t j = 0; j < nFaces; ++j )
{
    printf("%Iu -> %u %u %u\n", j, adj[j * 3], adj[j * 3 + 1], adj[j * 3 + 2] );
}
#endif

        hr = Validate( &mesh->indices.front(), nFaces, nVerts, adj.get(), VALIDATE_DEFAULT | VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed Validate [adjacency] mesh data (%08X):\n%S\n%S\n", hr, szPath, msgs.c_str() );
            continue;
        }

        std::unique_ptr<XMFLOAT3[]> normals( new XMFLOAT3[ nVerts ] );
        memset( normals.get(), 0xff, sizeof(XMFLOAT3) * nVerts );

        hr = ComputeNormals( &mesh->indices.front(), nFaces, pos.get(), nVerts, CNORM_DEFAULT, normals.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed ComputeNormals (%08X):\n%S\n", hr, szPath );
            continue;
        }
        else
        {
            bool pass = true;
            for( size_t j = 0; j < nFaces; j += 3 )
            {
                uint16_t i0 = mesh->indices[ j ];
                uint16_t i1 = mesh->indices[ j+1 ];
                uint16_t i2 = mesh->indices[ j+2 ];

                if ( i0 == uint16_t(-1)
                     || i1 == uint16_t(-1)
                     || i2 == uint16_t(-1) )
                     continue;

                // ignore degenerate faces
                if ( i0 == i1
                     || i0 == i2
                     || i1 == i2 )
                     continue;

                XMVECTOR n = XMLoadFloat3( &normals[ i0 ] );
                XMVECTOR len = XMVector3Length( n );
                if ( XMVector3Less( len, g_MeshEpsilon ) )
                {
                    print( "%u: %f %f %f\n", i0, XMVectorGetX(n), XMVectorGetY(n), XMVectorGetZ(n) );
                    pass = false;
                }

                n = XMLoadFloat3( &normals[ i1 ] );
                len = XMVector3Length( n );
                if ( XMVector3Less( len, g_MeshEpsilon ) )
                {
                    print( "%u: %f %f %f\n", i1, XMVectorGetX(n), XMVectorGetY(n), XMVectorGetZ(n) );
                    pass = false;
                }

                n = XMLoadFloat3( &normals[ i2 ] );
                len = XMVector3Length( n );
                if ( XMVector3Less( len, g_MeshEpsilon ) )
                {
                    print( "%u: %f %f %f\n", i2, XMVectorGetX(n), XMVectorGetY(n), XMVectorGetZ(n) );
                    pass = false;
                }
            }

            if ( !pass )
            {
                printe("ERROR: ComputeNormals failed to provide non-zero length normal vectors:\n%S\n", szPath );
                success = false;
            }
        }

        if ( mesh->hasTexcoords )
        {
            std::unique_ptr<XMFLOAT3[]> tangents( new XMFLOAT3[ nVerts ] );
            std::unique_ptr<XMFLOAT3[]> bitangents( new XMFLOAT3[ nVerts ] );

            std::unique_ptr<XMFLOAT2[]> texcoords( new XMFLOAT2[ nVerts ] );
            for( size_t j = 0; j < nVerts; ++j )
                texcoords[ j ] = mesh->vertices[ j ].textureCoordinate;

            hr = ComputeTangentFrame( &mesh->indices.front(), nFaces, pos.get(), normals.get(), texcoords.get(), nVerts,
                                      tangents.get(), bitangents.get() );
            if ( FAILED(hr) )
            {
                success = false;
                printe( "ERROR: Failed ComputeTangentFrame (%08X):\n%S\n", hr, szPath );
                continue;
            }
            else
            {
                // Tangents
                bool pass = true;
                for( size_t j = 0; j < nFaces; j += 3 )
                {
                    uint16_t i0 = mesh->indices[ j ];
                    uint16_t i1 = mesh->indices[ j+1 ];
                    uint16_t i2 = mesh->indices[ j+2 ];

                    if ( i0 == uint16_t(-1)
                         || i1 == uint16_t(-1)
                         || i2 == uint16_t(-1) )
                         continue;

                    // ignore degenerate faces
                    if ( i0 == i1
                         || i0 == i2
                         || i1 == i2 )
                         continue;

                    XMVECTOR t = XMLoadFloat3( &tangents[ i0 ] );
                    XMVECTOR len = XMVector3Length( t );
                    if ( XMVector3Less( len, g_MeshEpsilon ) )
                    {
                        print( "%u: %f %f %f\n", i0, XMVectorGetX(t), XMVectorGetY(t), XMVectorGetZ(t) );
                        pass = false;
                    }

                    t = XMLoadFloat3( &tangents[ i1 ] );
                    len = XMVector3Length( t );
                    if ( XMVector3Less( len, g_MeshEpsilon ) )
                    {
                        print( "%u: %f %f %f\n", i1, XMVectorGetX(t), XMVectorGetY(t), XMVectorGetZ(t) );
                        pass = false;
                    }

                    t = XMLoadFloat3( &tangents[ i2 ] );
                    len = XMVector3Length( t );
                    if ( XMVector3Less( len, g_MeshEpsilon ) )
                    {
                        print( "%u: %f %f %f\n", i2, XMVectorGetX(t), XMVectorGetY(t), XMVectorGetZ(t) );
                        pass = false;
                    }
                }

                if ( !pass )
                {
                    printe("ERROR: ComputeTangentFrame failed to provide non-zero length tangent vectors:\n%S\n", szPath );
                    success = false;
                }

                // Bitangents
                pass = true;
                for( size_t j = 0; j < nFaces; j += 3 )
                {
                    uint16_t i0 = mesh->indices[ j ];
                    uint16_t i1 = mesh->indices[ j+1 ];
                    uint16_t i2 = mesh->indices[ j+2 ];

                    if ( i0 == uint16_t(-1)
                         || i1 == uint16_t(-1)
                         || i2 == uint16_t(-1) )
                         continue;

                    // ignore degenerate faces
                    if ( i0 == i1
                         || i0 == i2
                         || i1 == i2 )
                         continue;

                    XMVECTOR b = XMLoadFloat3( &bitangents[ i0 ] );
                    XMVECTOR len = XMVector3Length( b );
                    if ( XMVector3Less( len, g_MeshEpsilon ) )
                    {
                        print( "%u: %f %f %f\n", i0, XMVectorGetX(b), XMVectorGetY(b), XMVectorGetZ(b) );
                        pass = false;
                    }

                    b = XMLoadFloat3( &bitangents[ i1 ] );
                    len = XMVector3Length( b );
                    if ( XMVector3Less( len, g_MeshEpsilon ) )
                    {
                        print( "%u: %f %f %f\n", i1, XMVectorGetX(b), XMVectorGetY(b), XMVectorGetZ(b) );
                        pass = false;
                    }

                    b = XMLoadFloat3( &bitangents[ i2 ] );
                    len = XMVector3Length( b );
                    if ( XMVector3Less( len, g_MeshEpsilon ) )
                    {
                        print( "%u: %f %f %f\n", i2, XMVectorGetX(b), XMVectorGetY(b), XMVectorGetZ(b) );
                        pass = false;
                    }
                }

                if ( !pass )
                {
                    printe("ERROR: ComputeTangentFrame failed to provide non-zero length bitangent vectors:\n%S\n", szPath );
                    success = false;
                }
            }
        }
        else
        {
#ifdef DEBUG
            OutputDebugStringA( "INFO: no texture coordinates\n" );
#endif
        }

        ++npass;
    }

    print("\n%Iu meshes tested, %Iu meshes passed ", ncount, npass );

    return success;
}


//-------------------------------------------------------------------------------------
// MeshClean
bool Test02()
{
    bool success = true;

    size_t ncount = 0;
    size_t npass = 0;

    for( size_t index=0; index < _countof(g_TestMedia); ++index )
    {
        WCHAR szPath[MAX_PATH];
        ExpandEnvironmentStringsW( g_TestMedia[index].fname, szPath, MAX_PATH );

#ifdef DEBUG
        OutputDebugStringW(szPath);
        OutputDebugStringA("\n");
#endif

        WCHAR ext[_MAX_EXT];
        _wsplitpath_s( szPath, nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT );

        ++ncount;

        std::unique_ptr<WaveFrontReader<uint16_t>> mesh( new WaveFrontReader<uint16_t>() );

        print( "." );

        HRESULT hr;
        if ( _wcsicmp( ext, L".vbo" ) == 0 )
        {
            hr = mesh->LoadVBO( szPath );
        }
        else
        {
            hr = mesh->Load( szPath );
        }

        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed loading mesh data (%08X):\n%S\n", hr, szPath );
            continue;
        }

        size_t nFaces = mesh->indices.size() / 3;
        size_t nVerts = mesh->vertices.size();

#ifdef DEBUG
        char output[ 256 ] = { 0 };
        sprintf_s( output, "INFO: %Iu verts, %Iu faces\n", nVerts, nFaces );
        OutputDebugStringA( output );
#endif

        std::wstring msgs;
        hr = Validate( &mesh->indices.front(), nFaces, nVerts, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed Validate mesh data (%08X):\n%S\n%S\n", hr, szPath, msgs.c_str() );
            continue;
        }

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ mesh->indices.size() ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) *  mesh->indices.size() );

        std::unique_ptr<XMFLOAT3[]> pos( new XMFLOAT3[ nVerts ] );
        for( size_t j = 0; j < nVerts; ++j )
            pos[ j ] = mesh->vertices[ j ].position;

        hr = GenerateAdjacencyAndPointReps( &mesh->indices.front(), nFaces, pos.get(), nVerts, 0.f, nullptr, adj.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: failed GenerateAdjacencyAndPointReps (%08X)\n:%S", hr, szPath );
            continue;
        }

        hr = Validate( &mesh->indices.front(), nFaces, nVerts, adj.get(), VALIDATE_DEFAULT | VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed Validate [adjacency] (%08X):\n%S\n%S\n", hr, szPath, msgs.c_str() );
            continue;
        }

        // bowties
        std::unique_ptr<uint16_t[]> newIndices( new uint16_t[ nFaces * 3 ] );
        memcpy( newIndices.get(), &mesh->indices.front(), sizeof(uint16_t) * nFaces * 3 );

        std::unique_ptr<uint32_t[]> newAdj( new uint32_t[ mesh->indices.size() ] );
        memcpy( newAdj.get(), adj.get(), sizeof(uint32_t) * nFaces * 3 );

        std::vector<uint32_t> dupVerts;
        hr = Clean( newIndices.get(), nFaces, nVerts, newAdj.get(), nullptr, dupVerts, true );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: Clean [bowties] failed (%08X)\n:%S", hr, szPath );
            continue;
        }

        if ( g_TestMedia[index].bowtieDups != dupVerts.size() )
        {
            success = false;
            printe( "ERROR: Unexpected duplicate count for bowties %Iu .. %Iu:\n%S\n", dupVerts.size(), g_TestMedia[index].bowtieDups, szPath );
            continue;
        }

        size_t nTotalVerts = nVerts + dupVerts.size();

        hr = Validate( newIndices.get(), nFaces, nTotalVerts, newAdj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES | VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed Validate cleaned [bowties] mesh (%08X):\n%S\n%S\n", hr, szPath, msgs.c_str() );
            continue;
        }

        if ( !mesh->attributes.empty() )
        {
            // attributes
            std::unique_ptr<uint32_t[]> attributes( new uint32_t[ nFaces ] );
            for( size_t j = 0; j < mesh->attributes.size(); ++j )
                attributes[ j ] = mesh->attributes[ j ];

            memcpy( newIndices.get(), &mesh->indices.front(), sizeof(uint16_t) * nFaces * 3 );
            memcpy( newAdj.get(), adj.get(), sizeof(uint32_t) * nFaces * 3 );

            dupVerts.clear();
            hr = Clean( newIndices.get(), nFaces, nVerts, newAdj.get(), attributes.get(), dupVerts, false );
            if ( FAILED(hr) )
            {
                success = false;
                printe("ERROR: Clean [attributes] failed (%08X)\n:%S", hr, szPath );
                continue;
            }

            if ( g_TestMedia[index].attrDups != dupVerts.size() )
            {
                success = false;
                printe( "ERROR: Unexpected duplicate count for attributes %Iu .. %Iu:\n%S\n", dupVerts.size(), g_TestMedia[index].attrDups, szPath );
                continue;
            }

            nTotalVerts = nVerts + dupVerts.size();

            hr = Validate( newIndices.get(), nFaces, nTotalVerts, newAdj.get(), VALIDATE_DEFAULT | VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs );
            if ( FAILED(hr) )
            {
                success = false;
                printe( "ERROR: Failed Validate cleaned [attributes] mesh (%08X):\n%S\n%S\n", hr, szPath, msgs.c_str() );
                continue;
            }

            // attributes and bowties
            for( size_t j = 0; j < mesh->attributes.size(); ++j )
                attributes[ j ] = mesh->attributes[ j ];

            memcpy( newIndices.get(), &mesh->indices.front(), sizeof(uint16_t) * nFaces * 3 );
            memcpy( newAdj.get(), adj.get(), sizeof(uint32_t) * nFaces * 3 );

            dupVerts.clear();
            hr = Clean( newIndices.get(), nFaces, nVerts, newAdj.get(), attributes.get(), dupVerts, true );
            if ( FAILED(hr) )
            {
                success = false;
                printe("ERROR: Clean [full] failed (%08X)\n:%S", hr, szPath );
                continue;
            }

            size_t expectedDups = g_TestMedia[index].bowtieDups + g_TestMedia[index].attrDups;
            if ( expectedDups != dupVerts.size() )
            {
                success = false;
                printe( "ERROR: Unexpected duplicate [full] count %Iu .. %Iu:\n%S\n", dupVerts.size(), expectedDups, szPath );
                continue;
            }

            nTotalVerts = nVerts + dupVerts.size();

            hr = Validate( newIndices.get(), nFaces, nTotalVerts, newAdj.get(), VALIDATE_BACKFACING | VALIDATE_BOWTIES | VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs );
            if ( FAILED(hr) )
            {
                success = false;
                printe( "ERROR: Failed Validate cleaned [attributes] mesh (%08X):\n%S\n%S\n", hr, szPath, msgs.c_str() );
                continue;
            }
        }

        ++npass;
    }

    print("\n%Iu meshes tested, %Iu meshes passed ", ncount, npass );

    return success;
}


//-------------------------------------------------------------------------------------
// MeshOpt
bool Test03()
{
    bool success = true;

    size_t ncount = 0;
    size_t npass = 0;

    for( size_t index=0; index < _countof(g_TestMedia); ++index )
    {
        WCHAR szPath[MAX_PATH];
        ExpandEnvironmentStringsW( g_TestMedia[index].fname, szPath, MAX_PATH );

#ifdef DEBUG
        OutputDebugStringW(szPath);
        OutputDebugStringA("\n");
#endif

        WCHAR ext[_MAX_EXT];
        _wsplitpath_s( szPath, nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT );

        ++ncount;

        std::unique_ptr<WaveFrontReader<uint16_t>> mesh( new WaveFrontReader<uint16_t>() );

        print( "." );

        HRESULT hr;
        if ( _wcsicmp( ext, L".vbo" ) == 0 )
        {
            hr = mesh->LoadVBO( szPath );
        }
        else
        {
            hr = mesh->Load( szPath );
        }

        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed loading mesh data (%08X):\n%S\n", hr, szPath );
            continue;
        }

        size_t nFaces = mesh->indices.size() / 3;
        size_t nVerts = mesh->vertices.size();

#ifdef DEBUG
        char output[ 256 ] = { 0 };
        sprintf_s( output, "INFO: %Iu verts, %Iu faces\n", nVerts, nFaces );
        OutputDebugStringA( output );
#endif

        std::wstring msgs;
        hr = Validate( &mesh->indices.front(), nFaces, nVerts, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed Validate mesh data (%08X):\n%S\n%S\n", hr, szPath, msgs.c_str() );
            continue;
        }

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ mesh->indices.size() ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) *  mesh->indices.size() );

        std::unique_ptr<XMFLOAT3[]> pos( new XMFLOAT3[ nVerts ] );
        for( size_t j = 0; j < nVerts; ++j )
            pos[ j ] = mesh->vertices[ j ].position;

        hr = GenerateAdjacencyAndPointReps( &mesh->indices.front(), nFaces, pos.get(), nVerts, 0.f, nullptr, adj.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: failed GenerateAdjacencyAndPointReps (%08X)\n:%S", hr, szPath );
            continue;
        }

        hr = Validate( &mesh->indices.front(), nFaces, nVerts, adj.get(), VALIDATE_DEFAULT | VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed Validate [adjacency] mesh data (%08X):\n%S\n%S\n", hr, szPath, msgs.c_str() );
            continue;
        }

        static const uint32_t s_vcache[] = { OPTFACES_V_STRIPORDER, OPTFACES_V_DEFAULT, 24, 32 };
        static const uint32_t s_restart[] = { 0, OPTFACES_R_DEFAULT, 20, 16 };

        bool pass = true;

        for( size_t vindex = 0; vindex < _countof( s_vcache ); ++vindex )
        {
            float acmr, atvr;
            ComputeVertexCacheMissRate( &mesh->indices.front(), nFaces, nVerts, ( !s_vcache[ vindex ] ) ? OPTFACES_V_DEFAULT : s_vcache[ vindex ], acmr, atvr );

#ifdef DEBUG
            sprintf_s( output, "INFO: original: %u vache, ACMR %f, ATVR %f\n", ( !s_vcache[ vindex ] ) ? OPTFACES_V_DEFAULT : s_vcache[ vindex ], acmr, atvr );
            OutputDebugStringA( output );
#endif

            std::unique_ptr<uint32_t[]> faceRemap( new uint32_t[ nFaces ] );
            memset( faceRemap.get(), 0xff, sizeof(uint32_t) * nFaces );

            hr = OptimizeFaces( &mesh->indices.front(), nFaces, adj.get(), faceRemap.get(), s_vcache[ vindex ], s_restart[ vindex ] );
            if ( FAILED(hr) )
            {
                pass = false;
                success = false;
                printe("ERROR: OptimizeFaces %u vcache, %u restart failed (%08X):\n%S\n", s_vcache[ vindex ], s_restart[ vindex ], hr, szPath );
                continue;
            }
            else if ( !IsValidFaceRemap( &mesh->indices.front(), faceRemap.get(), nFaces ) )
            {
                pass = false;
                success = false;
                printe("ERROR: OptimizeFaces %u vcache, %u restart failed remap invalid:\n%S\n", s_vcache[ vindex ], s_restart[ vindex ], szPath );
                continue;
            }
            else
            {
                std::unique_ptr<uint16_t[]> newIndices( new uint16_t[ nFaces * 3 ] );
                hr = ReorderIB( &mesh->indices.front(), nFaces, faceRemap.get(), newIndices.get() );
                if ( FAILED(hr) )
                {
                    pass = false;
                    success = false;
                    printe("ERROR: ReorderIB %u vcache, %u restart failed (%08X)\n", s_vcache[ vindex ], s_restart[ vindex ], hr );
                    continue;
                }
                else
                {
                    float acmr2, atvr2;
                    ComputeVertexCacheMissRate( newIndices.get(), nFaces, nVerts, ( !s_vcache[ vindex ] ) ? OPTFACES_V_DEFAULT : s_vcache[ vindex ], acmr2, atvr2 );

#ifdef DEBUG
                    sprintf_s( output, "optimized: %u vache, ACMR %f, ATVR %f\n", ( !s_vcache[ vindex ] ) ? OPTFACES_V_DEFAULT : s_vcache[ vindex ], acmr2, atvr2 );
                    OutputDebugStringA( output );
#endif

                    switch( s_vcache[ vindex ] )
                    {
                    case OPTFACES_V_STRIPORDER:
                        if ( ( (acmr2 > g_TestMedia[index].stripOrderACMR) && fabs(acmr2 - g_TestMedia[index].stripOrderACMR) > g_Epsilon )
                             || ( (atvr2 > g_TestMedia[index].stripOrderATVR) && fabs(atvr2 - g_TestMedia[index].stripOrderATVR) > g_Epsilon ) )
                        {
                            pass = false;
                            success = false;
                            printe("ERROR: OptimizeFaces [strip order] failed compared to D3DX:\n%S\n", szPath );
                            print( "\toriginal: ACMR %f, ATVR %f\n", acmr, atvr );
                            print( "\toptimized: ACMR %f, ATVR %f\n", acmr2, atvr2 );
                            print( "\texpected: ACMR %f, ATVR %f\n", g_TestMedia[index].stripOrderACMR, g_TestMedia[index].stripOrderATVR );
                        }
                        break;

                    case OPTFACES_V_DEFAULT:
                        if ( ( (acmr2 > g_TestMedia[index].independentACMR) && fabs(acmr2 - g_TestMedia[index].independentACMR) > g_Epsilon )
                            || ( (atvr2 > g_TestMedia[index].independentATVR) && fabs(atvr2 - g_TestMedia[index].independentATVR) > g_Epsilon ) )
                        {
                            pass = false;
                            success = false;
                            printe("ERROR: OptimizeFaces [device independent] failed compared to D3DX:\n%S\n", szPath );
                            print( "\toriginal: ACMR %f, ATVR %f\n", acmr, atvr );
                            print( "\toptimized: ACMR %f, ATVR %f\n", acmr2, atvr2 );
                            print( "\texpected: ACMR %f, ATVR %f\n", g_TestMedia[index].independentACMR, g_TestMedia[index].independentATVR );
                        }
                        break;

                    default:
                        if ( ( (acmr2 > acmr) && fabs( acmr2 - acmr ) > g_Epsilon )
                             || ( (atvr2 > atvr) && fabs( atvr2 - atvr ) > g_Epsilon ) )
                        {
                            pass = false;
                            success = false;
                            printe("ERROR: OptimizeFaces %u vcache, %u restart failed making new version slower:\n%S\n",
                                   s_vcache[ vindex ], s_restart[ vindex ], szPath );
                            print( "\toriginal: ACMR %f, ATVR %f\n", acmr, atvr );
                            print( "\toptimized: ACMR %f, ATVR %f\n", acmr2, atvr2 );
                        }
                        break;
                    }
                }

                std::unique_ptr<uint32_t[]> vertRemap( new uint32_t[ nVerts ] );
                memset( vertRemap.get(), 0xff, sizeof(uint32_t) * nVerts );

                hr = OptimizeVertices( newIndices.get(), nFaces, nVerts, vertRemap.get() );
                if ( FAILED(hr) )
                {
                    pass = false;
                    success = false;
                    printe("ERROR: OptimizeVertices %u vcache, %u restart failed (%08X):\n%S\n", s_vcache[ vindex ], s_restart[ vindex ], hr, szPath );
                }
                else if ( !IsValidVertexRemap( newIndices.get(), nFaces, vertRemap.get(), nVerts ) )
                {
                    pass = false;
                    success = false;
                    printe("ERROR: OptimizeVertices %u vcache, %u remap invalid:\n%S\n", s_vcache[ vindex ], s_restart[ vindex ], szPath );
                }
                else
                {
                    hr = FinalizeIB( newIndices.get(), nFaces, vertRemap.get(), nVerts );
                    if ( FAILED(hr) )
                    {
                        pass = false;
                        success = false;
                        printe("ERROR: FinalizeIB (%u vcache, %u) failed (%08X):\n%S\n", s_vcache[ vindex ], s_restart[ vindex ], hr, szPath );
                    }

                    std::unique_ptr<WaveFrontReader<uint16_t>::Vertex> vb( new WaveFrontReader<uint16_t>::Vertex[ nVerts ] );
                    hr = FinalizeVB( &mesh->vertices.front(), sizeof(WaveFrontReader<uint16_t>::Vertex), nVerts, nullptr, 0, 
                                     vertRemap.get(), vb.get() );
                    if ( FAILED(hr) )
                    {
                        pass = false;
                        success = false;
                        printe("ERROR: FinalizeVB (%u vcache, %u) failed (%08X):\n%S\n", s_vcache[ vindex ], s_restart[ vindex ], hr, szPath );
                    }
                }
            }
        }
            
        if( pass )
            ++npass;
    }

    print("\n%Iu meshes tested, %Iu meshes passed ", ncount, npass );

    return success;
}


//-------------------------------------------------------------------------------------
// MeshOpt (attributes)
bool Test04()
{
    bool success = true;

    size_t ncount = 0;
    size_t npass = 0;

    for( size_t index=0; index < _countof(g_TestMedia); ++index )
    {
        WCHAR szPath[MAX_PATH];
        ExpandEnvironmentStringsW( g_TestMedia[index].fname, szPath, MAX_PATH );

#ifdef DEBUG
        OutputDebugStringW(szPath);
        OutputDebugStringA("\n");
#endif

        WCHAR ext[_MAX_EXT];
        _wsplitpath_s( szPath, nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT );

        ++ncount;

        std::unique_ptr<WaveFrontReader<uint16_t>> mesh( new WaveFrontReader<uint16_t>() );

        print( "." );

        HRESULT hr;
        if ( _wcsicmp( ext, L".vbo" ) == 0 )
        {
            hr = mesh->LoadVBO( szPath );
        }
        else
        {
            hr = mesh->Load( szPath );
        }

        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed loading mesh data (%08X):\n%S\n", hr, szPath );
            continue;
        }

        size_t nFaces = mesh->indices.size() / 3;
        size_t nVerts = mesh->vertices.size();

#ifdef DEBUG
        char output[ 256 ] = { 0 };
        sprintf_s( output, "INFO: %Iu verts, %Iu faces\n", nVerts, nFaces );
        OutputDebugStringA( output );
#endif

        std::wstring msgs;
        hr = Validate( &mesh->indices.front(), nFaces, nVerts, nullptr, VALIDATE_DEFAULT, &msgs );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed Validate mesh data (%08X):\n%S\n%S\n", hr, szPath, msgs.c_str() );
            continue;
        }

        std::unique_ptr<uint32_t[]> adj( new uint32_t[ mesh->indices.size() ] );
        memset( adj.get(), 0xff, sizeof(uint32_t) *  mesh->indices.size() );

        std::unique_ptr<XMFLOAT3[]> pos( new XMFLOAT3[ nVerts ] );
        for( size_t j = 0; j < nVerts; ++j )
            pos[ j ] = mesh->vertices[ j ].position;

        hr = GenerateAdjacencyAndPointReps( &mesh->indices.front(), nFaces, pos.get(), nVerts, 0.f, nullptr, adj.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: failed to generate required adj (%08X)\n:%S", hr, szPath );
            continue;
        }

        hr = Validate( &mesh->indices.front(), nFaces, nVerts, adj.get(), VALIDATE_DEFAULT | VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed Validate [adjacency] mesh data (%08X):\n%S\n%S\n", hr, szPath, msgs.c_str() );
            continue;
        }

        std::unique_ptr<uint32_t[]> attributes( new uint32_t[ nFaces ] );
        memset( attributes.get(), 0, sizeof(uint32_t) * nFaces );
        for( size_t j = 0; j < mesh->attributes.size(); ++j )
            attributes[ j ] = mesh->attributes[ j ];

        std::unique_ptr<uint16_t[]> sortedIndices( new uint16_t[ nFaces * 3 ] );
        memcpy( sortedIndices.get(), &mesh->indices.front(), sizeof(uint16_t) * nFaces * 3 );

        std::vector<uint32_t> dupVerts;
        hr = Clean( sortedIndices.get(), nFaces, nVerts, adj.get(), attributes.get(), dupVerts );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: Clean failed (%08X)\n:%S", hr, szPath );
            continue;
        }

#ifdef DEBUG
        sprintf_s( output, "INFO: %Iu duplicated verts\n", dupVerts.size() );
        OutputDebugStringA( output );
#endif

        if ( g_TestMedia[index].attrDups != dupVerts.size() )
        {
            success = false;
            printe( "ERROR: Unexpected duplicate count %Iu .. %Iu:\n%S\n", dupVerts.size(), g_TestMedia[index].attrDups, szPath );
            continue;
        }

        size_t nTotalVerts = nVerts + dupVerts.size();

        hr = Validate( sortedIndices.get(), nFaces, nTotalVerts, adj.get(), VALIDATE_BACKFACING | VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed Validate cleaned mesh (%08X):\n%S\n%S\n", hr, szPath, msgs.c_str() );
            continue;
        }

        std::unique_ptr<uint32_t[]> faceRemap( new uint32_t[ nFaces ] );
        memset( faceRemap.get(), 0xff, sizeof(uint32_t) * nFaces );

        hr = AttributeSort( nFaces, attributes.get(), faceRemap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: AttributeSort failed (%08X)\n:%S", hr, szPath );
            continue;
        }
        else if ( !IsValidFaceRemap( sortedIndices.get(), faceRemap.get(), nFaces ) )
        {
            success = false;
            printe("ERROR: Attribute failed remap invalid:\n%S\n", szPath );
            continue;
        }

        hr = ReorderIB( sortedIndices.get(), nFaces, faceRemap.get() );
        if ( FAILED(hr) )
        {
            success = false;
            printe("ERROR: AttributeSort application failed (%08X)\n", hr );
            continue;
        }

        hr = Validate( sortedIndices.get(), nFaces, nTotalVerts, adj.get(), VALIDATE_DEFAULT | VALIDATE_UNUSED | VALIDATE_ASYMMETRIC_ADJ, &msgs );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed Validate attribute sorted mesh (%08X):\n%S\n%S\n", hr, szPath, msgs.c_str() );
            continue;
        }

        static const uint32_t s_vcache[] = { OPTFACES_V_STRIPORDER, OPTFACES_V_DEFAULT, 24, 32 };
        static const uint32_t s_restart[] = { 0, OPTFACES_R_DEFAULT, 20, 16 };

        bool pass = true;

        for( size_t vindex = 0; vindex < _countof( s_vcache ); ++vindex )
        {
            float acmr, atvr;
            ComputeVertexCacheMissRate( sortedIndices.get(), nFaces, nTotalVerts, ( !s_vcache[ vindex ] ) ? OPTFACES_V_DEFAULT : s_vcache[ vindex ], acmr, atvr );

#ifdef DEBUG
            auto subsets = ComputeSubsets( attributes.get(), nFaces );
            sprintf_s( output, "INFO: %Iu attribute groups\nINFO: original: %u vache, ACMR %f, ATVR %f\n", subsets.size(),
                              ( !s_vcache[ vindex ] ) ? OPTFACES_V_DEFAULT : s_vcache[ vindex ], acmr, atvr );
            OutputDebugStringA( output );
#endif

            hr = OptimizeFacesEx( sortedIndices.get(), nFaces, adj.get(), attributes.get(), faceRemap.get(), s_vcache[ vindex ], s_restart[ vindex ] );
            if ( FAILED(hr) )
            {
                pass = false;
                success = false;
                printe("ERROR: OptimizeFaces %u vcache, %u restart failed (%08X):\n%S\n", s_vcache[ vindex ], s_restart[ vindex ], hr, szPath );
                continue;
            }
            else if ( !IsValidFaceRemap( sortedIndices.get(), faceRemap.get(), nFaces ) )
            {
                pass = false;
                success = false;
                printe("ERROR: OptimizeFaces %u vcache, %u restart failed remap invalid:\n%S\n", s_vcache[ vindex ], s_restart[ vindex ], szPath );
                continue;
            }
            else
            {
#if 0
printf("FACEREMAP:\n");
for(size_t j = 0; j < nFaces; ++j )
{
//if ( j >= 10 && j < (nFaces-10) ) j = nFaces - 10;
//printf("%Iu -> %u\n", j, faceRemap[j]);
printf("%u\n", faceRemap[j]);
}
#endif
                std::unique_ptr<uint16_t[]> newIndices( new uint16_t[ nFaces * 3 ] );

                hr = ReorderIB( sortedIndices.get(), nFaces, faceRemap.get(), newIndices.get() );
                if ( FAILED(hr) )
                {
                    pass = false;
                    success = false;
                    printe("ERROR: ReorderIB %u vcache, %u restart failed (%08X)\n", s_vcache[ vindex ], s_restart[ vindex ], hr );
                    continue;
                }
                else
                {
                    float acmr2, atvr2;
                    ComputeVertexCacheMissRate( newIndices.get(), nFaces, nTotalVerts, ( !s_vcache[ vindex ] ) ? OPTFACES_V_DEFAULT : s_vcache[ vindex ], acmr2, atvr2 );

#ifdef DEBUG
                    sprintf_s( output, "optimized: %u vache, ACMR %f, ATVR %f\n", ( !s_vcache[ vindex ] ) ? OPTFACES_V_DEFAULT : s_vcache[ vindex ], acmr2, atvr2 );
                    OutputDebugStringA( output );
#endif

                    switch( s_vcache[ vindex ] )
                    {
                    case OPTFACES_V_STRIPORDER:
                        if ( ( (acmr2 > g_TestMedia[index].attrStripOrderACMR) && fabs(acmr2 - g_TestMedia[index].attrStripOrderACMR) > g_Epsilon )
                             || ( (atvr2 > g_TestMedia[index].attrStripOrderATVR) && fabs(atvr2 - g_TestMedia[index].attrStripOrderATVR) > g_Epsilon ) )
                        {
                            pass = false;
                            success = false;
                            printe("ERROR: OptimizeFaces [strip order] failed compared to D3DX:\n%S\n", szPath );
                            print( "\toriginal: ACMR %f, ATVR %f\n", acmr, atvr );
                            print( "\toptimized: ACMR %f, ATVR %f\n", acmr2, atvr2 );
                            print( "\texpected: ACMR %f, ATVR %f\n", g_TestMedia[index].attrStripOrderACMR, g_TestMedia[index].attrStripOrderATVR );
                        }
                        break;

                    case OPTFACES_V_DEFAULT:
                        if ( ( (acmr2 > g_TestMedia[index].attrIndependentACMR) && fabs(acmr2 - g_TestMedia[index].attrIndependentACMR) > g_Epsilon )
                            || ( (atvr2 > g_TestMedia[index].attrIndependentATVR) && fabs(atvr2 - g_TestMedia[index].attrIndependentATVR) > g_Epsilon ) )
                        {
                            pass = false;
                            success = false;
                            printe("ERROR: OptimizeFaces [device independent] failed compared to D3DX:\n%S\n", szPath );
                            print( "\toriginal: ACMR %f, ATVR %f\n", acmr, atvr );
                            print( "\toptimized: ACMR %f, ATVR %f\n", acmr2, atvr2 );
                            print( "\texpected: ACMR %f, ATVR %f\n", g_TestMedia[index].attrIndependentACMR, g_TestMedia[index].attrIndependentATVR );
                        }
                        break;

                    default:
                        if ( ( (acmr2 > acmr) && fabs( acmr2 - acmr ) > g_Epsilon )
                             || ( (atvr2 > atvr) && fabs( atvr2 - atvr ) > g_Epsilon ) )
                        {
                            pass = false;
                            success = false;
                            printe("ERROR: OptimizeFaces %u vcache, %u restart failed making new version slower:\n%S\n",
                                   s_vcache[ vindex ], s_restart[ vindex ], szPath );
                            print( "\toriginal: ACMR %f, ATVR %f\n", acmr, atvr );
                            print( "\toptimized: ACMR %f, ATVR %f\n", acmr2, atvr2 );
                        }
                        break;
                    }
                }

                std::unique_ptr<uint32_t[]> vertRemap( new uint32_t[ nTotalVerts ] );
                memset( vertRemap.get(), 0xff, sizeof(uint32_t) * nTotalVerts );

                hr = OptimizeVertices( newIndices.get(), nFaces, nTotalVerts, vertRemap.get() );
                if ( FAILED(hr) )
                {
                    pass = false;
                    success = false;
                    printe("ERROR: OptimizeVertices %u vcache, %u restart failed (%08X):\n%S\n", s_vcache[ vindex ], s_restart[ vindex ], hr, szPath );
                }
                else if ( !IsValidVertexRemap( newIndices.get(), nFaces, vertRemap.get(), nTotalVerts ) )
                {
                    pass = false;
                    success = false;
                    printe("ERROR: OptimizeVertices %u vcache, %u remap invalid:\n%S\n", s_vcache[ vindex ], s_restart[ vindex ], szPath );
                }
                else
                {
                    hr = FinalizeIB( newIndices.get(), nFaces, vertRemap.get(), nTotalVerts );
                    if ( FAILED(hr) )
                    {
                        pass = false;
                        success = false;
                        printe("ERROR: FinalizeIB (%u vcache, %u) failed (%08X):\n%S\n", s_vcache[ vindex ], s_restart[ vindex ], hr, szPath );
                    }

                    std::unique_ptr<WaveFrontReader<uint16_t>::Vertex> vb( new WaveFrontReader<uint16_t>::Vertex[ nTotalVerts ] );
                    hr = FinalizeVB( &mesh->vertices.front(), sizeof(WaveFrontReader<uint16_t>::Vertex), nVerts,
                                     dupVerts.empty() ? nullptr : &dupVerts.front(), dupVerts.size(), 
                                     vertRemap.get(), vb.get() );
                    if ( FAILED(hr) )
                    {
                        pass = false;
                        success = false;
                        printe("ERROR: FinalizeVB (%u vcache, %u) failed (%08X):\n%S\n", s_vcache[ vindex ], s_restart[ vindex ], hr, szPath );
                    }
                }
            }
        }
            
        if( pass )
            ++npass;
    }

    print("\n%Iu meshes tested, %Iu meshes passed ", ncount, npass );

    return success;
}
