///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <BP_Note.hpp>
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
}

///////////////////////////////////////////////////////////////////////////////
void BP_Note::BeginPlay(void)
{
    // Call the Begin Play of the Super Class
    Super::BeginPlay();

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
IMPLEMENT_CLASS_WITH_SUPER(BP_Note, AActor)

}   // namespace tkd
