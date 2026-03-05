
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

set(${gem_name}_COMMON_API_FILES_CMAKE
    gem_common_api_files.cmake
    ${pal_dir}/gem_common_api_files.cmake
)

ly_add_target(
    NAME ${gem_name}.Common.API INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_COMMON_API_FILES_CMAKE}
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
)

# Add our actual ".API" targets.

ly_add_target(
    NAME ${gem_name}.Unified.API INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_COMMON_API_FILES_CMAKE}
        gem_client_api_files.cmake
    BUILD_DEPENDENCIES
        INTERFACE
            Gem::${gem_name}.Common.API
            Gem::O3deUtils_Core.Unified.API
            Gem::Multiplayer.Unified
            Gem::LyShine.Unified.API
)

ly_add_target(
    NAME ${gem_name}.Servers.API INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_COMMON_API_FILES_CMAKE}
    BUILD_DEPENDENCIES
        INTERFACE
            Gem::${gem_name}.Common.API
            Gem::O3deUtils_Core.Servers.API
            Gem::Multiplayer.Servers
)

ly_add_target(
    NAME ${gem_name}.Clients.API INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_COMMON_API_FILES_CMAKE}
        gem_client_api_files.cmake
    BUILD_DEPENDENCIES
        INTERFACE
            Gem::${gem_name}.Common.API
            Gem::O3deUtils_Core.Clients.API
            Gem::Multiplayer.Clients
            Gem::LyShine.Clients.API
)

# Define common data used by all unified, server, and client versions of our ".Private.Object" target.

set(${gem_name}_COMMON_PRIVATE_OBJECT_FILES_CMAKE
    gem_common_private_files.cmake
    ${pal_dir}/gem_common_private_files.cmake
)

set(${gem_name}_COMMON_PRIVATE_OBJECT_TARGET_PROPERTIES
    O3DE_PRIVATE_TARGET TRUE
)

ly_add_target(
    NAME ${gem_name}.Common.Private.Object INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_COMMON_PRIVATE_OBJECT_FILES_CMAKE}
    COMPILE_DEFINITIONS
        INTERFACE
            ${gem_name_uppercase}_EXPORTS
    TARGET_PROPERTIES
        O3DE_PRIVATE_TARGET TRUE
        ${${gem_name}_COMMON_PRIVATE_OBJECT_TARGET_PROPERTIES}
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
        ${${gem_name}_COMMON_PRIVATE_OBJECT_FILES_CMAKE}
        gem_client_private_files.cmake
    TARGET_PROPERTIES
        ${${gem_name}_COMMON_PRIVATE_OBJECT_TARGET_PROPERTIES}
    BUILD_DEPENDENCIES
        INTERFACE
            Gem::${gem_name}.Common.Private.Object
            Gem::${gem_name}.Unified.API
            Gem::O3deUtils_Core.Unified
            Gem::Multiplayer.Unified.Static
            Gem::LyShine.Unified
)

ly_add_target(
    NAME ${gem_name}.Servers.Private.Object INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_COMMON_PRIVATE_OBJECT_FILES_CMAKE}
    TARGET_PROPERTIES
        ${${gem_name}_COMMON_PRIVATE_OBJECT_TARGET_PROPERTIES}
    BUILD_DEPENDENCIES
        INTERFACE
            Gem::${gem_name}.Common.Private.Object
            Gem::${gem_name}.Servers.API
            Gem::O3deUtils_Core.Servers
            Gem::Multiplayer.Server.Static
)

ly_add_target(
    NAME ${gem_name}.Clients.Private.Object INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_COMMON_PRIVATE_OBJECT_FILES_CMAKE}
        gem_client_private_files.cmake
    TARGET_PROPERTIES
        ${${gem_name}_COMMON_PRIVATE_OBJECT_TARGET_PROPERTIES}
    BUILD_DEPENDENCIES
        INTERFACE
            Gem::${gem_name}.Common.Private.Object
            Gem::${gem_name}.Clients.API
            Gem::O3deUtils_Core.Clients
            Gem::Multiplayer.Client.Static
            Gem::LyShine.Clients
)

# Unset these variables, as they are no longer needed.
unset(${gem_name}_COMMON_PRIVATE_OBJECT_TARGET_PROPERTIES)

# Define common data used by all unified, server, and client versions of our main gem target.

set(${gem_name}_COMMON_FILES_CMAKE
    ${${gem_name}_COMMON_API_FILES_CMAKE}
    ${${gem_name}_COMMON_PRIVATE_OBJECT_FILES_CMAKE}
    gem_common_shared_files.cmake
    ${pal_dir}/gem_common_shared_files.cmake
    gem_common_autogen_files.cmake
)

set(${gem_name}_COMMON_AUTOGEN_RULES
    # @Christian TODO: [todo][cmake][auto-gen] List out the required multiplayer AutoGen rules here if we want
    # to make multiplayer auto-components. Last time I tried this, it was a bit tricky because (1) I'd have to make
    # the system component call `RegisterMultiplayerComponents` and we'd have to decide on whether we want the editor
    # target to also do that or not, and (2) I was getting compiler errors because it seems like it would try to
    # start building the C++ before the AutoGen custom command was run for certain targets, but Idk exactly.
)

# Add our main gem targets. These depend on their corresponding ".Private.Object" and ".API" targets.

ly_add_target(
    NAME ${gem_name}.Unified ${PAL_TRAIT_MONOLITHIC_DRIVEN_LIBRARY_TYPE}
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_COMMON_FILES_CMAKE}
        gem_client_api_files.cmake
        gem_client_private_files.cmake
    BUILD_DEPENDENCIES
        PUBLIC
            Gem::${gem_name}.Unified.API
        PRIVATE
            Gem::${gem_name}.Unified.Private.Object
    AUTOGEN_RULES
        ${${gem_name}_COMMON_AUTOGEN_RULES}
)

ly_add_target(
    NAME ${gem_name}.Servers ${PAL_TRAIT_MONOLITHIC_DRIVEN_LIBRARY_TYPE}
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_COMMON_FILES_CMAKE}
    BUILD_DEPENDENCIES
        PUBLIC
            Gem::${gem_name}.Servers.API
        PRIVATE
            Gem::${gem_name}.Servers.Private.Object
    AUTOGEN_RULES
        ${${gem_name}_COMMON_AUTOGEN_RULES}
)

ly_add_target(
    NAME ${gem_name}.Clients ${PAL_TRAIT_MONOLITHIC_DRIVEN_LIBRARY_TYPE}
    NAMESPACE Gem
    FILES_CMAKE
        ${${gem_name}_COMMON_FILES_CMAKE}
        gem_client_api_files.cmake
        gem_client_private_files.cmake
    BUILD_DEPENDENCIES
        PUBLIC
            Gem::${gem_name}.Clients.API
        PRIVATE
            Gem::${gem_name}.Clients.Private.Object
    AUTOGEN_RULES
        ${${gem_name}_COMMON_AUTOGEN_RULES}
)

# Unset these variables, as they are no longer needed.
unset(${gem_name}_COMMON_API_FILES_CMAKE)
unset(${gem_name}_COMMON_PRIVATE_OBJECT_FILES_CMAKE)
unset(${gem_name}_COMMON_FILES_CMAKE)
unset(${gem_name}_COMMON_AUTOGEN_RULES)

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

# If we are on a host platform, we want to add the host tools targets like the ${gem_name}.Tools MODULE target
if(PAL_TRAIT_BUILD_HOST_TOOLS)
    # The ${gem_name}.Tools.API target can be used by other gems that want to interact with the ${gem_name}.Tools module
    ly_add_target(
        NAME ${gem_name}.Tools.API INTERFACE
        NAMESPACE Gem
        FILES_CMAKE
            gem_tools_api_files.cmake
            ${pal_dir}/gem_tools_api_files.cmake
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
            gem_tools_private_files.cmake
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
            gem_tools_api_files.cmake
            gem_tools_private_files.cmake
            gem_tools_shared_files.cmake
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
                gem_tests_files.cmake
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
                    gem_tools_tests_files.cmake
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
