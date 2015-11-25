local ROOT_DIR = path.join(path.getdirectory(_SCRIPT), "..") .. "/"
local THIRD_PARTY_DIR = (ROOT_DIR .. "3rdparty/")

project "tiny"
	language "C++"
	kind "StaticLib"

	files {
		ROOT_DIR .. "src/**.h",
		ROOT_DIR .. "src/**.cpp",
		ROOT_DIR .. "include/tiny/**",
	}

	includedirs {
		ROOT_DIR .. "include/",
		ROOT_DIR .. "src/",

		THIRD_PARTY_DIR .. "opus/include/",

		"$(DXSDK_DIR)/include",
	}
