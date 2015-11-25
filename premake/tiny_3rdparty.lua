local THIRD_PARTY_DIR = path.join(path.join(path.getdirectory(_SCRIPT), "..") , "3rdparty") .. "/"

project "tiny_opus"
	kind "StaticLib"
	language "C++"

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

project "tiny_webrtc_base"
	kind "StaticLib"
	language "C++"

	flags {
		"NoExceptions",
	}

	files {
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_types.*",
		THIRD_PARTY_DIR .. "webrtc/webrtc/base/checks.*",
		THIRD_PARTY_DIR .. "webrtc/webrtc/base/platform_file*",
		THIRD_PARTY_DIR .. "webrtc/webrtc/base/platform_thread*",
		THIRD_PARTY_DIR .. "webrtc/webrtc/base/thread_checker**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/base/criticalsection.cc",
	}

	includedirs {
		THIRD_PARTY_DIR .. "webrtc/",
	}

	excludes {
		THIRD_PARTY_DIR .. "webrtc/webrtc/base/**_unittest.cc",
	}

	configuration "vs*"
		defines {
			"COMPILER_MSVC",
		}

	configuration "windows"
		defines {
			"WEBRTC_WIN",
		}

	configuration {}

project "tiny_webrtc_system_wrappers"
	kind "StaticLib"
	language "C++"

	files {
		THIRD_PARTY_DIR .. "webrtc/webrtc/system_wrappers/source/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/system_wrappers/include/**",
	}

	includedirs {
		THIRD_PARTY_DIR .. "webrtc/",
	}

	excludes {
		THIRD_PARTY_DIR .. "webrtc/webrtc/system_wrappers/**_unittest.cc",
		THIRD_PARTY_DIR .. "webrtc/webrtc/system_wrappers/**_unittest_disabled.cc",
		THIRD_PARTY_DIR .. "webrtc/webrtc/system_wrappers/source/logcat_trace_context.cc",
		THIRD_PARTY_DIR .. "webrtc/webrtc/system_wrappers/source/data_log.cc",
	}

	configuration "vs*"
		defines {
			"COMPILER_MSVC",
		}

	configuration "windows"
		defines {
			"WEBRTC_WIN",
			"WIN32_LEAN_AND_MEAN",
		}
		excludes {
			THIRD_PARTY_DIR .. "webrtc/webrtc/system_wrappers/**_posix.cc",
			THIRD_PARTY_DIR .. "webrtc/webrtc/system_wrappers/**_mac.cc",
			THIRD_PARTY_DIR .. "webrtc/webrtc/system_wrappers/**_android.c",
		}

	configuration {}
project "tiny_webrtc_audio_processing_c"
	kind "StaticLib"
	language "C"

	files {
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/aec/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/aecm/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/ns/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/utility/**",
	}

	includedirs {
		THIRD_PARTY_DIR .. "webrtc/",
	}

	excludes {
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/**.cc",
	}

	configuration "vs*"
		buildoptions {
			"/wd4244", -- warning C4244: '=' : conversion from 'T' to 'U', possible loss of data
			"/wd4127", -- warning C4127: conditional expression is constant
			"/wd4100", -- warning C4100: 'T' : unreferenced formal parameter
		}

	configuration "windows"
		excludes {
			THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/aec/*_mips.c",
			THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/aec/*_neon.c",
			THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/aecm/*_mips.c",
			THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/aecm/*_neon.c",
			THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/ns/*_mips.c",
			THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/ns/*_neon.c",
		}

	configuration {}

project "tiny_webrtc_audio_processing"
	kind "StaticLib"
	language "C++"

	files {
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/agc/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/beamformer/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/include/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/intelligibility/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/transient/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/vad/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/*.cc",
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/*.h",
	}

	excludes {
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/**_unittest.cc",
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/**_test.cc",
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/intelligibility/test/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_processing/transient/test/**",
	}

	includedirs {
		THIRD_PARTY_DIR .. "webrtc/",
	}

	configuration "vs*"
		buildoptions {
			"/wd4244", -- warning C4244: '=' : conversion from 'T' to 'U', possible loss of data
			"/wd4127", -- warning C4127: conditional expression is constant
			"/wd4100", -- warning C4100: 'T' : unreferenced formal parameter
			"/wd4351", -- warning C4351: new behavior: elements of array 'webrtc::TransientDetector::last_first_moment_' will be default initialized
		}
		defines {
			"_SCL_SECURE_NO_WARNINGS",
			"COMPILER_MSVC",
		}

	configuration "windows"
		defines {
			"WEBRTC_WIN",
			"WEBRTC_NS_FLOAT",
		}

	configuration {}

project "tiny_webrtc_common_audio"
	kind "StaticLib"
	language "C++"

	files {
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/include/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/resampler/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/vad/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/signal_processing/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/*.c",
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/*.cc",
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/*.h",
	}

	includedirs {
		THIRD_PARTY_DIR .. "webrtc/",
	}

	excludes {
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/**_unittest.cc",
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/**_neon.cc",
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/**_openmax.cc",
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/**_arm.S",
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/**_armv7.S",
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/**_mips.c",
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/**_neon.c",
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/wav_file.cc",
	}

	configuration "vs*"
		buildoptions {
			"/wd4244", -- warning C4244: '=' : conversion from 'T' to 'U', possible loss of data
			"/wd4100", -- warning C4100: 'T' : unreferenced formal parameter
		}
		defines {
			"_SCL_SECURE_NO_WARNINGS",
			"COMPILER_MSVC",
		}

	configuration "windows"
		defines {
			"WEBRTC_WIN",
		}

	configuration {}

project "tiny_webrtc_audio_coding_c"
	kind "StaticLib"
	language "C"

	files {
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_coding/codecs/isac/main/include/**",
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_coding/codecs/isac/main/source/**",
	}

	includedirs {
		THIRD_PARTY_DIR .. "webrtc/",
		THIRD_PARTY_DIR .. "webrtc/webrtc/common_audio/signal_processing/include/",
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_coding/codecs/isac/main/include/",
	}

	excludes {
		THIRD_PARTY_DIR .. "webrtc/webrtc/modules/audio_coding/**.cc",
	}
