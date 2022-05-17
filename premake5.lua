-- premake5.lua

include("conanbuildinfo.premake.lua")

workspace("CppEmbededLanguage")
    architecture "x86_64"

    conan_basic_setup()

    configurations
    {
        "Debug",
        "Release"
    }

    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    include("luaSum")
    include("luaFactorial")
