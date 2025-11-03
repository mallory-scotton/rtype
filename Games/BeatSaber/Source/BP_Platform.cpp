///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <BP_Platform.hpp>
#include <AC_DebugGrid.hpp>
#include <AC_FireflyParticles.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_Platform::BP_Platform(const FString& name)
    : AActor(name)
{
    // Add Components
    AddComponent<UPlaneComponent>("PL_PlatformPlane");
    AddComponent<AC_DebugGrid>("DCG_PlatformDebugGrid");
    AddComponent<USphereComponent>("SKY_Sphere");
    AddComponent<AC_FireflyParticles>("FP_PlatformFireflies");
}

///////////////////////////////////////////////////////////////////////////////
void BP_Platform::BeginPlay(void)
{
    Super::BeginPlay();

    // Initialization of the platform
    auto planeComp = GetComponent<UPlaneComponent>("PL_PlatformPlane");
    if (planeComp)
    {
        auto& plane = planeComp->GetPlane();
        plane.SetScale(FVector3(5.0f, 0.5f, 1.0f));
        plane.SetColor(FColor::Blue);
    }

    // Setup sky sphere
    auto skyComp = GetComponent<USphereComponent>("SKY_Sphere");
    if (skyComp)
    {
        FTransform transform = FTransform::Identity;
        transform.Scale(FVector3(50.0f));
        skyComp->SetLocalTransform(transform);
        skyComp->GetSphere().SetColor(FColor(0.1f, 0.1f, 0.2f));
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Platform, AActor)

}   // namespace tkd
