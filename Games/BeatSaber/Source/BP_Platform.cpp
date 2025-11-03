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
    AddComponent<UPlaneComponent>("PL_PlatformPlaneOutline");
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
    auto outlineComp =
        GetComponent<UPlaneComponent>("PL_PlatformPlaneOutline");
    if (planeComp && outlineComp)
    {
        FTransform transform = FTransform::Identity;
        transform.Scale(FVector3(4.0f, 0.5f, 3.0f));
        planeComp->SetLocalTransform(transform);
        planeComp->GetPlane().SetColor(FColor::Black);

        transform.Translate(FVector3(0.f, -0.001f, 0.f));
        outlineComp->SetLocalTransform(transform);
        outlineComp->GetPlane().SetColor(FColor::Blue);
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
