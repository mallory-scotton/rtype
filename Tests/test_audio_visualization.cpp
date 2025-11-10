#include <algorithm>
#include <cmath>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <vector>

int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 600), "SFML Audio Visualizer");
    window.setFramerateLimit(60);

    // Load audio file
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("song.egg")) { return -1; }

    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setLoop(true);
    sound.play();

    const sf::Int16* samples = buffer.getSamples();
    std::size_t sampleCount = buffer.getSampleCount();
    unsigned int sampleRate = buffer.getSampleRate();
    unsigned int channelCount = buffer.getChannelCount();

    const int numBars = 64;
    const float barWidth = static_cast<float>(window.getSize().x) / numBars;
    const float spacing = 2.0f;
    const float maxBarHeight = 500.0f;   // Define maximum bar height in pixels

    std::vector<sf::RectangleShape> bars(numBars);
    std::vector<float> smoothedHeights(numBars, 0.0f);
    float maxAmplitude = 0.0f;   // Track maximum amplitude for normalization

    // Top flicker bar (Beat Saber style)
    sf::RectangleShape topBar(sf::Vector2f(window.getSize().x, 10.0f));
    topBar.setPosition(0, 0);
    float topBarIntensity = 0.0f;
    float previousEnergy = 0.0f;   // For beat detection
    std::vector<float> energyHistory(
        43, 0.0f
    );   // ~43 frames for rolling average

    // Initialize bars
    for (int i = 0; i < numBars; ++i)
    {
        bars[i].setSize(sf::Vector2f(barWidth - spacing, 0));
        bars[i].setPosition(i * barWidth, window.getSize().y);

        // Rainbow gradient colors
        float hue = (float)i / numBars * 360.0f;
        float r = std::sin(hue * 0.0174533f) * 127 + 128;
        float g = std::sin((hue + 120) * 0.0174533f) * 127 + 128;
        float b = std::sin((hue + 240) * 0.0174533f) * 127 + 128;
        bars[i].setFillColor(sf::Color(r, g, b));
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) { window.close(); }
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape) { window.close(); }
                if (event.key.code == sf::Keyboard::Space)
                {
                    if (sound.getStatus() == sf::Sound::Playing)
                    {
                        sound.pause();
                    }
                    else { sound.play(); }
                }
            }
        }

        // Get current playback position
        sf::Time playTime = sound.getPlayingOffset();
        std::size_t currentSample =
            static_cast<std::size_t>(playTime.asSeconds() * sampleRate) *
            channelCount;

        // Calculate spectrum from current audio window
        std::vector<float> spectrum(numBars, 0.0f);
        int samplesPerBar = (sampleRate / 30) / numBars;   // ~30fps window

        for (int i = 0; i < numBars; ++i)
        {
            float sum = 0.0f;
            int count = 0;

            for (int j = 0; j < samplesPerBar; ++j)
            {
                std::size_t idx =
                    currentSample + (i * samplesPerBar + j) * channelCount;
                if (idx < sampleCount)
                {
                    // Average all channels
                    for (unsigned int ch = 0; ch < channelCount; ++ch)
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
        for (int i = 0; i < numBars; ++i)
        {
            if (spectrum[i] > currentMax) { currentMax = spectrum[i]; }
            instantEnergy +=
                spectrum[i] * spectrum[i];   // Energy = sum of squares
        }

        // Update energy history
        energyHistory.erase(energyHistory.begin());
        energyHistory.push_back(instantEnergy);

        // Calculate average energy over history
        float avgEnergy = 0.0f;
        for (float e: energyHistory) { avgEnergy += e; }
        avgEnergy /= energyHistory.size();

        // Beat detection: current energy is significantly higher than average
        const float beatThreshold =
            1.5f;   // Adjust sensitivity (lower = more sensitive)
        bool beatDetected = instantEnergy > (avgEnergy * beatThreshold) &&
                            instantEnergy > previousEnergy;
        previousEnergy = instantEnergy;

        // Update global max with smoothing
        if (currentMax > maxAmplitude) { maxAmplitude = currentMax; }
        else
        {
            maxAmplitude *=
                0.995f;   // Slowly decay max to adapt to quieter sections
        }

        // Prevent division by zero
        if (maxAmplitude < 0.001f) { maxAmplitude = 0.001f; }

        // Update top bar intensity - flash on beat detection
        if (beatDetected)
        {
            topBarIntensity = 1.0f;   // Instant flash
        }
        else
        {
            topBarIntensity *= 0.85f;   // Fast decay for flicker effect
        }

        // Set top bar color based on intensity
        int brightness = static_cast<int>(topBarIntensity * 255);
        topBar.setFillColor(sf::Color(brightness, brightness / 2, brightness));

        for (int i = 0; i < numBars; ++i)
        {
            // Normalize to maxBarHeight
            float normalizedValue = spectrum[i] / maxAmplitude;
            float targetHeight = normalizedValue * maxBarHeight;
            targetHeight = std::min(targetHeight, maxBarHeight);

            // Smooth transition
            smoothedHeights[i] += (targetHeight - smoothedHeights[i]) * 0.25f;

            bars[i].setSize(
                sf::Vector2f(barWidth - spacing, smoothedHeights[i])
            );
            bars[i].setPosition(
                i * barWidth, window.getSize().y - smoothedHeights[i]
            );
        }

        window.clear(sf::Color(20, 20, 30));

        // Draw top flicker bar
        window.draw(topBar);

        // Draw bars
        for (const auto& bar: bars) { window.draw(bar); }

        window.display();
    }

    return 0;
}
