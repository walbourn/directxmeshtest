//-------------------------------------------------------------------------------------
// misc.cpp
//
// Copyright (c) Microsoft Corporation.
//-------------------------------------------------------------------------------------

#include <assert.h>

#include "directxtest.h"

#include "DirectXMeshP.h"
#include "scoped.h"

using namespace DirectX;

//-------------------------------------------------------------------------------------
// Test coverage for internal helpers
bool Test00()
{
    bool success = true;

    // scoped.h
    {
        auto fa = make_AlignedArrayFloat(2048);
        if (!fa)
        {
            printe( "ERROR: make_AlignedArrayFloat FAILED\n" );
            success = false;
        }

        fa = make_AlignedArrayFloat(0);
        if (!fa)
        {
            printe( "ERROR: make_AlignedArrayFloat zero FAILED\n" );
            success = false;
        }

        fa = make_AlignedArrayFloat(UINT32_MAX * 2);
        if (fa)
        {
            printe( "ERROR: make_AlignedArrayFloat too big FAILED\n" );
            success = false;
        }

        auto va = make_AlignedArrayXMVECTOR(2048);
        if (!va)
        {
            printe( "ERROR: make_AlignedArrayXMVECTOR FAILED\n" );
            success = false;
        }

        va = make_AlignedArrayXMVECTOR(0);
        if (!va)
        {
            printe( "ERROR: make_AlignedArrayXMVECTOR zero FAILED\n" );
            success = false;
        }

        va = make_AlignedArrayXMVECTOR(UINT32_MAX * 2);
        if (va)
        {
            printe( "ERROR: make_AlignedArrayXMVECTOR too big FAILED\n" );
            success = false;
        }

        {
            wchar_t tempPath[MAX_PATH] = {};
            if (!GetTempPathW(MAX_PATH, tempPath))
            {
                printe( "ERROR: GetTempPath FAILED (%08X)\n", static_cast<unsigned int>(HRESULT_FROM_WIN32(GetLastError())));
                return false;
            }

            wchar_t tempFile[MAX_PATH] = {};
            if (!GetTempFileNameW(tempPath, L"dxtex", 0, tempFile))
            {
                printe( "ERROR: GetTempFileNameW FAILED (%08X)\n", static_cast<unsigned int>(HRESULT_FROM_WIN32(GetLastError())));
                return false;
            }

            HANDLE hFile = CreateFile2(
                tempFile,
                GENERIC_WRITE | DELETE, 0, CREATE_ALWAYS,
                nullptr);
            if (!hFile)
            {
                printe( "ERROR: CrateFile FAILED (%08X)\n%S\n", static_cast<unsigned int>(HRESULT_FROM_WIN32(GetLastError())), tempFile);
                return false;
            }

            {
                auto_delete_file delonfail(hFile);
                // Let this get destructed with the handle active.
            }

            if (!CloseHandle(hFile))
            {
                printe( "ERROR: CloseHandle FAILED (%08X)\n", static_cast<unsigned int>(HRESULT_FROM_WIN32(GetLastError())));
                return false;
            }

            if (GetFileAttributesW(tempFile) != INVALID_FILE_ATTRIBUTES)
            {
                printe( "ERROR: auto_delete_file failed\n");
                success = false;
            }
        }
    }

    return success;
}
