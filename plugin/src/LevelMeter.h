#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <algorithm>
#include <array>
#include <cmath>

/**
 * Stereo peak meter: two horizontal bars on a -60..0 dBFS scale (linear in dB),
 * green below -12 dB, yellow up to -3 dB, red above. The bar attacks instantly and
 * falls at kFallDbPerSec; a thin tick holds the recent peak for kPeakHoldMs and then
 * falls at kPeakFallDbPerSec. A CLIP box latches for kClipHoldMs after the processor
 * reports clipping, and a click on it clears it.
 *
 * All time-dependent state advances only through update(), which takes an explicit
 * timestamp, so the ballistics can be driven deterministically from a test.
 */
class LevelMeter : public juce::Component {
public:
    static constexpr float kFloorDb = -60.0f;
    static constexpr float kGreenTopDb = -12.0f;
    static constexpr float kYellowTopDb = -3.0f;
    static constexpr float kFallDbPerSec = 24.0f;
    static constexpr float kPeakFallDbPerSec = 12.0f;
    static constexpr double kPeakHoldMs = 1000.0;
    static constexpr double kClipHoldMs = 2000.0;

    /** Linear peak (0..1) to dBFS, floored at kFloorDb. */
    static float toDb(double linear) {
        if (!(linear > 0.0)) {
            return kFloorDb;
        }
        return std::max(kFloorDb, static_cast<float>(20.0 * std::log10(linear)));
    }

    /** Feed peaks (linear) measured since the previous call. nowMs is a monotonic clock. */
    void update(double leftPeak, double rightPeak, bool clipped, double nowMs) {
        const double dt = lastMs_ < 0.0 ? 0.0 : std::max(0.0, (nowMs - lastMs_) / 1000.0);
        lastMs_ = nowMs;
        const std::array<float, 2> input{toDb(leftPeak), toDb(rightPeak)};
        for (std::size_t ch = 0; ch < 2; ++ch) {
            float& level = levelDb_[ch];
            float& peak = peakDb_[ch];
            if (input[ch] >= level) {
                level = input[ch];
            } else {
                level = std::max(input[ch], level - kFallDbPerSec * static_cast<float>(dt));
            }
            if (level >= peak) {
                peak = level;
                peakHoldUntilMs_[ch] = nowMs + kPeakHoldMs;
            } else if (nowMs >= peakHoldUntilMs_[ch]) {
                peak = std::max(level, peak - kPeakFallDbPerSec * static_cast<float>(dt));
            }
        }
        if (clipped) {
            clip_ = true;
            clipUntilMs_ = nowMs + kClipHoldMs;
        } else if (clip_ && nowMs >= clipUntilMs_) {
            clip_ = false;
        }
        repaint();
    }

    float levelDb(int channel) const { return levelDb_[static_cast<std::size_t>(channel & 1)]; }
    float peakDb(int channel) const { return peakDb_[static_cast<std::size_t>(channel & 1)]; }
    bool clipLatched() const { return clip_; }
    void clearClip() {
        clip_ = false;
        repaint();
    }

    void mouseDown(const juce::MouseEvent& e) override {
        if (clipBox().contains(e.getPosition())) {
            clearClip();
        }
    }

    void paint(juce::Graphics& g) override {
        const auto track = juce::Colour(0xff2a2f3b);
        const auto text = juce::Colour(0xff9aa3b5);
        for (int ch = 0; ch < 2; ++ch) {
            const auto bar = barBounds(ch);
            g.setColour(track);
            g.fillRoundedRectangle(bar.toFloat(), 2.0f);

            const float levelX = dbToX(levelDb(ch), bar);
            drawSegment(g, bar, static_cast<float>(bar.getX()), std::min(levelX, dbToX(kGreenTopDb, bar)), juce::Colour(0xff3ddc84));
            if (levelDb(ch) > kGreenTopDb) {
                drawSegment(g, bar, dbToX(kGreenTopDb, bar), std::min(levelX, dbToX(kYellowTopDb, bar)),
                            juce::Colour(0xffffd54f));
            }
            if (levelDb(ch) > kYellowTopDb) {
                drawSegment(g, bar, dbToX(kYellowTopDb, bar), levelX, juce::Colour(0xffff5252));
            }
            if (peakDb(ch) > kFloorDb) {
                const float px = dbToX(peakDb(ch), bar);
                g.setColour(juce::Colour(0xffe8eaf0));
                g.fillRect(px - 1.0f, static_cast<float>(bar.getY()), 2.0f, static_cast<float>(bar.getHeight()));
            }
            g.setColour(text);
            g.setFont(juce::Font(juce::FontOptions(9.0f)));
            g.drawText(ch == 0 ? "L" : "R", 0, bar.getY() - 2, kLabelWidth - 2, bar.getHeight() + 4,
                       juce::Justification::centredRight, false);
        }

        const auto box = clipBox();
        g.setColour(clip_ ? juce::Colour(0xffff5252) : juce::Colour(0xff2c3242));
        g.fillRoundedRectangle(box.toFloat(), 3.0f);
        g.setColour(clip_ ? juce::Colours::white : text);
        g.setFont(juce::Font(juce::FontOptions(10.0f, juce::Font::bold)));
        g.drawText("CLIP", box, juce::Justification::centred, false);
    }

private:
    static constexpr int kLabelWidth = 12;
    static constexpr int kBarWidth = 170;
    static constexpr int kBarHeight = 8;
    static constexpr int kBarGap = 2;
    static constexpr int kClipGap = 6;
    static constexpr int kClipWidth = 38;

public:
    /** Preferred component size. */
    static constexpr int kPreferredWidth = kLabelWidth + kBarWidth + kClipGap + kClipWidth;
    static constexpr int kPreferredHeight = 28;

private:
    juce::Rectangle<int> barBounds(int channel) const {
        const int total = 2 * kBarHeight + kBarGap;
        const int top = (getHeight() - total) / 2;
        return {kLabelWidth, top + channel * (kBarHeight + kBarGap), kBarWidth, kBarHeight};
    }

    juce::Rectangle<int> clipBox() const {
        const int total = 2 * kBarHeight + kBarGap;
        return {kLabelWidth + kBarWidth + kClipGap, (getHeight() - total) / 2, kClipWidth, total};
    }

    static float dbToX(float db, juce::Rectangle<int> bar) {
        const float t = (std::clamp(db, kFloorDb, 0.0f) - kFloorDb) / (0.0f - kFloorDb);
        return static_cast<float>(bar.getX()) + t * static_cast<float>(bar.getWidth());
    }

    static void drawSegment(juce::Graphics& g, juce::Rectangle<int> bar, float x0, float x1, juce::Colour colour) {
        if (x1 > x0) {
            g.setColour(colour);
            g.fillRect(x0, static_cast<float>(bar.getY()), x1 - x0, static_cast<float>(bar.getHeight()));
        }
    }

    std::array<float, 2> levelDb_{kFloorDb, kFloorDb};
    std::array<float, 2> peakDb_{kFloorDb, kFloorDb};
    std::array<double, 2> peakHoldUntilMs_{0.0, 0.0};
    double lastMs_ = -1.0;
    double clipUntilMs_ = 0.0;
    bool clip_ = false;
};
