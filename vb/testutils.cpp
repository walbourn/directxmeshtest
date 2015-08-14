//-------------------------------------------------------------------------------------
// testutils.cpp
//  
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#include "directxtest.h"

#include "directxmeshp.h"

#include <dxgiformat.h>

using namespace DirectX;

//-------------------------------------------------------------------------------------
#define ENUM_CASE(x) case x: return L#x;

const wchar_t* GetName( DXGI_FORMAT fmt )
{
    switch( static_cast<int>(fmt) )
    {
    ENUM_CASE( DXGI_FORMAT_R32G32B32A32_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_R32G32B32A32_FLOAT )
    ENUM_CASE( DXGI_FORMAT_R32G32B32A32_UINT )
    ENUM_CASE( DXGI_FORMAT_R32G32B32A32_SINT )
    ENUM_CASE( DXGI_FORMAT_R32G32B32_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_R32G32B32_FLOAT )
    ENUM_CASE( DXGI_FORMAT_R32G32B32_UINT )
    ENUM_CASE( DXGI_FORMAT_R32G32B32_SINT )
    ENUM_CASE( DXGI_FORMAT_R16G16B16A16_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_R16G16B16A16_FLOAT )
    ENUM_CASE( DXGI_FORMAT_R16G16B16A16_UNORM )
    ENUM_CASE( DXGI_FORMAT_R16G16B16A16_UINT )
    ENUM_CASE( DXGI_FORMAT_R16G16B16A16_SNORM )
    ENUM_CASE( DXGI_FORMAT_R16G16B16A16_SINT )
    ENUM_CASE( DXGI_FORMAT_R32G32_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_R32G32_FLOAT )
    ENUM_CASE( DXGI_FORMAT_R32G32_UINT )
    ENUM_CASE( DXGI_FORMAT_R32G32_SINT )
    ENUM_CASE( DXGI_FORMAT_R32G8X24_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_D32_FLOAT_S8X24_UINT )
    ENUM_CASE( DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_X32_TYPELESS_G8X24_UINT )
    ENUM_CASE( DXGI_FORMAT_R10G10B10A2_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_R10G10B10A2_UNORM )
    ENUM_CASE( DXGI_FORMAT_R10G10B10A2_UINT )
    ENUM_CASE( DXGI_FORMAT_R11G11B10_FLOAT )
    ENUM_CASE( DXGI_FORMAT_R8G8B8A8_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_R8G8B8A8_UNORM )
    ENUM_CASE( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB )
    ENUM_CASE( DXGI_FORMAT_R8G8B8A8_UINT )
    ENUM_CASE( DXGI_FORMAT_R8G8B8A8_SNORM )
    ENUM_CASE( DXGI_FORMAT_R8G8B8A8_SINT )
    ENUM_CASE( DXGI_FORMAT_R16G16_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_R16G16_FLOAT )
    ENUM_CASE( DXGI_FORMAT_R16G16_UNORM )
    ENUM_CASE( DXGI_FORMAT_R16G16_UINT )
    ENUM_CASE( DXGI_FORMAT_R16G16_SNORM )
    ENUM_CASE( DXGI_FORMAT_R16G16_SINT )
    ENUM_CASE( DXGI_FORMAT_R32_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_D32_FLOAT )
    ENUM_CASE( DXGI_FORMAT_R32_FLOAT )
    ENUM_CASE( DXGI_FORMAT_R32_UINT )
    ENUM_CASE( DXGI_FORMAT_R32_SINT )
    ENUM_CASE( DXGI_FORMAT_R24G8_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_D24_UNORM_S8_UINT )
    ENUM_CASE( DXGI_FORMAT_R24_UNORM_X8_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_X24_TYPELESS_G8_UINT )
    ENUM_CASE( DXGI_FORMAT_R8G8_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_R8G8_UNORM )
    ENUM_CASE( DXGI_FORMAT_R8G8_UINT )
    ENUM_CASE( DXGI_FORMAT_R8G8_SNORM )
    ENUM_CASE( DXGI_FORMAT_R8G8_SINT )
    ENUM_CASE( DXGI_FORMAT_R16_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_R16_FLOAT )
    ENUM_CASE( DXGI_FORMAT_D16_UNORM )
    ENUM_CASE( DXGI_FORMAT_R16_UNORM )
    ENUM_CASE( DXGI_FORMAT_R16_UINT )
    ENUM_CASE( DXGI_FORMAT_R16_SNORM )
    ENUM_CASE( DXGI_FORMAT_R16_SINT )
    ENUM_CASE( DXGI_FORMAT_R8_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_R8_UNORM )
    ENUM_CASE( DXGI_FORMAT_R8_UINT )
    ENUM_CASE( DXGI_FORMAT_R8_SNORM )
    ENUM_CASE( DXGI_FORMAT_R8_SINT )
    ENUM_CASE( DXGI_FORMAT_A8_UNORM )
    ENUM_CASE( DXGI_FORMAT_R1_UNORM )
    ENUM_CASE( DXGI_FORMAT_R9G9B9E5_SHAREDEXP )
    ENUM_CASE( DXGI_FORMAT_R8G8_B8G8_UNORM )
    ENUM_CASE( DXGI_FORMAT_G8R8_G8B8_UNORM )
    ENUM_CASE( DXGI_FORMAT_BC1_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_BC1_UNORM )
    ENUM_CASE( DXGI_FORMAT_BC1_UNORM_SRGB )
    ENUM_CASE( DXGI_FORMAT_BC2_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_BC2_UNORM )
    ENUM_CASE( DXGI_FORMAT_BC2_UNORM_SRGB )
    ENUM_CASE( DXGI_FORMAT_BC3_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_BC3_UNORM )
    ENUM_CASE( DXGI_FORMAT_BC3_UNORM_SRGB )
    ENUM_CASE( DXGI_FORMAT_BC4_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_BC4_UNORM )
    ENUM_CASE( DXGI_FORMAT_BC4_SNORM )
    ENUM_CASE( DXGI_FORMAT_BC5_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_BC5_UNORM )
    ENUM_CASE( DXGI_FORMAT_BC5_SNORM )
    ENUM_CASE( DXGI_FORMAT_B5G6R5_UNORM )
    ENUM_CASE( DXGI_FORMAT_B5G5R5A1_UNORM )
    ENUM_CASE( DXGI_FORMAT_B8G8R8A8_UNORM )
    ENUM_CASE( DXGI_FORMAT_B8G8R8X8_UNORM )
    ENUM_CASE( DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM )
    ENUM_CASE( DXGI_FORMAT_B8G8R8A8_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_B8G8R8A8_UNORM_SRGB )
    ENUM_CASE( DXGI_FORMAT_B8G8R8X8_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_B8G8R8X8_UNORM_SRGB )
    ENUM_CASE( DXGI_FORMAT_BC6H_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_BC6H_UF16 )
    ENUM_CASE( DXGI_FORMAT_BC6H_SF16 )
    ENUM_CASE( DXGI_FORMAT_BC7_TYPELESS )
    ENUM_CASE( DXGI_FORMAT_BC7_UNORM )
    ENUM_CASE( DXGI_FORMAT_BC7_UNORM_SRGB )
    ENUM_CASE( DXGI_FORMAT_AYUV )
    ENUM_CASE( DXGI_FORMAT_Y410 )
    ENUM_CASE( DXGI_FORMAT_Y416 )
    ENUM_CASE( DXGI_FORMAT_NV12 )
    ENUM_CASE( DXGI_FORMAT_P010 )
    ENUM_CASE( DXGI_FORMAT_P016 )
    ENUM_CASE( DXGI_FORMAT_420_OPAQUE )
    ENUM_CASE( DXGI_FORMAT_YUY2 )
    ENUM_CASE( DXGI_FORMAT_Y210 )
    ENUM_CASE( DXGI_FORMAT_Y216 )
    ENUM_CASE( DXGI_FORMAT_NV11 )
    ENUM_CASE( DXGI_FORMAT_AI44 )
    ENUM_CASE( DXGI_FORMAT_IA44 )
    ENUM_CASE( DXGI_FORMAT_P8 )
    ENUM_CASE( DXGI_FORMAT_A8P8 )
    ENUM_CASE( DXGI_FORMAT_B4G4R4A4_UNORM )
    case XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM: return L"DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM";

    default:
        return L"UNKNOWN";
    }
}