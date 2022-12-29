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
    XII_MEMBER_PROPERTY("GlobalKey", m_szGlobalKey)->AddAttributes(new xiiDefaultValueAttribute("")),
    XII_MEMBER_PROPERTY("ObstacleCount", m_iObstacleCount)->AddAttributes(new xiiDefaultValueAttribute(10), new xiiClampValueAttribute(1, 20)),
    XII_MEMBER_PROPERTY("OffsetDistance", m_fOffsetDistance)->AddAttributes(new xiiDefaultValueAttribute(xiiVec2(30.0, 75.0f))),
    XII_MEMBER_PROPERTY("HeightRandomness", m_vHeightRandomness)->AddAttributes(new xiiDefaultValueAttribute(xiiVec2(-60, 60))),
    XII_MEMBER_PROPERTY("Veclocity", m_fVeclocity)->AddAttributes(new xiiDefaultValueAttribute(1.0f)),
    XII_MEMBER_PROPERTY("DestroyDistance", m_fDestroyDistance)->AddAttributes(new xiiDefaultValueAttribute(100.0f)),
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

  // Retrieve player handle
  {
    xiiGameObject* pObject = nullptr;
    if (GetWorld()->TryGetObjectWithGlobalKey(m_szGlobalKey.GetView(), pObject))
    {
      m_hTargetHandle = pObject->GetHandle();
    }
    else
    {
      xiiLog::Error("Failed to get player handle with key {0}. The component will be disabled.", m_szGlobalKey);
      SetActiveFlag(false);
      return;
    }
  }

  // Retrieve obstacle handles
  m_ObstacleHandles.Reserve(m_iObstacleCount);

  xiiStringBuilder sName;
  xiiUInt16        uiLoadedObstacles = 0;

  for (xiiUInt32 i = 0; i < m_iObstacleCount; ++i)
  {
    sName.Format("Obstacle-{0}", i);

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
  // Retrieve player object
  xiiGameObject* pTargetObject = nullptr;
  if (!GetWorld()->TryGetObject(m_hTargetHandle, pTargetObject))
  {
    xiiLog::Error("Failed to get player handle with key {0}. The component will be disabled.", m_szGlobalKey);
    SetActiveFlag(false);
    return;
  }

  xiiVec3 targetPosition = pTargetObject->GetGlobalPosition();

  // Loop through obstacles and update the ones that exceed the destroy distance threshold
  for (xiiUInt32 i = 0; i < m_ObstacleHandles.GetCount(); i++)
  {
    xiiGameObject* pObject = nullptr;
    if (GetWorld()->TryGetObject(m_ObstacleHandles[i], pObject))
    {
      xiiVec3 objectPosition = pObject->GetGlobalPosition();

      // Compute new position
      if (objectPosition.x < targetPosition.x - m_fDestroyDistance)
      {
        // Retrieve the position of the farthest object in front of the player
        xiiVec3 farthestObjectPosition;
        {
          xiiGameObject* pLastObject = nullptr;
          if (!GetWorld()->TryGetObject(m_ObstacleHandles[m_ObstacleHandles.GetCount() - 1], pLastObject))
          {
            xiiLog::Error("Failed to get last object. The component will be disabled.");
            SetActiveFlag(false);
            return;
          }

          farthestObjectPosition = pLastObject->GetGlobalPosition();
        }

        // Offset the obstacle position to recycle obstacle object.
        objectPosition.x = farthestObjectPosition.x + (GetWorld()->GetRandomNumberGenerator().FloatInRange(m_fOffsetDistance.x, m_fOffsetDistance.y) * (i + 1));

        // Randomize the height of the object.
        objectPosition.z = GetWorld()->GetRandomNumberGenerator().FloatMinMax(m_vHeightRandomness.x, m_vHeightRandomness.y);

        // Update object position
        pObject->SetGlobalPosition(objectPosition);
      }
    }
    else
    {
      xiiLog::Error("Failed to retrieve obstacle with global key {0}", i);
      continue;
    }
  }
}

void ObstacleManagerComponent::SerializeComponent(xiiWorldWriter& stream) const
{
  SUPER::SerializeComponent(stream);

  auto& s = stream.GetStream();

  s << m_iObstacleCount;
  s << m_fDestroyDistance;
  s << m_fOffsetDistance;
  s << m_fVeclocity;
  s << m_vHeightRandomness;
  s << m_szGlobalKey;
}

void ObstacleManagerComponent::DeserializeComponent(xiiWorldReader& stream)
{
  SUPER::DeserializeComponent(stream);
  const xiiUInt32 uiVersion = stream.GetComponentTypeVersion(GetStaticRTTI());

  auto& s = stream.GetStream();

  s >> m_iObstacleCount;
  s >> m_fDestroyDistance;
  s >> m_fOffsetDistance;
  s >> m_fVeclocity;
  s >> m_vHeightRandomness;
  s >> m_szGlobalKey;
}
