
#include <Source/Components/EntitySpawnerComponent_OneOffAtTransforms.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <Multiplayer/NetworkEntity/INetworkEntityManager.h>
#include <Multiplayer/IMultiplayer.h>
#include <O3deUtils/Misc/MultiplayerUtils.h>
#include <AzCore/Console/ILogger.h>
#include <AzCore/Debug/Trace.h>
#include <Multiplayer/NetworkEntity/NetworkEntityHandle.h>
#include <AzCore/Component/TransformBus.h>
#include <Multiplayer/Components/NetBindComponent.h>
#include <O3deUtils/Core/AzFrameworkUtils.h>

namespace O3deFramework
{
    void SpawnableAtTransform::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SpawnableAtTransform>()
                ->Version(1)
                ->Field("Spawnable", &SpawnableAtTransform::m_spawnable)
                ->Field("SpawnTransformEntityReference", &SpawnableAtTransform::m_spawnTransformEntityReference)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<SpawnableAtTransform>("SpawnableAtTransform", "")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &SpawnableAtTransform::m_spawnable,
                        "Spawnable",
                        "The network spawnable asset which will be created at the spawn transform. Only the first entity in the prefab will be spawned.")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &SpawnableAtTransform::m_spawnTransformEntityReference,
                        "Spawn Transform Entity Reference",
                        "Reference to the entity to use as a spawn transform.")
                    ;
            }
        }
    }

    AZ_COMPONENT_IMPL(EntitySpawnerComponent_OneOffAtTransforms, "EntitySpawnerComponent_OneOffAtTransforms", "{ECF6129E-A9A4-44E7-8DA2-B54154BE8E61}");

    void EntitySpawnerComponent_OneOffAtTransforms::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<EntitySpawnerComponent_OneOffAtTransforms, AZ::Component>()
                ->Version(1)
                ->Field("SpawnableAtTransformVector", &EntitySpawnerComponent_OneOffAtTransforms::m_spawnableAtTransformVector)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<EntitySpawnerComponent_OneOffAtTransforms>("EntitySpawnerComponent_OneOffAtTransforms", "[Description of functionality provided by this component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Level")) // @Christian: TODO: [todo][spawn] Maybe support non-level entities as well. If we do that, we should be using the net entity role of the net bind component, instead of the multiplayer agent type, in the `HasAuthority` function.
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &EntitySpawnerComponent_OneOffAtTransforms::m_spawnableAtTransformVector,
                        "Spawnable To Location Vector",
                        "")
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<EntitySpawnerComponent_OneOffAtTransforms>("EntitySpawnerComponent_OneOffAtTransforms Group")
                ->Attribute(AZ::Script::Attributes::Category, "O3deFramework Gem Group")
                ;
        }

        SpawnableAtTransform::Reflect(context);
    }

    void EntitySpawnerComponent_OneOffAtTransforms::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("EntitySpawnerComponent_OneOffAtTransformsService"));
    }

    void EntitySpawnerComponent_OneOffAtTransforms::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("EntitySpawnerComponent_OneOffAtTransformsService"));
    }

    void EntitySpawnerComponent_OneOffAtTransforms::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void EntitySpawnerComponent_OneOffAtTransforms::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void EntitySpawnerComponent_OneOffAtTransforms::Activate()
    {
#if AZ_TRAIT_SERVER
        AZ_Assert(!O3deUtils::IsRootSpawnableReady(), "@Christian: We assume that the root spawnable is not ready during activation of this component. It will become ready once all the entities of the root spawnable have been created.");
        AzFramework::RootSpawnableNotificationBus::Handler::BusConnect();
#endif // #if AZ_TRAIT_SERVER
    }

    void EntitySpawnerComponent_OneOffAtTransforms::Deactivate()
    {
#if AZ_TRAIT_SERVER
        AzFramework::RootSpawnableNotificationBus::Handler::BusDisconnect();
#endif // #if AZ_TRAIT_SERVER
    }

    void EntitySpawnerComponent_OneOffAtTransforms::OnRootSpawnableAssigned([[maybe_unused]] AZ::Data::Asset<AzFramework::Spawnable> rootSpawnable, [[maybe_unused]] uint32_t generation)
    {
    }

    void EntitySpawnerComponent_OneOffAtTransforms::OnRootSpawnableReady([[maybe_unused]] AZ::Data::Asset<AzFramework::Spawnable> rootSpawnable, [[maybe_unused]] uint32_t generation)
    {
#if AZ_TRAIT_SERVER
        if (HasAuthority())
        {
            DoSpawns();
        }

        // Now stop listening for the root spawnable to be ready. We only spawn our entities once after activation of this entity and that's it.
        AzFramework::RootSpawnableNotificationBus::Handler::BusDisconnect();
#endif // #if AZ_TRAIT_SERVER
    }

    void EntitySpawnerComponent_OneOffAtTransforms::OnRootSpawnableReleased([[maybe_unused]] uint32_t generation)
    {
    }

#if AZ_TRAIT_SERVER
    bool EntitySpawnerComponent_OneOffAtTransforms::HasAuthority() const
    {
        return O3deUtils::IsHosting();
    }

    void EntitySpawnerComponent_OneOffAtTransforms::DoSpawns() const
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

        constexpr Multiplayer::NetEntityRole netEntityRole = Multiplayer::NetEntityRole::Authority;

        for (const SpawnableAtTransform& spawnableAtTransform : m_spawnableAtTransformVector)
        {
            const AZ::Data::Asset<AzFramework::Spawnable>& spawnableAsset = spawnableAtTransform.m_spawnable.m_spawnableAsset;

            // Only spawn a single entity from the prefab, as this system expects only one entity.
            Multiplayer::PrefabEntityId prefabEntityId = O3deUtils::MakeSinglePrefabEntityIdFromSpawnableAsset(spawnableAsset);

            AZ::Transform spawnTransform{};
            AZ::TransformBus::EventResult(spawnTransform, spawnableAtTransform.m_spawnTransformEntityReference, &AZ::TransformBus::Events::GetWorldTM);

            {
                AZStd::fixed_string<256> logString;

                logString += '`';
                logString += __func__;
                logString += "`: ";
                logString += "Spawning prefab '";
                logString += prefabEntityId.m_prefabName.GetStringView();
                logString += "'.";
                // TODO: Also print out the transform it's spawning it at, and make a nice function to create a string from a transform.

                AZLOG_INFO(logString.data());
            }

            Multiplayer::INetworkEntityManager::EntityList createdEntities =
                O3deUtils::GetNetworkEntityManagerAsserted().CreateEntitiesImmediate(
                    AZStd::move(prefabEntityId),
                    netEntityRole,
                    spawnTransform);

            if (createdEntities.empty())
            {
                AZStd::fixed_string<256> logString;

                logString += '`';
                logString += __func__;
                logString += "`: ";
                logString += "Attempt to spawn prefab '";
                logString += spawnableAsset.GetHint();
                logString += "' failed. No entities were spawned.";
                logString += ' ';
                logString += "Ensure that the prefab contains a single entity that is network enabled with a network binding component.";

                AZLOG_ERROR(logString.data());
            }
        }
    }
#endif // #if AZ_TRAIT_SERVER
}
