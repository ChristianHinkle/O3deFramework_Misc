
#include <Source/LevelGameEntitySystemComponent.h>

#include <O3deFramework/Misc/O3deFramework_MiscTypeIds.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <O3deUtils/Misc/MultiplayerUtils.h>
#include <O3deUtils/Core/AzFrameworkUtils.h>
#include <AzCore/Console/ILogger.h>
#include <AzCore/Debug/Trace.h>
#include <Multiplayer/MultiplayerTypes.h>
#include <O3deFramework/Misc/SettingsRegistryAccessors.h>
#include <AzCore/Asset/AssetManagerBus.h>
#include <Multiplayer/NetworkEntity/INetworkEntityManager.h>
#include <LmbrCentral/Scripting/TagComponentBus.h>

namespace O3deFramework
{
    bool IsLevelGameEntity(const AZ::EntityId& entityId)
    {
        // @Christian: TODO: [todo][techdebt][tag] Use a proper "gameplay tag" system instead of LmbrCentral's tag component.
        bool result{};
        LmbrCentral::TagComponentRequestBus::EventResult(result, entityId, &LmbrCentral::TagComponentRequestBus::Handler::HasTag, LevelGameEntityTag);
        return result;
    }

    AZ_COMPONENT_IMPL(LevelGameEntitySystemComponent, "LevelGameEntitySystemComponent", "{FD0590C7-9F6F-4385-A74E-4FF8FD8DA656}");

    void LevelGameEntitySystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<LevelGameEntitySystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }
    }

    void LevelGameEntitySystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("LevelGameEntityService"));
    }

    void LevelGameEntitySystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("LevelGameEntityService"));
    }

    void LevelGameEntitySystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("MultiplayerService"));
    }

    void LevelGameEntitySystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC_CE("MultiplayerService"));
    }

    LevelGameEntitySystemComponent::LevelGameEntitySystemComponent()
    {
        if (LevelGameEntityInterface::Get() == nullptr)
        {
            LevelGameEntityInterface::Register(this);
        }
    }

    LevelGameEntitySystemComponent::~LevelGameEntitySystemComponent()
    {
        if (LevelGameEntityInterface::Get() == this)
        {
            LevelGameEntityInterface::Unregister(this);
        }
    }

    void LevelGameEntitySystemComponent::Init()
    {
    }

    void LevelGameEntitySystemComponent::Activate()
    {
        LevelGameEntityRequestBus::Handler::BusConnect();
        AzFramework::RootSpawnableNotificationBus::Handler::BusConnect();

        O3deUtils::GetNetworkEntityManagerAsserted().AddControllersActivatedHandler(m_onNetworkEntityControllersActivatedHandler);
        O3deUtils::GetNetworkEntityManagerAsserted().AddControllersDeactivatedHandler(m_onNetworkEntityControllersDeactivatedHandler);
    }

    void LevelGameEntitySystemComponent::Deactivate()
    {
        LevelGameEntityRequestBus::Handler::BusDisconnect();
        AzFramework::RootSpawnableNotificationBus::Handler::BusDisconnect();

        m_onNetworkEntityControllersActivatedHandler.Disconnect();
        m_onNetworkEntityControllersDeactivatedHandler.Disconnect();
    }

    AZ::EntityId LevelGameEntitySystemComponent::GetLevelGameEntityId() const
    {
        // @Christian: Remark: It might be nicer for these to go through the `GetCurrentLevelGameEntityReference` function if we made a const overload for it.
        return m_currentLevelGameEntityId;
    }

    Multiplayer::ConstNetworkEntityHandle LevelGameEntitySystemComponent::GetLevelGameEntityNetworkHandle() const
    {
        // @Christian: Remark: It might be nicer for these to go through the `GetCurrentLevelGameEntityReference` function if we made a const overload for it.
        return m_currentLevelGameEntityHandle;
    }

    void LevelGameEntitySystemComponent::OnRootSpawnableAssigned([[maybe_unused]] AZ::Data::Asset<AzFramework::Spawnable> rootSpawnable, [[maybe_unused]] uint32_t generation)
    {
    }

    void LevelGameEntitySystemComponent::OnRootSpawnableReady([[maybe_unused]] AZ::Data::Asset<AzFramework::Spawnable> rootSpawnable, [[maybe_unused]] uint32_t generation)
    {
#if AZ_TRAIT_SERVER
        if (HasAuthority())
        {
            // Whenever the root spawnable is ready (i.e., when a level has finished loading) spawn the "level game entity".
            SpawnNewLevelGameEntity();
        }
#endif // #if AZ_TRAIT_SERVER
    }

    void LevelGameEntitySystemComponent::OnRootSpawnableReleased([[maybe_unused]] uint32_t generation)
    {
    }

    void LevelGameEntitySystemComponent::OnNetworkEntityControllersActivated(
        [[maybe_unused]] const Multiplayer::ConstNetworkEntityHandle& entityHandle,
        [[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
        const AZ::EntityId entityLocalId = O3deUtils::GetEntityIdByNetEntityIdAsserted(entityHandle.GetNetEntityId());
        if (IsLevelGameEntity(entityLocalId))
        {
            {
                AZStd::fixed_string<256> logString;

                logString += '`';
                logString += __func__;
                logString += "`: ";
                logString += "Handling level game entity on controllers activated. We will set the current level game entity handle.";

                AZLOG_INFO(logString.data());
            }

            O3deUtils::ConstNetworkEntityHandleWithId currentLevelGameEntityReference = GetCurrentLevelGameEntityReference();

            if (O3deUtils::IsNetworkEntityHandleSet(currentLevelGameEntityReference.GetNetworkEntityHandle()))
            {
                // It's okay if it's already set by an earlier point. I.e., set by `SpawnNewLevelGameEntity`.
                AZ_Assert(currentLevelGameEntityReference.GetNetworkEntityHandle() == entityHandle, "A second level game entity is attempting to be used! Ignoring it.");
            }
            else
            {
                currentLevelGameEntityReference.Set(entityHandle, entityLocalId);
            }
        }
    }

    void LevelGameEntitySystemComponent::OnNetworkEntityControllersDeactivated(
        [[maybe_unused]] const Multiplayer::ConstNetworkEntityHandle& entityHandle,
        [[maybe_unused]] Multiplayer::EntityIsMigrating entityIsMigrating)
    {
        // @Christian: TODO: [todo][engine][temporary] Use the asserted version of the get entity id function once we fix the
        // engine to make sure this event gets called while the entity handle is still valid (i.e., still exists in the network entity tracker).
#if 0
        const AZ::EntityId entityLocalId = O3deUtils::GetEntityIdByNetEntityIdAsserted(entityHandle.GetNetEntityId());
#endif // #if 0

        const AZ::EntityId entityLocalId = O3deUtils::TryGetEntityIdByNetEntityId(entityHandle.GetNetEntityId());
        if (IsLevelGameEntity(entityLocalId))
        {
            {
                AZStd::fixed_string<256> logString;

                logString += '`';
                logString += __func__;
                logString += "`: ";
                logString += "Handling level game entity on controllers deactivated. We will reset the current level game entity handle.";

                AZLOG_INFO(logString.data());
            }

            GetCurrentLevelGameEntityReference().Reset();
        }
    }

#if AZ_TRAIT_SERVER
    bool LevelGameEntitySystemComponent::HasAuthority() const
    {
        return O3deUtils::IsHosting();
    }

    void LevelGameEntitySystemComponent::SpawnNewLevelGameEntity()
    {
        AZ_Assert(HasAuthority(), "Should only be called on authority.");
        AZ_Assert(!O3deUtils::IsNetworkEntityHandleSet(GetCurrentLevelGameEntityReference().GetNetworkEntityHandle()), "The current level game entity handle should not already be set.");

        {
            AZStd::fixed_string<256> logString;

            logString += '`';
            logString += __func__;
            logString += "`: ";
            logString += "Spawning new level game entity and assigning it as the current.";

            AZLOG_INFO(logString.data());
        }

        // @Christian: TODO: [todo] This assignment of the current entity references is redundant on the server.

        Multiplayer::ConstNetworkEntityHandle newLevelGameEntityHandle = CreateLevelGameEntity();
        AZ::EntityId newLevelGameEntityId = O3deUtils::TryGetEntityIdByNetEntityId(newLevelGameEntityHandle.GetNetEntityId());

        GetCurrentLevelGameEntityReference().Set(newLevelGameEntityHandle, newLevelGameEntityId);
    }

    Multiplayer::NetworkEntityHandle LevelGameEntitySystemComponent::CreateLevelGameEntity() const
    {
        AZ_Assert(HasAuthority(), "Should only be called on authority.");

        {
            AZStd::fixed_string<256> logString;

            logString += '`';
            logString += __func__;
            logString += "`: ";
            logString += "Spawning entities.";

            AZLOG_INFO(logString.data());
        }

        AZ_Assert(O3deUtils::IsRootSpawnableReady(), "The level should be fully loaded at this point, because we are about to sample the transform from an entity reference from the level.");

        AZStd::optional<AZ::Data::Asset<AzFramework::Spawnable>> spawnableAsset = SettingsRegistryAccessors::TryGetLevelGameEntityPrefabAsset();
        if (!spawnableAsset.has_value())
        {
            AZStd::fixed_string<256> logString;

            logString += '`';
            logString += __func__;
            logString += "`: ";
            logString += "No spawnable asset found from settings registry. Returning invalid entity handle.";

            AZLOG_INFO(logString.data());
            return {};
        }

        constexpr Multiplayer::NetEntityRole netEntityRole = Multiplayer::NetEntityRole::Authority;

        // Only spawn a single entity from the prefab, as this system expects only one entity.
        Multiplayer::PrefabEntityId prefabEntityId = O3deUtils::MakeSinglePrefabEntityIdFromSpawnableAsset(*spawnableAsset);

        {
            AZStd::fixed_string<256> logString;

            logString += '`';
            logString += __func__;
            logString += "`: ";
            logString += "Spawning prefab '";
            logString += prefabEntityId.m_prefabName.GetStringView();
            logString += "'.";

            AZLOG_INFO(logString.data());
        }

        Multiplayer::INetworkEntityManager::EntityList createdEntities =
            O3deUtils::GetNetworkEntityManagerAsserted().CreateEntitiesImmediate(
                AZStd::move(prefabEntityId),
                netEntityRole,
                AZ::Transform::Identity());

        if (createdEntities.empty())
        {
            AZStd::fixed_string<256> logString;

            logString += '`';
            logString += __func__;
            logString += "`: ";
            logString += "Attempt to spawn prefab '";
            logString += spawnableAsset->GetHint();
            logString += "' failed. No entities were spawned.";
            logString += ' ';
            logString += "Ensure that the prefab contains a single entity that is network enabled with a network binding component.";

            AZLOG_ERROR(logString.data());
            return {};
        }

        Multiplayer::NetworkEntityHandle& spawnedEntityHandle = createdEntities[0];

        const AZ::Entity* spawnedEntity = spawnedEntityHandle.GetEntity();
        if (!spawnedEntity)
        {
            AZStd::fixed_string<256> logString;

            logString += '`';
            logString += __func__;
            logString += "`: ";
            logString += "The spawned entity is null.";

            AZLOG_ERROR(logString.data());
        }

        return AZStd::move(spawnedEntityHandle);
    }
#endif // #if AZ_TRAIT_SERVER
}
