
#pragma once

#include <Source/O3deFramework_MiscSystemComponentBase.h>
#include <AzToolsFramework/API/ToolsApplicationAPI.h>

namespace O3deFramework
{
    class O3deFramework_MiscEditorSystemComponent
        : public O3deFramework_MiscSystemComponentBase
        , protected AzToolsFramework::EditorEvents::Bus::Handler
    {
        using BaseSystemComponent = O3deFramework_MiscSystemComponentBase;
    public:
        AZ_COMPONENT_DECL(O3deFramework_MiscEditorSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        O3deFramework_MiscEditorSystemComponent();
        ~O3deFramework_MiscEditorSystemComponent();

    protected:

        void Activate() override;
        void Deactivate() override;
    };
}
