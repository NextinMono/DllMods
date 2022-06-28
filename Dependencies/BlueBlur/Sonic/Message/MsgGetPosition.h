#pragma once

#include <Hedgehog/Universe/Engine/hhMessage.h>

namespace Sonic::Message
{
    class MsgGetPosition : public Hedgehog::Universe::MessageTypeGet
    {
    public:
        HH_FND_MSG_MAKE_TYPE(0x1681870);

        Hedgehog::Math::CVector* m_pPosition;

        MsgGetPosition(Hedgehog::Math::CVector* position) : m_pPosition(position) {}
        MsgGetPosition(Hedgehog::Math::CVector& position) : m_pPosition(&position) {}
    };

    BB_ASSERT_OFFSETOF(MsgGetPosition, m_pPosition, 0x10);
    BB_ASSERT_SIZEOF(MsgGetPosition, 0x14);
}