
#include <Source/O3deFramework_MiscModuleInterface.h>
#include <AzCore/Memory/Memory.h>

#include <O3deFramework/Misc/O3deFramework_MiscTypeIds.h>

#include <Source/O3deFramework_MiscSystemComponent.h>
#include <Source/Components/NetConnectionEntitySpawnerComponent.h>

namespace O3deFramework_Misc
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(O3deFramework_MiscModuleInterface,
        "O3deFramework_MiscModuleInterface", O3deFramework_MiscModuleInterfaceTypeId);
    AZ_RTTI_NO_TYPE_INFO_IMPL(O3deFramework_MiscModuleInterface, AZ::Module);
    AZ_CLASS_ALLOCATOR_IMPL(O3deFramework_MiscModuleInterface, AZ::SystemAllocator);

    O3deFramework_MiscModuleInterface::O3deFramework_MiscModuleInterface()
    {
        // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
        // Add ALL components descriptors associated with this gem to m_descriptors.
        // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
        // This happens through the [MyComponent]::Reflect() function.
        m_descriptors.insert(m_descriptors.end(), {
            O3deFramework_MiscSystemComponent::CreateDescriptor(),
            O3deFramework::NetConnectionEntitySpawnerComponent::CreateDescriptor(),
            });
    }

    AZ::ComponentTypeList O3deFramework_MiscModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<O3deFramework_MiscSystemComponent>(),
        };
    }
} // namespace O3deFramework_Misc
