#pragma once

#include <Hedgehog/Universe/Engine/hhMessage.h>

namespace Sonic::Message
{

	class MsgSetGlobalLightDirection : public Hedgehog::Universe::MessageTypeSet
	{
	public:
		HH_FND_MSG_MAKE_TYPE(0x016A9BF8);

		Hedgehog::Math::CQuaternion m_Direction;

		MsgSetGlobalLightDirection(const Hedgehog::Math::CQuaternion& rotation) : m_Direction(rotation) {}
	};

	BB_ASSERT_OFFSETOF(MsgSetGlobalLightDirection, m_Direction, 0x10);
	BB_ASSERT_SIZEOF(MsgSetGlobalLightDirection, 0x20);

}
namespace Sonic::Message
{

	class MsgSetBGColor : public Hedgehog::Universe::MessageTypeSet
	{
	public:
		HH_FND_MSG_MAKE_TYPE(0x016AA7B8);

		Hedgehog::Math::CVector m_Direction;

		MsgSetBGColor(const Hedgehog::Math::CVector& rotation) : m_Direction(rotation) {}
	};

	BB_ASSERT_OFFSETOF(MsgSetBGColor, m_Direction, 0x10);
	BB_ASSERT_SIZEOF(MsgSetBGColor, 0x20);

}

//namespace Hedgehog::Math
//{
//	hh::math::CQuaternion QuaternionFromAngleAxis(float angle, const hh::math::CVector& axis)
//	{
//		CQuaternion q;
//		float m = sqrt(axis.x() * axis.x() + axis.y() * axis.y() + axis.z() * axis.z());
//		float s = sinf(angle / 2) / m;
//		q.x() = axis.x() * s;
//		q.y() = axis.y() * s;
//		q.z() = axis.z() * s;
//		q.w() = cosf(angle / 2);
//		return q;
//	}
//}


