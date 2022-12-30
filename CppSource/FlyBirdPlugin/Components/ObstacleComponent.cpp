#include <FlyBirdPlugin/FlyBirdPluginPCH.h>

#include <Core/WorldSerializer/WorldReader.h>
#include <Core/WorldSerializer/WorldWriter.h>
#include <FlyBirdPlugin/Components/ObstacleComponent.h>
#include <Foundation/Utilities/ConversionUtils.h>
#include <JoltPlugin/Actors/JoltTriggerComponent.h>

// clang-format off
XII_BEGIN_COMPONENT_TYPE(ObstacleComponent, 1 /* version */, xiiComponentMode::Dynamic)
{
  XII_BEGIN_PROPERTIES
  {
    XII_MEMBER_PROPERTY("Speed", m_fSpeed)->AddAttributes(new xiiDefaultValueAttribute(1.0f)),
    XII_MEMBER_PROPERTY("RandomHeightRange", m_vRandomHeightRange)->AddAttributes(new xiiDefaultValueAttribute(xiiVec2(0.0f, 1.0f)))
  }
  XII_END_PROPERTIES;

  XII_BEGIN_ATTRIBUTES
  {
    new xiiCategoryAttribute("FlyBird"), // Component menu group
  }
  XII_END_ATTRIBUTES;
}
XII_END_COMPONENT_TYPE
// clang-format on

ObstacleComponent::ObstacleComponent()  = default;
ObstacleComponent::~ObstacleComponent() = default;

void ObstacleComponent::OnSimulationStarted()
{
  SUPER::OnSimulationStarted();

  // Assign a random height to the object
  xiiVec3 currentPosition = GetOwner()->GetGlobalPosition();

  float randomHeight = GetWorld()->GetRandomNumberGenerator().FloatInRange(m_vRandomHeightRange.x, m_vRandomHeightRange.y);
  currentPosition.z += randomHeight;

#if 0
  xiiLog::Info("Spawned obstacle with random height {0}, Current Position Z {1}", randomHeight, currentPosition.z);
#endif

  GetOwner()->SetGlobalPosition(currentPosition);
}

void ObstacleComponent::Update()
{
  // Update position
  xiiVec3 objectPosition = GetOwner()->GetGlobalPosition();

  // Enforce Y position at zero
  objectPosition.y = 0.0f;

  objectPosition.x -= (m_fSpeed * GetWorld()->GetClock().GetTimeDiff().AsFloatInSeconds());

#if 0
  xiiLog::Info("Current Time Diff {0}, Current Object Pos {1}, Current Speed {2}", GetWorld()->GetClock().GetTimeDiff().AsFloatInSeconds(), objectPosition.x, m_fSpeed);
#endif

  GetOwner()->SetGlobalPosition(objectPosition);

  // Destroy object if close to the world origin
  if (GetOwner()->GetGlobalPosition().DistanceSquared(xiiVec3(0)) < 20)
  {
    GetWorld()->DeleteObjectDelayed(GetOwner()->GetHandle());
  }

  // Update player score based if player triggers the middle zone
  xiiJoltTriggerComponent* pTriggerMiddle = nullptr;
  if (!GetOwner()->FindChildByName("Entrance")->TryGetComponentOfBaseType(pTriggerMiddle))
  {
    xiiLog::Info("Failed to retrieve obstacle middle trigger");
    return;
  }
}

void ObstacleComponent::SerializeComponent(xiiWorldWriter& stream) const
{
  SUPER::SerializeComponent(stream);

  auto& s = stream.GetStream();

  s << m_fSpeed;
  s << m_vRandomHeightRange;
}

void ObstacleComponent::DeserializeComponent(xiiWorldReader& stream)
{
  SUPER::DeserializeComponent(stream);
  const xiiUInt32 uiVersion = stream.GetComponentTypeVersion(GetStaticRTTI());

  auto& s = stream.GetStream();

  s >> m_fSpeed;
  s >> m_vRandomHeightRange;
}
