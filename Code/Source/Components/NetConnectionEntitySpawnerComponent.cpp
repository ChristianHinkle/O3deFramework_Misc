
#include <Source/Components/NetConnectionEntitySpawnerComponent.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <Multiplayer/NetworkEntity/INetworkEntityManager.h>
#include <Multiplayer/IMultiplayer.h>
#include <O3deUtils/Misc/MultiplayerUtils.h>
#include <AzCore/Console/ILogger.h>
#include <Multiplayer/NetworkEntity/NetworkEntityHandle.h>
#include <AzCore/Component/TransformBus.h>
#include <O3deUtils/Core/AzFrameworkUtils.h>

namespace O3deFramework
{
    AZ_COMPONENT_IMPL(NetConnectionEntitySpawnerComponent, "NetConnectionEntitySpawnerComponent", "{5C317DF1-7BD8-4CB2-8A4C-263740B14064}");

    void NetConnectionEntitySpawnerComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<NetConnectionEntitySpawnerComponent, AZ::Component>()
                ->Version(1)
                ->Field("NetConnectionEntitySpawnable", &NetConnectionEntitySpawnerComponent::m_netConnectionEntitySpawnable)
                ->Field("SpawnTransformEntityReference", &NetConnectionEntitySpawnerComponent::m_spawnTransformEntityReference)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<NetConnectionEntitySpawnerComponent>("NetConnectionEntitySpawnerComponent", "[Description of functionality provided by this component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Level"))
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &NetConnectionEntitySpawnerComponent::m_netConnectionEntitySpawnable,
                        "Net Connection Entity Spawnable Asset",
                        "The network spawnable asset which will be created as an autonomous entity for each connection that joins. Only the first entity in the prefab will be spawned and used. Create hierarchy from it if you need multiple.")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &NetConnectionEntitySpawnerComponent::m_spawnTransformEntityReference,
                        "Spawn Transform Entity Reference",
                        "Reference to the entity to use as a spawn transform for the net connection entity.")
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<NetConnectionEntitySpawnerComponent>("NetConnectionEntitySpawner Component Group")
                ->Attribute(AZ::Script::Attributes::Category, "O3deFramework Gem Group")
                ;
        }
    }

    void NetConnectionEntitySpawnerComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("NetConnectionEntitySpawnerComponentService"));
    }

    void NetConnectionEntitySpawnerComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("NetConnectionEntitySpawnerComponentService"));
    }

    void NetConnectionEntitySpawnerComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void NetConnectionEntitySpawnerComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void NetConnectionEntitySpawnerComponent::Activate()
    {
        AZ::Interface<IMultiplayerSpawner>::Register(this);
    }

    void NetConnectionEntitySpawnerComponent::Deactivate()
    {
        AZ::Interface<IMultiplayerSpawner>::Unregister(this);
    }

    Multiplayer::NetworkEntityHandle NetConnectionEntitySpawnerComponent::OnPlayerJoin(
        [[maybe_unused]] uint64_t userId,
        [[maybe_unused]] const Multiplayer::MultiplayerAgentDatum& agentDatum)
    {
        const AZ::Data::Asset<AzFramework::Spawnable>& spawnableAsset = m_netConnectionEntitySpawnable.m_spawnableAsset;

        // Only spawn a single entity from the prefab, as this system expects only one entity.
        Multiplayer::PrefabEntityId prefabEntityId = O3deUtils::MakeSinglePrefabEntityIdFromSpawnableAsset(spawnableAsset);

        constexpr Multiplayer::NetEntityRole netEntityRole = Multiplayer::NetEntityRole::Authority;

        const AZ::Transform& spawnTransform = GetSpawnTransformFromEntityReference();

        Multiplayer::INetworkEntityManager::EntityList entityList =
            O3deUtils::GetNetworkEntityManagerAsserted().CreateEntitiesImmediate(
                AZStd::move(prefabEntityId),
                netEntityRole,
                spawnTransform);

        if (entityList.empty())
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
            return Multiplayer::NetworkEntityHandle{};
        }

        // Return the spawned entity. This will be made autonomous by the multiplayer system.
        return AZStd::move(entityList[0]);
    }

    void NetConnectionEntitySpawnerComponent::OnPlayerLeave(
        Multiplayer::ConstNetworkEntityHandle entityHandle,
        [[maybe_unused]] const Multiplayer::ReplicationSet& replicationSet,
        [[maybe_unused]] AzNetworking::DisconnectReason reason)
    {
        Multiplayer::INetworkEntityManager& networkEntityManager = O3deUtils::GetNetworkEntityManagerAsserted();

        // Walk hierarchy backwards to remove all children before parents
        AZStd::vector<AZ::EntityId> hierarchy = entityHandle.GetEntity()->GetTransform()->GetEntityAndAllDescendants();
        for (auto it = hierarchy.rbegin(); it != hierarchy.rend(); ++it)
        {
            Multiplayer::ConstNetworkEntityHandle hierarchyEntityHandle = networkEntityManager.GetEntity(networkEntityManager.GetNetEntityIdById(*it));
            if (hierarchyEntityHandle)
            {
                networkEntityManager.MarkForRemoval(hierarchyEntityHandle);
            }
        }
    }

    AZ::Transform NetConnectionEntitySpawnerComponent::GetSpawnTransformFromEntityReference() const
    {
        AZ_Assert(O3deUtils::IsRootSpawnableReady(), "The level should be fully loaded at this point, because we are about to sample the transform from an entity reference from the level.");

        AZ::Transform result{};
        AZ::TransformBus::EventResult(result, m_spawnTransformEntityReference, &AZ::TransformBus::Events::GetWorldTM);
        return result;
    }
} // namespace O3deFramework
