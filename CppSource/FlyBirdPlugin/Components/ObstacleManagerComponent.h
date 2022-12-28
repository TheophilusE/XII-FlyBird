#pragma once

#include <Core/World/Component.h>
#include <Core/World/ComponentManager.h>
#include <Core/World/World.h>
#include <FlyBirdPlugin/FlyBirdPluginDLL.h>

using ObstacleManagerComponentManager = xiiComponentManagerSimple<class ObstacleManagerComponent, xiiComponentUpdateType::WhenSimulating>;

class ObstacleManagerComponent : public xiiComponent
{
  XII_DECLARE_COMPONENT_TYPE(ObstacleManagerComponent, xiiComponent, ObstacleManagerComponentManager);

  //////////////////////////////////////////////////////////////////////////
  // xiiComponent

public:
  virtual void SerializeComponent(xiiWorldWriter& stream) const override;
  virtual void DeserializeComponent(xiiWorldReader& stream) override;

protected:
  virtual void OnSimulationStarted() override;

  //////////////////////////////////////////////////////////////////////////
  // ObstacleManagerComponent

public:
  ObstacleManagerComponent();
  ~ObstacleManagerComponent();

private:

  void Update();

  xiiInt32 m_iObstacleCount;

  xiiDynamicArray<xiiGameObjectHandle> m_ObstacleHandles;
};
