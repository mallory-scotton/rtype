///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <BP_Platform.hpp>
#include <AC_DebugGrid.hpp>
#include <AC_FireflyParticles.hpp>
#include <ST_State.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
BP_Platform::BP_Platform(const FString& name)
    : AActor(name)
    , m_maxAmplitude(0.0f)
    , m_smoothedHeights(NUM_SPECTRUM_BARS, 0.0f)
    , m_barIntensity(0.0f)
    , m_previousEnergy(0.0f)
    , m_energyHistory(43, 0.0f)   // ~43 frames for rolling average
{
    // Add Components
    AddComponent<UPlaneComponent>("PL_PlatformPlane");
    AddComponent<UPlaneComponent>("PL_PlatformPlaneOutline");
    AddComponent<AC_DebugGrid>("DCG_PlatformDebugGrid");
    AddComponent<USphereComponent>("SKY_Sphere");
    AddComponent<AC_FireflyParticles>("FP_PlatformFireflies");

    // Note rails
    AddComponent<UCubeComponent>("CB_NoteRail");
    AddComponent<UCubeComponent>("CB_NotePillar1");
    AddComponent<UCubeComponent>("CB_NotePillar2");

    // Add Flicker Pillars
    AddComponent<UCubeComponent>("CB_FlickerPillar1");
    AddComponent<UCubeComponent>("CB_FlickerPillar2");

    // Add Bloom/Glow layers for Flicker Pillars (multi-layer for enhanced
    // effect)
    AddComponent<UCubeComponent>("CB_FlickerPillar1_Bloom1");
    AddComponent<UCubeComponent>("CB_FlickerPillar1_Bloom2");
    AddComponent<UCubeComponent>("CB_FlickerPillar1_Bloom3");
    AddComponent<UCubeComponent>("CB_FlickerPillar2_Bloom1");
    AddComponent<UCubeComponent>("CB_FlickerPillar2_Bloom2");
    AddComponent<UCubeComponent>("CB_FlickerPillar2_Bloom3");

    // Color Variation Rail
    AddComponent<UCubeComponent>("CB_ColorVariationPillar1");
    AddComponent<UCubeComponent>("CB_ColorVariationPillar2");

    // Add Spectrum Visualizer
    for (SizeT i = 0; i < NUM_SPECTRUM_BARS; ++i)
    {
        AddComponent<UCubeComponent>(FString::Format("SV_BarL{}", i));
        AddComponent<UCubeComponent>(FString::Format("SV_BarR{}", i));
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Platform::BeginPlay(void)
{
    Super::BeginPlay();

    // Initialization of the platform
    auto planeComp = GetComponent<UPlaneComponent>("PL_PlatformPlane");
    auto outlineComp =
        GetComponent<UPlaneComponent>("PL_PlatformPlaneOutline");
    if (planeComp && outlineComp)
    {
        FTransform transform = FTransform::Identity;
        transform.Scale(FVector3(3.0f, 0.5f, 2.0f));
        planeComp->SetLocalTransform(transform);
        planeComp->GetPlane().SetColor(FColor::Black);

        transform.Translate(FVector3(0.f, -0.001f, 0.f));
        transform.SetScale(FVector3(3.1f, 0.5f, 2.1f));
        outlineComp->SetLocalTransform(transform);
        outlineComp->GetPlane().SetColor(FLinearColor(2, 112, 241).ToColor());
    }

    auto noteRail = GetComponent<UCubeComponent>("CB_NoteRail");
    if (noteRail)
    {
        FTransform transform = FTransform::Identity;
        transform.Scale(FVector3(5.f, 0.5f, 1000.0f));
        transform.SetPosition(FVector3(0.f, -0.5f, -510.f));
        noteRail->SetLocalTransform(transform);
        noteRail->GetCube().SetColor(FLinearColor(5, 5, 5).ToColor());
    }

    auto pillar1 = GetComponent<UCubeComponent>("CB_NotePillar1");
    auto pillar2 = GetComponent<UCubeComponent>("CB_NotePillar2");
    if (pillar1 && pillar2)
    {
        FTransform transform = FTransform::Identity;
        transform.Scale(FVector3(0.5f, 100.0f, 0.5f));
        transform.SetPosition(FVector3(-2.25f, -50.5f, -10.25f));
        pillar1->SetLocalTransform(transform);
        pillar1->GetCube().SetColor(FLinearColor(5, 5, 5).ToColor());

        transform.SetPosition(FVector3(2.25f, -50.5f, -10.25f));
        pillar2->SetLocalTransform(transform);
        pillar2->GetCube().SetColor(FLinearColor(5, 5, 5).ToColor());
    }

    auto cvp1 = GetComponent<UCubeComponent>("CB_ColorVariationPillar1");
    auto cvp2 = GetComponent<UCubeComponent>("CB_ColorVariationPillar2");
    if (cvp1 && cvp2)
    {
        FTransform transform = FTransform::Identity;
        transform.Scale(FVector3(0.2f, 100.0f, 0.2f));
        transform.SetPosition(FVector3(-4.5f, -50.f, -10.05f));
        cvp1->SetLocalTransform(transform);
        cvp1->GetCube().SetColor(FLinearColor(5, 5, 5).ToColor());

        transform.SetPosition(FVector3(4.5f, -50.f, -10.05f));
        cvp2->SetLocalTransform(transform);
        cvp2->GetCube().SetColor(FLinearColor(5, 5, 5).ToColor());
    }

    auto fp1 = GetComponent<UCubeComponent>("CB_FlickerPillar1");
    auto fp2 = GetComponent<UCubeComponent>("CB_FlickerPillar2");
    if (fp1 && fp2)
    {
        FTransform transform = FTransform::Identity;
        transform.Scale(FVector3(0.1f, 0.1f, 100.f));
        transform.SetPosition(FVector3(-4.5f, -0.1f, -60.1f));

        fp1->SetLocalTransform(transform);
        fp1->GetCube().SetColor(FLinearColor(2, 112, 241).ToColor());

        transform.SetPosition(FVector3(4.5f, -0.1f, -60.1f));

        fp2->SetLocalTransform(transform);
        fp2->GetCube().SetColor(FLinearColor(2, 112, 241).ToColor());
    }

    // Setup bloom layers for flicker pillars (multi-layer glow effect)
    auto setupBloomLayer = [](UCubeComponent* bloom,
                              const FVector3& pos,
                              float scaleMultiplier,
                              UInt8 alpha)
    {
        if (bloom)
        {
            FTransform transform = FTransform::Identity;
            transform.Scale(
                FVector3(0.1f * scaleMultiplier, 0.1f * scaleMultiplier, 100.f)
            );
            transform.SetPosition(pos);
            bloom->SetLocalTransform(transform);
            bloom->GetCube().SetColor(
                FLinearColor(2, 112, 241, alpha).ToColor()
            );
        }
    };

    // Pillar 1 bloom layers (innermost to outermost)
    setupBloomLayer(
        GetComponent<UCubeComponent>("CB_FlickerPillar1_Bloom1"),
        FVector3(-4.5f, -0.1f, -60.1f),
        1.5f,
        180
    );
    setupBloomLayer(
        GetComponent<UCubeComponent>("CB_FlickerPillar1_Bloom2"),
        FVector3(-4.5f, -0.1f, -60.1f),
        2.0f,
        120
    );
    setupBloomLayer(
        GetComponent<UCubeComponent>("CB_FlickerPillar1_Bloom3"),
        FVector3(-4.5f, -0.1f, -60.1f),
        2.5f,
        60
    );

    // Pillar 2 bloom layers
    setupBloomLayer(
        GetComponent<UCubeComponent>("CB_FlickerPillar2_Bloom1"),
        FVector3(4.5f, -0.1f, -60.1f),
        1.5f,
        180
    );
    setupBloomLayer(
        GetComponent<UCubeComponent>("CB_FlickerPillar2_Bloom2"),
        FVector3(4.5f, -0.1f, -60.1f),
        2.0f,
        120
    );
    setupBloomLayer(
        GetComponent<UCubeComponent>("CB_FlickerPillar2_Bloom3"),
        FVector3(4.5f, -0.1f, -60.1f),
        2.5f,
        60
    );

    // Setup sky sphere
    auto skyComp = GetComponent<USphereComponent>("SKY_Sphere");
    if (skyComp)
    {
        FTransform transform = FTransform::Identity;
        transform.Scale(FVector3(100.0f));
        skyComp->SetLocalTransform(transform);
        skyComp->GetSphere().SetColor(FColor(0.1f, 0.1f, 0.2f));
    }

    // Setup spectrum visualizer bars
    for (SizeT i = 0; i < NUM_SPECTRUM_BARS; ++i)
    {
        FTransform lt = FTransform::Identity;
        FTransform rt = FTransform::Identity;

        FVector3 scale = FVector3(2.f, 2.f, 2.f);

        lt.SetPosition(FVector3(-9.f, -5.f, -12.0f - i * 4.f));
        lt.SetRotation(FRotator(0.f, -25.f, 0.f));
        lt.Scale(scale);

        rt.SetPosition(FVector3(9.f, -5.f, -12.0f - i * 4.f));
        rt.SetRotation(FRotator(0.f, 25.f, 0.f));
        rt.Scale(scale);

        auto bl =
            GetComponent<UCubeComponent>(FString::Format("SV_BarL{}", i));
        auto br =
            GetComponent<UCubeComponent>(FString::Format("SV_BarR{}", i));

        if (bl && br)
        {
            bl->SetLocalTransform(lt);
            bl->GetCube().SetColor(FLinearColor(5, 5, 5).ToColor());
            br->SetLocalTransform(rt);
            br->GetCube().SetColor(FLinearColor(5, 5, 5).ToColor());
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void BP_Platform::Tick(Float32 deltaTime)
{
    Super::Tick(deltaTime);

    auto& stateManager = ST_State::GetInstance();
    if (stateManager.music && stateManager.music->GetBuffer())
    {
        auto buffer = stateManager.music->GetBuffer();

        SizeT sampleCount = buffer->GetSampleCount();
        Float32 sampleRate = buffer->GetSampleRate();
        UInt32 channelCount = buffer->GetChannelCount();
        Int16* samples = buffer->GetSamples();

        Float32 playTime = stateManager.music->GetPlaybackPosition();
        SizeT currentSample =
            static_cast<SizeT>(playTime * sampleRate) * channelCount;

        std::vector<float> spectrum(NUM_SPECTRUM_BARS, 0.0f);
        int samplesPerBar = (sampleRate / 30) / NUM_SPECTRUM_BARS;

        for (SizeT i = 0; i < NUM_SPECTRUM_BARS; i++)
        {
            float sum = 0.0f;
            int count = 0;

            for (int j = 0; j < samplesPerBar; ++j)
            {
                SizeT idx =
                    currentSample + (i * samplesPerBar + j) * channelCount;
                if (idx < sampleCount)
                {
                    for (UInt32 ch = 0; ch < channelCount; ++ch)
                    {
                        if (idx + ch < sampleCount)
                        {
                            sum += std::abs(samples[idx + ch]);
                            count++;
                        }
                    }
                }
            }

            if (count > 0) { spectrum[i] = (sum / count) / 32768.0f; }
        }

        // Update bars with smoothing and normalization
        // Find current max amplitude and calculate instant energy
        float currentMax = 0.0f;
        float instantEnergy = 0.0f;   // For beat detection
        for (SizeT i = 0; i < NUM_SPECTRUM_BARS; ++i)
        {
            if (spectrum[i] > currentMax) { currentMax = spectrum[i]; }
            instantEnergy +=
                spectrum[i] * spectrum[i];   // Energy = sum of squares
        }

        // Update energy history
        m_energyHistory.erase(m_energyHistory.begin());
        m_energyHistory.push_back(instantEnergy);

        float avgEnergy = 0.0f;
        for (float e: m_energyHistory) { avgEnergy += e; }
        avgEnergy /= static_cast<float>(m_energyHistory.size());

        // Beat detection: current energy is significantly higher than average
        // Adjust sensitivity (lower = more sensitive)
        const float beatThreshold = 1.5f;
        bool beatDetected = instantEnergy > (avgEnergy * beatThreshold) &&
                            instantEnergy > m_previousEnergy;
        m_previousEnergy = instantEnergy;

        // Update global max with smoothing
        if (currentMax > m_maxAmplitude) { m_maxAmplitude = currentMax; }
        else
        {
            // Slowly decay max to adapt to quieter sections
            m_maxAmplitude *= 0.995f;
        }

        // Prevent division by zero
        if (m_maxAmplitude < 0.001f) { m_maxAmplitude = 0.001f; }

        // Update top bar intensity - flash on beat detection
        if (beatDetected)
        {
            m_barIntensity = 1.0f;   // Instant flash
        }
        else
        {
            m_barIntensity *= 0.85f;   // Fast decay for flicker effect
        }

        UInt8 brightness =
            static_cast<UInt8>(std::min(m_barIntensity * 255.f, 255.f));
        auto fp1 = GetComponent<UCubeComponent>("CB_FlickerPillar1");
        auto fp2 = GetComponent<UCubeComponent>("CB_FlickerPillar2");
        if (fp1 && fp2)
        {
            fp1->GetCube().SetColor(
                FLinearColor(2, 112, 241, brightness).ToColor()
            );
            fp2->GetCube().SetColor(
                FLinearColor(2, 112, 241, brightness).ToColor()
            );
        }

        // Update bloom layers with synchronized intensity for immersive glow
        // effect Each layer has progressively lower opacity for a natural
        // bloom falloff
        auto updateBloomLayer = [this](const FString& name, float alphaScale)
        {
            auto bloom = GetComponent<UCubeComponent>(name);
            if (bloom)
            {
                UInt8 bloomBrightness = static_cast<UInt8>(
                    std::min(m_barIntensity * 255.f * alphaScale, 255.f)
                );
                bloom->GetCube().SetColor(
                    FLinearColor(2, 112, 241, bloomBrightness).ToColor()
                );
            }
        };

        // Update bloom layers for both pillars with intensity falloff
        updateBloomLayer(
            "CB_FlickerPillar1_Bloom1", 0.7f
        );   // Inner layer - 70% intensity
        updateBloomLayer(
            "CB_FlickerPillar1_Bloom2", 0.5f
        );   // Mid layer - 50% intensity
        updateBloomLayer(
            "CB_FlickerPillar1_Bloom3", 0.25f
        );   // Outer layer - 25% intensity

        updateBloomLayer("CB_FlickerPillar2_Bloom1", 0.7f);
        updateBloomLayer("CB_FlickerPillar2_Bloom2", 0.5f);
        updateBloomLayer("CB_FlickerPillar2_Bloom3", 0.25f);

        for (SizeT i = 0; i < NUM_SPECTRUM_BARS; ++i)
        {
            // Normalize to maxBarHeight
            float normalizedValue = spectrum[i] / m_maxAmplitude;
            float targetHeight = normalizedValue * MAX_BAR_HEIGHT;
            targetHeight = std::min(targetHeight, MAX_BAR_HEIGHT);

            // Smooth transition
            m_smoothedHeights[i] +=
                (targetHeight - m_smoothedHeights[i]) * 0.25f;

            auto bl =
                GetComponent<UCubeComponent>(FString::Format("SV_BarL{}", i));
            auto br =
                GetComponent<UCubeComponent>(FString::Format("SV_BarR{}", i));

            if (!br || !bl) { continue; }

            FTransform lt = bl->GetLocalTransform();
            FTransform rt = br->GetLocalTransform();

            lt.SetScale(FVector3(2.f, m_smoothedHeights[i], 2.f));
            rt.SetScale(FVector3(2.f, m_smoothedHeights[i], 2.f));

            br->SetLocalTransform(rt);
            bl->SetLocalTransform(lt);
        }
    }
    else
    {
        for (SizeT i = 0; i < NUM_SPECTRUM_BARS; ++i)
        {
            auto bl =
                GetComponent<UCubeComponent>(FString::Format("SV_BarL{}", i));
            auto br =
                GetComponent<UCubeComponent>(FString::Format("SV_BarR{}", i));

            if (!br || !bl) { continue; }

            FTransform lt = bl->GetLocalTransform();
            FTransform rt = br->GetLocalTransform();

            lt.SetScale(FVector3(2.f, 0.1f, 2.f));
            rt.SetScale(FVector3(2.f, 0.1f, 2.f));

            br->SetLocalTransform(rt);
            bl->SetLocalTransform(lt);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS_WITH_SUPER(BP_Platform, AActor)

}   // namespace tkd
