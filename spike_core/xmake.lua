
spike_dir = "$(projectdir)/spike/"
spike_build_dir = "$(projectdir)/spike/build/"

target("spike")
	set_kind("phony")

	before_build(function (target)
		if os.exists("$(projectdir)/spike/build/libriscv.a") then
			return
		end 
		print("building spike...")
		print("It takes time. Get up and drink some water. :)")
		print("If you want to see what happens, try 'xmake build -v'")
		
		os.cd("$(projectdir)/spike/")
		os.mkdir("build")
		os.cd("build")
		os.vrun("../configure --prefix=$RISCV")
		os.vrun("make -j$(nproc)")
    end)

	add_includedirs(spike_dir .. "riscv", {public = true})
	add_includedirs(spike_dir .. "build", {public = true})
	add_includedirs(spike_dir .. "fesvr", {public = true})
	add_includedirs(spike_dir .. "softfloat", {public = true})
	add_includedirs(spike_dir, {public = true})

	add_syslinks("pthread", "dl", "boost_regex", "boost_system", "pthread", "boost_system", "boost_regex", {public = true})
	
	if link_kind == "shared" then 
		add_linkdirs(spike_build_dir, {public = true})
		add_links("spike_main", "riscv", "disasm", "softfloat", "fesvr", {public = true})
		add_rpathdirs(spike_build_dir, {public = true})
	else 
		add_links(spike_build_dir .. "libspike_main.a", {public = true})
		add_links(spike_build_dir .. "libriscv.a", {public = true})
		add_links(spike_build_dir .. "libdisasm.a", {public = true})
		add_links(spike_build_dir .. "libsoftfloat.a", {public = true})
		add_links(spike_build_dir .. "libfesvr.a", {public = true})
		add_links(spike_build_dir .. "libfdt.a", {public = true})
	end 


-- target("spike_core")
-- 	add_deps("spike")
-- 	set_kind("static")
-- 	add_files("*.cpp")
-- 	remove_files("spike_main.cpp")
-- 	add_cxxflags("-fPIC")
	
-- target("spike_main")
-- 	add_deps("spike_core")
-- 	add_deps("elf_reader")
-- 	set_kind("shared")
-- 	add_files("spike_main.cpp")	
-- 	add_cxxflags("-fvisibility=hidden")


target("spike_main")
	add_deps("elf_reader")
	add_deps("spike")
	add_files("*.cpp")
	set_kind("shared")
	add_cxxflags("-fvisibility=hidden")
	