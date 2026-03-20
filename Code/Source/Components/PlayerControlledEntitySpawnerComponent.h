
#pragma once

#include <AzCore/Component/Component.h>
#include <Multiplayer/IMultiplayerSpawner.h>
#include <Multiplayer/MultiplayerTypes.h>
#include <AzCore/Math/Transform.h>

namespace O3deFramework
{
    //! @brief A simple utility that spawns controlled enities for new players that join in.
    class PlayerControlledEntitySpawnerComponent
        : public AZ::Component
    {
    public:

        AZ_COMPONENT_DECL(PlayerControlledEntitySpawnerComponent);

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

#if AZ_TRAIT_SERVER
        bool HasAuthority() const;
        void SpawnForPlayer(const AZ::EntityId& playerEntityId) const;
#endif // #if AZ_TRAIT_SERVER

        //! @brief Pull the transform to spawn at from the spawn transform entity reference.
        AZ::Transform GetSpawnTransformFromEntityReference() const;

        void OnNetConnectionAdded(const Multiplayer::ConstNetworkEntityHandle& entityNetworkHandle, const AZ::EntityId& entityId);

    private:

        AZ::Event<const Multiplayer::ConstNetworkEntityHandle&, const AZ::EntityId&>::Handler m_onNetConnectionAddedHandler{
            [this](const Multiplayer::ConstNetworkEntityHandle& entityNetworkHandle, const AZ::EntityId& entityId)
            {
                OnNetConnectionAdded(entityNetworkHandle, entityId);
            }
        };

        // @Christian: TODO: [todo] Store a data structure containing the player event handlers per net connection.

        //AZStd::vector<AZ::Event<const Multiplayer::ConstNetworkEntityHandle&, const AZ::EntityId&>::Handler> m_

        Multiplayer::NetworkSpawnable m_spawnable{};
        AZ::EntityId m_spawnTransformEntityReference{};
    };
}
