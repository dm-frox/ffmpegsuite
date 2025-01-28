using System;

using FFmpegNetAux;
using FFmpegInterop;

namespace AppTools
{
    public static class FFmpegConfig
    {
        static LogLevel ToLogLevel(int logLevel)
        {
            return (logLevel < 0)
                ? LogLevel.None
                : ((logLevel > (int)LogLevel.Trace) ? LogLevel.Info : (LogLevel)logLevel);
        }

        static LogOptions ToLogOptions(int logOptions)
        {
            return ((logOptions < 0) || logOptions > (int)LogOptions.All) ? LogOptions.None : (LogOptions)logOptions;
        }

        public static bool Init(StartParams startParams, string logParamsFileName, string logFileName, bool enableLogging)
        {
            bool ret = false;

            try
            {
                Core.Initialize(startParams.FFmpegWrapFolder, startParams.FFmpegFolder);

                LogNet.PrintInfo("FFmpegConfig.Init, initialization OK, platform={0}, folders (wrap/ffmpeg): {1} / {2}",
                    StartParams.PlatformTag, startParams.FFmpegWrapFolder, startParams.FFmpegFolder);

                if (enableLogging)
                {
                    bool rd = PrmLog.Read(startParams.GetParamsFilePath(logParamsFileName),
                        out int wrapperLogLev, out int ffmpegLogLev, out int options);
                    var wrLev = ToLogLevel(wrapperLogLev);
                    var opts = ToLogOptions(options);
                    if (Core.OpenLog(wrLev, startParams.GetLogFilePath(logFileName), opts, 0, 0))
                    {
                        var ffLev = ToLogLevel(ffmpegLogLev);
                        Core.EnableFFmpegLogging(ffLev);

                        LogNet.PrintInfo("FFmpegConfig.Init, log levels (wrap/ffmpeg)={0}/{1}, opts={2}, read={3}",
                            wrLev, ffLev, opts, rd);
                    }
                    else
                    {
                        LogNet.PrintError("FFmpegConfig.Init, OpenWrapLog");
                    }
                }
                ret = true;
            }
            catch (Exception exc)
            {
                LogNet.PrintExc(exc, "FFmpegConfig.Init");
                LogNet.PrintError("FFmpegConfig.Init, folders (wrap/ffmpeg): {0} / {1}", 
                    startParams.FFmpegWrapFolder, startParams.FFmpegFolder);

            }
            return ret;
        }


    } // class FFmpegConfig

} // namespace AppTools
