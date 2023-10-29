

target("elf_reader")
	add_packages("elfio", {public = true})
	set_kind(link_kind)
	add_files("elf_reader.cpp")

	if link_kind == "static" then 
		add_cxxflags("-fPIC")
	end
