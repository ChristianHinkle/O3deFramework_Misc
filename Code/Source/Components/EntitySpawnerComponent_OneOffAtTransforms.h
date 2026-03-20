
#pragma once

#include <AzCore/Component/Component.h>
#include <Multiplayer/IMultiplayerSpawner.h>
#include <Multiplayer/MultiplayerTypes.h>
#include <AzCore/Math/Transform.h>
#include <AzFramework/Spawnable/RootSpawnableInterface.h>

namespace O3deFramework
{
    struct SpawnableAtTransform
    {
        AZ_TYPE_INFO(O3deFramework::SpawnableAtTransform, "{31CD4C3F-43E4-4957-AA0B-56D933ECF43E}");

        Multiplayer::NetworkSpawnable m_spawnable{};
        AZ::EntityId m_spawnTransformEntityReference{};

        static void Reflect(AZ::ReflectContext* context);
    };

    //! @brief
    class EntitySpawnerComponent_OneOffAtTransforms
        : public AZ::Component
        , protected AzFramework::RootSpawnableNotificationBus::Handler
    {
    public:

        AZ_COMPONENT_DECL(EntitySpawnerComponent_OneOffAtTransforms);

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

        //! AzFramework::RootSpawnableNotificationBus::Handler protected overrides.
        //! @{
        void OnRootSpawnableAssigned(AZ::Data::Asset<AzFramework::Spawnable> rootSpawnable, uint32_t generation) override;
        void OnRootSpawnableReady(AZ::Data::Asset<AzFramework::Spawnable> rootSpawnable, uint32_t generation) override;
        void OnRootSpawnableReleased(uint32_t generation) override;
        //! @}

#if AZ_TRAIT_SERVER
        bool HasAuthority() const;
        void DoSpawns() const;
#endif // #if AZ_TRAIT_SERVER

    private:

        AZStd::vector<SpawnableAtTransform> m_spawnableAtTransformVector;
    };
}
