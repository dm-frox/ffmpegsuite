#include "pch.h"

#include "OptionTools.h"

#include "ffmpeg.h"
#include "Misc.h"
#include "Logger.h"
#include "StrBuilder.h"

namespace
{
#pragma warning(disable:4706)
    template<typename F>
    void ForEachOpt(const AVClass* avc, F f)
    {
        if (avc)
        {
            const AVOption* opt = nullptr;
            while (opt = av_opt_next(&avc, opt))
            {
                f(opt);
            }
        }
#pragma warning(default:4706)
    }

} // namespace anon

namespace Fcore
{
    class OptsStr : public StrBuilder
    {
        static const char ColSepr;
        static const char RowSepr;

    public:
        explicit OptsStr(int capacity);
        ~OptsStr();

        void SetPrefix(const char* prefix);
        void SetPrefixEx(const char* prefix);
        void SetPrefixEx(const char* name, const char* nameType, const char* prefix);

        void AddOptionEx(const AVOption* opt);

    private:
        void AddItemEx(const char* item);
        void AddRowSepr();

        static const char* TypeToStr(const AVOption* opt);
        static const char* DefValToStr(const AVOption* opt);

        static const char* Str_(const char* s);

    }; // class OptsStr


    //static 
    std::string OptionTools::OptionsToStr(const AVClass* avc)
    {
        OptsStr str(128);
        ForEachOpt(avc, [&str](const AVOption* opt) { str.Append(opt->name); });
        return str.Str();
    }

    //static 
    std::string OptionTools::OptionsToStrEx(const AVClass* avc, const char* name, const char* nameType)
    {
        OptsStr str(4 * 1024);
        str.SetPrefixEx(name, nameType, "options");
        ForEachOpt(avc, [&str](const AVOption* opt) { str.AddOptionEx(opt); });
        return str.Str();
    }

    //static 
    void OptionTools::LogOptions(const char* txt, const AVClass* avc)
    {
        std::string str = OptionsToStr(avc);
        LOG_INFO("%s, options: %s", txt, str.c_str());
    }

    //static 
    void OptionTools::LogOptionsEx(const char* txt, const char* name, const char* nameType, const AVClass* avc)
    {
        std::string str = OptionsToStrEx(avc, name, nameType);
        LOG_INFO("%s, %s", txt, str.c_str());
    }

    //static 
    const AVClass* OptionTools::GetContextClass(const void* ctx)  // dangerous !!
    {
        struct ContextBase
        {
            const AVClass* Avc;
        };

        return ctx ? (static_cast<const ContextBase*>(ctx))->Avc : nullptr;
    }

    const char OptionTools::NullClassPtr[] = "0C";

    //static 
    const char* OptionTools::GetContextName(const void* ctx) // dangerous !!
    {
        const char* ret = nullptr;

        if (ctx)
        {
            if (const AVClass* avc = GetContextClass(ctx))
            {
                if (auto item_name = avc->item_name)
                {
                    ret = item_name(const_cast<void*>(ctx));
                }
                else
                {
                    ret = avc->class_name;
                }
            }
            else
            {
                ret = NullClassPtr;
            }
        }

        return ret;
    }

    //static 
    int OptionTools::SetOption(void* ctx, const char* name, const char* val)
    {
        return av_opt_set(ctx, name, val, 0);
    }

    //static 
    int OptionTools::SetOption(void* ctx, const char* name, int64_t val)
    {
        return av_opt_set_int(ctx, name, val, 0);
    }

    //static 
    int OptionTools::SetOption(void* ctx, const char* name, const void* val, int size)
    {
        return av_opt_set_bin(ctx, name, static_cast<const uint8_t*>(val), size, 0);
    }

// -------------------------------------------------------------------------

    const char OptsStr::ColSepr = '\t';
    const char OptsStr::RowSepr = '\n';

    //static
    const char* OptsStr::Str_(const char* s)
    {
        return s ? (*s ? s : " ") : "<null>";
    }

    OptsStr::OptsStr(int capacity)
        : StrBuilder(capacity)
    {}

    OptsStr::~OptsStr() = default;

    void  OptsStr::SetPrefix(const char* prefix)
    {
        m_Str = Str_(prefix);
        m_Str += ':';
        m_Str += ' ';
    }

    void  OptsStr::AddItemEx(const char* item)
    {
        m_Str += Str_(item);
        m_Str += ColSepr;
    }

    void  OptsStr::AddRowSepr()
    {
        m_Str += RowSepr;
    }

    void  OptsStr::SetPrefixEx(const char* prefix)
    {
        m_Str = Str_(prefix);
        AddRowSepr();
    }

    void OptsStr::SetPrefixEx(const char* name, const char* nameType, const char* prefix)
    {
        m_Str = Str_(name);
        m_Str += ' ';
        if (nameType && *nameType)
        {
            m_Str += Str_(nameType);
            m_Str += ' ';
        }
        m_Str += Str_(prefix);
        AddRowSepr();
    }

    void  OptsStr::AddOptionEx(const AVOption* opt)
    {
        AddItemEx(opt->name);
        AddItemEx(TypeToStr(opt));
        AddItemEx(DefValToStr(opt));
        AddItemEx(opt->help);
        AddRowSepr();
    }

    static char ThreadBuff[128] = "";

    const int BuffSize = sizeof(ThreadBuff);

    //static 
    const char* OptsStr::DefValToStr(const AVOption* opt)
    {
        ThreadBuff[0] = '\0';
        switch (opt->type)
        {
        case AV_OPT_TYPE_STRING:
            av_strlcatf(ThreadBuff, BuffSize, "%s", StrToLog(opt->default_val.str));
            break;
        case AV_OPT_TYPE_DOUBLE:
        case AV_OPT_TYPE_FLOAT:
            av_strlcatf(ThreadBuff, BuffSize, "%g", opt->default_val.dbl);
            break;
        case AV_OPT_TYPE_INT:
        case AV_OPT_TYPE_INT64:
        case AV_OPT_TYPE_UINT64:
        case AV_OPT_TYPE_DURATION:
        case AV_OPT_TYPE_COLOR:
        case AV_OPT_TYPE_CHLAYOUT:
        case AV_OPT_TYPE_BOOL:
        case AV_OPT_TYPE_CONST:
        case AV_OPT_TYPE_FLAGS:
            av_strlcatf(ThreadBuff, BuffSize, "%lld", opt->default_val.i64);
            break;
        case AV_OPT_TYPE_RATIONAL:
        case AV_OPT_TYPE_IMAGE_SIZE:
        case AV_OPT_TYPE_VIDEO_RATE:
            av_strlcatf(ThreadBuff, BuffSize, "%d/%d", opt->default_val.q.num, opt->default_val.q.den);
            break;
        case AV_OPT_TYPE_PIXEL_FMT:
            av_strlcatf(ThreadBuff, BuffSize, "%s", PixFmtToStr(opt->default_val.i64));
            break;
        case AV_OPT_TYPE_SAMPLE_FMT:
            av_strlcatf(ThreadBuff, BuffSize, "%s", SampFmtToStr(opt->default_val.i64));
            break;
        default:
            av_strlcatf(ThreadBuff, BuffSize, "<...>");
        }

        return ThreadBuff;
    }

#define OPT_ITEM(opt) case AV_OPT_TYPE_##opt: return #opt

    //static 
    const char* OptsStr::TypeToStr(const AVOption* opt)
    {
        switch (opt->type)
        {
            OPT_ITEM(BOOL);
            OPT_ITEM(FLAGS);
            OPT_ITEM(INT);
            OPT_ITEM(INT64);
            OPT_ITEM(DOUBLE);
            OPT_ITEM(FLOAT);
            OPT_ITEM(STRING);
            OPT_ITEM(RATIONAL);
            OPT_ITEM(BINARY);
            OPT_ITEM(DICT);
            OPT_ITEM(UINT64);
            OPT_ITEM(CONST);
            OPT_ITEM(IMAGE_SIZE);
            OPT_ITEM(PIXEL_FMT);
            OPT_ITEM(SAMPLE_FMT);
            OPT_ITEM(VIDEO_RATE);
            OPT_ITEM(DURATION);
            OPT_ITEM(COLOR);
            OPT_ITEM(CHLAYOUT);
        }
        return "?";
    }

#undef OPT_ITEM


} // namespace Fcore


/*
enum AVOptionType

AV_OPT_TYPE_FLAGS
AV_OPT_TYPE_INT
AV_OPT_TYPE_INT64
AV_OPT_TYPE_DOUBLE
AV_OPT_TYPE_FLOAT
AV_OPT_TYPE_STRING
AV_OPT_TYPE_RATIONAL
AV_OPT_TYPE_BINARY offset must point to a pointer immediately followed by an int for the length

AV_OPT_TYPE_DICT
AV_OPT_TYPE_UINT64
AV_OPT_TYPE_CONST
AV_OPT_TYPE_IMAGE_SIZE offset must point to two consecutive integers

AV_OPT_TYPE_PIXEL_FMT
AV_OPT_TYPE_SAMPLE_FMT
AV_OPT_TYPE_VIDEO_RATE offset must point to AVRational

AV_OPT_TYPE_DURATION
AV_OPT_TYPE_COLOR
AV_OPT_TYPE_CHANNEL_LAYOUT
AV_OPT_TYPE_BOOL
AV_OPT_TYPE_CHLAYOUT


struct AVOption

onst char *       name
const char *      help    // Short English help text More...
int               offset  // The offset relative to the context structure where the option value is stored. More...
enum AVOptionType type

union {
   int64_t      i64
   double       dbl
   const char * str
   AVRational   q

}                 default_val // The default value for scalar options More...

double            min // Minimum valid value for the option More...
double            max // Maximum valid value for the option More...
int               flags
const char *      unit // The logical unit to which the option belongs. More...
*/