-- Input hook library

project "Lib-Input"
	kind "StaticLib"
	language "C"
	files { "**.h", "**.c", "premake4.lua" }
	vpaths { [""] = { "../Libraries/Input" } }
	includedirs { ".", ".." }
	location ( "../../Projects/" .. os.get() .. "/" .. _ACTION )
	
	-- Linux specific stuff
	configuration "linux"
		targetextension ".a"
		buildoptions { "-fms-extensions" } -- Unnamed struct/union fields within structs/unions
		configuration "Debug" targetname "libinputd"
		configuration "Release" targetname "libinput"
	
	-- Windows specific stuff
	configuration "windows"
		targetextension ".lib"
		buildoptions { "/wd4201 /wd4206" } -- C4201: nameless struct/union, C4206: translation unit is empty
		configuration "Debug" targetname "inputd"
		configuration "Release" targetname "input"
