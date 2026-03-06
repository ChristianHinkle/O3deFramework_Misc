
# It's our job to define this cmake variable which the `ly_include_cmake_file_list` function looks at after this file is done being processed.
set(FILES)

# Append required multiplayer gem files.
block(SCOPE_FOR VARIABLES PROPAGATE FILES)
    get_property(multiplayer_gem_root GLOBAL PROPERTY "@GEMROOT:Multiplayer@")

    list(APPEND FILES
        ${multiplayer_gem_root}/Code/Include/Multiplayer/AutoGen/AutoComponent_Common.jinja
        ${multiplayer_gem_root}/Code/Include/Multiplayer/AutoGen/AutoComponent_Header.jinja
        ${multiplayer_gem_root}/Code/Include/Multiplayer/AutoGen/AutoComponent_Source.jinja
        ${multiplayer_gem_root}/Code/Include/Multiplayer/AutoGen/AutoComponentTypes_Header.jinja
        ${multiplayer_gem_root}/Code/Include/Multiplayer/AutoGen/AutoComponentTypes_Source.jinja
    )
endblock()

# Append our AutoGen xml files.
list(APPEND FILES
    Source/AutoGen/PlayerEntityManagerComponent.AutoComponent.xml
)
