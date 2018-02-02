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

    // Cube
    {
        std::unique_ptr<uint32_t[]> remap(new uint32_t[8]);
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 8);

        size_t ntests = 0;
        HRESULT hr = WeldVertices(8, s_cubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return true; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices cube identity failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests > 0)
        {
            printe("ERROR: WeldVertices cube identity failed (%Iu .. 0)\n", ntests);
            success = false;
        }
        else if (memcmp(s_cubePointReps, remap.get(), sizeof(uint32_t) * 8) != 0)
        {
            printe("ERROR: WeldVertices cube identity failed\n");
            success = false;
        }

        // invalid args
        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        hr = WeldVertices(UINT32_MAX, s_cubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != E_INVALIDARG)
        {
            printe("ERROR: WeldVertices expected failure for 32-max value verts (%08X)\n", hr);
            success = false;
        }
        hr = WeldVertices(8, nullptr, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != E_INVALIDARG)
        {
            printe("ERROR: WeldVertices expected failure for nullptr point reps (%08X)\n", hr);
            success = false;
        }
        hr = WeldVertices(8, s_cubePointReps, nullptr, [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != E_INVALIDARG)
        {
            printe("ERROR: WeldVertices expected failure for nullptr remap (%08X)\n", hr);
            success = false;
        }
        #pragma warning(pop)

        static const uint32_t s_badReps[8] =
        {
            0, 1, 2, 23, 4, 5, 6, 7,
        };

        hr = WeldVertices(8, s_badReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != E_UNEXPECTED)
        {
            printe("ERROR: WeldVertices expected failure for  bad vert count (%08X)\n", hr);
            success = false;
        }

        // Unused
        hr = WeldVertices(8, s_unused, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices cube unused failed (%08X)\n", hr);
            success = false;
        }
        hr = WeldVertices(8, s_unusedfirst, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices cube unused 1st failed (%08X)\n", hr);
            success = false;
        }
        hr = WeldVertices(8, s_unusedall, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { return true; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices cube unused all failed (%08X)\n", hr);
            success = false;
        }
    }

    // Face-mapped cube
    {
        std::unique_ptr<uint32_t[]> remap(new uint32_t[24]);
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);

        size_t ntests = 0;
        HRESULT hr = WeldVertices(24, s_fmCubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return false; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices fmcube A failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 32)
        {
            printe("ERROR: WeldVertices fmcube A failed (%Iu .. 32)\n", ntests);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices16, 12, remap.get(), 24))
        {
            printe("ERROR: WeldVertices fmcube A remap invalid\n");
            success = false;
        }

        ntests = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(24, s_fmCubePointReps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return true; });
        if (FAILED(hr))
        {
            printe("ERROR: WeldVertices fmcube B failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 16)
        {
            printe("ERROR: WeldVertices fmcube B failed (%Iu .. 16)\n", ntests);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices16, 12, remap.get(), 24, true))
        {
            printe("ERROR: WeldVertices fmcube B remap invalid\n");
            success = false;
        }

        // Epsilon
        ntests = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(24, s_fmCubePointRepsEps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return false; });
        if (hr != S_FALSE)
        {
            printe("ERROR: WeldVertices fmcube C failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 32)
        {
            printe("ERROR: WeldVertices fmcube C failed (%Iu .. 32)\n", ntests);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices16, 12, remap.get(), 24))
        {
            printe("ERROR: WeldVertices fmcube C remap invalid\n");
            success = false;
        }

        ntests = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(24, s_fmCubePointRepsEps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool { ++ntests;  return true; });
        if (FAILED(hr))
        {
            printe("ERROR: WeldVertices fmcube D failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 16)
        {
            printe("ERROR: WeldVertices fmcube D failed (%Iu .. 16)\n", ntests);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices16, 12, remap.get(), 24, true))
        {
            printe("ERROR: WeldVertices fmcube D remap invalid\n");
            success = false;
        }

        // Position epsilon
        ntests = 0;
        size_t nwelds = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(24, s_fmCubePointRepsEps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool
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
            printe("ERROR: WeldVertices fmcube E failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 16 && nwelds != 16)
        {
            printe("ERROR: WeldVertices fmcube E failed (%Iu .. 16, %Iu .. 16)\n", ntests, nwelds);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices16, 12, remap.get(), 24, true))
        {
            printe("ERROR: WeldVertices fmcube E remap invalid\n");
            success = false;
        }
        else
        {
            std::unique_ptr<uint16_t[]> newIndices(new uint16_t[12 * 3]);
            hr = FinalizeIB(g_fmCubeIndices16, 12, remap.get(), 24, newIndices.get());
            if (FAILED(hr))
            {
                printe("ERROR: WeldVertices fmcube E failed finalize IB (%08X)\n", hr);
                success = false;
            }
            else if (memcmp(g_fmCubeIndices16, newIndices.get(), sizeof(uint16_t) * 12 * 3) == 0)
            {
                printe("ERROR: WeldVertices fmcube E failed to change order of vertices\n");
                success = false;
            }
            else
            {
                std::vector<TestVertex> vertices;
                for (size_t j = 0; j < 24; ++j)
                {
                    TestVertex vert = { g_fmCubeVerts[j], g_fmCubeUVs[j] };
                    vertices.push_back(vert);
                }

                hr = FinalizeVB(vertices.data(), sizeof(TestVertex), vertices.size(), remap.get());
                if (FAILED(hr))
                {
                    printe("ERROR: WeldVertices fmcube E failed finalize VB (%08X)\n", hr);
                    success = false;
                }
            }
        }

        // Position, Normal, & UV epsilon
        // TODO -

        // Position & UV epsilon
        ntests = 0;
        nwelds = 0;
        memset(remap.get(), 0xcd, sizeof(uint32_t) * 24);
        hr = WeldVertices(24, s_fmCubePointRepsEps, remap.get(), [&](uint32_t v0, uint32_t v1) -> bool
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
            printe("ERROR: WeldVertices fmcube F failed (%08X)\n", hr);
            success = false;
        }
        else if (ntests != 16 && nwelds != 4)
        {
            printe("ERROR: WeldVertices fmcube F failed (%Iu .. 16, %Iu .. 4)\n", ntests, nwelds);
            success = false;
        }
        else if (!IsValidVertexRemap(g_fmCubeIndices16, 12, remap.get(), 24, true))
        {
            printe("ERROR: WeldVertices fmcube F remap invalid\n");
            success = false;
        }
        else
        {
            std::unique_ptr<uint16_t[]> newIndices(new uint16_t[12 * 3]);
            hr = FinalizeIB(g_fmCubeIndices16, 12, remap.get(), 24, newIndices.get());
            if (FAILED(hr))
            {
                printe("ERROR: WeldVertices fmcube F failed finalize IB (%08X)\n", hr);
                success = false;
            }
            else if (memcmp(g_fmCubeIndices16, newIndices.get(), sizeof(uint16_t) * 12 * 3) == 0)
            {
                printe("ERROR: WeldVertices fmcube F failed to change order of vertices\n");
                success = false;
            }
            else
            {
                size_t trailingUnused;
                hr = OptimizeVertices(newIndices.get(), 12, 24, remap.get(), &trailingUnused);
                if (FAILED(hr))
                {
                    printe("ERROR: WeldVertices fmcube F failed doing OptimizeVertices (%08X)\n", hr);
                    success = false;
                }
                else if (!IsValidVertexRemap(newIndices.get(), 12, remap.get(), 24))
                {
                    printe("ERROR: WeldVertices fmcube F failed remap invalid\n");
                    success = false;
                    for (size_t j = 0; j < 24; ++j)
                        print("%Iu -> %u\n", j, remap[j]);
                }
                else if (trailingUnused != 4)
                {
                    printe("ERROR: WeldVertices fmcube F optimize failed to produce 4 unused slots\n");
                    success = false;
                }
                else if (remap[23] != uint32_t(-1) || remap[22] != uint32_t(-1) || remap[21] != uint32_t(-1) || remap[20] != uint32_t(-1))
                {
                    printe("ERROR: WeldVertices fmcube F optimize failed to place unused slots at end\n");
                    success = false;
                }
                else
                {
                    std::unique_ptr<uint16_t[]> finalIndices(new uint16_t[12 * 3]);
                    hr = FinalizeIB(newIndices.get(), 12, remap.get(), 24, finalIndices.get());
                    if (FAILED(hr))
                    {
                        printe("ERROR: WeldVertices fmcube F failed optimize IB (%08X)\n", hr);
                        success = false;
                    }
                    else if (memcmp(g_fmCubeIndices16, finalIndices.get(), sizeof(uint16_t) * 12 * 3) == 0
                             || memcmp(newIndices.get(), finalIndices.get(), sizeof(uint16_t) * 12 * 3) == 0)
                    {
                        printe("ERROR: WeldVertices fmcube F optimize failed to change order of vertices\n");
                        success = false;
                    }
                    else
                    {
                        std::vector<TestVertex> vertices;
                        for (size_t j = 0; j < 24; ++j)
                        {
                            TestVertex vert = { g_fmCubeVerts[j], g_fmCubeUVs[j] };
                            vertices.push_back(vert);
                        }

                        // TODO - CompactVB instead of FinalizeVB

                        hr = FinalizeVB(vertices.data(), sizeof(TestVertex), vertices.size(), remap.get());
                        if (FAILED(hr))
                        {
                            printe("ERROR: WeldVertices fmcube F failed finalize VB (%08X)\n", hr);
                            success = false;
                        }
                    }
                }
            }
        }
    }

    return success;
}