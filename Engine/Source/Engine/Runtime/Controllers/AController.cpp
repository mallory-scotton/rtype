///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Controllers/AController.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
AController::AController(const FString& name)
    : AActor(name)
    , m_pawn(nullptr)
    , PossessRPC(
          *this,
          "PossessRPC",
          ERPCType::Client,
          std::bind(&AController::RPC_Possess, this, std::placeholders::_1)
      )
    , UnPossessRPC(
          *this,
          "UnPossessRPC",
          ERPCType::Client,
          std::bind(&AController::RPC_UnPossess, this)
      )
{}

///////////////////////////////////////////////////////////////////////////////
AController::~AController()
{
    if (m_pawn) { UnPossess(); }
}

///////////////////////////////////////////////////////////////////////////////
void AController::Possess(APawn* pawn)
{
    if (m_pawn == pawn) { return; }

    // Unpossess current pawn if any
    if (m_pawn != nullptr) { UnPossess(); }

    m_pawn = pawn;
    if (pawn)
    {
        // Emit the Possess event
        Emit(Events::Possess{ .pawn = pawn });
    }
}

///////////////////////////////////////////////////////////////////////////////
void AController::UnPossess(void)
{
    if (m_pawn == nullptr) { return; }

    APawn* oldPawn = m_pawn;
    m_pawn = nullptr;
    // Emit the UnPossess event
    Emit(Events::UnPossess{ .pawn = oldPawn });
}

///////////////////////////////////////////////////////////////////////////////
APawn* AController::GetPawn(void) const { return m_pawn; }

///////////////////////////////////////////////////////////////////////////////
void AController::RPC_Possess(const UUID& pawnID)
{
    UObject* obj = UObject::FindByUUID(pawnID);
    if (obj == nullptr) { return; }
    APawn* pawn = obj->As<APawn>();
    if (pawn == nullptr) { return; }
    Possess(pawn);
}

///////////////////////////////////////////////////////////////////////////////
void AController::RPC_UnPossess(void) { UnPossess(); }

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(AController, AActor)

}   // namespace tkd
