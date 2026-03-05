
#pragma once

#include <O3deFramework/Misc/O3deFramework_MiscConfiguration.h>
#include <AzCore/Settings/SettingsRegistry.h>
#include <AzCore/std/optional.h>

namespace AZ
{
    namespace Data
    {
        template <class T>
        class Asset;
    }
}

namespace AzFramework
{
    class Spawnable;
}

namespace O3deFramework::SettingsRegistryAccessors
{
    constexpr AZStd::string_view LevelGameEntityPrefabPathname = "/O3deFramework/Misc/LevelGameEntityNetworkSpawnablePathname";

    O3DEFRAMEWORK_MISC_API AZStd::optional<AZ::SettingsRegistryInterface::FixedValueString> TryGetLevelGameEntityPrefabPathname();
    O3DEFRAMEWORK_MISC_API AZStd::optional<AZ::Data::Asset<AzFramework::Spawnable>> TryGetLevelGameEntityPrefabAsset();
}
