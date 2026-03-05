
#pragma once

#include <O3deFramework/Misc/O3deFramework_MiscTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace O3deFramework
{
    class O3deFramework_MiscRequests
    {
    public:
        AZ_RTTI(O3deFramework_MiscRequests, O3deFramework_MiscRequestsTypeId);
        virtual ~O3deFramework_MiscRequests() = default;
        // Put your public methods here
    };

    class O3deFramework_MiscBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using O3deFramework_MiscRequestBus = AZ::EBus<O3deFramework_MiscRequests, O3deFramework_MiscBusTraits>;
    using O3deFramework_MiscInterface = AZ::Interface<O3deFramework_MiscRequests>;

}
