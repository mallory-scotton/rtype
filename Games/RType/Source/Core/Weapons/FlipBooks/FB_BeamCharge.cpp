///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Weapons/FlipBooks/FB_BeamCharge.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FB_BeamCharge::FB_BeamCharge(void)
    : UFlipBook("FB_BeamCharge", ESourceMode::FromSprite, EPlayMode::Once)
{
    SetSpriteSheetPath("Assets/Images/T_Projectiles.png");
    AddFrame(FRectangle(0, 51, 33, 33));
    AddFrame(FRectangle(33, 51, 33, 33));
    AddFrame(FRectangle(66, 51, 33, 33));
    AddFrame(FRectangle(99, 51, 33, 33));
    AddFrame(FRectangle(132, 51, 33, 33));
    AddFrame(FRectangle(165, 51, 33, 33));
    AddFrame(FRectangle(198, 51, 33, 33));
    AddFrame(FRectangle(231, 51, 33, 33));
    SetDuration(1.f);
    SetPlaybackSpeed(1.f);
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(FB_BeamCharge, UFlipBook)

}   // namespace tkd
