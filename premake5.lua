-- premake5.lua

include("conanbuildinfo.premake.lua")

workspace("CppEmbededLanguage")
    architecture "x86_64"

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

    group "Lua"
        include("luaSum")
        include("luaFactorial")
    group "CSharp"
        include("cSharpSum")
    group ""
