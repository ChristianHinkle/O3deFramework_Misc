
#include <AzCore/Memory/Memory_fwd.h>
#include <AzCore/Module/Module.h>
#include <AzCore/RTTI/RTTIMacros.h>
#include <AzCore/RTTI/TypeInfoSimple.h>
#include <O3deFramework/Misc/O3deFramework_MiscConfiguration.h>

namespace O3deFramework_Misc
{
    class O3DEFRAMEWORK_MISC_API O3deFramework_MiscModuleInterface
        : public AZ::Module
    {
    public:
        AZ_TYPE_INFO_WITH_NAME_DECL_API(O3DEFRAMEWORK_MISC_API, O3deFramework_MiscModuleInterface)
        AZ_RTTI_NO_TYPE_INFO_DECL()
        AZ_CLASS_ALLOCATOR_DECL

        O3deFramework_MiscModuleInterface();

        AZ::ComponentTypeList GetRequiredSystemComponents() const override;
    };
}// namespace O3deFramework_Misc
