#include <FlyBirdPlugin/FlyBirdPluginPCH.h>

#include <Core/WorldSerializer/WorldReader.h>
#include <Core/WorldSerializer/WorldWriter.h>
#include <FlyBirdPlugin/Components/ObstacleManagerComponent.h>
#include <Foundation/Utilities/ConversionUtils.h>
#include <RendererCore/Components/CameraComponent.h>

// clang-format off
XII_BEGIN_COMPONENT_TYPE(ObstacleManagerComponent, 1 /* version */, xiiComponentMode::Dynamic)
{
  XII_BEGIN_PROPERTIES
  {
    XII_MEMBER_PROPERTY("ObstacleCount", m_iObstacleCount)->AddAttributes(new xiiDefaultValueAttribute(10), new xiiClampValueAttribute(1, 20))
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

ObstacleManagerComponent::ObstacleManagerComponent()  = default;
ObstacleManagerComponent::~ObstacleManagerComponent() = default;

void ObstacleManagerComponent::OnSimulationStarted()
{
  SUPER::OnSimulationStarted();

  // Retrieve obstacle handles
  m_ObstacleHandles.Reserve(m_iObstacleCount);

  xiiStringBuilder sName;
  xiiUInt16        uiLoadedObstacles = 0;

  for (xiiUInt32 i = 0; i < m_iObstacleCount; ++i)
  {
    sName.Format("Obstacle-{0}", i);

    xiiLog::Info(sName);

    xiiGameObject* pObject = nullptr;
    if (GetWorld()->TryGetObjectWithGlobalKey(sName.GetView(), pObject))
    {
      m_ObstacleHandles.PushBack(pObject->GetHandle());
      ++uiLoadedObstacles;
    }
    else
    {
      xiiLog::Error("Failed to retrieve obstacle with global key {0}", i);
    }
  }

  m_ObstacleHandles.SetCount(uiLoadedObstacles);
}

void ObstacleManagerComponent::Update()
{
}

void ObstacleManagerComponent::SerializeComponent(xiiWorldWriter& stream) const
{
  SUPER::SerializeComponent(stream);

  auto& s = stream.GetStream();

  s << m_iObstacleCount;
}

void ObstacleManagerComponent::DeserializeComponent(xiiWorldReader& stream)
{
  SUPER::DeserializeComponent(stream);
  const xiiUInt32 uiVersion = stream.GetComponentTypeVersion(GetStaticRTTI());

  auto& s = stream.GetStream();

  s >> m_iObstacleCount;
}