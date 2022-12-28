#pragma once

#include <Core/World/Component.h>
#include <Core/World/ComponentManager.h>
#include <Core/World/World.h>
#include <FlyBirdPlugin/FlyBirdPluginDLL.h>

using FollowCameraComponentManager = xiiComponentManagerSimple<class FollowCameraComponent, xiiComponentUpdateType::WhenSimulating>;

class FollowCameraComponent : public xiiComponent
{
  XII_DECLARE_COMPONENT_TYPE(FollowCameraComponent, xiiComponent, FollowCameraComponentManager);

  //////////////////////////////////////////////////////////////////////////
  // xiiComponent

public:
  virtual void SerializeComponent(xiiWorldWriter& stream) const override;
  virtual void DeserializeComponent(xiiWorldReader& stream) override;

protected:
  virtual void OnSimulationStarted() override;

  //////////////////////////////////////////////////////////////////////////
  // FollowCameraComponent

public:
  FollowCameraComponent();
  ~FollowCameraComponent();

private:

  void Update();

  float m_fSpeed = 0.0;

  xiiString m_szGlobalKey;

  xiiVec3 m_vPositionOffset;

  xiiGameObjectHandle m_hTargetHandle;
};
