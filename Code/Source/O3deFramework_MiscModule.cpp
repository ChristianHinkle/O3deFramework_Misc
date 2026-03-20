
#include <Source/O3deFramework_MiscModuleInterface.h>
#include <O3deFramework_MiscTypeIds.h>
#include <Source/O3deFramework_MiscSystemComponent.h>
#include <Source/Components/EntitySpawnerComponent_OneOffAtTransforms.h>

namespace O3deFramework
{
    class O3deFramework_MiscModule : public O3deFramework_MiscModuleInterface
    {
    public:
        AZ_RTTI(O3deFramework_MiscModule, O3deFramework_MiscModuleTypeId, O3deFramework_MiscModuleInterface);
        AZ_CLASS_ALLOCATOR(O3deFramework_MiscModule, AZ::SystemAllocator);

        O3deFramework_MiscModule()
        {
            // Append non-editor items.
            m_descriptors.insert(m_descriptors.end(), {
                O3deFramework_MiscSystemComponent::CreateDescriptor(),
                EntitySpawnerComponent_OneOffAtTransforms::CreateDescriptor(),
                }
            );
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            AZ::ComponentTypeList list = O3deFramework_MiscModuleInterface::GetRequiredSystemComponents();

            // Append non-editor items.
            list.insert(list.end(), {
                azrtti_typeid<O3deFramework_MiscSystemComponent>(),
                }
            );

            return list;
        }
    };
}

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), O3deFramework::O3deFramework_MiscModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_O3deFramework_Misc, O3deFramework::O3deFramework_MiscModule)
#endif
