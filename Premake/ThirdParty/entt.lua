libs      = libs      or {}
libs.entt = libs.entt or {
	location = ""
}

local entt = libs.entt

function entt:setup()
	self.location = common:projectLocation()

	kind("Utility")

	includedirs({ self.location .. "/single_include/" })

	files({ self.location .. "/single_include/**" })
end

function entt:setupDep()
	externalincludedirs({ self.location .. "/single_include/" })
end