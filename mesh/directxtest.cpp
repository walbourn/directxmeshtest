//-------------------------------------------------------------------------------------
// DirectXTest.cpp
//  
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#include "directxtest.h"

#include "DirectXMesh.h"

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
extern bool Test08();
extern bool Test09();
extern bool Test10();
extern bool Test11();
extern bool Test12();
extern bool Test13();
extern bool Test14();
extern bool Test15();
extern bool Test16();
extern bool Test17();
extern bool Test18();
extern bool Test19();
extern bool Test20();
extern bool Test21();
extern bool Test22();
extern bool Test23();
extern bool Test24();
extern bool Test25();
extern bool Test26();
extern bool Test27();
extern bool Test28();
extern bool Test29();
extern bool Test30();

TestInfo g_Tests[] =
{
    { "IsValid*/BPE", Test01 },
    { "TestHelpers (internal)", Test02 },
    { "ReorderIB", Test18 },
    { "FinalizeIB", Test03 },
    { "FinalizeVB", Test04 },
    { "FinalizeVB (duplicates)", Test05 },
    { "Validate", Test06 },
    { "GenerateAdjacencyAndPointReps (point reps)", Test07 },
    { "GenerateAdjacencyAndPointReps (adjacency)", Test08 },
    { "ConvertPointRepsToAdjacency", Test12 },
    { "Validate (adjacency)", Test09 },
    { "ReorderIBAndAdjacency", Test23 },
    { "FinalizeVBAndPointReps", Test19 },
    { "FinalizeVBAndPointReps (duplicates)", Test20 },
    { "GenerateGSAdjacency", Test10 },
    { "ComputeNormals", Test11 },
    { "ComputeTangentFrame", Test13 },
    { "Clean", Test14 },
    { "Clean (attributes)", Test21 },
    { "ComputeSubsets", Test24 },
    { "AttributeSort", Test15 },
    { "ComputeVertexCacheMissRate", Test22 },
    { "OptimizeFaces", Test16 },
    { "OptimizeFacesLRU", Test25 },
    { "OptimizeVertices", Test17 },
    { "WeldVertices", Test26 },
    { "ConcatenateMesh", Test30 },
    { "CompactVB", Test27 },
    { "ComputeMeshlets", Test28 },
    { "ComputeCullData", Test29 }
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

        _CrtDumpMemoryLeaks();
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

#ifdef _MSC_VER
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if( FAILED(hr))
    {
        printe("ERROR: CoInitializeEx fails (%08X)\n", static_cast<unsigned int>(hr));
        return -1;
    }

    if ( !RunTests() )
        return -1;
       
    return 0;
}
