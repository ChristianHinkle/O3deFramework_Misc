
#include <Source/O3deFramework_MiscSystemComponent.h>

#include <O3deFramework_MiscTypeIds.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace O3deFramework
{
    AZ_COMPONENT_IMPL(O3deFramework_MiscSystemComponent, "O3deFramework_MiscSystemComponent",
        O3deFramework_MiscSystemComponentTypeId, BaseSystemComponent);

    void O3deFramework_MiscSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<O3deFramework_MiscSystemComponent, BaseSystemComponent>()
                ->Version(0)
                ;
        }
    }

    void O3deFramework_MiscSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("O3deFramework_MiscService"));
    }

    void O3deFramework_MiscSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("O3deFramework_MiscService"));
    }

    void O3deFramework_MiscSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void O3deFramework_MiscSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    O3deFramework_MiscSystemComponent::O3deFramework_MiscSystemComponent() = default;
    O3deFramework_MiscSystemComponent::~O3deFramework_MiscSystemComponent() = default;

    void O3deFramework_MiscSystemComponent::Activate()
    {
        BaseSystemComponent::Activate();
    }

    void O3deFramework_MiscSystemComponent::Deactivate()
    {
        BaseSystemComponent::Deactivate();
    }
}
