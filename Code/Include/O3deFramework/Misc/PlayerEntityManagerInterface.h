
#pragma once

#include <AzCore/Component/ComponentBus.h>
#include <Multiplayer/MultiplayerTypes.h>
#include <AzCore/Component/EntityId.h>
#include <Multiplayer/NetworkEntity/NetworkEntityHandle.h>

namespace Multiplayer
{
    struct NetworkSpawnable;
}

namespace O3deFramework
{
    //! @brief This determines the max supported number of players in local multiplayer / split-screen. We
    //!        give it a very generous number to stay fairly generic to any kind of game. [multiplayer[local]]
    constexpr std::size_t MaxNumPlayersPerConnection = 8;

    using PlayerNetEntityIdsVector = AZStd::fixed_vector<Multiplayer::NetEntityId, MaxNumPlayersPerConnection>;

    inline int GetNumPlayersToCreate(const AZ::EntityId& playerEntityManagerEntityId);
    inline const Multiplayer::NetworkSpawnable& GetPlayerEntitySpawnable(const AZ::EntityId& playerEntityManagerEntityId);

    inline void SetNumPlayersToCreate(const AZ::EntityId& playerEntityManagerEntityId, int newValue);
    inline void SetPlayerEntitySpawnable(const AZ::EntityId& playerEntityManagerEntityId, Multiplayer::NetworkSpawnable&& newValue);

    inline AZ::EntityId GetPlayerEntityIdByIndex(const AZ::EntityId& playerEntityManagerEntityId, std::size_t index);
    inline Multiplayer::ConstNetworkEntityHandle GetPlayerEntityNetworkHandleByIndex(const AZ::EntityId& playerEntityManagerEntityId, std::size_t index);

    class PlayerEntityManagerRequests
        : public AZ::ComponentBus
    {
    public:

        AZ_RTTI(O3deFramework::PlayerEntityManagerRequests, "{14A6C103-9C99-4B44-9B95-5CD178644BB8}");

        virtual int GetNumPlayersToCreate() const = 0;
        virtual const Multiplayer::NetworkSpawnable& GetPlayerEntitySpawnable() const = 0;

        virtual void SetNumPlayersToCreate(int newValue) = 0;
        virtual void SetPlayerEntitySpawnable(Multiplayer::NetworkSpawnable&& newValue) = 0;

        virtual AZ::EntityId GetPlayerEntityIdByIndex(std::size_t index) const = 0;
        virtual Multiplayer::ConstNetworkEntityHandle GetPlayerEntityNetworkHandleByIndex(std::size_t index) const = 0;

    private:

        // Weird version of functions which are compatible with being called by the EBus.

        friend const Multiplayer::NetworkSpawnable& GetPlayerEntitySpawnable(const AZ::EntityId&);
        friend void SetPlayerEntitySpawnable(const AZ::EntityId&, Multiplayer::NetworkSpawnable&&);

        AZ_FORCE_INLINE const Multiplayer::NetworkSpawnable* GetPlayerEntitySpawnableNotNullPtr() const
        {
            return &GetPlayerEntitySpawnable();
        }

        AZ_FORCE_INLINE void SetPlayerEntitySpawnableLvalueMoved(Multiplayer::NetworkSpawnable& newValueWhichWillBeMoved)
        {
            SetPlayerEntitySpawnable(AZStd::move(newValueWhichWillBeMoved));
        }
    };

    using PlayerEntityManagerRequestBus = AZ::EBus<PlayerEntityManagerRequests>;

    AZ_FORCE_INLINE int GetNumPlayersToCreate(const AZ::EntityId& playerEntityManagerEntityId)
    {
        int result{};
        PlayerEntityManagerRequestBus::EventResult(result, playerEntityManagerEntityId, &PlayerEntityManagerRequestBus::Events::GetNumPlayersToCreate);
        return result;
    }
    AZ_FORCE_INLINE const Multiplayer::NetworkSpawnable& GetPlayerEntitySpawnable(const AZ::EntityId& playerEntityManagerEntityId)
    {
        const Multiplayer::NetworkSpawnable* result = nullptr;
        PlayerEntityManagerRequestBus::EventResult(result, playerEntityManagerEntityId, &PlayerEntityManagerRequestBus::Events::GetPlayerEntitySpawnableNotNullPtr);
        return *result;
    }

    AZ_FORCE_INLINE void SetNumPlayersToCreate(const AZ::EntityId& playerEntityManagerEntityId, int newValue)
    {
        PlayerEntityManagerRequestBus::Event(playerEntityManagerEntityId, &PlayerEntityManagerRequestBus::Events::SetNumPlayersToCreate, newValue);
    }
    AZ_FORCE_INLINE void SetPlayerEntitySpawnable(const AZ::EntityId& playerEntityManagerEntityId, Multiplayer::NetworkSpawnable&& newValue)
    {
        PlayerEntityManagerRequestBus::Event(playerEntityManagerEntityId, &PlayerEntityManagerRequestBus::Events::SetPlayerEntitySpawnableLvalueMoved, newValue);
    }

    AZ_FORCE_INLINE AZ::EntityId GetPlayerEntityIdByIndex(const AZ::EntityId& playerEntityManagerEntityId, std::size_t index)
    {
        AZ::EntityId result{};
        PlayerEntityManagerRequestBus::EventResult(result, playerEntityManagerEntityId, &PlayerEntityManagerRequestBus::Events::GetPlayerEntityIdByIndex, index);
        return result;
    }
    AZ_FORCE_INLINE Multiplayer::ConstNetworkEntityHandle GetPlayerEntityNetworkHandleByIndex(const AZ::EntityId& playerEntityManagerEntityId, std::size_t index)
    {
        Multiplayer::ConstNetworkEntityHandle result{};
        PlayerEntityManagerRequestBus::EventResult(result, playerEntityManagerEntityId, &PlayerEntityManagerRequestBus::Events::GetPlayerEntityNetworkHandleByIndex, index);
        return result;
    }
}
