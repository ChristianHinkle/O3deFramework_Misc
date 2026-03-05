
#pragma once

#include <AzToolsFramework/API/ToolsApplicationAPI.h>

#include <Source/O3deFramework_MiscSystemComponent.h>

namespace O3deFramework
{
    /// System component for O3deFramework_Misc editor
    class O3deFramework_MiscEditorSystemComponent
        : public O3deFramework_MiscSystemComponent
        , protected AzToolsFramework::EditorEvents::Bus::Handler
    {
        using BaseSystemComponent = O3deFramework_MiscSystemComponent;
    public:
        AZ_COMPONENT_DECL(O3deFramework_MiscEditorSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        O3deFramework_MiscEditorSystemComponent();
        ~O3deFramework_MiscEditorSystemComponent();

    private:
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;
    };
}
