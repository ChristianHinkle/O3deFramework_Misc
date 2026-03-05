
#pragma once

#include <O3deFramework/Misc/O3deFramework_MiscTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>
#include <AzCore/Component/EntityId.h>
#include <Multiplayer/NetworkEntity/NetworkEntityHandle.h>
#include <O3deFramework/Misc/O3deFramework_MiscConfiguration.h>
#include <cstdint>
#include <AzNetworking/ConnectionLayer/IConnection.h>

namespace O3deFramework
{
    O3DEFRAMEWORK_MISC_API AZ::EntityId GetLocalNetConnectionEntityId();
    O3DEFRAMEWORK_MISC_API Multiplayer::ConstNetworkEntityHandle GetLocalNetConnectionEntityNetworkHandle();

#if AZ_TRAIT_SERVER
    O3DEFRAMEWORK_MISC_API AZ::EntityId GetNetConnectionEntityIdByMultiplayerUserId(std::uint64_t temporaryUserId);
    O3DEFRAMEWORK_MISC_API Multiplayer::ConstNetworkEntityHandle GetNetConnectionEntityNetworkHandleByMultiplayerUserId(std::uint64_t temporaryUserId);

    O3DEFRAMEWORK_MISC_API AZ::EntityId GetNetConnectionEntityIdByConnectionId(AzNetworking::ConnectionId connectionId);
    O3DEFRAMEWORK_MISC_API Multiplayer::ConstNetworkEntityHandle GetNetConnectionEntityNetworkHandleByConnectionId(AzNetworking::ConnectionId connectionId);
#endif // #if AZ_TRAIT_SERVER

    class NetConnectionEntityRequests
    {
    public:

        AZ_RTTI(NetConnectionEntityRequests, "{7BD91359-8186-4F3B-A7E4-7DE9D18F1846}");

        virtual ~NetConnectionEntityRequests() = default;

        virtual AZ::EntityId GetLocalNetConnectionEntityId() const = 0;
        virtual Multiplayer::ConstNetworkEntityHandle GetLocalNetConnectionEntityNetworkHandle() const = 0;

#if AZ_TRAIT_SERVER
        virtual AZ::EntityId GetNetConnectionEntityIdByMultiplayerUserId(std::uint64_t temporaryUserId) const = 0;
        virtual Multiplayer::ConstNetworkEntityHandle GetNetConnectionEntityNetworkHandleByMultiplayerUserId(std::uint64_t temporaryUserId) const = 0;

        virtual AZ::EntityId GetNetConnectionEntityIdByConnectionId(AzNetworking::ConnectionId connectionId) const = 0;
        virtual Multiplayer::ConstNetworkEntityHandle GetNetConnectionEntityNetworkHandleByConnectionId(AzNetworking::ConnectionId connectionId) const = 0;
#endif // #if AZ_TRAIT_SERVER
    };

    class NetConnectionEntityBusTraits
        : public AZ::EBusTraits
    {
    public:

        //! AZ::EBusTraits overrides.
        //! @{
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //! @}
    };

    using NetConnectionEntityRequestBus = AZ::EBus<NetConnectionEntityRequests, NetConnectionEntityBusTraits>;
    using NetConnectionEntityInterface = AZ::Interface<NetConnectionEntityRequests>;

}
