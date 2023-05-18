
using namespace hh::math;
using namespace Sonic;



//"Index1_R" :
//"Middle1_R"
//"Pinky1_R" :
//	Ring1_R" : "
//	"Thumb1_R" :
SharedPtrTypeless sound,soundUnleash, soundUnleashStart;
SharedPtrTypeless indexParticle_L, indexParticle_R;
SharedPtrTypeless middleParticle_L, middleParticle_R;
SharedPtrTypeless pinkyParticle_L, pinkyParticle_R;
SharedPtrTypeless ringParticle_L, ringParticle_R;
SharedPtrTypeless thumbParticle_L, thumbParticle_R;
SharedPtrTypeless shield;
SharedPtrTypeless berserk[5];
std::vector<Sonic::EKeyState> currentButtonChain;
std::vector<WerehogAttack> attacks;
bool canJump;
int jumpcount;
int comboAttackIndex;
bool isUsingShield;
float timerCombo;
float timerAttack;
float timerComboMax = 1.55f;
float timerAttackMax= 1.25f;
int comboProgress = 0;
float lifeWerehog = 5.0f;
bool unleashMode;
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
void PlayAnim(std::string name)
{
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
	playerContext->m_pPlayer->SendMessageImm(playerContext->m_pPlayer->m_ActorID, spAnimInfo);
	playerContext->ChangeAnimation(name.c_str());
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
	if (timerCombo < timerComboMax)
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
void DespawnParticlesHand()
{
	indexParticle_L.reset();
	middleParticle_L.reset();
	pinkyParticle_L.reset();
	ringParticle_L.reset();
	thumbParticle_L.reset();


	indexParticle_R.reset();
	middleParticle_R.reset();
	pinkyParticle_R.reset();
	ringParticle_R.reset();
	thumbParticle_R.reset();
}
void SpawnParticleOnHand(const char* glitterName, bool right)
{
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	if (right)
	{
		auto index = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Index1_L");
		auto middle = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Middle1_L");
		auto pinky = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Pinky1_L");
		auto ring = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Ring1_L");
		auto thumb = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Thumb1_L");
		if (!indexParticle_L)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), indexParticle_L, &index, glitterName, 1);
		if (!middleParticle_L)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), middleParticle_L, &middle, glitterName, 1);
		if (!pinkyParticle_L)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), pinkyParticle_L, &pinky, glitterName, 1);
		if (!ringParticle_L)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), ringParticle_L, &ring, glitterName, 1);
		if (!thumbParticle_L)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), thumbParticle_L, &thumb, glitterName, 1);
	}
	else
	{
		auto index = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Index1_R");
		auto middle = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Middle1_R");
		auto pinky = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Pinky1_R");
		auto ring = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Ring1_R");
		auto thumb = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Thumb1_R");
		if (!indexParticle_R)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), indexParticle_R, &index, glitterName, 1);
		if (!middleParticle_R)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), middleParticle_R, &middle, glitterName, 1);
		if (!pinkyParticle_R)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), pinkyParticle_R, &pinky, glitterName, 1);
		if (!ringParticle_R)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), ringParticle_R, &ring, glitterName, 1);
		if (!thumbParticle_R)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), thumbParticle_R, &thumb, glitterName, 1);
	}

	

}

void KillBerserkEffect()
{
	berserk[0].reset();
	berserk[1].reset();
	berserk[2].reset();
}
void CreateBerserkEffect()
{
	KillBerserkEffect();
	auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	auto node0 = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Reference");
	auto node1 = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Hand_R");
	auto node2 = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Hand_L");
	if (!berserk[0])
		Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), berserk[0], &node0, "evil_berserk01", 1);
	if (!berserk[1])
		Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), berserk[1], &node1, "evil_berserk02", 1);
	if (!berserk[2])
		Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), berserk[2], &node2, "evil_berserk02", 1);
}
void ExecuteAttackCommand(int attackIndex,int comboIndex)
{
	comboAttackIndex = attackIndex;
	auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	Common::SonicContextSetCollision(SonicCollision::TypeSonicSquatKick, true);
	Common::CreatePlayerSupportShockWave(playerContext->m_spMatrixNode->m_Transform.m_Position, 0.15f, 5, 0.1f);
	PlayAnim(attacks.at(attackIndex).animations[comboIndex]);
	Common::PlaySoundStatic(sound, attacks.at(attackIndex).cueIDs[comboIndex]);

	SpawnParticleOnHand("slash", true);
	SpawnParticleOnHand("slash", false);
	timerAttack = 0;
}

//find a better way please
bool init = false;
void Particle_Checker()
{
	if (timerAttack > timerAttackMax)
		DespawnParticlesHand();
}
HOOK(void, __fastcall, CHudSonicStageUpdateParallel, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	originalCHudSonicStageUpdateParallel(This, Edx, in_rUpdateInfo);
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();

	// Force disable extended boost.
	*(uint32_t*)((uint32_t)*CONTEXT->ms_pInstance + 0x680) = 1;
	CONTEXT->m_ChaosEnergy = min(CONTEXT->m_ChaosEnergy, 100);

	Hedgehog::Base::CSharedString stateCheck = playerContext->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName();
	std::string stateCheckS(stateCheck.c_str());

	if (!init)
	{
		playerContext->m_JumpThrust = CVector(playerContext->m_JumpThrust.x(), 12, playerContext->m_JumpThrust.z());
		playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_BoostEnableChaosEnergy] = 1000.0f;
		playerContext->m_spParameter->m_scpNode->m_ValueMap[Sonic::Player::ePlayerSpeedParameter_AirBoostEnableChaosEnergy] = 1000.0f;
		playerContext->m_pSkills = 0;
		
	}
	/*sonic->m_spParameter->m_scpNode->m_ValueMap.erase(Sonic::Player::ePlayerSpeedParameter_BoostEnableChaosEnergy);
	sonic->m_spParameter->m_scpNode->m_ValueMap.erase(Sonic::Player::ePlayerSpeedParameter_AirBoostEnableChaosEnergy);*/
	DebugDrawText::log((std::string("Timer Combo:") +std::to_string(timerCombo)).c_str(), 0);
	DebugDrawText::log((std::string("Timer Combo Max:") + std::to_string(timerComboMax)).c_str(),0);
	DebugDrawText::log((std::string("Timer Attack:") +std::to_string(timerAttack)).c_str(), 0);
	DebugDrawText::log((std::string("Timer Attack Max:") + std::to_string(timerAttackMax)).c_str(),0);
	DebugDrawText::log((std::string("Current combo:") + std::string(attacks[comboAttackIndex].comboName)).c_str(), 0);
	DebugDrawText::log((std::string("Combo progress:") + std::to_string(comboProgress)).c_str(), 0);
	DebugDrawText::log((std::string("Boost") + std::to_string(CONTEXT->m_ChaosEnergy)).c_str(), 0);
	DebugDrawText::log("\n", 0);
	DebugDrawText::log((std::string("Life") + std::to_string(lifeWerehog)).c_str(), 0);
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];

	if (unleashMode && CONTEXT->m_ChaosEnergy > 0)
	{
		playerContext->m_ChaosEnergy -= in_rUpdateInfo.DeltaTime * 5;
	}
	else
	{
		unleashMode = false;
		KillBerserkEffect();
		soundUnleash.reset();
	}
	if ((inputPtr->IsDown(eKeyState_RightBumper) && CONTEXT->m_ChaosEnergy == 100.0f) && !unleashMode)
	{		
		
		PlayAnim("Evilsonic_BerserkerS");
		CreateBerserkEffect();
		Common::PlaySoundStatic(soundUnleashStart, 126);
		Common::PlaySoundStatic(soundUnleash, 127);
		unleashMode = true;
	}
	if (inputPtr->IsDown(eKeyState_LeftBumper) && !isUsingShield)
	{
		isUsingShield = true;
		//ef_ch_sng_yh1_boost2
		auto node = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Reference");
		if (!shield)
			Common::fCGlitterCreate(playerContext->m_pPlayer->m_spContext.get(), shield, &node, "evil_berserk01", 1);

		PlayAnim("Evilsonic_guard_idle");
	}
	else
		isUsingShield = false;
	if (inputPtr->IsDown(eKeyState_X) || inputPtr->IsDown(eKeyState_Y))
	{
		timerCombo = 0;
	}
	RegisterInputs();
	Particle_Checker();
	
	

	DebugDrawText::log(stateCheckS.c_str(), 0);
	if (timerCombo > timerComboMax)
	{
		comboProgress = 0;
		currentButtonChain.clear();
		Common::SonicContextSetCollision(SonicCollision::TypeSonicSquatKick, false); 
		if (inputPtr->IsDown(Sonic::eKeyState_RightTrigger))
			playerContext->m_MaxVelocity = 20;
		else
			playerContext->m_MaxVelocity = 10;
	}
	else
	{
		if ((timerCombo > 0.1f && comboProgress > 0 || comboProgress == 0) && (timerAttack > timerAttackMax) && currentButtonChain.size() > comboProgress)
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
					timerCombo = 0;
					timerAttackMax = attack.duration[comboProgress-1];
					playerContext->m_MaxVelocity = attack.moveSpeed;
					break;
				}
			}
		}
	}
	//if (inputPtr->IsTapped(Sonic::eKeyState_X))
	//{
	//	timerCombo = 0;
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

	timerCombo += in_rUpdateInfo.DeltaTime;
	timerAttack += in_rUpdateInfo.DeltaTime;

}
HOOK(char, __stdcall, SonicStateGrounded, 0xDFF660, int* a1, bool a2)
{
	canJump = true;
	jumpcount = 0;
	return originalSonicStateGrounded(a1, a2);
}

HOOK(void, __fastcall, CPlayerAddCallback, 0xE799F0, Sonic::Player::CPlayer* This, void* Edx,
	const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder, Sonic::CGameDocument* pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase)
{
	originalCPlayerAddCallback(This, Edx, worldHolder, pGameDocument, spDatabase);

	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	playerContext->m_ChaosEnergy = 0;
	lifeWerehog = 5.0f;
}
int timerHoming;
HOOK(int, __fastcall, HomingBegin, 0x01232040, CQuaternion* This)
{
	if (CONTEXT->m_HomingAttackTargetActorID)
	{
		return originalHomingBegin(This);
	}
	else

	return 0;
}
HOOK(void, __fastcall, CSonicProcMsgDamage, 0xE27890, Sonic::Player::CSonic* This, void* _, hh::fnd::Message& in_rMsg)
{
	lifeWerehog -= 1;
	if (lifeWerehog > 0)
	{
		PlayAnim("Evilsonic_damageMB");
	}
	else
	{
		CONTEXT->m_RingCount = 0;
		originalCSonicProcMsgDamage(This, _, in_rMsg);

	}
}
HOOK(void, __fastcall, ProcMsgRestartStage, 0xE76810, Sonic::Player::CPlayer* This, void* Edx, hh::fnd::Message& message)
{
	originalProcMsgRestartStage(This, Edx, message);
	lifeWerehog = 5.0f;
} 

void evSonic::Install()
{
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNCA", "evilsonic_attackNCA");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNCB", "evilsonic_attackNCB");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNCC", "evilsonic_attackNCC");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNCD", "evilsonic_attackNCD");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNCE", "evilsonic_attackNCE");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNSA", "evilsonic_attackNSA");
	CustomAnimationManager::RegisterAnimation("Evilsonic_attackNSB", "evilsonic_attackNSB");
	CustomAnimationManager::RegisterAnimation("Evilsonic_damageMB", "evilsonic_damageMB");
	CustomAnimationManager::RegisterAnimation("Evilsonic_guard_idle", "evilsonic_guard_idle");
	CustomAnimationManager::RegisterAnimation("Evilsonic_BerserkerS", "evilsonic_BerserkerS");
	CustomAnimationManager::RegisterAnimation("JumpEvil1", "evilsonic_jumpVS");
	CustomAnimationManager::RegisterAnimation("JumpEvil2", "evilsonic_jumpVS2");


	/*const char* comboName;
	const char* stateName;
	Sonic::EKeyState combo[5];
	int cueIDs[5];
	const char* animations[5];*/
	attacks.push_back(WerehogAttack({ "Wild Whirl", {eKeyState_X,eKeyState_X,eKeyState_X,eKeyState_X,eKeyState_X }, {134,134,135,136,136 }, {"Evilsonic_attackNCA","Evilsonic_attackNCB","Evilsonic_attackNCC","Evilsonic_attackNCD","Evilsonic_attackNCE"}, {0.15F,0.15F,0.15F,0.55F,0.85f}, 1 }));
	attacks.push_back(WerehogAttack({ "Wild Whirl2", {eKeyState_Y,eKeyState_Y }, {134,134,135,136,136 }, {"Evilsonic_attackNSA","Evilsonic_attackNSB"}, {0.15F,0.15F},1 }));
	WRITE_MEMORY(0x01271FD1, char*, "ef_null"); // Disable original game's jumpball creation
	WRITE_MEMORY(0x015E9078, char*, "ef_null"); // Disable empty boost
	
	INSTALL_HOOK(ProcMsgRestartStage);
	INSTALL_HOOK(CSonicProcMsgDamage);
	INSTALL_HOOK(CHudSonicStageUpdateParallel);
	INSTALL_HOOK(SonicStateGrounded);
	INSTALL_HOOK(HomingBegin);
	//WRITE_JUMP(0x01232055, 0x01232073);
	//Unmap boost (use chaos energy as unleashed meter)
	WRITE_MEMORY(0x0111BEEC, uint32_t, -1);
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