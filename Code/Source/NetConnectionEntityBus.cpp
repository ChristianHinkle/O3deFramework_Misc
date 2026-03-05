
#include <O3deFramework/Misc/NetConnectionEntityBus.h>

namespace O3deFramework
{
    AZ::EntityId GetLocalNetConnectionEntityId()
    {
        NetConnectionEntityRequests* currentNetConnectionEntityInterface = AZ::Interface<NetConnectionEntityRequests>::Get();
        if (!currentNetConnectionEntityInterface)
        {
            return AZ::EntityId{};
        }

        return currentNetConnectionEntityInterface->GetLocalNetConnectionEntityId();
    }

    Multiplayer::ConstNetworkEntityHandle GetLocalNetConnectionEntityNetworkHandle()
    {
        NetConnectionEntityRequests* currentNetConnectionEntityInterface = AZ::Interface<NetConnectionEntityRequests>::Get();
        if (!currentNetConnectionEntityInterface)
        {
            return {};
        }

        return currentNetConnectionEntityInterface->GetLocalNetConnectionEntityNetworkHandle();
    }

#if AZ_TRAIT_SERVER
    AZ::EntityId GetNetConnectionEntityIdByMultiplayerUserId(std::uint64_t temporaryUserId)
    {
        NetConnectionEntityRequests* currentNetConnectionEntityInterface = AZ::Interface<NetConnectionEntityRequests>::Get();
        if (!currentNetConnectionEntityInterface)
        {
            return AZ::EntityId{};
        }

        return currentNetConnectionEntityInterface->GetNetConnectionEntityIdByMultiplayerUserId(temporaryUserId);
    }

    Multiplayer::ConstNetworkEntityHandle GetNetConnectionEntityNetworkHandleByMultiplayerUserId(std::uint64_t temporaryUserId)
    {
        NetConnectionEntityRequests* currentNetConnectionEntityInterface = AZ::Interface<NetConnectionEntityRequests>::Get();
        if (!currentNetConnectionEntityInterface)
        {
            return {};
        }

        return currentNetConnectionEntityInterface->GetNetConnectionEntityNetworkHandleByMultiplayerUserId(temporaryUserId);
    }

    AZ::EntityId GetNetConnectionEntityIdByConnectionId(AzNetworking::ConnectionId connectionId)
    {
        NetConnectionEntityRequests* currentNetConnectionEntityInterface = AZ::Interface<NetConnectionEntityRequests>::Get();
        if (!currentNetConnectionEntityInterface)
        {
            return AZ::EntityId{};
        }

        return currentNetConnectionEntityInterface->GetNetConnectionEntityIdByConnectionId(connectionId);
    }

    Multiplayer::ConstNetworkEntityHandle GetNetConnectionEntityNetworkHandleByConnectionId(AzNetworking::ConnectionId connectionId)
    {
        NetConnectionEntityRequests* currentNetConnectionEntityInterface = AZ::Interface<NetConnectionEntityRequests>::Get();
        if (!currentNetConnectionEntityInterface)
        {
            return {};
        }

        return currentNetConnectionEntityInterface->GetNetConnectionEntityNetworkHandleByConnectionId(connectionId);
    }
#endif // #if AZ_TRAIT_SERVER
}
