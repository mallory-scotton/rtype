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

    // Note rails
    AddComponent<UCubeComponent>("CB_NoteRail");
    AddComponent<UCubeComponent>("CB_NotePillar1");
    AddComponent<UCubeComponent>("CB_NotePillar2");

    // Color Variation Rail
    AddComponent<UCubeComponent>("CB_ColorVariationPillar1");
    AddComponent<UCubeComponent>("CB_ColorVariationPillar2");
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
        transform.Scale(FVector3(3.0f, 0.5f, 2.0f));
        planeComp->SetLocalTransform(transform);
        planeComp->GetPlane().SetColor(FColor::Black);

        transform.Translate(FVector3(0.f, -0.001f, 0.f));
        transform.SetScale(FVector3(3.1f, 0.5f, 2.1f));
        outlineComp->SetLocalTransform(transform);
        outlineComp->GetPlane().SetColor(FLinearColor(2, 112, 241).ToColor());
    }

    auto noteRail = GetComponent<UCubeComponent>("CB_NoteRail");
    if (noteRail)
    {
        FTransform transform = FTransform::Identity;
        transform.Scale(FVector3(5.f, 0.5f, 1000.0f));
        transform.SetPosition(FVector3(0.f, -0.5f, -510.f));
        noteRail->SetLocalTransform(transform);
        noteRail->GetCube().SetColor(FLinearColor(5, 5, 5).ToColor());
    }

    auto pillar1 = GetComponent<UCubeComponent>("CB_NotePillar1");
    auto pillar2 = GetComponent<UCubeComponent>("CB_NotePillar2");
    if (pillar1 && pillar2)
    {
        FTransform transform = FTransform::Identity;
        transform.Scale(FVector3(0.5f, 100.0f, 0.5f));
        transform.SetPosition(FVector3(-2.25f, -50.5f, -10.25f));
        pillar1->SetLocalTransform(transform);
        pillar1->GetCube().SetColor(FLinearColor(5, 5, 5).ToColor());

        transform.SetPosition(FVector3(2.25f, -50.5f, -10.25f));
        pillar2->SetLocalTransform(transform);
        pillar2->GetCube().SetColor(FLinearColor(5, 5, 5).ToColor());
    }

    auto cvp1 = GetComponent<UCubeComponent>("CB_ColorVariationPillar1");
    auto cvp2 = GetComponent<UCubeComponent>("CB_ColorVariationPillar2");
    if (cvp1 && cvp2)
    {
        FTransform transform = FTransform::Identity;
        transform.Scale(FVector3(0.2f, 100.0f, 0.2f));
        transform.SetPosition(FVector3(-4.5f, -50.f, -10.05f));
        cvp1->SetLocalTransform(transform);
        cvp1->GetCube().SetColor(FLinearColor(5, 5, 5).ToColor());

        transform.SetPosition(FVector3(4.5f, -50.f, -10.05f));
        cvp2->SetLocalTransform(transform);
        cvp2->GetCube().SetColor(FLinearColor(5, 5, 5).ToColor());
    }

    // Setup sky sphere
    auto skyComp = GetComponent<USphereComponent>("SKY_Sphere");
    if (skyComp)
    {
        FTransform transform = FTransform::Identity;
        transform.Scale(FVector3(100.0f));
        skyComp->SetLocalTransform(transform);
        skyComp->GetSphere().SetColor(FColor(0.1f, 0.1f, 0.2f));
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Platform, AActor)

}   // namespace tkd
