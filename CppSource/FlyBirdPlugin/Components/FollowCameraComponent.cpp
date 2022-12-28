#include <FlyBirdPlugin/FlyBirdPluginPCH.h>

#include <Core/WorldSerializer/WorldReader.h>
#include <Core/WorldSerializer/WorldWriter.h>
#include <FlyBirdPlugin/Components/FollowCameraComponent.h>
#include <Foundation/Utilities/ConversionUtils.h>
#include <RendererCore/Components/CameraComponent.h>

// clang-format off
XII_BEGIN_COMPONENT_TYPE(FollowCameraComponent, 1 /* version */, xiiComponentMode::Dynamic)
{
  XII_BEGIN_PROPERTIES
  {
    XII_MEMBER_PROPERTY("GlobalKey", m_szGlobalKey)->AddAttributes(new xiiDefaultValueAttribute("")),
    XII_MEMBER_PROPERTY("Speed", m_fSpeed)->AddAttributes(new xiiDefaultValueAttribute(1.0f), new xiiClampValueAttribute(0.0f, 1.0f)),
    XII_MEMBER_PROPERTY("PositionOffset", m_vPositionOffset)->AddAttributes(new xiiDefaultValueAttribute(xiiVec3(0.0f)))
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

FollowCameraComponent::FollowCameraComponent()  = default;
FollowCameraComponent::~FollowCameraComponent() = default;

void FollowCameraComponent::OnSimulationStarted()
{
  SUPER::OnSimulationStarted();

  // Verify if there is an attached component
  xiiCameraComponent* pCameraComponent = nullptr;
  if (!GetOwner()->TryGetComponentOfBaseType(pCameraComponent))
  {
    xiiLog::Error("Camera Component not found.");
    SetActiveFlag(false);
    return;
  }

  // Retrieve tracked object handle
  xiiGameObject* pTrackedObject = nullptr;
  if (GetWorld()->TryGetObjectWithGlobalKey(m_szGlobalKey.GetView(), pTrackedObject))
  {
    m_hTargetHandle = pTrackedObject->GetHandle();
  }
  else
  {
    xiiLog::Error("Failed to find tracked object with global key '{}'", m_szGlobalKey);
    SetActiveFlag(false);
    return;
  }
}

void FollowCameraComponent::Update()
{
  if (m_hTargetHandle.IsInvalidated())
  {
    xiiLog::Error("Invalid tracked provided! Component will be disabled.");
    SetActiveFlag(false);
    return;
  }

  // Retrieve tracked object
  xiiGameObject* pTrackedObject = nullptr;
  if (!GetWorld()->TryGetObject(m_hTargetHandle, pTrackedObject))
  {
    xiiLog::Error("Tracked object is null! Component will be disabled.");
    SetActiveFlag(false);
    return;
  }

  // Update position
  xiiVec3 currentPosition = GetOwner()->GetGlobalPosition();
  xiiVec3 targetPosition  = pTrackedObject->GetGlobalPosition() + m_vPositionOffset;
  xiiVec3 newPosition     = xiiVec3(xiiMath::Lerp(currentPosition.x, targetPosition.x, m_fSpeed), xiiMath::Lerp(currentPosition.y, targetPosition.y, m_fSpeed), xiiMath::Lerp(currentPosition.z, targetPosition.z, m_fSpeed));
  GetOwner()->SetGlobalPosition(newPosition);

#if 0
  xiiStringBuilder sData;

  xiiConversionUtils::ToString(targetPosition, sData);
  xiiLog::Info("Target Object Position {}", sData.GetData());

  xiiConversionUtils::ToString(currentPosition, sData);
  xiiLog::Info("Current Object Position {}", sData.GetData());

  xiiConversionUtils::ToString(newPosition, sData);
  xiiLog::Info("New Object Position {}", sData.GetData());
#endif
}

void FollowCameraComponent::SerializeComponent(xiiWorldWriter& stream) const
{
  SUPER::SerializeComponent(stream);

  auto& s = stream.GetStream();

  s << m_fSpeed;
  s << m_szGlobalKey;
  s << m_vPositionOffset;
}

void FollowCameraComponent::DeserializeComponent(xiiWorldReader& stream)
{
  SUPER::DeserializeComponent(stream);
  const xiiUInt32 uiVersion = stream.GetComponentTypeVersion(GetStaticRTTI());

  auto& s = stream.GetStream();

  s >> m_fSpeed;
  s >> m_szGlobalKey;
  s >> m_vPositionOffset;
}
