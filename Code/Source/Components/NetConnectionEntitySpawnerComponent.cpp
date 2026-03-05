
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
#include <LmbrCentral/Scripting/TagComponentBus.h>
#include <CppUtils/Core/Algorithm.h>

namespace O3deFramework
{
    bool IsNetConnectionEntity(const AZ::EntityId& entityId)
    {
        // @Christian: TODO: [todo][techdebt][tag] Use a proper "gameplay tag" system instead of LmbrCentral's tag component.
        bool result{};
        LmbrCentral::TagComponentRequestBus::EventResult(result, entityId, &LmbrCentral::TagComponentRequestBus::Handler::HasTag, NetConnectionEntityTag);
        return result;
    }

#if AZ_TRAIT_SERVER
    NetConnectionDataContainer::ServerElementReference NetConnectionDataContainer::ServerEmplaceNewElement()
    {
        AZ_Assert(O3deUtils::IsHosting(), "This function should only be called on the server.");

        NetConnectionSpawnedEntityData& spawnedEntityData = m_entityDataVector.emplace_back();
        NetConnectionIdentificationData& identificationData = m_identificationDataVector.emplace_back();

        return ServerElementReference{
            .m_entityData = spawnedEntityData,
            .m_identificationData = identificationData
        };
    }

    std::size_t NetConnectionDataContainer::ServerEraseElement(const AZ::EntityId entityId)
    {
        AZ_Assert(O3deUtils::IsHosting(), "This function should only be called on the server.");

        const std::size_t foundIndex = FindIndexByEntityId(entityId);
        if (foundIndex == CppUtils::npos)
        {
            return 0u;
        }

        m_entityDataVector.erase(m_entityDataVector.begin() + foundIndex);
        m_identificationDataVector.erase(m_identificationDataVector.begin() + foundIndex);
        return 1u;
    }
#endif // #if AZ_TRAIT_SERVER

#if AZ_TRAIT_CLIENT
    NetConnectionDataContainer::ClientElementReference NetConnectionDataContainer::ClientEmplaceNewElement()
    {
        AZ_Assert(!O3deUtils::IsHosting(), "This function should only be called on the client.");

        NetConnectionSpawnedEntityData& spawnedEntityData = m_entityDataVector.emplace_back();

        return ClientElementReference{
            .m_entityData = spawnedEntityData
        };
    }

    std::size_t NetConnectionDataContainer::ClientEraseElement(const AZ::EntityId entityId)
    {
        AZ_Assert(!O3deUtils::IsHosting(), "This function should only be called on the client.");

        const std::size_t foundIndex = FindIndexByEntityId(entityId);
        if (foundIndex == CppUtils::npos)
        {
            return 0u;
        }

        m_entityDataVector.erase(m_entityDataVector.begin() + foundIndex);
        return 1u;
    }
#endif // #if AZ_TRAIT_CLIENT

    std::size_t NetConnectionDataContainer::FindIndexByEntityId(const AZ::EntityId entityId) const
    {
        const std::size_t foundIndex = CppUtils::index_find_if(
            m_entityDataVector.begin(),
            m_entityDataVector.end(),
            [entityId](const NetConnectionSpawnedEntityData& item)
            {
                return item.GetEntityId() == entityId;
            }
        );

#if AZ_TRAIT_SERVER
        if (O3deUtils::IsHosting())
        {
            if (foundIndex != CppUtils::npos)
            {
                AZ_Assert(foundIndex < m_identificationDataVector.size(), "The found index should be valid for both containers! Otherwise the two containers have gone out of sync!");
            }
        }
#endif // #if AZ_TRAIT_SERVER

        return foundIndex;
    }

#if AZ_TRAIT_SERVER
    std::size_t NetConnectionDataContainer::FindIndexByMultiplayerUserId(std::uint64_t temporaryUserId) const
    {
        AZ_Assert(O3deUtils::IsHosting(), "This function should only be called on the server.");

        const std::size_t foundIndex = CppUtils::index_find_if(
            m_identificationDataVector.begin(),
            m_identificationDataVector.end(),
            [temporaryUserId](const NetConnectionIdentificationData& item)
            {
                return item.m_multiplayerUserId == temporaryUserId;
            }
        );

        if (foundIndex != CppUtils::npos)
        {
            AZ_Assert(foundIndex < m_entityDataVector.size(), "The found index should be valid for both containers! Otherwise the two containers have gone out of sync!");
        }

        return foundIndex;
    }

    std::size_t NetConnectionDataContainer::FindIndexByConnectionId(AzNetworking::ConnectionId connectionId) const
    {
        AZ_Assert(O3deUtils::IsHosting(), "This function should only be called on the server.");

        const std::size_t foundIndex = CppUtils::index_find_if(
            m_identificationDataVector.begin(),
            m_identificationDataVector.end(),
            [connectionId](const NetConnectionIdentificationData& item)
            {
                return item.m_connectionId == connectionId;
            }
        );

        if (foundIndex != CppUtils::npos)
        {
            AZ_Assert(foundIndex < m_entityDataVector.size(), "The found index should be valid for both containers! Otherwise the two containers have gone out of sync!");
        }

        return foundIndex;
    }
#endif // #if AZ_TRAIT_SERVER

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

#if AZ_TRAIT_CLIENT
        O3deUtils::GetNetworkEntityManagerAsserted().AddControllersActivatedHandler(m_onNetworkEntityControllersActivatedHandler);
        O3deUtils::GetNetworkEntityManagerAsserted().AddControllersDeactivatedHandler(m_onNetworkEntityControllersDeactivatedHandler);
#endif // #if AZ_TRAIT_CLIENT
    }

    void NetConnectionEntitySpawnerComponent::Deactivate()
    {
        AZ::Interface<IMultiplayerSpawner>::Unregister(this);

#if AZ_TRAIT_CLIENT
        m_onNetworkEntityControllersActivatedHandler.Disconnect();
        m_onNetworkEntityControllersDeactivatedHandler.Disconnect();
#endif // #if AZ_TRAIT_CLIENT
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

        AZ_Assert(entityList.size() == 1u, "It shouldn't be possible for more than one to have been spawned.");

        Multiplayer::NetworkEntityHandle& spawnedEntityNetworkHandle = entityList[0];

#if AZ_TRAIT_SERVER
        {
            NetConnectionDataContainer::ServerElementReference netConnectionData = m_netConnectionDataContainer.ServerEmplaceNewElement();

            netConnectionData.m_entityData.GetSpawnedEntityReference().Set(spawnedEntityNetworkHandle, O3deUtils::GetEntityIdByNetEntityIdAsserted(spawnedEntityNetworkHandle.GetNetEntityId()));

            netConnectionData.m_identificationData.m_multiplayerUserId = userId;
            netConnectionData.m_identificationData.m_connectionId = agentDatum.m_id;
        }
#endif // #if AZ_TRAIT_SERVER

        // Return the spawned entity. This will be made autonomous by the multiplayer system.
        return AZStd::move(spawnedEntityNetworkHandle);
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

#if AZ_TRAIT_SERVER
        {
            AZ::EntityId entityId = O3deUtils::GetEntityIdByNetEntityIdAsserted(entityHandle.GetNetEntityId());

            const AZStd::size_t numRemoved = m_netConnectionDataContainer.ServerEraseElement(entityId);
            AZ_Assert(numRemoved == 1u, "Expected to remove at least and exactly 1 item for the player who left.");
        }
#endif // #if AZ_TRAIT_SERVER
    }

    AZ::EntityId NetConnectionEntitySpawnerComponent::GetLocalNetConnectionEntityId() const
    {
        return GetLocalNetConnectionSpawnedEntityData().GetEntityId();
    }

    Multiplayer::ConstNetworkEntityHandle NetConnectionEntitySpawnerComponent::GetLocalNetConnectionEntityNetworkHandle() const
    {
        return GetLocalNetConnectionSpawnedEntityData().GetNetworkHandle();
    }

#if AZ_TRAIT_SERVER
    AZ::EntityId NetConnectionEntitySpawnerComponent::GetNetConnectionEntityIdByMultiplayerUserId(std::uint64_t temporaryUserId) const
    {
        return GetNetConnectionSpawnedEntityDataByMultiplayerUserId(temporaryUserId).GetEntityId();
    }

    Multiplayer::ConstNetworkEntityHandle NetConnectionEntitySpawnerComponent::GetNetConnectionEntityNetworkHandleByMultiplayerUserId(std::uint64_t temporaryUserId) const
    {
        return GetNetConnectionSpawnedEntityDataByMultiplayerUserId(temporaryUserId).GetNetworkHandle();
    }

    AZ::EntityId NetConnectionEntitySpawnerComponent::GetNetConnectionEntityIdByConnectionId(AzNetworking::ConnectionId connectionId) const
    {
        return GetNetConnectionSpawnedEntityDataByConnectionId(connectionId).GetEntityId();
    }

    Multiplayer::ConstNetworkEntityHandle NetConnectionEntitySpawnerComponent::GetNetConnectionEntityNetworkHandleByConnectionId(AzNetworking::ConnectionId connectionId) const
    {
        return GetNetConnectionSpawnedEntityDataByConnectionId(connectionId).GetNetworkHandle();
    }
#endif // #if AZ_TRAIT_SERVER

    NetConnectionSpawnedEntityData NetConnectionEntitySpawnerComponent::GetLocalNetConnectionSpawnedEntityData() const
    {
#if AZ_TRAIT_SERVER
        if (O3deUtils::IsHosting())
        {
            return GetNetConnectionSpawnedEntityDataByConnectionId(O3deUtils::LocalClientConnectionId);
        }
#endif // #if AZ_TRAIT_SERVER

#if AZ_TRAIT_CLIENT
        {
            // We are a client or we're a listening server who is not hosting at the moment.

            std::span entityDataSpan = m_netConnectionDataContainer.GetEntityDataSpan();

            auto foundEntityDataIt = std::find_if(entityDataSpan.begin(), entityDataSpan.end(),
                [](const NetConnectionSpawnedEntityData& item)
                {
                    AZ::EntityId entityId = item.GetEntityId();

                    const bool isLocalNetConnectionEntity = O3deUtils::IsNetEntityRoleAutonomous(entityId);
                    return isLocalNetConnectionEntity;
                }
            );

            if (foundEntityDataIt == entityDataSpan.end())
            {
                return {};
            }

            return *foundEntityDataIt;
        }
#else
        {
            // We are a dedicated server who is not hosting at the moment.
            return {};
        }
#endif
    }

#if AZ_TRAIT_SERVER
    NetConnectionSpawnedEntityData NetConnectionEntitySpawnerComponent::GetNetConnectionSpawnedEntityDataByMultiplayerUserId(std::uint64_t temporaryUserId) const
    {
        AZ_Assert(O3deUtils::IsHosting(), "This function should only be called on the server.");

        const std::size_t foundIndex = m_netConnectionDataContainer.FindIndexByMultiplayerUserId(temporaryUserId);
        if (foundIndex == CppUtils::npos)
        {
            return {};
        }

        return m_netConnectionDataContainer.GetEntityDataSpan()[foundIndex];
    }

    NetConnectionSpawnedEntityData NetConnectionEntitySpawnerComponent::GetNetConnectionSpawnedEntityDataByConnectionId(AzNetworking::ConnectionId connectionId) const
    {
        AZ_Assert(O3deUtils::IsHosting(), "This function should only be called on the server.");

        const std::size_t foundIndex = m_netConnectionDataContainer.FindIndexByConnectionId(connectionId);
        if (foundIndex == CppUtils::npos)
        {
            return {};
        }

        return m_netConnectionDataContainer.GetEntityDataSpan()[foundIndex];
    }
#endif // #if AZ_TRAIT_SERVER

    AZ::Transform NetConnectionEntitySpawnerComponent::GetSpawnTransformFromEntityReference() const
    {
        AZ_Assert(O3deUtils::IsRootSpawnableReady(), "The level should be fully loaded at this point, because we are about to sample the transform from an entity reference from the level.");

        AZ::Transform result{};
        AZ::TransformBus::EventResult(result, m_spawnTransformEntityReference, &AZ::TransformBus::Events::GetWorldTM);
        return result;
    }

#if AZ_TRAIT_CLIENT
    void NetConnectionEntitySpawnerComponent::OnNetworkEntityControllersActivated(
        [[maybe_unused]] const Multiplayer::ConstNetworkEntityHandle& entityHandle,
        [[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
        if (!O3deUtils::IsHosting())
        {
            const AZ::EntityId entityLocalId = O3deUtils::GetEntityIdByNetEntityIdAsserted(entityHandle.GetNetEntityId());
            if (IsNetConnectionEntity(entityLocalId))
            {
                {
                    AZStd::fixed_string<256> logString;

                    logString += '`';
                    logString += __func__;
                    logString += "`: ";
                    logString += "Handling net connection entity on controllers activated. Adding new net connection data to the container.";
                    logString += ' ';
                    logString += "Entity id: ";
                    logString += O3deUtils::EntityIdToString(entityLocalId);
                    logString += '.';

                    AZLOG_INFO(logString.data());
                }

                NetConnectionDataContainer::ClientElementReference newElement = m_netConnectionDataContainer.ClientEmplaceNewElement();

                newElement.m_entityData.GetSpawnedEntityReference().Set(entityHandle, entityLocalId);
            }
        }
    }

    void NetConnectionEntitySpawnerComponent::OnNetworkEntityControllersDeactivated(
        [[maybe_unused]] const Multiplayer::ConstNetworkEntityHandle& entityHandle,
        [[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
        if (!O3deUtils::IsHosting())
        {
            // @Christian: TODO: [todo][engine][temporary] Use the asserted version of the get entity id function once we fix the
            // engine to make sure this event gets called while the entity handle is still valid (i.e., still exists in the network entity tracker).
#if 0
            const AZ::EntityId entityLocalId = O3deUtils::GetEntityIdByNetEntityIdAsserted(entityHandle.GetNetEntityId());
#endif // #if 0

            const AZ::EntityId entityLocalId = O3deUtils::TryGetEntityIdByNetEntityId(entityHandle.GetNetEntityId());
            if (IsNetConnectionEntity(entityLocalId))
            {
                {
                    AZStd::fixed_string<256> logString;

                    logString += '`';
                    logString += __func__;
                    logString += "`: ";
                    logString += "Handling net connection entity on controllers deactivated. Removing existing net connection data from the container.";
                    logString += ' ';
                    logString += "Entity id: ";
                    logString += O3deUtils::EntityIdToString(entityLocalId);
                    logString += '.';

                    AZLOG_INFO(logString.data());
                }

                const std::size_t numRemoved = m_netConnectionDataContainer.ClientEraseElement(entityLocalId);
                AZ_Assert(numRemoved == 1u, "Expected to remove at least and exactly 1 item for the player who left.");
            }
        }
    }
#endif // #if AZ_TRAIT_CLIENT
} // namespace O3deFramework
