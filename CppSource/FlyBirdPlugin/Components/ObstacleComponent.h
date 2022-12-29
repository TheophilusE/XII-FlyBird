#pragma once

#include <Core/World/Component.h>
#include <Core/World/ComponentManager.h>
#include <Core/World/World.h>
#include <FlyBirdPlugin/FlyBirdPluginDLL.h>

using FollowCameraComponentManager = xiiComponentManagerSimple<class ObstacleComponent, xiiComponentUpdateType::WhenSimulating>;

class ObstacleComponent : public xiiComponent
{
  XII_DECLARE_COMPONENT_TYPE(ObstacleComponent, xiiComponent, FollowCameraComponentManager);

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
