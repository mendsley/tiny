local ROOT_DIR = path.join(path.getdirectory(_SCRIPT), "..") .. "/"

project "tiny"
	language "C++"
	kind "StaticLib"

	files {
		ROOT_DIR .. "src/**",
		ROOT_DIR .. "include/tiny/**",
	}

	includedirs {
		ROOT_DIR .. "include/",
		ROOT_DIR .. "src/",

		"$(DXSDK_DIR)/include",
	}
