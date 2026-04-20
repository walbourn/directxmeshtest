# GitHub Copilot Instructions for DirectXMesh Test Suite

These instructions define how GitHub Copilot should assist with the DirectXMesh test suite located in this repository ([Test Suite](https://github.com/walbourn/directxmeshtest)). They supplement the [parent project instructions](https://github.com/microsoft/DirectXMesh/blob/main/.github/copilot-instructions.md) with test-specific conventions.

## Context

- **Project**: Test suite for the DirectXMesh geometry processing library
- **Repository**: <https://github.com/walbourn/directxmeshtest>
- **Language**: C++17
- **Test Framework**: Custom lightweight framework (no third-party dependency)
- **Build System**: CMake / CTest (built as subdirectory of the main DirectXMesh repo)
- **Documentation**: <https://github.com/walbourn/directxmeshtest/wiki>
- **Parent Project**: <https://github.com/microsoft/DirectXMesh>

## Test Framework

The test suite uses a custom, minimal framework — **not** GoogleTest, Catch2, or any third-party library. Do not introduce external test frameworks.

### Test Registration

Each test executable has an entry point file (`directxtest.cpp` or `main.cpp`) with this structure:

```cpp
typedef bool (*TestFN)();

struct TestInfo
{
    const char *name;
    TestFN func;
};

extern bool Test01();
extern bool Test02();
// ...

TestInfo g_Tests[] =
{
    { "DescriptiveName", Test01 },
    { "AnotherTest", Test02 },
    // ...
};
```

Tests are run sequentially via `RunTests()`, which prints `PASS` or `FAIL` for each test and returns an overall pass/fail status. The entry point is `wmain()`.

**Per-executable differences:**
- `mesh/`, `vb/`, and `cp/` use `directxtest.h` which defines `print`/`printe` macros, and their `wmain()` calls `XMVerifyCPUSupport()` and `CoInitializeEx()`.
- `utilitiestest/` uses `main.cpp` with direct `printf` calls (no `directxtest.h`), no `XMVerifyCPUSupport`, no `CoInitializeEx`.
- `headertest/` is a compile-only smoke test with a trivial `wmain` that just returns 0.

### Test Function Pattern

Every test function follows this pattern:

```cpp
bool TestNN()
{
    bool success = true;

    // Test case block
    {
        HRESULT hr = SomeAPICall(...);
        if (FAILED(hr))
        {
            printe("ERROR: FunctionName variant failed (%08X)\n", static_cast<unsigned int>(hr));
            success = false;
        }
        else if (!ValidateResult(...))
        {
            printe("ERROR: FunctionName variant produced incorrect results\n");
            success = false;
        }
    }

    // More test case blocks...

    return success;
}
```

Key rules:
- Test functions return `bool` (`true` = pass, `false` = fail).
- In `mesh/`, `vb/`, and `cp/`, use `printe(...)` (macro for `printf`) for error messages and `print(...)` for informational output.
- In `utilitiestest/`, use `printf(...)` directly.
- Always prefix error messages with `"ERROR: "`.
- Never throw exceptions from test functions. Accumulate failures via the `success` flag.
- Use scoped blocks `{ }` to isolate each test case within a function.

## Naming Conventions

| Element | Convention | Example |
| --- | --- | --- |
| Test functions | `TestNN` (zero-padded number) | `Test01`, `Test28` |
| D3D12 variants (VB tests) | `TestNN_DX12` suffix | `Test01_DX12` |
| Static test data (shared headers) | `extern const __declspec(selectany)` with `g_` prefix | `g_cubeVerts`, `g_VBGuid` |
| Static test data (`.cpp` files) | `static const` or `const` in anonymous namespace with `s_` prefix | `s_cubeAdj` |
| Bad/invalid test data | `g_bad` or `s_bad` prefix | `g_badIndices16_I0` |
| Unused/degenerate data | `g_unused` or `g_degen` prefix | `g_unused16`, `g_degenCubeIndices16` |
| Test data enums | `IB_TEST_TYPE`, `VB_FILL_TYPE` | `IB_IDENTITY`, `VB_REVERSE` |

> The `_DX12` suffix convention for test functions applies to `vb/` tests. Other test areas (e.g., `utilitiestest/`) register D3D12 tests as regular numbered functions without a suffix.

## Test Executables

| Executable | CTest Name | Source Directory | Purpose |
| --- | --- | --- | --- |
| `headertest` | — | `headertest/` | Verifies public headers compile cleanly at `/Wall` |
| `utilitiestest` | `utilsTest` | `utilitiestest/` | Tests `WaveFrontReader.h` and `FlexibleVertexFormat.h` |
| `xtmesh` | `mesh` | `mesh/` | Core mesh operations (adjacency, normals, optimization, meshlets, etc.) |
| `xtvb` | `vertexBuffer` | `vb/` | VB reader/writer, input layout validation |
| `xtcp` | `contentProcess` | `cp/` | End-to-end content processing with real mesh files |
| `fuzzloaders` | — | `fuzzloaders/` | Fuzz testing harness for mesh file loaders |

## Shared Test Infrastructure (`Tests/common/`)

| Header | Purpose |
| --- | --- |
| `directxtest.h` | Base includes, `print`/`printe` macros, `MEDIA_PATH`/`TEMP_PATH` definitions. Used by `mesh/`, `vb/`, and `cp/` — **not** by `utilitiestest/` or `headertest/`. |
| `TestHelpers.h` | IB/VB creation helpers, remap/pointrep/meshlet validators, `CompareArray` with epsilon |
| `TestGeometry.h` | Static test geometry: cube, face-mapped cube, box, tetrahedron, bowtie, backface meshes (both 16-bit and 32-bit indices) |
| `ShapesGenerator.h` | Procedural shape generation (cube, sphere, cylinder, torus) templated on index type |
| `TestInputLayouts.h` | Pre-defined `D3D11_INPUT_ELEMENT_DESC` and `D3D12_INPUT_ELEMENT_DESC` arrays for common vertex formats |

For `mesh/`, `vb/`, and `cp/` test files, include `"directxtest.h"` first, followed by `"DirectXMesh.h"`, then any needed common headers.

## Test Data Patterns

### Static Constant Data (Preferred for Unit Tests)

Pre-defined arrays in anonymous namespaces (`.cpp` files) or with `extern const __declspec(selectany)` (shared headers):

```cpp
// In .cpp files (anonymous namespace)
namespace
{
    const uint32_t s_cubeAdj[3 * 12] =
    {
        1, 6, 8,
        // ...
    };
}

// In shared headers (TestGeometry.h, TestHelpers.h)
extern const __declspec(selectany) DirectX::XMFLOAT3 g_cubeVerts[8] = { ... };
```

### Generated Data

Use helpers from `TestHelpers.h`:

```cpp
auto ib = CreateIndexBuffer<uint16_t>(count, IB_IDENTITY);
auto vb = CreateVertexBuffer32(count, VB_IDENTITY);
```

### Procedural Shapes

Use `ShapesGenerator<T>` for complex geometry:

```cpp
std::vector<uint16_t> indices;
std::vector<ShapesGenerator<uint16_t>::Vertex> vertices;
ShapesGenerator<uint16_t>::CreateSphere(indices, vertices, 1.0f, 16, false);
```

### File-Based Data

For content processing tests, load from the `MEDIA_PATH` environment variable using `ExpandEnvironmentStringsW`:

```cpp
wchar_t szPath[MAX_PATH] = {};
DWORD ret = ExpandEnvironmentStringsW(MEDIA_PATH L"model._obj", szPath, MAX_PATH);
if (!ret || ret > MAX_PATH)
{
    printe("ERROR: ExpandEnvironmentStrings FAILED\n");
    return false;
}
```

File-based tests typically iterate over a test media array, loading each file and counting passes:

```cpp
size_t ncount = 0;
size_t npass = 0;
for (size_t index = 0; index < std::size(g_TestMedia); ++index)
{
    // ExpandEnvironmentStringsW, load, validate, ++ncount, ++npass
    print(".");
}
print("\n Processed %zu files, %zu passed\n", ncount, npass);
```

## Error Checking Patterns

### HRESULT Validation

```cpp
HRESULT hr = SomeFunction(...);
if (FAILED(hr))
{
    printe("ERROR: FunctionName(16) variant failed (%08X)\n", static_cast<unsigned int>(hr));
    success = false;
}
```

Always cast HRESULT to `static_cast<unsigned int>(hr)` in format strings using `%08X`.

### Expected HRESULT for Invalid Inputs

For invalid-input tests, check the **exact** expected HRESULT value, not just `FAILED(hr)`:

```cpp
hr = SomeFunction(nullptr, 0, ...);
if (hr != E_INVALIDARG)
{
    printe("ERROR: FunctionName expected E_INVALIDARG (%08X)\n", static_cast<unsigned int>(hr));
    success = false;
}
```

Common expected codes: `E_INVALIDARG`, `E_FAIL`, `HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW)`.

### Result Comparison

Use the validation helpers in `TestHelpers.h`:

```cpp
if (!CompareArray(actual, expected, count))   // XMFLOAT3 with epsilon
if (!IsValidPointReps(pointRep, nVerts))
if (!IsValidFaceRemap(indices, faceRemap, nFaces))
if (!IsValidVertexRemap(indices, nFaces, vertexRemap, nVerts))
if (!IsValidMeshlet(meshlet, nVerts, nPrims))
if (!IsValidMeshletTriangle(mtri, nVerts))
```

These are the preferred validation helpers. Use them rather than writing ad-hoc validation loops.

### Float Comparison

Use `g_MeshEpsilon` (≈1.19e-6) with `XMVector3NearEqual` for floating-point comparisons. Do not use exact equality for float results.

### Suppressing Warnings for Intentional Bad-Argument Tests

When testing intentional invalid arguments (null pointers, etc.), suppress MSVC static analysis warnings locally:

```cpp
#pragma warning(push)
#pragma warning(disable : 6385 6387)
    hr = SomeFunction(nullptr, 0, ...);
#pragma warning(pop)
```

This prevents code analysis false positives on intentionally invalid calls.

## D3D11 vs D3D12 Handling

- D3D11 tests are always built.
- D3D12 source files are included at build time when `BUILD_DX12` is enabled or on ARM64 (see `Tests/CMakeLists.txt`).
- In `vb/`, D3D12 test **functions** use the `_DX12` suffix (e.g., `Test01_DX12`) and are registered conditionally with `#if (_WIN32_WINNT >= 0x0A00)`.
- In `vb/`, D3D12 test **source files** use the `12` suffix (e.g., `inputdesc12.cpp`, `vb12.cpp`).
- In `utilitiestest/`, D3D12 tests are registered as regular numbered functions (e.g., `Test08` for `CreateInputLayoutFromFVF(D3D12)`) gated by `WINAPI_FAMILY` checks.

## Guidelines for Writing New Tests

1. **Always test both 16-bit and 32-bit index paths.** Most DirectXMesh functions have `uint16_t` and `uint32_t` overloads — test both.

2. **Test invalid inputs.** Verify that functions return the **exact** expected HRESULT (`E_INVALIDARG`, `E_FAIL`, etc.) for null pointers, zero counts, out-of-range indices, and degenerate geometry.

3. **Test edge cases.** Include degenerate triangles, bowtie configurations, backfacing triangles, unused faces (index = `-1`), and maximum-size meshes (near `UINT16_MAX` or `UINT32_MAX` vertices).

4. **Use reference data for deterministic validation.** Pre-compute expected results (normals, adjacency, remaps) and store them as static constant arrays for comparison.

5. **Prefer deterministic data, but shuffled data is acceptable for remap tests.** Most tests use static reference data. Some remap tests in `mesh/remap.cpp` use `std::shuffle` with `std::default_random_engine` to test ordering invariance — this is the only accepted pattern for non-deterministic data.

6. **Use `ShapesGenerator`** for tests that need non-trivial geometry (spheres, tori) to exercise algorithms more thoroughly than simple cubes.

7. **When adding a new `TestNN` function**, assign the next available number and register it in the `g_Tests[]` array in the corresponding entry point file.

8. **Group tests by API function.** Each `.cpp` file in `mesh/` typically covers one or two related API functions (e.g., `normals.cpp` → `ComputeNormals`, `meshlets.cpp` → `ComputeMeshlets`/`ComputeCullData`).

9. **Include `BUILD_BVT_ONLY` guards** for tests that require media files, so build-verification tests can run without external assets:
   ```cpp
   #ifndef BUILD_BVT_ONLY
       // Tests that require DIRECTXMESH_MEDIA_PATH
   #endif
   ```

10. **Memory management.** Use RAII (`std::unique_ptr`, `std::vector`) for all allocations — never use raw `new`/`delete`. The `mesh/`, `vb/`, and `cp/` test harnesses enable CRT memory leak detection via `_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)` and call `_CrtDumpMemoryLeaks()` after each test on MSVC.

## Building and Running Tests

The test suite must be built from the parent DirectXMesh project. It cannot be built standalone:

```bash
# Configure (from DirectXMesh root)
cmake --preset=x64-Debug -DBUILD_TESTING=ON -DBUILD_BVT=ON

# Build
cmake --build out/build/x64-Debug

# Run tests
ctest --test-dir out/build/x64-Debug --output-on-failure
```

For tests that use media files, set the `DIRECTXMESH_MEDIA_PATH` environment variable to point to the `Tests/media/` directory.

## File Header Convention

Test source files typically use a simplified header (no URL line), though shared headers in `common/` may include the project URL:

```cpp
//-------------------------------------------------------------------------------------
// filename.cpp
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------
```

## Code Style

Follow the same `.editorconfig` rules as the main DirectXMesh project: 4-space indentation, CRLF line endings, `latin1` charset. See the root `.editorconfig` for full details.
