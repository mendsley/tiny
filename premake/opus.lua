local THIRD_PARTY_DIR = path.join(path.join(path.getdirectory(_SCRIPT), "..") , "3rdparty") .. "/"

project "opus"
	kind "StaticLib"
	language "C"

	exceptionhandling "Off"

	files {
		-- opus
		THIRD_PARTY_DIR .. "opus/celt/*",
		THIRD_PARTY_DIR .. "opus/silk/*",
		THIRD_PARTY_DIR .. "opus/silk/float/**",
		THIRD_PARTY_DIR .. "opus/src/**",
	}

	includedirs {
		-- opus
		THIRD_PARTY_DIR .. "opus/include/",
		THIRD_PARTY_DIR .. "opus/celt/",
		THIRD_PARTY_DIR .. "opus/silk/",
		THIRD_PARTY_DIR .. "opus/silk/float/",
		THIRD_PARTY_DIR .. "opus/src/",
		THIRD_PARTY_DIR .. "contrib/opus/",
	}

	filter "system:windows"
		defines {
			-- opus
			"USE_ALLOCA",
			"HAVE_CONFIG_H",
		}

		excludes {
			-- opus
			THIRD_PARTY_DIR .. "opus/src/mlp_train.c",
			THIRD_PARTY_DIR .. "opus/src/opus_compare.c",
			THIRD_PARTY_DIR .. "opus/src/*_demo.c",
			THIRD_PARTY_DIR .. "opus/celt/*_demo.c",
		}

	filter "action:vs*"
		buildoptions {
			--"/wd4100", -- warning C4100: 'T' : unreferenced formal parameter
			--"/wd4127", -- warning C4127: conditional expression is constant
			"/wd4244", -- warning C4244: '=' : conversion from 'T' to 'U', possible loss of data
			--"/wd4351", -- warning C4351: new behavior: elements of array 'webrtc::TransientDetector::last_first_moment_' will be default initialized
			--"/wd4457", -- warbing C4457: declaration of 'identifier' hides function parameter
			--"/wd4701", -- warning C4701: potentially uninitialized local variable 'T' used
			--"/wd4703", -- warning C4703: potentially uninitialized local pointer variable 'pcm_transition' used
			--"/wd4706", -- warning C4706: assignment within conditional expression
			"/wd4310", -- warning C4310: cast truncates constant value
		}

	filter {}

