#pragma once

#include <Core/Input/Declarations.h>
#include <Core/World/Declarations.h>
#include <FlyBirdPlugin/FlyBirdPluginDLL.h>
#include <GameEngine/GameApplication/GameApplication.h>
#include <GameEngine/GameState/FallbackGameState.h>
#include <GameEngine/GameState/GameState.h>

class FlyBirdGameState : public xiiFallbackGameState
{
  XII_ADD_DYNAMIC_REFLECTION(FlyBirdGameState, xiiFallbackGameState);

public:
  FlyBirdGameState();
  ~FlyBirdGameState();

  virtual xiiGameStatePriority DeterminePriority(xiiWorld* pWorld) const override;

  virtual void ProcessInput() override;

protected:
  virtual void ConfigureMainWindowInputDevices(xiiWindow* pWindow) override;
  virtual void ConfigureInputActions() override;
  virtual void ConfigureMainCamera() override;

private:
  virtual void OnActivation(xiiWorld* pWorld, const xiiTransform* pStartPosition) override;
  virtual void OnDeactivation() override;
  virtual void BeforeWorldUpdate() override;
  virtual void AfterWorldUpdate() override;
};
