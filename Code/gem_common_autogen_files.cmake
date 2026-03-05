
# It's our job to define this cmake variable which the `ly_include_cmake_file_list` function looks at after this file is done being processed.
set(FILES)

# @Christian TODO: [todo][cmake][auto-gen] List out the required multiplayer AutoGen files here if we want
# to make multiplayer auto-components. Last time I tried this, it was a bit tricky because (1) I'd have to make
# the system component call `RegisterMultiplayerComponents` and we'd have to decide on whether we want the editor
# target to also do that or not, and (2) I was getting compiler errors because it seems like it would try to
# start building the C++ before the AutoGen custom command was run for certain targets, but Idk exactly.
# Append required multiplayer gem files.
