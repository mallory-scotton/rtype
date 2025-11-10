///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <BP_Note.hpp>
#include <AC_NoteArrow.hpp>
#include <AC_SwordBlade.hpp>
#include <algorithm>
#include <BP_Sword.hpp>
#include <cmath>
#include <Engine/Assets/URessource.hpp>
#include <numeric>
#include <ST_State.hpp>
#include <unordered_set>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

namespace
{

///////////////////////////////////////////////////////////////////////////////
static float SignedDist(const FVector3& p, const FVector3& n)
{
    return p.x * n.x + p.y * n.y + p.z * n.z;
}

///////////////////////////////////////////////////////////////////////////////
static FVertex LerpVertex(const FVertex& a, const FVertex& b, float t)
{
    FVector3 pos = a.position + (b.position - a.position) * t;
    // Simple linear interpolation for color and uv
    FColor col;
    col.r = a.color.r + (b.color.r - a.color.r) * t;
    col.g = a.color.g + (b.color.g - a.color.g) * t;
    col.b = a.color.b + (b.color.b - a.color.b) * t;
    col.a = a.color.a + (b.color.a - a.color.a) * t;
    FVector2 uv = a.uv + (b.uv - a.uv) * t;
    return FVertex(pos, col, uv);
}

///////////////////////////////////////////////////////////////////////////////
static std::vector<FVertex> ClipPolygonAgainstPlane(
    const std::vector<FVertex>& poly, const FVector3& n, bool keepPositive
)
{
    std::vector<FVertex> out;
    if (poly.empty()) { return out; }

    auto side = [&](const FVertex& v) -> float
    { return SignedDist(v.position, n); };

    for (size_t i = 0; i < poly.size(); ++i)
    {
        const FVertex& A = poly[i];
        const FVertex& B = poly[(i + 1) % poly.size()];
        float da = side(A);
        float db = side(B);
        bool ina = keepPositive ? (da >= 0.0f) : (da <= 0.0f);
        bool inb = keepPositive ? (db >= 0.0f) : (db <= 0.0f);

        if (ina) { out.push_back(A); }

        if (ina != inb)
        {
            // Edge intersects plane, compute t
            float t = da / (da - db);
            if (t < 0.f) { t = 0.f; }
            if (t > 1.f) { t = 1.f; }
            FVertex ip = LerpVertex(A, B, t);
            out.push_back(ip);
        }
    }

    return out;
}

///////////////////////////////////////////////////////////////////////////////
static void TriangulatePolygonToTriangles(
    const std::vector<FVertex>& poly, std::vector<FVertex>& outTriangles
)
{
    if (poly.size() < 3) { return; }
    const FVertex& c = poly[0];
    for (size_t i = 1; i + 1 < poly.size(); ++i)
    {
        outTriangles.push_back(c);
        outTriangles.push_back(poly[i]);
        outTriangles.push_back(poly[i + 1]);
    }
}

}   // namespace

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

    // Save initial transform
    m_originalTransform = GetTransform();

    // Setup spawn animation: pick a random direction (left/right/top/bottom)
    // and place the cube offscreen there. We'll interpolate from this start
    // transform into m_originalTransform over m_spawnDuration while
    // advancing both start/target forward so the note continues moving.
    {
        // safe random direction (0..3)
        int dir = std::rand() % 4;
        float spawnDist = 1.2f;   // world units off-axis to start
        FTransform start = m_originalTransform;
        FVector3 pos = start.GetPosition();
        switch (dir)
        {
        case 0 : pos.x += spawnDist; break;   // right
        case 1 : pos.x -= spawnDist; break;   // left
        case 2 : pos.y += spawnDist; break;   // top
        case 3 : pos.y -= spawnDist; break;   // bottom
        default: pos.x += spawnDist; break;
        }
        start.SetPosition(pos);
        m_spawnStartTransform = start;
        m_spawnTargetTransform = m_originalTransform;
        m_spawnTime = 0.0f;
        m_spawning = true;

        // Place actor at the spawn start immediately
        SetTransform(m_spawnStartTransform);
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
                    // Ignore repeated hits once processed
                    if (m_cutProcessed) { return; }

                    if (info.otherActor && info.otherActor->Is<BP_Sword>() &&
                        info.otherComponent &&
                        info.otherComponent->Is<AC_SwordBlade>())
                    {
                        // Handle cut visually (split, particles) and mark
                        // processed
                        OnCut(info);

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
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Note::OnCut(const FCollisionInfo& info)
{
    // Mark processed to avoid re-triggering
    m_cutProcessed = true;

    // Disable/hide collisions so we don't spam scoring/animations
    auto cutCollision =
        GetComponent<UBoxCollisionComponent>("BC_CutCollision");
    if (cutCollision)
    {
        cutCollision->SetHiddenInGame(true);
        // shrink collision to avoid further overlaps
        cutCollision->SetBoxExtent(FVector3f(0.f, 0.f, 0.f));
    }
    auto badCutCollision =
        GetComponent<UBoxCollisionComponent>("BC_BadCutCollision");
    if (badCutCollision)
    {
        badCutCollision->SetHiddenInGame(true);
        badCutCollision->SetBoxExtent(FVector3f(0.f, 0.f, 0.f));
    }

    // Grab the visual cube and duplicate it into two fragment components
    auto cubeComp = GetComponent<UChamferCubeComponent>("SM_Cube");
    if (!cubeComp) { return; }

    // Hide the original cube; we'll replace it with two sliced halves
    auto& originalPrim = cubeComp->GetChamferCube();
    cubeComp->SetHiddenInGame(true);

    // Retrieve original vertices (in primitive local space)
    const auto& origVerts = originalPrim.GetVertices();
    if (origVerts.empty()) { return; }

    // Prepare containers for positive/negative side triangles
    std::vector<FVertex> posTris;
    std::vector<FVertex> negTris;
    std::vector<FVector3> cutPoints;

    // Compute cut plane normal from the collision info (use actual blade
    // impact) Transform the world-space impact normal into the primitive-local
    // space
    FVector3 planeN(1.f, 0.f, 0.f);
    FVector3 worldImpactNormal = info.impactNormal;
    // Fallback if impact normal is degenerate
    if (worldImpactNormal.Length() < 1e-4f)
    {
        // Fall back to the note's intended cut direction
        switch (m_cutDirection)
        {
        case ECutDirection::Up:
            worldImpactNormal = FVector3(0.f, 1.f, 0.f);
            break;
        case ECutDirection::Down:
            worldImpactNormal = FVector3(0.f, -1.f, 0.f);
            break;
        case ECutDirection::Left:
            worldImpactNormal = FVector3(-1.f, 0.f, 0.f);
            break;
        case ECutDirection::Right:
            worldImpactNormal = FVector3(1.f, 0.f, 0.f);
            break;
        case ECutDirection::UpLeft:
            worldImpactNormal = FVector3(-0.7071f, 0.7071f, 0.f);
            break;
        case ECutDirection::UpRight:
            worldImpactNormal = FVector3(0.7071f, 0.7071f, 0.f);
            break;
        case ECutDirection::DownLeft:
            worldImpactNormal = FVector3(-0.7071f, -0.7071f, 0.f);
            break;
        case ECutDirection::DownRight:
            worldImpactNormal = FVector3(0.7071f, -0.7071f, 0.f);
            break;
        case ECutDirection::None:
        default                 : worldImpactNormal = FVector3(1.f, 0.f, 0.f); break;
        }
    }

    // primitive local -> world transform for the cube primitive
    FTransform worldT = GetTransform();
    FTransform primLocalT = cubeComp->GetLocalTransform();
    FTransform primWorldT = worldT * primLocalT;

    // Transform world normal into primitive-local space
    planeN = primWorldT.Inverse().TransformDirection(worldImpactNormal);
    // Normalize safety
    if (planeN.Length() < 1e-6f) { planeN = FVector3(1.f, 0.f, 0.f); }
    else { planeN = planeN.Normalized(); }

    // Process triangles
    for (size_t i = 0; i + 2 < origVerts.size(); i += 3)
    {
        std::vector<FVertex> tri = { origVerts[i],
                                     origVerts[i + 1],
                                     origVerts[i + 2] };

        // Clip for positive side
        auto clippedPos = ClipPolygonAgainstPlane(tri, planeN, true);
        TriangulatePolygonToTriangles(clippedPos, posTris);

        // Clip for negative side (invert keep side)
        auto clippedNeg = ClipPolygonAgainstPlane(tri, planeN, false);
        TriangulatePolygonToTriangles(clippedNeg, negTris);

        // Collect intersection points when triangle crosses plane
        float d0 = SignedDist(tri[0].position, planeN);
        float d1 = SignedDist(tri[1].position, planeN);
        float d2 = SignedDist(tri[2].position, planeN);
        auto pushIntersection =
            [&](const FVertex& A, const FVertex& B, float da, float db)
        {
            if ((da >= 0.0f && db < 0.0f) || (da < 0.0f && db >= 0.0f))
            {
                float t = da / (da - db);
                if (t < 0.f) { t = 0.f; }
                if (t > 1.f) { t = 1.f; }
                FVertex ip = LerpVertex(A, B, t);
                cutPoints.push_back(ip.position);
            }
        };

        pushIntersection(tri[0], tri[1], d0, d1);
        pushIntersection(tri[1], tri[2], d1, d2);
        pushIntersection(tri[2], tri[0], d2, d0);
    }

    // If we have cut points, build a cap polygon and triangulate it
    if (!cutPoints.empty())
    {
        // Remove duplicates (epsilon)
        const float eps = 1e-4f;
        std::vector<FVector3> uniquePts;
        for (auto& p: cutPoints)
        {
            bool found = false;
            for (auto& q: uniquePts)
            {
                if ((p - q).Length() < eps)
                {
                    found = true;
                    break;
                }
            }
            if (!found) { uniquePts.push_back(p); }
        }

        // Compute centroid
        FVector3 centroid(0.f, 0.f, 0.f);
        for (auto& p: uniquePts) { centroid += p; }
        centroid = centroid / static_cast<float>(uniquePts.size());

        // Build local basis for plane
        FVector3 up = planeN;
        FVector3 arbitrary =
            (std::abs(up.x) < 0.9f) ? FVector3(1, 0, 0) : FVector3(0, 1, 0);
        FVector3 u = up.Cross(arbitrary).Normalized();
        FVector3 v = up.Cross(u).Normalized();

        // Sort points by angle around centroid
        std::vector<std::pair<float, FVector3>> angPts;
        for (auto& p: uniquePts)
        {
            FVector3 d = p - centroid;
            float x = d.Dot(u);
            float y = d.Dot(v);
            float ang = std::atan2(y, x);
            angPts.push_back({ ang, p });
        }
        std::sort(
            angPts.begin(),
            angPts.end(),
            [](auto& a, auto& b) { return a.first < b.first; }
        );

        // Build polygon vertices with simple color/uv
        std::vector<FVertex> capPoly;
        for (auto& ap: angPts)
        {
            capPoly.push_back(FVertex(
                ap.second, originalPrim.GetColor(), FVector2(0.5f, 0.5f)
            ));
        }

        // Triangulate cap for positive side - ensure winding matches plane
        // normal
        std::vector<FVertex> capTriangles;
        TriangulatePolygonToTriangles(capPoly, capTriangles);

        // Ensure cap triangle normals face the plane normal for posTris;
        // otherwise flip winding
        if (!capTriangles.empty())
        {
            // compute first triangle normal
            FVector3 p0 = capTriangles[0].position;
            FVector3 p1 = capTriangles[1].position;
            FVector3 p2 = capTriangles[2].position;
            FVector3 n = (p1 - p0).Cross(p2 - p0).Normalized();
            if (n.Dot(planeN) < 0.0f)
            {
                // flip ordering of each triangle
                for (size_t i = 0; i + 2 < capTriangles.size(); i += 3)
                {
                    std::swap(capTriangles[i + 1], capTriangles[i + 2]);
                }
            }
        }

        // Add cap to both pos and neg (neg side gets opposite winding)
        posTris.insert(
            posTris.end(), capTriangles.begin(), capTriangles.end()
        );

        // For negative side, reverse winding
        std::vector<FVertex> capTrianglesFlipped = capTriangles;
        for (size_t i = 0; i + 2 < capTrianglesFlipped.size(); i += 3)
        {
            std::swap(capTrianglesFlipped[i + 1], capTrianglesFlipped[i + 2]);
        }
        negTris.insert(
            negTris.end(),
            capTrianglesFlipped.begin(),
            capTrianglesFlipped.end()
        );
    }

    // Create fragment primitives by overwriting their vertex lists
    auto fragA = AddComponent<UChamferCubeComponent>("SM_Cube_FragA");
    auto fragB = AddComponent<UChamferCubeComponent>("SM_Cube_FragB");
    if (!fragA || !fragB) { return; }

    // Assign generated triangle lists into the fragment primitives
    {
        auto& primA = fragA->GetChamferCube();
        auto& primB = fragB->GetChamferCube();
        auto& vertsA = const_cast<std::vector<FVertex>&>(primA.GetVertices());
        auto& vertsB = const_cast<std::vector<FVertex>&>(primB.GetVertices());
        vertsA = posTris;
        vertsB = negTris;
        // copy color/texture from original
        primA.SetTexture(originalPrim.GetTexture());
        primB.SetTexture(originalPrim.GetTexture());
        primA.SetColor(originalPrim.GetColor());
        primB.SetColor(originalPrim.GetColor());
    }

    // Frag transforms: position them slightly apart along the cut normal
    FTransform baseLocal = cubeComp->GetLocalTransform();
    FVector3 basePos = baseLocal.GetPosition();
    FVector3 offset = planeN * 0.18f;   // split offset in local space

    FTransform aLocal = baseLocal;
    aLocal.SetPosition(basePos + offset);
    FTransform bLocal = baseLocal;
    bLocal.SetPosition(basePos - offset);

    // Scale fragments slightly smaller on the cut axis to simulate half
    FVector3 baseScale = baseLocal.GetScale();
    FVector3 scaleA = baseScale;
    FVector3 scaleB = baseScale;
    if (std::abs(planeN.x) > 0.7f)
    {
        scaleA.x *= 0.5f;
        scaleB.x *= 0.5f;
    }
    else if (std::abs(planeN.y) > 0.7f)
    {
        scaleA.y *= 0.5f;
        scaleB.y *= 0.5f;
    }
    else
    {
        scaleA.z *= 0.5f;
        scaleB.z *= 0.5f;
    }
    aLocal.SetScale(scaleA);
    bLocal.SetScale(scaleB);

    fragA->SetLocalTransform(aLocal);
    fragB->SetLocalTransform(bLocal);
    fragA->SetHiddenInGame(false);
    fragB->SetHiddenInGame(false);

    // Setup simple fragment motion (world space approx)
    // reuse worldT computed above (primitive/world transforms)

    // Compute world-space fragment positions then derive motion directions
    FTransform worldA = worldT * aLocal;
    FTransform worldB = worldT * bLocal;
    FVector3 dirA = (worldA.GetPosition() - worldT.GetPosition());
    FVector3 dirB = (worldB.GetPosition() - worldT.GetPosition());
    // normalize safely
    if (dirA.Length() > 0.0001f) { dirA = dirA / dirA.Length(); }
    else { dirA = FVector3(1.f, 0.f, 0.f); }
    if (dirB.Length() > 0.0001f) { dirB = dirB / dirB.Length(); }
    else { dirB = FVector3(-1.f, 0.f, 0.f); }

    // Random small variations
    auto rnd = [](float a, float b) -> float
    {
        return a + (static_cast<float>(std::rand()) /
                    static_cast<float>(RAND_MAX)) *
                       (b - a);
    };

    // Bias fragments to move away along the world impact normal so the split
    // follows blade motion
    FVector3 impactBiasA = worldImpactNormal * rnd(0.6f, 1.6f);
    FVector3 impactBiasB = worldImpactNormal * rnd(0.6f, 1.6f);
    m_fragmentVelocityA = dirA * rnd(1.2f, 2.0f) +
                          FVector3(0.f, 0.f, rnd(1.0f, 2.0f)) + impactBiasA;
    m_fragmentVelocityB = dirB * rnd(0.8f, 1.5f) +
                          FVector3(0.f, 0.f, rnd(0.6f, 1.2f)) - impactBiasB;
    m_fragmentAngularA =
        FVector3(rnd(-300.f, 300.f), rnd(-300.f, 300.f), rnd(-300.f, 300.f));
    m_fragmentAngularB =
        FVector3(rnd(-200.f, 200.f), rnd(-200.f, 200.f), rnd(-200.f, 200.f));

    m_fragmentLifetime = 1.6f;   // seconds of fragment animation
    m_fragmentsActive = true;

    // Setup and emit particles for the cut explosion
    auto handle =
        URessource::GetTextureHandle("Assets/Textures/CircleGradient.png");
    if (handle.IsValid()) { m_cutParticles.SetTexture(handle.Get()); }
    // Use a small sphere emitter around the impact point so sparkles spread
    // naturally
    m_cutParticles.SetEmitterType(EParticleEmitterType::Sphere);
    m_cutParticles.SetEmitterRadius(0.04f);
    // Use the impact point from collision info as emitter origin (world space)
    m_cutParticles.SetEmitterPosition(info.impactPoint);
    // Brighter, whiter sparkles fading to orange and then transparent
    m_cutParticles.SetParticleColor(
        FColor(1.0f, 0.95f, 0.8f, 1.0f), FColor(1.0f, 0.6f, 0.2f, 0.0f)
    );
    m_cutParticles.SetAdditiveBlending(true);
    // Slightly longer lifetimes and larger sizes for visibility and glow
    m_cutParticles.SetParticleLifetime(0.25f, 0.9f);
    m_cutParticles.SetParticleSize(0.02f, 0.12f);
    m_cutParticles.SetParticleSpeed(1.2f, 4.5f);
    // Emit more particles so sparkles are dense enough
    m_cutParticles.EmitParticles(90);

    // Optionally mark scoring here, or leave to calling system. We'll schedule
    // deletion Create some small debris cube components for chunkier pieces
    m_debrisComps.clear();
    m_debrisVelocities.clear();
    m_debrisAngularVel.clear();
    for (SizeT i = 0; i < m_debrisCount; ++i)
    {
        auto dcomp = AddComponent<UChamferCubeComponent>(
            FString("SM_Debris_") + FString::ToString(i)
        );
        if (!dcomp) { continue; }
        // Give them a tiny scale and random offset around the cut center
        FTransform dt = aLocal;   // base off fragment A local
        float s = rnd(0.08f, 0.18f);
        dt.SetScale(FVector3(s, s, s));
        FVector3 jitter(
            rnd(-0.05f, 0.05f), rnd(-0.05f, 0.05f), rnd(-0.05f, 0.05f)
        );
        dt.SetPosition(dt.GetPosition() + jitter);
        dcomp->SetLocalTransform(dt);
        dcomp->SetHiddenInGame(false);
        // set color/texture like original
        dcomp->GetChamferCube().SetColor(originalPrim.GetColor());
        dcomp->GetChamferCube().SetTexture(originalPrim.GetTexture());

        // velocities
        FVector3 vel =
            (dirA * rnd(0.8f, 2.5f) + FVector3(0, 0, rnd(0.6f, 2.0f))) *
            rnd(0.5f, 1.5f);
        FVector3 ang = FVector3(
            rnd(-600.f, 600.f), rnd(-600.f, 600.f), rnd(-600.f, 600.f)
        );

        m_debrisComps.push_back(dcomp);
        m_debrisVelocities.push_back(vel);
        m_debrisAngularVel.push_back(ang);
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Note::Tick(float deltaTime)
{
    // Call the Tick of the Super Class
    Super::Tick(deltaTime);

    // Move the note toward the player using the configured speed.
    // If spawning, interpolate from the offscreen start into the target
    // while advancing both start and target forward so the note keeps
    // moving along Z during the lateral spawn animation.
    if (m_spawning)
    {
        float dz = deltaTime * m_speed;
        // advance both start and target forward so interpolation follows
        m_spawnStartTransform.Translate(FVector3(0.f, 0.f, dz));
        m_spawnTargetTransform.Translate(FVector3(0.f, 0.f, dz));

        m_spawnTime += deltaTime;
        float t = m_spawnTime / m_spawnDuration;
        if (t >= 1.0f)
        {
            t = 1.0f;
            m_spawning = false;
        }

        // ease-out cubic for a nicer feel
        float et = 1.0f - std::pow(1.0f - t, 3.0f);

        // Interpolate position, rotation and scale
        FVector3 p = m_spawnStartTransform.GetPosition() +
                     (m_spawnTargetTransform.GetPosition() -
                      m_spawnStartTransform.GetPosition()) *
                         et;
        // Interpolate rotations component-wise (TRotator doesn't support
        // subtraction or scalar multiplication)
        TRotator r0 = m_spawnStartTransform.GetRotation();
        TRotator r1 = m_spawnTargetTransform.GetRotation();
        float pitch = r0.GetPitch() + (r1.GetPitch() - r0.GetPitch()) * et;
        float yaw = r0.GetYaw() + (r1.GetYaw() - r0.GetYaw()) * et;
        float roll = r0.GetRoll() + (r1.GetRoll() - r0.GetRoll()) * et;
        TRotator r(pitch, yaw, roll);
        FVector3 s = m_spawnStartTransform.GetScale() +
                     (m_spawnTargetTransform.GetScale() -
                      m_spawnStartTransform.GetScale()) *
                         et;

        FTransform newT;
        newT.SetPosition(p);
        newT.SetRotation(r);
        newT.SetScale(s);
        SetTransform(newT);
    }
    else
    {
        FTransform transform = GetTransform();
        transform.Translate(FVector3(0.f, 0.f, deltaTime * m_speed));
        SetTransform(transform);
    }

    // Read back the current transform after movement for the deletion check
    FTransform transform = GetTransform();

    // Get the state manager
    auto& stateManager = ST_State::GetInstance();

    // Delete note when it passes the player position
    if (transform.GetPosition().z > 2.0f ||
        stateManager.gameState == EBeatSaberGameState::Menu)
    {
        MarkForDeletion();
    }

    // Update fragments and particle animation when cut
    if (m_fragmentsActive)
    {
        // Update fragment lifetime
        m_fragmentLifetime -= deltaTime;

        // Update particle system
        m_cutParticles.Update(deltaTime);

        // Update fragment components motion
        auto fragA = GetComponent<UChamferCubeComponent>("SM_Cube_FragA");
        auto fragB = GetComponent<UChamferCubeComponent>("SM_Cube_FragB");

        if (fragA)
        {
            FTransform t = fragA->GetLocalTransform();
            t.Translate(m_fragmentVelocityA * deltaTime);
            // apply simple rotation based on angular velocity (degrees/sec)
            t.SetRotation(
                t.GetRotation() + TRotator(
                                      m_fragmentAngularA.x * deltaTime,
                                      m_fragmentAngularA.y * deltaTime,
                                      m_fragmentAngularA.z * deltaTime
                                  )
            );
            fragA->SetLocalTransform(t);
        }

        if (fragB)
        {
            FTransform t = fragB->GetLocalTransform();
            t.Translate(m_fragmentVelocityB * deltaTime);
            t.SetRotation(
                t.GetRotation() + TRotator(
                                      m_fragmentAngularB.x * deltaTime,
                                      m_fragmentAngularB.y * deltaTime,
                                      m_fragmentAngularB.z * deltaTime
                                  )
            );
            fragB->SetLocalTransform(t);
        }

        // Update debris pieces
        for (SizeT i = 0; i < m_debrisComps.size(); ++i)
        {
            auto* d = m_debrisComps[i];
            if (!d) { continue; }
            FTransform dt = d->GetLocalTransform();
            dt.Translate(m_debrisVelocities[i] * deltaTime);
            dt.SetRotation(
                dt.GetRotation() + TRotator(
                                       m_debrisAngularVel[i].x * deltaTime,
                                       m_debrisAngularVel[i].y * deltaTime,
                                       m_debrisAngularVel[i].z * deltaTime
                                   )
            );
            d->SetLocalTransform(dt);
        }

        // When fragments lifetime is over, finish the note
        if (m_fragmentLifetime <= 0.0f)
        {
            // final clear
            m_fragmentsActive = false;
            // ensure particles are cleared next frame or stop emitting
            m_cutParticles.Clear();
            // Remove this actor from the world
            MarkForDeletion();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
ENoteType BP_Note::GetNoteType(void) const { return m_type; }

///////////////////////////////////////////////////////////////////////////////
ECutDirection BP_Note::GetCutDirection(void) const { return m_cutDirection; }

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Note, AActor)

}   // namespace tkd
