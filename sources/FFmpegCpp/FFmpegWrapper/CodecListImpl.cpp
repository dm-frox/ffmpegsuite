#include "pch.h"

#include "CodecListImpl.h"

namespace Fwrap
{
    CodecListImpl::CodecListImpl() = default;

    CodecListImpl::~CodecListImpl() = default;

    void CodecListImpl::Setup()
    {
        m_CodecList.Setup();
    }

    int CodecListImpl::Count() const
    {
        return m_CodecList.Count();
    }

    const ICodecNode* CodecListImpl::operator[](int i) const
    {
        return m_CodecList[i];
    }

    int CodecListImpl::CodecCount() const
    {
        return m_CodecList.CodecCount();
    }

    const char* CodecListImpl::ContextOptionsEx() const
    {
        return m_CodecList.CtxOptsEx();
    }

// --------------------------------------------------------------------
// factory 

    ICodecList* ICodecList::CreateInstance()
    {
        return static_cast<ICodecList*>(new CodecListImpl());
    }

} // namespace Fwrap