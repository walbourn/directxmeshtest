//-------------------------------------------------------------------------------------
// concat.cpp
//  
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#include "directxtest.h"

#include "DirectXMesh.h"

#include "TestHelpers.h"
#include "TestGeometry.h"

using namespace DirectX;
using namespace TestGeometry;

//-------------------------------------------------------------------------------------
// ConcatenateMesh
bool Test30()
{
    bool success = true;

    // cube (identity)
    {
        std::unique_ptr<uint32_t[]> faceDestMap(new uint32_t[12]);
        memset(faceDestMap.get(), 0xcd, sizeof(uint32_t) * 12);

        std::unique_ptr<uint32_t[]> vertDestMap(new uint32_t[8]);
        memset(vertDestMap.get(), 0xcd, sizeof(uint32_t) * 8);

        size_t nFaces = 0;
        size_t nVerts = 0;

        HRESULT hr = ConcatenateMesh(12, 8, faceDestMap.get(), vertDestMap.get(), nFaces, nVerts);
        if (FAILED(hr))
        {
            printe("ERROR: ConcatenateMesh cube failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (nFaces != 12 || nVerts != 8)
        {
            success = false;
            printe("ERROR: ConcatenateMesh cube failed, totals invalid (faces: %zu..12; verts: %zu..8)\n", nFaces, nVerts);
        }
        else if (!IsValidFaceDestMap(g_cubeIndices16, faceDestMap.get(), 12, 0))
        {
            success = false;
            printe("ERROR: ConcatenateMesh cube failed, face map invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, faceDestMap[j]);
        }
        else if (!IsValidVertexDestMap(g_cubeIndices16, 12, vertDestMap.get(), 8, 0))
        {
            printe("ERROR: ConcatenateMesh cube failed vert map invalid\n");
            success = false;
            for (size_t j = 0; j < 8; ++j)
                print("%zu -> %u\n", j, vertDestMap[j]);
        }
    }

    // invalid args
    {
        std::unique_ptr<uint32_t[]> faceDestMap(new uint32_t[12]);
        memset(faceDestMap.get(), 0xcd, sizeof(uint32_t) * 12);

        std::unique_ptr<uint32_t[]> vertDestMap(new uint32_t[8]);
        memset(vertDestMap.get(), 0xcd, sizeof(uint32_t) * 8);

        size_t nFaces = 0;
        size_t nVerts = 0;

        HRESULT hr = ConcatenateMesh(UINT32_MAX, 8, faceDestMap.get(), vertDestMap.get(), nFaces, nVerts);
        if (hr != HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW))
        {
            printe("\nERROR: ConcatenateMesh expected failure for 32-max value faces (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = ConcatenateMesh(12, UINT32_MAX, faceDestMap.get(), vertDestMap.get(), nFaces, nVerts);
        if (hr != E_INVALIDARG)
        {
            printe("\nERROR: ConcatenateMesh expected failure for 32-max value nverts (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        #pragma warning(push)
        #pragma warning(disable:6387)
        hr = ConcatenateMesh(12, 8, nullptr, vertDestMap.get(), nFaces, nVerts);
        if (hr != E_INVALIDARG)
        {
            printe("\nERROR: ConcatenateMesh expected failure for null facemap (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = ConcatenateMesh(12, 8, faceDestMap.get(), nullptr, nFaces, nVerts);
        if (hr != E_INVALIDARG)
        {
            printe("\nERROR: ConcatenateMesh expected failure for null vertmap (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        #pragma warning(pop)

        nFaces = UINT32_MAX;
        nVerts = 1000;
        hr = ConcatenateMesh(12, 8, faceDestMap.get(), vertDestMap.get(), nFaces, nVerts);
        if (hr != E_FAIL)
        {
            printe("\nERROR: ConcatenateMesh expected failure for too many faces (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        nFaces = 1000;
        nVerts = UINT32_MAX;
        hr = ConcatenateMesh(12, 8, faceDestMap.get(), vertDestMap.get(), nFaces, nVerts);
        if (hr != E_FAIL)
        {
            printe("\nERROR: ConcatenateMesh expected failure for too many verts (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
    }

    // cube+box+face-cube
    {
        std::vector<uint32_t> mergedFaces;
        std::vector<uint32_t> mergedIndices;
        std::vector<uint32_t> mergedVerts;

        std::unique_ptr<uint32_t[]> faceDestMap(new uint32_t[12]);
        memset(faceDestMap.get(), 0xcd, sizeof(uint32_t) * 12);

        // max of 8 / 8 / 24
        std::unique_ptr<uint32_t[]> vertDestMap(new uint32_t[24]);
        memset(vertDestMap.get(), 0xcd, sizeof(uint32_t) * 24);

        size_t nFaces = 0;
        size_t nVerts = 0;

        bool passed = true;

        // Cube
        size_t baseFace = nFaces;
        size_t baseVerts = nVerts;
        HRESULT hr = ConcatenateMesh(12, 8, faceDestMap.get(), vertDestMap.get(), nFaces, nVerts);
        if (FAILED(hr))
        {
            printe("ERROR: ConcatenateMesh cube+box+face-cube [1] failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
            passed = false;
        }
        else if (nFaces != 12 || nVerts != 8)
        {
            success = false;
            passed = false;
            printe("ERROR: ConcatenateMesh cube+box+face-cube [1] failed, totals invalid (faces: %zu..12; verts: %zu..8)\n", nFaces, nVerts);
        }
        else if (!IsValidFaceDestMap(g_cubeIndices16, faceDestMap.get(), 12, 0))
        {
            success = false;
            passed = false;
            printe("ERROR: ConcatenateMesh cube+box+face-cube [1] failed, face map invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, faceDestMap[j]);
        }
        else if (!IsValidVertexDestMap(g_cubeIndices16, 12, vertDestMap.get(), 8, 0))
        {
            printe("ERROR: ConcatenateMesh cube+box+face-cube [1] failed vert map invalid\n");
            success = false;
            passed = false;
            for (size_t j = 0; j < 8; ++j)
                print("%zu -> %u\n", j, vertDestMap[j]);
        }
        else
        {
            mergedFaces.resize(nFaces);
            mergedIndices.resize(nFaces * 3);
            mergedVerts.resize(nVerts);

            for (size_t j = 0; j < 12; ++j)
            {
                size_t face = faceDestMap[j];

                mergedFaces[face] = static_cast<uint32_t>(j + baseFace);

                mergedIndices[face * 3] = vertDestMap[g_cubeIndices16[j * 3]];
                mergedIndices[face * 3 + 1] = vertDestMap[g_cubeIndices16[j * 3 + 1]];
                mergedIndices[face * 3 + 2] = vertDestMap[g_cubeIndices16[j * 3 + 2]];
            }

            for (size_t j = 0; j < 8; ++j)
            {
                mergedVerts[vertDestMap[j]] = static_cast<uint32_t>(j + baseVerts);
            }

            hr = Validate(mergedIndices.data(), mergedFaces.size(), mergedVerts.size(), nullptr, VALIDATE_UNUSED);
            if (FAILED(hr))
            {
                printe("ERROR: ConcatenateMesh cube+box+face-cube [1] failed merge\n");
                success = false;
                passed = false;
            }
        }

        // Box
        baseFace = nFaces;
        baseVerts = nVerts;
        hr = ConcatenateMesh(12, 8, faceDestMap.get(), vertDestMap.get(), nFaces, nVerts);
        if (FAILED(hr))
        {
            printe("ERROR: ConcatenateMesh cube+box+face-cube [2] failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
            passed = false;
        }
        else if (nFaces != 24 || nVerts != 16)
        {
            success = false;
            passed = false;
            printe("ERROR: ConcatenateMesh cube+box+face-cube [2] failed, totals invalid (faces: %zu..24; verts: %zu..16)\n", nFaces, nVerts);
        }
        else if (!IsValidFaceDestMap(g_boxIndices16, faceDestMap.get(), 12, 12))
        {
            success = false;
            passed = false;
            printe("ERROR: ConcatenateMesh cube+box+face-cube [2] failed, face map invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, faceDestMap[j]);
        }
        else if (!IsValidVertexDestMap(g_boxIndices16, 12, vertDestMap.get(), 8, 8))
        {
            printe("ERROR: ConcatenateMesh cube+box+face-cube [2] failed vert map invalid\n");
            success = false;
            passed = false;
            for (size_t j = 0; j < 8; ++j)
                print("%zu -> %u\n", j, vertDestMap[j]);
        }
        else if (passed)
        {
            mergedFaces.resize(nFaces);
            mergedIndices.resize(nFaces * 3);
            mergedVerts.resize(nVerts);

            for (size_t j = 0; j < 12; ++j)
            {
                size_t face = faceDestMap[j];

                mergedFaces[face] = static_cast<uint32_t>(j + baseFace);

                mergedIndices[face * 3] = vertDestMap[g_boxIndices16[j * 3]];
                mergedIndices[face * 3 + 1] = vertDestMap[g_boxIndices16[j * 3 + 1]];
                mergedIndices[face * 3 + 2] = vertDestMap[g_boxIndices16[j * 3 + 2]];
            }

            for (uint32_t j = 0; j < 8; ++j)
            {
                mergedVerts[vertDestMap[j]] = static_cast<uint32_t>(j + baseVerts);
            }

            hr = Validate(mergedIndices.data(), mergedFaces.size(), mergedVerts.size(), nullptr, VALIDATE_UNUSED);
            if (FAILED(hr))
            {
                printe("ERROR: ConcatenateMesh cube+box+face-cube [2] failed merge\n");
                success = false;
                passed = false;
            }
        }

        // Face-mapped Cube
        baseFace = nFaces;
        baseVerts = nVerts;
        hr = ConcatenateMesh(12, 24, faceDestMap.get(), vertDestMap.get(), nFaces, nVerts);
        if (FAILED(hr))
        {
            printe("ERROR: ConcatenateMesh cube+box+face-cube [3] failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
            passed = false;
        }
        else if (nFaces != 36 || nVerts != 40)
        {
            success = false;
            passed = false;
            printe("ERROR: ConcatenateMesh cube+box+face-cube [3] failed, totals invalid (faces: %zu..36; verts: %zu..40)\n", nFaces, nVerts);
        }
        else if (!IsValidFaceDestMap(g_fmCubeIndices16, faceDestMap.get(), 12, 24))
        {
            success = false;
            passed = false;
            printe("ERROR: ConcatenateMesh cube+box+face-cube [3] failed, face map invalid\n");
            for (size_t j = 0; j < 12; ++j)
                print("%zu -> %u\n", j, faceDestMap[j]);
        }
        else if (!IsValidVertexDestMap(g_fmCubeIndices16, 12, vertDestMap.get(), 24, 16))
        {
            printe("ERROR: ConcatenateMesh cube+box+face-cube [3] failed vert map invalid\n");
            success = false;
            passed = false;
            for (size_t j = 0; j < 24; ++j)
                print("%zu -> %u\n", j, vertDestMap[j]);
        }
        else if (passed)
        {
            mergedFaces.resize(nFaces);
            mergedIndices.resize(nFaces * 3);
            mergedVerts.resize(nVerts);

            for (size_t j = 0; j < 12; ++j)
            {
                size_t face = faceDestMap[j];

                mergedFaces[face] = static_cast<uint32_t>(j + baseFace);

                mergedIndices[face * 3] = vertDestMap[g_fmCubeIndices16[j * 3]];
                mergedIndices[face * 3 + 1] = vertDestMap[g_fmCubeIndices16[j * 3 + 1]];
                mergedIndices[face * 3 + 2] = vertDestMap[g_fmCubeIndices16[j * 3 + 2]];
            }

            for (uint32_t j = 0; j < 24; ++j)
            {
                mergedVerts[vertDestMap[j]] = static_cast<uint32_t>(j + baseVerts);
            }

            hr = Validate(mergedIndices.data(), mergedFaces.size(), mergedVerts.size(), nullptr, VALIDATE_UNUSED);
            if (FAILED(hr))
            {
                printe("ERROR: ConcatenateMesh cube+box+face-cube [3] failed merge\n");
                success = false;
                passed = false;
            }
            else
            {
                uint32_t curface = 0;
                for (auto it : mergedFaces)
                {
                    if (it != curface)
                    {
                        printe("ERROR: ConcatenateMesh cube+box+face-cube [3] failed merged faces\n");
                        success = false;
                        break;
                    }

                    ++curface;
                }

                uint32_t curvert = 0;
                for (auto it : mergedVerts)
                {
                    if (it != curvert)
                    {
                        printe("ERROR: ConcatenateMesh cube+box+face-cube [3] failed merged verts\n");
                        success = false;
                        break;
                    }

                    ++curvert;
                }
            }
        }
    }

    return success;
}
