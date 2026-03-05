
#pragma once

#include <AzCore/PlatformDef.h>

#if defined(AZ_MONOLITHIC_BUILD)
#   define O3DEFRAMEWORK_MISC_API
#else
#   if defined(O3DEFRAMEWORK_MISC_EXPORTS)
#       define O3DEFRAMEWORK_MISC_API AZ_DLL_EXPORT
#   else
#       define O3DEFRAMEWORK_MISC_API AZ_DLL_IMPORT
#   endif
#endif
