local ROOT_DIR = path.join(path.getdirectory(_SCRIPT), ".") .. "/"
local EXAMPLES_DIR = (ROOT_DIR .. "examples/")

solution "tiny_sln"
	platforms {
		"x32",
		"x64",
	}
	language "C++"
	configurations {
		"Debug",
		"Release",
	}

	flags {
		"NoMinimalRebuild",
		"Symbols",
		"StaticRuntime",
	}

	configuration "Release"
		optimize "full"

	configuration {}

	dofile "premake/tiny.lua"

function example_project(name)
	project ("example_" .. name)
		kind "ConsoleApp"

		files {
			EXAMPLES_DIR .. name .. "/**",
		}
		includedirs {
			ROOT_DIR .. "include/",
		}

		links {
			"tiny",
		}

		configuration "windows"
			links {
				"ws2_32",
				"Iphlpapi",
			}

		configuration {}
end

example_project("test")
example_project("twopeers")
example_project("audio_sin")
example_project("audio_micecho")
