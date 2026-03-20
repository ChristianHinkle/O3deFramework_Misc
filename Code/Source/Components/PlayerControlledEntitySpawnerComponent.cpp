
#include <Source/Components/PlayerControlledEntitySpawnerComponent.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <Multiplayer/NetworkEntity/INetworkEntityManager.h>
#include <Multiplayer/IMultiplayer.h>
#include <O3deUtils/Misc/MultiplayerUtils.h>
#include <AzCore/Console/ILogger.h>
#include <AzCore/Debug/Trace.h>
#include <Multiplayer/NetworkEntity/NetworkEntityHandle.h>
#include <AzCore/Component/TransformBus.h>
#include <Multiplayer/Components/NetBindComponent.h>
#include <O3deUtils/Core/AzFrameworkUtils.h>
#include <Include/O3deFramework/Misc/NetConnectionEntityBus.h>
#include <Include/O3deFramework/Misc/PlayerEntityManagerInterface.h>
#include <Include/O3deFramework/Misc/EntityControlInterface.h>

namespace O3deFramework
{
    AZ_COMPONENT_IMPL(PlayerControlledEntitySpawnerComponent, "PlayerControlledEntitySpawnerComponent", "{3DE6AB85-7797-4287-8CC0-95AE78AB8EFA}");

    void PlayerControlledEntitySpawnerComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<PlayerControlledEntitySpawnerComponent, AZ::Component>()
                ->Version(1)
                ->Field("Spawnable", &PlayerControlledEntitySpawnerComponent::m_spawnable)
                ->Field("SpawnTransformEntityReference", &PlayerControlledEntitySpawnerComponent::m_spawnTransformEntityReference)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<PlayerControlledEntitySpawnerComponent>("PlayerControlledEntitySpawnerComponent", "[Description of functionality provided by this component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Level")) // @Christian: TODO: [todo][spawn] Maybe support non-level entities as well. If we do that, we should be using the net entity role of the net bind component, instead of the multiplayer agent type, in the `HasAuthority` function.
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &PlayerControlledEntitySpawnerComponent::m_spawnable,
                        "Net Connection Entity Spawnable Asset",
                        "The network spawnable asset which will be created at the spawn transform. Only the first entity in the prefab will be spawned.")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &PlayerControlledEntitySpawnerComponent::m_spawnTransformEntityReference,
                        "Spawn Transform Entity Reference",
                        "Reference to the entity to use as a spawn transform.")
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<PlayerControlledEntitySpawnerComponent>("PlayerControlledEntitySpawnerComponent Group")
                ->Attribute(AZ::Script::Attributes::Category, "O3deFramework Gem Group")
                ;
        }
    }

    void PlayerControlledEntitySpawnerComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("PlayerControlledEntitySpawnerComponentService"));
    }

    void PlayerControlledEntitySpawnerComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("PlayerControlledEntitySpawnerComponentService"));
    }

    void PlayerControlledEntitySpawnerComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void PlayerControlledEntitySpawnerComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void PlayerControlledEntitySpawnerComponent::Activate()
    {
#if AZ_TRAIT_SERVER
        O3deFramework::AddEventOnNetConnectionAdded(m_onNetConnectionAddedHandler);
#endif // #if AZ_TRAIT_SERVER
    }

    void PlayerControlledEntitySpawnerComponent::Deactivate()
    {
#if AZ_TRAIT_SERVER
        m_onNetConnectionAddedHandler.Disconnect();
#endif // #if AZ_TRAIT_SERVER
    }

#if AZ_TRAIT_SERVER
    bool PlayerControlledEntitySpawnerComponent::HasAuthority() const
    {
        return O3deUtils::IsHosting();
    }

    void PlayerControlledEntitySpawnerComponent::SpawnForPlayer(const AZ::EntityId& playerEntityId) const
    {
        AZ_Assert(HasAuthority(), "Should only be called on authority.");

        {
            AZStd::fixed_string<256> logString;

            logString += '`';
            logString += __func__;
            logString += "`: ";
            logString += "Spawning entity for player entity: ";
            logString += O3deUtils::EntityIdToString(playerEntityId);
            logString += '.';

            AZLOG_INFO(logString.data());
        }

        AZ_Assert(O3deUtils::IsRootSpawnableReady(), "The level should be fully loaded at this point, because we are about to sample the transform from an entity reference from the level.");

        constexpr Multiplayer::NetEntityRole netEntityRole = Multiplayer::NetEntityRole::Authority;

        const AZ::Data::Asset<AzFramework::Spawnable>& spawnableAsset = m_spawnable.m_spawnableAsset;

        // Only spawn a single entity from the prefab, as this system expects only one entity.
        Multiplayer::PrefabEntityId prefabEntityId = O3deUtils::MakeSinglePrefabEntityIdFromSpawnableAsset(spawnableAsset);

        AZ::Transform spawnTransform{};
        AZ::TransformBus::EventResult(spawnTransform, m_spawnTransformEntityReference, &AZ::TransformBus::Events::GetWorldTM);

        {
            AZStd::fixed_string<256> logString;

            logString += '`';
            logString += __func__;
            logString += "`: ";
            logString += "Spawning prefab '";
            logString += prefabEntityId.m_prefabName.GetStringView();
            logString += "'.";
            // TODO: Also print out the transform it's spawning it at, and make a nice function to create a string from a transform.

            AZLOG_INFO(logString.data());
        }

        Multiplayer::INetworkEntityManager::EntityList createdEntities =
            O3deUtils::GetNetworkEntityManagerAsserted().CreateEntitiesImmediate(
                AZStd::move(prefabEntityId),
                netEntityRole,
                spawnTransform);

        if (createdEntities.empty())
        {
            AZStd::fixed_string<256> logString;

            logString += '`';
            logString += __func__;
            logString += "`: ";
            logString += "Attempt to spawn prefab '";
            logString += spawnableAsset.GetHint();
            logString += "' failed. No entities were spawned.";
            logString += ' ';
            logString += "Ensure that the prefab contains a single entity that is network enabled with a network binding component.";

            AZLOG_ERROR(logString.data());
            return;
        }

        // Make the player control the entity that we just spawned.

        // @Christian: TODO: [todo] Maybe decouple entity controlling out of this component. I figured this was a pretty specific component anyway, mainly
        // just for convenience, but if we do want to generalize this component, we should take stuff like this out.
        EntityControlRequestFunctions::ControlEntity(playerEntityId, createdEntities[0].GetNetEntityId());
    }
#endif // #if AZ_TRAIT_SERVER

    AZ::Transform PlayerControlledEntitySpawnerComponent::GetSpawnTransformFromEntityReference() const
    {
        AZ_Assert(O3deUtils::IsRootSpawnableReady(), "The level should be fully loaded at this point, because we are about to sample the transform from an entity reference from the level.");

        AZ::Transform result{};
        AZ::TransformBus::EventResult(result, m_spawnTransformEntityReference, &AZ::TransformBus::Events::GetWorldTM);
        return result;
    }

    void PlayerControlledEntitySpawnerComponent::OnNetConnectionAdded([[maybe_unused]] const Multiplayer::ConstNetworkEntityHandle& entityNetworkHandle, [[maybe_unused]] const AZ::EntityId& entityId)
    {
        // @Christian: TODO: [todo] Now subscribe to the event for this net connection getting new players added.

        //AddEventOnPlayerAdded(entityId, )
    }
}
