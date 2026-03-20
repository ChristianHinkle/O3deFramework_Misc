
#pragma once

#include <Source/AutoGen/EntityControlComponent.AutoComponent.h>
#include <Multiplayer/MultiplayerTypes.h>
#include <O3deFramework/Misc/EntityControlInterface.h>

namespace AZ
{
    class Transform;
}

namespace O3deFramework
{
    //! @class EntityControlComponentController
    class EntityControlComponentController
        : public EntityControlComponentControllerBase
    {
    public:

        explicit EntityControlComponentController(EntityControlComponent& parent);

        //! EntityControlComponentControllerBase public overrides.
        //! @{
        void OnActivate(Multiplayer::EntityIsMigrating entityIsMigrating) override;
        void OnDeactivate(Multiplayer::EntityIsMigrating entityIsMigrating) override;
        //! @}
    };

    //! @class EntityControlComponent
    //! @todo @Christian: TODO: [todo][multiplayer][contro] This is completely untested currently.
    class EntityControlComponent
        : public EntityControlComponentBase
        , protected EntityControlRequestBus::Handler
    {
    public:

        AZ_MULTIPLAYER_COMPONENT(O3deFramework::EntityControlComponent, s_entityControlComponentConcreteUuid, O3deFramework::EntityControlComponentBase);

        //! Component descriptor interface.
        //! @{
        static void Reflect(AZ::ReflectContext* context);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);
        //! @}

        //! EntityControlComponentBase public overrides.
        //! @{
        void OnActivate(Multiplayer::EntityIsMigrating entityIsMigrating) override;
        void OnDeactivate(Multiplayer::EntityIsMigrating entityIsMigrating) override;
        //! @}

    protected:

        //! EntityControlRequestBus::Handler overrides.
        //! @{
        bool ControlEntity(const AZ::EntityId& entityId) override;
        bool ControlEntity(const Multiplayer::NetEntityId& netEntityId) override;
        bool UncontrolEntity(const AZ::EntityId& entityId) override;
        bool UncontrolEntity(const Multiplayer::NetEntityId& netEntityId) override;
        //! @}

        const EntityControlComponentController* GetEntityControlComponentController() const
        {
            return static_cast<const EntityControlComponentController*>(GetController());
        }
        EntityControlComponentController* GetEntityControlComponentController()
        {
            return static_cast<EntityControlComponentController*>(GetController());
        }

#if AZ_TRAIT_SERVER
        bool TryAddToControlledNetEntityIds(const Multiplayer::NetEntityId& netEntityId);

        bool TryRemoveFromControlledNetEntityIds(const Multiplayer::NetEntityId& netEntityId);

        void HandleAddedControlledEntity(const AZ::EntityId& entityId);

        void HandleRemovedControlledEntity(const AZ::EntityId& entityId);
#endif // #if AZ_TRAIT_SERVER
    };
}
