// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "FlexibleVertexFormat.h"

#include <memory>

namespace
{
    constexpr D3DVERTEXELEMENT9 c_VertexPosition[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        D3DDECL_END()
    };

    constexpr D3DVERTEXELEMENT9 c_VertexPosition4[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        D3DDECL_END()
    };

    constexpr D3DVERTEXELEMENT9 c_VertexPositionSCREEN[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
        D3DDECL_END()
    };

    constexpr D3DVERTEXELEMENT9 c_VertexPositionTL[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
        { 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        { 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1 },
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    constexpr D3DVERTEXELEMENT9 c_VertexPositionColor[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    };

    constexpr D3DVERTEXELEMENT9 c_VertexPositionSpecularColor[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1 },
        D3DDECL_END()
    };

    constexpr D3DVERTEXELEMENT9 c_VertexPositionTexture[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    constexpr D3DVERTEXELEMENT9 c_VertexPositionDualTexture[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
        D3DDECL_END()
    };

    constexpr D3DVERTEXELEMENT9 c_VertexPositionColorTexture[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    constexpr D3DVERTEXELEMENT9 c_VertexPositionNormal[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        D3DDECL_END()
    };

    constexpr D3DVERTEXELEMENT9 c_VertexPositionNormalColor[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    };

    constexpr D3DVERTEXELEMENT9 c_VertexPositionNormalTexture[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    constexpr D3DVERTEXELEMENT9 c_VertexPositionNormalColorTexture[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        { 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    struct FVFVS
    {
        uint32_t                    fvf;
        size_t                      vertexSize;
        const D3DVERTEXELEMENT9*    pDecl;
        size_t                      declLength;
    };

    constexpr FVFVS s_fvfVertexSize[] =
    {
        { D3DFVF_XYZ, 12u, c_VertexPosition, 1 },
        { D3DFVF_XYZW, 16u, c_VertexPosition4, 1 },
        { D3DFVF_XYZRHW, 16u, c_VertexPositionSCREEN, 1 },
        { D3DFVF_XYZ | D3DFVF_DIFFUSE, 16u, c_VertexPositionColor, 2 },
        { D3DFVF_XYZ | D3DFVF_SPECULAR, 16u, c_VertexPositionSpecularColor, 2 },
        { D3DFVF_XYZ | D3DFVF_TEX1, 20u, c_VertexPositionTexture, 2 },
        { D3DFVF_XYZ | D3DFVF_TEX2, 28u, c_VertexPositionDualTexture, 3 },
        { D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, 24u, c_VertexPositionColorTexture, 3 },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 /*D3DFVF_VERTEX*/, 32u, c_VertexPositionNormalTexture, 3 },
        { D3DFVF_XYZ | D3DFVF_NORMAL, 24u, c_VertexPositionNormal, 2 },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE, 28u, c_VertexPositionNormalColor, 3 },
        { D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1, 36u, c_VertexPositionNormalColorTexture, 4 },        
        { D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 /*D3DFVF_TLVERTEX*/, 32u, c_VertexPositionTL, 4 },

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
// ComputeVertexSize(FVF)
bool Test02()
{
    bool success = true;

    for (size_t j = 0; j < std::size(s_fvfVertexSize); ++j)
    {
        size_t vsize = FVF::ComputeVertexSize(s_fvfVertexSize[j].fvf);

        if (s_fvfVertexSize[j].vertexSize != vsize)
        {
            success = false;
            printf("\nERROR: %zu: %zu .. %zu\n", j, vsize, s_fvfVertexSize[j].vertexSize);
        }
    }

    // invalid args
    if (FVF::ComputeVertexSize(D3DFVF_RESERVED0) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test A failed\n");
    }

    if (FVF::ComputeVertexSize(D3DFVF_RESERVED2) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test B failed\n");
    }

    if (FVF::ComputeVertexSize(0xFF) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test C failed\n");
    }

    if (FVF::ComputeVertexSize(D3DFVF_XYZ | 0xF00 /* 15 texture coords*/) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test D failed\n");
    }

    return success;
}

//-------------------------------------------------------------------------------------
// ComputeVertexSize(pDecl)
bool Test03()
{
    bool success = true;

    for (size_t j = 0; j < std::size(s_fvfVertexSize); ++j)
    {
        if (!s_fvfVertexSize[j].pDecl)
            continue;

        size_t vsize = FVF::ComputeVertexSize(s_fvfVertexSize[j].pDecl, 0);

        if (s_fvfVertexSize[j].vertexSize != vsize)
        {
            success = false;
            printf("\nERROR: %zu: %zu .. %zu\n", j, vsize, s_fvfVertexSize[j].vertexSize);
        }

        vsize = FVF::ComputeVertexSize(s_fvfVertexSize[j].pDecl, s_fvfVertexSize[j].declLength + 1, 0);

        if (s_fvfVertexSize[j].vertexSize != vsize)
        {
            success = false;
            printf("\nERROR (2): %zu: %zu .. %zu\n", j, vsize, s_fvfVertexSize[j].vertexSize);
        }
    }

    // invalid args
    if (FVF::ComputeVertexSize(nullptr, 0) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test A failed\n");
    }

    if (FVF::ComputeVertexSize(nullptr, 0, 0) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test B failed\n");
    }

    if (FVF::ComputeVertexSize(c_VertexPosition, 18) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test C failed\n");
    }

    if (FVF::ComputeVertexSize(c_VertexPosition, 1, 18) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test D failed\n");
    }

    if (FVF::ComputeVertexSize(c_VertexPosition, MAXD3DDECLLENGTH + 2, 0) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test E failed\n");
    }

    // large decl tests
    auto bigDecl = std::make_unique<D3DVERTEXELEMENT9[]>(MAXD3DDECLLENGTH + 1);
    auto tooBigDecl = std::make_unique<D3DVERTEXELEMENT9[]>(MAXD3DDECLLENGTH + 2);
    size_t offset = 0;
    for (size_t j = 0; j < MAXD3DDECLLENGTH; ++j)
    {
        bigDecl[j] = c_VertexPosition[0];
        bigDecl[j].Offset = static_cast<WORD>(offset);

        tooBigDecl[j] = c_VertexPosition[0];
        tooBigDecl[j].Offset = static_cast<WORD>(offset);

        offset += 12;
    }
    bigDecl[MAXD3DDECLLENGTH] = D3DDECL_END();

    tooBigDecl[MAXD3DDECLLENGTH] = c_VertexPosition[0];
    tooBigDecl[MAXD3DDECLLENGTH].Offset = static_cast<WORD>(offset);
    tooBigDecl[MAXD3DDECLLENGTH + 1] = D3DDECL_END();

    size_t vsize = FVF::ComputeVertexSize(bigDecl.get(), 0);
    if (vsize != 768)
    {
        success = false;
        printf("\nERROR: big decl failed: %zu .. 768\n", vsize);
    }

    if (FVF::ComputeVertexSize(tooBigDecl.get(), 0) != 0)
    {
        success = false;
        printf("\nERROR: too big decl test failed\n");
    }

    return success;
}

//-------------------------------------------------------------------------------------
// GetDeclLength
bool Test04()
{
    bool success = true;

    for (size_t j = 0; j < std::size(s_fvfVertexSize); ++j)
    {
        if (!s_fvfVertexSize[j].pDecl)
            continue;

        size_t len = FVF::GetDeclLength(s_fvfVertexSize[j].pDecl);

        if (s_fvfVertexSize[j].declLength != len)
        {
            success = false;
            printf("\nERROR: %zu: %zu .. %zu\n", j, len, s_fvfVertexSize[j].declLength);
        }
    }

    // invalid args
    if (FVF::GetDeclLength(nullptr) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test A failed\n");
    }

    // large decl tests
    auto bigDecl = std::make_unique<D3DVERTEXELEMENT9[]>(MAXD3DDECLLENGTH + 1);
    auto tooBigDecl = std::make_unique<D3DVERTEXELEMENT9[]>(MAXD3DDECLLENGTH + 2);
    size_t offset = 0;
    for (size_t j = 0; j < MAXD3DDECLLENGTH; ++j)
    {
        bigDecl[j] = c_VertexPosition[0];
        bigDecl[j].Offset = static_cast<WORD>(offset);

        tooBigDecl[j] = c_VertexPosition[0];
        tooBigDecl[j].Offset = static_cast<WORD>(offset);

        offset += 12;
    }
    bigDecl[MAXD3DDECLLENGTH] = D3DDECL_END();

    tooBigDecl[MAXD3DDECLLENGTH] = c_VertexPosition[0];
    tooBigDecl[MAXD3DDECLLENGTH].Offset = static_cast<WORD>(offset);
    tooBigDecl[MAXD3DDECLLENGTH + 1] = D3DDECL_END();

    size_t len = FVF::GetDeclLength(bigDecl.get());
    if (len != MAXD3DDECLLENGTH)
    {
        success = false;
        printf("\nERROR: big decl failed: %zu .. %u\n", len, MAXD3DDECLLENGTH);
    }

    if (FVF::GetDeclLength(tooBigDecl.get()) != 0)
    {
        success = false;
        printf("\nERROR: too big decl test failed\n");
    }

    return success;
}

//-------------------------------------------------------------------------------------
// ComputeFVF
bool Test05()
{
    bool success = true;

    for (size_t j = 0; j < std::size(s_fvfVertexSize); ++j)
    {
        if (!s_fvfVertexSize[j].pDecl)
            continue;

        uint32_t fvfCode = FVF::ComputeFVF(s_fvfVertexSize[j].pDecl);

        if (fvfCode != s_fvfVertexSize[j].fvf)
        {
            success = false;
            printf("\nERROR: %zu: %08X .. %08X\n", j, fvfCode, s_fvfVertexSize[j].fvf);
        }

        fvfCode = FVF::ComputeFVF(s_fvfVertexSize[j].pDecl, s_fvfVertexSize[j].declLength + 1);

        if (fvfCode != s_fvfVertexSize[j].fvf)
        {
            success = false;
            printf("\nERROR (2): %zu: %08X .. %08X\n", j, fvfCode, s_fvfVertexSize[j].fvf);
        }
    }

    // invalid args
    if (FVF::ComputeFVF(nullptr) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test A failed\n");
    }

    if (FVF::ComputeFVF(nullptr, 0) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test B failed\n");
    }

    if (FVF::ComputeFVF(c_VertexPosition, MAXD3DDECLLENGTH + 2) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test C failed\n");
    }

    // large decl tests
    auto tooBigDecl = std::make_unique<D3DVERTEXELEMENT9[]>(MAXD3DDECLLENGTH + 2);
    size_t offset = 0;
    for (size_t j = 0; j < MAXD3DDECLLENGTH; ++j)
    {
        tooBigDecl[j] = c_VertexPosition[0];
        tooBigDecl[j].Offset = static_cast<WORD>(offset);

        offset += 12;
    }

    tooBigDecl[MAXD3DDECLLENGTH] = c_VertexPosition[0];
    tooBigDecl[MAXD3DDECLLENGTH].Offset = static_cast<WORD>(offset);
    tooBigDecl[MAXD3DDECLLENGTH + 1] = D3DDECL_END();

    if (FVF::ComputeFVF(tooBigDecl.get()) != 0)
    {
        success = false;
        printf("\nERROR: too big decl test failed\n");
    }

    return success;
}

//-------------------------------------------------------------------------------------
// CreateDeclFromFVF
bool Test06()
{
    bool success = true;

    for (size_t j = 0; j < std::size(s_fvfVertexSize); ++j)
    {
        std::vector<D3DVERTEXELEMENT9> decl;

        if (!FVF::CreateDeclFromFVF(s_fvfVertexSize[j].fvf, decl))
        {
            if (s_fvfVertexSize[j].fvf == D3DFVF_XYZB5)
                continue;

            success = false;
            printf("\nERROR: %zu: %08X failed\n", j, s_fvfVertexSize[j].fvf);
        }

        if (decl.size() != s_fvfVertexSize[j].declLength + 1)
        {
            success = false;
            printf("\nERROR (2): %zu: %zu .. %zu\n", j, decl.size(), s_fvfVertexSize[j].declLength + 1);
        }
        else if (s_fvfVertexSize[j].pDecl)
        {
            // TODO - Compare decls
        }
    }

    // invalid args
    std::vector<D3DVERTEXELEMENT9> decl;
    if (FVF::CreateDeclFromFVF(D3DFVF_RESERVED0, decl) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test A failed\n");
    }

    if (FVF::CreateDeclFromFVF(D3DFVF_RESERVED2, decl) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test B failed\n");
    }

    if (FVF::CreateDeclFromFVF(0xFF, decl) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test C failed\n");
    }

    if (FVF::CreateDeclFromFVF(D3DFVF_XYZ | 0xF00 /* 15 texture coords*/, decl) != 0)
    {
        success = false;
        printf("\nERROR: invalid args test D failed\n");
    }

    return success;
}
