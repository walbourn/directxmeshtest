// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WaveFrontReader.h"

#include "directxtest.h"

namespace
{
    enum : uint32_t
    {
        FLAGS_NONE = 0x0,
        FLAGS_NO_NORMALS = 0x1,
        FLAGS_NO_TEXCOORDS = 0x2,
        FLAGS_POSONLY = (FLAGS_NO_NORMALS | FLAGS_NO_TEXCOORDS),
        FLAGS_32BITONLY = 0x8000,
    };

    struct TestMedia
    {
        uint32_t options;
        const wchar_t* fname;
        size_t nverts;
        size_t nfaces;
        size_t nmtls;
    };

    const TestMedia s_TestMedia[] =
    {
        { FLAGS_NO_TEXCOORDS, MEDIA_PATH L"cube._obj", 24, 12, 1 },
        { FLAGS_NO_TEXCOORDS, MEDIA_PATH L"teapot._obj", 530, 1024, 1 },
        { FLAGS_NONE, MEDIA_PATH L"cup._obj", 1340, 1880, 3 },

    #ifndef BUILD_BVT_ONLY
        { FLAGS_POSONLY, MEDIA_PATH L"diamond._obj", 6, 9, 1 },
        { FLAGS_POSONLY, MEDIA_PATH L"tetrahedron._obj", 4, 4, 1 },
        { FLAGS_POSONLY, MEDIA_PATH L"octahedron._obj", 6, 8, 1 },
        { FLAGS_POSONLY, MEDIA_PATH L"icosahedron._obj", 12, 20, 1 },
        { FLAGS_POSONLY, MEDIA_PATH L"dodecahedron._obj", 20, 36, 1 },
        { FLAGS_POSONLY, MEDIA_PATH L"shuttle._obj", 310, 616, 8 },
        { FLAGS_POSONLY, MEDIA_PATH L"airboat._obj", 5797, 11566, 8 },

        { FLAGS_NO_NORMALS, MEDIA_PATH L"SuperSimpleRunner._obj", 166, 100, 1 },
        { FLAGS_NO_NORMALS, MEDIA_PATH L"Head_Big_Ears._obj", 17575, 34560, 2 },

        { FLAGS_NONE, MEDIA_PATH L"player_ship_a._obj", 621, 593, 2 },

        { FLAGS_32BITONLY | FLAGS_NO_NORMALS, MEDIA_PATH L"FSEngineGeo._obj", 25116, 15074, 2 },
        { FLAGS_32BITONLY, MEDIA_PATH L"FSEngineGeoN._obj", 26589, 15074, 2 },
        { FLAGS_32BITONLY, MEDIA_PATH L"John40K._obj", 40289, 40000, 2 },

        { FLAGS_NONE, MEDIA_PATH "sphere.vbo", 561, 1056, 1 },
        { FLAGS_NONE, MEDIA_PATH "cylinder.vbo", 130, 126, 1 },
        { FLAGS_NONE, MEDIA_PATH "torus.vbo", 1089, 2178, 1 },
        { FLAGS_NONE, MEDIA_PATH "Head_Big_Ears.vbo", 17575, 34560, 1 },

        // TODO: Need files with the following extended mtls: Ke, map_Ks, norm, map_emissive, map_ORM
    #endif
    };
}

bool Test01()
{
    bool success = true;

    size_t ncount = 0;
    size_t npass = 0;

    for (size_t index = 0; index < std::size(s_TestMedia); ++index)
    {
        wchar_t szPath[MAX_PATH] = {};
        DWORD ret = ExpandEnvironmentStringsW(s_TestMedia[index].fname, szPath, MAX_PATH);
        if (!ret || ret > MAX_PATH)
        {
            printe("ERROR: ExpandEnvironmentStrings FAILED\n");
            return false;
        }

#ifdef _DEBUG
        OutputDebugString(szPath);
        OutputDebugStringA("\n");
#endif

        wchar_t ext[_MAX_EXT];
        _wsplitpath_s( szPath, nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT );

        bool pass = true;

        uint32_t flags = s_TestMedia[index].options;
        if ((flags & FLAGS_32BITONLY) == 0)
        {
            DX::WaveFrontReader<uint16_t> wfReader;

            HRESULT hr;
            if ( _wcsicmp( ext, L".vbo" ) == 0 )
            {
                hr = wfReader.LoadVBO(szPath);
            }
            else
            {
                 hr = wfReader.Load(szPath);
            }
            if (FAILED(hr))
            {
                printf("ERROR: WaveFront OBJ 16-bit load failed (%08X):\n%ls\n", static_cast<unsigned int>(hr), szPath);
                success = false;
                pass = false;
            }
            else
            {
                if (wfReader.vertices.size() != s_TestMedia[index].nverts
                    || wfReader.indices.size() != s_TestMedia[index].nfaces * 3
                    || (!wfReader.attributes.empty() && wfReader.attributes.size() != s_TestMedia[index].nfaces)
                    || wfReader.materials.size() != s_TestMedia[index].nmtls
                    || (wfReader.hasNormals != ((flags & FLAGS_NO_NORMALS) == 0))
                    || (wfReader.hasTexcoords != ((flags & FLAGS_NO_TEXCOORDS) == 0))
                    )
                {
                    printf("ERROR: WaveFront OBJ 16-bit test failed\n:%ls\n\tverts: %zu   inds: %zu (faces: %zu)   attr: %zu   mats: %zu%s%s\n",
                        szPath,
                        wfReader.vertices.size(),
                        wfReader.indices.size(),
                        wfReader.indices.size() / 3,
                        wfReader.attributes.size(),
                        wfReader.materials.size(),
                        wfReader.hasNormals ? " normals" : "",
                        wfReader.hasTexcoords ? " texcoords" : "");
                    success = false;
                    pass = false;
                }
            }
        }

        // 32-bit test
        {
            DX::WaveFrontReader<uint32_t> wfReader;

            HRESULT hr;
            if ( _wcsicmp( ext, L".vbo" ) == 0 )
            {
                hr = wfReader.LoadVBO(szPath);
            }
            else
            {
                hr = wfReader.Load(szPath);
            }

            if (FAILED(hr))
            {
                printf("ERROR: WaveFront OBJ 32-bit load failed (%08X):\n%ls\n", static_cast<unsigned int>(hr), szPath);
                success = false;
                pass = false;
            }
            else
            {
                if (wfReader.vertices.size() != s_TestMedia[index].nverts
                    || wfReader.indices.size() != s_TestMedia[index].nfaces * 3
                    || (!wfReader.attributes.empty() && wfReader.attributes.size() != s_TestMedia[index].nfaces)
                    || wfReader.materials.size() != s_TestMedia[index].nmtls
                    || (wfReader.hasNormals != ((flags & FLAGS_NO_NORMALS) == 0))
                    || (wfReader.hasTexcoords != ((flags & FLAGS_NO_TEXCOORDS) == 0))
                    )
                {
                    printf("ERROR: WaveFront OBJ 32-bit test failed\n:%ls\n\tverts: %zu   inds: %zu (faces: %zu)   attr: %zu   mats: %zu%s%s\n",
                        szPath,
                        wfReader.vertices.size(),
                        wfReader.indices.size(),
                        wfReader.indices.size() / 3,
                        wfReader.attributes.size(),
                        wfReader.materials.size(),
                        wfReader.hasNormals ? " normals" : "",
                        wfReader.hasTexcoords ? " texcoords" : "");
                    success = false;
                    pass = false;
                }
            }
        }

        if (pass)
            ++npass;

        print(".");

        ++ncount;
    }

    // invalid args
    {
        DX::WaveFrontReader<uint16_t> wfReader;

        #pragma warning(push)
        #pragma warning(disable:6385 6387)
        HRESULT hr = wfReader.Load(nullptr);
        if (hr != E_INVALIDARG)
        {
            printf("ERROR: WaveFront OBJ load should have failed with invalid arg (actual %08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = wfReader.Load(L"File-does-not-exist._obj");
        if (hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            printf("ERROR: WaveFront OBJ load should have failed with file not found (actual %08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = wfReader.LoadMTL(nullptr);
        if (hr != E_INVALIDARG)
        {
            printf("ERROR: WaveFront OBJ loadmtl should have failed with invalid arg (actual %08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = wfReader.LoadMTL(L"File-does-not-exist._obj");
        if (hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            printf("ERROR: WaveFront OBJ loadmtl should have failed with file not found (actual %08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = wfReader.LoadVBO(nullptr);
        if (hr != E_INVALIDARG)
        {
            printf("ERROR: WaveFront OBJ loadvbo should have failed with invalid arg (actual %08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }

        hr = wfReader.LoadVBO(L"File-does-not-exist.vbo");
        if (hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            printf("ERROR: WaveFront OBJ loadvbo should have failed with file not found (actual %08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        #pragma warning(pop)
    }

    print("\n%zu obj files tested, %zu obj files passed ", ncount, npass);

    return success;
}
