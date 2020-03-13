//-------------------------------------------------------------------------------------
// DirectXTest.cpp
//  
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------

#include "directxtest.h"

#include "directxmesh.h"

using namespace DirectX;

//-------------------------------------------------------------------------------------
// Types and globals

typedef bool (*TestFN)();

struct TestInfo
{
    const char *name;
    TestFN func;
};

extern bool Test01();
extern bool Test02();
extern bool Test03();
extern bool Test04();
extern bool Test05();
extern bool Test06();
extern bool Test07();

extern bool Test01_DX12();
extern bool Test02_DX12();
extern bool Test03_DX12();
extern bool Test04_DX12();
extern bool Test05_DX12();
extern bool Test06_DX12();
extern bool Test07_DX12();

TestInfo g_Tests[] =
{
    { "IsValid (D3D11)", Test01 },
    { "ComputeInputLayout (D3D11)", Test02 },
    { "VBReader (D3D11)", Test03 },
    { "VBWriter (D3D11)", Test04 },
    { "VBReader Formats (D3D11)", Test05 },
    { "VBWriter Formats (D3D11)", Test06 },
    { "VBR/W Examples (D3D11)", Test07 },

#if (_WIN32_WINNT >= 0x0A00)
    { "IsValid (D3D12)", Test01_DX12 },
    { "ComputeInputLayout (D3D12)", Test02_DX12 },
    { "VBReader (D3D12)", Test03_DX12 },
    { "VBWriter (D3D12)", Test04_DX12 },
    { "VBReader Formats (D3D12)", Test05_DX12 },
    { "VBWriter Formats (D3D12)", Test06_DX12 },
    { "VBR/W Examples (D3D12)", Test07_DX12 },
#endif
};


//-------------------------------------------------------------------------------------
bool RunTests()
{
    UINT nPass = 0;
    UINT nFail = 0;

    for( UINT i=0; i < ( sizeof(g_Tests) / sizeof(TestInfo) ); ++i)
    {
        print("%s: ", g_Tests[i].name );

        if ( g_Tests[i].func() )
        {
            ++nPass;
            print("PASS\n");
        }
        else
        {
            ++nFail;
            print("FAIL\n");
        }
    }

    print("Ran %d tests, %d pass, %d fail\n", nPass+nFail, nPass, nFail);

    return (nFail == 0);
}


//-------------------------------------------------------------------------------------
int __cdecl main()
{
    print("**************************************************************\n");
    print("*** " _DIRECTX_TEST_NAME_ " test\n" );
    print("*** Library Version %03d\n", DIRECTX_MESH_VERSION );
    print("**************************************************************\n");

    if ( !XMVerifyCPUSupport() )
    {
        printe("ERROR: XMVerifyCPUSupport fails on this system, not a supported platform\n");
        return -1;
    }

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if(FAILED(hr))
    {
        printe("ERROR: CoInitializeEx fails (%08X)\n", static_cast<unsigned int>(hr));
        return -1;
    }

    if ( !RunTests() )
        return -1;
       
    return 0;
}