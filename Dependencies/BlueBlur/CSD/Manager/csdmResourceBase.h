#pragma once

#include <CSD/Manager/csdmRCPtr.h>

namespace Chao::CSD
{
     template<typename T>
    class CResourceBase
    {
    public:
        RCPtr<unsigned char> m_rcContainerData;
        T* m_pResourceData;

        virtual ~CResourceBase() = default;

        virtual void CopyResource(const CResourceBase& in_rOther)
        {
            m_rcContainerData = in_rOther.m_rcContainerData;
            m_pResourceData = in_rOther.m_pResourceData;
        }
    };

    BB_ASSERT_OFFSETOF(CResourceBase<void>, m_rcContainerData, 0x4);
    BB_ASSERT_OFFSETOF(CResourceBase<void>, m_pResourceData, 0xC);
    BB_ASSERT_SIZEOF(CResourceBase<void>, 0x10);
}