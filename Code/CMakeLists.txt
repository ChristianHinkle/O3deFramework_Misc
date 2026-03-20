
# Currently we are in the Code folder: ${CMAKE_CURRENT_LIST_DIR}
# Get the platform specific folder ${pal_dir} for the current folder: ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME}
# Note: o3de_pal_dir will take care of the details for us, as this may be a restricted platform
#       in which case it will see if that platform is present here or in the restricted folder.
#       i.e. It could here in our gem : Gems/${gem_name}/Code/Platform/<platorm_name>  or
#            <restricted_folder>/<platform_name>/Gems/${gem_name}/Code
o3de_pal_dir(pal_dir ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME} "${gem_restricted_path}" "${gem_path}" "${gem_parent_relative_path}")

# Now that we have the platform abstraction layer (PAL) folder for this folder, thats where we will find the
# traits for this platform. Traits for a platform are defines for things like whether or not something in this gem
# is supported by this platform.
include(${pal_dir}/PAL_${PAL_PLATFORM_NAME_LOWERCASE}.cmake)

# Check to see if building the Gem Modules are supported for the current platform
if(NOT PAL_TRAIT_${gem_name_uppercase}_SUPPORTED)
    return()
endif()

# Define common data used by all unified, server, and client versions of our ".API" target.

set(${gem_name}_BASE_API_FILES_CMAKE
    gem_base_api_files.cmake
    ${pal_dir}/gem_base_api_files.cmake
)

set(${gem_name}_BASE_API_SERVER_FILES_CMAKE
    gem_base_api_server_files.cmake
)

set(${gem_name}_BASE_API_CLIENT_FILES_CMAKE
    gem_base_api_client_files.cmake
)

# Define an API target with common properties for all unified, server, and client targets.
ly_add_target(
    NAME ${gem_name}.Common.API INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_BASE_API_FILES_CMAKE}
    TARGET_PROPERTIES
        O3DE_PRIVATE_TARGET TRUE
    INCLUDE_DIRECTORIES
        INTERFACE
            Include
    BUILD_DEPENDENCIES
        INTERFACE
            CppUtils::Core::Include
            CppUtils::Misc::Include
            CppUtils::StdReimpl::Include
            AZ::AzCore
            Gem::LmbrCentral.API
)

# Add our actual ".API" targets.

ly_add_target(
    NAME ${gem_name}.Unified.API INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_BASE_API_FILES_CMAKE}
        ${${gem_name}_BASE_API_SERVER_FILES_CMAKE}
        ${${gem_name}_BASE_API_CLIENT_FILES_CMAKE}
    BUILD_DEPENDENCIES
        INTERFACE
            Gem::${gem_name}.Common.API
            Gem::O3deUtils_Core.Unified.API
            Gem::O3deUtils_Misc.Unified.API
            Gem::Multiplayer.Unified
)

ly_add_target(
    NAME ${gem_name}.Servers.API INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_BASE_API_FILES_CMAKE}
        ${${gem_name}_BASE_API_SERVER_FILES_CMAKE}
    BUILD_DEPENDENCIES
        INTERFACE
            Gem::${gem_name}.Common.API
            Gem::O3deUtils_Core.Servers.API
            Gem::O3deUtils_Misc.Servers.API
            Gem::Multiplayer.Servers
)

ly_add_target(
    NAME ${gem_name}.Clients.API INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_BASE_API_FILES_CMAKE}
        ${${gem_name}_BASE_API_CLIENT_FILES_CMAKE}
    BUILD_DEPENDENCIES
        INTERFACE
            Gem::${gem_name}.Common.API
            Gem::O3deUtils_Core.Clients.API
            Gem::O3deUtils_Misc.Clients.API
            Gem::Multiplayer.Clients
)

set(${gem_name}_BASE_PRIVATE_FILES_CMAKE
    gem_base_private_files.cmake
    ${pal_dir}/gem_base_private_files.cmake
)

set(${gem_name}_BASE_PRIVATE_SERVER_FILES_CMAKE
    gem_base_private_server_files.cmake
)

set(${gem_name}_BASE_PRIVATE_CLIENT_FILES_CMAKE
    gem_base_private_client_files.cmake
)

set(${gem_name}_BASE_PRIVATE_TARGET_PROPERTIES
    O3DE_PRIVATE_TARGET TRUE
)

# Define a private target with common properties for all unified, server, and client targets.
ly_add_target(
    NAME ${gem_name}.Common.Private.Object INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_BASE_PRIVATE_FILES_CMAKE}
    COMPILE_DEFINITIONS
        INTERFACE
            ${gem_name_uppercase}_EXPORTS
    TARGET_PROPERTIES
        O3DE_PRIVATE_TARGET TRUE
        ${${gem_name}_BASE_PRIVATE_TARGET_PROPERTIES}
    INCLUDE_DIRECTORIES
        INTERFACE
            .
    BUILD_DEPENDENCIES
        INTERFACE
            CppUtils::Core::Static
            CppUtils::Misc::Static
            CppUtils::StdReimpl::Static
            AZ::AzCore
            AZ::AzFramework
            Legacy::CryCommon
)

# Add our ".Private.Object" targets. @Christian: TODO: [todo][techdebt][cmake][multiplayer] Support multiplayer compile definitions (`AZ_TRAIT_CLIENT` and `AZ_TRAIT_SERVER`) without needing to depend on the multiplayer gem?

ly_add_target(
    NAME ${gem_name}.Unified.Private.Object INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_BASE_PRIVATE_FILES_CMAKE}
        ${${gem_name}_BASE_PRIVATE_SERVER_FILES_CMAKE}
        ${${gem_name}_BASE_PRIVATE_CLIENT_FILES_CMAKE}
    TARGET_PROPERTIES
        ${${gem_name}_BASE_PRIVATE_TARGET_PROPERTIES}
    BUILD_DEPENDENCIES
        INTERFACE
            Gem::${gem_name}.Common.Private.Object
            Gem::${gem_name}.Unified.API
            Gem::O3deUtils_Core.Unified
            Gem::O3deUtils_Misc.Unified
            Gem::Multiplayer.Unified.Static
            Gem::LmbrCentral.Unified
)

ly_add_target(
    NAME ${gem_name}.Servers.Private.Object INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_BASE_PRIVATE_FILES_CMAKE}
        ${${gem_name}_BASE_PRIVATE_SERVER_FILES_CMAKE}
    TARGET_PROPERTIES
        ${${gem_name}_BASE_PRIVATE_TARGET_PROPERTIES}
    BUILD_DEPENDENCIES
        INTERFACE
            Gem::${gem_name}.Common.Private.Object
            Gem::${gem_name}.Servers.API
            Gem::O3deUtils_Core.Servers
            Gem::O3deUtils_Misc.Servers
            Gem::Multiplayer.Server.Static
            Gem::LmbrCentral.Servers
)

ly_add_target(
    NAME ${gem_name}.Clients.Private.Object INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_BASE_PRIVATE_FILES_CMAKE}
        ${${gem_name}_BASE_PRIVATE_CLIENT_FILES_CMAKE}
    TARGET_PROPERTIES
        ${${gem_name}_BASE_PRIVATE_TARGET_PROPERTIES}
    BUILD_DEPENDENCIES
        INTERFACE
            Gem::${gem_name}.Common.Private.Object
            Gem::${gem_name}.Clients.API
            Gem::O3deUtils_Core.Clients
            Gem::O3deUtils_Misc.Clients
            Gem::Multiplayer.Client.Static
            Gem::LmbrCentral.Clients
)

set(${gem_name}_BASE_FILES_CMAKE
    ${${gem_name}_BASE_API_FILES_CMAKE}
    ${${gem_name}_BASE_PRIVATE_FILES_CMAKE}
)

set(${gem_name}_BASE_SERVER_FILES_CMAKE
    ${${gem_name}_BASE_API_SERVER_FILES_CMAKE}
    ${${gem_name}_BASE_PRIVATE_SERVER_FILES_CMAKE}
)

set(${gem_name}_BASE_CLIENT_FILES_CMAKE
    ${${gem_name}_BASE_API_CLIENT_FILES_CMAKE}
    ${${gem_name}_BASE_PRIVATE_CLIENT_FILES_CMAKE}
)

set(${gem_name}_BASE_AUTOGEN_RULES
    *.AutoComponent.xml,AutoComponent_Header.jinja,$path/$fileprefix.AutoComponent.h
    *.AutoComponent.xml,AutoComponent_Source.jinja,$path/$fileprefix.AutoComponent.cpp
    *.AutoComponent.xml,AutoComponentTypes_Header.jinja,$path/AutoComponentTypes.h
    *.AutoComponent.xml,AutoComponentTypes_Source.jinja,$path/AutoComponentTypes.cpp
)

# Add our actual game gem targets. These depend on their corresponding ".Private.Object" and ".API" targets.

set(${gem_name}_GAME_FILES_CMAKE
    gem_game_shared_files.cmake
    ${pal_dir}/gem_game_shared_files.cmake
    gem_base_autogen_files.cmake
)

ly_add_target(
    NAME ${gem_name}.Unified ${PAL_TRAIT_MONOLITHIC_DRIVEN_LIBRARY_TYPE}
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_GAME_FILES_CMAKE}
        ${${gem_name}_BASE_FILES_CMAKE}
        ${${gem_name}_BASE_SERVER_FILES_CMAKE}
        ${${gem_name}_BASE_CLIENT_FILES_CMAKE}
    BUILD_DEPENDENCIES
        PUBLIC
            Gem::${gem_name}.Unified.API
        PRIVATE
            Gem::${gem_name}.Unified.Private.Object
    AUTOGEN_RULES
        ${${gem_name}_BASE_AUTOGEN_RULES}
)

ly_add_target(
    NAME ${gem_name}.Servers ${PAL_TRAIT_MONOLITHIC_DRIVEN_LIBRARY_TYPE}
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_GAME_FILES_CMAKE}
        ${${gem_name}_BASE_FILES_CMAKE}
        ${${gem_name}_BASE_SERVER_FILES_CMAKE}
    BUILD_DEPENDENCIES
        PUBLIC
            Gem::${gem_name}.Servers.API
        PRIVATE
            Gem::${gem_name}.Servers.Private.Object
    AUTOGEN_RULES
        ${${gem_name}_BASE_AUTOGEN_RULES}
)

ly_add_target(
    NAME ${gem_name}.Clients ${PAL_TRAIT_MONOLITHIC_DRIVEN_LIBRARY_TYPE}
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_GAME_FILES_CMAKE}
        ${${gem_name}_BASE_FILES_CMAKE}
        ${${gem_name}_BASE_CLIENT_FILES_CMAKE}
    BUILD_DEPENDENCIES
        PUBLIC
            Gem::${gem_name}.Clients.API
        PRIVATE
            Gem::${gem_name}.Clients.Private.Object
    AUTOGEN_RULES
        ${${gem_name}_BASE_AUTOGEN_RULES}
)

# Unset these variables, as they are no longer needed.
unset(${gem_name}_BASE_API_FILES_CMAKE)
unset(${gem_name}_BASE_API_SERVER_FILES_CMAKE)
unset(${gem_name}_BASE_API_CLIENT_FILES_CMAKE)
unset(${gem_name}_BASE_PRIVATE_FILES_CMAKE)
unset(${gem_name}_BASE_PRIVATE_SERVER_FILES_CMAKE)
unset(${gem_name}_BASE_PRIVATE_CLIENT_FILES_CMAKE)
unset(${gem_name}_BASE_PRIVATE_TARGET_PROPERTIES)
unset(${gem_name}_BASE_FILES_CMAKE)
unset(${gem_name}_BASE_SERVER_FILES_CMAKE)
unset(${gem_name}_BASE_CLIENT_FILES_CMAKE)
unset(${gem_name}_BASE_AUTOGEN_RULES)
unset(${gem_name}_GAME_FILES_CMAKE)

# Include the gem name into the Client Module source file
# for use with the AZ_DECLARE_MODULE_CLASS macro
# This is to allow renaming of the gem to also cause
# the CreateModuleClass_Gem_<gem-name> function which
# is used to bootstrap the gem in monolithic builds to link to the new gem name
ly_add_source_properties(
SOURCES
    Source/${gem_name}Module.cpp
PROPERTY COMPILE_DEFINITIONS
    VALUES
        O3DE_GEM_NAME=${gem_name}
        O3DE_GEM_VERSION=${gem_version})

# Add in CMake dependencies for each gem dependency listed in this gem's gem.json file
# for the Clients, Servers, Unified gem variants
o3de_add_variant_dependencies_for_gem_dependencies(GEM_NAME ${gem_name} VARIANTS Clients Servers Unified)

# @Christian: TODO: [todo][multiplayer][autogen][techdebt] We should make it so the editor targets also do autogen properly. We should make it behave
# like the unified target as much as possible. Currently, testing multiplayer in the editor with `editorsv_clientserver` disabled results in
# the server launcher crashing for some reason. It happened once I migrated the first multiplayer component from the game to this gem.

# If we are on a host platform, we want to add the host tools targets like the ${gem_name}.Tools MODULE target
if(PAL_TRAIT_BUILD_HOST_TOOLS)
    # The ${gem_name}.Tools.API target can be used by other gems that want to interact with the ${gem_name}.Tools module
    ly_add_target(
        NAME ${gem_name}.Tools.API INTERFACE
        NAMESPACE Gem
        FILES_CMAKE
            gem_editor_api_files.cmake
            ${pal_dir}/gem_editor_api_files.cmake
        INCLUDE_DIRECTORIES
            INTERFACE
                Include
        BUILD_DEPENDENCIES
            INTERFACE
                AZ::AzToolsFramework
    )

    # The ${gem_name}.Tools.Private.Object target is an internal target
    # which is only to be used by this gems CMakeLists.txt and any subdirectories
    # Other gems should not use this target
    ly_add_target(
        NAME ${gem_name}.Tools.Private.Object INTERFACE
        NAMESPACE Gem
        FILES_CMAKE
            gem_editor_private_files.cmake
        TARGET_PROPERTIES
            O3DE_PRIVATE_TARGET TRUE
        INCLUDE_DIRECTORIES
            INTERFACE
                .
        BUILD_DEPENDENCIES
            INTERFACE
                AZ::AzToolsFramework
                Gem::${gem_name}.Unified
    )

    ly_add_target(
        NAME ${gem_name}.Tools GEM_MODULE
        NAMESPACE Gem
        AUTOMOC
        FILES_CMAKE
            gem_editor_api_files.cmake
            gem_editor_private_files.cmake
            gem_editor_shared_files.cmake
        BUILD_DEPENDENCIES
            PUBLIC
                Gem::${gem_name}.Tools.API
            PRIVATE
                Gem::${gem_name}.Tools.Private.Object
    )

    # Include the gem name into the Editor Module source file
    # for use with the AZ_DECLARE_MODULE_CLASS macro
    # This is to allow renaming of the gem to also cause
    # the CreateModuleClass_Gem_<gem-name> function which
    # is used to bootstrap the gem in monolithic builds to link to the new gem name
    ly_add_source_properties(
    SOURCES
        Source/Tools/${gem_name}EditorModule.cpp
    PROPERTY COMPILE_DEFINITIONS
        VALUES
            O3DE_GEM_NAME=${gem_name}
            O3DE_GEM_VERSION=${gem_version})

    ly_create_alias(NAME ${gem_name}.Builders NAMESPACE Gem TARGETS Gem::${gem_name}.Tools)
    ly_create_alias(NAME ${gem_name}.Builders.API NAMESPACE Gem TARGETS Gem::${gem_name}.Tools.API)

    # Add in CMake dependencies for each gem dependency listed in this gem's gem.json file
    # for the Tools and Builders gem variants
    o3de_add_variant_dependencies_for_gem_dependencies(GEM_NAME ${gem_name} VARIANTS Tools Builders)
endif()

################################################################################
# Tests
################################################################################
# See if globally, tests are supported
if(PAL_TRAIT_BUILD_TESTS_SUPPORTED)
    # We globally support tests, see if we support tests on this platform for ${gem_name}.Tests
    if(PAL_TRAIT_${gem_name_uppercase}_TEST_SUPPORTED)
        # We support ${gem_name}.Tests on this platform, add dependency on the ".Private.Object" target
        ly_add_target(
            NAME ${gem_name}.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
            NAMESPACE Gem
            FILES_CMAKE
                gem_game_tests_files.cmake
            INCLUDE_DIRECTORIES
                PRIVATE
                    Tests
                    .
                    Include
            BUILD_DEPENDENCIES
                PRIVATE
                    AZ::AzTest
                    AZ::AzFramework
                    Gem::${gem_name}.Unified.Private.Object
        )

        # Add ${gem_name}.Tests to googletest
        ly_add_googletest(
            NAME Gem::${gem_name}.Tests
        )
    endif()

    # If we are a host platform we want to add tools test like editor tests here
    if(PAL_TRAIT_BUILD_HOST_TOOLS)
        # We are a host platform, see if Editor tests are supported on this platform
        if(PAL_TRAIT_${gem_name_uppercase}_EDITOR_TEST_SUPPORTED)
            # We support ${gem_name}.Tools.Tests on this platform, add ${gem_name}.Tools.Tests target which depends on
            # private ${gem_name}.Tools.Private.Object target
            ly_add_target(
                NAME ${gem_name}.Tools.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
                NAMESPACE Gem
                FILES_CMAKE
                    gem_editor_tests_files.cmake
                INCLUDE_DIRECTORIES
                    PRIVATE
                        Tests
                        .
                        Include
                BUILD_DEPENDENCIES
                    PRIVATE
                        AZ::AzTest
                        Gem::${gem_name}.Tools.Private.Object
            )

            # Add ${gem_name}.Tools.Tests to googletest
            ly_add_googletest(
                NAME Gem::${gem_name}.Tools.Tests
            )
        endif()
    endif()
endif()
