///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Levels/L_RType_Level1.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
L_RType_Level1::L_RType_Level1()
    : Super("L_RType_Level1")
{
    SetLevelName("L_RType_Level1");
    SetGameMode("RTypeGameMode");
    AddActorEntry("BP_Background", "Background1", FTransform::Identity);
    AddActorEntry(
        "BP_MonsterSpawner", "MonsterSpawner1", FTransform::Identity
    );
    FTransform spawnerTransform1;
    spawnerTransform1.SetPosition(FVector3(2.6f, -3.4f, 0.0f));
    AddActorEntry("BP_MonsterSpawner", "MonsterSpawner2", spawnerTransform1);
    // FTransform spawnerTransform2;
    // spawnerTransform2.SetPosition(FVector3(1.1f, 4.4f, 0.0f));
    // AddActorEntry("BP_MonsterSpawner", "MonsterSpawner3",
    // spawnerTransform2); FTransform spawnerTransform3;
    // spawnerTransform3.SetPosition(FVector3(5.7f, -0.1f, 0.0f));
    // AddActorEntry("BP_MonsterSpawner", "MonsterSpawner4",
    // spawnerTransform3); FTransform spawnerTransform4;
    // spawnerTransform4.SetPosition(FVector3(-3.5f, 0.8f, 0.0f));
    // AddActorEntry("BP_MonsterSpawner", "MonsterSpawner5",
    // spawnerTransform4); FTransform spawnerTransform5;
    // spawnerTransform5.SetPosition(FVector3(-6.7f, -1.1f, 0.0f));
    // AddActorEntry("BP_MonsterSpawner", "MonsterSpawner5",
    // spawnerTransform5);
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(L_RType_Level1, ULevel)

}   // namespace tkd
