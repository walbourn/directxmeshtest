//--------------------------------------------------------------------------------------
// File: fuzzloaders.cpp
//
// Simple command-line tool for fuzz-testing for mesh loaders
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation.
//
// http://go.microsoft.com/fwlink/?LinkID=324981
//--------------------------------------------------------------------------------------

#pragma warning(push)
#pragma warning(disable : 4005)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP
#pragma warning(pop)

#include <Windows.h>
#include <dxgiformat.h>

#include <cassert>
#include <cstdio>
#include <cwchar>
#include <memory>
#include <new>
#include <list>
#include <tuple>
#include <utility>

#include "DirectXMesh.h"
#include "WaveFrontReader.h"
#include "Mesh.h"

#define TOOL_VERSION DIRECTX_MESH_VERSION
#include "CmdLineHelpers.h"

using namespace Helpers;


namespace
{
    const wchar_t* g_ToolName = L"fuzzloaders";
    const wchar_t* g_Description = L"Microsoft (R) DirectXMesh File Fuzzing Harness";

#ifndef FUZZING_BUILD_MODE

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////

    enum OPTIONS : uint32_t
    {
        OPT_RECURSIVE = 1,
        OPT_WAVEFRONT_OBJ,
        OPT_WAVEFRONT_MTL,
        OPT_VBO,
        OPT_MAX
    };

    static_assert(OPT_MAX <= 32, "dwOptions is a unsigned int bitfield");

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////

    const SValue<uint32_t> g_pOptions[] =
    {
        { L"r",         OPT_RECURSIVE },
        { L"wfo",       OPT_WAVEFRONT_OBJ },
        { L"mtl",       OPT_WAVEFRONT_MTL },
        { L"vbo",       OPT_VBO },
        { nullptr,      0 }
    };

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////

    void PrintUsage()
    {
        PrintLogo(false, g_ToolName, g_Description);

        static const wchar_t* const s_usage =
            L"Usage: fuzzloaders <options> <files>\n"
            L"\n"
            L"   -r                  wildcard filename search is recursive\n"
            L"   -wfo                force use of WaveFront OBJ loader\n"
            L"   -mtl                force use of WaveFront MTL loader\n"
            L"   -vbo                force use of VBO loader\n";

        wprintf(L"%ls", s_usage);
    }

#endif // !FUZZING_BUILD_MODE
}

//--------------------------------------------------------------------------------------
// Entry-point
//--------------------------------------------------------------------------------------
#ifndef FUZZING_BUILD_MODE

#ifdef _PREFAST_
#pragma prefast(disable : 28198, "Command-line tool, frees all memory on exit")
#endif

int __cdecl wmain(_In_ int argc, _In_z_count_(argc) wchar_t* argv[])
{
    // Process command line
    uint32_t dwOptions = 0;
    std::list<SConversion> conversion;

    for (int iArg = 1; iArg < argc; iArg++)
    {
        PWSTR pArg = argv[iArg];

        if (('-' == pArg[0]) || ('/' == pArg[0]))
        {
            pArg++;
            PWSTR pValue;

            for (pValue = pArg; *pValue && (':' != *pValue); pValue++);

            if (*pValue)
                *pValue++ = 0;

            uint32_t dwOption = LookupByName(pArg, g_pOptions);

            if (!dwOption || (dwOptions & (1 << dwOption)))
            {
                PrintUsage();
                return 1;
            }

            dwOptions |= 1 << dwOption;

            switch (dwOption)
            {
            case OPT_WAVEFRONT_OBJ:
            case OPT_WAVEFRONT_MTL:
            case OPT_VBO:
                {
                    uint32_t mask = (1 << OPT_WAVEFRONT_OBJ)
                        | (1 << OPT_WAVEFRONT_MTL)
                        | (1 << OPT_VBO);
                    mask &= ~(1 << dwOption);
                    if (dwOptions & mask)
                    {
                        wprintf(L"-wfo, -mtl, -vbo are mutually exclusive options\n");
                        return 1;
                    }
                }
                break;

            default:
                break;
            }
        }
        else if (wcspbrk(pArg, L"?*") != nullptr)
        {
            size_t count = conversion.size();
            SearchForFiles(pArg, conversion, (dwOptions & (1 << OPT_RECURSIVE)) != 0, nullptr);
            if (conversion.size() <= count)
            {
                wprintf(L"No matching files found for %ls\n", pArg);
                return 1;
            }
        }
        else
        {
            SConversion conv = {};
            conv.szSrc = pArg;

            conversion.push_back(conv);
        }
    }

    if (conversion.empty())
    {
        wprintf(L"ERROR: Need at least 1 image file to fuzz\n\n");
        PrintUsage();
        return 0;
    }

    for (auto& pConv : conversion)
    {
        wchar_t ext[_MAX_EXT] = {};
        _wsplitpath_s(pConv.szSrc.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);
        const bool iswfo = (_wcsicmp(ext, L"._obj") == 0) || (_wcsicmp(ext, L".obj") == 0);
        const bool ismtl = (_wcsicmp(ext, L".mtl") == 0);
        const bool isvbo = (_wcsicmp(ext, L".vbo") == 0);

        bool usewfo = false;
        bool usemtl = false;
        bool usevbo = false;
        if (dwOptions & (1 << OPT_WAVEFRONT_OBJ))
        {
            usewfo = true;
        }
        else if (dwOptions & (1 << OPT_WAVEFRONT_MTL))
        {
            usemtl = true;
        }
        else if (dwOptions & (1 << OPT_VBO))
        {
            usevbo = true;
        }
        else
        {
            usewfo = true;
            usemtl = true;
            usevbo = true;
        }

        // Load source image
#ifdef _DEBUG
        OutputDebugStringW(pConv.szSrc.c_str());
        OutputDebugStringA("\n");
#endif

        if (usewfo)
        {
            DX::WaveFrontReader<uint32_t> wfr;

            // Loads ignoring any MTL file reference.
            HRESULT hr = wfr.Load(pConv.szSrc.c_str(), true, false);
            if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            {
                wprintf(L"ERROR: WaveFront OBJ file not not found:\n%ls\n", pConv.szSrc.c_str());
                return 1;
            }
            else if (FAILED(hr)
                     && hr != E_INVALIDARG
                     && hr != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED)
                     && hr != E_OUTOFMEMORY
                     && hr != HRESULT_FROM_WIN32(ERROR_HANDLE_EOF)
                     && hr != HRESULT_FROM_WIN32(ERROR_INVALID_DATA)
                     && (hr != E_FAIL || (hr == E_FAIL && iswfo)))
            {
#ifdef _DEBUG
                char buff[128] = {};
                sprintf_s(buff, "WaveFront OBJ failed with %08X\n", static_cast<unsigned int>(hr));
                OutputDebugStringA(buff);
#endif
                wprintf(L"!");
            }
            else
            {
                wprintf(L"%ls", SUCCEEDED(hr) ? L"*" : L".");
            }
        }

        if (usemtl)
        {
            DX::WaveFrontReader<uint32_t> wfr;

            HRESULT hr = wfr.LoadMTL(pConv.szSrc.c_str());
            if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            {
                wprintf(L"ERROR: WaveFront MTL file not not found:\n%ls\n", pConv.szSrc.c_str());
                return 1;
            }
            else if (FAILED(hr)
                     && hr != E_INVALIDARG
                     && hr != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED)
                     && hr != E_OUTOFMEMORY
                     && hr != HRESULT_FROM_WIN32(ERROR_HANDLE_EOF)
                     && hr != HRESULT_FROM_WIN32(ERROR_INVALID_DATA)
                     && (hr != E_FAIL || (hr == E_FAIL && ismtl)))
            {
#ifdef _DEBUG
                char buff[128] = {};
                sprintf_s(buff, "WaveFront MTL failed with %08X\n", static_cast<unsigned int>(hr));
                OutputDebugStringA(buff);
#endif
                wprintf(L"!");
            }
            else
            {
                wprintf(L"%ls", SUCCEEDED(hr) ? L"*" : L".");
            }
        }

        if (usevbo)
        {
            std::unique_ptr<Mesh> inMesh;
            HRESULT hr = Mesh::CreateFromVBO(pConv.szSrc.c_str(), inMesh);
            if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            {
                wprintf(L"ERROR: VBO file not not found:\n%ls\n", pConv.szSrc.c_str());
                return 1;
            }
            else if (FAILED(hr)
                     && hr != E_INVALIDARG
                     && hr != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED)
                     && hr != E_OUTOFMEMORY
                     && hr != HRESULT_FROM_WIN32(ERROR_HANDLE_EOF)
                     && hr != HRESULT_FROM_WIN32(ERROR_INVALID_DATA)
                     && hr != HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW)
                     && (hr != E_FAIL || (hr == E_FAIL && isvbo)))
            {
#ifdef _DEBUG
                char buff[128] = {};
                sprintf_s(buff, "VBO failed with %08X\n", static_cast<unsigned int>(hr));
                OutputDebugStringA(buff);
#endif
                wprintf(L"!");
            }
            else
            {
                wprintf(L"%ls", SUCCEEDED(hr) ? L"*" : L".");
            }
        }
        fflush(stdout);
    }

    wprintf(L"\n*** FUZZING COMPLETE ***\n");

    return 0;
}


#else // FUZZING_BUILD_MODE


//--------------------------------------------------------------------------------------
// Libfuzzer entry-point
//--------------------------------------------------------------------------------------
extern "C" __declspec(dllexport) int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    // Disk version
    wchar_t tempFileName[MAX_PATH] = {};
    wchar_t tempPath[MAX_PATH] = {};

    if (!GetTempPathW(MAX_PATH, tempPath))
        return 0;

    if (!GetTempFileNameW(tempPath, L"fuzz", 0, tempFileName))
        return 0;

    {
        ScopedHandle hFile(safe_handle(CreateFileW(tempFileName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL, nullptr)));
        if (!hFile)
            return 0;

        DWORD bytesWritten = 0;
        if (!WriteFile(hFile.get(), data, static_cast<DWORD>(size), &bytesWritten, nullptr))
            return 0;
    }

    {
        DX::WaveFrontReader<uint32_t> wfr;
        // Loads ignoring any MTL file reference.
        std::ignore = wfr.Load(tempFileName, true, false);
    }

    {
        DX::WaveFrontReader<uint32_t> wfr;
        std::ignore = wfr.LoadMTL(tempFileName);
    }

    {
        std::unique_ptr<Mesh> inMesh;
        std::ignore = Mesh::CreateFromVBO(tempFileName, inMesh);
    }

    return 0;
}

#endif // FUZZING_BUILD_MODE
