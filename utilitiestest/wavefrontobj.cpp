// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WaveFrontReader.h"

#define MEDIA_PATH L"D:\\Microsoft\\directxmeshmedia\\"

bool Test01()
{
    bool success = true;

    printf(".");

    // 16-bit cup
    {
        WaveFrontReader<uint16_t> wfReader;

        HRESULT hr = wfReader.Load(MEDIA_PATH L"cup._obj");
        if (FAILED(hr))
        {
            printf("ERROR: WaveFront OBJ 16-bit cup load failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        if (wfReader.vertices.size() != 1340
            || wfReader.indices.size() != 5640
            || wfReader.attributes.size() != 1880
            || wfReader.materials.size() != 3
            || !wfReader.hasNormals
            || !wfReader.hasTexcoords)
        {
            printf("ERROR: WaveFront OBJ 16-bit cup test failed\n\tverts: %zu   inds: %zu   attr: %zu   mats: %zu\n",
                wfReader.vertices.size(),
                wfReader.indices.size(),
                wfReader.attributes.size(),
                wfReader.materials.size());
            success = false;
        }

    }

    printf(".");

    // 32-bit cup
    {
        WaveFrontReader<uint32_t> wfReader;

        HRESULT hr = wfReader.Load(MEDIA_PATH L"cup._obj");
        if (FAILED(hr))
        {
            printf("ERROR: WaveFront OBJ 32-bit cup load failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        if (wfReader.vertices.size() != 1340
            || wfReader.indices.size() != 5640
            || wfReader.attributes.size() != 1880
            || wfReader.materials.size() != 3
            || !wfReader.hasNormals
            || !wfReader.hasTexcoords)
        {
            printf("ERROR: WaveFront OBJ 32-bit cup test failed\n\tverts: %zu   inds: %zu   attr: %zu   mats: %zu\n",
                wfReader.vertices.size(),
                wfReader.indices.size(),
                wfReader.attributes.size(),
                wfReader.materials.size());
            success = false;
        }

    }

    printf(".");

    // large test case
    {
        WaveFrontReader<uint32_t> wfReader;

        HRESULT hr = wfReader.Load(MEDIA_PATH L"John40K._obj");
        if (FAILED(hr))
        {
            printf("ERROR: WaveFront OBJ large test case load failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        if (wfReader.vertices.size() != 40289
            || wfReader.indices.size() != 120000
            || wfReader.attributes.size() != 40000
            || wfReader.materials.size() != 2
            || !wfReader.hasNormals
            || !wfReader.hasTexcoords)
        {
            printf("ERROR: WaveFront OBJ large test failed\n\tverts: %zu   inds: %zu   attr: %zu   mats: %zu\n",
                wfReader.vertices.size(),
                wfReader.indices.size(),
                wfReader.attributes.size(),
                wfReader.materials.size());
            success = false;
        }

    }

    printf(" ");

    return success;
}
