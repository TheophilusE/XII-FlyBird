#include <FlyBirdPlugin/FlyBirdPluginPCH.h>

#include <Core/Input/InputManager.h>
#include <Core/System/Window.h>
#include <Core/World/World.h>
#include <FlyBirdPlugin/GameState/FlyBirdGameState.h>
#include <Foundation/Configuration/CVar.h>
#include <Foundation/Logging/Log.h>
#include <JoltPlugin/Actors/JoltDynamicActorComponent.h>
#include <RendererCore/Debug/DebugRenderer.h>
#include <RendererCore/Meshes/MeshComponent.h>
#include <GameEngine/Gameplay/BlackboardComponent.h>

/// CVars

xiiCVarBool cvar_DebugDisplay("FlyBird.DebugDisplay", false, xiiCVarFlags::Default, "Whether the game should display debug geometry.");

xiiCVarBool cvar_PlayerAlive("FlyBird.PlayerAlive", true, xiiCVarFlags::Default, "Whether player is still alive");

xiiCVarInt cvar_PlayerScore("FlyBird.PlayerScore", 0, xiiCVarFlags::Save, "The player's current score.");

xiiCVarFloat cvar_PlayerJumpImpulse("FlyBird.JumpImpulse", 50.0f, xiiCVarFlags::Default, "The player's jump impulse");


XII_BEGIN_DYNAMIC_REFLECTED_TYPE(FlyBirdGameState, 1, xiiRTTIDefaultAllocator<FlyBirdGameState>)
XII_END_DYNAMIC_REFLECTED_TYPE;

FlyBirdGameState::FlyBirdGameState()  = default;
FlyBirdGameState::~FlyBirdGameState() = default;

void FlyBirdGameState::OnActivation(xiiWorld* pWorld, const xiiTransform* pStartPosition)
{
  XII_LOG_BLOCK("GameState::Activate");

  SUPER::OnActivation(pWorld, pStartPosition);
}

void FlyBirdGameState::OnDeactivation()
{
  XII_LOG_BLOCK("GameState::Deactivate");

  SUPER::OnDeactivation();
}

void FlyBirdGameState::AfterWorldUpdate()
{
  SUPER::AfterWorldUpdate();

  if (cvar_DebugDisplay)
  {
    xiiDebugRenderer::DrawLineSphere(m_pMainWorld, xiiBoundingSphere(xiiVec3::ZeroVector(), 1.0f), xiiColor::Orange);
  }

  // Draw controls
  xiiDebugRenderer::Draw2DText(m_pMainWorld, "Press 'Space' to fly", xiiVec2I32(10, 10), xiiColor::White);

  // Draw player score
  xiiStringBuilder sData;

  if (cvar_PlayerAlive)
  {
    sData.Format("Score: {0}", cvar_PlayerScore);
    xiiDebugRenderer::DrawInfoText(m_pMainWorld, xiiDebugRenderer::ScreenPlacement::TopCenter, "", sData.GetData(), xiiColor::GreenYellow);
  }
  else
  {
    sData.Format("Game Over. Final Score: {0}", cvar_PlayerScore);
    xiiDebugRenderer::DrawInfoText(m_pMainWorld, xiiDebugRenderer::ScreenPlacement::TopCenter, "", sData.GetData(), xiiColor::GreenYellow);
  }
}

void FlyBirdGameState::BeforeWorldUpdate()
{
  XII_LOCK(m_pMainWorld->GetWriteMarker());
}

xiiGameStatePriority FlyBirdGameState::DeterminePriority(xiiWorld* pWorld) const
{
  return xiiGameStatePriority::Default;
}

void FlyBirdGameState::ConfigureMainWindowInputDevices(xiiWindow* pWindow)
{
  SUPER::ConfigureMainWindowInputDevices(pWindow);

  // Setup devices here
}

static void RegisterInputAction(const char* szInputSet, const char* szInputAction, const char* szKey1, const char* szKey2 = nullptr, const char* szKey3 = nullptr)
{
  xiiInputActionConfig cfg;
  cfg.m_bApplyTimeScaling    = true;
  cfg.m_sInputSlotTrigger[0] = szKey1;
  cfg.m_sInputSlotTrigger[1] = szKey2;
  cfg.m_sInputSlotTrigger[2] = szKey3;

  xiiInputManager::SetInputActionConfig(szInputSet, szInputAction, cfg, true);
}

void FlyBirdGameState::ConfigureInputActions()
{
  SUPER::ConfigureInputActions();

  RegisterInputAction("FlyBirdPlugin", "Fly", xiiInputSlot_KeySpace, xiiInputSlot_Controller0_ButtonA, xiiInputSlot_MouseButton2);
}

void FlyBirdGameState::ProcessInput()
{
  SUPER::ProcessInput();

  xiiWorld* pWorld = m_pMainWorld;

  // Retrieve player object
  xiiGameObject* pPlayerObject = nullptr;
  if (!pWorld->TryGetObjectWithGlobalKey("Player", pPlayerObject))
  {
    xiiLog::Error("Failed to retrieve player object with key '{0}'", "Player");
    return;
  }

  // Retrieve blackboard component for animation
  xiiBlackboardComponent* pAnimationBoard = nullptr;
  if (!pPlayerObject->FindChildByName("Mesh")->TryGetComponentOfBaseType(pAnimationBoard))
  {
    xiiLog::Error("Failed to retrieve player animation blackboard '{0}'", "Player");
    return;
  }

  if (xiiInputManager::GetInputActionState("FlyBirdPlugin", "Fly") == xiiKeyState::Down && cvar_PlayerAlive)
  {
    xiiJoltDynamicActorComponent* pDynamicActor = nullptr;
    pPlayerObject->TryGetComponentOfBaseType(pDynamicActor);

    // Apply upward impulse
    pDynamicActor->AddLinearImpulse(xiiVec3(0, 0, cvar_PlayerJumpImpulse * pWorld->GetClock().GetTimeDiff().AsFloatInSeconds()));

    // Set blackboard entry to update animation using the fly animation.
    pAnimationBoard->SetEntryValue("IsFly", 1);
  }
  else
  {
    // Set blackboard entry to update animation using the fly stay animation.
    pAnimationBoard->SetEntryValue("IsFly", 0);
  }
}

void FlyBirdGameState::ConfigureMainCamera()
{
  SUPER::ConfigureMainCamera();

  // Do custom camera setup here
}
