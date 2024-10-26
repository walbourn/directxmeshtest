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
        OPT_SDKMESH,
        OPT_MAX
    };

    static_assert(OPT_MAX <= 32, "dwOptions is a DWORD bitfield");

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////

    const SValue<uint32_t> g_pOptions[] =
    {
        { L"r",         OPT_RECURSIVE },
        { L"wfo",       OPT_WAVEFRONT_OBJ },
        { L"sdkmesh",   OPT_SDKMESH },
        { nullptr,      0 }
    };

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////

    //---------------------------------------------------------------------------------
    // Memory blob
    class Blob
    {
    public:
        Blob() noexcept : m_buffer(nullptr), m_size(0) {}
        Blob(Blob&& moveFrom) noexcept : m_buffer(nullptr), m_size(0) { *this = std::move(moveFrom); }
        ~Blob() { Release(); }

        Blob& operator= (Blob&& moveFrom) noexcept
        {
            if (this != &moveFrom)
            {
                Release();

                m_buffer = moveFrom.m_buffer;
                m_size = moveFrom.m_size;

                moveFrom.m_buffer = nullptr;
                moveFrom.m_size = 0;
            }
            return *this;
        }

        Blob(const Blob&) = delete;
        Blob& operator=(const Blob&) = delete;

        HRESULT Initialize(_In_ size_t size) noexcept
        {
            if (!size)
                return E_INVALIDARG;

            Release();

            m_buffer = new (std::nothrow) uint8_t[size];
            if (!m_buffer)
            {
                Release();
                return E_OUTOFMEMORY;
            }

            m_size = size;

            return S_OK;
        }

        void Release() noexcept
        {
            if (m_buffer)
            {
                delete [] m_buffer;
                m_buffer = nullptr;
            }

            m_size = 0;
        }

        void *GetBufferPointer() const noexcept { return m_buffer; }
        size_t GetBufferSize() const noexcept { return m_size; }

    private:
        void*   m_buffer;
        size_t  m_size;
    };

    //-------------------------------------------------------------------------------------
    HRESULT LoadBlobFromFile(_In_z_ const wchar_t* szFile, Blob& blob)
    {
        if (szFile == nullptr)
            return E_INVALIDARG;

        ScopedHandle hFile(safe_handle(CreateFileW(szFile, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
            FILE_FLAG_SEQUENTIAL_SCAN, nullptr)));
        if (!hFile)
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        // Get the file size
        LARGE_INTEGER fileSize = {};
        if (!GetFileSizeEx(hFile.get(), &fileSize))
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        // File is too big for 32-bit allocation, so reject read (4 GB should be plenty large enough for our test images)
        if (fileSize.HighPart > 0)
        {
            return HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE);
        }

        // Need at least 1 byte of data
        if (!fileSize.LowPart)
        {
            return E_FAIL;
        }

        // Create blob memory
        HRESULT hr = blob.Initialize(fileSize.LowPart);
        if (FAILED(hr))
            return hr;

        // Load entire file into blob memory
        DWORD bytesRead = 0;
        if (!ReadFile(hFile.get(), blob.GetBufferPointer(), static_cast<DWORD>(blob.GetBufferSize()), &bytesRead, nullptr))
        {
            blob.Release();
            return HRESULT_FROM_WIN32(GetLastError());
        }

        // Verify we got the whole blob loaded
        if (bytesRead != blob.GetBufferSize())
        {
            blob.Release();
            return E_FAIL;
        }

        return S_OK;
    }

    void PrintUsage()
    {
        PrintLogo(false, g_ToolName, g_Description);

        static const wchar_t* const s_usage =
            L"Usage: fuzzloaders <options> <files>\n"
            L"\n"
            L"   -r                  wildcard filename search is recursive\n"
            L"   -wfo                force use of WaveFront OBJ loader\n"
            L"   -sdkmesh            force use of SDKMESH loader\n";

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
    DWORD dwOptions = 0;
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

            DWORD dwOption = LookupByName(pArg, g_pOptions);

            if (!dwOption || (dwOptions & (1 << dwOption)))
            {
                PrintUsage();
                return 1;
            }

            dwOptions |= 1 << dwOption;

            switch (dwOption)
            {
            case OPT_WAVEFRONT_OBJ:
            case OPT_SDKMESH:
                {
                    DWORD mask = (1 << OPT_WAVEFRONT_OBJ)
                        | (1 << OPT_SDKMESH);
                    mask &= ~(1 << dwOption);
                    if (dwOptions & mask)
                    {
                        wprintf(L"-wfo, -sdkmesh are mutually exclusive options\n");
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
            SConversion conv;
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
        wchar_t ext[_MAX_EXT];
        _wsplitpath_s(pConv.szSrc.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);
        bool iswfo = (_wcsicmp(ext, L"._obj") == 0) || (_wcsicmp(ext, L".obj") == 0);
        bool issdkmesh = (_wcsicmp(ext, L".sdkmesh") == 0);

        bool usewfo = false;
        bool usesdkmesh = false;
        if (dwOptions & (1 << OPT_WAVEFRONT_OBJ))
        {
            usewfo = true;
        }
        else if (dwOptions & (1 << OPT_SDKMESH))
        {
            usesdkmesh = true;
        }

        // Load source image
#ifdef _DEBUG
        OutputDebugStringW(pConv.szSrc.c_str());
        OutputDebugStringA("\n");
#endif

        if (usewfo)
        {
            bool pass = false;

#if 0
            hr = DirectX::LoadFromDDSFile(pConv.szSrc.c_str(), c_ddsFlags, nullptr, result);
            if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            {
                wprintf(L"ERROR: DDSTexture file not not found:\n%ls\n", pConv.szSrc.c_str());
                return 1;
            }
            else if (FAILED(hr) && hr != E_INVALIDARG && hr != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED) && hr != E_OUTOFMEMORY && hr != HRESULT_FROM_WIN32(ERROR_HANDLE_EOF) && (hr != E_FAIL || (hr == E_FAIL && isdds)))
            {
#ifdef _DEBUG
                char buff[128] = {};
                sprintf_s(buff, "DDSTextureFromFile failed with %08X\n", static_cast<unsigned int>(hr));
                OutputDebugStringA(buff);
#endif
                wprintf(L"!");
            }
            else
            {
                pass = true;
            }

            // Validate memory version
            {
                DirectX::Blob blob;
                hr = LoadBlobFromFile(pConv.szSrc.c_str(), blob);

                if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                {
                    wprintf(L"ERROR: DDSTexture file not not found:\n%ls\n", pConv.szSrc.c_str());
                    return 1;
                }
                if (FAILED(hr) && hr != E_OUTOFMEMORY)
                {
#ifdef _DEBUG
                    char buff[128] = {};
                    sprintf_s(buff, "LoadBlobFromFile failed with %08X\n", static_cast<unsigned int>(hr));
                    OutputDebugStringA(buff);
#endif
                    wprintf(L"!");
                }
                else
                {
                    hr = DirectX::LoadFromDDSMemory(blob.GetBufferPointer(), blob.GetBufferSize(), c_ddsFlags, nullptr, result);
                    if (FAILED(hr) && hr != E_INVALIDARG && hr != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED) && hr != E_OUTOFMEMORY && hr != HRESULT_FROM_WIN32(ERROR_HANDLE_EOF) && (hr != E_FAIL || (hr == E_FAIL && isdds)))
                    {
#ifdef _DEBUG
                        char buff[128] = {};
                        sprintf_s(buff, "DDSTextureFromMemory failed with %08X\n", static_cast<unsigned int>(hr));
                        OutputDebugStringA(buff);
#endif
                        wprintf(L"!");
                    }
                    else if (pass)
                    {
                        wprintf(L"%ls", SUCCEEDED(hr) ? L"*" : L".");
                    }
                }
            }
#endif
        }
        else if (usesdkmesh)
        {
#if 0
            hr = DirectX::LoadFromHDRFile(pConv.szSrc.c_str(), nullptr, result); // LoadFromHDRFile exercises LoadFromHDRMemory
            if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            {
                wprintf(L"ERROR: HDRTexture file not not found:\n%ls\n", pConv.szSrc.c_str());
                return 1;
            }
            else if (FAILED(hr) && hr != E_INVALIDARG && hr != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED) && hr != E_OUTOFMEMORY && hr != HRESULT_FROM_WIN32(ERROR_HANDLE_EOF) && (hr != E_FAIL || (hr == E_FAIL && ishdr)))
            {
#ifdef _DEBUG
                char buff[128] = {};
                sprintf_s(buff, "HDRTexture failed with %08X\n", static_cast<unsigned int>(hr));
                OutputDebugStringA(buff);
#endif
                wprintf(L"!");
            }
            else
            {
                wprintf(L"%ls", SUCCEEDED(hr) ? L"*" : L".");
            }
#endif
        }
        else
        {
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
    // Validate memory version

#if 0
    std::ignore = DirectX::LoadFromDDSMemory(data, size, c_ddsFlags, nullptr, result);
    std::ignore = DirectX::LoadFromTGAMemory(data, size, DirectX::TGA_FLAGS_NONE, nullptr, result);
    std::ignore = DirectX::LoadFromTGAMemory(data, size, DirectX::TGA_FLAGS_BGR, nullptr, result);
    std::ignore = LoadFromHDRMemory(data, size, nullptr, result);
#endif

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

#if 0
    std::ignore = DirectX::LoadFromDDSFile(tempFileName, c_ddsFlags, nullptr, result);
    std::ignore = DirectX::LoadFromHDRFile(tempFileName, nullptr, result);
    std::ignore = DirectX::LoadFromTGAFile(tempFileName, DirectX::TGA_FLAGS_NONE, nullptr, result);
    std::ignore = DirectX::LoadFromTGAFile(tempFileName, DirectX::TGA_FLAGS_BGR, nullptr, result);
    std::ignore = LoadFromPortablePixMap(tempFileName, nullptr, result);
    std::ignore = LoadFromPortablePixMapHDR(tempFileName, nullptr, result);
#endif

    return 0;
}

#endif // FUZZING_BUILD_MODE
