#include <FlyBirdPlugin/FlyBirdPluginPCH.h>

#include <Core/Input/InputManager.h>
#include <Core/System/Window.h>
#include <Core/World/World.h>
#include <FlyBirdPlugin/GameState/FlyBirdGameState.h>
#include <Foundation/Configuration/CVar.h>
#include <Foundation/Logging/Log.h>
#include <RendererCore/Debug/DebugRenderer.h>
#include <RendererCore/Meshes/MeshComponent.h>

xiiCVarBool cvar_DebugDisplay("FlyBird.DebugDisplay", false, xiiCVarFlags::Default, "Whether the game should display debug geometry.");

XII_BEGIN_DYNAMIC_REFLECTED_TYPE(FlyBirdGameState, 1, xiiRTTIDefaultAllocator<FlyBirdGameState>)
XII_END_DYNAMIC_REFLECTED_TYPE;

FlyBirdGameState::FlyBirdGameState() = default;
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

  xiiDebugRenderer::Draw2DText(m_pMainWorld, "Press 'O' to spawn objects", xiiVec2I32(10, 10), xiiColor::White);
  xiiDebugRenderer::Draw2DText(m_pMainWorld, "Press 'P' to remove objects", xiiVec2I32(10, 30), xiiColor::White);
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

  // setup devices here
}

static void RegisterInputAction(const char* szInputSet, const char* szInputAction, const char* szKey1, const char* szKey2 = nullptr, const char* szKey3 = nullptr)
{
  xiiInputActionConfig cfg;
  cfg.m_bApplyTimeScaling = true;
  cfg.m_sInputSlotTrigger[0] = szKey1;
  cfg.m_sInputSlotTrigger[1] = szKey2;
  cfg.m_sInputSlotTrigger[2] = szKey3;

  xiiInputManager::SetInputActionConfig(szInputSet, szInputAction, cfg, true);
}

void FlyBirdGameState::ConfigureInputActions()
{
  SUPER::ConfigureInputActions();

  RegisterInputAction("FlyBirdPlugin", "SpawnObject", xiiInputSlot_KeyO, xiiInputSlot_Controller0_ButtonA, xiiInputSlot_MouseButton2);
  RegisterInputAction("FlyBirdPlugin", "DeleteObject", xiiInputSlot_KeyP, xiiInputSlot_Controller0_ButtonB);
}

void FlyBirdGameState::ProcessInput()
{
  SUPER::ProcessInput();

  xiiWorld* pWorld = m_pMainWorld;

  if (xiiInputManager::GetInputActionState("FlyBirdPlugin", "SpawnObject") == xiiKeyState::Pressed)
  {
    const xiiVec3 pos = GetMainCamera()->GetCenterPosition() + GetMainCamera()->GetCenterDirForwards();

    // make sure we are allowed to modify the world
    XII_LOCK(pWorld->GetWriteMarker());

    // create a game object at the desired position
    xiiGameObjectDesc desc;
    desc.m_LocalPosition = pos;

    xiiGameObject* pObject = nullptr;
    xiiGameObjectHandle hObject = pWorld->CreateObject(desc, pObject);

    m_SpawnedObjects.PushBack(hObject);

    // attach a mesh component to the object
    xiiMeshComponent* pMesh;
    pWorld->GetOrCreateComponentManager<xiiMeshComponentManager>()->CreateComponent(pObject, pMesh);

    // Set the mesh to use.
    // Here we use a path relative to the project directory.
    // We have to reference the 'transformed' file, not the source file.
    // This would break if the source asset is moved or renamed.
    pMesh->SetMeshFile("AssetCache/Common/Meshes/Sphere.xiiMesh");

    // here we use the asset GUID to reference the transformed asset
    // we can copy the GUID from the asset browser
    // the GUID is stable even if the source asset gets moved or renamed
    // using asset collections we could also give a nice name like 'Blue Material' to this asset
    xiiMaterialResourceHandle hMaterial = xiiResourceManager::LoadResource<xiiMaterialResource>("{ aa1c5601-bc43-fbf8-4e07-6a3df3af51e7 }");

    // override the mesh material in the first slot with something different
    pMesh->SetMaterial(0, hMaterial);
  }

  if (xiiInputManager::GetInputActionState("FlyBirdPlugin", "DeleteObject") == xiiKeyState::Pressed)
  {
    if (!m_SpawnedObjects.IsEmpty())
    {
      // make sure we are allowed to modify the world
      XII_LOCK(pWorld->GetWriteMarker());

      xiiGameObjectHandle hObject = m_SpawnedObjects.PeekBack();
      m_SpawnedObjects.PopBack();

      // this is only for demonstration purposes, removing the object will delete all attached components as well
      xiiGameObject* pObject = nullptr;
      if (pWorld->TryGetObject(hObject, pObject))
      {
        xiiMeshComponent* pMesh = nullptr;
        if (pObject->TryGetComponentOfBaseType(pMesh))
        {
          pMesh->DeleteComponent();
        }
      }

      // delete the object, all its children and attached components
      pWorld->DeleteObjectDelayed(hObject);
    }
  }
}

void FlyBirdGameState::ConfigureMainCamera()
{
  SUPER::ConfigureMainCamera();

  // Do custom camera setup here
}
