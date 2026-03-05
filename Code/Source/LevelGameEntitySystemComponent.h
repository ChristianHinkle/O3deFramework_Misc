
#pragma once

#include <AzCore/Component/Component.h>

#include <O3deFramework/Misc/LevelGameEntityBus.h>
#include <AzFramework/Spawnable/RootSpawnableInterface.h>
#include <Multiplayer/NetworkEntity/NetworkEntityHandle.h>
#include <Multiplayer/MultiplayerTypes.h>
#include <Multiplayer/NetworkEntity/INetworkEntityManager.h>
#include <LmbrCentral/Scripting/TagComponentBus.h>
#include <AzCore/Component/EntityId.h>
#include <O3deUtils/Misc/NetworkEntityHandleWithId.h>

namespace O3deFramework
{
    bool IsLevelGameEntity(const AZ::EntityId& entityId);

    // @Christian: TODO: [todo][techdebt][tag] Use a proper "gameplay tag" system instead of LmbrCentral's tag component.
    constexpr LmbrCentral::Tag LevelGameEntityTag = AZ_CRC_CE("O3deFramework.Misc.LevelGameEntity");

    //! @brief
    class LevelGameEntitySystemComponent
        : public AZ::Component
        , protected LevelGameEntityRequestBus::Handler
        , protected AzFramework::RootSpawnableNotificationBus::Handler
    {
    public:

        AZ_COMPONENT_DECL(LevelGameEntitySystemComponent);

        LevelGameEntitySystemComponent();
        ~LevelGameEntitySystemComponent() override;

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
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        //! @}

        //! LevelGameEntityRequestBus::Handler protected overrides.
        //! @{
        AZ::EntityId GetLevelGameEntityId() const override;
        Multiplayer::ConstNetworkEntityHandle GetLevelGameEntityNetworkHandle() const override;
        //! @}

        //! AzFramework::RootSpawnableNotificationBus::Handler protected overrides.
        //! @{
        void OnRootSpawnableAssigned(AZ::Data::Asset<AzFramework::Spawnable> rootSpawnable, uint32_t generation) override;
        void OnRootSpawnableReady(AZ::Data::Asset<AzFramework::Spawnable> rootSpawnable, uint32_t generation) override;
        void OnRootSpawnableReleased(uint32_t generation) override;
        //! @}

        void OnNetworkEntityControllersActivated(const Multiplayer::ConstNetworkEntityHandle& entityHandle, Multiplayer::EntityIsMigrating entityIsMigrating);
        void OnNetworkEntityControllersDeactivated(const Multiplayer::ConstNetworkEntityHandle& entityHandle, Multiplayer::EntityIsMigrating entityIsMigrating);

#if AZ_TRAIT_SERVER
        bool HasAuthority() const;
        void SpawnNewLevelGameEntity();
        Multiplayer::NetworkEntityHandle CreateLevelGameEntity() const;
#endif // #if AZ_TRAIT_SERVER

    private:

        O3deUtils::ConstNetworkEntityHandleWithId GetCurrentLevelGameEntityReference()
        {
            return O3deUtils::ConstNetworkEntityHandleWithId{m_currentLevelGameEntityHandle, m_currentLevelGameEntityId};
        }

    private:

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

        // @Christian: TODO: [todo] It'd be nice if we could emit compiler errors if these encapsulated data
        // members were ever accessed directly without going through `GetCurrentLevelGameEntityReference`.
        Multiplayer::ConstNetworkEntityHandle m_currentLevelGameEntityHandle{};

        // @Christian: TODO: [todo] It'd be nice if we could emit compiler errors if these encapsulated data
        // members were ever accessed directly without going through `GetCurrentLevelGameEntityReference`.
        AZ::EntityId m_currentLevelGameEntityId{};
    };
}
