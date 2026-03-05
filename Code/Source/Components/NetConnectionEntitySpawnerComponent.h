
#pragma once

#include <AzCore/Component/Component.h>
#include <Multiplayer/IMultiplayerSpawner.h>
#include <O3deFramework/Misc/NetConnectionEntityBus.h>
#include <Multiplayer/MultiplayerTypes.h>
#include <AzCore/Math/Transform.h>
#include <O3deUtils/Misc/NetworkEntityHandleWithId.h>
#include <Multiplayer/NetworkEntity/INetworkEntityManager.h>
#include <LmbrCentral/Scripting/TagComponentBus.h>
#include <span>

namespace O3deFramework
{
    bool IsNetConnectionEntity(const AZ::EntityId& entityId);

    // @Christian: TODO: [todo][techdebt][tag] Use a proper "gameplay tag" system instead of LmbrCentral's tag component.
    constexpr LmbrCentral::Tag NetConnectionEntityTag = AZ_CRC_CE("O3deFramework.Misc.NetConnectionEntity");

    struct NetConnectionSpawnedEntityData
    {
    public:

        O3deUtils::ConstNetworkEntityHandleWithId GetSpawnedEntityReference()
        {
            return O3deUtils::ConstNetworkEntityHandleWithId{m_networkHandle, m_entityId};
        }

        Multiplayer::ConstNetworkEntityHandle GetNetworkHandle() const { return m_networkHandle; }
        AZ::EntityId GetEntityId() const { return m_entityId; }

    private:

        Multiplayer::ConstNetworkEntityHandle m_networkHandle{};

        AZ::EntityId m_entityId{};
    };

    struct NetConnectionIdentificationData
    {
    public:

        std::uint64_t m_multiplayerUserId{};

        AzNetworking::ConnectionId m_connectionId{};
    };

    struct NetConnectionDataContainer
    {
    public:

#if AZ_TRAIT_SERVER
        struct ServerElementReference
        {
            NetConnectionSpawnedEntityData& m_entityData;
            NetConnectionIdentificationData& m_identificationData;
        };
#endif // #if AZ_TRAIT_SERVER

#if AZ_TRAIT_CLIENT
        struct ClientElementReference
        {
            NetConnectionSpawnedEntityData& m_entityData;
        };
#endif // #if AZ_TRAIT_CLIENT

    public:

#if AZ_TRAIT_SERVER
        ServerElementReference ServerEmplaceNewElement();
        std::size_t ServerEraseElement(const AZ::EntityId entityId);
#endif // #if AZ_TRAIT_SERVER

#if AZ_TRAIT_CLIENT
        ClientElementReference ClientEmplaceNewElement();
        std::size_t ClientEraseElement(const AZ::EntityId entityId);
#endif // #if AZ_TRAIT_CLIENT

        std::size_t FindIndexByEntityId(const AZ::EntityId entityId) const;

        std::span<const NetConnectionSpawnedEntityData> GetEntityDataSpan() const { return m_entityDataVector; }

#if AZ_TRAIT_SERVER
        std::size_t FindIndexByMultiplayerUserId(std::uint64_t temporaryUserId) const;
        std::size_t FindIndexByConnectionId(AzNetworking::ConnectionId connectionId) const;

        std::span<const NetConnectionIdentificationData> GetIdentificationDataSpan() const { return m_identificationDataVector; }
#endif // #if AZ_TRAIT_SERVER

    private:

        AZStd::vector<NetConnectionSpawnedEntityData> m_entityDataVector;

#if AZ_TRAIT_SERVER
        AZStd::vector<NetConnectionIdentificationData> m_identificationDataVector;
#endif // #if AZ_TRAIT_SERVER
    };

    //! @brief Handles spawning the "net connection entity" for new connections that join the server. The spawned entity is the root
    //!        entity of autonomous control for the net connection.
    //! @todo @Christian: [todo][multiplayer] Consider changing this to be a system component rather than a level component. I see no reason to tie its
    //!       lifetime to the level. And I think we would always want this to be active - not on a per level basis.
    class NetConnectionEntitySpawnerComponent
        : public AZ::Component
        , protected Multiplayer::IMultiplayerSpawner
        , protected NetConnectionEntityRequestBus::Handler
    {
    public:

        AZ_COMPONENT_DECL(NetConnectionEntitySpawnerComponent);

        //! Component descriptor interface.
        //! @{
        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);
        //! @}

    protected:

        //! AZ::Component protected overrides.
        //! @{
        void Activate() override;
        void Deactivate() override;
        //! @}

        //! Multiplayer::IMultiplayerSpawner protected overrides.
        //! @{
        Multiplayer::NetworkEntityHandle OnPlayerJoin(uint64_t userId, const Multiplayer::MultiplayerAgentDatum& agentDatum) override;
        void OnPlayerLeave(Multiplayer::ConstNetworkEntityHandle entityHandle, const Multiplayer::ReplicationSet& replicationSet, AzNetworking::DisconnectReason reason) override;
        //! @}

        //! NetConnectionEntityRequestBus::Handler protected overrides.
        //! @{
        AZ::EntityId GetLocalNetConnectionEntityId() const override;
        Multiplayer::ConstNetworkEntityHandle GetLocalNetConnectionEntityNetworkHandle() const override;
#if AZ_TRAIT_SERVER
        AZ::EntityId GetNetConnectionEntityIdByMultiplayerUserId(std::uint64_t temporaryUserId) const override;
        Multiplayer::ConstNetworkEntityHandle GetNetConnectionEntityNetworkHandleByMultiplayerUserId(std::uint64_t temporaryUserId) const override;
        AZ::EntityId GetNetConnectionEntityIdByConnectionId(AzNetworking::ConnectionId connectionId) const override;
        Multiplayer::ConstNetworkEntityHandle GetNetConnectionEntityNetworkHandleByConnectionId(AzNetworking::ConnectionId connectionId) const override;
#endif // #if AZ_TRAIT_SERVER
        //! @}

        NetConnectionSpawnedEntityData GetLocalNetConnectionSpawnedEntityData() const;
#if AZ_TRAIT_SERVER
        NetConnectionSpawnedEntityData GetNetConnectionSpawnedEntityDataByMultiplayerUserId(std::uint64_t temporaryUserId) const;
        NetConnectionSpawnedEntityData GetNetConnectionSpawnedEntityDataByConnectionId(AzNetworking::ConnectionId connectionId) const;
#endif // #if AZ_TRAIT_SERVER

        //! @brief Pull the transform to spawn at from the spawn transform entity reference.
        AZ::Transform GetSpawnTransformFromEntityReference() const;

#if AZ_TRAIT_CLIENT
        void OnNetworkEntityControllersActivated(const Multiplayer::ConstNetworkEntityHandle& entityHandle, Multiplayer::EntityIsMigrating entityIsMigrating);
        void OnNetworkEntityControllersDeactivated(const Multiplayer::ConstNetworkEntityHandle& entityHandle, Multiplayer::EntityIsMigrating entityIsMigrating);
#endif // #if AZ_TRAIT_CLIENT

    private:

        Multiplayer::NetworkSpawnable m_netConnectionEntitySpawnable;

#if AZ_TRAIT_CLIENT
        Multiplayer::ControllersActivatedEvent::Handler m_onNetworkEntityControllersActivatedHandler{
            [this](const Multiplayer::ConstNetworkEntityHandle& entityHandle, Multiplayer::EntityIsMigrating entityIsMigrating)
            {
                OnNetworkEntityControllersActivated(entityHandle, entityIsMigrating);
            }
        };

        Multiplayer::ControllersDeactivatedEvent::Handler m_onNetworkEntityControllersDeactivatedHandler{
            [this](const Multiplayer::ConstNetworkEntityHandle& entityHandle, Multiplayer::EntityIsMigrating entityIsMigrating)
            {
                OnNetworkEntityControllersDeactivated(entityHandle, entityIsMigrating);
            }
        };
#endif // #if AZ_TRAIT_CLIENT

        NetConnectionDataContainer m_netConnectionDataContainer;

        // @Christian: TODO: [todo][techdebt][multiplayer] This component should not be determining where the characters are spawned. Move
        // this to a different component which would handle spawning actual character entity.
        AZ::EntityId m_spawnTransformEntityReference;
    };
} // namespace O3deFramework
