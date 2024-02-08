//-------------------------------------------------------------------------------------
// vb12.cpp
//  
// Copyright (c) Microsoft Corporation.
//-------------------------------------------------------------------------------------

#include "directxtest.h"

#ifdef USING_DIRECTX_HEADERS
#include <directx/d3d12.h>
#else
#include <d3d12.h>
#endif

#include "DirectXMesh.h"

#include "DirectXMeshP.h"

#include "TestHelpers.h"
#include "TestInputLayouts.h"
#include "WaveFrontReader.h"
#include "scoped.h"

using namespace DirectX;
using namespace TestInputLayouts12;

namespace
{

    //-----------------------------------------------------------------------------------

    const XMVECTORF32 g_VBEpsilon = { { { 0.01f, 0.01f, 0.01f, 0.01f } } };

    //-----------------------------------------------------------------------------------

    struct SimpleVertex
    {
        XMFLOAT3 Pos;
        XMFLOAT2 Tex;
    };

    const SimpleVertex s_cubeVB[] =
    {
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
    };

    const D3D12_INPUT_ELEMENT_DESC s_cubeIL[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    //-----------------------------------------------------------------------------------

    struct VSStarterKitVertex1
    {
        XMFLOAT3 position;
        XMFLOAT3 normal;
        XMFLOAT4 tangent;
        uint32_t color;
        XMFLOAT2 texureCoordinate;
    };

    static_assert(sizeof(VSStarterKitVertex1) == 52, "Mismatch with VS Starter Kit");

    const VSStarterKitVertex1 s_VSStarterKitVB1[] =
    {
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.f), 0xFF0000FF, XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.f), 0xFF00FF00, XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.f), 0xFFFF0000, XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.f), 0xFFFFFFFF, XMFLOAT2(1.0f, 1.0f) },

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.f, -1.f, 0.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f), 0xFF0000FF, XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.f, -1.f, 0.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f), 0xFF00FF00, XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.f, -1.f, 0.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f), 0xFFFF0000, XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.f, -1.f, 0.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f), 0xFFFFFFFF, XMFLOAT2(0.0f, 1.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.f, 0.f, 0.f), XMFLOAT4(0.0f, 0.0f, -1.0f, 1.f), 0xFF0000FF, XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.f, 0.f, 0.f), XMFLOAT4(0.0f, 0.0f, -1.0f, 1.f), 0xFF00FF00, XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.f, 0.f, 0.f), XMFLOAT4(0.0f, 0.0f, -1.0f, 1.f), 0xFFFF0000, XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.f, 0.f, 0.f), XMFLOAT4(0.0f, 0.0f, -1.0f, 1.f), 0xFFFFFFFF, XMFLOAT2(0.0f, 0.0f) },

        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.f), 0xFF0000FF, XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.f), 0xFF00FF00, XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.f), 0xFFFF0000, XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.f, 0.f, 0.f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.f), 0xFFFFFFFF, XMFLOAT2(1.0f, 0.0f) },

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f), 0xFF0000FF, XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f), 0xFF00FF00, XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f), 0xFFFF0000, XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f), 0xFFFFFFFF, XMFLOAT2(0.0f, 0.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.f, 0.f, 1.f), XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.f), 0xFF0000FF, XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.f, 0.f, 1.f), XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.f), 0xFF00FF00, XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.f, 0.f, 1.f), XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.f), 0xFFFF0000, XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.f, 0.f, 1.f), XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.f), 0xFFFFFFFF, XMFLOAT2(1.0f, 0.0f) },
    };

    struct VSStarterKitVertex2
    {
        uint8_t  boneIndex[4];
        XMFLOAT4 boneWeight;
    };

    static_assert(sizeof(VSStarterKitVertex2) == 20, "Mismatch with VS Starter Kit");

    const VSStarterKitVertex2 s_VSStarterKitVB2[] =
    {
        { { 0, 1, 2, 3 }, XMFLOAT4(0.25f, 0.5f, 0.75f, 1.f) },
        { { 0, 1, 2, 3 }, XMFLOAT4(0.25f, 0.5f, 0.75f, 1.f) },
        { { 0, 1, 2, 3 }, XMFLOAT4(0.25f, 0.5f, 0.75f, 1.f) },
        { { 0, 1, 2, 3 }, XMFLOAT4(0.25f, 0.5f, 0.75f, 1.f) },

        { { 1, 2, 3, 0 }, XMFLOAT4(0.5f, 0.75f, 1.f, 0.25f) },
        { { 1, 2, 3, 0 }, XMFLOAT4(0.5f, 0.75f, 1.f, 0.25f) },
        { { 1, 2, 3, 0 }, XMFLOAT4(0.5f, 0.75f, 1.f, 0.25f) },
        { { 1, 2, 3, 0 }, XMFLOAT4(0.5f, 0.75f, 1.f, 0.25f) },

        { { 2, 3, 0, 1 }, XMFLOAT4(0.75f, 1.f, 0.25f, 0.5f) },
        { { 2, 3, 0, 1 }, XMFLOAT4(0.75f, 1.f, 0.25f, 0.5f) },
        { { 2, 3, 0, 1 }, XMFLOAT4(0.75f, 1.f, 0.25f, 0.5f) },
        { { 2, 3, 0, 1 }, XMFLOAT4(0.75f, 1.f, 0.25f, 0.5f) },

        { { 3, 0, 1, 2 }, XMFLOAT4(1.f, 0.25f, 0.5f, 0.75f) },
        { { 3, 0, 1, 2 }, XMFLOAT4(1.f, 0.25f, 0.5f, 0.75f) },
        { { 3, 0, 1, 2 }, XMFLOAT4(1.f, 0.25f, 0.5f, 0.75f) },
        { { 3, 0, 1, 2 }, XMFLOAT4(1.f, 0.25f, 0.5f, 0.75f) },

        { { 3, 2, 1, 0 }, XMFLOAT4(1.f, 0.75f, 0.5f, 0.25f) },
        { { 3, 2, 1, 0 }, XMFLOAT4(1.f, 0.75f, 0.5f, 0.25f) },
        { { 3, 2, 1, 0 }, XMFLOAT4(1.f, 0.75f, 0.5f, 0.25f) },
        { { 3, 2, 1, 0 }, XMFLOAT4(1.f, 0.75f, 0.5f, 0.25f) },

        { { 2, 3, 0, 1, }, XMFLOAT4(0.75f, 1.f, 0.25f, 0.5f) },
        { { 2, 3, 0, 1, }, XMFLOAT4(0.75f, 1.f, 0.25f, 0.5f) },
        { { 2, 3, 0, 1, }, XMFLOAT4(0.75f, 1.f, 0.25f, 0.5f) },
        { { 2, 3, 0, 1, }, XMFLOAT4(0.75f, 1.f, 0.25f, 0.5f) },
    };

    //-----------------------------------------------------------------------------------

    struct TestVB
    {
        DXGI_FORMAT         format;
        XMFLOAT4            vector;
        size_t              stride;
        uint8_t             bytes[16];
    };

    // format | vector | stride| vb-data
    // format | vector | stride| vb-data
    const TestVB g_TestVBs[] =
    {
        // DXGI_FORMAT_R32G32B32A32_FLOAT
        { DXGI_FORMAT_R32G32B32A32_FLOAT, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), 16, { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R32G32B32A32_FLOAT, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 16, { 0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x3F } },
        { DXGI_FORMAT_R32G32B32A32_FLOAT, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, { 0x00,0x00,0x80,0x3F,0x00,0x00,0x80,0x3F,0x00,0x00,0x80,0x3F,0x00,0x00,0x80,0x3F } },
        { DXGI_FORMAT_R32G32B32A32_FLOAT, XMFLOAT4(-1.0f, -1.0f, -1.0f, -1.0f), 16, { 0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x80, 0xBF } },
        { DXGI_FORMAT_R32G32B32A32_FLOAT, XMFLOAT4(0.25f, 0.5f, 0.75f, 1.0f), 16, { 0x00,0x00,0x80,0x3E,0x00,0x00,0x00,0x3F,0x00,0x00,0x40,0x3F,0x00,0x00,0x80,0x3F } },
        { DXGI_FORMAT_R32G32B32A32_FLOAT, XMFLOAT4(FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN), 16, { 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00 } },
        { DXGI_FORMAT_R32G32B32A32_FLOAT, XMFLOAT4(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX), 16, { 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F } },
        { DXGI_FORMAT_R32G32B32A32_FLOAT, XMFLOAT4(-FLT_MIN, -FLT_MIN, -FLT_MIN, -FLT_MIN), 16, { 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80 } },
        { DXGI_FORMAT_R32G32B32A32_FLOAT, XMFLOAT4(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX), 16, { 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF } },
        // DXGI_FORMAT_R32G32B32A32_UINT
        { DXGI_FORMAT_R32G32B32A32_UINT, XMFLOAT4(4294967296.f, 4294967296.f, 4294967296.f, 4294967296.f), 16, { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R32G32B32A32_UINT, XMFLOAT4(1.0f, 2.0f, 3.0f, 4.0f), 16, { 0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00 } },
        // DXGI_FORMAT_R32G32B32A32_SINT
        { DXGI_FORMAT_R32G32B32A32_SINT, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, { 0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R32G32B32A32_SINT, XMFLOAT4(-1.0f, -1.0f, -1.0f, -1.0f), 16, { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R32G32B32A32_SINT, XMFLOAT4(1.0f, 2.0f, 3.0f, 4.0f), 16, { 0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R32G32B32A32_SINT, XMFLOAT4(-1.0f, -2.0f, -3.0f, -4.0f), 16, { 0xFF,0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFD,0xFF,0xFF,0xFF,0xFC,0xFF,0xFF,0xFF } },
        // DXGI_FORMAT_R32G32B32_FLOAT
        { DXGI_FORMAT_R32G32B32_FLOAT, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.f), 12, { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R32G32B32_FLOAT, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.f), 12, { 0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x3F } },
        { DXGI_FORMAT_R32G32B32_FLOAT, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.f), 12, { 0x00,0x00,0x80,0x3F,0x00,0x00,0x80,0x3F,0x00,0x00,0x80,0x3F } },
        { DXGI_FORMAT_R32G32B32_FLOAT, XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.f), 12, { 0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x80, 0xBF } },
        { DXGI_FORMAT_R32G32B32_FLOAT, XMFLOAT4(0.25f, 0.5f, 0.75f, 0.f), 12, { 0x00,0x00,0x80,0x3E,0x00,0x00,0x00,0x3F,0x00,0x00,0x40,0x3F } },
        { DXGI_FORMAT_R32G32B32_FLOAT, XMFLOAT4(FLT_MIN, FLT_MIN, FLT_MIN, 0.f), 12, { 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00 } },
        { DXGI_FORMAT_R32G32B32_FLOAT, XMFLOAT4(FLT_MAX, FLT_MAX, FLT_MAX, 0.f), 12, { 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F } },
        { DXGI_FORMAT_R32G32B32_FLOAT, XMFLOAT4(-FLT_MIN, -FLT_MIN, -FLT_MIN, 0.f), 12, { 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80 } },
        { DXGI_FORMAT_R32G32B32_FLOAT, XMFLOAT4(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0.f), 12, { 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF } },
        // DXGI_FORMAT_R32G32B32_UINT
        { DXGI_FORMAT_R32G32B32_UINT, XMFLOAT4(4294967296.f, 4294967296.f, 4294967296.f, 0.f), 12, { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R32G32B32_UINT, XMFLOAT4(1.0f, 2.0f, 3.0f, 0.f), 12, { 0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00 } },
        // DXGI_FORMAT_R32G32B32_SINT
        { DXGI_FORMAT_R32G32B32_SINT, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.f), 12, { 0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R32G32B32_SINT, XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.f), 12, { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R32G32B32_SINT, XMFLOAT4(1.0f, 2.0f, 3.0f, 0.f), 12, { 0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R32G32B32_SINT, XMFLOAT4(-1.0f, -2.0f, -3.0f, 0.f), 12, { 0xFF,0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFD,0xFF,0xFF,0xFF } },
        // DXGI_FORMAT_R16G16B16A16_FLOAT
        { DXGI_FORMAT_R16G16B16A16_FLOAT, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), 8, { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R16G16B16A16_FLOAT, XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f), 8, { 0x00,0x34,0x00,0x34,0x00,0x34,0x00,0x3C } },
        { DXGI_FORMAT_R16G16B16A16_FLOAT, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 8, { 0x00,0x38,0x00,0x38,0x00,0x38,0x00,0x38 } },
        { DXGI_FORMAT_R16G16B16A16_FLOAT, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 8, { 0x00,0x3C,0x00,0x3C,0x00,0x3C,0x00,0x3C } },
        { DXGI_FORMAT_R16G16B16A16_FLOAT, XMFLOAT4(-1.0f, -1.0f, -1.0f, -1.0f), 8, { 0x00,0xBC,0x00,0xBC,0x00,0xBC,0x00,0xBC } },
        { DXGI_FORMAT_R16G16B16A16_FLOAT, XMFLOAT4(0.25f, 0.5f, 0.75f, 1.0f), 8, { 0x00,0x34,0x00,0x38,0x00,0x3A,0x00,0x3C } },
        { DXGI_FORMAT_R16G16B16A16_FLOAT, XMFLOAT4(5.96046448e-8f, 5.96046448e-8f, 5.96046448e-8f, 1.0f), 8, { 0x01,0x00,0x01,0x00,0x01,0x00,0x00,0x3C } },
        { DXGI_FORMAT_R16G16B16A16_FLOAT, XMFLOAT4(6.09755516e-5f, 6.09755516e-5f, 6.09755516e-5f, 1.0f), 8, { 0xFF,0x03,0xFF,0x03,0xFF,0x03,0x00,0x3C } },
        { DXGI_FORMAT_R16G16B16A16_FLOAT, XMFLOAT4(6.10352e-5f, 6.10352e-5f, 6.10352e-5f, 6.10352e-5f), 8, { 0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x04 } },
        { DXGI_FORMAT_R16G16B16A16_FLOAT, XMFLOAT4(1.00097656f, 1.00097656f, 1.00097656f, 1.0f), 8, { 0x01,0x3C,0x01,0x3C,0x01,0x3C,0x00,0x3C } },
        { DXGI_FORMAT_R16G16B16A16_FLOAT, XMFLOAT4(65504.f, 65504.f, 65504.f, 65504.f), 8, { 0xFF,0x7B,0xFF,0x7B,0xFF,0x7B,0xFF,0x7B } },
        { DXGI_FORMAT_R16G16B16A16_FLOAT, XMFLOAT4(-6.10352e-5f, -6.10352e-5f, -6.10352e-5f, -6.10352e-5f), 8, { 0x00,0x84,0x00,0x84,0x00,0x84,0x00,0x84 } },
        { DXGI_FORMAT_R16G16B16A16_FLOAT, XMFLOAT4(-65504.f, -65504.f, -65504.f, -65504.f), 8, { 0xFF,0xFB,0xFF,0xFB,0xFF,0xFB,0xFF,0xFB } },
        // DXGI_FORMAT_R16G16B16A16_UNORM
        { DXGI_FORMAT_R16G16B16A16_UNORM, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 8, { 0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80 } },
        { DXGI_FORMAT_R16G16B16A16_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 8, { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R16G16B16A16_UNORM, XMFLOAT4(0.25f, 0.5f, 0.75f, 1.0f), 8, { 0x00,0x40,0x00,0x80,0xFF,0xBF,0xFF,0xFF } },
        // DXGI_FORMAT_R16G16B16A16_UINT
        { DXGI_FORMAT_R16G16B16A16_UINT, XMFLOAT4(65535.f, 65535.f, 65535.f, 65535.f), 8, { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R16G16B16A16_UINT, XMFLOAT4(1.0f, 2.0f, 3.0f, 4.0f), 8, { 0x01,0x00,0x02,0x00,0x03,0x00,0x04,0x00 } },
        // DXGI_FORMAT_R16G16B16A16_SNORM
        { DXGI_FORMAT_R16G16B16A16_SNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 8, { 0xFF,0x7F,0xFF,0x7F,0xFF,0x7F,0xFF,0x7F } },
        { DXGI_FORMAT_R16G16B16A16_SNORM, XMFLOAT4(-1.0f, -1.0f, -1.0f, -1.0f), 8, { 0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80 } },
        #ifdef _M_ARM64
        { DXGI_FORMAT_R16G16B16A16_SNORM, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 8, { 0xFF,0x3F,0xFF,0x3F,0xFF,0x3F,0xFF,0x3F } },
        { DXGI_FORMAT_R16G16B16A16_SNORM, XMFLOAT4(0.25f, 0.5f, 0.75f, 1.0f), 8, { 0xFF,0x1F,0xFF,0x3F,0xFF,0x5F,0xFF,0x7F } },
        { DXGI_FORMAT_R16G16B16A16_SNORM, XMFLOAT4(-0.5f, -0.5f, -0.5f, -0.5f), 8, {0x01,0xC0,0x01,0xC0,0x01,0xC0,0x01,0xC0 } },
        { DXGI_FORMAT_R16G16B16A16_SNORM, XMFLOAT4(-0.25f, -0.5f, -0.75f, -1.0f), 8, { 0x01,0xE0,0x01,0xC0,0x01,0xA0,0x01,0x80 } },
        #else
        { DXGI_FORMAT_R16G16B16A16_SNORM, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 8, { 0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40 } },
        { DXGI_FORMAT_R16G16B16A16_SNORM, XMFLOAT4(0.25f, 0.5f, 0.75f, 1.0f), 8, { 0x00,0x20,0x00,0x40,0xFF,0x5F,0xFF,0x7F } },
        { DXGI_FORMAT_R16G16B16A16_SNORM, XMFLOAT4(-0.5f, -0.5f, -0.5f, -0.5f), 8, { 0x00,0xC0,0x00,0xC0,0x00,0xC0,0x00,0xC0 } },
        { DXGI_FORMAT_R16G16B16A16_SNORM, XMFLOAT4(-0.25f, -0.5f, -0.75f, -1.0f), 8, { 0x00,0xE0,0x00,0xC0,0x01,0xA0,0x01,0x80 } },
        #endif
        // DXGI_FORMAT_R16G16B16A16_SINT
        { DXGI_FORMAT_R16G16B16A16_SINT, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 8, { 0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00 } },
        { DXGI_FORMAT_R16G16B16A16_SINT, XMFLOAT4(-1.0f, -1.0f, -1.0f, -1.0f), 8, { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R16G16B16A16_SINT, XMFLOAT4(1.0f, 2.0f, 3.0f, 4.0f), 8, { 0x01,0x00,0x02,0x00,0x03,0x00,0x04,0x00 } },
        { DXGI_FORMAT_R16G16B16A16_SINT, XMFLOAT4(-1.0f, -2.0f, -3.0f, -4.0f), 8, { 0xFF,0xFF,0xFE,0xFF,0xFD,0xFF,0xFC,0xFF } },
        // DXGI_FORMAT_R32G32_FLOAT
        { DXGI_FORMAT_R32G32_FLOAT, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.f), 8, { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R32G32_FLOAT, XMFLOAT4(0.5f, 0.5f, 0.0f, 0.f), 8, { 0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x3F } },
        { DXGI_FORMAT_R32G32_FLOAT, XMFLOAT4(1.0f, 1.0f, 0.0f, 0.f), 8, { 0x00,0x00,0x80,0x3F,0x00,0x00,0x80,0x3F } },
        { DXGI_FORMAT_R32G32_FLOAT, XMFLOAT4(-1.0f, -1.0f, 0.0f, 0.f), 8, { 0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x80, 0xBF } },
        { DXGI_FORMAT_R32G32_FLOAT, XMFLOAT4(0.25f, 0.5f, 0.0f, 0.f), 8, { 0x00,0x00,0x80,0x3E,0x00,0x00,0x00,0x3F } },
        { DXGI_FORMAT_R32G32_FLOAT, XMFLOAT4(FLT_MIN, FLT_MIN, 0.0f, 0.f), 8, { 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00 } },
        { DXGI_FORMAT_R32G32_FLOAT, XMFLOAT4(FLT_MAX, FLT_MAX, 0.0f, 0.f), 8, { 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F } },
        { DXGI_FORMAT_R32G32_FLOAT, XMFLOAT4(-FLT_MIN, -FLT_MIN, 0.0f, 0.f), 8, { 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80 } },
        { DXGI_FORMAT_R32G32_FLOAT, XMFLOAT4(-FLT_MAX, -FLT_MAX, 0.0f, 0.f), 8, { 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF } },
        // DXGI_FORMAT_R32G32_UINT
        { DXGI_FORMAT_R32G32_UINT, XMFLOAT4(4294967296.f, 4294967296.f, 0.0f, 0.f), 8, { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R32G32_UINT, XMFLOAT4(1.0f, 2.0f, 0.0f, 0.f), 8, { 0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00 } },
        // DXGI_FORMAT_R32G32_SINT
        { DXGI_FORMAT_R32G32_SINT, XMFLOAT4(1.0f, 1.0f, 0.0f, 0.f), 8, { 0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R32G32_SINT, XMFLOAT4(-1.0f, -1.0f, 0.0f, 0.f), 8, { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R32G32_SINT, XMFLOAT4(1.0f, 2.0f, 0.0f, 0.f), 8, { 0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R32G32_SINT, XMFLOAT4(-1.0f, -2.0f, 0.0f, 0.f), 8, { 0xFF,0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF } },
        // DXGI_FORMAT_R10G10B10A2_UNORM
        { DXGI_FORMAT_R10G10B10A2_UNORM, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.666667f), 4, { 0xFF, 0xFD, 0xF7, 0x9F } },
        { DXGI_FORMAT_R10G10B10A2_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 4, { 0xFF, 0xFF, 0xFF, 0xFF } },
        { DXGI_FORMAT_R10G10B10A2_UNORM, XMFLOAT4(0.25f, 0.5f, 0.75f, 0.0f), 4, { 0xFF, 0xFC, 0xF7, 0x2F } },
        // DXGI_FORMAT_R10G10B10A2_UINT
        { DXGI_FORMAT_R10G10B10A2_UINT, XMFLOAT4(1023.f, 1023.f, 1023.f, 3.f), 4, { 0xFF, 0xFF, 0xFF, 0xFF } },
        { DXGI_FORMAT_R10G10B10A2_UINT, XMFLOAT4(1.0f, 2.0f, 3.0f, 3.0f), 4, { 0x01, 0x08, 0x30, 0xC0 } },
        // DXGI_FORMAT_R11G11B10_FLOAT
        { DXGI_FORMAT_R11G11B10_FLOAT, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.f), 4, { 0x80, 0x03, 0x1C, 0x70 } },
        { DXGI_FORMAT_R11G11B10_FLOAT, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.f), 4, { 0xC0, 0x03, 0x1E, 0x78 } },
        { DXGI_FORMAT_R11G11B10_FLOAT, XMFLOAT4(0.25f, 0.5f, 0.75f, 0.f), 4, { 0x40, 0x03, 0x1C, 0x74 } },
        { DXGI_FORMAT_R11G11B10_FLOAT, XMFLOAT4(6.10352e-5f, 6.10352e-5f, 6.10352e-5f, 0.f), 4, { 0x40, 0x00, 0x02, 0x08 } },
        { DXGI_FORMAT_R11G11B10_FLOAT, XMFLOAT4(65024.f, 65024.f, 64512.f, 0.f), 4, { 0xBF, 0xFF, 0xFD, 0xF7 } },
        // DXGI_FORMAT_R8G8B8A8_UNORM
        { DXGI_FORMAT_R8G8B8A8_UNORM, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 4, { 0x7F, 0x7F, 0x7F, 0x7F } },
        { DXGI_FORMAT_R8G8B8A8_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 4, { 0xFF, 0xFF, 0xFF, 0xFF } },
        { DXGI_FORMAT_R8G8B8A8_UNORM, XMFLOAT4(0.25f, 0.5f, 0.75f, 1.0f), 4, { 0x3F, 0x7F, 0xBF, 0xFF } },
        // DXGI_FORMAT_R8G8B8A8_UINT
        { DXGI_FORMAT_R8G8B8A8_UINT, XMFLOAT4(255.f, 255.f, 255.f, 255.f), 4, { 0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R8G8B8A8_UINT, XMFLOAT4(1.0f, 2.0f, 3.0f, 4.0f), 4, { 0x01,0x02,0x03,0x04 } },
        // DXGI_FORMAT_R8G8B8A8_SNORM
        { DXGI_FORMAT_R8G8B8A8_SNORM, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 4, { 0x3F,0x3F,0x3F,0x3F } },
        { DXGI_FORMAT_R8G8B8A8_SNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 4, { 0x7F,0x7F,0x7F,0x7F } },
        { DXGI_FORMAT_R8G8B8A8_SNORM, XMFLOAT4(0.25f, 0.5f, 0.75f, 1.0f), 4, { 0x1F,0x3F,0x5F,0x7F} },
        { DXGI_FORMAT_R8G8B8A8_SNORM, XMFLOAT4(-1.0f, -1.0f, -1.0f, -1.0f), 4, { 0x81,0x81,0x81,0x81 } },
#ifdef _M_ARM64
        { DXGI_FORMAT_R8G8B8A8_SNORM, XMFLOAT4(-0.5f, -0.5f, -0.5f, -0.5f), 4, { 0xC1,0xC1,0xC1,0xC1 } },
        { DXGI_FORMAT_R8G8B8A8_SNORM, XMFLOAT4(-0.25f, -0.5f, -0.75f, -1.0f), 4, { 0xE1,0xC1,0xA1,0x81 } },
#else
        { DXGI_FORMAT_R8G8B8A8_SNORM, XMFLOAT4(-0.5f, -0.5f, -0.5f, -0.5f), 4, { 0xC1,0xC0,0xC0,0xC0 } },
        { DXGI_FORMAT_R8G8B8A8_SNORM, XMFLOAT4(-0.25f, -0.5f, -0.75f, -1.0f), 4, { 0xE1,0xC0,0xA0,0x81 } },
#endif
        // DXGI_FORMAT_R8G8B8A8_SINT
        { DXGI_FORMAT_R8G8B8A8_SINT, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 4, { 0x01,0x01,0x01,0x01 } },
        { DXGI_FORMAT_R8G8B8A8_SINT, XMFLOAT4(-1.0f, -1.0f, -1.0f, -1.0f), 4, { 0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R8G8B8A8_SINT, XMFLOAT4(1.0f, 2.0f, 3.0f, 4.0f), 4, { 0x01,0x02,0x03,0x04 } },
        { DXGI_FORMAT_R8G8B8A8_SINT, XMFLOAT4(-1.0f, -2.0f, -3.0f, -4.0f), 4, { 0xFF,0xFE,0xFD,0xFC } },
        // DXGI_FORMAT_R16G16_FLOAT
        { DXGI_FORMAT_R16G16_FLOAT, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.f), 4, { 0x00,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R16G16_FLOAT, XMFLOAT4(0.25f, 0.25f, 0.0f, 0.f), 4, { 0x00,0x34,0x00,0x34 } },
        { DXGI_FORMAT_R16G16_FLOAT, XMFLOAT4(0.5f, 0.5f, 0.0f, 0.f), 4, { 0x00,0x38,0x00,0x38 } },
        { DXGI_FORMAT_R16G16_FLOAT, XMFLOAT4(1.0f, 1.0f, 0.0f, 0.f), 4, { 0x00,0x3C,0x00,0x3C } },
        { DXGI_FORMAT_R16G16_FLOAT, XMFLOAT4(-1.0f, -1.0f, 0.0f, 0.f), 4, { 0x00,0xBC,0x00,0xBC } },
        { DXGI_FORMAT_R16G16_FLOAT, XMFLOAT4(0.25f, 0.5f, 0.0f, 0.f), 4, { 0x00,0x34,0x00,0x38 } },
        { DXGI_FORMAT_R16G16_FLOAT, XMFLOAT4(5.96046448e-8f, 5.96046448e-8f, 0.0f, 0.f), 4, { 0x01,0x00,0x01,0x00 } },
        { DXGI_FORMAT_R16G16_FLOAT, XMFLOAT4(6.09755516e-5f, 6.09755516e-5f, 0.0f, 0.f), 4, { 0xFF,0x03,0xFF,0x03 } },
        { DXGI_FORMAT_R16G16_FLOAT, XMFLOAT4(6.10352e-5f, 6.10352e-5f, 0.0f, 0.f), 4, { 0x00,0x04,0x00,0x04 } },
        { DXGI_FORMAT_R16G16_FLOAT, XMFLOAT4(1.00097656f, 1.00097656f, 0.0f, 0.f), 4, { 0x01,0x3C,0x01,0x3C } },
        { DXGI_FORMAT_R16G16_FLOAT, XMFLOAT4(65504.f, 65504.f, 0.0f, 0.f), 4, { 0xFF,0x7B,0xFF,0x7B } },
        { DXGI_FORMAT_R16G16_FLOAT, XMFLOAT4(-6.10352e-5f, -6.10352e-5f, 0.0f, 0.f), 4, { 0x00,0x84,0x00,0x84 } },
        { DXGI_FORMAT_R16G16_FLOAT, XMFLOAT4(-65504.f, -65504.f, 0.0f, 0.f), 4, { 0xFF,0xFB,0xFF,0xFB } },
        // DXGI_FORMAT_R16G16_UNORM
        { DXGI_FORMAT_R16G16_UNORM, XMFLOAT4(0.5f, 0.5f, 0.0f, 0.f), 4, { 0x00,0x80,0x00,0x80 } },
        { DXGI_FORMAT_R16G16_UNORM, XMFLOAT4(1.0f, 1.0f, 0.0f, 0.f), 4, { 0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R16G16_UNORM, XMFLOAT4(0.25f, 0.5f, 0.0f, 0.f), 4, { 0x00,0x40,0x00,0x80 } },
        // DXGI_FORMAT_R16G16_UINT
        { DXGI_FORMAT_R16G16_UINT, XMFLOAT4(65535.f, 65535.f, 0.f, 0.f), 4, { 0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R16G16_UINT, XMFLOAT4(1.0f, 2.0f, 0.0f, 0.f), 4, { 0x01,0x00,0x02,0x00 } },
        // DXGI_FORMAT_R16G16_SNORM
        { DXGI_FORMAT_R16G16_SNORM, XMFLOAT4(1.0f, 1.0f, 0.0f, 0.f), 4, { 0xFF,0x7F,0xFF,0x7F } },
        { DXGI_FORMAT_R16G16_SNORM, XMFLOAT4(-1.0f, -1.0f, 0.0f, 0.f), 4, { 0x01,0x80,0x01,0x80 } },
#ifdef _M_ARM64
        { DXGI_FORMAT_R16G16_SNORM, XMFLOAT4(0.5f, 0.5f, 0.0f, 0.f), 4, { 0xFF,0x3F,0xFF,0x3F } },
        { DXGI_FORMAT_R16G16_SNORM, XMFLOAT4(0.25f, 0.5f, 0.0f, 0.f), 4, { 0xFF,0x1F,0xFF,0x3F } },
        { DXGI_FORMAT_R16G16_SNORM, XMFLOAT4(-0.5f, -0.5f, 0.0f, 0.f), 4, { 0x01,0xC0,0x01,0xC0 } },
        { DXGI_FORMAT_R16G16_SNORM, XMFLOAT4(-0.25f, -0.5f, 0.0f, 0.f), 4, { 0x01,0xE0,0x01,0xC0 } },
#else
        { DXGI_FORMAT_R16G16_SNORM, XMFLOAT4(0.5f, 0.5f, 0.0f, 0.f), 4, { 0x00,0x40,0x00,0x40 } },
        { DXGI_FORMAT_R16G16_SNORM, XMFLOAT4(0.25f, 0.5f, 0.0f, 0.f), 4, { 0x00,0x20,0x00,0x40 } },
        { DXGI_FORMAT_R16G16_SNORM, XMFLOAT4(-0.5f, -0.5f, 0.0f, 0.f), 4, { 0x00,0xC0,0x00,0xC0 } },
        { DXGI_FORMAT_R16G16_SNORM, XMFLOAT4(-0.25f, -0.5f, 0.0f, 0.f), 4, { 0x00,0xE0,0x00,0xC0 } },
#endif
        // DXGI_FORMAT_R16G16_SINT
        { DXGI_FORMAT_R16G16_SINT, XMFLOAT4(1.0f, 1.0f, 0.0f, 0.f), 4, { 0x01,0x00,0x01,0x00 } },
        { DXGI_FORMAT_R16G16_SINT, XMFLOAT4(-1.0f, -1.0f, 0.0f, 0.f), 4, { 0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R16G16_SINT, XMFLOAT4(1.0f, 2.0f, 0.0f, 0.f), 4, { 0x01,0x00,0x02,0x00 } },
        { DXGI_FORMAT_R16G16_SINT, XMFLOAT4(-1.0f, -2.0f, 0.0f, 0.f), 4, { 0xFF,0xFF,0xFE,0xFF } },
        // DXGI_FORMAT_R32_FLOAT
        { DXGI_FORMAT_R32_FLOAT, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.f), 4, { 0x00,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R32_FLOAT, XMFLOAT4(0.5f, 0.0f, 0.0f, 0.f), 4, { 0x00,0x00,0x00,0x3F } },
        { DXGI_FORMAT_R32_FLOAT, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.f), 4, { 0x00,0x00,0x80,0x3F } },
        { DXGI_FORMAT_R32_FLOAT, XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.f), 4, { 0x00,0x00,0x80,0xBF } },
        { DXGI_FORMAT_R32_FLOAT, XMFLOAT4(0.25f, 0.0f, 0.0f, 0.f), 4, { 0x00,0x00,0x80,0x3E } },
        { DXGI_FORMAT_R32_FLOAT, XMFLOAT4(FLT_MIN, 0.0f, 0.0f, 0.f), 4, { 0x00,0x00,0x80,0x00 } },
        { DXGI_FORMAT_R32_FLOAT, XMFLOAT4(FLT_MAX, 0.0f, 0.0f, 0.f), 4, { 0xFF,0xFF,0x7F,0x7F } },
        { DXGI_FORMAT_R32_FLOAT, XMFLOAT4(-FLT_MIN, 0.0f, 0.0f, 0.f), 4, { 0x00,0x00,0x80,0x80 } },
        { DXGI_FORMAT_R32_FLOAT, XMFLOAT4(-FLT_MAX, 0.0f, 0.0f, 0.f), 4, { 0xFF,0xFF,0x7F,0xFF } },
        // DXGI_FORMAT_R32_UINT
        { DXGI_FORMAT_R32_UINT, XMFLOAT4(4294967296.f, 0.0f, 0.0f, 0.f), 4, { 0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R32_UINT, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.f), 4, { 0x01,0x00,0x00,0x00 } },
        // DXGI_FORMAT_R32_SINT
        { DXGI_FORMAT_R32_SINT, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.f), 4, { 0x01,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R32_SINT, XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.f), 4, { 0xFF,0xFF,0xFF,0xFF } },
        // DXGI_FORMAT_R8G8_UNORM
        { DXGI_FORMAT_R8G8_UNORM, XMFLOAT4(0.5f, 0.5f, 0.0f, 0.f), 2, { 0x80, 0x80 } },
        { DXGI_FORMAT_R8G8_UNORM, XMFLOAT4(1.0f, 1.0f, 0.0f, 0.f), 2, { 0xFF, 0xFF } },
        { DXGI_FORMAT_R8G8_UNORM, XMFLOAT4(0.25f, 0.5f, 0.0f, 0.f), 2, { 0x40, 0x80 } },
        // DXGI_FORMAT_R8G8_UINT
        { DXGI_FORMAT_R8G8_UINT, XMFLOAT4(255.f, 255.f, 0.0f, 0.f), 2, { 0xFF,0xFF } },
        { DXGI_FORMAT_R8G8_UINT, XMFLOAT4(1.0f, 2.0f, 0.0f, 0.f), 2, { 0x01,0x02 } },
        // DXGI_FORMAT_R8G8_SNORM
        { DXGI_FORMAT_R8G8_SNORM, XMFLOAT4(1.0f, 1.0f, 0.0f, 0.f), 2, { 0x7F,0x7F } },
        { DXGI_FORMAT_R8G8_SNORM, XMFLOAT4(-1.0f, -1.0f, 0.0f, 0.f), 2, { 0x81,0x81 } },
#ifdef _M_ARM64
        { DXGI_FORMAT_R8G8_SNORM, XMFLOAT4(0.5f, 0.5f, 0.0f, 0.f), 2, { 0x3F,0x3F } },
        { DXGI_FORMAT_R8G8_SNORM, XMFLOAT4(0.25f, 0.5f, 0.0f, 0.f), 2, { 0x1F,0x3F } },
        { DXGI_FORMAT_R8G8_SNORM, XMFLOAT4(-0.5f, -0.5f, 0.0f, 0.f), 2, { 0xC1,0xC1 } },
        { DXGI_FORMAT_R8G8_SNORM, XMFLOAT4(-0.25f, -0.5f, 0.0f, 0.f), 2, { 0xE1,0xC1 } },
#else
        { DXGI_FORMAT_R8G8_SNORM, XMFLOAT4(0.5f, 0.5f, 0.0f, 0.f), 2, { 0x40,0x40 } },
        { DXGI_FORMAT_R8G8_SNORM, XMFLOAT4(0.25f, 0.5f, 0.0f, 0.f), 2, { 0x20,0x40 } },
        { DXGI_FORMAT_R8G8_SNORM, XMFLOAT4(-0.5f, -0.5f, 0.0f, 0.f), 2, { 0xC0,0xC0 } },
        { DXGI_FORMAT_R8G8_SNORM, XMFLOAT4(-0.25f, -0.5f, 0.0f, 0.f), 2, { 0xE0,0xC0 } },
#endif
        // DXGI_FORMAT_R8G8_SINT
        { DXGI_FORMAT_R8G8_SINT, XMFLOAT4(1.0f, 1.0f, 0.0f, 0.f), 2, { 0x01,0x01 } },
        { DXGI_FORMAT_R8G8_SINT, XMFLOAT4(-1.0f, -1.0f, 0.0f, 0.f), 2, { 0xFF,0xFF } },
        { DXGI_FORMAT_R8G8_SINT, XMFLOAT4(1.0f, 2.0f, 0.0f, 0.f), 2, { 0x01,0x02 } },
        { DXGI_FORMAT_R8G8_SINT, XMFLOAT4(-1.0f, -2.0f, 0.0f, 0.f), 2, { 0xFF,0xFE } },
        // DXGI_FORMAT_R16_FLOAT
        { DXGI_FORMAT_R16_FLOAT, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.f), 2, { 0x00,0x00 } },
        { DXGI_FORMAT_R16_FLOAT, XMFLOAT4(0.25f, 0.0f, 0.0f, 0.f), 2, { 0x00,0x34 } },
        { DXGI_FORMAT_R16_FLOAT, XMFLOAT4(0.5f, 0.0f, 0.0f, 0.f), 2, { 0x00,0x38 } },
        { DXGI_FORMAT_R16_FLOAT, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.f), 2, { 0x00,0x3C } },
        { DXGI_FORMAT_R16_FLOAT, XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.f), 2, { 0x00,0xBC } },
        { DXGI_FORMAT_R16_FLOAT, XMFLOAT4(5.96046448e-8f, 0.0f, 0.0f, 0.f), 2, { 0x01, 0x00 } },
        { DXGI_FORMAT_R16_FLOAT, XMFLOAT4(6.09755516e-5f, 0.0f, 0.0f, 0.f), 2, { 0xFF, 0x03 } },
        { DXGI_FORMAT_R16_FLOAT, XMFLOAT4(6.10352e-5f, 0.0f, 0.0f, 0.f), 2, { 0x00,0x04 } },
        { DXGI_FORMAT_R16_FLOAT, XMFLOAT4(1.00097656f, 0.0f, 0.0f, 0.f), 2, { 0x01, 0x3C } },
        { DXGI_FORMAT_R16_FLOAT, XMFLOAT4(65504.f, 0.0f, 0.0f, 0.f), 2, { 0xFF,0x7B } },
        { DXGI_FORMAT_R16_FLOAT, XMFLOAT4(-6.10352e-5f, 0.0f, 0.0f, 0.f), 2, { 0x00,0x84 } },
        { DXGI_FORMAT_R16_FLOAT, XMFLOAT4(-65504.f, 0.0f, 0.0f, 0.f), 2, { 0xFF,0xFB } },
        // DXGI_FORMAT_R16_UNORM
        { DXGI_FORMAT_R16_UNORM, XMFLOAT4(0.5f, 0.0f, 0.0f, 0.f), 2, { 0x00,0x80 } },
        { DXGI_FORMAT_R16_UNORM, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.f), 2, { 0xFF,0xFF } },
        { DXGI_FORMAT_R16_UNORM, XMFLOAT4(0.25f, 0.0f, 0.0f, 0.f), 2, { 0x00,0x40 } },
        // DXGI_FORMAT_R16_UINT
        { DXGI_FORMAT_R16_UINT, XMFLOAT4(65535.f, 0.0f, 0.0f, 0.f), 2, { 0xFF,0xFF } },
        { DXGI_FORMAT_R16_UINT, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.f), 2, { 0x01,0x00 } },
        // DXGI_FORMAT_R16_SNORM
        { DXGI_FORMAT_R16_SNORM, XMFLOAT4(0.5f, 0.0f, 0.0f, 0.f), 2, { 0xFF,0x3F } },
        { DXGI_FORMAT_R16_SNORM, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.f), 2, { 0xFF,0x7F } },
        { DXGI_FORMAT_R16_SNORM, XMFLOAT4(0.25f, 0.0f, 0.0f, 0.f), 2, { 0xFF,0x1F } },
        { DXGI_FORMAT_R16_SNORM, XMFLOAT4(-0.5f, 0.0f, 0.0f, 0.f), 2, { 0x01,0xC0 } },
        { DXGI_FORMAT_R16_SNORM, XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.f), 2, { 0x01,0x80 } },
        { DXGI_FORMAT_R16_SNORM, XMFLOAT4(-0.25f, 0.0f, 0.0f, 0.f), 2, { 0x01,0xE0 } },
        // DXGI_FORMAT_R16_SINT
        { DXGI_FORMAT_R16_SINT, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.f), 2, { 0x01,0x00 } },
        { DXGI_FORMAT_R16_SINT, XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.f), 2, { 0xFF,0xFF } },
        // DXGI_FORMAT_R8_UNORM
        { DXGI_FORMAT_R8_UNORM, XMFLOAT4(0.5f, 0.0f, 0.0f, 0.f), 1, { 0x7F } },
        { DXGI_FORMAT_R8_UNORM, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.f), 1, { 0xFF } },
        { DXGI_FORMAT_R8_UNORM, XMFLOAT4(0.25f, 0.0f, 0.0f, 0.f), 1, { 0x3F } },
        // DXGI_FORMAT_R8_UINT
        { DXGI_FORMAT_R8_UINT, XMFLOAT4(255.f, 0.0f, 0.0f, 0.f), 1, { 0xFF } },
        { DXGI_FORMAT_R8_UINT, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.f), 1, { 0x01 } },
        // DXGI_FORMAT_R8_SNORM
        { DXGI_FORMAT_R8_SNORM, XMFLOAT4(0.5f, 0.0f, 0.0f, 0.f), 1, { 0x3F } },
        { DXGI_FORMAT_R8_SNORM, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.f), 1, { 0x7F } },
        { DXGI_FORMAT_R8_SNORM, XMFLOAT4(0.25f, 0.0f, 0.0f, 0.f), 1, { 0x1F } },
        { DXGI_FORMAT_R8_SNORM, XMFLOAT4(-0.5f, 0.0f, 0.0f, 0.f), 1, { 0xC1 } },
        { DXGI_FORMAT_R8_SNORM, XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.f), 1, { 0x81 } },
        { DXGI_FORMAT_R8_SNORM, XMFLOAT4(-0.25f, 0.0f, 0.0f, 0.f), 1, { 0xE1 } },
        // DXGI_FORMAT_R8_SINT
        { DXGI_FORMAT_R8_SINT, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.f), 1, { 0x01 } },
        { DXGI_FORMAT_R8_SINT, XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.f), 1, { 0xFF } },
        // DXGI_FORMAT_B5G6R5_UNORM
        { DXGI_FORMAT_B5G6R5_UNORM, XMFLOAT4(0.516129f, 0.507937f, 0.516129f, 0.f), 2, { 0x10, 0x84 } },
        { DXGI_FORMAT_B5G6R5_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.f), 2, { 0xFF, 0xFF } },
#ifdef _M_ARM64
        { DXGI_FORMAT_B5G6R5_UNORM, XMFLOAT4(0.225806f, 0.507937f, 0.74193f, 0.f), 2, { 0x17, 0x3C } },
#else
        { DXGI_FORMAT_B5G6R5_UNORM, XMFLOAT4(0.25f, 0.507937f, 0.75f, 0.f), 2, { 0x17, 0x44 } },
#endif
        // DXGI_FORMAT_B5G5R5A1_UNORM
        { DXGI_FORMAT_B5G5R5A1_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 2, { 0xFF, 0xFF } },
#ifdef _M_ARM64
        { DXGI_FORMAT_B5G5R5A1_UNORM, XMFLOAT4(0.516129f, 0.516129f, 0.516129f, 1.f), 2, { 0x10, 0xC2 } },
        { DXGI_FORMAT_B5G5R5A1_UNORM, XMFLOAT4(0.258065f, 0.516129f, 0.709667f, 1.0f), 2, { 0x16, 0xA2 } },
#else
        { DXGI_FORMAT_B5G5R5A1_UNORM, XMFLOAT4(0.548387f, 0.548387f, 0.548387f, 1.f), 2, { 0x31, 0xC6 } },
        { DXGI_FORMAT_B5G5R5A1_UNORM, XMFLOAT4(0.266667f, 0.548387f, 0.733333f, 1.0f), 2, { 0x37, 0xA2 } },
#endif
        // DXGI_FORMAT_B8G8R8A8_UNORM
        { DXGI_FORMAT_B8G8R8A8_UNORM, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 4, { 0x7F, 0x7F, 0x7F, 0x7F } },
        { DXGI_FORMAT_B8G8R8A8_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 4, { 0xFF, 0xFF, 0xFF, 0xFF } },
        { DXGI_FORMAT_B8G8R8A8_UNORM, XMFLOAT4(0.25f, 0.5f, 0.75f, 1.0f), 4, { 0xBF, 0x7F, 0x3F, 0xFF } },
        // DXGI_FORMAT_B8G8R8X8_UNORM
        { DXGI_FORMAT_B8G8R8X8_UNORM, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.f), 4, { 0x7F, 0x7F, 0x7F, 0x0 } },
        { DXGI_FORMAT_B8G8R8X8_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.f), 4, { 0xFF, 0xFF, 0xFF, 0x0 } },
        { DXGI_FORMAT_B8G8R8X8_UNORM, XMFLOAT4(0.25f, 0.5f, 0.75f, 0.f), 4, { 0xBF, 0x7F, 0x3F, 0x0 } },
        // DXGI_FORMAT_B4G4R4A4_UNORM
        { DXGI_FORMAT_B4G4R4A4_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 2, { 0xFF, 0xFF } },
#ifdef _M_ARM64
        { DXGI_FORMAT_B4G4R4A4_UNORM, XMFLOAT4(0.466667f, 0.466667f, 0.466667f, 0.466667f), 2, { 0x77, 0x77 } },
        { DXGI_FORMAT_B4G4R4A4_UNORM, XMFLOAT4(0.266667f, 0.466667f, 0.666667f, 1.0f), 2, { 0x7A, 0xF4 } },
#else
        { DXGI_FORMAT_B4G4R4A4_UNORM, XMFLOAT4(0.533333f, 0.533333f, 0.533333f, 0.533333f), 2, { 0x88, 0x88 } },
        { DXGI_FORMAT_B4G4R4A4_UNORM, XMFLOAT4(0.266667f, 0.533333f, 0.733333f, 1.0f), 2, { 0x8B, 0xF4 } },
#endif
        // XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM [Xbox]
        { XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 4, { 0xFF, 0xFD, 0xF7, 0xDF } },
        { XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM, XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), 4, { 0x01, 0x06, 0x18, 0xE0 } },
#ifdef _M_ARM64
        { XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.666667f), 4, { 0xFF, 0xFC, 0xF3, 0x8F } },
        { XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM, XMFLOAT4(0.25f, 0.5f, 0.75f, 0.0f), 4, { 0x7F, 0xFC, 0xF3, 0x17 } },
        { XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM, XMFLOAT4(-0.500978f, -0.500978f, -0.500978f, 0.666667f), 4, { 0x01, 0x03, 0x0C, 0xB0 } },
        { XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM, XMFLOAT4(-0.25f, -0.5f, -0.75f, 0.0f), 4, { 0x81, 0x03, 0x0C, 0x28 } },
#else
        { XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.666667f), 4, { 0x00, 0xFD, 0xF3, 0x8F } },
        { XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM, XMFLOAT4(0.25f, 0.5f, 0.75f, 0.0f), 4, { 0x80, 0xFC, 0xF3, 0x17 } },
        { XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM, XMFLOAT4(-0.500978f, -0.500978f, -0.500978f, 0.666667f), 4, { 0x00, 0x03, 0x0C, 0xB0 } },
        { XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM, XMFLOAT4(-0.25f, -0.5f, -0.75f, 0.0f), 4, { 0x80, 0x03, 0x0C, 0x28 } },
#endif
    };

    const TestVB g_TestVBs_x2Bias[] =
    {
        // DXGI_FORMAT_R16G16B16A16_UNORM
        { DXGI_FORMAT_R16G16B16A16_UNORM, XMFLOAT4(-1.0f, -1.0f, -1.0f, -1.0f), 8, { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R16G16B16A16_UNORM, XMFLOAT4(-0.5f, -0.5f, -0.5f, -0.5f), 8, { 0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40 } },
        { DXGI_FORMAT_R16G16B16A16_UNORM, XMFLOAT4(0.f, 0.f, 0.f, 0.f), 8, { 0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80 } },
        { DXGI_FORMAT_R16G16B16A16_UNORM, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 8, { 0xFF,0xBF,0xFF,0xBF,0xFF,0xBF,0xFF,0xBF } },
        { DXGI_FORMAT_R16G16B16A16_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 8, { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R16G16B16A16_UNORM, XMFLOAT4(-0.25f, 0.5f, -0.75f, 1.0f), 8, { 0x00,0x60,0xFF,0xBF,0x00,0x20,0xFF,0xFF } },
        // DXGI_FORMAT_R10G10B10A2_UNORM
        { DXGI_FORMAT_R10G10B10A2_UNORM, XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.666667f), 4, { 0x00,0x00,0x00,0x80 } },
        { DXGI_FORMAT_R10G10B10A2_UNORM, XMFLOAT4(-0.5f, -0.5f, -0.5f, 0.666667f), 4, { 0xFF,0xFC,0xF3,0x8F } },
        { DXGI_FORMAT_R10G10B10A2_UNORM, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.666667f), 4, { 0xFF,0xFE,0xFB,0xAF } },
        { DXGI_FORMAT_R10G10B10A2_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 4, { 0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R10G10B10A2_UNORM, XMFLOAT4(-0.25f, 0.5f, -0.75f, 0.0f), 4, { 0x7F,0xFD,0xFB,0x07 } },
        // DXGI_FORMAT_R11G11B10_FLOAT
        { DXGI_FORMAT_R11G11B10_FLOAT, XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.f), 4, { 0x00,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R11G11B10_FLOAT, XMFLOAT4(-0.5f, -0.5f, -0.5f, 0.f), 4, { 0x40,0x03,0x1A,0x68 } },
        { DXGI_FORMAT_R11G11B10_FLOAT, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.f), 4, { 0x80,0x03,0x1C,0x70 } },
        { DXGI_FORMAT_R11G11B10_FLOAT, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.f), 4, { 0xA0,0x03,0x1D,0x74 } },
        { DXGI_FORMAT_R11G11B10_FLOAT, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.f), 4, { 0xC0,0x03,0x1E,0x78 } },
        { DXGI_FORMAT_R11G11B10_FLOAT, XMFLOAT4(-.25, 0.5f, -0.75f, 0.f), 4, { 0x60,0x03,0x1D,0x60 } },
        // DXGI_FORMAT_R8G8B8A8_UNORM
        { DXGI_FORMAT_R8G8B8A8_UNORM, XMFLOAT4(-1.0f, -1.0f, -1.0f, -1.f), 4, { 0x00,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R8G8B8A8_UNORM, XMFLOAT4(-0.5f, -0.5f, -0.5f, -0.5f), 4, { 0x3F,0x3F,0x3F,0x3F } },
        { DXGI_FORMAT_R8G8B8A8_UNORM, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.f), 4, { 0x7F, 0x7F, 0x7F, 0x7F } },
        { DXGI_FORMAT_R8G8B8A8_UNORM, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 4, { 0xBF,0xBF,0xBF,0xBF } },
        { DXGI_FORMAT_R8G8B8A8_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 4, { 0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R8G8B8A8_UNORM, XMFLOAT4(-0.25f, 0.5f, -0.75f, 1.0f), 4, { 0x5F,0xBF,0x1F,0xFF } },
        // DXGI_FORMAT_R16G16_UNORM
        { DXGI_FORMAT_R16G16_UNORM, XMFLOAT4(-1.0f, -1.0f, 0.0f, 0.f), 4, { 0x00,0x00,0x00,0x00 } },
        { DXGI_FORMAT_R16G16_UNORM, XMFLOAT4(-0.5f, -0.5f, 0.0f, 0.f), 4, { 0x00,0x40,0x00,0x40 } },
        { DXGI_FORMAT_R16G16_UNORM, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.f), 4, { 0x00,0x80,0x00,0x80 } },
        { DXGI_FORMAT_R16G16_UNORM, XMFLOAT4(1.0f, 1.0f, 0.0f, 0.f), 4, { 0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_R16G16_UNORM, XMFLOAT4(-0.25f, 0.5f, 0.0f, 0.f), 4, { 0x00,0x60,0xFF,0xBF } },
        // DXGI_FORMAT_R8G8_UNORM
        { DXGI_FORMAT_R8G8_UNORM, XMFLOAT4(-1.0f, -1.0f, 0.0f, 0.f), 2, { 0x00,0x00 } },
        { DXGI_FORMAT_R8G8_UNORM, XMFLOAT4(-0.5f, -0.5f, 0.0f, 0.f), 2, { 0x40,0x40 } },
        { DXGI_FORMAT_R8G8_UNORM, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.f), 2, { 0x80,0x80 } },
        { DXGI_FORMAT_R8G8_UNORM, XMFLOAT4(1.0f, 1.0f, 0.0f, 0.f), 2, { 0xFF, 0xFF } },
        { DXGI_FORMAT_R8G8_UNORM, XMFLOAT4(0.25f, -0.5f, 0.0f, 0.f), 2, { 0x9F,0x40 } },
        // DXGI_FORMAT_R16_UNORM
        { DXGI_FORMAT_R16_UNORM, XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.f), 2, { 0x00,0x00 } },
        { DXGI_FORMAT_R16_UNORM, XMFLOAT4(-0.5f, 0.0f, 0.0f, 0.f), 2, { 0x00,0x40 } },
        { DXGI_FORMAT_R16_UNORM, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.f), 2, { 0x00,0x80 } },
        { DXGI_FORMAT_R16_UNORM, XMFLOAT4(0.5f, 0.0f, 0.0f, 0.f), 2, { 0xFF,0xBF } },
        { DXGI_FORMAT_R16_UNORM, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.f), 2, { 0xFF,0xFF } },
        { DXGI_FORMAT_R16_UNORM, XMFLOAT4(0.25f, 0.0f, 0.0f, 0.f), 2, { 0xFF,0x9F } },
        // DXGI_FORMAT_R8_UNORM
        { DXGI_FORMAT_R8_UNORM, XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.f), 1, { 0x00 } },
        { DXGI_FORMAT_R8_UNORM, XMFLOAT4(-0.5f, 0.0f, 0.0f, 0.f), 1, { 0x3F } },
        { DXGI_FORMAT_R8_UNORM, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.f), 1, { 0x7F } },
        { DXGI_FORMAT_R8_UNORM, XMFLOAT4(0.5f, 0.0f, 0.0f, 0.f), 1, { 0xBF } },
        { DXGI_FORMAT_R8_UNORM, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.f), 1, { 0xFF } },
        { DXGI_FORMAT_R8_UNORM, XMFLOAT4(0.25f, 0.0f, 0.0f, 0.f), 1, { 0x9F } },
        // DXGI_FORMAT_B5G6R5_UNORM
        { DXGI_FORMAT_B5G6R5_UNORM, XMFLOAT4(-1.f, -1.f, -1.f, 0.f), 2, { 0x00,0x00 } },
        { DXGI_FORMAT_B5G6R5_UNORM, XMFLOAT4(-0.483871f, -0.523809f, -0.483871f, 0.f), 2, { 0xE8,0x41 } },
        { DXGI_FORMAT_B5G6R5_UNORM, XMFLOAT4(0.548387f, 0.523810f, 0.548387f, 0.f), 2, { 0x18,0xC6 } },
        { DXGI_FORMAT_B5G6R5_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.f), 2, { 0xFF, 0xFF } },
#ifdef _M_ARM64
        { DXGI_FORMAT_B5G6R5_UNORM, XMFLOAT4(0.032258f, -0.015873f, 0.032258f, 0.f), 2, { 0xF0,0x83 } },
        { DXGI_FORMAT_B5G6R5_UNORM, XMFLOAT4(0.161290f, -0.523809f, 0.677419f, 0.f), 2, { 0xFA,0x91 } },
#else
        { DXGI_FORMAT_B5G6R5_UNORM, XMFLOAT4(0.032258f, 0.015873f, 0.032258f, 0.f), 2, { 0x10,0x84 } },
        { DXGI_FORMAT_B5G6R5_UNORM, XMFLOAT4(0.225806f, -0.523809f, 0.741935f, 0.f), 2, { 0xFB,0x99 } },
#endif
        // DXGI_FORMAT_B5G5R5A1_UNORM
        { DXGI_FORMAT_B5G5R5A1_UNORM, XMFLOAT4(-1.f, -1.f, -1.f, 0.f), 2, { 0x00,0x00 } },
        { DXGI_FORMAT_B5G5R5A1_UNORM, XMFLOAT4(-0.483871f, -0.483871f, -0.483871f, 0.f), 2, { 0x08,0x21 } },
        { DXGI_FORMAT_B5G5R5A1_UNORM, XMFLOAT4(0.032258f, 0.032258f, 0.032258f, 0.f), 2, { 0x10,0x42} },
        { DXGI_FORMAT_B5G5R5A1_UNORM, XMFLOAT4(0.483871f, 0.483871f, 0.483871f, 1.f), 2, { 0xF7,0xDE } },
        { DXGI_FORMAT_B5G5R5A1_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 2, { 0xFF,0xFF } },
#ifdef _M_ARM64
        { DXGI_FORMAT_B5G5R5A1_UNORM, XMFLOAT4(0.290323f, -0.548387f, 0.677419f, 1.0f), 2, { 0xFA,0xD0 } },
#else
        { DXGI_FORMAT_B5G5R5A1_UNORM, XMFLOAT4(0.290323f, -0.548387f, 0.741935f, 1.0f), 2, { 0xFB,0xD0 } },
#endif
        // DXGI_FORMAT_B8G8R8A8_UNORM
        { DXGI_FORMAT_R8G8B8A8_UNORM, XMFLOAT4(-1.0f, -1.0f, -1.0f, -1.f), 4, { 0x00,0x00,0x00,0x00 } },
        { DXGI_FORMAT_B8G8R8A8_UNORM, XMFLOAT4(-0.5f, -0.5f, -0.5f, -0.5f), 4, { 0x3F,0x3F,0x3F,0x3F } },
        { DXGI_FORMAT_B8G8R8A8_UNORM, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.f), 4, { 0x7F, 0x7F, 0x7F, 0x7F } },
        { DXGI_FORMAT_B8G8R8A8_UNORM, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 4, { 0xBF,0xBF,0xBF,0xBF } },
        { DXGI_FORMAT_R8G8B8A8_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 4, { 0xFF,0xFF,0xFF,0xFF } },
        { DXGI_FORMAT_B8G8R8A8_UNORM, XMFLOAT4(-0.25f, 0.5f, -0.75f, 1.0f), 4, { 0x1F,0xBF,0x5F,0xFF } },
        // DXGI_FORMAT_B8G8R8X8_UNORM
        { DXGI_FORMAT_B8G8R8X8_UNORM, XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.f), 4, { 0x00,0x00,0x00,0x00 } },
        { DXGI_FORMAT_B8G8R8X8_UNORM, XMFLOAT4(-0.5f, -0.5f, -0.5f, 0.f), 4, { 0x3F,0x3F,0x3F,0x00 } },
        { DXGI_FORMAT_B8G8R8X8_UNORM, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.f), 4, { 0x7F,0x7F,0x7F,0x00 } },
        { DXGI_FORMAT_B8G8R8X8_UNORM, XMFLOAT4(0.5f, 0.5f, 0.5f, 0.f), 4, { 0xBF,0xBF,0xBF,0x00 } },
        { DXGI_FORMAT_B8G8R8X8_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.f), 4, { 0xFF,0xFF,0xFF,0x00 } },
        { DXGI_FORMAT_B8G8R8X8_UNORM, XMFLOAT4(-0.25f, 0.5f, -0.75f, 0.f), 4, { 0x1F,0xBF,0x5F,0x00 } },
        // DXGI_FORMAT_B4G4R4A4_UNORM
        { DXGI_FORMAT_B4G4R4A4_UNORM, XMFLOAT4(-1.f, -1.f, -1.f, -1.f), 2, { 0x00,0x00 } },
        { DXGI_FORMAT_B4G4R4A4_UNORM, XMFLOAT4(0.066667f, 0.066667f, 0.066667f, 0.066667f), 2, { 0x88,0x88 } },
        { DXGI_FORMAT_B4G4R4A4_UNORM, XMFLOAT4(0.466667f, 0.466667f, 0.466667f, 0.466667f), 2, { 0xBB,0xBB } },
        { DXGI_FORMAT_B4G4R4A4_UNORM, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 2, { 0xFF, 0xFF } },
#ifdef _M_ARM64
        { DXGI_FORMAT_B4G4R4A4_UNORM, XMFLOAT4(-0.60000f, -0.60000f, -0.60000f, -0.60000f), 2, { 0x33,0x33} },
        { DXGI_FORMAT_B4G4R4A4_UNORM, XMFLOAT4(0.2f, 0.466667f, 0.6f, 1.0f), 2, { 0xBC,0xF9 } },
#else
        { DXGI_FORMAT_B4G4R4A4_UNORM, XMFLOAT4(-0.466667f, -0.466667f, -0.466667f, -0.466667f), 2, { 0x44,0x44 } },
        { DXGI_FORMAT_B4G4R4A4_UNORM, XMFLOAT4(0.333333f, 0.466667f, 0.733333f, 1.0f), 2, { 0xBD,0xFA } },
#endif
    };

    inline bool IsX2BiasSupported(DXGI_FORMAT format)
    {
        switch (format)
        {
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_B5G6R5_UNORM:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_B4G4R4A4_UNORM:
            return true;

        default:
            return false;
        }
    }

    //-----------------------------------------------------------------------------------

    struct VBMedia
    {
        const D3D12_INPUT_ELEMENT_DESC* ilDesc;
        size_t                          ilNumElements;
        size_t                          stride;
        const wchar_t *                 fname;
    };

    const VBMedia g_VBMedia[] =
    {
        { g_layout, std::size(g_layout), sizeof(WaveFrontReader<uint16_t>::Vertex), MEDIA_PATH L"cube._obj" },
        { g_layout, std::size(g_layout), sizeof(WaveFrontReader<uint16_t>::Vertex), MEDIA_PATH L"cup._obj" },
        { g_layout, std::size(g_layout), sizeof(WaveFrontReader<uint16_t>::Vertex), MEDIA_PATH L"tetrahedron._obj" },
        { g_layout, std::size(g_layout), sizeof(WaveFrontReader<uint16_t>::Vertex), MEDIA_PATH L"teapot._obj" },
        { g_layout, std::size(g_layout), sizeof(WaveFrontReader<uint16_t>::Vertex), MEDIA_PATH L"sphere.vbo" },
        { g_layout, std::size(g_layout), sizeof(WaveFrontReader<uint16_t>::Vertex), MEDIA_PATH L"cylinder.vbo" },
        { g_layout, std::size(g_layout), sizeof(WaveFrontReader<uint16_t>::Vertex), MEDIA_PATH L"torus.vbo" },
    };
}

//-------------------------------------------------------------------------------------

extern const wchar_t* GetName( DXGI_FORMAT fmt );

//-------------------------------------------------------------------------------------
// VBReader
bool Test03_DX12()
{
    bool success = true;

    // Single-stream
    {
        std::unique_ptr<VBReader> reader( new VBReader() );

        D3D12_INPUT_LAYOUT_DESC desc = { s_cubeIL, static_cast<UINT>(std::size(s_cubeIL)) };
        HRESULT hr = reader->Initialize( desc );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed setting up VB reader (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else
        {
            size_t nVerts = std::size(s_cubeVB);

            hr = reader->AddStream( &s_cubeVB, nVerts, 0, sizeof(SimpleVertex) );
            if ( FAILED(hr) )
            {
                success = false;
                printe( "ERROR: Failed setting up stream for VB reader (%08X)\n", static_cast<unsigned int>(hr) );
            }
            else
            {
                auto buff = make_AlignedArrayXMVECTOR(nVerts);

                // Position data
                memset( buff.get(), 0xff, sizeof(XMVECTOR) * nVerts );

                hr = reader->Read( buff.get(), "POSITION", 0, nVerts );
                if ( FAILED(hr) )
                {
                    success = false;
                    printe( "ERROR: Failed reading VB POSITION (%08X)\n", static_cast<unsigned int>(hr) );
                }
                else
                {
                    for( size_t j = 0; j < nVerts; ++j )
                    {
                        XMVECTOR v = buff[ j ];
                        XMVECTOR chk = XMLoadFloat3( &s_cubeVB[ j ].Pos );

                        if ( !XMVector3NearEqual( chk, v, g_VBEpsilon ) )
                        {
                            success = false;
                            printe( "ERROR: Failed comparing VB POSITION %zu: %f,%f,%f ... %f,%f,%f\n", j,
                                    XMVectorGetX( v ), XMVectorGetY( v ), XMVectorGetZ( v ),
                                    s_cubeVB[ j ].Pos.x, s_cubeVB[ j ].Pos.y, s_cubeVB[ j ].Pos.z );
                        }
                    }
                }

                // Texcoord data
                memset( buff.get(), 0xff, sizeof(XMVECTOR) * nVerts );

                hr = reader->Read( buff.get(), "TEXCOORD", 0, nVerts );
                if ( FAILED(hr) )
                {
                    success = false;
                    printe( "ERROR: Failed reading VB TEXCOORD (%08X)\n", static_cast<unsigned int>(hr) );
                }
                else
                {
                    for( size_t j = 0; j < nVerts; ++j )
                    {
                        XMVECTOR v = buff[ j ];
                        XMVECTOR chk = XMLoadFloat2( &s_cubeVB[ j ].Tex );

                        if ( !XMVector2NearEqual( chk, v, g_VBEpsilon ) )
                        {
                            success = false;
                            printe( "ERROR: Failed comparing VB TEXCOORD %zu: %f,%f ... %f,%f \n", j,
                                    XMVectorGetX( v ), XMVectorGetY( v ),
                                    s_cubeVB[ j ].Tex.x, s_cubeVB[ j ].Tex.y );
                        }
                    }
                }

                // Position data (semantic alias)
                memset( buff.get(), 0xff, sizeof(XMVECTOR) * nVerts );

                hr = reader->Read( buff.get(), "SV_Position", 0, nVerts );
                if ( FAILED(hr) )
                {
                    success = false;
                    printe( "ERROR: Failed reading VB SV_Position (%08X)\n", static_cast<unsigned int>(hr) );
                }
                else
                {
                    for( size_t j = 0; j < nVerts; ++j )
                    {
                        XMVECTOR v = buff[ j ];
                        XMVECTOR chk = XMLoadFloat3( &s_cubeVB[ j ].Pos );

                        if ( !XMVector3NearEqual( chk, v, g_VBEpsilon ) )
                        {
                            success = false;
                            printe( "ERROR: Failed comparing VB SV_Position %zu: %f,%f,%f ... %f,%f,%f\n", j,
                                    XMVectorGetX( v ), XMVectorGetY( v ), XMVectorGetZ( v ),
                                    s_cubeVB[ j ].Pos.x, s_cubeVB[ j ].Pos.y, s_cubeVB[ j ].Pos.z );
                        }
                    }
                }

                // Position data (XMFLOAT3)
                {
                    std::unique_ptr<XMFLOAT3[]> buff3( new XMFLOAT3[ nVerts ] );
                    memset( buff3.get(), 0xff, sizeof(XMFLOAT3) * nVerts );

                    hr = reader->Read( buff3.get(), "POSITION", 0, nVerts );
                    if ( FAILED(hr) )
                    {
                        success = false;
                        printe( "ERROR: Failed reading VB XMFLOAT3 (%08X)\n", static_cast<unsigned int>(hr) );
                    }
                    else
                    {
                        for( size_t j = 0; j < nVerts; ++j )
                        {
                            XMVECTOR v = XMLoadFloat3( &buff3[ j ] );
                            XMVECTOR chk = XMLoadFloat3( &s_cubeVB[ j ].Pos );

                            if ( !XMVector3NearEqual( chk, v, g_VBEpsilon ) )
                            {
                                success = false;
                                printe( "ERROR: Failed comparing VB XMFLOAT3 %zu: %f,%f,%f ... %f,%f,%f\n", j,
                                        XMVectorGetX( v ), XMVectorGetY( v ), XMVectorGetZ( v ),
                                        s_cubeVB[ j ].Pos.x, s_cubeVB[ j ].Pos.y, s_cubeVB[ j ].Pos.z );
                            }
                        }
                    }
                }

                // Texcoord data (XMFLOAT2)
                {
                    std::unique_ptr<XMFLOAT2[]> buff2( new XMFLOAT2[ nVerts ] );
                    memset( buff2.get(), 0xff, sizeof(XMFLOAT2) * nVerts );

                    hr = reader->Read( buff2.get(), "TEXCOORD", 0, nVerts );
                    if ( FAILED(hr) )
                    {
                        success = false;
                        printe( "ERROR: Failed reading VB XMFLOAT2 (%08X)\n", static_cast<unsigned int>(hr) );
                    }
                    else
                    {
                        for( size_t j = 0; j < nVerts; ++j )
                        {
                            XMVECTOR v = XMLoadFloat2( &buff2[ j ] );
                            XMVECTOR chk = XMLoadFloat2( &s_cubeVB[ j ].Tex );

                            if ( !XMVector2NearEqual( chk, v, g_VBEpsilon ) )
                            {
                                success = false;
                                printe( "ERROR: Failed comparing VB XMFLOAT2 %zu: %f,%f ... %f,%f \n", j,
                                        XMVectorGetX( v ), XMVectorGetY( v ),
                                        s_cubeVB[ j ].Tex.x, s_cubeVB[ j ].Tex.y );
                            }
                        }
                    }
                }

                // Expected error cases
                memset( buff.get(), 0xff, sizeof(XMVECTOR) * nVerts );

                #pragma warning(push)
                #pragma warning(disable:6387)
                hr = reader->Read( buff.get(), nullptr, 0, nVerts );
                if ( SUCCEEDED( hr ) )
                {
                    success = false;
                    printe( "ERROR: VB reader with null semantic name should fail\n" );
                }
                #pragma warning(pop)

                hr = reader->Read( buff.get(), "NORMAL", 0, nVerts );
                if ( SUCCEEDED( hr ) )
                {
                    success = false;
                    printe( "ERROR: VB reader with NORMAL semantic name should fail\n" );
                }

                hr = reader->Read( buff.get(), "POSITION", 2, nVerts );
                if ( SUCCEEDED( hr ) )
                {
                    success = false;
                    printe( "ERROR: VB reader with POSITION2 semantic name should fail\n" );
                }
            }
        }
    }

    // Multi-stream
    {
        std::unique_ptr<VBReader> reader( new VBReader() );

        D3D12_INPUT_LAYOUT_DESC desc = { g_VSStarterKitAnimation, static_cast<UINT>(std::size(g_VSStarterKitAnimation)) };
        HRESULT hr = reader->Initialize( desc );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed setting up VB m/s reader (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else
        {
            size_t nVerts = std::size(s_VSStarterKitVB1);

            hr = reader->AddStream( &s_VSStarterKitVB1, nVerts, 0, sizeof(VSStarterKitVertex1) );
            if ( FAILED(hr) )
            {
                success = false;
                printe( "ERROR: Failed setting up m/s stream 0 for VB reader (%08X)\n", static_cast<unsigned int>(hr) );
            }
            else
            {
                hr = reader->AddStream( &s_VSStarterKitVB2, nVerts, 1, sizeof(VSStarterKitVertex2) );
                if ( FAILED(hr) )
                {
                    success = false;
                    printe( "ERROR: Failed setting up m/s stream 1 for VB reader (%08X)\n", static_cast<unsigned int>(hr) );
                }
                else
                {
                    auto buff = make_AlignedArrayXMVECTOR(nVerts);

                    // Position data
                    memset( buff.get(), 0xff, sizeof(XMVECTOR) * nVerts );

                    hr = reader->Read( buff.get(), "POSITION", 0, nVerts );
                    if ( FAILED(hr) )
                    {
                        success = false;
                        printe( "ERROR: Failed reading m/s VB POSITION (%08X)\n", static_cast<unsigned int>(hr) );
                    }
                    else
                    {
                        for( size_t j = 0; j < nVerts; ++j )
                        {
                            XMVECTOR v = buff[ j ];
                            XMVECTOR chk = XMLoadFloat3( &s_VSStarterKitVB1[ j ].position );

                            if ( !XMVector3NearEqual( chk, v, g_VBEpsilon ) )
                            {
                                success = false;
                                printe( "ERROR: Failed comparing m/s VB POSITION %zu: %f,%f,%f ... %f,%f,%f\n", j,
                                        XMVectorGetX( v ), XMVectorGetY( v ), XMVectorGetZ( v ),
                                        s_VSStarterKitVB1[ j ].position.x, s_VSStarterKitVB1[ j ].position.y, s_VSStarterKitVB1[ j ].position.z );
                            }
                        }
                    }

                    // Texcoord data
                    memset( buff.get(), 0xff, sizeof(XMVECTOR) * nVerts );

                    hr = reader->Read( buff.get(), "TEXCOORD", 0, nVerts );
                    if ( FAILED(hr) )
                    {
                        success = false;
                        printe( "ERROR: Failed reading m/s VB TEXCOORD (%08X)\n", static_cast<unsigned int>(hr) );
                    }
                    else
                    {
                        for( size_t j = 0; j < nVerts; ++j )
                        {
                            XMVECTOR v = buff[ j ];
                            XMVECTOR chk = XMLoadFloat2( &s_VSStarterKitVB1[ j ].texureCoordinate );

                            if ( !XMVector2NearEqual( chk, v, g_VBEpsilon ) )
                            {
                                success = false;
                                printe( "ERROR: Failed comparing m/s VB TEXCOORD %zu: %f,%f ... %f,%f \n", j,
                                        XMVectorGetX( v ), XMVectorGetY( v ),
                                        s_VSStarterKitVB1[ j ].texureCoordinate.x, s_VSStarterKitVB1[ j ].texureCoordinate.y );
                            }
                        }
                    }

                    // Blend weights data
                    memset( buff.get(), 0xff, sizeof(XMVECTOR) * nVerts );

                    hr = reader->Read( buff.get(), "BLENDWEIGHT", 0, nVerts );
                    if ( FAILED(hr) )
                    {
                        success = false;
                        printe( "ERROR: Failed reading m/s VB BLENDWEIGHT (%08X)\n", static_cast<unsigned int>(hr) );
                    }
                    else
                    {
                        for( size_t j = 0; j < nVerts; ++j )
                        {
                            XMVECTOR v = buff[ j ];
                            XMVECTOR chk = XMLoadFloat4( &s_VSStarterKitVB2[ j ].boneWeight );

                            if ( !XMVector4NearEqual( chk, v, g_VBEpsilon ) )
                            {
                                success = false;
                                printe( "ERROR: Failed comparing m/s VB BLENDWEIGHT %zu: %f,%f ... %f,%f \n", j,
                                        XMVectorGetX( v ), XMVectorGetY( v ),
                                        s_VSStarterKitVB1[ j ].texureCoordinate.x, s_VSStarterKitVB1[ j ].texureCoordinate.y );
                            }
                        }
                    }
                }
            }
        }
    }

    // GetElement
    {
        std::unique_ptr<VBReader> reader( new VBReader() );

        D3D12_INPUT_LAYOUT_DESC desc = { g_VSStarterKitAnimation, static_cast<UINT>(std::size(g_VSStarterKitAnimation)) };
        HRESULT hr = reader->Initialize( desc );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed setting up VB m/s reader (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else
        {
            auto e = reader->GetElement12( "POSITION", 0 );
            if ( !e )
            {
                success = false;
                printe( "ERROR: Failed to find POSITION0 element\n" );
            }
            else if ( e->SemanticIndex != 0
                      || e->Format != DXGI_FORMAT_R32G32B32_FLOAT
                      || e->InputSlot != 0
                      || e->AlignedByteOffset != 0
                      || e->InputSlotClass != D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA
                      || e->InstanceDataStepRate != 0 )
            {
                success = false;
                printe( "ERROR: POSITION0 data invalid\n" );
            }

            e = reader->GetElement12( "POSITION", 1 );
            if ( e )
            {
                success = false;
                printe( "ERROR: Shouldn't find POSITION1 element\n" );
            }

            e = reader->GetElement12( "TEXCOORD", 0 );
            if ( !e )
            {
                success = false;
                printe( "ERROR: Failed to find TEXCOORD0 element\n" );
            }
            else if ( e->SemanticIndex != 0
                      || e->Format != DXGI_FORMAT_R32G32_FLOAT
                      || e->InputSlot != 0
                      || e->AlignedByteOffset != 44
                      || e->InputSlotClass != D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA
                      || e->InstanceDataStepRate != 0 )
            {
                success = false;
                printe( "ERROR: TEXCOORD0 data invalid\n" );
            }

            e = reader->GetElement12( "BINORMAL", 0 );
            if ( e )
            {
                success = false;
                printe( "ERROR: Shouldn't find POSITION1 element\n" );
            }

            e = reader->GetElement12( "BLENDWEIGHT", 0 );
            if ( !e )
            {
                success = false;
                printe( "ERROR: Failed to find BLENDWEIGHT element\n" );
            }
            else if ( e->SemanticIndex != 0
                      || e->Format != DXGI_FORMAT_R32G32B32A32_FLOAT
                      || e->InputSlot != 1
                      || e->AlignedByteOffset != 4
                      || e->InputSlotClass != D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA
                      || e->InstanceDataStepRate != 0 )
            {
                success = false;
                printe( "ERROR: BLENDWEIGHT data invalid\n" );
            }
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// VBWriter
bool Test04_DX12()
{
    bool success = true;

    // Single-stream
    {
        std::unique_ptr<VBWriter> writer( new VBWriter() );

        D3D12_INPUT_LAYOUT_DESC desc = { s_cubeIL, static_cast<UINT>(std::size(s_cubeIL)) };
        HRESULT hr = writer->Initialize( desc );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed setting up VB writer (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else
        {
            size_t nVerts = std::size(s_cubeVB);

            std::unique_ptr<uint8_t[]> vb( new uint8_t[ sizeof(SimpleVertex) * nVerts ] );

            hr = writer->AddStream( vb.get(), nVerts, 0, sizeof(SimpleVertex) );
            if ( FAILED(hr) )
            {
                success = false;
                printe( "ERROR: Failed setting up stream for VB writer (%08X)\n", static_cast<unsigned int>(hr) );
            }
            else
            {
                memset( vb.get(), 0xff, sizeof(SimpleVertex) * nVerts );

                auto posbuff = make_AlignedArrayXMVECTOR(nVerts);
                auto texbuff = make_AlignedArrayXMVECTOR(nVerts);

                for( size_t j = 0; j < nVerts; ++j )
                {
                    posbuff[ j ] = XMLoadFloat3( &s_cubeVB[ j ].Pos );
                    texbuff[ j ] = XMLoadFloat2( &s_cubeVB[ j ].Tex );
                }

                hr = writer->Write( posbuff.get(), "POSITION", 0, nVerts );
                if ( FAILED(hr) )
                {
                    success = false;
                    printe( "ERROR: Failed writing VB POSITION (%08X)\n", static_cast<unsigned int>(hr) );
                }
                else
                {
                    hr = writer->Write( texbuff.get(), "TEXCOORD", 0, nVerts );
                    if ( FAILED(hr) )
                    {
                        success = false;
                        printe( "ERROR: Failed writing VB TEXCOORD (%08X)\n", static_cast<unsigned int>(hr) );
                    }
                    else if ( memcmp( vb.get(), s_cubeVB, sizeof(s_cubeVB) ) != 0 )
                    {
                        success = false;
                        printe( "ERROR: VB written does not match expected value\n" );
                    }
                }

                // Position data (semantic alias)
                hr = writer->Write( posbuff.get(), "SV_Position", 0, nVerts );
                if ( FAILED(hr) )
                {
                    success = false;
                    printe( "ERROR: Failed writing VB SV_Position (%08X)\n", static_cast<unsigned int>(hr) );
                }
                else if ( memcmp( vb.get(), s_cubeVB, sizeof(s_cubeVB) ) != 0 )
                {
                    success = false;
                    printe( "ERROR: VB written does not match expected value\n" );
                }

                // XMFLOAT2/XMFLOAT3
                {
                    memset( vb.get(), 0xff, sizeof(SimpleVertex) * nVerts );

                    std::vector<XMFLOAT3> buff3;
                    std::vector<XMFLOAT2> buff2;

                    buff3.reserve( nVerts );
                    buff2.reserve( nVerts );
                    for( size_t j = 0; j < nVerts; ++j )
                    {
                        buff3.push_back( s_cubeVB[ j ].Pos );
                        buff2.push_back( s_cubeVB[ j ].Tex );
                    }

                    hr = writer->Write( buff3.data(), "POSITION", 0, nVerts );
                    if ( FAILED(hr) )
                    {
                        success = false;
                        printe( "ERROR: Failed writing VB POSITION (%08X)\n", static_cast<unsigned int>(hr) );
                    }
                    else
                    {
                        hr = writer->Write( buff2.data(), "TEXCOORD", 0, nVerts );
                        if ( FAILED(hr) )
                        {
                            success = false;
                            printe( "ERROR: Failed writing VB TEXCOORD (%08X)\n", static_cast<unsigned int>(hr) );
                        }
                        else if ( memcmp( vb.get(), s_cubeVB, sizeof(s_cubeVB) ) != 0 )
                        {
                            success = false;
                            printe( "ERROR: VB written does not match expected value\n" );
                        }
                    }
                }

                // Expected error cases
                hr = writer->Write( posbuff.get(), nullptr, 0, nVerts );
                if ( SUCCEEDED( hr ) )
                {
                    success = false;
                    printe( "ERROR: VB writer with null semantic name should fail\n" );
                }

                hr = writer->Write( posbuff.get(), "NORMAL", 0, nVerts );
                if ( SUCCEEDED( hr ) )
                {
                    success = false;
                    printe( "ERROR: VB writer with NORMAL semantic name should fail\n" );
                }

                hr = writer->Write( posbuff.get(), "POSITION", 2, nVerts );
                if ( SUCCEEDED( hr ) )
                {
                    success = false;
                    printe( "ERROR: VB writer with POSITION2 semantic name should fail\n" );
                }
            }
        }
    }

    // Multi-stream
    {
        std::unique_ptr<VBWriter> writer( new VBWriter() );

        D3D12_INPUT_LAYOUT_DESC desc = { g_VSStarterKitAnimation, static_cast<UINT>(std::size(g_VSStarterKitAnimation)) };
        HRESULT hr = writer->Initialize( desc );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed setting up VB m/s writer (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else
        {
            size_t nVerts = std::size(s_VSStarterKitVB1);

            std::unique_ptr<uint8_t[]> vb1( new uint8_t[ sizeof(VSStarterKitVertex1) * nVerts ] );
            std::unique_ptr<uint8_t[]> vb2( new uint8_t[ sizeof(VSStarterKitVertex2) * nVerts ] );

            hr = writer->AddStream( vb1.get(), nVerts, 0, sizeof(VSStarterKitVertex1) );
            if ( FAILED(hr) )
            {
                success = false;
                printe( "ERROR: Failed setting up stream 0 for VB m/s writer (%08X)\n", static_cast<unsigned int>(hr) );
            }
            else
            {
                hr = writer->AddStream( vb2.get(), nVerts, 1, sizeof(VSStarterKitVertex2) );
                if ( FAILED(hr) )
                {
                    success = false;
                    printe( "ERROR: Failed setting up stream 1 for VB m/s writer (%08X)\n", static_cast<unsigned int>(hr) );
                }
                else
                {
                    using namespace DirectX::PackedVector;

                    memset( vb1.get(), 0xff, sizeof(VSStarterKitVertex1) * nVerts );
                    memset( vb2.get(), 0xff, sizeof(VSStarterKitVertex2) * nVerts );

                    auto posBuff = make_AlignedArrayXMVECTOR(nVerts);
                    auto normalBuff = make_AlignedArrayXMVECTOR(nVerts);
                    auto tangentBuff = make_AlignedArrayXMVECTOR(nVerts);
                    auto colorBuff = make_AlignedArrayXMVECTOR(nVerts);
                    auto texBuff = make_AlignedArrayXMVECTOR(nVerts);
                    auto boneIndexBuff = make_AlignedArrayXMVECTOR(nVerts);
                    auto boneWeightsBuff = make_AlignedArrayXMVECTOR(nVerts);

                    for( size_t j = 0; j < nVerts; ++j )
                    {
                        posBuff[ j ] = XMLoadFloat3( &s_VSStarterKitVB1[ j ].position );
                        normalBuff[ j ] = XMLoadFloat3( &s_VSStarterKitVB1[ j ].normal );
                        tangentBuff[ j ] = XMLoadFloat4( &s_VSStarterKitVB1[ j ].tangent );
                        colorBuff[ j ] = XMLoadUByteN4( reinterpret_cast<const XMUBYTEN4*>( &s_VSStarterKitVB1[ j ].color ) );
                        texBuff[ j ] = XMLoadFloat2( &s_VSStarterKitVB1[ j ].texureCoordinate );
                        boneIndexBuff[ j ] = XMLoadUByte4( reinterpret_cast<const XMUBYTE4*>( &s_VSStarterKitVB2[ j ].boneIndex[0] ) );
                        boneWeightsBuff[ j ] = XMLoadFloat4( &s_VSStarterKitVB2[ j ].boneWeight );
                    }

                    bool pass = true;

                    hr = writer->Write( posBuff.get(), "POSITION", 0, nVerts );
                    if ( FAILED(hr) )
                    {
                        success = false;
                        pass = false;
                        printe( "ERROR: Failed writing VB m/s POSITION (%08X)\n", static_cast<unsigned int>(hr) );
                    }

                    hr = writer->Write( normalBuff.get(), "NORMAL", 0, nVerts );
                    if ( FAILED(hr) )
                    {
                        success = false;
                        pass = false;
                        printe( "ERROR: Failed writing VB m/s NORMAL (%08X)\n", static_cast<unsigned int>(hr) );
                    }

                    hr = writer->Write( texBuff.get(), "TEXCOORD", 0, nVerts );
                    if ( FAILED(hr) )
                    {
                        success = false;
                        pass = false;
                        printe( "ERROR: Failed writing VB m/s TEXCOORD (%08X)\n", static_cast<unsigned int>(hr) );
                    }

                    hr = writer->Write( tangentBuff.get(), "TANGENT", 0, nVerts );
                    if ( FAILED(hr) )
                    {
                        success = false;
                        pass = false;
                        printe( "ERROR: Failed writing VB m/s TANGENT (%08X)\n", static_cast<unsigned int>(hr) );
                    }

                    hr = writer->Write( colorBuff.get(), "COLOR", 0, nVerts );
                    if ( FAILED(hr) )
                    {
                        success = false;
                        pass = false;
                        printe( "ERROR: Failed writing VB m/s COLOR (%08X)\n", static_cast<unsigned int>(hr) );
                    }

                    hr = writer->Write( boneIndexBuff.get(), "BLENDINDICES", 0, nVerts );
                    if ( FAILED(hr) )
                    {
                        success = false;
                        pass = false;
                        printe( "ERROR: Failed writing VB m/s BLENDINDICES (%08X)\n", static_cast<unsigned int>(hr) );
                    }

                    hr = writer->Write( boneWeightsBuff.get(), "BLENDWEIGHT", 0, nVerts );
                    if ( FAILED(hr) )
                    {
                        success = false;
                        pass = false;
                        printe( "ERROR: Failed writing VB m/s BLENDWEIGHT (%08X)\n", static_cast<unsigned int>(hr) );
                    }

                    if ( pass )
                    {
                        if ( memcmp( vb1.get(), s_VSStarterKitVB1, sizeof(s_VSStarterKitVB1) ) != 0 )
                        {
                            success = false;
                            printe( "ERROR: VB m/s 0 written does not match expected value\n" );
                        }
                        if ( memcmp( vb2.get(), s_VSStarterKitVB2, sizeof(s_VSStarterKitVB2) ) != 0 )
                        {
                            success = false;
                            printe( "ERROR: VB m/s 1 written does not match expected value\n" );
                        }
                    }
                }
            }
        }
    }

    // GetElement
    {
        std::unique_ptr<VBWriter> writer( new VBWriter() );

        D3D12_INPUT_LAYOUT_DESC desc = { g_VSStarterKitAnimation, static_cast<UINT>(std::size(g_VSStarterKitAnimation)) };
        HRESULT hr = writer->Initialize( desc );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed setting up VB m/s reader (%08X)\n", static_cast<unsigned int>(hr) );
        }
        else
        {
            auto e = writer->GetElement12( "POSITION", 0 );
            if ( !e )
            {
                success = false;
                printe( "ERROR: Failed to find POSITION0 element\n" );
            }
            else if ( e->SemanticIndex != 0
                      || e->Format != DXGI_FORMAT_R32G32B32_FLOAT
                      || e->InputSlot != 0
                      || e->AlignedByteOffset != 0
                      || e->InputSlotClass != D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA
                      || e->InstanceDataStepRate != 0 )
            {
                success = false;
                printe( "ERROR: POSITION0 data invalid\n" );
            }

            e = writer->GetElement12( "POSITION", 1 );
            if ( e )
            {
                success = false;
                printe( "ERROR: Shouldn't find POSITION1 element\n" );
            }

            e = writer->GetElement12( "TEXCOORD", 0 );
            if ( !e )
            {
                success = false;
                printe( "ERROR: Failed to find TEXCOORD0 element\n" );
            }
            else if ( e->SemanticIndex != 0
                      || e->Format != DXGI_FORMAT_R32G32_FLOAT
                      || e->InputSlot != 0
                      || e->AlignedByteOffset != 44
                      || e->InputSlotClass != D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA
                      || e->InstanceDataStepRate != 0 )
            {
                success = false;
                printe( "ERROR: TEXCOORD0 data invalid\n" );
            }

            e = writer->GetElement12( "BINORMAL", 0 );
            if ( e )
            {
                success = false;
                printe( "ERROR: Shouldn't find POSITION1 element\n" );
            }

            e = writer->GetElement12( "BLENDWEIGHT", 0 );
            if ( !e )
            {
                success = false;
                printe( "ERROR: Failed to find BLENDWEIGHT element\n" );
            }
            else if ( e->SemanticIndex != 0
                      || e->Format != DXGI_FORMAT_R32G32B32A32_FLOAT
                      || e->InputSlot != 1
                      || e->AlignedByteOffset != 4
                      || e->InputSlotClass != D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA
                      || e->InstanceDataStepRate != 0 )
            {
                success = false;
                printe( "ERROR: BLENDWEIGHT data invalid\n" );
            }
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// VBReader Formats
bool Test05_DX12()
{
    bool success = true;

    for( size_t index=0; index < std::size(g_TestVBs); ++index )
    {
        auto& v = g_TestVBs[index];

        assert( BytesPerElement(v.format) == v.stride );

        std::unique_ptr<VBReader> reader( new VBReader() );

        D3D12_INPUT_ELEMENT_DESC ilDesc = { "DATA", 0, v.format, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

        D3D12_INPUT_LAYOUT_DESC desc = { &ilDesc, 1 };
        HRESULT hr = reader->Initialize( desc );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed setting up VB reader for format %ls, index %zu (%08X)\n", GetName( v.format ), index, static_cast<unsigned int>(hr) );
        }
        else
        {
            hr = reader->AddStream( &v.bytes[0], 1, 0, v.stride );
            if ( FAILED(hr) )
            {
                success = false;
                printe( "ERROR: Failed setting up stream for VB reader for format %ls, index %zu (%08X)\n", GetName( v.format ), index, static_cast<unsigned int>(hr) );
            }
            else
            {
                XM_ALIGNED_DATA(16) XMVECTOR temp;
                hr = reader->Read( &temp, "DATA", 0, 1 );
                if ( FAILED(hr) )
                {
                    success = false;
                    printe( "ERROR: Failed reading VB format %ls, index %zu (%08X)\n", GetName( v.format ), index, static_cast<unsigned int>(hr) );
                }
                else
                {
                    XMVECTOR chk = XMLoadFloat4( &v.vector );

                    if ( !XMVector4NearEqual( chk, temp, g_VBEpsilon ) )
                    {
                        success = false;
                        printe( "ERROR: Failed reading VB format %ls, index %zu: %f %f %f %f ... %f %f %f %f\n", GetName( v.format ), index,
                                XMVectorGetX(temp), XMVectorGetY(temp), XMVectorGetZ(temp), XMVectorGetW(temp),
                                v.vector.x, v.vector.y, v.vector.z, v.vector.w );
                    }
                }

                // Confirm that x2bias flag has expected impact on this format
                hr = reader->Read(&temp, "DATA", 0, 1, true);
                if (FAILED(hr))
                {
                    success = false;
                    printe("ERROR: Failed reading VB format %ls (x2bias), index %zu (%08X)\n", GetName(v.format), index, static_cast<unsigned int>(hr));
                }
                else
                {
                    XMVECTOR chk = XMLoadFloat4(&v.vector);

                    bool isx2bias = IsX2BiasSupported(v.format);

                    if (XMVector4NearEqual(chk, temp, g_VBEpsilon) == isx2bias)
                    {
                        if (!isx2bias || (fabs(XMVectorGetX(chk) - 1.f) > 0.001f))
                        {
                            success = false;
                            printe("ERROR: Failed reading VB format %ls (x2bias %d), index %zu: %f %f %f %f ... %f %f %f %f\n", GetName(v.format), isx2bias, index,
                                XMVectorGetX(temp), XMVectorGetY(temp), XMVectorGetZ(temp), XMVectorGetW(temp),
                                v.vector.x, v.vector.y, v.vector.z, v.vector.w);
                        }
                    }
                }
            }
        }
    }

    for (size_t index = 0; index < std::size(g_TestVBs_x2Bias); ++index)
    {
        auto& v = g_TestVBs_x2Bias[index];

        assert(IsX2BiasSupported(v.format));
        assert(BytesPerElement(v.format) == v.stride);

        std::unique_ptr<VBReader> reader(new VBReader());

        D3D12_INPUT_ELEMENT_DESC ilDesc = { "DATA", 0, v.format, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

        D3D12_INPUT_LAYOUT_DESC desc = { &ilDesc, 1 };
        HRESULT hr = reader->Initialize( desc );
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: Failed setting up VB reader for format %ls X2BIAS, index %zu (%08X)\n", GetName(v.format), index, static_cast<unsigned int>(hr));
        }
        else
        {
            hr = reader->AddStream(&v.bytes[0], 1, 0, v.stride);
            if (FAILED(hr))
            {
                success = false;
                printe("ERROR: Failed setting up stream for VB reader for format %ls X2BIAS, index %zu (%08X)\n", GetName(v.format), index, static_cast<unsigned int>(hr));
            }
            else
            {
                XM_ALIGNED_DATA(16) XMVECTOR temp;
                hr = reader->Read(&temp, "DATA", 0, 1, true);
                if (FAILED(hr))
                {
                    success = false;
                    printe("ERROR: Failed reading VB format %ls X2BIAS, index %zu (%08X)\n", GetName(v.format), index, static_cast<unsigned int>(hr));
                }
                else
                {
                    XMVECTOR chk = XMLoadFloat4(&v.vector);

                    if (!XMVector4NearEqual(chk, temp, g_VBEpsilon))
                    {
                        success = false;
                        printe("ERROR: Failed reading VB format %ls X2BIAS, index %zu: %f %f %f %f ... %f %f %f %f\n", GetName(v.format), index,
                            XMVectorGetX(temp), XMVectorGetY(temp), XMVectorGetZ(temp), XMVectorGetW(temp),
                            v.vector.x, v.vector.y, v.vector.z, v.vector.w);
                    }
                }
            }
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// VBWriter Formats
bool Test06_DX12()
{
    bool success = true;
    uint8_t buff[16];

    for( size_t index=0; index < std::size(g_TestVBs); ++index )
    {
        auto& v = g_TestVBs[index];

        assert( BytesPerElement(v.format) == v.stride );

        std::unique_ptr<VBWriter> writer( new VBWriter() );

        D3D12_INPUT_ELEMENT_DESC ilDesc = { "DATA", 0, v.format, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

        D3D12_INPUT_LAYOUT_DESC desc = { &ilDesc, 1 };
        HRESULT hr = writer->Initialize( desc );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed setting up VB writer for format %ls, index %zu (%08X)\n", GetName( v.format ), index, static_cast<unsigned int>(hr) );
        }
        else
        {
            memset( buff, 0, sizeof(buff) );

            hr = writer->AddStream( buff, 1, 0, v.stride );
            if ( FAILED(hr) )
            {
                success = false;
                printe( "ERROR: Failed setting up stream for VB writer for format %ls, index %zu (%08X)\n", GetName( v.format ), index, static_cast<unsigned int>(hr) );
            }
            else
            {
                XMVECTOR temp = XMLoadFloat4( &v.vector );
                hr = writer->Write( &temp, "DATA", 0, 1 );
                if ( FAILED(hr) )
                {
                    success = false;
                    printe( "ERROR: Failed writing VB writer %ls, index %zu (%08X)\n", GetName( v.format ), index, static_cast<unsigned int>(hr) );
                }
                else if ( memcmp( buff, &v.bytes[0], v.stride ) != 0 )
                {
                    success = false;
                    printe( "ERROR: Failed writing VB format %ls, index %zu: %f %f %f %f\n", GetName( v.format ), index,
                            v.vector.x, v.vector.y, v.vector.z, v.vector.w );
                    printe( "    %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n... %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
                            buff[0], buff[1], buff[2], buff[3], buff[4], buff[5], buff[6], buff[7],
                            buff[8], buff[9], buff[10], buff[11], buff[12], buff[13], buff[14], buff[15],
                            v.bytes[0], v.bytes[1], v.bytes[2], v.bytes[3], v.bytes[4], v.bytes[5], v.bytes[6], v.bytes[7],
                            v.bytes[8], v.bytes[9], v.bytes[10], v.bytes[11], v.bytes[12], v.bytes[13], v.bytes[14], v.bytes[15] );
                }

                // Confirm that x2bias flag has expected impact on this format
                hr = writer->Write(&temp, "DATA", 0, 1, true);
                if (FAILED(hr))
                {
                    success = false;
                    printe("ERROR: Failed writing VB writer %ls (x2bias), index %zu (%08X)\n", GetName(v.format), index, static_cast<unsigned int>(hr));
                }
                {
                    bool isx2bias = IsX2BiasSupported(v.format);

                    if ( (!isx2bias && memcmp(buff, &v.bytes[0], v.stride) != 0)
                         || (isx2bias && memcpy(buff, &v.bytes[0], v.stride) == 0) )
                    {
                        success = false;
                        printe("ERROR: Failed writing VB format %ls (x2bais %d), index %zu: %f %f %f %f\n", GetName(v.format), isx2bias, index,
                            v.vector.x, v.vector.y, v.vector.z, v.vector.w);
                        printe("    %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n... %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
                            buff[0], buff[1], buff[2], buff[3], buff[4], buff[5], buff[6], buff[7],
                            buff[8], buff[9], buff[10], buff[11], buff[12], buff[13], buff[14], buff[15],
                            v.bytes[0], v.bytes[1], v.bytes[2], v.bytes[3], v.bytes[4], v.bytes[5], v.bytes[6], v.bytes[7],
                            v.bytes[8], v.bytes[9], v.bytes[10], v.bytes[11], v.bytes[12], v.bytes[13], v.bytes[14], v.bytes[15]);
                    }
                }
            }
        }
    }

    for (size_t index = 0; index < std::size(g_TestVBs_x2Bias); ++index)
    {
        auto& v = g_TestVBs_x2Bias[index];

        assert(IsX2BiasSupported(v.format));
        assert(BytesPerElement(v.format) == v.stride);

        std::unique_ptr<VBWriter> writer(new VBWriter());

        D3D12_INPUT_ELEMENT_DESC ilDesc = { "DATA", 0, v.format, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

        D3D12_INPUT_LAYOUT_DESC desc = { &ilDesc, 1 };
        HRESULT hr = writer->Initialize( desc );
        if (FAILED(hr))
        {
            success = false;
            printe("ERROR: Failed setting up VB writer for format %ls X2BIAS, index %zu (%08X)\n", GetName(v.format), index, static_cast<unsigned int>(hr));
        }
        else
        {
            memset(buff, 0, sizeof(buff));

            hr = writer->AddStream(buff, 1, 0, v.stride);
            if (FAILED(hr))
            {
                success = false;
                printe("ERROR: Failed setting up stream for VB writer for format %ls X2BIAS, index %zu (%08X)\n", GetName(v.format), index, static_cast<unsigned int>(hr));
            }
            else
            {
                XMVECTOR temp = XMLoadFloat4(&v.vector);
                hr = writer->Write(&temp, "DATA", 0, 1, true);
                if (FAILED(hr))
                {
                    success = false;
                    printe("ERROR: Failed writing VB writer %ls X2BIAS, index %zu (%08X)\n", GetName(v.format), index, static_cast<unsigned int>(hr));
                }
                else if (memcmp(buff, &v.bytes[0], v.stride) != 0)
                {
                    success = false;
                    printe("ERROR: Failed writing VB format %ls X2BIAS, index %zu: %f %f %f %f\n", GetName(v.format), index,
                        v.vector.x, v.vector.y, v.vector.z, v.vector.w);
                    printe("    %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n... %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
                        buff[0], buff[1], buff[2], buff[3], buff[4], buff[5], buff[6], buff[7],
                        buff[8], buff[9], buff[10], buff[11], buff[12], buff[13], buff[14], buff[15],
                        v.bytes[0], v.bytes[1], v.bytes[2], v.bytes[3], v.bytes[4], v.bytes[5], v.bytes[6], v.bytes[7],
                        v.bytes[8], v.bytes[9], v.bytes[10], v.bytes[11], v.bytes[12], v.bytes[13], v.bytes[14], v.bytes[15]);
                }
            }
        }
    }

    return success;
}


//-------------------------------------------------------------------------------------
// VB R/W Examples
bool Test07_DX12()
{
    bool success = true;

    size_t ncount = 0;
    size_t npass = 0;

    for( size_t index=0; index < std::size(g_VBMedia); ++index )
    {
        wchar_t szPath[MAX_PATH];
        DWORD ret = ExpandEnvironmentStringsW(g_VBMedia[index].fname, szPath, MAX_PATH);
        if ( !ret || ret > MAX_PATH )
        {
            printe( "ERROR: ExpandEnvironmentStrings FAILED\n" );
            return false;
        }

#ifdef _DEBUG
        OutputDebugStringW(szPath);
        OutputDebugStringA("\n");
#endif

        wchar_t ext[_MAX_EXT];
        _wsplitpath_s( szPath, nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT );

        std::unique_ptr<WaveFrontReader<uint16_t>> mesh( new WaveFrontReader<uint16_t>() );

        HRESULT hr;
        if ( _wcsicmp( ext, L".vbo" ) == 0 )
        {
            hr = mesh->LoadVBO( szPath );
        }
        else
        {
            hr = mesh->Load( szPath );
        }
        
        ++ncount;

        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed loading mesh data (%08X):\n%ls\n", static_cast<unsigned int>(hr), szPath );
            continue;
        }

        D3D12_INPUT_LAYOUT_DESC desc = { g_VBMedia[index].ilDesc, static_cast<UINT>(g_VBMedia[index].ilNumElements) };
        std::unique_ptr<VBReader> reader( new VBReader() );
        hr = reader->Initialize( desc );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed setting up VB reader (%08X):\n%ls\n", static_cast<unsigned int>(hr), szPath );
            continue;
        }

        size_t nVerts = mesh->vertices.size();

        hr = reader->AddStream( mesh->vertices.data(), nVerts, 0, g_VBMedia[index].stride );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed setting up stream for VB reader (%08X):\n%ls\n", static_cast<unsigned int>(hr), szPath );
            continue;
        }

        std::unique_ptr<VBWriter> writer( new VBWriter() );
        hr = writer->Initialize( desc );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed setting up VB writer (%08X):\n%ls\n", static_cast<unsigned int>(hr), szPath );
            continue;
        }

        std::unique_ptr<uint8_t[]> vb( new uint8_t[ nVerts * g_VBMedia[index].stride ] );

        hr = writer->AddStream( vb.get(), nVerts, 0, g_VBMedia[index].stride );
        if ( FAILED(hr) )
        {
            success = false;
            printe( "ERROR: Failed setting up stream for VB writer (%08X):\n%ls\n", static_cast<unsigned int>(hr), szPath );
            continue;
        }

        auto temp = make_AlignedArrayXMVECTOR(nVerts);

        for( size_t j = 0; j < g_VBMedia[index].ilNumElements; ++j )
        {
            memset( temp.get(), 0xff, sizeof(XMVECTOR) * nVerts );

            hr = reader->Read( temp.get(), g_VBMedia[index].ilDesc[ j ].SemanticName, g_VBMedia[index].ilDesc[ j ].SemanticIndex, nVerts );
            if ( FAILED(hr) )
            {
                success = false;
                printe( "ERROR: Failed reading VB %s %u (%08X):\n%ls\n",
                        g_VBMedia[index].ilDesc[ j ].SemanticName, g_VBMedia[index].ilDesc[ j ].SemanticIndex, static_cast<unsigned int>(hr), szPath );
            }
            else
            {
                hr = writer->Write( temp.get(), g_VBMedia[index].ilDesc[ j ].SemanticName, g_VBMedia[index].ilDesc[ j ].SemanticIndex, nVerts );
                if ( FAILED(hr) )
                {
                    success = false;
                    printe( "ERROR: Failed writing VB writer %s %u (%08X):\n%ls\n",
                            g_VBMedia[index].ilDesc[ j ].SemanticName, g_VBMedia[index].ilDesc[ j ].SemanticIndex, static_cast<unsigned int>(hr), szPath );
                }
            }
        }

        if ( memcmp( mesh->vertices.data(), vb.get(), nVerts * g_VBMedia[index].stride ) == 0 )
        {
            ++npass;
        }
        else
        {
            success = false;
            printe( "ERROR: Failed comparing VB read/write cycle results:\n%ls\n", szPath );
        }
    }

    print("%zu meshes tested, %zu meshes passed ", ncount, npass );

    return success;
}
