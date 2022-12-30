#pragma once

#include <Core/World/Component.h>
#include <Core/World/ComponentManager.h>
#include <Core/World/World.h>
#include <FlyBirdPlugin/FlyBirdPluginDLL.h>

class xiiMsgTriggerTriggered;

using ObstacleComponentManager = xiiComponentManagerSimple<class ObstacleComponent, xiiComponentUpdateType::WhenSimulating>;

class ObstacleComponent : public xiiComponent
{
  XII_DECLARE_COMPONENT_TYPE(ObstacleComponent, xiiComponent, ObstacleComponentManager);

  //////////////////////////////////////////////////////////////////////////
  // xiiComponent

public:
  virtual void SerializeComponent(xiiWorldWriter& stream) const override;
  virtual void DeserializeComponent(xiiWorldReader& stream) override;

protected:
  virtual void OnSimulationStarted() override;

  //////////////////////////////////////////////////////////////////////////
  // ObstacleComponent

public:
  ObstacleComponent();
  ~ObstacleComponent();

private:

  void Update();

  float m_fSpeed = 1.0f;

  xiiVec2 m_vRandomHeightRange;
};
