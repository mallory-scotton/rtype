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
    AddComponent<UCubeComponent>("SM_Cube");
}

///////////////////////////////////////////////////////////////////////////////
void BP_Note::BeginPlay(void)
{
    // Call the Begin Play of the Super Class
    Super::BeginPlay();

    // Setup color based on the note type
    auto cubeComp = GetComponent<UCubeComponent>("SM_Cube");
    if (cubeComp)
    {
        auto& cube = cubeComp->GetCube();
        switch (m_type)
        {
        case ENoteType::LeftHand : cube = UCubePrimitive(FColor::Red); break;
        case ENoteType::RightHand: cube = UCubePrimitive(FColor::Blue); break;
        case ENoteType::Bomb     : cube = UCubePrimitive(FColor::Black); break;
        default                  : cube = UCubePrimitive(FColor::White); break;
        }
        m_handle =
            URessource::GetTextureHandle("Assets/Textures/GridBox_Default.png"
            );
        cube.SetTexture(m_handle.Get());
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
