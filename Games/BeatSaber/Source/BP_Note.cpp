///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <BP_Note.hpp>
#include <AC_NoteArrow.hpp>
#include <AC_SwordBlade.hpp>
#include <BP_Sword.hpp>
#include <Engine/Assets/URessource.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_Note::BP_Note(ENoteType type, ECutDirection cutDirection, float speed)
    : AActor("BP_Note")
    , m_type(type)
    , m_cutDirection(cutDirection)
    , m_speed(speed)
{
    AddComponent<UChamferCubeComponent>("SM_Cube");
    AddComponent<UBoxCollisionComponent>("BC_CutCollision");
    AddComponent<UBoxCollisionComponent>("BC_BadCutCollision");

    // Add note arrow component for directional indicator
    auto arrowType = (cutDirection == ECutDirection::None)
                         ? AC_NoteArrow::EType::Dot
                         : AC_NoteArrow::EType::Arrow;
    AddComponent<AC_NoteArrow>("AC_NoteArrow", arrowType);
}

///////////////////////////////////////////////////////////////////////////////
void BP_Note::BeginPlay(void)
{
    // Call the Begin Play of the Super Class
    Super::BeginPlay();

    {
        // Rotate the note based on cut direction
        FTransform transform = GetTransform();
        switch (m_cutDirection)
        {
        case ECutDirection::Up  : break;
        case ECutDirection::None: break;
        case ECutDirection::Down:
            transform.SetRotation(TRotator(0.f, 180.f, 0.f));
            break;
        case ECutDirection::Left:
            transform.SetRotation(TRotator(0.f, -90.f, 0.f));
            break;
        case ECutDirection::Right:
            transform.SetRotation(TRotator(0.f, 90.f, 0.f));
            break;
        case ECutDirection::UpLeft:
            transform.SetRotation(TRotator(0.f, -45.f, 0.f));
            break;
        case ECutDirection::UpRight:
            transform.SetRotation(TRotator(0.f, 45.f, 0.f));
            break;
        case ECutDirection::DownLeft:
            transform.SetRotation(TRotator(0.f, -135.f, 0.f));
            break;
        case ECutDirection::DownRight:
            transform.SetRotation(TRotator(0.f, 135.f, 0.f));
            break;
        default: break;
        }
        SetTransform(transform);
    }

    // Setup color based on the note type
    auto cubeComp = GetComponent<UChamferCubeComponent>("SM_Cube");
    if (cubeComp)
    {
        auto& cube = cubeComp->GetChamferCube();
        switch (m_type)
        {
        case ENoteType::LeftHand:
            cube = UChamferCubePrimitive(FLinearColor(215, 12, 16).ToColor());
            break;
        case ENoteType::RightHand:
            cube = UChamferCubePrimitive(FLinearColor(2, 112, 241).ToColor());
            break;
        case ENoteType::Bomb:
            cube = UChamferCubePrimitive(FColor::Black);
            break;
        default: cube = UChamferCubePrimitive(FColor::White); break;
        }
        FTransform transform = cubeComp->GetLocalTransform();
        transform.SetScale(FVector3(0.5f, 0.5f, 0.5f));
        cubeComp->SetLocalTransform(transform);
    }

    // Setup collision box sizes
    auto cutCollision =
        GetComponent<UBoxCollisionComponent>("BC_CutCollision");
    if (cutCollision)
    {
        cutCollision->SetBoxExtent(FVector3f(0.3f, 0.3f, 0.6f));
        cutCollision->SetHiddenInGame(false);
        FTransform transform = cutCollision->GetLocalTransform();
        transform.SetPosition(FVector3(0.f, 0.f, 0.2f));
        cutCollision->SetLocalTransform(transform);

        auto* cs = cutCollision->GetCollisionSystem();
        if (cs)
        {
            cs->BindOnOverlapBegin(
                cutCollision,
                [this](const FCollisionInfo& info)
                {
                    if (info.otherActor && info.otherActor->Is<BP_Sword>() &&
                        info.otherComponent &&
                        info.otherComponent->Is<AC_SwordBlade>())
                    {
                        // Note hit by sword - handle scoring, effects, etc.
                        // here
                        MarkForDeletion();

#if TKD_ENGINE_CLIENT
                        auto& vr = Window::GetVRSystem();
                        vr.TriggerHapticPulse(
                            info.otherActor->As<BP_Sword>()->GetHand(), 0.1f
                        );
#endif
                    }
                }
            );
        }
    }

    auto badCutCollision =
        GetComponent<UBoxCollisionComponent>("BC_BadCutCollision");
    if (badCutCollision)
    {
        badCutCollision->SetBoxExtent(FVector3f(0.2f, 0.2f, 0.2f));
        badCutCollision->SetHiddenInGame(false);
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Note::Tick(float deltaTime)
{
    // Call the Tick of the Super Class
    Super::Tick(deltaTime);

    // Move the note toward the player using the configured speed
    FTransform transform = GetTransform();
    transform.Translate(FVector3(0.f, 0.f, deltaTime * m_speed));
    SetTransform(transform);

    // Delete note when it passes the player position
    if (transform.GetPosition().z > 2.0f) { MarkForDeletion(); }
}

///////////////////////////////////////////////////////////////////////////////
ENoteType BP_Note::GetNoteType(void) const { return m_type; }

///////////////////////////////////////////////////////////////////////////////
ECutDirection BP_Note::GetCutDirection(void) const { return m_cutDirection; }

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Note, AActor)

}   // namespace tkd
