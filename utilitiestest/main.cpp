// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <cstdint>
#include <cstdio>

//-------------------------------------------------------------------------------------
// Types and globals

typedef bool (*TestFN)();

struct TestInfo
{
    const char* name;
    TestFN func;
};

extern bool Test01();
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
extern bool Test02();
extern bool Test03();
extern bool Test04();
extern bool Test05();
extern bool Test06();
extern bool Test07();
extern bool Test08();
#endif

TestInfo g_Tests[] =
{
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
    { "ComputeVertexSize(FVF)", Test02 },
    { "ComputeVertexSize(pDecl)", Test03 },
    { "GetDeclLength", Test04 },
    { "ComputeFVF", Test05 },
    { "CreateDeclFromFVF", Test06 },
    { "CreateInputLayoutFromFVF(D3D11)", Test07 },
    { "CreateInputLayoutFromFVF(D3D12)", Test08 },
#endif
    { "WaveFrontReader", Test01 }
};


//-------------------------------------------------------------------------------------
bool RunTests()
{
    size_t nPass = 0;
    size_t nFail = 0;

    for (size_t i = 0; i < (sizeof(g_Tests) / sizeof(TestInfo)); ++i)
    {
        printf("%s: ", g_Tests[i].name);

        if (g_Tests[i].func())
        {
            ++nPass;
            printf("PASS\n");
        }
        else
        {
            ++nFail;
            printf("FAIL\n");
        }
    }

    printf("Ran %zu tests, %zu pass, %zu fail\n", nPass + nFail, nPass, nFail);

    return (nFail == 0);
}


//-------------------------------------------------------------------------------------
int __cdecl wmain()
{
    printf("**************************************************************\n");
    printf("*** DirectXMesh Utilities test\n");
    printf("**************************************************************\n");

    if (!RunTests())
        return -1;

    return 0;
}
