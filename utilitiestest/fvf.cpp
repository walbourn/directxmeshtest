// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "FlexibleVertexFormat.h"

#include <iterator>

//-------------------------------------------------------------------------------------
// ComputeFVFVertexSize
namespace
{
    struct FVFVS
    {
        uint32_t fvf;
        size_t   vertexSize;
    };

    constexpr FVFVS s_fvfVertexSize[] =
    {
        { D3DFVF_XYZ, 12u },
        { D3DFVF_XYZRHW, 16u },
        { D3DFVF_XYZB1, 16u },
        { D3DFVF_XYZB2, 20u },
        { D3DFVF_XYZB3, 24u },
        { D3DFVF_XYZB4, 28u },
        { D3DFVF_XYZB5, 32u },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, 32u },
        { D3DFVF_XYZB1 | D3DFVF_NORMAL | D3DFVF_TEX1, 36u },
        { D3DFVF_XYZB2 | D3DFVF_NORMAL | D3DFVF_TEX1, 40u },
        { D3DFVF_XYZB3 | D3DFVF_NORMAL | D3DFVF_TEX1, 44u },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2, 40u },
        { D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1, 48u },
        { D3DFVF_XYZ | D3DFVF_TEX1, 20u },
        { D3DFVF_XYZ | D3DFVF_TEX2, 28u },
        { D3DFVF_XYZ | D3DFVF_TEX3, 36u },
        { D3DFVF_XYZ | D3DFVF_TEX4, 44u },
        { D3DFVF_XYZ | D3DFVF_TEX5, 52u },
        { D3DFVF_XYZ | D3DFVF_TEX6, 60u },
        { D3DFVF_XYZ | D3DFVF_TEX7, 68u },
        { D3DFVF_XYZ | D3DFVF_TEX8, 76u },
        { D3DFVF_XYZ | D3DFVF_NORMAL, 24u },
        { D3DFVF_XYZ | D3DFVF_DIFFUSE, 16u },
        { D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE, 20u },
        { D3DFVF_XYZ | D3DFVF_SPECULAR, 16u },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE, 28u },
        { D3DFVF_XYZRHW | D3DFVF_TEX1, 24u },
        { D3DFVF_XYZRHW | D3DFVF_TEX2, 32u },
        { D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, 24u },
        { D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, 28u },
        { D3DFVF_XYZRHW | D3DFVF_DIFFUSE, 20u },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2, 48u },
        { D3DFVF_XYZB3 | D3DFVF_NORMAL | D3DFVF_DIFFUSE, 40u },
        { D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE1(1), 28u },
        { D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0), 24u },
        { D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0), 28u },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 /*D3DFVF_VERTEX*/, 32u },
        { D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 /*D3DFVF_TLVERTEX*/, 32u },
        { D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1, 28u },
    };
}

bool Test02()
{
    bool success = true;

    for (size_t j = 0; j < std::size(s_fvfVertexSize); ++j)
    {
        size_t vsize = FVF::ComputeFVFVertexSize(s_fvfVertexSize[j].fvf);

        if (s_fvfVertexSize[j].vertexSize != vsize)
        {
            success = false;
            printf("\nERROR: %zu: %zu .. %zu\n", j, vsize, s_fvfVertexSize[j].vertexSize);
        }
    }

    return success;
}
