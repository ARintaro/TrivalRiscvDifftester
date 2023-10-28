target("difftester")
    set_kind("binary")
    add_deps("elf_reader")
    add_includedirs("$(projectdir)/utils")
    add_files("*.cpp")
    