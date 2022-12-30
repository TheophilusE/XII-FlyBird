#pragma once

#include <Core/World/Component.h>
#include <Core/World/ComponentManager.h>
#include <Core/World/World.h>
#include <FlyBirdPlugin/FlyBirdPluginDLL.h>

class xiiMsgTriggerTriggered;

using PlayerComponentManager = xiiComponentManagerSimple<class PlayerComponent, xiiComponentUpdateType::WhenSimulating>;

class PlayerComponent : public xiiComponent
{
  XII_DECLARE_COMPONENT_TYPE(PlayerComponent, xiiComponent, PlayerComponentManager);

  //////////////////////////////////////////////////////////////////////////
  // xiiComponent

public:
  virtual void SerializeComponent(xiiWorldWriter& stream) const override;
  virtual void DeserializeComponent(xiiWorldReader& stream) override;

protected:
  virtual void OnSimulationStarted() override;

  //////////////////////////////////////////////////////////////////////////
  // PlayerComponent

public:
  PlayerComponent();
  ~PlayerComponent();

protected:

  void OnTriggerActivated(xiiMsgTriggerTriggered& message);

private:

  void Update();
};
