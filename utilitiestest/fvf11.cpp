// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <d3d11.h>

#include "FlexibleVertexFormat.h"

namespace
{
    struct FVFVS
    {
        uint32_t                        fvf;
        size_t                          vertexSize;
        const D3D11_INPUT_ELEMENT_DESC* pIL;
        size_t                          layoutLen;
    };

    constexpr FVFVS s_fvfVertexSize[] =
    {
        { D3DFVF_XYZ, 12u, nullptr, 1 },
        { D3DFVF_XYZW, 16u, nullptr, 1 },
        { D3DFVF_XYZRHW, 16u, nullptr, 1 },
        { D3DFVF_XYZ | D3DFVF_DIFFUSE, 16u, nullptr, 2 },
        { D3DFVF_XYZ | D3DFVF_SPECULAR, 16u, nullptr, 2 },
        { D3DFVF_XYZ | D3DFVF_TEX1, 20u, nullptr, 2 },
        { D3DFVF_XYZ | D3DFVF_TEX2, 28u, nullptr, 3 },
        { D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, 24u, nullptr, 3 },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 /*D3DFVF_VERTEX*/, 32u, nullptr, 3 },
        { D3DFVF_XYZ | D3DFVF_NORMAL, 24u, nullptr, 2 },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE, 28u, nullptr, 3 },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1, 36u, nullptr, 4 },
        { D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 /*D3DFVF_TLVERTEX*/, 32u, nullptr, 4 },

        { D3DFVF_XYZB1, 16u, nullptr, 2 },
        { D3DFVF_XYZB2, 20u, nullptr, 2 },
        { D3DFVF_XYZB3, 24u, nullptr, 2 },
        { D3DFVF_XYZB4, 28u, nullptr, 2 },
        { D3DFVF_XYZB5, 32u, nullptr, 0 },
        { D3DFVF_XYZ | D3DFVF_TEX3, 36u, nullptr, 4 },
        { D3DFVF_XYZ | D3DFVF_TEX4, 44u, nullptr, 5 },
        { D3DFVF_XYZ | D3DFVF_TEX5, 52u, nullptr, 6 },
        { D3DFVF_XYZ | D3DFVF_TEX6, 60u, nullptr, 7 },
        { D3DFVF_XYZ | D3DFVF_TEX7, 68u, nullptr, 8 },
        { D3DFVF_XYZ | D3DFVF_TEX8, 76u, nullptr, 9 },
        { D3DFVF_XYZB1 | D3DFVF_NORMAL | D3DFVF_TEX1, 36u, nullptr, 4 },
        { D3DFVF_XYZB2 | D3DFVF_NORMAL | D3DFVF_TEX1, 40u, nullptr, 4 },
        { D3DFVF_XYZB3 | D3DFVF_NORMAL | D3DFVF_TEX1, 44u, nullptr, 4 },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2, 40u, nullptr, 4 },
        { D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1, 48u, nullptr, 5 },
        { D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE, 20u, nullptr, 3 },
        { D3DFVF_XYZRHW | D3DFVF_TEX1, 24u, nullptr, 2 },
        { D3DFVF_XYZRHW | D3DFVF_TEX2, 32u, nullptr, 3 },
        { D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, 28u, nullptr, 3 },
        { D3DFVF_XYZRHW | D3DFVF_DIFFUSE, 20u, nullptr, 2 },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2, 48u, nullptr, 6 },
        { D3DFVF_XYZB3 | D3DFVF_NORMAL | D3DFVF_DIFFUSE, 40u, nullptr, 4 },
        { D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE1(1), 28u, nullptr, 3 },
        { D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0), 24u, nullptr, 2 },
        { D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0), 28u, nullptr, 2 },
        { D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1, 28u, nullptr, 4 },
    };
}

//-------------------------------------------------------------------------------------
// CreateInputLayoutFromFVF(D3D11)
bool Test07()
{
    bool success = true;

    for (size_t j = 0; j < std::size(s_fvfVertexSize); ++j)
    {
        std::vector<D3D11_INPUT_ELEMENT_DESC> il;

        if (!FVF::CreateInputLayoutFromFVF(s_fvfVertexSize[j].fvf, il))
        {
            if (s_fvfVertexSize[j].fvf == D3DFVF_XYZB5)
                continue;

            success = false;
            printf("\nERROR: %zu: %08X failed\n", j, s_fvfVertexSize[j].fvf);
        }

        if (il.size() != s_fvfVertexSize[j].layoutLen + 1)
        {
            success = false;
            printf("\nERROR (2): %zu: %zu .. %zu\n", j, il.size(), s_fvfVertexSize[j].layoutLen + 1);
        }
    }

    return success;
}
