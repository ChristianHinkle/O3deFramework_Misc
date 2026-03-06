
#pragma once

#include <Source/O3deFramework_MiscSystemComponentBase.h>

namespace O3deFramework
{
    class O3deFramework_MiscSystemComponent
        : public O3deFramework_MiscSystemComponentBase
    {
        using BaseSystemComponent = O3deFramework_MiscSystemComponentBase;
    public:
        AZ_COMPONENT_DECL(O3deFramework_MiscSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        O3deFramework_MiscSystemComponent();
        ~O3deFramework_MiscSystemComponent();

    protected:

        void Activate() override;
        void Deactivate() override;
    };
}
