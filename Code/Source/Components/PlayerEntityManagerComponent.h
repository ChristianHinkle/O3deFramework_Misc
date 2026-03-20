
#pragma once

#include <Source/AutoGen/PlayerEntityManagerComponent.AutoComponent.h>
#include <Multiplayer/MultiplayerTypes.h>
#include <O3deFramework/Misc/PlayerEntityManagerInterface.h>

namespace AZ
{
    class Transform;
}

namespace O3deFramework
{
    class PlayerEntityManagerComponentController;

    //! @class PlayerEntityManagerComponent
    //! @brief Spawns a specified number of player-representitive entities. Note that these do not persist across level loads. Assume
    //!        that they will be destroyed and recreated across level trasitions and store your persistent player data externally from
    //!        these player entities. For now, I plan to use these entities solely for the purpose of replicating data through multiplayer
    //!        components. I'll try to keep actual player data and player functionality decoupled from the entity.
    //!
    //!        An important purpose of this component is to to support local multiplayer / split-screen, by allowing multiple players to
    //!        exist per network connection. E.g., I'm using this in combination with the `NetConnectionEntitySpawnerComponent`. I have that
    //!        spawning a prefab with this player entity manager on it, which then spawns individual instances of "players".
    //!
    //!        For multiplayer, you probably want to make sure this entity has a `NetworkHierarchyRootComponent` on it, and make sure the player
    //!        entities have a `NetworkHierarchyChildComponent` on them.
    class PlayerEntityManagerComponent
        : public PlayerEntityManagerComponentBase
        , protected PlayerEntityManagerRequestBus::Handler
    {
    public:

        // Necessary for the controller to access our members. We are okay with the component class and controller class being fairly
        // coupled together. Alternatively, we could have the controller class use the event bus API to access what it needs from this
        // component. Feel free to change it to that way and eliminate this friend coupling.
        friend PlayerEntityManagerComponentController;

        AZ_MULTIPLAYER_COMPONENT(O3deFramework::PlayerEntityManagerComponent, s_playerEntityManagerComponentConcreteUuid, O3deFramework::PlayerEntityManagerComponentBase);

        //! Component descriptor interface.
        //! @{
        static void Reflect(AZ::ReflectContext* context);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);
        //! @}

        //! PlayerEntityManagerComponentBase public overrides.
        //! @{
        void OnActivate(Multiplayer::EntityIsMigrating entityIsMigrating) override;
        void OnDeactivate(Multiplayer::EntityIsMigrating entityIsMigrating) override;
        //! @}

    protected:

        //! PlayerEntityManagerRequestBus::Handler overrides.
        //! @{
        int GetNumPlayersToCreate() const override;
        const Multiplayer::NetworkSpawnable& GetPlayerEntitySpawnable() const override;
        void SetNumPlayersToCreate(int newValue) override;
        void SetPlayerEntitySpawnable(Multiplayer::NetworkSpawnable&& newValue) override;
        AZ::EntityId GetPlayerEntityIdByIndex(std::size_t index) const override;
        Multiplayer::ConstNetworkEntityHandle GetPlayerEntityNetworkHandleByIndex(std::size_t index) const override;
        //! @}

    private:

        Multiplayer::NetworkSpawnable m_playerEntitySpawnable{};

        int m_numPlayersToCreate = 1;
    };

    //! @class PlayerEntityManagerComponentController
    class PlayerEntityManagerComponentController
        : public PlayerEntityManagerComponentControllerBase
    {
    public:

        explicit PlayerEntityManagerComponentController(PlayerEntityManagerComponent& parent);

        //! PlayerEntityManagerComponentControllerBase public overrides.
        //! @{
        void OnActivate(Multiplayer::EntityIsMigrating entityIsMigrating) override;
        void OnDeactivate(Multiplayer::EntityIsMigrating entityIsMigrating) override;
        //! @}

    protected:

#if AZ_TRAIT_SERVER
        void CreatePlayerEntities();
#endif // #if AZ_TRAIT_SERVER

#if AZ_TRAIT_SERVER
        static Multiplayer::NetworkEntityHandle CreatePlayerEntity(
            const Multiplayer::PrefabEntityId& playerEntityPrefab);
#endif // #if AZ_TRAIT_SERVER

        AZ_INLINE ComponentType& GetOwnerCasted()
        {
            return static_cast<ComponentType&>(GetOwner());
        }
    };
}
