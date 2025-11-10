///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Core/Boss/FlipBook/FB_BossIdle.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FB_BossIdle::FB_BossIdle(void)
    : UFlipBook("FB_BossIdle", ESourceMode::FromSprite, EPlayMode::Loop)
{
    SetSpriteSheetPath("Assets/Images/r-typesheet36.png");
    AddFrame(FRectangle(0, 0, 264, 143));
    AddFrame(FRectangle(264, 0, 264, 143));
    AddFrame(FRectangle(0, 143, 264, 143));
    AddFrame(FRectangle(264, 143, 264, 143));
    AddFrame(FRectangle(0, 286, 264, 143));
    AddFrame(FRectangle(264, 286, 264, 143));
    AddFrame(FRectangle(0, 429, 264, 143));
    AddFrame(FRectangle(264, 429, 264, 143));
    SetDuration(2.f);
    SetPlaybackSpeed(2.f);
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(FB_BossIdle, UFlipBook)

}   // namespace tkd
