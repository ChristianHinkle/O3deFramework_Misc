
#pragma once

#include <AzCore/Component/ComponentBus.h>
#include <Multiplayer/MultiplayerTypes.h>

namespace Multiplayer
{
    struct NetworkSpawnable;
}

namespace O3deFramework
{
    using ControlledNetEntityIdsVector = AZStd::vector<Multiplayer::NetEntityId>;

    namespace EntityControlRequestFunctions
    {
        inline bool ControlEntity(const AZ::EntityId& controllerEntityId, const AZ::EntityId& entityId);
        inline bool ControlEntity(const AZ::EntityId& controllerEntityId, const Multiplayer::NetEntityId& netEntityId);

        inline bool UncontrolEntity(const AZ::EntityId& controllerEntityId, const AZ::EntityId& entityId);
        inline bool UncontrolEntity(const AZ::EntityId& controllerEntityId, const Multiplayer::NetEntityId& netEntityId);
    }

    class EntityControlRequests
        : public AZ::ComponentBus
    {
    public:

        AZ_RTTI(O3deFramework::EntityControlRequests, "{583D8F75-A79D-40C4-AFC2-29A437C72563}");

        virtual bool ControlEntity(const AZ::EntityId& entityId) = 0;
        virtual bool ControlEntity(const Multiplayer::NetEntityId& netEntityId) = 0;

        virtual bool UncontrolEntity(const AZ::EntityId& entityId) = 0;
        virtual bool UncontrolEntity(const Multiplayer::NetEntityId& netEntityId) = 0;
    };

    using EntityControlRequestBus = AZ::EBus<EntityControlRequests>;

    namespace EntityControlRequestFunctions
    {
        AZ_FORCE_INLINE bool ControlEntity(const AZ::EntityId& controllerEntityId, const AZ::EntityId& entityId)
        {
            bool result{};
            EntityControlRequestBus::EventResult(
                result,
                controllerEntityId,
                static_cast<bool (EntityControlRequestBus::Events::*)(const AZ::EntityId&)>(
                    &EntityControlRequestBus::Events::ControlEntity),
                entityId
            );
            return result;
        }
        AZ_FORCE_INLINE bool ControlEntity(const AZ::EntityId& controllerEntityId, const Multiplayer::NetEntityId& netEntityId)
        {
            bool result{};
            EntityControlRequestBus::EventResult(
                result,
                controllerEntityId,
                static_cast<bool (EntityControlRequestBus::Events::*)(const Multiplayer::NetEntityId&)>(
                    &EntityControlRequestBus::Events::ControlEntity),
                netEntityId
            );
            return result;
        }

        AZ_FORCE_INLINE bool UncontrolEntity(const AZ::EntityId& controllerEntityId, const AZ::EntityId& entityId)
        {
            bool result{};
            EntityControlRequestBus::EventResult(
                result,
                controllerEntityId,
                static_cast<bool (EntityControlRequestBus::Events::*)(const AZ::EntityId&)>(
                    &EntityControlRequestBus::Events::UncontrolEntity),
                entityId
            );
            return result;
        }
        AZ_FORCE_INLINE bool UncontrolEntity(const AZ::EntityId& controllerEntityId, const Multiplayer::NetEntityId& netEntityId)
        {
            bool result{};
            EntityControlRequestBus::EventResult(
                result,
                controllerEntityId,
                static_cast<bool (EntityControlRequestBus::Events::*)(const Multiplayer::NetEntityId&)>(
                    &EntityControlRequestBus::Events::UncontrolEntity),
                netEntityId
            );
            return result;
        }
    }
}
