
#include <Source/Components/EntityControlComponent.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <Multiplayer/NetworkEntity/INetworkEntityManager.h>
#include <Multiplayer/IMultiplayer.h>
#include <O3deUtils/Misc/MultiplayerUtils.h>
#include <AzCore/Console/ILogger.h>
#include <AzCore/Debug/Trace.h>
#include <Multiplayer/NetworkEntity/NetworkEntityHandle.h>
#include <Multiplayer/Components/NetBindComponent.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Component/Entity.h>
#include <CppUtils/Core/Algorithm.h>
#include <O3deUtils/Core/AzCoreUtils.h>

namespace O3deFramework
{
    EntityControlComponentController::EntityControlComponentController(EntityControlComponent& parent)
        : EntityControlComponentControllerBase(parent)
    {
    }

    void EntityControlComponentController::OnActivate([[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
    }

    void EntityControlComponentController::OnDeactivate([[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
    }

    void EntityControlComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<EntityControlComponent, EntityControlComponentBase>()
                ->Version(1)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<EntityControlComponent>("EntityControlComponent", "[Description of functionality provided by this component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ;
            }
        }

        EntityControlComponentBase::Reflect(context);
    }

    void EntityControlComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        EntityControlComponentBase::GetRequiredServices(required);
    }

    void EntityControlComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        EntityControlComponentBase::GetDependentServices(dependent);
    }

    void EntityControlComponent::OnActivate([[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
    }

    void EntityControlComponent::OnDeactivate([[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
    }

    bool EntityControlComponent::ControlEntity(const AZ::EntityId& entityId)
    {
        {
            AZStd::fixed_string<256> logString;

            logString += '`';
            logString += __func__;
            logString += "`: ";
            logString += "Entity to control: ";
            logString += O3deUtils::EntityIdToString(entityId);
            logString += '.';

            AZLOG_INFO(logString.data());
        }

        const Multiplayer::NetEntityId netEntityId = O3deUtils::GetNetworkEntityManagerAsserted().GetNetEntityIdById(entityId);
        if (netEntityId == Multiplayer::InvalidNetEntityId)
        {
            // @Christian: TODO: [todo][techdebt][multiplayer] Implement support for being able to contorl non-networked entities.
            AZ_Assert(false, "Currently not supported.");
            return false;
        }

        return ControlEntity(netEntityId);
    }
    bool EntityControlComponent::ControlEntity(const Multiplayer::NetEntityId& netEntityId)
    {
        {
            AZStd::fixed_string<256> logString;

            logString += '`';
            logString += __func__;
            logString += "`: ";
            logString += "Entity to control: ";
            logString += O3deUtils::NetEntityIdToString(netEntityId);
            logString += ".";

            AZLOG_INFO(logString.data());
        }

        constexpr AZStd::string_view noAuthorityLogString = "Has no network authority to change control.";

#if !AZ_TRAIT_SERVER
        {
            AZLOG_ERROR(noAuthorityLogString.data());
            return false;
        }
#else
        {
            if (!O3deUtils::GetNetBindComponentAsserted(GetEntityId()).IsNetEntityRoleAuthority())
            {
                AZLOG_ERROR(noAuthorityLogString.data());
                return false;
            }

            if (!TryAddToControlledNetEntityIds(netEntityId))
            {
                return false;
            }

            const AZ::EntityId entityId = O3deUtils::GetEntityIdByNetEntityIdAsserted(netEntityId);
            HandleAddedControlledEntity(entityId);
            return true;
        }
#endif // #if !AZ_TRAIT_SERVER
    }

    bool EntityControlComponent::UncontrolEntity(const AZ::EntityId& entityId)
    {
        {
            AZStd::fixed_string<256> logString;

            logString += '`';
            logString += __func__;
            logString += "`: ";
            logString += "Entity to uncontrol: ";
            logString += O3deUtils::EntityIdToString(entityId);
            logString += '.';

            AZLOG_INFO(logString.data());
        }

        const Multiplayer::NetEntityId netEntityId = O3deUtils::GetNetworkEntityManagerAsserted().GetNetEntityIdById(entityId);
        if (netEntityId == Multiplayer::InvalidNetEntityId)
        {
            // @Christian: TODO: [todo][techdebt][multiplayer] Implement support for being able to contorl non-networked entities.
            AZ_Assert(false, "Currently not supported.");
            return false;
        }

        return UncontrolEntity(netEntityId);
    }
    bool EntityControlComponent::UncontrolEntity(const Multiplayer::NetEntityId& netEntityId)
    {
        {
            AZStd::fixed_string<256> logString;

            logString += '`';
            logString += __func__;
            logString += "`: ";
            logString += "Entity to uncontrol: ";
            logString += O3deUtils::NetEntityIdToString(netEntityId);
            logString += ".";

            AZLOG_ERROR(logString.data());
        }

        constexpr AZStd::string_view noAuthorityLogString = "Has no network authority to change control.";

#if !AZ_TRAIT_SERVER
        {
            AZLOG_ERROR(noAuthorityLogString.data());
            return false;
        }
#else
        {
            if (!O3deUtils::GetNetBindComponentAsserted(GetEntityId()).IsNetEntityRoleAuthority())
            {
                AZLOG_ERROR(noAuthorityLogString.data());
                return false;
            }

            if (!TryRemoveFromControlledNetEntityIds(netEntityId))
            {
                return false;
            }

            const AZ::EntityId entityId = O3deUtils::GetEntityIdByNetEntityIdAsserted(netEntityId);
            HandleRemovedControlledEntity(entityId);
            return true;
        }
#endif // #if !AZ_TRAIT_SERVER
    }

#if AZ_TRAIT_SERVER
    bool EntityControlComponent::TryAddToControlledNetEntityIds(const Multiplayer::NetEntityId& netEntityId)
    {
        AZ_Assert(O3deUtils::GetNetBindComponentAsserted(GetEntityId()).IsNetEntityRoleAuthority(), "This logic is authority only.");

        EntityControlComponentController* multiplayerController = GetEntityControlComponentController();
        AZ_Assert(multiplayerController, "Shouldn't be null.");

        {
            const ControlledNetEntityIdsVector& controlledNetEntityIds = multiplayerController->GetControlledNetEntityIds();
            if (CppUtils::contains(controlledNetEntityIds, netEntityId))
            {
                {
                    AZStd::fixed_string<256> logString;

                    logString += '`';
                    logString += __func__;
                    logString += "`: ";
                    logString += "Entity already exists in the controlled net entity id container. Returning false.";

                    AZLOG_ERROR(logString.data());
                }

                return false;
            }
        }

        multiplayerController->ModifyControlledNetEntityIds().emplace_back(netEntityId);
        return true;
    }

    bool EntityControlComponent::TryRemoveFromControlledNetEntityIds(const Multiplayer::NetEntityId& netEntityId)
    {
        AZ_Assert(O3deUtils::GetNetBindComponentAsserted(GetEntityId()).IsNetEntityRoleAuthority(), "This logic is authority only.");

        EntityControlComponentController* multiplayerController = GetEntityControlComponentController();
        AZ_Assert(multiplayerController, "Shouldn't be null.");

        ControlledNetEntityIdsVector& controlledNetEntityIds = multiplayerController->ModifyControlledNetEntityIds();
        const AZStd::size_t numRemoved = AZStd::erase(controlledNetEntityIds, netEntityId);

        return numRemoved > 0u;
    }

    void EntityControlComponent::HandleAddedControlledEntity(const AZ::EntityId& entityId)
    {
        AZ_Assert(O3deUtils::GetNetBindComponentAsserted(GetEntityId()).IsNetEntityRoleAuthority(), "This logic is authority only.");

        const AZ::EntityId& owningEntityId = GetEntityId();

        // @Christian TODO: [todo][multiplayer][control] I think this is weird to have this component changing the parent. Maybe make it customizable what happens on controlled/uncontrolled. Maybe let them decide the specific parent.
        AZ::TransformBus::Event(entityId, &AZ::TransformInterface::SetParent, owningEntityId);
    }

    void EntityControlComponent::HandleRemovedControlledEntity(const AZ::EntityId& entityId)
    {
        AZ_Assert(O3deUtils::GetNetBindComponentAsserted(GetEntityId()).IsNetEntityRoleAuthority(), "This logic is authority only.");

        const AZ::EntityId& owningEntityId = GetEntityId();

        AZ::EntityId parentEntityId{};
        AZ::TransformBus::EventResult(parentEntityId, owningEntityId, &AZ::TransformInterface::GetParentId);

        // @Christian TODO: [todo][multiplayer][control] I think this is weird to have this component changing the parent. Maybe make it customizable what happens on controlled/uncontrolled. Maybe let them decide the specific parent.
        AZ::TransformBus::Event(entityId, &AZ::TransformInterface::SetParent, parentEntityId);
    }
#endif // #if AZ_TRAIT_SERVER
}
