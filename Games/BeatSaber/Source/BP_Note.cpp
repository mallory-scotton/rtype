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
BP_Note::BP_Note(ENoteType type, ECutDirection cutDirection)
    : AActor("BP_Note")
    , m_type(type)
    , m_cutDirection(cutDirection)
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
        // switch (m_type)
        // {
        // case ENoteType::LeftHand : cube = UCubePrimitive(FColor::Red);
        // break; case ENoteType::RightHand: cube =
        // UCubePrimitive(FColor::Blue); break; case ENoteType::Bomb     : cube
        // = UCubePrimitive(FColor::Black); break; default                  :
        // cube = UCubePrimitive(FColor::White); break;
        // }
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

    static float acc = 0.f;
    acc += deltaTime;
    FTransform transform;
    transform.SetPosition(FVector3(std::sin(acc) * 5.f, 0.f, 0.f));
    SetTransform(transform);
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Note, AActor)

}   // namespace tkd
