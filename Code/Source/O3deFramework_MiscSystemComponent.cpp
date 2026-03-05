
#include <Source/O3deFramework_MiscSystemComponent.h>

#include <O3deFramework/Misc/O3deFramework_MiscTypeIds.h>

#include <AzCore/Serialization/SerializeContext.h>

namespace O3deFramework
{
    AZ_COMPONENT_IMPL(O3deFramework_MiscSystemComponent, "O3deFramework_MiscSystemComponent",
        O3deFramework_MiscSystemComponentTypeId);

    void O3deFramework_MiscSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<O3deFramework_MiscSystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }
    }

    void O3deFramework_MiscSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("O3deFramework_MiscService"));
    }

    void O3deFramework_MiscSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("O3deFramework_MiscService"));
    }

    void O3deFramework_MiscSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void O3deFramework_MiscSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    O3deFramework_MiscSystemComponent::O3deFramework_MiscSystemComponent()
    {
        if (O3deFramework_MiscInterface::Get() == nullptr)
        {
            O3deFramework_MiscInterface::Register(this);
        }
    }

    O3deFramework_MiscSystemComponent::~O3deFramework_MiscSystemComponent()
    {
        if (O3deFramework_MiscInterface::Get() == this)
        {
            O3deFramework_MiscInterface::Unregister(this);
        }
    }

    void O3deFramework_MiscSystemComponent::Init()
    {
    }

    void O3deFramework_MiscSystemComponent::Activate()
    {
        O3deFramework_MiscRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
    }

    void O3deFramework_MiscSystemComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        O3deFramework_MiscRequestBus::Handler::BusDisconnect();
    }

    void O3deFramework_MiscSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

}
