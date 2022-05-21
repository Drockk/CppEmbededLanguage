#include <cassert>
#include <iostream>
#include <string>
#include <chrono>

#include "nethost.h"

// Header files copied from https://github.com/dotnet/core-setup
#include "coreclr_delegates.h"
#include "hostfxr.h"

#include <fmt/core.h>


#ifdef WINDOWS
#include <Windows.h>

#define STR(s) L ## s
constexpr auto DIR_SEPARATOR = L'\\';

#else
#include <dlfcn.h>

#define STR(s) s
constexpr auto DIR_SEPARATOR = '/';
//#define MAX_PATH PATH_MAX

#endif

using string_t = std::basic_string<char_t>;

namespace
{
    // Globals to hold hostfxr exports
    hostfxr_initialize_for_runtime_config_fn initFptr;
    hostfxr_get_runtime_delegate_fn getDelegateFptr;
    hostfxr_close_fn closeFptr;

    // Forward declarations
    bool loadHostfxr();
    load_assembly_and_get_function_pointer_fn getDotnetLoadAssembly(const char_t* configPath);
}

int main(int argc, char* argv[])
{
    char_t hostPath[MAX_PATH];
#if WINDOWS
    [[maybe_unused]] const auto size = ::GetFullPathNameW(reinterpret_cast<wchar_t*>(argv[0]), sizeof hostPath / sizeof(char_t), hostPath, nullptr);
    assert(size != 0);
#else
    [[maybe_unused]] const auto resolved = realpath(argv[0], hostPath);
    assert(resolved != nullptr);
#endif

    [[maybe_unused]] auto n{ 0 };
    [[maybe_unused]] auto a{ 0 };
    [[maybe_unused]] auto b{ 0 };
    if (argc == 1) {
        std::cerr << "No arguments were passed." << std::endl;
        return 1;
    }
    if (argc == 2) { 
        try {
            n = std::stoi(argv[1]);
        } catch ([[maybe_unused]] std::invalid_argument const& ex) {
            std::cerr << fmt::format("Invalid number: {}\n", argv[1]);
        } catch ([[maybe_unused]] std::out_of_range const& ex) {
            std::cerr << fmt::format("Number out of range: {}\n", argv[1]);
        }
    }
    else if (argc == 3) {
        try {
            a = std::stoi(argv[1]);
            b = std::stoi(argv[2]);
        }
        catch ([[maybe_unused]] std::invalid_argument const& ex) {
            std::cerr << fmt::format("Invalid number: {} or {}.\n", argv[1], argv[2]);
        }
        catch ([[maybe_unused]] std::out_of_range const& ex) {
            std::cerr << fmt::format("Number out of range: {} or {}.\n", argv[1], argv[2]);
        }
    }

    string_t rootPath = hostPath;
    const auto pos = rootPath.find_last_of(DIR_SEPARATOR);
    assert(pos != string_t::npos);
    rootPath = rootPath.substr(0, pos + 1);

    //
    // STEP 1: Load HostFxr and get exported hosting functions
    //
    if (!loadHostfxr())
    {
        assert(false && "Failure: load_hostfxr()");
        return EXIT_FAILURE;
    }

    //
    // STEP 2: Initialize and start the .NET Core runtime
    //
    const string_t configPath = rootPath + STR("sumAndFraction.runtimeconfig.json");
    load_assembly_and_get_function_pointer_fn loadAssemblyAndGetFunctionPointer = getDotnetLoadAssembly(
        configPath.c_str());
    assert(loadAssemblyAndGetFunctionPointer != nullptr && "Failure: get_dotnet_load_assembly()");

    //
    // STEP 3: Load managed assembly and get function pointer to a managed method
    //
    const string_t dotnetlibPath = rootPath + STR("sumAndFraction.dll");
    const auto dotnetType = STR("sumAndFraction.BinaryTrees, sumAndFraction");
    //const auto dotnetTypeMethod = STR("Sum");

    //component_entry_point_fn sum = nullptr;
    //[[maybe_unused]] int rc = loadAssemblyAndGetFunctionPointer(
    //    dotnetlibPath.c_str(),
    //    dotnetType,
    //    dotnetTypeMethod,
    //    nullptr /*delegate_type_name*/,
    //    nullptr,
    //    reinterpret_cast<void**>(&sum));
    //assert(rc == 0 && hello != nullptr && "Failure: loadAssemblyAndGetFunctionPointer()");

    //
    // STEP 4: Run managed code
    //
    //struct SumArgs
    //{
    //    int a;
    //    int b;
    //};
    //SumArgs args
    //{
    //    a,
    //    b
    //};

    //const auto test = sum(&args, sizeof(args));
    //std::cout << test << std::endl;

    const auto dotnetTypeMethod = STR("BinaryTreePub");
    component_entry_point_fn fr = nullptr;
    [[maybe_unused]] const auto rc = loadAssemblyAndGetFunctionPointer(
        dotnetlibPath.c_str(),
        dotnetType,
        dotnetTypeMethod,
        nullptr /*delegate_type_name*/,
        nullptr,
        reinterpret_cast<void**>(&fr));
    assert(rc == 0 && fr != nullptr && "Failure: loadAssemblyAndGetFunctionPointer()");

    struct FactorialArgs
    {
        int n;
    };
    FactorialArgs factorialArgs;
    factorialArgs.n = n;

    const auto start = std::chrono::high_resolution_clock::now();
    fr(&factorialArgs, sizeof factorialArgs);
    const auto stop = std::chrono::high_resolution_clock::now();

    std::cout << fmt::format("Runtime: {}s\n", duration_cast<std::chrono::seconds>(stop - start).count());

    return EXIT_SUCCESS;
}

/********************************************************************************************
 * Function used to load and activate .NET Core
 ********************************************************************************************/

namespace
{
    // Forward declarations
    void* loadLibrary(const char_t*);
    void* getExport(void*, const char*);

#ifdef WINDOWS
    void* loadLibrary(const char_t* path)
    {
        const HMODULE h = ::LoadLibraryW(path);
        assert(h != nullptr);
        return h;
    }
    void* getExport(void* h, const char* name)
    {
        void* f = GetProcAddress(static_cast<HMODULE>(h), name);
        assert(f != nullptr);
        return f;
    }
#else
    void* loadLibrary(const char_t* path)
    {
        void* h = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
        assert(h != nullptr);
        return h;
    }
    void* getExport(void* h, const char* name)
    {
        void* f = dlsym(h, name);
        assert(f != nullptr);
        return f;
    }
#endif

    bool loadHostfxr()
    {
        char_t buffer[MAX_PATH];
        size_t bufferSize = sizeof buffer / sizeof(char_t);
        if (const int rc = get_hostfxr_path(buffer, &bufferSize, nullptr); rc != 0)
            return false;

        void* lib = loadLibrary(buffer);
        initFptr = static_cast<hostfxr_initialize_for_runtime_config_fn>(getExport(lib, "hostfxr_initialize_for_runtime_config"));
        getDelegateFptr = static_cast<hostfxr_get_runtime_delegate_fn>(getExport(lib, "hostfxr_get_runtime_delegate"));
        closeFptr = static_cast<hostfxr_close_fn>(getExport(lib, "hostfxr_close"));

        return (initFptr && getDelegateFptr && closeFptr);
    }

    load_assembly_and_get_function_pointer_fn getDotnetLoadAssembly(const char_t* configPath)
    {
        void* loadAssemblyAndGetFunctionPointer = nullptr;
        hostfxr_handle cxt = nullptr;
        int rc = initFptr(configPath, nullptr, &cxt);
        if (rc != 0 || cxt == nullptr)
        {
            std::cerr << "Init failed: " << std::hex << std::showbase << rc << std::endl;
            closeFptr(cxt);
            return nullptr;
        }

        rc = getDelegateFptr(cxt, hdt_load_assembly_and_get_function_pointer, &loadAssemblyAndGetFunctionPointer);
        if (rc != 0 || loadAssemblyAndGetFunctionPointer == nullptr)
            std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc << std::endl;

        closeFptr(cxt);
        return static_cast<load_assembly_and_get_function_pointer_fn>(loadAssemblyAndGetFunctionPointer);
    }
}
