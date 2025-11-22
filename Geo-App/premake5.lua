workspace "GeoApp"
    architecture "x64"
    
    configurations {
        "Debug",
        "Release"
    }

    outdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    IncludeDir = {}
    IncludeDir["GLEW"]  = "vendor/glew/include"
    IncludeDir["GLFW"]  = "vendor/glfw/include"
    IncludeDir["STB"]   = "vendor/stb"
    IncludeDir["GLM"]   = "vendor/glm"
    IncludeDir["ASSIMP"] = "vendor/assimp/include"

project "GeoApp"
    location "GeoApp"
    kind "ConsoleApp"
    language "C++"

    targetdir("bin/" .. outdir .. "/%{prj.name}")
    objdir("bin-int/" .. outdir .. "/%{prj.name}")

 files {
		"%{prj.name}/**.h",
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.c",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",  
		"%{prj.name}/vendor/stb/**.h",
		"%{prj.name}/vendor/stb/**.c",
		"%{prj.name}/vendor/imgui/**.h",
		"%{prj.name}/vendor/imgui/**.cpp",
		"%{prj.name}/vendor/imgui/**.inl",


		"%{prj.name}/src/**.vert",
		"%{prj.name}/src/**.geom",
		"%{prj.name}/src/**.frag"
	}	

    includedirs {
        "%{prj.name}/src",
		"%{IncludeDir.GLEW}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.STB}",
		"%{IncludeDir.GLM}",
		"%{IncludeDir.ASSIMP}",
	}

    libdirs {
        "vendor/glew/lib/Release/x64",
		"vendor/glfw/lib-vc2022",
        "vendor/assimp/lib"
    }

    links {
        "glew32s",
        "glfw3",
        "opengl32.lib",
        "assimp-vc143-mtd.lib",
        "Ws2_32.lib",
    }

    filter "system:windows"
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"

        defines {
            "GLEW_STATIC",
            "GLM_FORCE_RADIANS",
            "_CRT_SECURE_NO_WARNINGS",
            "_USE_MATCH_DEFINES"
        }

        filter "configurations:Debug"
            defines "CG_DEBUG"
            buildoptions "/MDd"
            symbols "On"

        filter "configurations:Release"
            buildoptions "/MD"
            optimize "On"