
#pragma once

namespace O3deFramework_Misc
{
    // System Component TypeIds
    inline constexpr const char* O3deFramework_MiscSystemComponentTypeId = "{E691EAAA-B2BA-4D99-A24E-BF83CC4B4DAE}";
    inline constexpr const char* O3deFramework_MiscEditorSystemComponentTypeId = "{26FDBC34-5276-4B42-B957-EDB0BA9105DF}";

    // Module derived classes TypeIds
    inline constexpr const char* O3deFramework_MiscModuleInterfaceTypeId = "{10BB54CE-5A44-4E37-8B70-33E7F7186472}";
    inline constexpr const char* O3deFramework_MiscModuleTypeId = "{9459827B-EB6D-4E3D-AE91-33C60D355F4C}";
    // The Editor Module by default is mutually exclusive with the Client Module
    // so they use the Same TypeId
    inline constexpr const char* O3deFramework_MiscEditorModuleTypeId = O3deFramework_MiscModuleTypeId;

    // Interface TypeIds
    inline constexpr const char* O3deFramework_MiscRequestsTypeId = "{26D94541-F316-4953-B555-4D03AE36F202}";
} // namespace O3deFramework_Misc
