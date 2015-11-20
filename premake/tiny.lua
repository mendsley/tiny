local ROOT_DIR = path.join(path.getdirectory(_SCRIPT), "..") .. "/"

project "tiny"
	language "C++"
	kind "StaticLib"

	files {
		ROOT_DIR .. "src/**.h",
		ROOT_DIR .. "src/**.cpp",
		ROOT_DIR .. "src/voice/opus/celt/*",
		ROOT_DIR .. "src/voice/opus/silk/*",
		ROOT_DIR .. "src/voice/opus/silk/float/**",
		ROOT_DIR .. "src/voice/opus/src/**",
		ROOT_DIR .. "include/tiny/**",
	}

	includedirs {
		ROOT_DIR .. "include/",
		ROOT_DIR .. "src/",
		ROOT_DIR .. "src/voice/opus/include",
		ROOT_DIR .. "src/voice/opus/celt",
		ROOT_DIR .. "src/voice/opus/silk",
		ROOT_DIR .. "src/voice/opus/silk/float",
		ROOT_DIR .. "src/voice/opus/src",
		ROOT_DIR .. "src/voice/opus",

		"$(DXSDK_DIR)/include",
	}

	configuration "windows"
		defines {
			"USE_ALLOCA",
			"HAVE_CONFIG_H",
		}

	configuration {}
