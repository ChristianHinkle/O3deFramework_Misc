
#include <O3deFramework/Misc/O3deFramework_MiscTypeIds.h>
#include <Source/O3deFramework_MiscModuleInterface.h>
#include <Source/Tools/O3deFramework_MiscEditorSystemComponent.h>

namespace O3deFramework_Misc
{
    class O3deFramework_MiscEditorModule
        : public O3deFramework_MiscModuleInterface
    {
    public:
        AZ_RTTI(O3deFramework_MiscEditorModule, O3deFramework_MiscEditorModuleTypeId, O3deFramework_MiscModuleInterface);
        AZ_CLASS_ALLOCATOR(O3deFramework_MiscEditorModule, AZ::SystemAllocator);

        O3deFramework_MiscEditorModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                O3deFramework_MiscEditorSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList {
                azrtti_typeid<O3deFramework_MiscEditorSystemComponent>(),
            };
        }
    };
}// namespace O3deFramework_Misc

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME, _Editor), O3deFramework_Misc::O3deFramework_MiscEditorModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_O3deFramework_Misc_Editor, O3deFramework_Misc::O3deFramework_MiscEditorModule)
#endif
