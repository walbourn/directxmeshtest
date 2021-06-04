// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <d3d12.h>

#include "FlexibleVertexFormat.h"

#include <cstring>

namespace
{
    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPosition[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPosition4[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionColor[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",       0, DXGI_FORMAT_B8G8R8A8_UNORM,  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionTL[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",       0, DXGI_FORMAT_B8G8R8A8_UNORM,     0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",       1, DXGI_FORMAT_B8G8R8A8_UNORM,     0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionBeta1[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BLENDWEIGHT", 0, DXGI_FORMAT_R32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionBeta2[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionBeta3[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionBeta4[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionBeta5[] =
    {
        { "SV_Position",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BLENDWEIGHT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionBeta5Color[] =
    {
        { "SV_Position",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BLENDWEIGHT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BLENDINDICES", 0, DXGI_FORMAT_B8G8R8A8_UNORM,     0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionSpecularColor[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",       1, DXGI_FORMAT_B8G8R8A8_UNORM,  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionTexture[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionDualTexture[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    1, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionDual31Texture[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    1, DXGI_FORMAT_R32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionColorTexture[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",       0, DXGI_FORMAT_B8G8R8A8_UNORM,  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionNormal[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionNormalColor[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",       0, DXGI_FORMAT_B8G8R8A8_UNORM,  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionNormalTexture[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    constexpr D3D12_INPUT_ELEMENT_DESC c_VertexPositionNormalColorTexture[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",       0, DXGI_FORMAT_B8G8R8A8_UNORM,  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    struct FVFVS
    {
        uint32_t                  fvf;
        size_t                    vertexSize;
        D3D12_INPUT_LAYOUT_DESC   ilayout;
    };

    constexpr FVFVS s_fvfVertexSize[] =
    {
        { D3DFVF_XYZ, 12u, { c_VertexPosition, 1 } },
        { D3DFVF_XYZW, 16u, { c_VertexPosition4, 1 } },
        { D3DFVF_XYZRHW, 16u, { c_VertexPosition4, 1 } },
        { D3DFVF_XYZ | D3DFVF_DIFFUSE, 16u, { c_VertexPositionColor, 2 } },
        { D3DFVF_XYZ | D3DFVF_SPECULAR, 16u, { c_VertexPositionSpecularColor, 2 } },
        { D3DFVF_XYZ | D3DFVF_TEX1, 20u, { c_VertexPositionTexture, 2 } },
        { D3DFVF_XYZ | D3DFVF_TEX2, 28u, { c_VertexPositionDualTexture, 3 } },
        { D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, 24u, { c_VertexPositionColorTexture, 3 } },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 /*D3DFVF_VERTEX*/, 32u, { c_VertexPositionNormalTexture, 3 } },
        { D3DFVF_XYZ | D3DFVF_NORMAL, 24u, { c_VertexPositionNormal, 2 } },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE, 28u, { c_VertexPositionNormalColor, 3 } },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1, 36u, { c_VertexPositionNormalColorTexture, 4 } },
        { D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 /*D3DFVF_TLVERTEX*/, 32u, { c_VertexPositionTL, 4 } },

        { D3DFVF_XYZB1, 16u, { c_VertexPositionBeta1, 2 } },
        { D3DFVF_XYZB2, 20u, { c_VertexPositionBeta2, 2 } },
        { D3DFVF_XYZB3, 24u, { c_VertexPositionBeta3, 2 } },
        { D3DFVF_XYZB4, 28u, { c_VertexPositionBeta4, 2 } },
        { D3DFVF_XYZB5 | D3DFVF_LASTBETA_UBYTE4, 32u, { c_VertexPositionBeta5, 3 } },
        { D3DFVF_XYZB5 | D3DFVF_LASTBETA_D3DCOLOR, 32u, { c_VertexPositionBeta5Color, 3 } },

        { D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE1(1), 28u, { c_VertexPositionDual31Texture, 3 } },

        { D3DFVF_XYZ | D3DFVF_TEX3, 36u, { nullptr, 4 } },
        { D3DFVF_XYZ | D3DFVF_TEX4, 44u, { nullptr, 5 } },
        { D3DFVF_XYZ | D3DFVF_TEX5, 52u, { nullptr, 6 } },
        { D3DFVF_XYZ | D3DFVF_TEX6, 60u, { nullptr, 7 } },
        { D3DFVF_XYZ | D3DFVF_TEX7, 68u, { nullptr, 8 } },
        { D3DFVF_XYZ | D3DFVF_TEX8, 76u, { nullptr, 9 } },
        { D3DFVF_XYZB1 | D3DFVF_NORMAL | D3DFVF_TEX1, 36u, { nullptr, 4 } },
        { D3DFVF_XYZB2 | D3DFVF_NORMAL | D3DFVF_TEX1, 40u, { nullptr, 4 } },
        { D3DFVF_XYZB3 | D3DFVF_NORMAL | D3DFVF_TEX1, 44u, { nullptr, 4 } },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2, 40u, { nullptr, 4 } },
        { D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1, 48u, { nullptr, 5 } },
        { D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE, 20u, { nullptr, 3 } },
        { D3DFVF_XYZRHW | D3DFVF_TEX1, 24u, { nullptr, 2 } },
        { D3DFVF_XYZRHW | D3DFVF_TEX2, 32u, { nullptr, 3 } },
        { D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, 28u, { nullptr, 3 } },
        { D3DFVF_XYZRHW | D3DFVF_DIFFUSE, 20u, { nullptr, 2 } },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2, 48u, { nullptr, 6 } },
        { D3DFVF_XYZB3 | D3DFVF_NORMAL | D3DFVF_DIFFUSE, 40u, { nullptr, 4 } },
        { D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0), 24u, { nullptr, 2 } },
        { D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0), 28u, { nullptr, 2 } },
        { D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1, 28u, { nullptr, 4 } }
    };

    bool IsMatch(_In_reads_(alength) const D3D12_INPUT_ELEMENT_DESC* a, size_t alength,
        _In_reads_(blength) const D3D12_INPUT_ELEMENT_DESC* b, size_t blength)
    {
        if (alength != blength)
            return false;

        for (size_t j = 0; j < alength; ++j)
        {
            if (a[j].AlignedByteOffset != b[j].AlignedByteOffset
                || a[j].Format != b[j].Format
                || a[j].InputSlot != b[j].InputSlot
                || a[j].InputSlotClass != b[j].InputSlotClass
                || a[j].InstanceDataStepRate != b[j].InstanceDataStepRate
                || a[j].SemanticIndex != b[j].SemanticIndex)
            {
                return false;
            }

            if (_stricmp(a[j].SemanticName, b[j].SemanticName) != 0)
                return false;
        }

        return true;
    }
}

//-------------------------------------------------------------------------------------
// CreateInputLayoutFromFVF(D3D12)
bool Test08()
{
    bool success = true;

    for (size_t j = 0; j < std::size(s_fvfVertexSize); ++j)
    {
        std::vector<D3D12_INPUT_ELEMENT_DESC> il;

        if (!FVF::CreateInputLayoutFromFVF(s_fvfVertexSize[j].fvf, il))
        {
            if (s_fvfVertexSize[j].fvf == D3DFVF_XYZB5)
                continue;

            success = false;
            printf("\nERROR: %zu: %08X failed\n", j, s_fvfVertexSize[j].fvf);
        }

        if (il.size() != s_fvfVertexSize[j].ilayout.NumElements)
        {
            success = false;
            printf("\nERROR (2): %zu: %zu .. %u\n", j, il.size(), s_fvfVertexSize[j].ilayout.NumElements);
        }
        else if (s_fvfVertexSize[j].ilayout.pInputElementDescs
            && !IsMatch(s_fvfVertexSize[j].ilayout.pInputElementDescs, s_fvfVertexSize[j].ilayout.NumElements, il.data(), il.size()))
        {
            success = false;
            printf("\nERROR (3): %zu: input layout does not match\n", j);
        }
    }

    // invalid args
    std::vector<D3D12_INPUT_ELEMENT_DESC> il;
    if (FVF::CreateInputLayoutFromFVF(D3DFVF_RESERVED0, il) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test A failed\n");
    }

    if (FVF::CreateInputLayoutFromFVF(D3DFVF_RESERVED2, il) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test B failed\n");
    }

    if (FVF::CreateInputLayoutFromFVF(0xFF, il) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test C failed\n");
    }

    if (FVF::CreateInputLayoutFromFVF(D3DFVF_XYZB5, il) != 0)
    {
        // 5 betas not supported without indices
        success = false;
        printf("\nERROR: invalid args test D failed\n");
    }

    if (FVF::CreateInputLayoutFromFVF(D3DFVF_XYZ | 0xF00 /* 15 texture coords*/, il) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test E failed\n");
    }

    return success;
}
