
o3de_gem_setup("O3deFramework_Misc")

string(TOLOWER ${gem_name} gem_name_lowercase)
string(TOUPPER ${gem_name} gem_name_uppercase)

ly_add_external_target_path(${CMAKE_CURRENT_SOURCE_DIR}/3rdParty)

add_subdirectory(Code)
