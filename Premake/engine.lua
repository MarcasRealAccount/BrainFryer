libs            = libs            or {}
libs.brainfryer = libs.brainfryer or {
	name       = "",
	location   = ""
}

local brainfryer = libs.brainfryer

function brainfryer:setup()
	self.name     = common:projectName()
	self.location = common:projectLocation()

	kind("StaticLib")
	common:outDirs(true)

	includedirs({ self.location .. "/Inc/" })

	files({
		self.location .. "/Inc/**",
		self.location .. "/Src/**"
	})

	filter("system:windows")
		links({ "d3d12.lib", "dxgi.lib", "Dbghelp.lib" })
	filter({})

	pkgdeps({
		"glm@latest",
		"entt@[3.11,4.0)",
		"spdlog@[1.11,2.0)"
	})
end

function brainfryer:setupDep()
	links({ self.name })
	externalincludedirs({ self.location .. "/Inc/" })

	pkgdeps({
		"glm@latest",
		"entt@[3.11,4.0)",
		"spdlog@[1.11,2.0)"
	})
end