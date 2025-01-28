#include "pch.h"

#include "CodecList.h"

#include "Core.h"
#include "Exceptions.h"


namespace FFmpegInterop
{
    CodecInfo::CodecInfo(InfoPrt p)
        : m_Info(p)
    {
        ASSERT_PTR(m_Info);
    }

    StrType^ CodecInfo::ToString()
    {
        return Name;
    }

    bool CodecInfo::Decoder::get()
    {
        return m_Info->IsDecoder();
    }

    bool CodecInfo::Experimental::get()
    {
        return m_Info->IsExperimental();
    }

    StrType^ CodecInfo::Name::get()
    {
        return gcnew StrType(m_Info->Name());
    }

    StrType^ CodecInfo::LongName::get()
    {
        return gcnew StrType(m_Info->LongName());
    }

    StrType^ CodecInfo::Formats::get()
    {
        return gcnew StrType(m_Info->Formats());
    }

    StrType^ CodecInfo::HWAccels::get()
    {
        return gcnew StrType(m_Info->HWAccels());
    }

    StrType^ CodecInfo::PrivOptions::get()
    {
        return gcnew StrType(m_Info->PrivOptions());
    }

    StrType^ CodecInfo::PrivOptionsEx::get()
    {
        return gcnew StrType(m_Info->PrivOptionsEx());
    }

// ------------------------------------------------------------------------------------

    CodecNode::CodecNode(NodePrt p)
        : m_Node(p)
    {
        ASSERT_PTR(m_Node);
    }

    StrType^ CodecNode::ToString()
    {
        return Name;
    }

    int CodecNode::CodecCount::get()
    {
        return m_Node->CodecCount();
    }

    int CodecNode::DecoderCount::get()
    {
        return m_Node->DecoderCount();
    }

    int CodecNode::EncoderCount::get()
    {
        return m_Node->EncoderCount();
    }

    StrType^ CodecNode::Name::get()
    {
        return gcnew StrType(m_Node->Name());
    }

    StrType^ CodecNode::LongName::get()
    {
        return gcnew StrType(m_Node->LongName());
    }

    StrType^ CodecNode::MediaTypeStr::get()
    {
        return gcnew StrType(m_Node->MediaTypeStr());
    }

    int CodecNode::MediaType::get()
    {
        return m_Node->MediaType();
    }

    bool CodecNode::Lossless::get()
    {
        return m_Node->Lossless();
    }

    bool CodecNode::Lossy::get()
    {
        return m_Node->Lossy();
    }

    bool CodecNode::IntraOnly::get()
    {
        return m_Node->IntraOnly();
    }

    CodecInfo^ CodecNode::Decoder(int i)
    {
        CodecInfo^ ret = nullptr;
        if (0 <= i && i < m_Node->DecoderCount())
        {
            ret = gcnew CodecInfo(m_Node->Decoder(i));
        }
        return ret;
    }

    CodecInfo^ CodecNode::Encoder(int i)
    {
        CodecInfo^ ret = nullptr;
        if (0 <= i && i < m_Node->EncoderCount())
        {
            ret = gcnew CodecInfo(m_Node->Encoder(i));
        }
        return ret;
    }

// ------------------------------------------------------------------------------------

    CodecList::CodecList()
        : CodecListBase(Core::Ptr()->CreateCodecList())
    {
        ASSERT_IMPL();

        Ptr()->Setup();
    }

    int CodecList::Count::get()
    {
        return Ptr()->Count();
    }

    CodecNode^ CodecList::default::get(int i)
    {
        return gcnew CodecNode(Ptr()->operator[](i));
    }

    int CodecList::CodecCount::get()
    {
        return Ptr()->CodecCount();
    }

    StrType^ CodecList::ContextOptionsEx::get()
    {
        return gcnew StrType(Ptr()->ContextOptionsEx());
    }

} // namespace FFmpegInterop