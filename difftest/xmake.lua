target("difftester")
    set_kind("binary")
    add_deps("elf_reader")
    add_deps("spike")
    add_files("*.cpp")
    
    