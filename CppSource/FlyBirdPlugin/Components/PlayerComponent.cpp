#include <FlyBirdPlugin/FlyBirdPluginPCH.h>

#include <Core/Messages/TriggerMessage.h>
#include <Core/WorldSerializer/WorldReader.h>
#include <Core/WorldSerializer/WorldWriter.h>
#include <FlyBirdPlugin/Components/PlayerComponent.h>
#include <Foundation/Utilities/ConversionUtils.h>

// clang-format off
XII_BEGIN_COMPONENT_TYPE(PlayerComponent, 1 /* version */, xiiComponentMode::Dynamic)
{
  // XII_BEGIN_PROPERTIES
  // {
  // }
  // XII_END_PROPERTIES;

  XII_BEGIN_ATTRIBUTES
  {
    new xiiCategoryAttribute("FlyBird"), // Component menu group
  }
  XII_END_ATTRIBUTES;

  XII_BEGIN_MESSAGEHANDLERS
  {
    XII_MESSAGE_HANDLER(xiiMsgTriggerTriggered, OnTriggerActivated)
  }
  XII_END_MESSAGEHANDLERS;
}
XII_END_COMPONENT_TYPE
// clang-format on

PlayerComponent::PlayerComponent()  = default;
PlayerComponent::~PlayerComponent() = default;

void PlayerComponent::OnSimulationStarted()
{
  SUPER::OnSimulationStarted();
}

void PlayerComponent::Update()
{
}

void PlayerComponent::OnTriggerActivated(xiiMsgTriggerTriggered& message)
{
  xiiLog::Info("Activated Trigger {0}", message.m_sMessage);
}

void PlayerComponent::SerializeComponent(xiiWorldWriter& stream) const
{
  SUPER::SerializeComponent(stream);

  auto& s = stream.GetStream();
}

void PlayerComponent::DeserializeComponent(xiiWorldReader& stream)
{
  SUPER::DeserializeComponent(stream);
  const xiiUInt32 uiVersion = stream.GetComponentTypeVersion(GetStaticRTTI());

  auto& s = stream.GetStream();
}
