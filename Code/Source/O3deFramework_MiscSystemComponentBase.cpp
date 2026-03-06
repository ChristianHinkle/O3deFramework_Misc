
#include <Source/O3deFramework_MiscSystemComponentBase.h>

#include <O3deFramework_MiscTypeIds.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <Source/AutoGen/AutoComponentTypes.h>

namespace O3deFramework
{
    AZ_COMPONENT_IMPL(O3deFramework_MiscSystemComponentBase, "O3deFramework_MiscSystemComponentBase",
        O3deFramework_MiscSystemComponentBaseTypeId);

    void O3deFramework_MiscSystemComponentBase::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<O3deFramework_MiscSystemComponentBase, AZ::Component>()
                ->Version(0)
                ;
        }
    }

    void O3deFramework_MiscSystemComponentBase::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("O3deFramework_MiscBaseService"));
    }

    void O3deFramework_MiscSystemComponentBase::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("O3deFramework_MiscBaseService"));
    }

    void O3deFramework_MiscSystemComponentBase::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void O3deFramework_MiscSystemComponentBase::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    O3deFramework_MiscSystemComponentBase::O3deFramework_MiscSystemComponentBase()
    {
        if (O3deFramework_MiscInterface::Get() == nullptr)
        {
            O3deFramework_MiscInterface::Register(this);
        }
    }

    O3deFramework_MiscSystemComponentBase::~O3deFramework_MiscSystemComponentBase()
    {
        if (O3deFramework_MiscInterface::Get() == this)
        {
            O3deFramework_MiscInterface::Unregister(this);
        }
    }

    void O3deFramework_MiscSystemComponentBase::Init()
    {
    }

    void O3deFramework_MiscSystemComponentBase::Activate()
    {
        O3deFramework_MiscRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();

        // Register our auto-components.
        RegisterMultiplayerComponents();
    }

    void O3deFramework_MiscSystemComponentBase::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        O3deFramework_MiscRequestBus::Handler::BusDisconnect();
    }

    void O3deFramework_MiscSystemComponentBase::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }
}
