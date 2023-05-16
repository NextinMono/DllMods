
using namespace hh::math;
using namespace Sonic;
enum AttackType {
	Straight,
	Hook,
	Aerial,
	Special
};
struct WerehogAttack
{
	const char* comboName;
	Sonic::EKeyState combo[5];
	int cueIDs[5];
	const char* animations[5];
	const float duration[5];
	AttackType attackType;
};

SharedPtrTypeless sound;
SharedPtrTypeless particle;
SharedPtrTypeless particle1;
SharedPtrTypeless particle2;
SharedPtrTypeless particle3;
SharedPtrTypeless particle4;
std::vector<Sonic::EKeyState> currentButtonChain;
std::vector<WerehogAttack> attacks;

bool canJump;
int jumpcount;
float timerAttack;
float timerAttackMax = 60;
int comboProgress = 0;
//replace with something that makes more sense
double calculateDistance(const std::vector<EKeyState>& array1, const std::vector<EKeyState>& array2) {
	if (array1.size() != array2.size()) {
		// Arrays must have the same size for a valid comparison
		return -1.0;
	}

	double sum = 0.0;
	for (size_t i = 0; i < array1.size(); ++i) {
		double diff = array1[i] - array2[i];
		sum += diff * diff;
	}

	return std::sqrt(sum);
}
void PlayAnim(const char* name)
{
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
	playerContext->m_pPlayer->SendMessageImm(playerContext->m_pPlayer->m_ActorID, spAnimInfo);

	playerContext->ChangeAnimation(name);
}
float GetVelocity()
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	if (inputPtr->IsDown(Sonic::eKeyState_RightTrigger))
		return 25;
	else
		return 17;
}
void RegisterInputs()
{
	if (timerAttack < timerAttackMax)
	{
		auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
		auto state = inputPtr->TappedState;
		if(state == eKeyState_A || state == eKeyState_X || state == eKeyState_Y)
		currentButtonChain.push_back(state);
		if (currentButtonChain.size() > 8)
		{
			currentButtonChain.erase(currentButtonChain.begin());
		}
		DebugDrawText::log("Current chain:", 0);
		for (size_t i = 0; i < currentButtonChain.size(); i++)
		{
			DebugDrawText::log(std::to_string(currentButtonChain.at(i)).c_str(), 0);
		}
	}
	else
		currentButtonChain.clear();
}
void ExecuteAttackCommand(int attackIndex,int comboIndex)
{
	auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	Common::SonicContextSetCollision(SonicCollision::TypeSonicSquatKick, true);
	Common::CreatePlayerSupportShockWave(playerContext->m_spMatrixNode->m_Transform.m_Position, 0.15f, 5, 0.1f);
	//auto ChestNode = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Hand_R"); //Set up Chest bone matrix for VFX
	//Eigen::Affine3f affine;
	//affine = ChestNode->m_WorldMatrix;

	//if (!particle)
	//	Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), particle, &ChestNode, "slash", 1);
	PlayAnim(attacks.at(attackIndex).animations[comboIndex]);
	Common::PlaySoundStatic(sound, attacks.at(attackIndex).cueIDs[comboIndex]);
}

void SpawnParticleOnHand(const char* glitterName, bool right)
{
	particle.reset();
	particle1.reset();
	particle2.reset();
	particle3.reset();
	particle4.reset();
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	auto index = playerContext->m_pPlayer->m_spCharacterModel->GetNode(right ? "Index1_R" : "Index1_L");
	auto middle = playerContext->m_pPlayer->m_spCharacterModel->GetNode(right ? "Middle1_R" : "Middle1_L");
	auto pinky = playerContext->m_pPlayer->m_spCharacterModel->GetNode(right ? "Pinky1_R" : "Pinky1_L");
	auto ring = playerContext->m_pPlayer->m_spCharacterModel->GetNode(right ? "Ring1_R" : "Ring1_L");
	auto thumb = playerContext->m_pPlayer->m_spCharacterModel->GetNode(right ? "Thumb1_R" : "Thumb1_L");
	if (!particle)
		Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), particle, &index, glitterName, 1);
	if (!particle1)
		Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), particle1, &middle, glitterName, 1);
	if (!particle2)
		Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), particle2, &pinky, glitterName, 1);
	if (!particle3)
		Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), particle3, &ring, glitterName, 1);
	if (!particle4)
		Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), particle4, &thumb, glitterName, 1);

}
void DespawnParticlesHand()
{
	particle.reset();
	particle1.reset();
	particle2.reset();
	particle3.reset();
	particle4.reset();
}
HOOK(void, __fastcall, CHudSonicStageUpdateParallel, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	originalCHudSonicStageUpdateParallel(This, Edx, in_rUpdateInfo);
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();

	// Force disable extended boost.
	*(uint32_t*)((uint32_t)*CONTEXT->ms_pInstance + 0x680) = 1;
	Hedgehog::Base::CSharedString stateCheck = playerContext->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName();
	std::string stateCheckS(stateCheck.c_str());
	// Always clamp boost to 100.
	CONTEXT->m_ChaosEnergy = min(CONTEXT->m_ChaosEnergy, 0);



	playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_BoostEnableChaosEnergy] = 1000.0f;
	playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_AirBoostEnableChaosEnergy] = 1000.0f;

	/*sonic->m_spParameter->m_scpNode->m_ValueMap.erase(Sonic::Player::ePlayerSpeedParameter_BoostEnableChaosEnergy);
	sonic->m_spParameter->m_scpNode->m_ValueMap.erase(Sonic::Player::ePlayerSpeedParameter_AirBoostEnableChaosEnergy);*/

	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];

	if (inputPtr->IsDown(eKeyState_X) || inputPtr->IsDown(eKeyState_Y))
	{
		timerAttack = 0;
	}
	RegisterInputs();
	if (inputPtr->IsDown(eKeyState_DpadLeft))
	{
		SpawnParticleOnHand("slash", true);
	}
	if (inputPtr->IsDown(Sonic::eKeyState_RightTrigger))
		playerContext->m_MaxVelocity = 20;
	else
		playerContext->m_MaxVelocity = 10;

	playerContext->m_JumpThrust = CVector(playerContext->m_JumpThrust.x(), 12, playerContext->m_JumpThrust.z());
	DebugDrawText::log(stateCheckS.c_str(), 0);
	if (timerAttack > timerAttackMax)
	{
		comboProgress = 0;
		currentButtonChain.clear();
		Common::SonicContextSetCollision(SonicCollision::TypeSonicSquatKick, false);
	}
	else
	{
		if ((timerAttack > 5 && comboProgress > 0 || comboProgress == 0) && currentButtonChain.size() > comboProgress)
		{
			int closestAttackIndex = 0;
			int closestComboIndex = 0;
			bool doneAttack = false;
			// Iterate through the attacks and find the closest combo
			for (size_t i = 0; i < attacks.size(); ++i)
			{
				if (doneAttack) break;
				auto attack = attacks[i];

				if (attack.combo[comboProgress] == currentButtonChain[comboProgress]) {
					// Execute the attack command based on the closest combo
					ExecuteAttackCommand(i, comboProgress);
					doneAttack = true;
					comboProgress++;
					timerAttack = 0;
					break;
				}

			}
		}

	}
	timerAttack += 1;
	//if (inputPtr->IsTapped(Sonic::eKeyState_X))
	//{
	//	timerAttack = 0;
	//	switch (xCount)
	//	{
	//	case 0:
	//	{
	//		PlayAnim("Evilsonic_attackNCA");
	//		/*playerContext->SetStateFlag(63, 1);*/
	//		Common::SonicContextSetCollision(SonicCollision::TypeSonicSquatKick, true);
	//		Common::PlaySoundStatic(sound, 134);
	//		auto matrix = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Arm12_R")->m_WorldMatrix.translation();
	//		Common::CreatePlayerSupportShockWave(playerContext->m_spMatrixNode->m_Transform.m_Position, 0.15f, 5, 0.1f);
	//		break;
	//	}
	//	case 1:
	//	{
	//		PlayAnim("Evilsonic_attackNCB");
	//		Common::PlaySoundStatic(sound, 134);
	//		Common::CreatePlayerSupportShockWave(playerContext->m_spMatrixNode->m_Transform.m_Position, 0.15f, 5, 0.1f);
	//		break;
	//	}
	//	case 2:
	//	{
	//		PlayAnim("Evilsonic_attackNCC");
	//		Common::PlaySoundStatic(sound, 135);
	//		Common::CreatePlayerSupportShockWave(playerContext->m_spMatrixNode->m_Transform.m_Position, 0.15f, 5, 0.1f);
	//		break;
	//	}
	//	case 3:
	//	{
	//		PlayAnim("Evilsonic_attackNCD");
	//		Common::PlaySoundStatic(sound, 136);
	//		Common::CreatePlayerSupportShockWave(playerContext->m_spMatrixNode->m_Transform.m_Position, 0.15f, 5, 0.1f);
	//		break;
	//	}
	//	case 4:
	//	{
	//		PlayAnim("Evilsonic_attackNCE");
	//		Common::PlaySoundStatic(sound, 136);
	//		Common::CreatePlayerSupportShockWave(playerContext->m_spMatrixNode->m_Transform.m_Position, 0.15f, 8, 0.1f);
	//		break;
	//	}
	//	}
	//	xCount++;
	//}
	if (inputPtr->IsTapped(Sonic::eKeyState_A))
	{
		if (canJump && jumpcount >= 1)
		{
			Eigen::Vector3f playerPosition;
			Eigen::Quaternionf playerRotation;

			alignas(16) MsgApplyImpulse message {};
			//please make this better
			message.m_position = playerPosition;
			message.m_impulse = playerContext->m_JumpThrust;
			message.m_impulseType = ImpulseType::None;
			message.m_outOfControl = 0.0f;
			message.m_notRelative = false;
			message.m_snapPosition = false;
			message.m_pathInterpolate = false;
			message.m_alwaysMinusOne = -1.0f;
			Common::ApplyPlayerApplyImpulse(message);
			message.m_position = playerPosition;
			message.m_impulse = playerContext->m_JumpThrust;
			message.m_impulseType = ImpulseType::None;
			message.m_outOfControl = 0.0f;
			message.m_notRelative = false;
			message.m_snapPosition = false;
			message.m_pathInterpolate = false;
			message.m_alwaysMinusOne = -1.0f;
			Common::ApplyPlayerApplyImpulse(message);
			canJump = false;
			playerContext->ChangeState("JumpShort");
			PlayAnim("JumpEvil2");
		}
		if (canJump && jumpcount == 0)
			PlayAnim("JumpEvil1");

		jumpcount++;
	}
	/* if (stateCheckS.find("Jump") != std::string::npos)
	 {

	 }*/

}
HOOK(char, __stdcall, SonicStateGrounded, 0xDFF660, int* a1, bool a2)
{
	canJump = true;
	jumpcount = 0;
	return originalSonicStateGrounded(a1, a2);
}
void evSonic::Install()
{
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNCA", "evilsonic_attackNCA");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNCB", "evilsonic_attackNCB");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNCC", "evilsonic_attackNCC");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNCD", "evilsonic_attackNCD");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNCE", "evilsonic_attackNCE");
	CustomAnimationManager::RegisterAnimation("JumpEvil1", "evilsonic_jumpVS");
	CustomAnimationManager::RegisterAnimation("JumpEvil2", "evilsonic_jumpVS2");


	/*const char* comboName;
	const char* stateName;
	Sonic::EKeyState combo[5];
	int cueIDs[5];
	const char* animations[5];*/
	attacks.push_back(WerehogAttack({ "Wild Whirl", {eKeyState_X,eKeyState_X,eKeyState_X,eKeyState_X,eKeyState_X }, {134,134,135,136,136 }, {"Evilsonic_attackNCA","Evilsonic_attackNCB","Evilsonic_attackNCC","Evilsonic_attackNCD","Evilsonic_attackNCE"}, {2,2,3,3,5} }));
	attacks.push_back(WerehogAttack({ "Wild Whirl2", {eKeyState_Y,eKeyState_X,eKeyState_X,eKeyState_X,eKeyState_X }, {134,134,135,136,136 }, {"Evilsonic_attackNCA","Evilsonic_attackNCB","Evilsonic_attackNCC","Evilsonic_attackNCD","Evilsonic_attackNCE"}, {2,2,3,3,5} }));
	WRITE_MEMORY(0x01271FD1, char*, "ef_null"); // Disable original game's jumpball creation
	WRITE_MEMORY(0x015E9078, char*, "ef_null"); // Disable empty boost
	
	INSTALL_HOOK(CHudSonicStageUpdateParallel);
	INSTALL_HOOK(SonicStateGrounded);

	WRITE_NOP(0xDFE1C4, 2);
	WRITE_NOP(0xDFE1D2, 2);
	WRITE_MEMORY(0x00DF1D91, uint32_t, -1);
	//From Brianuu - Disable squat and sliding
	WRITE_MEMORY(0xDFF8D5, uint8_t, 0xEB, 0x05);
	WRITE_MEMORY(0xDFF856, uint8_t, 0xE9, 0x81, 0x00, 0x00, 0x00);
	//From Hyper - unmap drift
	WRITE_MEMORY(0xDF2DFF, uint32_t, -1);
	WRITE_MEMORY(0xDFF62B, uint32_t, -1);
	WRITE_MEMORY(0xDFF816, uint32_t, -1);
	WRITE_MEMORY(0x1119549, uint32_t, -1);
	WRITE_MEMORY(0x119910D, uint32_t, -1);
}