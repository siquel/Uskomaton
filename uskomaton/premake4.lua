solution("uskomaton")
	configurations { "Debug", "Release"}
		flags { "ExtraWarnings"  }

	configuration { "gmake" }
		buildoptions { "-std=c++11", "-Wno-write-strings" }

	configuration { "linux" }
		defines { "LINUX "}

	project "uskomaton"
		kind "ConsoleApp"
		language "C++"
		files { "include/**.hpp", "src/**.cpp"}
		includedirs { "include" }
		prebuildcommands { "cd src/perl && perl generate_header.pl" }
		postbuildcommands { "cp scripts -r bin/ ", "cp server.conf bin/server.conf.example" }
		targetdir "bin"		

		configuration "linux"
			links { "boost_system", "boost_thread", "boost_filesystem", "perl", "pthread"  }
			includedirs { "/usr/lib/perl5/core_perl/CORE/" }
			libdirs { "/usr/lib/perl5/core_perl/CORE" }
