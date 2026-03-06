
#include <Source/Tools/O3deFramework_MiscEditorSystemComponent.h>

#include <O3deFramework_MiscTypeIds.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace O3deFramework
{
    AZ_COMPONENT_IMPL(O3deFramework_MiscEditorSystemComponent, "O3deFramework_MiscEditorSystemComponent",
        O3deFramework_MiscEditorSystemComponentTypeId, BaseSystemComponent);

    void O3deFramework_MiscEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<O3deFramework_MiscEditorSystemComponent, BaseSystemComponent>()
                ->Version(0)
                ;
        }
    }

    void O3deFramework_MiscEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("O3deFramework_MiscEditorService"));
    }

    void O3deFramework_MiscEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("O3deFramework_MiscEditorService"));
    }

    void O3deFramework_MiscEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void O3deFramework_MiscEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    O3deFramework_MiscEditorSystemComponent::O3deFramework_MiscEditorSystemComponent() = default;
    O3deFramework_MiscEditorSystemComponent::~O3deFramework_MiscEditorSystemComponent() = default;

    void O3deFramework_MiscEditorSystemComponent::Activate()
    {
        BaseSystemComponent::Activate();

        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
    }

    void O3deFramework_MiscEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();

        BaseSystemComponent::Deactivate();
    }
}
