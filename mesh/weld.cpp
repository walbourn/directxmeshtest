//-------------------------------------------------------------------------------------
// weld.cpp
//  
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#include "directxtest.h"
#include "TestHelpers.h"
#include "TestGeometry.h"
#include "ShapesGenerator.h"

#include "directxmesh.h"

#include "TestHelpers.h"
#include "TestGeometry.h"

using namespace DirectX;
using namespace TestGeometry;

namespace
{
    const XMVECTORF32 s_Epsilon = { { { 1e-20f, 1e-20f, 1e-20f, 1e-20f } } };

    // Cube
    const uint32_t s_cubePointReps[8] =
    {
        0, 1, 2, 3, 4, 5, 6, 7,
    };

    // Face-mapped cube
    const uint32_t s_fmCubePointReps[24] =
    {
        0, 1, 2, 3, 4, 5,
        6, 7, 7, 4, 0, 3,
        6, 5, 1, 2, 4, 5,
        1, 0, 7, 6, 2, 3,
    };

    const uint32_t s_fmCubePointRepsEps[24] =
    {
        19, 18, 22,  3, 16, 17,
        12, 7,   7, 16, 19,  3,
        12, 17, 18, 22, 16, 17,
        18, 19,  7, 12, 22,  3,
    };

    const XMFLOAT3 s_fmCubeNormals[24] =
    {
        XMFLOAT3(0.f, 1.f, 0.f),
        XMFLOAT3(0.f, 1.f, 0.f),
        XMFLOAT3(0.f, 1.f, 0.f),
        XMFLOAT3(0.f, 1.f, 0.f),
        XMFLOAT3(0.f, -1.f, 0.f),
        XMFLOAT3(0.f, -1.f, 0.f),
        XMFLOAT3(0.f, -1.f, 0.f),
        XMFLOAT3(0.f, -1.f, 0.f),
        XMFLOAT3(-1.f, 0.f, 0.f),
        XMFLOAT3(-1.f, 0.f, 0.f),
        XMFLOAT3(-1.f, 0.f, 0.f),
        XMFLOAT3(-1.f, 0.f, 0.f),
        XMFLOAT3(1.f, 0.f, 0.f),
        XMFLOAT3(1.f, 0.f, 0.f),
        XMFLOAT3(1.f, 0.f, 0.f),
        XMFLOAT3(1.f, 0.f, 0.f),
        XMFLOAT3(0.f, 0.f, -1.f),
        XMFLOAT3(0.f, 0.f, -1.f),
        XMFLOAT3(0.f, 0.f, -1.f),
        XMFLOAT3(0.f, 0.f, -1.f),
        XMFLOAT3(0.f, 0.f, 1.f),
        XMFLOAT3(0.f, 0.f, 1.f),
        XMFLOAT3(0.f, 0.f, 1.f),
        XMFLOAT3(0.f, 0.f, 1.f),
    };

    // Unused
    const uint32_t s_unused[8] =
    {
        0, 1, 2, uint32_t(-1), 4, 5, 6, 7,
    };

    const uint32_t s_unusedfirst[8] =
    {
        uint32_t(-1), 1, 2, 3, 4, 5, 6, 7,
    };

    const uint32_t s_unusedall[8] =
    {
        uint32_t(-1), uint32_t(-1), uint32_t(-1), uint32_t(-1), uint32_t(-1), uint32_t(-1), uint32_t(-1), uint32_t(-1),
    };

    struct TestVertex
    {
        XMFLOAT3 position;
        XMFLOAT2 texcoord;
    };
}

//-------------------------------------------------------------------------------------
bool Test26()
{
    bool success = true;

    // 16-bit Cube
    {
        std::unique_ptr<uint32_t[]> remap(new uint32_t[8]);
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 8);

        std::unique_ptr<uint16_t[]> newIndices(new uint16_t[12 * 3]);
        memcpy(newIndices.get(), g_cubeIndices16, sizeof(g_cubeIndices16));

        size_t ntests = 0;
        HRESULT hr = WeldVertices(newIndices.get(), 12, 8, s_cubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return true; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices(16) cube identity failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests > 0)
        {
            printe("ERROR: WeldVertices(16) cube identity failed (%Iu .. 0)\n", ntests);
            success = false;
        }
        else if (memcmp(s_cubePointReps, remap.get(), sizeof(uint32_t) * 8) != 0)
        {
            printe("ERROR: WeldVertices(16) cube identity failed\n");
            success = false;
        }

        // null remap
        memcpy(newIndices.get(), g_cubeIndices16, sizeof(g_cubeIndices16));
        ntests = 0;
        hr = WeldVertices(newIndices.get(), 12, 8, s_cubePointReps, nullptr, [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return true; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices(16) cube identity [null remap] failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests > 0)
        {
            printe("ERROR: WeldVertices(16) cube identity [null remap] failed (%Iu .. 0)\n", ntests);
            success = false;
        }

        // invalid args
        memcpy(newIndices.get(), g_cubeIndices16, sizeof(g_cubeIndices16));

        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = WeldVertices(newIndices.get(), 12, D3D11_16BIT_INDEX_STRIP_CUT_VALUE, s_cubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != E_INVALIDARG)
        {
            printe("ERROR: WeldVertices(16) expected failure for strip cut verts (%08X)\n", hr);
            success = false;
        }
        hr = WeldVertices(newIndices.get(), 12, UINT32_MAX, s_cubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != E_INVALIDARG)
        {
            printe("ERROR: WeldVertices(16) expected failure for 32-max value verts (%08X)\n", hr);
            success = false;
        }
        hr = WeldVertices(newIndices.get(), UINT32_MAX, 8, s_cubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW))
        {
            printe("\nERROR: WeldVertices(16) expected failure for 32-max value faces (%08X)\n", hr);
            success = false;
        }
        hr = WeldVertices(newIndices.get(), 12, 8, nullptr, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != E_INVALIDARG)
        {
            printe("ERROR: WeldVertices(16) expected failure for nullptr point reps (%08X)\n", hr);
            success = false;
        }
        #pragma warning(pop)

        static const uint32_t s_badReps[8] =
        {
            0, 1, 2, 23, 4, 5, 6, 7,
        };

        hr = WeldVertices(newIndices.get(), 12, 8, s_badReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != E_UNEXPECTED)
        {
            printe("ERROR: WeldVertices(16) expected failure for  bad vert count (%08X)\n", hr);
            success = false;
        }

        // Unused
        hr = WeldVertices(newIndices.get(), 12, 8, s_unused, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices(16) cube unused failed (%08X)\n", hr);
            success = false;
        }
        hr = WeldVertices(newIndices.get(), 12, 8, s_unusedfirst, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices(16) cube unused 1st failed (%08X)\n", hr);
            success = false;
        }
        hr = WeldVertices(newIndices.get(), 12, 8, s_unusedall, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices(16) cube unused all failed (%08X)\n", hr);
            success = false;
        }
    }

    // 16-bit Face-mapped cube
    {
        std::unique_ptr<uint32_t[]> remap(new uint32_t[24]);
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);

        std::unique_ptr<uint16_t[]> newIndices(new uint16_t[12 * 3]);
        memcpy(newIndices.get(), g_fmCubeIndices16, sizeof(g_fmCubeIndices16));

        size_t ntests = 0;
        HRESULT hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return false; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices(16) fmcube A failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 32)
        {
            printe("ERROR: WeldVertices(16) fmcube A failed (%Iu .. 32)\n", ntests);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices16, 12, remap.get(), 24))
        {
            printe("ERROR: WeldVertices(16) fmcube A remap invalid\n");
            success = false;
        }

        memcpy(newIndices.get(), g_fmCubeIndices16, sizeof(g_fmCubeIndices16));
        ntests = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return true; });
        if (hr != S_OK)
        {
            printe("ERROR: WeldVertices(16) fmcube B failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 16)
        {
            printe("ERROR: WeldVertices(16) fmcube B failed (%Iu .. 16)\n", ntests);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices16, 12, remap.get(), 24, true, true))
        {
            printe("ERROR: WeldVertices(16) fmcube B remap invalid\n");
            success = false;
        }

        // Position
        memcpy(newIndices.get(), g_fmCubeIndices16, sizeof(g_fmCubeIndices16));
        ntests = 0;
        size_t nwelds = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool
        {
            ++ntests;

            XMVECTOR vA = XMLoadFloat3(&g_fmCubeVerts[v0]);
            XMVECTOR vB = XMLoadFloat3(&g_fmCubeVerts[v1]);

            if (XMVector3NearEqual(vA, vB, s_Epsilon))
            {
                ++nwelds;
                return true;
            }
            return false;
        });
        if (FAILED(hr))
        {
            printe("ERROR: WeldVertices(16) fmcube C failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 16 && nwelds != 16)
        {
            printe("ERROR: WeldVertices(16) fmcube C failed (%Iu .. 16, %Iu .. 16)\n", ntests, nwelds);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices16, 12, remap.get(), 24, true, true))
        {
            printe("ERROR: WeldVertices(16) fmcube C remap invalid\n");
            success = false;
        }
        else if (memcmp(g_fmCubeIndices16, newIndices.get(), sizeof(g_fmCubeIndices16)) == 0)
        {
            printe("ERROR: WeldVertices(16) fmcube C indices unchanged\n");
            success = false;
        }

        // Position, Normal, & UV
        memcpy(newIndices.get(), g_fmCubeIndices16, sizeof(g_fmCubeIndices16));
        ntests = 0;
        nwelds = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool
        {
            ++ntests;

            XMVECTOR vA = XMLoadFloat3(&g_fmCubeVerts[v0]);
            XMVECTOR vB = XMLoadFloat3(&g_fmCubeVerts[v1]);

            XMVECTOR nA = XMLoadFloat3(&s_fmCubeNormals[v0]);
            XMVECTOR nB = XMLoadFloat3(&s_fmCubeNormals[v1]);

            XMVECTOR uvA = XMLoadFloat2(&g_fmCubeUVs[v0]);
            XMVECTOR uvB = XMLoadFloat2(&g_fmCubeUVs[v1]);

            if (XMVector3NearEqual(vA, vB, s_Epsilon))
            {
                if (XMVector3NearEqual(nA, nB, s_Epsilon))
                {
                    if (XMVector2NearEqual(uvA, uvB, s_Epsilon))
                    {
                        ++nwelds;
                        return true;
                    }
                }
            }

            return false;
        });
        if (FAILED(hr))
        {
            printe("ERROR: WeldVertices(16) fmcube D failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 32 && nwelds != 0)
        {
            printe("ERROR: WeldVertices(16) fmcube D failed (%Iu .. 32, %Iu .. 0)\n", ntests, nwelds);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices16, 12, remap.get(), 24, true))
        {
            printe("ERROR: WeldVertices(16) fmcube D remap invalid\n");
            success = false;
        }
        else if (memcmp(g_fmCubeIndices16, newIndices.get(), sizeof(g_fmCubeIndices16)) != 0)
        {
            printe("ERROR: WeldVertices(16) fmcube D indices should be unchanged\n");
            success = false;
        }

        // Position & UV
        memcpy(newIndices.get(), g_fmCubeIndices16, sizeof(g_fmCubeIndices16));
        ntests = 0;
        nwelds = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool
        {
            ++ntests;

            XMVECTOR vA = XMLoadFloat3(&g_fmCubeVerts[v0]);
            XMVECTOR vB = XMLoadFloat3(&g_fmCubeVerts[v1]);

            XMVECTOR uvA = XMLoadFloat2(&g_fmCubeUVs[v0]);
            XMVECTOR uvB = XMLoadFloat2(&g_fmCubeUVs[v1]);

            if (XMVector3NearEqual(vA, vB, s_Epsilon))
            {
                if (XMVector2NearEqual(uvA, uvB, s_Epsilon))
                {
                    ++nwelds;
                    return true;
                }
            }

            return false;
        });
        if (FAILED(hr))
        {
            printe("ERROR: WeldVertices(16) fmcube E failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 16 && nwelds != 4)
        {
            printe("ERROR: WeldVertices(16) fmcube E failed (%Iu .. 16, %Iu .. 4)\n", ntests, nwelds);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices16, 12, remap.get(), 24, true, true))
        {
            printe("ERROR: WeldVertices(16) fmcube E remap invalid\n");
            success = false;
        }

        // Epsilon
        memcpy(newIndices.get(), g_fmCubeIndices16, sizeof(g_fmCubeIndices16));
        ntests = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointRepsEps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return false; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices(16) fmcube eps A failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 32)
        {
            printe("ERROR: WeldVertices(16) fmcube eps A failed (%Iu .. 32)\n", ntests);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices16, 12, remap.get(), 24))
        {
            printe("ERROR: WeldVertices(16) fmcube eps A remap invalid\n");
            success = false;
        }

        memcpy(newIndices.get(), g_fmCubeIndices16, sizeof(g_fmCubeIndices16));
        ntests = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointRepsEps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return true; });
        if (FAILED(hr))
        {
            printe("ERROR: WeldVertices(16) fmcube eps B failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 16)
        {
            printe("ERROR: WeldVertices(16) fmcube eps B failed (%Iu .. 16)\n", ntests);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices16, 12, remap.get(), 24, true, true))
        {
            printe("ERROR: WeldVertices(16) fmcube eps B remap invalid\n");
            success = false;
        }

    }

    // 32-bit Cube
    {
        std::unique_ptr<uint32_t[]> remap(new uint32_t[8]);
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 8);

        std::unique_ptr<uint32_t[]> newIndices(new uint32_t[12 * 3]);
        memcpy(newIndices.get(), g_cubeIndices32, sizeof(g_cubeIndices32));

        size_t ntests = 0;
        HRESULT hr = WeldVertices(newIndices.get(), 12, 8, s_cubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return true; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices(32) cube identity failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests > 0)
        {
            printe("ERROR: WeldVertices(32) cube identity failed (%Iu .. 0)\n", ntests);
            success = false;
        }
        else if (memcmp(s_cubePointReps, remap.get(), sizeof(uint32_t) * 8) != 0)
        {
            printe("ERROR: WeldVertices(32) cube identity failed\n");
            success = false;
        }

        // null remap
        memcpy(newIndices.get(), g_cubeIndices32, sizeof(g_cubeIndices32));
        ntests = 0;
        hr = WeldVertices(newIndices.get(), 12, 8, s_cubePointReps, nullptr, [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return true; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices(32) cube identity [null remap] failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests > 0)
        {
            printe("ERROR: WeldVertices(32) cube identity [null remap] failed (%Iu .. 0)\n", ntests);
            success = false;
        }

        // invalid args
        memcpy(newIndices.get(), g_cubeIndices32, sizeof(g_cubeIndices32));

        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = WeldVertices(newIndices.get(), 12, D3D11_32BIT_INDEX_STRIP_CUT_VALUE, s_cubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != E_INVALIDARG)
        {
            printe("ERROR: WeldVertices(32) expected failure for strip cut verts (%08X)\n", hr);
            success = false;
        }
        hr = WeldVertices(newIndices.get(), 12, UINT32_MAX, s_cubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != E_INVALIDARG)
        {
            printe("ERROR: WeldVertices(32) expected failure for 32-max value verts (%08X)\n", hr);
            success = false;
        }
        hr = WeldVertices(newIndices.get(), UINT32_MAX, 8, s_cubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW))
        {
            printe("\nERROR: WeldVertices(32) expected failure for 32-max value faces (%08X)\n", hr);
            success = false;
        }
        hr = WeldVertices(newIndices.get(), 12, 8, nullptr, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != E_INVALIDARG)
        {
            printe("ERROR: WeldVertices(32) expected failure for nullptr point reps (%08X)\n", hr);
            success = false;
        }
        #pragma warning(pop)

        static const uint32_t s_badReps[8] =
        {
            0, 1, 2, 23, 4, 5, 6, 7,
        };

        hr = WeldVertices(newIndices.get(), 12, 8, s_badReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != E_UNEXPECTED)
        {
            printe("ERROR: WeldVertices(32) expected failure for  bad vert count (%08X)\n", hr);
            success = false;
        }

        // Unused
        hr = WeldVertices(newIndices.get(), 12, 8, s_unused, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices(32) cube unused failed (%08X)\n", hr);
            success = false;
        }
        hr = WeldVertices(newIndices.get(), 12, 8, s_unusedfirst, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices(32) cube unused 1st failed (%08X)\n", hr);
            success = false;
        }
        hr = WeldVertices(newIndices.get(), 12, 8, s_unusedall, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices(32) cube unused all failed (%08X)\n", hr);
            success = false;
        }
    }

    // 32-bit Face-mapped cube
    {
        std::unique_ptr<uint32_t[]> remap(new uint32_t[24]);
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);

        std::unique_ptr<uint32_t[]> newIndices(new uint32_t[12 * 3]);
        memcpy(newIndices.get(), g_fmCubeIndices32, sizeof(g_fmCubeIndices32));

        size_t ntests = 0;
        HRESULT hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return false; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices(32) fmcube A failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 32)
        {
            printe("ERROR: WeldVertices(32) fmcube A failed (%Iu .. 32)\n", ntests);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices32, 12, remap.get(), 24))
        {
            printe("ERROR: WeldVertices(32) fmcube A remap invalid\n");
            success = false;
        }

        memcpy(newIndices.get(), g_fmCubeIndices32, sizeof(g_fmCubeIndices32));
        ntests = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return true; });
        if (hr != S_OK)
        {
            printe("ERROR: WeldVertices(32) fmcube B failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 16)
        {
            printe("ERROR: WeldVertices(32) fmcube B failed (%Iu .. 16)\n", ntests);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices32, 12, remap.get(), 24, true, true))
        {
            printe("ERROR: WeldVertices(32) fmcube B remap invalid\n");
            success = false;
        }

        // Position
        memcpy(newIndices.get(), g_fmCubeIndices32, sizeof(g_fmCubeIndices32));
        ntests = 0;
        size_t nwelds = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool
        {
            ++ntests;

            XMVECTOR vA = XMLoadFloat3(&g_fmCubeVerts[v0]);
            XMVECTOR vB = XMLoadFloat3(&g_fmCubeVerts[v1]);

            if (XMVector3NearEqual(vA, vB, s_Epsilon))
            {
                ++nwelds;
                return true;
            }
            return false;
        });
        if (FAILED(hr))
        {
            printe("ERROR: WeldVertices(32) fmcube C failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 16 && nwelds != 16)
        {
            printe("ERROR: WeldVertices(32) fmcube C failed (%Iu .. 16, %Iu .. 16)\n", ntests, nwelds);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices32, 12, remap.get(), 24, true, true))
        {
            printe("ERROR: WeldVertices(32) fmcube C remap invalid\n");
            success = false;
        }
        else if (memcmp(g_fmCubeIndices32, newIndices.get(), sizeof(g_fmCubeIndices32)) == 0)
        {
            printe("ERROR: WeldVertices(32) fmcube C indices unchanged\n");
            success = false;
        }

        // Position, Normal, & UV
        memcpy(newIndices.get(), g_fmCubeIndices32, sizeof(g_fmCubeIndices32));
        ntests = 0;
        nwelds = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool
        {
            ++ntests;

            XMVECTOR vA = XMLoadFloat3(&g_fmCubeVerts[v0]);
            XMVECTOR vB = XMLoadFloat3(&g_fmCubeVerts[v1]);

            XMVECTOR nA = XMLoadFloat3(&s_fmCubeNormals[v0]);
            XMVECTOR nB = XMLoadFloat3(&s_fmCubeNormals[v1]);

            XMVECTOR uvA = XMLoadFloat2(&g_fmCubeUVs[v0]);
            XMVECTOR uvB = XMLoadFloat2(&g_fmCubeUVs[v1]);

            if (XMVector3NearEqual(vA, vB, s_Epsilon))
            {
                if (XMVector3NearEqual(nA, nB, s_Epsilon))
                {
                    if (XMVector2NearEqual(uvA, uvB, s_Epsilon))
                    {
                        ++nwelds;
                        return true;
                    }
                }
            }

            return false;
        });
        if (FAILED(hr))
        {
            printe("ERROR: WeldVertices(32) fmcube D failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 32 && nwelds != 0)
        {
            printe("ERROR: WeldVertices(32) fmcube D failed (%Iu .. 32, %Iu .. 0)\n", ntests, nwelds);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices32, 12, remap.get(), 24, true))
        {
            printe("ERROR: WeldVertices(32) fmcube D remap invalid\n");
            success = false;
        }
        else if (memcmp(g_fmCubeIndices32, newIndices.get(), sizeof(g_fmCubeIndices32)) != 0)
        {
            printe("ERROR: WeldVertices(32) fmcube D indices should be unchanged\n");
            success = false;
        }

        // Position & UV
        memcpy(newIndices.get(), g_fmCubeIndices32, sizeof(g_fmCubeIndices32));
        ntests = 0;
        nwelds = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool
        {
            ++ntests;

            XMVECTOR vA = XMLoadFloat3(&g_fmCubeVerts[v0]);
            XMVECTOR vB = XMLoadFloat3(&g_fmCubeVerts[v1]);

            XMVECTOR uvA = XMLoadFloat2(&g_fmCubeUVs[v0]);
            XMVECTOR uvB = XMLoadFloat2(&g_fmCubeUVs[v1]);

            if (XMVector3NearEqual(vA, vB, s_Epsilon))
            {
                if (XMVector2NearEqual(uvA, uvB, s_Epsilon))
                {
                    ++nwelds;
                    return true;
                }
            }

            return false;
        });
        if (FAILED(hr))
        {
            printe("ERROR: WeldVertices(32) fmcube E failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 16 && nwelds != 4)
        {
            printe("ERROR: WeldVertices(32) fmcube E failed (%Iu .. 16, %Iu .. 4)\n", ntests, nwelds);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices32, 12, remap.get(), 24, true, true))
        {
            printe("ERROR: WeldVertices(32) fmcube E remap invalid\n");
            success = false;
        }

        // Epsilon
        memcpy(newIndices.get(), g_fmCubeIndices32, sizeof(g_fmCubeIndices32));
        ntests = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointRepsEps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return false; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices(32) fmcube eps A failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 32)
        {
            printe("ERROR: WeldVertices(32) fmcube eps A failed (%Iu .. 32)\n", ntests);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices32, 12, remap.get(), 24))
        {
            printe("ERROR: WeldVertices(32) fmcube eps A remap invalid\n");
            success = false;
        }

        memcpy(newIndices.get(), g_fmCubeIndices32, sizeof(g_fmCubeIndices32));
        ntests = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointRepsEps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return true; });
        if (FAILED(hr))
        {
            printe("ERROR: WeldVertices(32) fmcube eps B failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 16)
        {
            printe("ERROR: WeldVertices(32) fmcube eps B failed (%Iu .. 16)\n", ntests);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices32, 12, remap.get(), 24, true, true))
        {
            printe("ERROR: WeldVertices(32) fmcube eps B remap invalid\n");
            success = false;
        }
    }

    return success;
}

//-------------------------------------------------------------------------------------
bool Test27()
{
    bool success = true;

    // 16-bit Face-mapped cube
    {
        std::unique_ptr<uint16_t[]> newIndices(new uint16_t[12 * 3]);
        memcpy(newIndices.get(), g_fmCubeIndices16, sizeof(g_fmCubeIndices16));

        // Position only
        HRESULT hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointReps, nullptr, [&](uint32_t v0, uint32_t v1) -> bool
        {
            XMVECTOR vA = XMLoadFloat3(&g_fmCubeVerts[v0]);
            XMVECTOR vB = XMLoadFloat3(&g_fmCubeVerts[v1]);

            if (XMVector3NearEqual(vA, vB, s_Epsilon))
            {
                return true;
            }
            return false;
        });
        if (hr != S_OK)
        {
            printe("ERROR: WeldVertices(16) failed (%08X)\n", hr);
            success = false;
        }
        else if (memcmp(g_fmCubeIndices16, newIndices.get(), sizeof(g_fmCubeIndices16)) == 0)
        {
            printe("ERROR: WeldVertices(16) indices unchanged\n");
            success = false;
        }
        else
        {
            std::unique_ptr<uint32_t[]> remap(new uint32_t[24]);
            memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
            size_t trailingUnused;
            hr = OptimizeVertices(newIndices.get(), 12, 24, remap.get(), &trailingUnused);
            if (FAILED(hr))
            {
                printe("ERROR: OptimizeVertices(16) failed (%08X)\n", hr);
                success = false;
            }
            else if (trailingUnused != 16)
            {
                printe("ERROR: OptimizeVertices(16) failed to produce enough unused slots (%Iu .. 16)\n", trailingUnused);
                success = false;
            }
            else if (!IsValidVertexRemap(newIndices.get(), 12, remap.get(), 24))
            {
                printe("ERROR: OptimizeVertices(16) failed remap invalid\n");
                success = false;
                for (size_t j = 0; j < 24; ++j)
                    print("%Iu -> %u\n", j, remap[j]);
            }
            else if (remap[23] != uint32_t(-1) || remap[22] != uint32_t(-1) || remap[21] != uint32_t(-1) || remap[20] != uint32_t(-1)
                || remap[19] != uint32_t(-1) || remap[18] != uint32_t(-1) || remap[17] != uint32_t(-1) || remap[16] != uint32_t(-1)
                || remap[15] != uint32_t(-1) || remap[14] != uint32_t(-1) || remap[13] != uint32_t(-1) || remap[12] != uint32_t(-1)
                || remap[11] != uint32_t(-1) || remap[10] != uint32_t(-1) || remap[9] != uint32_t(-1) || remap[8] != uint32_t(-1)
                || remap[7] == uint32_t(-1) || remap[6] == uint32_t(-1) || remap[5] == uint32_t(-1) || remap[4] == uint32_t(-1)
                || remap[3] == uint32_t(-1) || remap[2] == uint32_t(-1) || remap[1] == uint32_t(-1) || remap[0] == uint32_t(-1)
                )
            {
                printe("ERROR: OptimizeVertices(16) failed to place unused slots at end\n");
                success = false;
            }
            else
            {
                std::unique_ptr<uint16_t[]> finalIndices(new uint16_t[12 * 3]);
                hr = FinalizeIB(newIndices.get(), 12, remap.get(), 24, finalIndices.get());
                if (FAILED(hr))
                {
                    printe("ERROR: FinalizeIB(16) failed (%08X)\n", hr);
                    success = false;
                }
                else if (memcmp(g_fmCubeIndices16, finalIndices.get(), sizeof(uint16_t) * 12 * 3) == 0
                    || memcmp(newIndices.get(), finalIndices.get(), sizeof(uint16_t) * 12 * 3) == 0)
                {
                    printe("ERROR: FinalizeIB(16) failed to change order of vertices\n");
                    success = false;
                }
                else
                {
                    // TestVertex
                    std::unique_ptr<TestVertex> finalVertices(new TestVertex[16]);
                    memset(finalVertices.get(), 0xcd, sizeof(TestVertex) * 16);

                    std::vector<TestVertex> vertices;
                    for (size_t j = 0; j < 24; ++j)
                    {
                        TestVertex vert = { g_fmCubeVerts[j], g_fmCubeUVs[j] };
                        vertices.push_back(vert);
                    }

                    hr = CompactVB(vertices.data(), sizeof(TestVertex), vertices.size(), trailingUnused, remap.get(), finalVertices.get());
                    if (FAILED(hr))
                    {
                        printe("ERROR: CompactVB(20) 16-bit failed (%08X)\n", hr);
                        success = false;
                    }

                    // uint32_t
                    std::unique_ptr<uint32_t[]> vertUInt(new uint32_t[24]);
                    for (uint32_t j = 0; j < 24; ++j)
                    {
                        vertUInt[j] = j;
                    }

                    std::unique_ptr<uint32_t[]> finalUInt(new uint32_t[24]);
                    memset(finalUInt.get(), 0xcd, sizeof(uint32_t) * 24);

                    hr = CompactVB(vertUInt.get(), 4, 24, trailingUnused, remap.get(), finalUInt.get());
                    if (FAILED(hr))
                    {
                        printe("ERROR: CompactVB(4) 16-bit failed (%08X)\n", hr);
                        success = false;
                    }

                    for (size_t j = 0; j < 24; ++j)
                    {
                        if (remap[j] == uint32_t(-1))
                        {
                            if (finalUInt[j] != 0xcdcdcdcd)
                            {
                                printe("ERROR: CompactVB(4) 16-bit failed creating correct new VB (%Iu: %u .. 0xcdcdcdcd)\n", j, finalUInt[j]);
                                success = false;
                            }
                        }
                        else if (remap[j] != finalUInt[j])
                        {
                            printe("ERROR: CompactVB(4) 16-bit failed creating correct new VB (%Iu: %u .. %u)\n", j, finalUInt[j], remap[j]);
                            success = false;
                        }
                    }

                    // invalid args
                    #pragma warning(push)
                    #pragma warning(disable:6385 6387)
                    hr = CompactVB(vertices.data(), sizeof(TestVertex), vertices.size(), trailingUnused, nullptr, finalVertices.get());
                    if (SUCCEEDED(hr))
                    {
                        printe("\nERROR: CompactVB nullptr remap expected failure\n");
                        success = false;
                    }
                    hr = CompactVB(vertices.data(), UINT32_MAX, vertices.size(), trailingUnused, remap.get(), finalVertices.get());
                    if (hr != E_INVALIDARG)
                    {
                        printe("\nERROR: CompactVB expected failure for bad stride value (%08X)\n", hr);
                        success = false;
                    }
                    hr = CompactVB(vertices.data(), sizeof(TestVertex), D3D11_32BIT_INDEX_STRIP_CUT_VALUE, trailingUnused, remap.get(), finalVertices.get());
                    if (hr != E_INVALIDARG)
                    {
                        printe("\nERROR: CompactVB expected failure for strip cut value (%08X)\n", hr);
                        success = false;
                    }
                    hr = CompactVB(vertices.data(), sizeof(TestVertex), UINT32_MAX, trailingUnused, remap.get(), finalVertices.get());
                    if (hr != E_INVALIDARG)
                    {
                        printe("\nERROR: CompactVB expected failure for 32-max value verts (%08X)\n", hr);
                        success = false;
                    }
                    hr = CompactVB(vertices.data(), sizeof(TestVertex), vertices.size(), UINT32_MAX, remap.get(), finalVertices.get());
                    if (hr != E_INVALIDARG)
                    {
                        printe("\nERROR: CompactVB expected failure for 32-max value trailing unused (%08X)\n", hr);
                        success = false;
                    }
                    hr = CompactVB(vertices.data(), sizeof(TestVertex), vertices.size(), vertices.size() + 12, remap.get(), finalVertices.get());
                    if (hr != E_INVALIDARG)
                    {
                        printe("\nERROR: CompactVB expected failure for too many unused(%08X)\n", hr);
                        success = false;
                    }
                    #pragma warning(pop)
                }
            }
        }

        // Position & UV
        memcpy(newIndices.get(), g_fmCubeIndices16, sizeof(g_fmCubeIndices16));
        hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointReps, nullptr, [&](uint32_t v0, uint32_t v1) -> bool
        {
            XMVECTOR vA = XMLoadFloat3(&g_fmCubeVerts[v0]);
            XMVECTOR vB = XMLoadFloat3(&g_fmCubeVerts[v1]);

            XMVECTOR uvA = XMLoadFloat2(&g_fmCubeUVs[v0]);
            XMVECTOR uvB = XMLoadFloat2(&g_fmCubeUVs[v1]);

            if (XMVector3NearEqual(vA, vB, s_Epsilon))
            {
                if (XMVector2NearEqual(uvA, uvB, s_Epsilon))
                {
                    return true;
                }
            }

            return false;
        });
        if (hr != S_OK)
        {
            printe("ERROR: WeldVertices(16) position & uv failed (%08X)\n", hr);
            success = false;
        }
        else if (memcmp(g_fmCubeIndices16, newIndices.get(), sizeof(g_fmCubeIndices16)) == 0)
        {
            printe("ERROR: WeldVertices(16) position & uv indices unchanged\n");
            success = false;
        }
        else
        {
            std::unique_ptr<uint32_t[]> remap(new uint32_t[24]);
            memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
            size_t trailingUnused;
            hr = OptimizeVertices(newIndices.get(), 12, 24, remap.get(), &trailingUnused);
            if (FAILED(hr))
            {
                printe("ERROR: OptimizeVertices(16) position & uv failed (%08X)\n", hr);
                success = false;
            }
            else if (trailingUnused != 4)
            {
                printe("ERROR: OptimizeVertices(16) position & uv failed to produce enough unused slots (%Iu .. 16)\n", trailingUnused);
                success = false;
            }
            else if (!IsValidVertexRemap(newIndices.get(), 12, remap.get(), 24))
            {
                printe("ERROR: OptimizeVertices(16) position & uv failed remap invalid\n");
                success = false;
                for (size_t j = 0; j < 24; ++j)
                    print("%Iu -> %u\n", j, remap[j]);
            }
            else if (remap[23] != uint32_t(-1) || remap[22] != uint32_t(-1) || remap[21] != uint32_t(-1) || remap[20] != uint32_t(-1)
                || remap[19] == uint32_t(-1) || remap[18] == uint32_t(-1) || remap[17] == uint32_t(-1) || remap[16] == uint32_t(-1)
                || remap[15] == uint32_t(-1) || remap[14] == uint32_t(-1) || remap[13] == uint32_t(-1) || remap[12] == uint32_t(-1)
                || remap[11] == uint32_t(-1) || remap[10] == uint32_t(-1) || remap[9] == uint32_t(-1) || remap[8] == uint32_t(-1)
                || remap[7] == uint32_t(-1) || remap[6] == uint32_t(-1) || remap[5] == uint32_t(-1) || remap[4] == uint32_t(-1)
                || remap[3] == uint32_t(-1) || remap[2] == uint32_t(-1) || remap[1] == uint32_t(-1) || remap[0] == uint32_t(-1)
                )
            {
                printe("ERROR: OptimizeVertices(16) position & uv failed to place unused slots at end\n");
                success = false;
            }
            else
            {
                std::unique_ptr<uint16_t[]> finalIndices(new uint16_t[12 * 3]);
                hr = FinalizeIB(newIndices.get(), 12, remap.get(), 24, finalIndices.get());
                if (FAILED(hr))
                {
                    printe("ERROR: FinalizeIB(16) position & uv failed (%08X)\n", hr);
                    success = false;
                }
                else if (memcmp(g_fmCubeIndices16, finalIndices.get(), sizeof(uint16_t) * 12 * 3) == 0
                    || memcmp(newIndices.get(), finalIndices.get(), sizeof(uint16_t) * 12 * 3) == 0)
                {
                    printe("ERROR: FinalizeIB(16) position & uv failed to change order of vertices\n");
                    success = false;
                }
                else
                {
                    // TestVertex
                    std::unique_ptr<TestVertex> finalVertices(new TestVertex[20]);
                    memset(finalVertices.get(), 0xcd, sizeof(TestVertex) * 20);

                    std::vector<TestVertex> vertices;
                    for (size_t j = 0; j < 24; ++j)
                    {
                        TestVertex vert = { g_fmCubeVerts[j], g_fmCubeUVs[j] };
                        vertices.push_back(vert);
                    }

                    hr = CompactVB(vertices.data(), sizeof(TestVertex), vertices.size(), trailingUnused, remap.get(), finalVertices.get());
                    if (FAILED(hr))
                    {
                        printe("ERROR: CompactVB(20) 16-bit position & uv failed (%08X)\n", hr);
                        success = false;
                    }

                    // uint32_t
                    std::unique_ptr<uint32_t[]> vertUInt(new uint32_t[24]);
                    for (uint32_t j = 0; j < 24; ++j)
                    {
                        vertUInt[j] = j;
                    }

                    std::unique_ptr<uint32_t[]> finalUInt(new uint32_t[24]);
                    memset(finalUInt.get(), 0xcd, sizeof(uint32_t) * 24);

                    hr = CompactVB(vertUInt.get(), 4, 24, trailingUnused, remap.get(), finalUInt.get());
                    if (FAILED(hr))
                    {
                        printe("ERROR: CompactVB(4) 16-bit position & uv failed (%08X)\n", hr);
                        success = false;
                    }

                    for (size_t j = 0; j < 24; ++j)
                    {
                        if (remap[j] == uint32_t(-1))
                        {
                            if (finalUInt[j] != 0xcdcdcdcd)
                            {
                                printe("ERROR: CompactVB(4) 16-bit position & uv failed creating correct new VB (%Iu: %u .. 0xcdcdcdcd)\n", j, finalUInt[j]);
                                success = false;
                            }
                        }
                        else if (remap[j] != finalUInt[j])
                        {
                            printe("ERROR: CompactVB(4) 16-bit position & uv failed creating correct new VB (%Iu: %u .. %u)\n", j, finalUInt[j], remap[j]);
                            success = false;
                        }
                    }
                }
            }
        }
    }

    // 32-bit Face-mapped cube
    {
        std::unique_ptr<uint32_t[]> newIndices(new uint32_t[12 * 3]);
        memcpy(newIndices.get(), g_fmCubeIndices32, sizeof(g_fmCubeIndices32));

        // Position only
        HRESULT hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointReps, nullptr, [&](uint32_t v0, uint32_t v1) -> bool
        {
            XMVECTOR vA = XMLoadFloat3(&g_fmCubeVerts[v0]);
            XMVECTOR vB = XMLoadFloat3(&g_fmCubeVerts[v1]);

            if (XMVector3NearEqual(vA, vB, s_Epsilon))
            {
                return true;
            }
            return false;
        });
        if (hr != S_OK)
        {
            printe("ERROR: WeldVertices(32) failed (%08X)\n", hr);
            success = false;
        }
        else if (memcmp(g_fmCubeIndices32, newIndices.get(), sizeof(g_fmCubeIndices32)) == 0)
        {
            printe("ERROR: WeldVertices(32) indices unchanged\n");
            success = false;
        }
        else
        {
            std::unique_ptr<uint32_t[]> remap(new uint32_t[24]);
            memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
            size_t trailingUnused;
            hr = OptimizeVertices(newIndices.get(), 12, 24, remap.get(), &trailingUnused);
            if (FAILED(hr))
            {
                printe("ERROR: OptimizeVertices(32) failed (%08X)\n", hr);
                success = false;
            }
            else if (trailingUnused != 16)
            {
                printe("ERROR: OptimizeVertices(32) failed to produce enough unused slots (%Iu .. 16)\n", trailingUnused);
                success = false;
            }
            else if (!IsValidVertexRemap(newIndices.get(), 12, remap.get(), 24))
            {
                printe("ERROR: OptimizeVertices(32) failed remap invalid\n");
                success = false;
                for (size_t j = 0; j < 24; ++j)
                    print("%Iu -> %u\n", j, remap[j]);
            }
            else if (remap[23] != uint32_t(-1) || remap[22] != uint32_t(-1) || remap[21] != uint32_t(-1) || remap[20] != uint32_t(-1)
                || remap[19] != uint32_t(-1) || remap[18] != uint32_t(-1) || remap[17] != uint32_t(-1) || remap[16] != uint32_t(-1)
                || remap[15] != uint32_t(-1) || remap[14] != uint32_t(-1) || remap[13] != uint32_t(-1) || remap[12] != uint32_t(-1)
                || remap[11] != uint32_t(-1) || remap[10] != uint32_t(-1) || remap[9] != uint32_t(-1) || remap[8] != uint32_t(-1)
                || remap[7] == uint32_t(-1) || remap[6] == uint32_t(-1) || remap[5] == uint32_t(-1) || remap[4] == uint32_t(-1)
                || remap[3] == uint32_t(-1) || remap[2] == uint32_t(-1) || remap[1] == uint32_t(-1) || remap[0] == uint32_t(-1)
                )
            {
                printe("ERROR: OptimizeVertices(32) failed to place unused slots at end\n");
                success = false;
            }
            else
            {
                std::unique_ptr<uint32_t[]> finalIndices(new uint32_t[12 * 3]);
                hr = FinalizeIB(newIndices.get(), 12, remap.get(), 24, finalIndices.get());
                if (FAILED(hr))
                {
                    printe("ERROR: FinalizeIB(32) failed (%08X)\n", hr);
                    success = false;
                }
                else if (memcmp(g_fmCubeIndices32, finalIndices.get(), sizeof(uint32_t) * 12 * 3) == 0
                    || memcmp(newIndices.get(), finalIndices.get(), sizeof(uint32_t) * 12 * 3) == 0)
                {
                    printe("ERROR: FinalizeIB(32) failed to change order of vertices\n");
                    success = false;
                }
                else
                {
                    // TestVertex
                    std::unique_ptr<TestVertex> finalVertices(new TestVertex[16]);
                    memset(finalVertices.get(), 0xcd, sizeof(TestVertex) * 16);

                    std::vector<TestVertex> vertices;
                    for (size_t j = 0; j < 24; ++j)
                    {
                        TestVertex vert = { g_fmCubeVerts[j], g_fmCubeUVs[j] };
                        vertices.push_back(vert);
                    }

                    hr = CompactVB(vertices.data(), sizeof(TestVertex), vertices.size(), trailingUnused, remap.get(), finalVertices.get());
                    if (FAILED(hr))
                    {
                        printe("ERROR: CompactVB(20) 32-bit failed (%08X)\n", hr);
                        success = false;
                    }

                    // uint32_t
                    std::unique_ptr<uint32_t[]> vertUInt(new uint32_t[24]);
                    for (uint32_t j = 0; j < 24; ++j)
                    {
                        vertUInt[j] = j;
                    }

                    std::unique_ptr<uint32_t[]> finalUInt(new uint32_t[24]);
                    memset(finalUInt.get(), 0xcd, sizeof(uint32_t) * 24);

                    hr = CompactVB(vertUInt.get(), 4, 24, trailingUnused, remap.get(), finalUInt.get());
                    if (FAILED(hr))
                    {
                        printe("ERROR: CompactVB(4) 32-bit failed (%08X)\n", hr);
                        success = false;
                    }

                    for (size_t j = 0; j < 24; ++j)
                    {
                        if (remap[j] == uint32_t(-1))
                        {
                            if (finalUInt[j] != 0xcdcdcdcd)
                            {
                                printe("ERROR: CompactVB(4) 32-bit failed creating correct new VB (%Iu: %u .. 0xcdcdcdcd)\n", j, finalUInt[j]);
                                success = false;
                            }
                        }
                        else if (remap[j] != finalUInt[j])
                        {
                            printe("ERROR: CompactVB(4) 32-bit failed creating correct new VB (%Iu: %u .. %u)\n", j, finalUInt[j], remap[j]);
                            success = false;
                        }
                    }
                }
            }
        }

        // Position & UV
        memcpy(newIndices.get(), g_fmCubeIndices32, sizeof(g_fmCubeIndices32));
        hr = WeldVertices(newIndices.get(), 12, 24, s_fmCubePointReps, nullptr, [&](uint32_t v0, uint32_t v1) -> bool
        {
            XMVECTOR vA = XMLoadFloat3(&g_fmCubeVerts[v0]);
            XMVECTOR vB = XMLoadFloat3(&g_fmCubeVerts[v1]);

            XMVECTOR uvA = XMLoadFloat2(&g_fmCubeUVs[v0]);
            XMVECTOR uvB = XMLoadFloat2(&g_fmCubeUVs[v1]);

            if (XMVector3NearEqual(vA, vB, s_Epsilon))
            {
                if (XMVector2NearEqual(uvA, uvB, s_Epsilon))
                {
                    return true;
                }
            }

            return false;
        });
        if (hr != S_OK)
        {
            printe("ERROR: WeldVertices(32) position & uv failed (%08X)\n", hr);
            success = false;
        }
        else if (memcmp(g_fmCubeIndices32, newIndices.get(), sizeof(g_fmCubeIndices32)) == 0)
        {
            printe("ERROR: WeldVertices(32) position & uv indices unchanged\n");
            success = false;
        }
        else
        {
            std::unique_ptr<uint32_t[]> remap(new uint32_t[24]);
            memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
            size_t trailingUnused;
            hr = OptimizeVertices(newIndices.get(), 12, 24, remap.get(), &trailingUnused);
            if (FAILED(hr))
            {
                printe("ERROR: OptimizeVertices(32) position & uv failed (%08X)\n", hr);
                success = false;
            }
            else if (trailingUnused != 4)
            {
                printe("ERROR: OptimizeVertices(32) position & uv failed to produce enough unused slots (%Iu .. 16)\n", trailingUnused);
                success = false;
            }
            else if (!IsValidVertexRemap(newIndices.get(), 12, remap.get(), 24))
            {
                printe("ERROR: OptimizeVertices(32) position & uv failed remap invalid\n");
                success = false;
                for (size_t j = 0; j < 24; ++j)
                    print("%Iu -> %u\n", j, remap[j]);
            }
            else if (remap[23] != uint32_t(-1) || remap[22] != uint32_t(-1) || remap[21] != uint32_t(-1) || remap[20] != uint32_t(-1)
                || remap[19] == uint32_t(-1) || remap[18] == uint32_t(-1) || remap[17] == uint32_t(-1) || remap[16] == uint32_t(-1)
                || remap[15] == uint32_t(-1) || remap[14] == uint32_t(-1) || remap[13] == uint32_t(-1) || remap[12] == uint32_t(-1)
                || remap[11] == uint32_t(-1) || remap[10] == uint32_t(-1) || remap[9] == uint32_t(-1) || remap[8] == uint32_t(-1)
                || remap[7] == uint32_t(-1) || remap[6] == uint32_t(-1) || remap[5] == uint32_t(-1) || remap[4] == uint32_t(-1)
                || remap[3] == uint32_t(-1) || remap[2] == uint32_t(-1) || remap[1] == uint32_t(-1) || remap[0] == uint32_t(-1)
                )
            {
                printe("ERROR: OptimizeVertices(32) position & uv failed to place unused slots at end\n");
                success = false;
            }
            else
            {
                std::unique_ptr<uint32_t[]> finalIndices(new uint32_t[12 * 3]);
                hr = FinalizeIB(newIndices.get(), 12, remap.get(), 24, finalIndices.get());
                if (FAILED(hr))
                {
                    printe("ERROR: FinalizeIB(32) position & uv failed (%08X)\n", hr);
                    success = false;
                }
                else if (memcmp(g_fmCubeIndices32, finalIndices.get(), sizeof(uint32_t) * 12 * 3) == 0
                    || memcmp(newIndices.get(), finalIndices.get(), sizeof(uint32_t) * 12 * 3) == 0)
                {
                    printe("ERROR: FinalizeIB(32) position & uv failed to change order of vertices\n");
                    success = false;
                }
                else
                {
                    // TestVertex
                    std::unique_ptr<TestVertex> finalVertices(new TestVertex[20]);
                    memset(finalVertices.get(), 0xcd, sizeof(TestVertex) * 20);

                    std::vector<TestVertex> vertices;
                    for (size_t j = 0; j < 24; ++j)
                    {
                        TestVertex vert = { g_fmCubeVerts[j], g_fmCubeUVs[j] };
                        vertices.push_back(vert);
                    }

                    hr = CompactVB(vertices.data(), sizeof(TestVertex), vertices.size(), trailingUnused, remap.get(), finalVertices.get());
                    if (FAILED(hr))
                    {
                        printe("ERROR: CompactVB(20) 32-bit position & uv failed (%08X)\n", hr);
                        success = false;
                    }

                    // uint32_t
                    std::unique_ptr<uint32_t[]> vertUInt(new uint32_t[24]);
                    for (uint32_t j = 0; j < 24; ++j)
                    {
                        vertUInt[j] = j;
                    }

                    std::unique_ptr<uint32_t[]> finalUInt(new uint32_t[24]);
                    memset(finalUInt.get(), 0xcd, sizeof(uint32_t) * 24);

                    hr = CompactVB(vertUInt.get(), 4, 24, trailingUnused, remap.get(), finalUInt.get());
                    if (FAILED(hr))
                    {
                        printe("ERROR: CompactVB(4) 32-bit position & uv failed (%08X)\n", hr);
                        success = false;
                    }

                    for (size_t j = 0; j < 24; ++j)
                    {
                        if (remap[j] == uint32_t(-1))
                        {
                            if (finalUInt[j] != 0xcdcdcdcd)
                            {
                                printe("ERROR: CompactVB(4) 32-bit position & uv failed creating correct new VB (%Iu: %u .. 0xcdcdcdcd)\n", j, finalUInt[j]);
                                success = false;
                            }
                        }
                        else if (remap[j] != finalUInt[j])
                        {
                            printe("ERROR: CompactVB(4) 32-bit position & uv failed creating correct new VB (%Iu: %u .. %u)\n", j, finalUInt[j], remap[j]);
                            success = false;
                        }
                    }
                }
            }
        }
    }

    return success;
}