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
extern bool Test02();

TestInfo g_Tests[] =
{
    { "WaveFrontReader", Test01 },
    { "ComputeFVFVertexSize", Test02 }
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
int __cdecl main()
{
    printf("**************************************************************\n");
    printf("*** DirectXMesh Utilities test\n");
    printf("**************************************************************\n");

    if (!RunTests())
        return -1;

    return 0;
}
