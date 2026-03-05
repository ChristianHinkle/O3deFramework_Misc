
#include <Source/O3deFramework_MiscModuleInterface.h>
#include <O3deFramework/Misc/O3deFramework_MiscTypeIds.h>
#include <Source/Tools/O3deFramework_MiscEditorSystemComponent.h>

namespace O3deFramework
{
    class O3deFramework_MiscEditorModule : public O3deFramework_MiscModuleInterface
    {
    public:
        AZ_RTTI(O3deFramework_MiscEditorModule, O3deFramework_MiscEditorModuleTypeId, O3deFramework_MiscModuleInterface);
        AZ_CLASS_ALLOCATOR(O3deFramework_MiscEditorModule, AZ::SystemAllocator);

        O3deFramework_MiscEditorModule()
        {
            // Append editor-only items.
            m_descriptors.insert(m_descriptors.end(), {
                O3deFramework_MiscEditorSystemComponent::CreateDescriptor(),
                }
            );
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            AZ::ComponentTypeList list = O3deFramework_MiscModuleInterface::GetRequiredSystemComponents();

            // Append editor-only items.
            list.insert(list.end(), {
                azrtti_typeid<O3deFramework_MiscEditorSystemComponent>(),
                }
            );

            return list;
        }
    };
}

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME, _Editor), O3deFramework::O3deFramework_MiscEditorModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_O3deFramework_Misc_Editor, O3deFramework::O3deFramework_MiscEditorModule)
#endif
