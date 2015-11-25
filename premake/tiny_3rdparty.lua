local THIRD_PARTY_DIR = path.join(path.join(path.getdirectory(_SCRIPT), "..") , "3rdparty") .. "/"

project "tiny_opus"
	kind "StaticLib"
	language "C++"

	--warnings "default"

	files {
		THIRD_PARTY_DIR .. "opus/celt/*",
		THIRD_PARTY_DIR .. "opus/silk/*",
		THIRD_PARTY_DIR .. "opus/silk/float/**",
		THIRD_PARTY_DIR .. "opus/src/**",
	}

	includedirs {
		THIRD_PARTY_DIR .. "opus/include",
		THIRD_PARTY_DIR .. "opus/celt",
		THIRD_PARTY_DIR .. "opus/silk",
		THIRD_PARTY_DIR .. "opus/silk/float",
		THIRD_PARTY_DIR .. "opus/src",
		THIRD_PARTY_DIR .. "opus",
	}
	
	configuration "windows"
		defines {
			"USE_ALLOCA",
			"HAVE_CONFIG_H",
		}

	configuration "vs*"
		buildoptions {
			"/wd4244", -- warning C4244: '=' : conversion from 'T' to 'U', possible loss of data
			"/wd4127", -- warning C4127: conditional expression is constant
 			"/wd4310", -- warning C4310: cast truncates constant value
			"/wd4706", -- warning C4706: assignment within conditional expression
			"/wd4701", -- warning C4701: potentially uninitialized local variable 'T' used
			"/wd4703", -- warning C4703: potentially uninitialized local pointer variable 'pcm_transition' used
		}

	configuration {}
