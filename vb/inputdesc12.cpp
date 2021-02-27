//-------------------------------------------------------------------------------------
// inputdesc12.cpp
//  
// Copyright (c) Microsoft Corporation.
//-------------------------------------------------------------------------------------

#include "directxtest.h"

#include <d3d12.h>
#include "DirectXMesh.h"

#include "TestHelpers.h"
#include "TestInputLayouts.h"

using namespace DirectX;
using namespace TestInputLayouts12;

namespace
{
    const uint32_t VSStarterKitAnimationStride1 = 52;
    const uint32_t VSStarterKitAnimationStride2 = 20;

    const D3D12_INPUT_ELEMENT_DESC s_instlayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "mTransform", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
        { "mTransform", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
        { "mTransform", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
        { "mTransform", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
    };

    const D3D12_INPUT_ELEMENT_DESC s_leaflayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "mTransform", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 10 },
        { "mTransform", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 10 },
        { "mTransform", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 10 },
        { "mTransform", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 10 },
        { "fOcc",       0, DXGI_FORMAT_R32_FLOAT,          1, 64, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 10 },
    };

    struct TestDesc
    {
        const char* name;
        size_t elements;
        const D3D12_INPUT_ELEMENT_DESC* desc;
        uint32_t stride;
        uint32_t offsets[8];
    };

    const TestDesc g_InputDescs[] =
    {
        { "VertexPositionColor", 2, g_VertexPositionColor, 28, { 0, 12 } },
        { "VertexPositionTexture", 2, g_VertexPositionTexture, 20, {  0, 12 } },
        { "VertexPositionNormal", 2, g_VertexPositionNormal, 24, { 0, 12 } },
        { "VertexPositionColorTexture", 3, g_VertexPositionColorTexture, 36, { 0, 12, 28 } },
        { "VertexPositionNormalColor", 3, g_VertexPositionNormalColor, 40, { 0, 12, 24 } },
        { "VertexPositionNormalTexture", 3, g_VertexPositionNormalTexture, 32, { 0, 12, 24 } },
        { "VertexPositionNormalColorTexture", 4, g_VertexPositionNormalColorTexture, 48, { 0, 12, 24, 40 } },
        { "VertexPositionNormalTangentColorTexture", 5, g_VertexPositionNormalTangentColorTexture, 52, { 0, 12, 24, 40, 44 } },
        { "VertexPositionNormalTangentColorTextureSkinning", 7, g_VertexPositionNormalTangentColorTextureSkinning, 60, { 0, 12, 24, 40, 44, 52, 56 } },
        { "D3DTutorial", 2, g_D3DTutorial, 24, { 0, 12 } },
        { "VSStarterKit", 5, g_VSStarterKit, 52, { 0, 12, 24, 40, 44 } },
        { "tangentSpaceVertexLayout", 5, g_tangentspacevertexlayout, 56, { 0, 12, 20, 32, 44 } },
        { "quadLayout", 2, g_quadlayout, 24, { 0, 16 } },
        { "UncompressedLayout", 4, g_UncompressedLayout, 44, { 0, 12, 24, 32 } },
        { "CompressedLayout", 4, g_CompressedLayout, 24, { 0, 12, 16, 20 } },
        { "SkinnedLayout", 6, g_SkinnedLayout, 52, { 0, 12, 16, 20, 32, 40 } },
        { "UnSkinnedLayout", 4, g_UnSkinnedLayout, 48, { 0, 16, 28, 36 } },
        { "layout", 3, g_layout, 32, { 0, 12, 24 } },
        { "colorLayout", 1, g_colorLayout, 16, { 0 } },
    };
}


//-------------------------------------------------------------------------------------
// IsValid
bool Test01_DX12()
{
    bool success = true;

    for (size_t j = 0; j < std::size(g_InputDescs); ++j)
    {
        D3D12_INPUT_LAYOUT_DESC desc = { g_InputDescs[j].desc, static_cast<UINT>(g_InputDescs[j].elements) };
        if (!IsValid(desc))
        {
            printe("ERROR: IsValid failed for desc %s\n", g_InputDescs[j].name);
            success = false;
        }
    }

    // Multi-stream
    D3D12_INPUT_LAYOUT_DESC desc = { g_VSStarterKitAnimation, static_cast<UINT>(std::size(g_VSStarterKitAnimation)) };
    if (!IsValid(desc))
    {
        printe("ERROR: IsValid failed for desc VSStarterKitAnimation\n");
        success = false;
    }

    // Instance
    desc.NumElements = static_cast<UINT>(std::size(s_instlayout));
    desc.pInputElementDescs = s_instlayout;
    if (!IsValid( desc ))
    {
        printe("ERROR: IsValid failed for desc instlayout\n");
        success = false;
    }

    desc.NumElements = static_cast<UINT>(std::size(s_leaflayout));
    desc.pInputElementDescs = s_leaflayout;
    if (!IsValid( desc ))
    {
        printe("ERROR: IsValid failed for desc leaflayout\n");
        success = false;
    }

    return success;
}


//-------------------------------------------------------------------------------------
// ComputeInputLayout
bool Test02_DX12()
{
    bool success = true;

    for( size_t j = 0; j < std::size(g_InputDescs); ++j )
    {
        uint32_t offsets[ D3D12_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT ];
        memset( offsets, 0xff, sizeof(uint32_t) * D3D12_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT );

        uint32_t strides[ D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT ];

        D3D12_INPUT_LAYOUT_DESC desc = { g_InputDescs[j].desc, static_cast<UINT>(g_InputDescs[j].elements) };
        ComputeInputLayout( desc, offsets, strides );

        if ( strides[0] != g_InputDescs[ j ].stride )
        {
            printe("ERROR: ComputeInputLayout failed for desc %s (expected stride %u)\n", g_InputDescs[ j ].name, g_InputDescs[ j ].stride );
            success = false;
            print("\tstride: %u\n", strides[0] );
        }
        else if ( memcmp( offsets, g_InputDescs[ j ].offsets, sizeof(uint32_t) * g_InputDescs[ j ].elements ) != 0 )
        {
            printe("ERROR: ComputeInputLayout failed for desc %s\n", g_InputDescs[ j ].name );
            success = false;

            print("\toffsets: " );
            for( size_t k = 0; k < g_InputDescs[ j ].elements; ++k )
            {
                print( "%u   ", offsets[ k ] );
            }
            print("\n\texpected: ");
            for( size_t k = 0; k < g_InputDescs[ j ].elements; ++k )
            {
                print( "%u   ", offsets[ k ] );
            }
            print("\n");
        }
        else
        {
            // null offsets
            ComputeInputLayout( desc, nullptr, strides );

            if ( strides[0] != g_InputDescs[ j ].stride )
            {
                printe("ERROR: ComputeInputLayout [null offsets] failed for desc %s (expected stride %u)\n", g_InputDescs[ j ].name, g_InputDescs[ j ].stride );
                success = false;
                print("\tstride: %u\n", strides[0] );
            }

            // null strides
            memset( offsets, 0xff, sizeof(uint32_t) * D3D12_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT );

            ComputeInputLayout( desc, offsets, nullptr );

            if ( memcmp( offsets, g_InputDescs[ j ].offsets, sizeof(uint32_t) * g_InputDescs[ j ].elements ) != 0 )
            {
                printe("ERROR: ComputeInputLayout [null strides] failed for desc %s\n", g_InputDescs[ j ].name );
                success = false;

                print("\toffsets: " );
                for( size_t k = 0; k < g_InputDescs[ j ].elements; ++k )
                {
                    print( "%u   ", offsets[ k ] );
                }
                print("\n\texpected: ");
                for( size_t k = 0; k < g_InputDescs[ j ].elements; ++k )
                {
                    print( "%u   ", offsets[ k ] );
                }
                print("\n");
            }
        }
    }

    // Multi-stream
    {
        uint32_t offsets[ D3D12_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT ];
        memset( offsets, 0xff, sizeof(uint32_t) * D3D12_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT );

        uint32_t strides[ D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT ];

        D3D12_INPUT_LAYOUT_DESC desc = { g_VSStarterKitAnimation, static_cast<UINT>(std::size(g_VSStarterKitAnimation)) };
        ComputeInputLayout( desc, offsets, strides );

        if ( strides[0] != VSStarterKitAnimationStride1
             || strides[1] != VSStarterKitAnimationStride2 )
        {
            printe("ERROR: ComputeInputLayout failed for desc VSStarterKitAnimation (expected strides %u / %u)\n", VSStarterKitAnimationStride1, VSStarterKitAnimationStride2 );
            success = false;

            print("\tstrides:" );
            for( size_t k = 0; k < 2; ++k )
            {
                print( "%u   ", strides[ k ] );
            }
            print("\n");
        }
        else if ( offsets[0] != 0
                  || offsets[1] != 12
                  || offsets[2] != 24
                  || offsets[3] != 40
                  || offsets[4] != 44
                  || offsets[5] != 0
                  || offsets[6] != 4 )
        {
            printe("ERROR: ComputeInputLayout failed for desc VSStarterKitAnimation\n" );
            success = false;

            print("\toffsets:" );
            for( size_t k = 0; k < std::size(g_VSStarterKitAnimation); ++k )
            {
                print( "%u   ", offsets[ k ] );
            }
            print("\n\texpected: 0  12  24  40  44  0  4");
        }
    }

    // Instance
    {
        uint32_t offsets[D3D12_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT];
        memset(offsets, 0xff, sizeof(uint32_t) * D3D12_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT);

        uint32_t strides[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

        D3D12_INPUT_LAYOUT_DESC desc = { s_instlayout, static_cast<UINT>(std::size(s_instlayout)) };
        ComputeInputLayout( desc, offsets, strides );

        if ( strides[0] != 32
             || strides[1] != 64 )
        {
            printe("ERROR: ComputeInputLayout failed for desc instlayout (expected strides 32 / 64)\n" );
            success = false;

            print("\tstrides:" );
            for( size_t k = 0; k < 2; ++k )
            {
                print( "%u   ", strides[ k ] );
            }
            print("\n");
        }
        else if ( offsets[0] != 0
                  || offsets[1] != 12
                  || offsets[2] != 24
                  || offsets[3] != 0
                  || offsets[4] != 16
                  || offsets[5] != 32
                  || offsets[6] != 48 )
        {
            printe("ERROR: ComputeInputLayout failed for desc instlayout\n" );
            success = false;

            print("\toffsets:" );
            for( size_t k = 0; k < std::size(s_instlayout); ++k )
            {
                print( "%u   ", offsets[ k ] );
            }
            print("\n\texpected: 0  12  24  0  16  32  48");
        }

        memset( offsets, 0xff, sizeof(uint32_t) * D3D12_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT );

        desc.NumElements = static_cast<UINT>(std::size(s_leaflayout));
        desc.pInputElementDescs = s_leaflayout;
        ComputeInputLayout( desc, offsets, strides );

        if ( strides[0] != 20
             || strides[1] != 68 )
        {
            printe("ERROR: ComputeInputLayout failed for desc leaflayout (expected strides 20 / 68)\n" );
            success = false;

            print("\tstrides:" );
            for( size_t k = 0; k < 2; ++k )
            {
                print( "%u   ", strides[ k ] );
            }
            print("\n");
        }
        else if ( offsets[0] != 0
                  || offsets[1] != 12
                  || offsets[2] != 0
                  || offsets[3] != 16
                  || offsets[4] != 32
                  || offsets[5] != 48
                  || offsets[6] != 64 )
        {
            printe("ERROR: ComputeInputLayout failed for desc leaflayout\n" );
            success = false;

            print("\toffsets:" );
            for( size_t k = 0; k < std::size(s_leaflayout); ++k )
            {
                print( "%u   ", offsets[ k ] );
            }
            print("\n\texpected: 0  12  24  0  16  32  48");
        }
    }

    return success;
}
