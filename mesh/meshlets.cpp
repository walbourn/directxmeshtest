//-------------------------------------------------------------------------------------
// meshlets.cpp
//  
// Copyright (c) Microsoft Corporation. All rights reserved.
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

    const float g_Epsilon = 0.0001f;
}

//-------------------------------------------------------------------------------------
// ComputeMeshlets
bool Test28()
{
    bool success = true;

    // 16-bit single submesh
    {
        std::vector<Meshlet> meshlets;
        std::vector<uint8_t> uniqueVertexIB;
        std::vector<MeshletTriangle> primitiveIndices;
        HRESULT hr = ComputeMeshlets(g_fmCubeIndices16, 12, g_fmCubeVerts, 24, nullptr,
            meshlets, uniqueVertexIB, primitiveIndices);
        if (FAILED(hr))
        {
            printe("\nERROR: ComputeMeshlets(16) fmcube failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (meshlets.size() != 1
            || uniqueVertexIB.size() != (24 * sizeof(uint16_t))
            || primitiveIndices.size() != 12)
        {
            printe("\nERROR: ComputeMeshlets(16) fmcube failed producing correctg numbers of meshlets\n");
            success = false;
        }
        else if (!IsValidMeshlet(*meshlets.cbegin(), 24, primitiveIndices.size()))
        {
            printe("\nERROR: ComputeMeshlets(16) fmcube produced an invalid meshlet\n");
            success = false;
        }
        else
        {
            for (auto it : primitiveIndices)
            {
                if (!IsValidMeshletTriangle(it, 24))
                {
                    printe("\nERROR: ComputeMeshlets(16) fmcube produced invalid meshlet triangles\n");
                    success = false;
                }
            }

            auto uniqueVertexIndices = reinterpret_cast<const uint16_t*>(uniqueVertexIB.data());
            size_t vertIndices = uniqueVertexIB.size() / sizeof(uint16_t);

            for(size_t j = 0; j < vertIndices; ++j)
            {
                if (uniqueVertexIndices[j] >= 24)
                {
                    printe("\nERROR: ComputeMeshlets(16) fmcube produced invalid unique verts\n");
                    success = false;
                }
            }
        }
    }

    // 16-bit single submesh w/ adj provided
    {
        std::vector<Meshlet> meshlets;
        std::vector<uint8_t> uniqueVertexIB;
        std::vector<MeshletTriangle> primitiveIndices;
        HRESULT hr = ComputeMeshlets(g_fmCubeIndices16, 12, g_fmCubeVerts, 24, s_fmCubeAdj,
            meshlets, uniqueVertexIB, primitiveIndices);
        if (FAILED(hr))
        {
            printe("\nERROR: ComputeMeshlets(16) fmcube adj failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (meshlets.size() != 1
            || uniqueVertexIB.size() != (24 * sizeof(uint16_t))
            || primitiveIndices.size() != 12)
        {
            printe("\nERROR: ComputeMeshlets(16) fmcube adj failed producing correctg numbers of meshlets\n");
            success = false;
        }
        else if (!IsValidMeshlet(*meshlets.cbegin(), 24, primitiveIndices.size()))
        {
            printe("\nERROR: ComputeMeshlets(16) fmcube adj produced an invalid meshlet\n");
            success = false;
        }
        else
        {
            for (auto it : primitiveIndices)
            {
                if (!IsValidMeshletTriangle(it, 24))
                {
                    printe("\nERROR: ComputeMeshlets(16) fmcube adj produced invalid meshlet triangles\n");
                    success = false;
                }
            }

            auto uniqueVertexIndices = reinterpret_cast<const uint16_t*>(uniqueVertexIB.data());
            size_t vertIndices = uniqueVertexIB.size() / sizeof(uint16_t);

            for (size_t j = 0; j < vertIndices; ++j)
            {
                if (uniqueVertexIndices[j] >= 24)
                {
                    printe("\nERROR: ComputeMeshlets(16) fmcube adj produced invalid unique verts\n");
                    success = false;
                }
            }
        }
    }

    // 32-bit single submesh
    {
        std::vector<Meshlet> meshlets;
        std::vector<uint8_t> uniqueVertexIB;
        std::vector<MeshletTriangle> primitiveIndices;
        HRESULT hr = ComputeMeshlets(g_fmCubeIndices32, 12, g_fmCubeVerts, 24, nullptr,
            meshlets, uniqueVertexIB, primitiveIndices);
        if (FAILED(hr))
        {
            printe("\nERROR: ComputeMeshlets(32) fmcube failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (meshlets.size() != 1
            || uniqueVertexIB.size() != (24 * sizeof(uint32_t))
            || primitiveIndices.size() != 12)
        {
            printe("\nERROR: ComputeMeshlets(32) fmcube failed producing correctg numbers of meshlets\n");
            success = false;
        }
        else if (!IsValidMeshlet(*meshlets.cbegin(), 24, primitiveIndices.size()))
        {
            printe("\nERROR: ComputeMeshlets(32) fmcube produced an invalid meshlet\n");
            success = false;
        }
        else
        {
            for (auto it : primitiveIndices)
            {
                if (!IsValidMeshletTriangle(it, 24))
                {
                    printe("\nERROR: ComputeMeshlets(32) fmcube produced invalid meshlet triangles\n");
                    success = false;
                }
            }

            auto uniqueVertexIndices = reinterpret_cast<const uint32_t*>(uniqueVertexIB.data());
            size_t vertIndices = uniqueVertexIB.size() / sizeof(uint32_t);

            for (size_t j = 0; j < vertIndices; ++j)
            {
                if (uniqueVertexIndices[j] >= 24)
                {
                    printe("\nERROR: ComputeMeshlets(32) fmcube produced invalid unique verts\n");
                    success = false;
                }
            }
        }
    }

    // 32-bit single submesh w/ adj provided
    {
        std::vector<Meshlet> meshlets;
        std::vector<uint8_t> uniqueVertexIB;
        std::vector<MeshletTriangle> primitiveIndices;
        HRESULT hr = ComputeMeshlets(g_fmCubeIndices32, 12, g_fmCubeVerts, 24, s_fmCubeAdj,
            meshlets, uniqueVertexIB, primitiveIndices);
        if (FAILED(hr))
        {
            printe("\nERROR: ComputeMeshlets(32) fmcube adj failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (meshlets.size() != 1
            || uniqueVertexIB.size() != (24 * sizeof(uint32_t))
            || primitiveIndices.size() != 12)
        {
            printe("\nERROR: ComputeMeshlets(32) fmcube adj failed producing correctg numbers of meshlets\n");
            success = false;
        }
        else if (!IsValidMeshlet(*meshlets.cbegin(), 24, primitiveIndices.size()))
        {
            printe("\nERROR: ComputeMeshlets(32) fmcube adj produced an invalid meshlet\n");
            success = false;
        }
        else
        {
            for (auto it : primitiveIndices)
            {
                if (!IsValidMeshletTriangle(it, 24))
                {
                    printe("\nERROR: ComputeMeshlets(32) fmcube adj produced invalid meshlet triangles\n");
                    success = false;
                }
            }

            auto uniqueVertexIndices = reinterpret_cast<const uint32_t*>(uniqueVertexIB.data());
            size_t vertIndices = uniqueVertexIB.size() / sizeof(uint32_t);

            for (size_t j = 0; j < vertIndices; ++j)
            {
                if (uniqueVertexIndices[j] >= 24)
                {
                    printe("\nERROR: ComputeMeshlets(32) fmcube adj produced invalid unique verts\n");
                    success = false;
                }
            }
        }
    }

    // 16-bit multiple submesh
    // TODO -

    // 32-bit multiple submesh
    // TODO -

    return success;
}


//-------------------------------------------------------------------------------------
// ComputeCullData
bool Test29()
{
    bool success = true;

    // 16-bit
    {
        std::vector<Meshlet> meshlets;
        std::vector<uint8_t> uniqueVertexIB;
        std::vector<MeshletTriangle> primitiveIndices;
        HRESULT hr = ComputeMeshlets(g_fmCubeIndices16, 12, g_fmCubeVerts, 24, s_fmCubeAdj,
            meshlets, uniqueVertexIB, primitiveIndices);
        if (FAILED(hr))
        {
            printe("\nERROR: ComputeMeshlets(16) fmcube failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else
        {
            auto uniqueVertexIndices = reinterpret_cast<const uint16_t*>(uniqueVertexIB.data());
            size_t vertIndices = uniqueVertexIB.size() / sizeof(uint16_t);

            CullData cull = {};
            hr = ComputeCullData(g_fmCubeVerts, 24,
                meshlets.data(), meshlets.size(),
                uniqueVertexIndices, vertIndices,
                primitiveIndices.data(), primitiveIndices.size(), &cull);
            if (FAILED(hr))
            {
                printe("\nERROR: ComputeCullData(16) fmcube failed (%08X)\n", static_cast<unsigned int>(hr));
                success = false;
            }
            else if (fabsf(cull.BoundingSphere.Radius - 2.15166283f) > g_Epsilon)
            {
                printe("ERROR: ComputeCullData(16) fmcube bounding radius: %f .. %f\n", cull.BoundingSphere.Radius, 2.15166283f);
                success = false;
            }

        }
    }

    // 32-bit
    {
        std::vector<Meshlet> meshlets;
        std::vector<uint8_t> uniqueVertexIB;
        std::vector<MeshletTriangle> primitiveIndices;
        HRESULT hr = ComputeMeshlets(g_fmCubeIndices32, 12, g_fmCubeVerts, 24, s_fmCubeAdj,
            meshlets, uniqueVertexIB, primitiveIndices);
        if (FAILED(hr))
        {
            printe("\nERROR: ComputeMeshlets(32) fmcube failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else
        {
            auto uniqueVertexIndices = reinterpret_cast<const uint32_t*>(uniqueVertexIB.data());
            size_t vertIndices = uniqueVertexIB.size() / sizeof(uint32_t);

            CullData cull = {};
            hr = ComputeCullData(g_fmCubeVerts, 24,
                meshlets.data(), meshlets.size(),
                uniqueVertexIndices, vertIndices,
                primitiveIndices.data(), primitiveIndices.size(), &cull);
            if (FAILED(hr))
            {
                printe("\nERROR: ComputeCullData(32) fmcube failed (%08X)\n", static_cast<unsigned int>(hr));
                success = false;
            }
            else if (fabsf(cull.BoundingSphere.Radius - 2.15166283f) > g_Epsilon)
            {
                printe("ERROR: ComputeCullData(32) fmcube bounding radius: %f .. %f\n", cull.BoundingSphere.Radius, 2.15166283f);
                success = false;
            }

        }
    }

    return success;
}
