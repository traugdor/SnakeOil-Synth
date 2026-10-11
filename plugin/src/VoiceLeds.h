#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <algorithm>
#include <array>
#include <cstdint>

/**
 * Twelve small round LEDs, one per playable voice. A voice that was held shows green, a
 * released voice still ringing out shows amber, an idle one a dark recessed socket.
 * Brightness rises instantly and decays linearly over kFadeMs.
 *
 * Like LevelMeter, all time-dependent state advances only through update(), which takes
 * an explicit timestamp so the ballistics can be driven deterministically from a test.
 */
class VoiceLeds : public juce::Component, public juce::SettableTooltipClient {
public:
    static constexpr int kCount = 12;
    static constexpr int kMargin = 5;
    static constexpr int kDiameter = 9;
    static constexpr int kGap = 5;
    static constexpr double kFadeMs = 120.0;
    /** Width needed for the whole row, glow margin included. */
    static constexpr int kPreferredWidth = kCount * kDiameter + (kCount - 1) * kGap + 2 * kMargin;
    static constexpr int kPreferredHeight = kDiameter + 2 * kMargin;

    VoiceLeds() {
        setTooltip("Voice activity: one light per playable voice. Green = note held, amber = released note "
                   "still ringing.");
        setOpaque(false);
    }

    /**
     * Feed the activity bitmasks collected since the previous call (bit i = voice i).
     * A voice in both masks counts as held.
     */
    void update(std::uint32_t gatedMask, std::uint32_t releasingMask, double nowMs) {
        const double dt = lastMs_ < 0.0 ? 0.0 : std::max(0.0, nowMs - lastMs_);
        lastMs_ = nowMs;
        const float drop = static_cast<float>(dt / kFadeMs);
        for (std::size_t i = 0; i < kCount; ++i) {
            const std::uint32_t bit = 1u << i;
            green_[i] = (gatedMask & bit) != 0 ? 1.0f : std::max(0.0f, green_[i] - drop);
            amber_[i] = (releasingMask & bit) != 0 ? 1.0f : std::max(0.0f, amber_[i] - drop);
        }
        repaint();
    }

    /** Green brightness 0..1 of LED i. */
    float greenLevel(int i) const { return green_[static_cast<std::size_t>(i)]; }
    /** Amber brightness 0..1 of LED i. */
    float amberLevel(int i) const { return amber_[static_cast<std::size_t>(i)]; }
    /** Overall lit level 0..1 of LED i (the larger of the two colours). */
    float level(int i) const { return std::max(greenLevel(i), amberLevel(i)); }

    /** Bounds of LED i in this component's coordinates. */
    juce::Rectangle<int> ledBounds(int i) const {
        const int y = (getHeight() - kDiameter) / 2;
        return {kMargin + i * (kDiameter + kGap), y, kDiameter, kDiameter};
    }

    void paint(juce::Graphics& g) override {
        const auto green = juce::Colour(0xff3ddc6a);
        const auto amber = juce::Colour(0xffffb02e);
        for (int i = 0; i < kCount; ++i) {
            const auto r = ledBounds(i).toFloat();
            const bool useGreen = greenLevel(i) > 0.0f;
            const float b = useGreen ? greenLevel(i) : amberLevel(i);
            const auto lit = useGreen ? green : amber;

            if (b > 0.0f) {  // soft halo
                const auto centre = r.getCentre();
                const float radius = r.getWidth() * 1.0f;
                juce::ColourGradient halo(lit.withAlpha(0.45f * b), centre.x, centre.y, lit.withAlpha(0.0f),
                                          centre.x + radius, centre.y, true);
                g.setGradientFill(halo);
                g.fillEllipse(r.expanded(radius - r.getWidth() * 0.5f));
            }

            // Recessed socket: dark body, light rim.
            g.setColour(juce::Colour(0xff0b0d11));
            g.fillEllipse(r);
            if (b > 0.0f) {
                const auto body = juce::Colour(0xff10281a).interpolatedWith(lit, b);
                g.setColour(useGreen ? body : juce::Colour(0xff2a2010).interpolatedWith(lit, b));
                g.fillEllipse(r.reduced(0.5f));
                g.setColour(juce::Colours::white.withAlpha(0.45f * b));  // specular dot
                g.fillEllipse(r.getX() + r.getWidth() * 0.28f, r.getY() + r.getHeight() * 0.2f,
                              r.getWidth() * 0.3f, r.getHeight() * 0.26f);
            }
            g.setColour(juce::Colour(0xff3a4150).interpolatedWith(lit, 0.6f * b));
            g.drawEllipse(r.reduced(0.5f), 1.0f);
        }
    }

private:
    std::array<float, kCount> green_{};
    std::array<float, kCount> amber_{};
    double lastMs_ = -1.0;
};
