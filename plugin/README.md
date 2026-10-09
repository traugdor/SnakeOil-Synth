# SnakeOil Synth VST3 plugin

C++ port of the Python reference synth (`midi_synth/`) as a VST3 instrument (plus a Standalone app) built on JUCE.
Design: `docs/superpowers/specs/2026-10-06-vst3-plugin-design.md`. Status: phases P1-P5 complete and P6 partly -
the JUCE shell runs a DSP core that matches the Python reference bit-for-bit across 42 golden scenarios: oscillators
(saw + square layer, PWM, fm/am/ring/sync), envelopes, filter env/key-tracking/velocity, 12/24 dB filters + whistle,
glide, LFOs, unison, noise, limiter, stereo effects (chorus, ping-pong delay, reverb, bitcrush), the mod matrix,
hybrid voice allocation with tail slots, and host-tempo-synced delay. A custom editor (scrolling grouped knobs,
a mod-matrix table, a graphical level meter) drives every parameter. Remaining: the P6 real-time-safety pass (the engine still looks parameters up by string each block),
and P7 (CLAP/macOS/installer). The JUCE plugin build is not compiled in the development sandbox (no
CMake/JUCE/system headers); only the no-JUCE DSP, golden harness and self-test are built there.

## Build, test, install (Windows, no admin)

Requires Visual Studio 2026 Community (MSVC). The scripts load the MSVC environment and use the CMake and Ninja
bundled with Visual Studio. Run from the repo root in `cmd` or Git Bash:

    plugin\tools\build.bat            # configure + build Release into plugin\build\Release
    plugin\tools\build.bat debug      # Debug build into plugin\build\Debug
    plugin\tools\test.bat             # run CTest
    plugin\tools\install_vst3.bat     # copy the bundle to %LOCALAPPDATA%\Programs\Common\VST3

The first configure fetches JUCE and doctest from GitHub (needs network; sources land in `plugin\build`).

Outputs: `plugin\build\Release\SnakeOilSynth_artefacts\Release\VST3\SnakeOil Synth.vst3` and
`...\Standalone\SnakeOil Synth.exe`.

## Layout

- `dsp/snakeoil/` - `snakeoil_dsp`, pure C++20 DSP with no JUCE dependency (headers plus `version.cpp`):
  `biquad.hpp`, `constants.hpp`, `effects.hpp`, `engine.hpp`, `envelope.hpp`, `lfo.hpp`, `limiter.hpp`,
  `mod_matrix.hpp`, `oscillator.hpp`, `smoother.hpp`, `version.hpp`, `voice.hpp`.
- `src/` - JUCE wrapper:
  - `PluginProcessor.h/.cpp` - parameters (APVTS built from the registry), MIDI, engine glue, meter readout.
  - `PluginEditor.h/.cpp` - the editor: a slim title strip above a `juce::Viewport` whose content reproduces the
    Python app's hand-placed five-column grid (Oscillator 1 | Oscillator 2 | Modulation | Master | Tempo + Noise;
    Filter | Filter Env | Amp Envelope | LFO | Mod Matrix; Effects across three columns | Unison | Glide). Columns
    are as wide as their widest box, rows as tall as their tallest, and boxes stretch to fill their cell. The
    registry groups LFO 1 and LFO 2 share one "LFO" box as two vertical stacks; Effects is laid out as toggle
    blocks (each root toggle heads a block, its dependents - found through the generated `under` field - sit in
    rows beneath it, at most six columns wide); the Mod Matrix is a Source / Scale / Destination table; the level
    meter sits inside the Master box next to its controls, with a static "GR off" label (the processor reports
    no gain reduction). Toggles are Off/On buttons. Content is about 1460 px wide, so the default window is
    that size (capped at 1500 x 800); a narrower window scrolls the fixed grid horizontally and vertically. The
    dark `LookAndFeel_V4` theme lives in the same file. Unknown registry groups form an extra row at the bottom.
  - `LevelMeter.h` - header-only stereo peak meter (two vertical bars, -60..0 dBFS, peak hold, latching CLIP);
    its ballistics take an explicit timestamp so they can be driven from a test.
  - `params_gen.hpp` - generated parameter registry (do not edit by hand).
- `tests/` - doctest unit tests (`snakeoil_tests`), the golden harness (`golden_check`), `dsp_selftest`, and
  `editor_snapshot.cpp` (the `EditorSnapshot` tool, below).
- `tools/` - build, test and install scripts.

## Editor layout test and snapshots

`EditorSnapshot` is a console app that builds the real processor and editor without a host:

    EditorSnapshot --check             # layout invariants; non-zero exit on failure (ctest: editor_layout)
    EditorSnapshot --png <dir>         # write editor_default.png and editor_full.png (whole scrollable body)

`--check` runs at the default, the full-content and the minimum window size (at the minimum the grid scrolls,
so only the content-space checks apply) and verifies: every knob has at least a 56 px square dial area; every
control lies strictly inside its group box; no two controls in a group overlap; every group lies inside the
content and no two group boxes overlap; the boxes sit in the Python app's grid (rows by y, columns by x, equal
height per row, Effects spanning the first three columns, Mod Matrix right of the LFO); the LFO box holds two
stacks (LFO 1 left of LFO 2) under centred headers; every Effects dependent lies below its root toggle within
its block; each mod-matrix row has its three controls on one line under the header; the level meter is in the
Master box right of its controls; there is exactly one control per parameter (85); and at the default and full
sizes there is no horizontal overflow. The binary is
`plugin\build\Release\EditorSnapshot_artefacts\Release\EditorSnapshot.exe`. Look at the PNGs after any layout change.

## Golden sound lock

`golden_check <scenario.json> <expected.f32>` replays a scenario and compares the
rendered audio with the Python reference within 1e-6 absolute. `ctest` runs the
`default` scenario. Regenerate the scenario and expected audio after an
intentional sound change:

    python tools/export_cpp_golden.py default

`tools/export_params.py` writes `plugin/params.json` (the parameter registry) for
the generated parameter layout in P2.

## Host setup

- REAPER: Options > Preferences > Plug-ins > VST. Add `%LOCALAPPDATA%\Programs\Common\VST3` to the VST plug-in
  paths if it is not scanned, then Re-scan.
- Cakewalk Sonar: Preferences > Audio > Plug-in Manager. Add the same path to the scan paths and rescan.

## Phases

P0 scaffolding and toolchain; P1 DSP core and golden harness; P2 plugin v1 (parameters, MIDI, generic editor);
P3 sound-shaping parity; P4 mod matrix, tempo sync, voice allocation, meters; P5 custom editor and presets;
P6 hardening and validation; P7 optional CLAP, macOS, installer.

## Licensing

JUCE is AGPL-3.0 or commercial. Read its license before distributing any binary.
