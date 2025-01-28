#include "pch.h"

#include "Demuxer.h"

#include "Logger.h"
#include "CommonConsts.h"
#include "Misc.h"
#include "PacketTools.h"

namespace Fcore
{

    void Demuxer::Reset()
    {
        m_Streams.Reset();
        ResetPosition();
        m_Cntr = 0;
    }

    void Demuxer::ResetPosition()
    {
        if (Base::CanSeekFmt())
        {
            int flag = AVSEEK_FLAG_BACKWARD;
            int res = Base::SeekFrame(-1, 0, flag);
            if (res >= 0)
            {
                LOG_INFO(__FUNCTION__);
            }
            Base::Flush();
        }
    }

    int Demuxer::SetStreamToSeek(int idx)
    {
        if (Base::CanSeekFmt() && m_Streams.IndexIsValid(idx))
        {
            m_StrmToSeek = idx;
        }
        return m_StrmToSeek;
    }

    int Demuxer::SetPosition(double posSec)
    {
        int ret = ErrorBadState;
        if (CanSeekFmt() && m_Streams.IndexIsValid(m_StrmToSeek))
        {
            const StreamReader& strm = m_Streams[m_StrmToSeek];
            int64_t posTs = strm.PosSecToPosTs(posSec);
            int flag = AVSEEK_FLAG_BACKWARD;
            ret = Base::SeekFrame(m_StrmToSeek, posTs, flag);
            if (ret >= 0)
            {
                LOG_INFO("%s, strmIdx=%d (%s), pos(sec/ts)=%5.3lf/%lld", __FUNCTION__, 
                    m_StrmToSeek, strm.MediaTypeStr(), posSec, posTs);
            }
        }
        else
        {
            LOG_ERROR("%s, cannot seek: strmIdx=%d, pos(sec)=%8.3lf", __FUNCTION__, m_StrmToSeek, posSec);
        }
        return ret;
    }

} // namespace Fcore

//#define AVSEEK_FLAG_BACKWARD 1 ///< seek backward
//#define AVSEEK_FLAG_BYTE     2 ///< seeking based on position in bytes
//#define AVSEEK_FLAG_ANY      4 ///< seek to any frame, even non-keyframes
//#define AVSEEK_FLAG_FRAME    8 ///< seeking based on frame number
//

//int av_seek_frame(AVFormatContext *   s,
//  int     stream_index,
//  int64_t     timestamp,
//  int     flags
//  )
//
//  Seek to the keyframe at timestamp.
//
//  'timestamp' in 'stream_index'.
//
//  Parameters
//  s   media file handle
//  stream_index    If stream_index is(-1), a default stream is selected, and timestamp is automatically converted from AV_TIME_BASE units to the stream specific time_base.
//  timestamp   Timestamp in AVStream.time_base units or, if no stream is specified, in AV_TIME_BASE units.
//  flags   flags which select direction and seeking mode
//
//  Returns
//  >= 0 on success
//
//  Definition at line 2109 of file utils.c.
//
//  Referenced by avformat_seek_file(), libquvi_read_seek(), movie_common_init(), open_input_stream(), and rewind_file().
//  int avformat_seek_file(AVFormatContext *    s,
//  int     stream_index,
//  int64_t     min_ts,
//  int64_t     ts,
//  int64_t     max_ts,
//  int     flags
//  )
//
//  Seek to timestamp ts.
//
//  Seeking will be done so that the point from which all active streams can be presented successfully will be closest to ts and within min / max_ts.Active streams are all streams that have AVStream.discard < AVDISCARD_ALL.
//
//  If flags contain AVSEEK_FLAG_BYTE, then all timestamps are in bytes and are the file position(this may not be supported by all demuxers).If flags contain AVSEEK_FLAG_FRAME, then all timestamps are in frames in the stream with stream_index(this may not be supported by all demuxers).Otherwise all timestamps are in units of the stream selected by stream_index or if stream_index is - 1, in AV_TIME_BASE units.If flags contain AVSEEK_FLAG_ANY, then non - keyframes are treated as keyframes(this may not be supported by all demuxers).If flags contain AVSEEK_FLAG_BACKWARD, it is ignored.
//
//  Parameters
//  s   media file handle
//  stream_index    index of the stream which is used as time base reference
//  min_ts  smallest acceptable timestamp
//  ts  target timestamp
//  max_ts  largest acceptable timestamp
//  flags   flags
//
//  Returns
//  >= 0 on success, error code otherwise
//
//  Note
//  This is part of the new seek API which is still under construction.Thus do not use this yet.It may change at any time, do not expect ABI compatibility yet!
//
//  Definition at line 2132 of file utils.c.
//
//  Referenced by av_seek_frame(), main(), open_input_file(), read_interval_packets(), read_thread(), seek_subtitle(), and try_seek().

