-- premake5.lua
project "cSharpSum"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    conan_basic_setup()

    linkoptions
    {
        conan_exelinkflags
    }

    files
    {
        "src/**.hpp",
        "src/**.h",
        "src/**.cpp"
    }

    filter "system:windows"
        systemversion "latest"
        defines { "WINDOWS" }

        links
        {
            "nethost.lib"
        }

    filter "system:linux"
        systemversion "latest"
        defines { "LINUX" }

        libdirs { "/usr/share/dotnet/packs/Microsoft.NETCore.App.Host.linux-x64/6.0.5/runtimes/linux-x64/native/" }
        links {"nethost"}

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
