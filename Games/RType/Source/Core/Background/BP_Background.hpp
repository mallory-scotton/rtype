///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{
///////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////
class BP_Background : public AActor
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Class members
    ///////////////////////////////////////////////////////////////////////////
    Float32 m_scrollSpeed;      //<! The speed at which the background scrolls
    Float32 m_parallaxFactor;   //<! The parallax factor for the background
    FilePath m_texturePath;     //<! Path to the background texture
    // Two billboards used to tile the background seamlessly
    // Width of the texture in world units (used to compute wrap position)
    Float32 m_textureWidth;
    Float32 m_scaleX;       //<! Scale factor in X direction
    Float32 m_worldWidth;   //<! Width of the background in world units

    // Additional layers for parallax effect
    FilePath m_texturePathPlanet;     //<! Path to the planet texture
    FilePath m_texturePathStars;      //<! Path to the stars texture

    Float32 m_textureWidthPlanet;     //<! Width in pixels for planet texture
    Float32 m_textureWidthStars;      //<! Width in pixels for stars texture

    Float32 m_worldWidthPlanet;       //<! World width for planet layer
    Float32 m_worldWidthStars;        //<! World width for stars layer

    Float32 m_parallaxPlanetFactor;   //<! Parallax factor for planet layer
    Float32 m_parallaxStarsFactor;    //<! Parallax factor for stars layer

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    BP_Background();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Called when the object is first created
    ///
    ///////////////////////////////////////////////////////////////////////////
    void BeginPlay(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Called every frame to update the background
    ///
    /// \param deltaTime The time elapsed since the last frame
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Tick(Float32 deltaTime) override;

protected:
    // Add protected members here
    // Helper to update billboards when texture changes
    void SetupBillboards(void);

private:
    ///////////////////////////////////////////////////////////////////////////
    void ChangeTexture(
        const FilePath& newTexturePath = "Assets/Images/bg-back.png",
        const FilePath& newPlanetPath = "Assets/Images/bg-planet.png",
        const FilePath& newStarsPath = "Assets/Images/bg-stars.png"
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class metadata
    ///////////////////////////////////////////////////////////////////////////
    DECLARE_CLASS_WITH_SUPER(BP_Background, AActor)
};

}   // namespace tkd

