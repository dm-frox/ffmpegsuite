#include "pch.h"

#include "DeviceList.h"

#include "ffmpeg.h"
#include "Logger.h"


namespace Fcore
{
#pragma warning(disable:4706)
    // static
    int DeviceList::LogListInput()
    {
        LOG_INFO("Try to get input device list, avdevice: vers=%d", avdevice_version()); 

        const AVInputFormat *inputFmt = nullptr;
        while ((inputFmt = av_input_video_device_next(inputFmt)))
        {
            LOG_INFO(" -- device, %s (%s)", inputFmt->name, inputFmt->long_name);

            AVDeviceInfoList *devInfoList = nullptr;
            int rr = avdevice_list_input_sources(inputFmt, nullptr, nullptr, &devInfoList);
            if (rr >= 0)
            {
                int n = devInfoList->nb_devices;
                if (n > 0)
                {
                    for (int i = 0; i < n; ++i)
                    {
                        AVDeviceInfo *devInfo = devInfoList->devices[i];
                        LOG_INFO(" ---- dev info, %s (%s)", devInfo->device_name, devInfo->device_description);
                    }
                }
                else
                {
                    LOG_INFO("%s", " ---- dev info, empty");
                }
            }
            else
            {
                LOG_ERROR("%s, avdevice_list_input_sources, %s", __FUNCTION__, FmtErr(rr));
            }
        }
        return 0;
#pragma warning(default:4706)
    }

} // namespace Fcore
