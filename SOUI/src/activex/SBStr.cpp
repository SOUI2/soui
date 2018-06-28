#include "souistd.h"

#include "activex/SBStr.h"

namespace SOUI
{
        sbstr::sbstr(const char16* non_bstr)
            : bstr_(SysAllocString(non_bstr)) {}

        sbstr::sbstr(int nSize, const char16* non_bstr)
        {
            if (nSize == 0)
            {
                bstr_ = NULL;
            }
            else
            {
                bstr_ = ::SysAllocStringLen(non_bstr, nSize);
            }
        }

        sbstr::~sbstr()
        {
            SysFreeString(bstr_);
        }

        void sbstr::Reset(BSTR bstr)
        {
            if(bstr != bstr_)
            {
                // if |bstr_| is NULL, SysFreeString does nothing.
                SysFreeString(bstr_);
                bstr_ = bstr;
            }
        }

        BSTR sbstr::Release()
        {
            BSTR bstr = bstr_;
            bstr_ = NULL;
            return bstr;
        }

        void sbstr::Swap(sbstr& bstr2)
        {
            BSTR tmp = bstr_;
            bstr_ = bstr2.bstr_;
            bstr2.bstr_ = tmp;
        }

        BSTR* sbstr::Receive()
        {
            return &bstr_;
        }

        BSTR sbstr::Allocate(const char16* str)
        {
            Reset(SysAllocString(str));
            return bstr_;
        }

        BSTR sbstr::AllocateBytes(size_t bytes)
        {
            Reset(SysAllocStringByteLen(NULL, static_cast<UINT>(bytes)));
            return bstr_;
        }

        void sbstr::SetByteLen(size_t bytes)
        {
            uint32* data = reinterpret_cast<uint32*>(bstr_);
            data[-1] = static_cast<uint32>(bytes);
        }

        size_t sbstr::Length() const
        {
            return SysStringLen(bstr_);
        }

        size_t sbstr::ByteLength() const
        {
            return SysStringByteLen(bstr_);
        }

} //namespace base