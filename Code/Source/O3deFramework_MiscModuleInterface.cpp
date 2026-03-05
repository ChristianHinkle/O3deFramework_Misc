
#include <Source/O3deFramework_MiscModuleInterface.h>
#include <AzCore/Memory/Memory.h>

#include <O3deFramework/Misc/O3deFramework_MiscTypeIds.h>

#include <Source/O3deFramework_MiscSystemComponent.h>
#include <Source/Components/NetConnectionEntitySpawnerComponent.h>
#include <Source/LevelGameEntitySystemComponent.h>

namespace O3deFramework
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(O3deFramework_MiscModuleInterface,
        "O3deFramework_MiscModuleInterface", O3deFramework_MiscModuleInterfaceTypeId);
    AZ_RTTI_NO_TYPE_INFO_IMPL(O3deFramework_MiscModuleInterface, AZ::Module);
    AZ_CLASS_ALLOCATOR_IMPL(O3deFramework_MiscModuleInterface, AZ::SystemAllocator);

    O3deFramework_MiscModuleInterface::O3deFramework_MiscModuleInterface()
    {
        // Append all items used by both editor and non-editor targets.
        m_descriptors.insert(m_descriptors.end(), {
            O3deFramework_MiscSystemComponent::CreateDescriptor(),
            NetConnectionEntitySpawnerComponent::CreateDescriptor(),
            LevelGameEntitySystemComponent::CreateDescriptor(),
            });
    }

    AZ::ComponentTypeList O3deFramework_MiscModuleInterface::GetRequiredSystemComponents() const
    {
        // Append all items used by both editor and non-editor targets.
        return AZ::ComponentTypeList{
            azrtti_typeid<LevelGameEntitySystemComponent>(),
        };
    }
}
