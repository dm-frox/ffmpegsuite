#include "pch.h"

#include "DShowDevList.h"

#include "windefs.h"

#ifdef VISUAL_STUDIO_

#include <DShow.h>

#include "ArrSize.h"
#include "SysDevColl.h"

#endif // VISUAL_STUDIO_


namespace Ftool
{

#ifdef VISUAL_STUDIO_

    const int DefListLen = 4;
    const int DefNameLen = 256;

    const GUID* const CatGuids[] =
    {
        &CLSID_VideoInputDeviceCategory,
        &CLSID_AudioInputDeviceCategory,
    };


    DevList GetDShowDevList(int devType)
    {

        DevList devList;

        if (0 <= devType && devType < Size(CatGuids))
        {
            devList.reserve(DefListLen);

            std::wstring name;
            name.reserve(DefNameLen);
            SysDevColl sysDevColl(CatGuids[devType]);
            for (const auto& it : sysDevColl)
            {
                name.clear();
                it.GetName(name);
                devList.push_back(std::move(name));
            }
        }

        return devList;
    }

#else // unix zoo, mingw

    DevList GetDShowDevList(int devType) // stub
    {
        return DevList();
    }

#endif // VISUAL_STUDIO_

} // namespace Ftool

