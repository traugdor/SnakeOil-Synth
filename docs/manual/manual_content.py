"""Text of the SnakeOil Synth user manual. Rendered by build_manual.py."""

CSS = r"""
@page { size: Letter; margin: 22mm 20mm 24mm 20mm;
  @top-left { content: "SnakeOil Synth User Manual"; font: 8.5pt Arial, sans-serif; color: #6b7280; }
  @top-right { content: "Version 0.1.0"; font: 8.5pt Arial, sans-serif; color: #6b7280; }
  @bottom-center { content: counter(page); font: 9pt Arial, sans-serif; color: #374151; } }
@page cover { margin: 0; @top-left { content: none; } @top-right { content: none; } @bottom-center { content: none; } }
:root { --accent: #0e6f8e; --ink: #1f2937; --soft: #eef6f9; }
* { box-sizing: border-box; }
html { font: 10.2pt/1.5 Arial, "Helvetica Neue", sans-serif; color: var(--ink); }
body { margin: 0; }
p { margin: 0 0 8pt; orphans: 3; widows: 3; }
a { color: inherit; text-decoration: none; }
h1, h2, h3 { font-family: Arial, sans-serif; color: #0b4a5f; break-after: avoid; }
h1 { font-size: 22pt; margin: 0 0 12pt; padding-bottom: 6pt; border-bottom: 2.5pt solid var(--accent); }
h2 { font-size: 14.5pt; margin: 20pt 0 6pt; }
h3 { font-size: 11.5pt; margin: 14pt 0 4pt; color: #1f2937; }
h1.newpage { break-before: page; }
.num { color: var(--accent); margin-right: 4pt; }
ul, ol { margin: 0 0 8pt; padding-left: 18pt; } li { margin-bottom: 3pt; }
code, .mono { font-family: Consolas, "Courier New", monospace; font-size: 9.2pt; background: #f1f5f9; padding: 0 2pt; border-radius: 2pt; }
.kbd { font-family: Arial, sans-serif; font-size: 8.5pt; border: 0.7pt solid #9ca3af; border-bottom-width: 1.6pt; border-radius: 3pt; padding: 0 4pt; background: #f9fafb; }
table { border-collapse: collapse; width: 100%; margin: 6pt 0 12pt; font-size: 9pt; break-inside: auto; }
th { background: #0b4a5f; color: #fff; text-align: left; padding: 4pt 6pt; font-weight: 600; }
td { border-bottom: 0.6pt solid #d1d5db; padding: 4pt 6pt; vertical-align: top; }
tr { break-inside: avoid; } tbody tr:nth-child(even) td { background: #f8fafc; }
table.controls { break-inside: avoid; }
table.controls td:first-child { font-weight: 700; white-space: nowrap; }
table.compact td, table.compact th { padding: 2.5pt 5pt; }
.callout { border-left: 3.5pt solid var(--accent); background: var(--soft); padding: 6pt 9pt; margin: 8pt 0 10pt; break-inside: avoid; }
.callout.tip { border-color: #15803d; background: #effaf2; }
.callout.warn { border-color: #b45309; background: #fff7ea; }
figure { margin: 8pt 0 12pt; break-inside: avoid; }
figcaption { font-size: 8.5pt; color: #4b5563; margin-top: 3pt; text-align: center; }
.crop .cropimg { margin: 0 auto; border: 0.8pt solid #9ca3af; background-repeat: no-repeat; }
.shot { position: relative; width: 100%; border: 0.8pt solid #9ca3af; }
.shot img { display: block; width: 100%; }
.badge { position: absolute; width: 17pt; height: 17pt; margin: -8.5pt 0 0 -8.5pt; border-radius: 50%; background: #d9480f; color: #fff;
  font: 700 9pt/17pt Arial, sans-serif; text-align: center; box-shadow: 0 0 0 1.4pt #fff; }
.legend { columns: 2; column-gap: 20pt; font-size: 9.2pt; margin: 4pt 0 10pt; }
.legend div { break-inside: avoid; margin-bottom: 2pt; }
.legend b { display: inline-block; width: 16pt; height: 16pt; line-height: 16pt; border-radius: 50%; background: #d9480f; color: #fff; text-align: center; font-size: 8.5pt; margin-right: 5pt; }
.flow { display: flex; flex-wrap: wrap; align-items: stretch; gap: 5pt 0; margin: 8pt 0 12pt; }
.flow .st { border: 1pt solid var(--accent); background: var(--soft); border-radius: 4pt; padding: 5pt 7pt; font-size: 8.8pt; text-align: center; min-width: 78pt; }
.flow .st small { display: block; color: #4b5563; font-size: 7.6pt; }
.flow .ar { align-self: center; padding: 0 4pt; color: var(--accent); font-weight: 700; }
.flow .st.hot { background: #fff1e6; border-color: #d9480f; }
.cover { page: cover; break-after: page; height: 100vh; min-height: 11in; background: linear-gradient(160deg, #0b2530 0%, #0e3a4d 55%, #0e6f8e 100%); color: #fff; padding: 2.1in 1in 0; position: relative; }
.cover .brand { font: 700 11pt Arial; letter-spacing: 3pt; color: #7dd3fc; text-transform: uppercase; }
.cover h1 { color: #fff; border: 0; font-size: 46pt; line-height: 1.05; margin: 14pt 0 10pt; padding: 0; }
.cover .sub { font-size: 17pt; color: #cbe9f5; margin-bottom: 36pt; }
.cover .ver { font-size: 11pt; color: #cbe9f5; line-height: 1.7; }
.cover .snake { position: absolute; left: 1in; right: 1in; bottom: 1.1in; font-size: 9pt; color: #9bd1e6; border-top: 0.8pt solid #3f8aa6; padding-top: 8pt; }
.legal { font-size: 9pt; color: #374151; }
.legal h2 { font-size: 11pt; margin-top: 14pt; }
.toc-title { break-before: page; }
.toc a { display: flex; align-items: baseline; }
.toc .tn { width: 28pt; flex: none; color: var(--accent); font-weight: 700; }
.toc .tt { flex: 1; border-bottom: 0.6pt dotted #9ca3af; margin-right: 5pt; }
.toc .tp { width: 20pt; text-align: right; flex: none; }
.toc1 { margin-top: 7pt; font-weight: 700; font-size: 10.4pt; } .toc2 { font-size: 9.4pt; padding-left: 14pt; }
.index { columns: 2; column-gap: 22pt; font-size: 9pt; }
.index .letter { font-weight: 700; color: var(--accent); margin: 8pt 0 2pt; break-after: avoid; }
.ientry { display: flex; justify-content: space-between; border-bottom: 0.5pt dotted #cbd5e1; break-inside: avoid; }
.ipages { text-align: right; padding-left: 8pt; }
.xref { color: var(--accent); font-weight: 700; }
.range { white-space: nowrap; }
.small { font-size: 8.5pt; color: #4b5563; }
"""

COVER = """
<section class="cover">
  <div class="brand">SnakeOil</div>
  <h1>SnakeOil Synth</h1>
  <div class="sub">User Manual</div>
  <div class="ver">VST3 instrument and standalone application for Windows<br>
  Version 0.1.0 &middot; Two oscillators &middot; Moog-style 12/24 dB filter<br>
  Mod matrix &middot; Unison &middot; Stereo effects</div>
  <div class="snake">A C++ port of the SnakeOil Synth reference synthesizer. VST is a trademark of Steinberg Media Technologies GmbH.</div>
</section>
"""

FRONT = """
<section class="legal">
<h1 style="break-before:avoid">About this manual</h1>
<p><b>Product.</b> SnakeOil Synth, version 0.1.0 (pre-release), VST3 plug-in and standalone application, 64-bit Windows.</p>
<p><b>Who this is for.</b> Musicians who want to play and program SnakeOil Synth. You do not need to know how synthesizers work internally: every control is explained in plain words, and the reference section tells you the range, the default and the sound of every setting. If you already know subtractive synthesis, skim chapters 1&ndash;5 and use chapters 6&ndash;9 as a reference.</p>
<p><b>How it is organised.</b> The manual follows the usual structure of a synthesizer manual and keeps tasks and reference apart: <i>getting started</i> (installation, first sound, how the window works), <i>concepts</i> (signal flow, voices), <i>reference</i> (one section per box of the window, then MIDI, parameters, specifications) and <i>help</i> (recipes, troubleshooting, glossary, index). Chapter numbers in blue, like <span class="xref">6</span>, are links in the PDF.</p>
<p><b>Conventions.</b> Control names appear in <b>bold</b> exactly as they are printed in the window. Values are written as they appear in the value boxes, for example <code>0.75</code>, <code>2000</code> Hz or <code>+12</code>. A <i>Note</i> adds information, a <i>Tip</i> suggests a way of working and a <i>Caution</i> warns about something that can surprise you or hurt your ears.</p>
<div class="callout warn"><b>Caution.</b> A synthesizer can produce loud, sudden sounds, especially with the filter resonance turned up, with <b>Volume</b> above 1.00, or with effects feeding back. Keep your monitors or headphones at a moderate level while you explore.</div>
<h2>Software notices</h2>
<p>SnakeOil Synth is built with the JUCE framework, which is available under the AGPL-3.0 licence or a commercial licence. Read the JUCE licence before you distribute binaries. VST is a registered trademark of Steinberg Media Technologies GmbH. All other product names are trademarks of their owners.</p>
</section>
"""


def box_for(m, name):
    m.crop(name)


def build(m):
    # ------------------------------------------------------------------ 1
    m.h(1, "intro", "Welcome to SnakeOil Synth")
    m.p("SnakeOil Synth is a polyphonic, subtractive-style synthesizer with a character of its own: a layered saw-and-pulse oscillator, a second oscillator that can frequency-modulate, ring-modulate or hard-sync to the first, a resonant low-pass filter that you can switch between a gentle 12 dB slope and a Moog-style 24 dB ladder that sings at the top of its resonance range, two LFOs, an eight-row modulation matrix, stackable unison voices and a chain of stereo effects.")
    m.p("It started life as a Python program, and the version you are using is a C++ re-implementation of that program. The sound engine has been checked sample for sample against the original, so the two versions sound the same; the C++ version simply has far more headroom, which is why it can ring more released notes at once.")
    m.h(2, "intro-features", "What you get")
    m.ul([
        "<b>Two oscillators</b> &ndash; oscillator 1 is a band-limited saw with an optional pulse layer, oscillator 2 is a pulse (square) wave. Both have pulse-width control, and oscillator 2 has coarse and fine tuning.",
        "<b>Five oscillator interaction modes</b> &ndash; off, FM, AM, ring modulation and hard sync, with one amount control.",
        "<b>Noise generator</b> &ndash; white, pink or brown, mixed into every voice.",
        "<b>Resonant low-pass filter</b> &ndash; 12 or 24 dB per octave, with its own envelope, key tracking and velocity response, per voice or on the master bus. The 24 dB mode self-oscillates into a pure whistle.",
        "<b>Two ADSR envelopes</b> &ndash; one for the volume, one for the filter.",
        "<b>Two LFOs</b> &ndash; six wave shapes, aimed at pitch, filter, pulse width, volume or each other's speed.",
        "<b>Eight-row modulation matrix</b> &ndash; note number, both LFOs, mod wheel and aftertouch can steer 33 destinations.",
        "<b>Glide</b> (portamento), <b>unison</b> (up to 12 stacked voices per note) and a tempo-synchronised <b>delay</b>.",
        "<b>Stereo effects</b> &ndash; chorus, delay with ping-pong, reverb and bitcrusher.",
        "<b>Auto limiter, soft clipper and stereo level meter</b> to keep the output under control.",
        "<b>85 parameters</b>, all of which your host can automate and store with the project.",
    ])
    m.tag("Features")
    m.h(2, "intro-formats", "Plug-in or standalone")
    m.p("SnakeOil Synth is delivered in two forms. The <b>VST3 plug-in</b> loads inside a digital audio workstation (a &ldquo;host&rdquo;) such as REAPER or Cakewalk Sonar, where it receives MIDI from your tracks and sends audio into the mixer. The <b>standalone application</b> runs by itself and talks to your audio and MIDI hardware directly. Both have exactly the same window and the same sound. Where they differ, the manual says so.")
    m.tag("VST3", "Standalone application")

    # ------------------------------------------------------------------ 2
    m.h(1, "install", "Installation and setup")
    m.h(2, "install-req", "System requirements")
    m.table(("Item", "Requirement"), [
        ("Operating system", "Windows 10 or Windows 11, 64-bit"),
        ("Plug-in format", "VST3 (64-bit). Tested in REAPER and Cakewalk Sonar"),
        ("Audio", "Any sample rate your host or audio interface supports. The engine adapts to the rate; 44.1 kHz and 48 kHz are the ones that have been verified against the reference"),
        ("MIDI", "A MIDI keyboard or controller (or a MIDI track in your host). The plug-in needs no other input"),
        ("Display", "The window is resizable and scrolls. It is laid out for about 1,470 by 810 pixels and fits smaller screens by scrolling"),
    ], cls="compact", widths=("28%", "72%"))
    m.h(2, "install-vst3", "Installing the VST3 plug-in")
    m.p("A VST3 plug-in is a folder named <code>SnakeOil Synth.vst3</code>. Copy that whole folder into your VST3 folder. For a normal per-user installation, which needs no administrator rights, this is:")
    m.p("<code>%LOCALAPPDATA%\\Programs\\Common\\VST3</code>")
    m.p("The system-wide folder, <code>C:\\Program Files\\Common Files\\VST3</code>, works as well but needs administrator rights to write to. If you build the plug-in from source, the script <code>plugin\\tools\\install_vst3.bat</code> copies the freshly built folder into the per-user location for you.")
    m.note("note", "Close your host before you replace an installed copy. A host keeps the plug-in file open while it is running, and Windows cannot overwrite a file that is in use.")
    m.h(2, "install-reaper", "Setting up REAPER")
    m.ol([
        "Open <b>Options &rsaquo; Preferences &rsaquo; Plug-ins &rsaquo; VST</b>.",
        "If the per-user VST3 folder is not already listed in the VST plug-in paths, add <code>%LOCALAPPDATA%\\Programs\\Common\\VST3</code> (type the expanded path, for example <code>C:\\Users\\yourname\\AppData\\Local\\Programs\\Common\\VST3</code>).",
        "Click <b>Re-scan</b> (a full re-scan if the plug-in does not appear), then <b>OK</b>.",
        "Create a track, click <b>FX</b> on the track, press <b>Add</b>, and choose <b>VST3: SnakeOil Synth (SnakeOil)</b>.",
        "Arm the track for recording and set its input to your MIDI keyboard (or draw MIDI items), then play.",
    ])
    m.h(2, "install-sonar", "Setting up Cakewalk Sonar")
    m.ol([
        "Open <b>Edit &rsaquo; Preferences &rsaquo; Audio &rsaquo; Plug-in Manager</b> (the exact menu path varies by version).",
        "Make sure the VST3 folder above is one of the scan paths, then run <b>Scan</b>.",
        "SnakeOil Synth is filed under <b>Synth</b> as an <b>Instrument</b>. Insert it with <b>Insert &rsaquo; Soft Synth</b> or from the Synth Rack.",
        "Choose your MIDI input for the instrument track and play.",
    ])
    m.note("tip", "If Sonar does not list the plug-in after you install or update it, run a full rescan from the Plug-in Manager. Look under <b>Synth</b>, as an <b>Instrument</b>.")
    m.h(2, "install-standalone", "Running the standalone application")
    m.p("The standalone program is a single executable called <code>SnakeOil Synth.exe</code>. It is not copied anywhere by the install script; run it from where it was built or copy it where you like. When it starts you see the main window. Click the <b>Options</b> button in the top-left corner to choose your <b>audio output device</b>, sample rate, buffer size and <b>MIDI input</b> device. Pick the lowest buffer size that plays without crackles; on Windows, an ASIO driver (such as ASIO4ALL or your interface's own) gives the lowest delay between pressing a key and hearing the note.")
    m.tag("Options button", "Audio device", "Buffer size")

    # ------------------------------------------------------------------ 3
    m.h(1, "quick", "Quick start: your first sound")
    m.p("Load SnakeOil Synth in your host (or start the standalone app). When it opens for the first time every control is at its factory value, which is a bright, simple, ready-to-play lead. To hear it:")
    m.ol([
        "Press a key on your MIDI keyboard. You should hear a bright saw-and-pulse tone at the pitch of the key you pressed.",
        "Open the <b>Filter</b> box and drag <b>Cutoff</b> up and down while you hold a note. The sound gets darker and brighter. This is the single most important control on any subtractive synthesizer.",
        "In <b>Oscillator 2</b> bring <b>Level</b> up to about <code>0.50</code>. Oscillator 2 is silent at the factory setting, so this adds a second voice an octave above the first (<b>Octave up</b> is on by default).",
        "Change <b>Fine</b> in oscillator 2 a little, or raise <b>Detune</b> in <b>Unison</b> after setting <b>Voices</b> to <code>3</code>, and the sound widens and thickens.",
        "Switch on <b>Reverb</b> in the <b>Effects</b> box for some space.",
    ])
    m.note("tip", "Nothing happens when you play, or the sound is very quiet? Check that <b>Volume</b> in the <b>Master</b> box is at <code>0.80</code>, that <b>Level</b> in <b>Oscillator 1</b> is at <code>1.00</code>, and that your host track is armed and routed to the instrument. Chapter [[ref:trouble]] has a full checklist.")
    m.h(2, "quick-defaults", "The factory sound in one table")
    m.table(("Part", "Factory setting"), [
        ("Oscillator 1", "Saw wave, square layer on at 0.50, level 1.00, octave switch off"),
        ("Oscillator 2", "Square wave, level 0.00 (silent), octave up on"),
        ("Modulation", "Mode fm, amount 0.00 (no interaction)"),
        ("Filter", "12 dB low-pass, cutoff 2000 Hz, resonance 0.00, per voice; envelope amount, key tracking and velocity amount all 0.00"),
        ("Amp envelope", "Attack 6 ms, decay 120 ms, sustain 0.75, release 180 ms"),
        ("LFOs", "Rate 5 Hz, depth 0.00 (off); LFO 1 to pitch, LFO 2 to filter"),
        ("Unison, glide, noise", "All off (1 voice, glide time 0, noise level 0)"),
        ("Effects", "All four off"),
        ("Master", "Volume 0.80, velocity on, auto limiter off"),
        ("Voices", "12 playable notes plus 12 tail slots for released notes"),
    ], cls="compact", widths=("24%", "76%"))

    # ------------------------------------------------------------------ 4
    m.h(1, "ui", "The window")
    m.p("The window is one scrolling panel of boxes. Each box groups the controls for one part of the synthesizer. The layout follows the original program: five columns of boxes in three rows. Figure 1 numbers the boxes; chapter [[ref:ref]] describes them in the same order.")
    badges = []
    for i, name in enumerate(["Oscillator 1", "Oscillator 2", "Modulation", "Master", "Tempo", "Noise", "Filter",
                              "Filter Env", "Amp Envelope", "LFO", "Mod Matrix", "Effects", "Unison", "Glide"], 1):
        x1, y1, x2, y2 = __import__("build_manual").BOXES[name]
        badges.append('<span class="badge" style="left:%.2f%%;top:%.2f%%">%d</span>'
                      % ((x1 + 3) * 100.0 / 1470, (y1 + 1) * 100.0 / 808, i))
    m.raw('<figure><div class="shot"><img src="assets/standalone-ui.png" alt="SnakeOil Synth main window">%s</div>'
          '<figcaption>Figure 1. The SnakeOil Synth window (standalone application shown). The numbers match the legend below.</figcaption></figure>'
          % "".join(badges))
    legend = ["Oscillator 1", "Oscillator 2", "Modulation", "Master", "Tempo", "Noise", "Filter", "Filter Env",
              "Amp Envelope", "LFO", "Mod Matrix", "Effects", "Unison", "Glide"]
    secs = ["osc1", "osc2", "mod", "master", "tempo", "noise", "filter", "fenv", "aenv", "lfo", "matrix", "fx", "unison", "glide"]
    m.raw('<div class="legend">%s</div>' % "".join(
        '<div><b>%d</b>%s &ndash; section [[ref:%s]]</div>' % (i + 1, n, s) for i, (n, s) in enumerate(zip(legend, secs))))
    m.h(2, "ui-controls", "Working the controls")
    m.table(("Control", "How to use it"), [
        ("Knob", "Drag it up or down (or left and right). <b>Double-click</b> to return it to its factory value. The mouse wheel does <i>not</i> turn knobs, so you can scroll the window with the wheel without changing the sound by accident."),
        ("Value box", "The number under every knob. Click it, type a number, then press <span class='kbd'>Enter</span>. This is the fastest way to dial in an exact setting such as a cutoff of <code>261.63</code> Hz. Values outside the control's range are clipped to the range."),
        ("Off/On button", "Click to switch. Lit blue means on."),
        ("Drop-down list", "Click to open the list, click an entry. Used for choices such as <b>Mode</b>, <b>Wave</b> and <b>Division</b>."),
        ("Horizontal slider", "Used for the eight <b>Scale</b> controls of the mod matrix. Drag the handle; double-click to return it to 0. The bar grows from the centre line, to the left for negative and to the right for positive values."),
        ("Level meter", "The two vertical bars in the <b>Master</b> box. See section [[ref:master]]."),
        ("Tails n/n label", "The small grey text under the <b>Unison</b> controls. Click it to switch between 12 and 24 tail slots. See section [[ref:voices-tails]]."),
        ("Tooltips", "Hold the pointer still over a shortened label or over the <b>Mod Matrix</b> box to see the full text."),
        ("Scroll and resize", "Drag the bottom-right corner to resize the window; scroll bars appear when the window is smaller than the content. Standalone only: the <b>Options</b> button opens the audio and MIDI settings."),
    ], cls="compact", widths=("20%", "80%"))
    m.tag("Double-click reset", "Value box", "Tooltips", "Window resizing")
    m.h(2, "ui-automation", "Automation, presets and saving")
    m.p("Every control, except the tail-slot switch, is a normal plug-in parameter. Your host lists all 85 of them, can record and play back their movement as <b>automation</b>, and lets you map them to a hardware controller. The host also stores the settings with your project and saves and recalls them as <b>presets</b> using its own preset system. The standalone application normally remembers its last state between runs.")
    m.note("note", "The original Python program had built-in &ldquo;profiles&rdquo; for learning MIDI controller assignments, a patch library, an on-screen QWERTY keyboard and a WAV recorder. The plug-in does not repeat these, because hosts do them better: use your host's MIDI mapping or parameter-learn feature, its preset manager and its own recording.")
    m.note("note", "The tail-slot setting (section [[ref:voices-tails]]) is saved with the project too, but it is a mode of the synth, not an automatable parameter.")
    m.tag("Automation", "Presets")

    # ------------------------------------------------------------------ 5
    m.h(1, "flow", "How the sound is made")
    m.p("Knowing the path the sound takes makes every control easier to understand. Each note you play starts one <b>voice</b> (or several, with unison). A voice makes its sound, shapes it and then joins all other voices. The mix is then polished by effects and a pair of output safeguards.")
    m.h(2, "flow-voice", "Inside one voice")
    m.raw("""<div class="flow">
<div class="st">Oscillator 1<small>saw + pulse layer</small></div><div class="ar">&rarr;</div>
<div class="st">Modulation<small>FM / AM / ring / sync</small></div><div class="ar">&rarr;</div>
<div class="st">Oscillator 2<small>square</small></div><div class="ar">+</div>
<div class="st">Noise<small>white / pink / brown</small></div><div class="ar">&rarr;</div>
<div class="st hot">Filter<small>low-pass 12 or 24 dB</small></div><div class="ar">&rarr;</div>
<div class="st">Amp envelope<small>ADSR + velocity</small></div><div class="ar">&rarr;</div>
<div class="st">Pan<small>unison spread</small></div></div>""")
    m.p("Oscillator 1 is always heard at its own <b>Level</b> and is also the source that oscillator 2 listens to in the <b>Modulation</b> box. Oscillator 2 and the noise are added to oscillator 1. The sum goes through the filter, then through the amplitude envelope, which turns the continuous tone into a note with a beginning and an end. Finally each voice is placed in the stereo field. The <b>LFOs</b>, the <b>filter envelope</b> and the <b>mod matrix</b> move the controls above while the note plays.")
    m.h(2, "flow-master", "From the voices to the output")
    m.raw("""<div class="flow">
<div class="st">All voices<small>summed</small></div><div class="ar">&rarr;</div>
<div class="st">Master-bus filter<small>only if switched on</small></div><div class="ar">&rarr;</div>
<div class="st">Chorus</div><div class="ar">&rarr;</div><div class="st">Delay</div><div class="ar">&rarr;</div>
<div class="st">Reverb</div><div class="ar">&rarr;</div><div class="st">Bitcrush<small>effects you switched on</small></div><div class="ar">&rarr;</div>
<div class="st">Volume</div><div class="ar">&rarr;</div><div class="st">Auto limiter<small>if on</small></div><div class="ar">&rarr;</div>
<div class="st hot">Soft clipper</div><div class="ar">&rarr;</div><div class="st">Meter &amp; output</div></div>""")
    m.p("The effects always run in the order chorus, delay, reverb, bitcrush, whichever you switch on. <b>Volume</b> comes after the effects, so it scales the whole sound including echoes and reverb tails. The auto limiter and the soft clipper are the last stages: the limiter (when on) keeps peaks from reaching the clipper, and the clipper smoothly rounds any peak that still gets through instead of letting it distort harshly.")
    m.tag("Signal flow", "Voice")

    # ------------------------------------------------------------------ 6
    m.h(1, "ref", "Reference: the boxes of the window")
    m.p("This chapter explains every box, in the order of the numbers in Figure 1: first the top row (oscillators, modulation, master, tempo, noise), then the middle row (filter, envelopes, LFO, mod matrix) and the bottom row (effects, unison, glide). For each control you find its <b>range</b>, its <b>default</b> (the factory value) and what it does to the sound. Some controls change the numbers they display as you turn them: all ranges here are the ranges of the value box.")

    # ---- Oscillator 1
    m.h(2, "osc1", "Oscillator 1")
    m.crop("Oscillator 1")
    m.p("Oscillator 1 is the main voice. It is a <b>saw wave</b>, rich in harmonics and bright, with an optional <b>square layer</b> added on top. The waves are built to avoid aliasing (the harsh, inharmonic fizz that simple digital oscillators produce at high pitches), so they stay clean across the keyboard.")
    m.controls([
        ("Level", "0.00 &ndash; 1.00", "1.00", "Volume of oscillator 1 in the mix before the filter. At <code>0.00</code> oscillator 1 is silent, but it still acts as the modulator for the <b>Modulation</b> box."),
        ("Square layer", "Off / On", "On", "Adds a pulse wave on top of the saw. The pulse is derived from the same ramp as the saw, in the style of the Juno synthesizers, so the two stay phase-locked and sound like one fat wave rather than two."),
        ("Sq Level", "0.00 &ndash; 1.00", "0.50", "How much of the square layer is added. At <code>0.00</code> you hear the plain saw."),
        ("PWM", "0.00 &ndash; 0.50", "0.00", "<i>Pulse width</i> of the square layer. <code>0.50</code> is a plain square wave (hollow, clarinet-like); smaller values give a narrower pulse (thin, nasal). At <code>0.00</code> the pulse is at its narrowest and adds little loudness, so raising PWM also makes the layer louder. Movement of this control (by hand, LFO or the matrix) gives the classic &ldquo;chorused&rdquo; pulse-width-modulation sound."),
        ("Octave down", "Off / On", "Off", "Plays oscillator 1 one octave below the note you pressed. Useful for basses and for making the saw heavier."),
    ])
    m.tag("Saw wave", "Square layer", "PWM (pulse width)", "Octave switches", "Oscillator 1")

    # ---- Oscillator 2
    m.h(2, "osc2", "Oscillator 2")
    m.crop("Oscillator 2")
    m.p("Oscillator 2 is a <b>square (pulse) wave</b>. It is silent at the factory setting. Bring it in with <b>Level</b> and tune it against oscillator 1 for thick, detuned sounds or for harmonic intervals. It is also the oscillator that the <b>Modulation</b> box processes.")
    m.controls([
        ("Level", "0.00 &ndash; 1.00", "0.00", "Volume of oscillator 2. In FM, AM and ring modes oscillator 2 must be audible (level above <code>0.00</code>) for the modulation to be heard."),
        ("Coarse", "&minus;12.0 &ndash; +12.0", "0.0", "Tuning in semitones relative to the note, in steps you can type to a tenth (<code>7.0</code> is a perfect fifth up, <code>12.0</code> an octave up). Use whole numbers for intervals."),
        ("Fine", "&minus;0.50 &ndash; +0.50", "0.00", "Fine tuning in <i>cents</i> (hundredths of a semitone). Even a few hundredths of a cent against oscillator 1 produce a slow shimmering beat; ±0.5 cent is the entire range, so this is a very gentle control. For a wider detune use <b>Coarse</b> together with <b>Unison Detune</b>."),
        ("PWM", "0.00 &ndash; 0.50", "0.00", "Pulse width of the square: <code>0.50</code> = plain square, smaller = thinner."),
        ("Octave up", "Off / On", "On", "Plays oscillator 2 one octave above the note (in addition to the <b>Coarse</b> setting). On by default."),
    ])
    m.note("tip", "Total pitch of oscillator 2 = note + <b>Coarse</b> + <b>Fine</b> + one octave if <b>Octave up</b> is on. To tune oscillator 2 a fifth above oscillator 1 at the same octave, switch <b>Octave up</b> off and set <b>Coarse</b> to <code>7.0</code>.")
    m.tag("Oscillator 2", "Coarse tuning", "Fine tuning", "Cents")

    # ---- Modulation
    m.h(2, "mod", "Modulation (oscillator interaction)")
    m.crop("Modulation")
    m.p("This box makes oscillator 1 and oscillator 2 influence each other, which is where a lot of metallic, bell-like, hollow and aggressive timbres come from. Oscillator 1 is always the <b>modulator</b>; oscillator 2 is the <b>carrier</b> that is changed by it. You still hear oscillator 1 at its own level, so set oscillator 1's <b>Level</b> to <code>0.00</code> if you want to hear only the processed oscillator 2.")
    m.controls([
        ("Mode", "off, fm, am, ring, sync", "fm", "Chooses the kind of interaction (table below)."),
        ("Amount", "0.00 &ndash; 1.00", "0.00", "How strong the interaction is. At <code>0.00</code> the modes do nothing (oscillator 2 simply plays on its own), so the <b>Amount</b> must be above zero to hear a mode."),
    ])
    m.table(("Mode", "What happens", "How it sounds"), [
        ("off", "Oscillators are independent.", "Plain two-oscillator sound."),
        ("fm", "Oscillator 1 bends the phase of oscillator 2 (phase modulation). <b>Amount</b> sets the modulation index, from 0 up to 8.", "Small amounts add brightness and growl; larger amounts give bell, glassy or metallic, sometimes noisy tones. Strongly pitch-dependent."),
        ("am", "Oscillator 2's volume follows oscillator 1's wave.", "Tremolo that turns into rough, buzzy sidebands at audio rates."),
        ("ring", "Oscillator 2 is multiplied by oscillator 1, blended in by <b>Amount</b>.", "Clangorous, inharmonic bell and robotic sounds. Original pitches disappear at full amount."),
        ("sync", "Oscillator 2 is reset by oscillator 1 every cycle. <b>Amount</b> blends from free-running (<code>0.00</code>) to fully synced (<code>1.00</code>).", "The classic searing lead. Sweep <b>Coarse</b> on oscillator 2 (or modulate it with an LFO or envelope) to hear the sync sweep."),
    ], cls="compact", widths=("11%", "47%", "42%"))
    m.note("tip", "FM is very sensitive. Try <b>Amount</b> <code>0.10</code>&ndash;<code>0.30</code> first. Set oscillator 2 <b>Coarse</b> to whole-number semitones for harmonic tones and to odd values for clangy ones. With the mod matrix you can drive <b>Modulation Amount</b> from the mod wheel or an LFO for evolving timbres.")
    m.tag("FM", "AM", "Ring modulation", "Hard sync", "Modulation")

    # ---- Master
    m.h(2, "master", "Master")
    m.crop("Master")
    m.p("The <b>Master</b> box sets the overall volume and contains the output safeguards and the level meter.")
    m.controls([
        ("Volume", "0.00 &ndash; 1.20", "0.80", "Overall output gain, applied after the effects. Above <code>1.00</code> adds gain and pushes the soft clipper harder."),
        ("Velocity", "Off / On", "On", "On: harder key presses are louder (and, with <b>Vel&gt;Cut</b>, brighter). Off: every note plays at one fixed medium velocity (about 79 percent, MIDI velocity 100) no matter how you play."),
        ("Auto Limiter", "Off / On", "Off", "Turns the volume down automatically whenever the sound would clip. See below."),
        ("Level meter", "&minus;60 &ndash; 0 dBFS", "&ndash;", "Stereo output meter with a CLIP light."),
        ("GR readout", "&ndash;", "GR off", "Reserved for the limiter's gain-reduction display. In this version it is a fixed placeholder and does not yet show a value or respond to clicks."),
    ])
    m.h(3, "master-meter", "The level meter and the CLIP light")
    m.p("The two vertical bars show the left and right output level on a decibel scale from <b>&minus;60 dB</b> (bottom) to <b>0 dB</b> (top, full scale). A bar is <b>green</b> up to &minus;12 dB, <b>yellow</b> from &minus;12 to &minus;3 dB and <b>red</b> above &minus;3 dB. The bars rise instantly and fall at about 24 dB per second. A thin tick above each bar holds the highest recent peak for one second, then falls at about 12 dB per second.")
    m.p("The <b>CLIP</b> box above the bars lights red when the signal reaches full scale at the input of the soft clipper, which means the clipper is working hard and the sound is being distorted. It stays lit for two seconds after the last clip. Click it to clear it at once. If it lights often: lower <b>Volume</b>, play fewer notes, reduce the oscillator and effect levels, or switch on the <b>Auto Limiter</b>. The meter only watches the sound; it never changes it.")
    m.h(3, "master-limiter", "The auto limiter")
    m.p("With <b>Auto Limiter</b> on, the synth lowers its own volume when a peak would exceed a ceiling of 0.98 (about &minus;0.2 dBFS). It has an <b>instant attack</b> and, unusually, <b>no release</b>: the gain drops to exactly the amount the loudest peak needs and then <i>stays there</i>. Both channels use the same gain so the stereo image does not shift. The held gain reduction is cleared (the volume returns to normal) when:")
    m.ul(["the sound has been silent, below &minus;60 dBFS, for half a second without a break. Effect tails count as sound, so a decaying reverb tail never makes the volume jump up;",
          "you switch the limiter off and on again."])
    m.note("note", "With no reduction held, quiet sounds pass through unchanged. While a reduction is held, everything is lowered by it. With the limiter off the sound is exactly as it would be without the circuit.")
    m.note("warn", "The soft clipper at the very end of the chain is always on. It rounds peaks smoothly (like analogue saturation), so you will hear it as a gentle compression and warmth when the signal is hot, not as a hard digital crackle.")
    m.tag("Volume", "Velocity", "Auto limiter", "Level meter", "CLIP light", "Soft clipper", "Gain reduction")

    # ---- Tempo
    m.h(2, "tempo", "Tempo")
    m.crop("Tempo")
    m.p("The <b>Tempo</b> box holds one control, used for the tempo-synchronised delay (section [[ref:fx-delay]]).")
    m.controls([
        ("BPM", "40 &ndash; 240", "120", "The tempo, in beats per minute, that a synced delay follows when the host does <b>not</b> provide a tempo (for example in the standalone application). When you use the plug-in in a host that reports its tempo, the delay follows the <b>host tempo</b> instead and this knob is ignored."),
    ])
    m.tag("BPM", "Host tempo", "Tempo")

    # ---- Noise
    m.h(2, "noise", "Noise")
    m.crop("Noise")
    m.p("The noise generator mixes hiss into every voice. Because it sits <i>before</i> the filter and the amp envelope, it is shaped just like the oscillators: a short amp envelope turns it into percussive bursts, a filter envelope sweeps it, and with unison each voice has different noise.")
    m.controls([
        ("Level", "0.00 &ndash; 1.00", "0.00", "Volume of the noise. At <code>0.00</code> the generator is off and costs nothing. All three colours have the same loudness at the same level, so switching colour does not change the volume."),
        ("Color", "white, pink, brown", "white", "<b>white</b>: flat and bright, like a hiss or a cymbal. <b>pink</b>: softer, falling 3 dB per octave, like rain or wind. <b>brown</b>: dark, falling 6 dB per octave, like a distant rumble."),
    ])
    m.tag("Noise", "White noise", "Pink noise", "Brown noise")

    # ---- Filter
    m.h(2, "filter", "Filter")
    m.crop("Filter")
    m.p("The filter is the heart of the sound. It is a <b>low-pass</b> filter: it lets low frequencies through and removes the highs above its <b>cutoff</b>. Closing the filter makes the sound darker and rounder; opening it makes it brighter. <b>Resonance</b> boosts the frequencies around the cutoff, giving the filter its voice.")
    m.controls([
        ("Cutoff", "20 &ndash; 20000 Hz", "2000", "The frequency where the filter starts to cut. The control is logarithmic, so equal knob movements are equal musical intervals. Turned fully to the right (<code>20000</code>) the filter is <b>bypassed</b> altogether."),
        ("Resonance", "0.00 &ndash; 1.00", "0.00", "Emphasis at the cutoff. Low values add a slight edge; high values give a whistling, vowel-like peak. In 24 dB mode, values above 0.9 start a pure self-oscillating whistle (section [[ref:filter-whistle]])."),
        ("Slope", "12 dB, 24 dB", "12 dB", "The steepness of the filter. <b>12 dB</b>: the classic 2-pole filter, bright and open. <b>24 dB</b>: a Moog-style 4-pole ladder, steeper and darker at the same cutoff, with a warm, thick character; the bass thins as resonance rises, as on the real thing."),
        ("Master-bus filter", "Off / On", "Off", "Off: every voice has its own filter (the best sound; filter envelope, key tracking, velocity and the LFO act per note). On: one filter filters the whole mix (lighter on the CPU: use it if the audio glitches). With it on, the per-voice controls Env Amt, Key Trk and Vel&gt;Cut have no effect, and the 24 dB whistle is not produced."),
        ("Env Amt", "&minus;1.00 &ndash; +1.00", "0.00", "How far the <b>Filter Env</b> moves the cutoff: up to 6 octaves at <code>1.00</code>. Negative values close the filter when the envelope rises. <code>0.00</code> = envelope has no effect."),
        ("Key Trk", "0.00 &ndash; 1.00", "0.00", "<i>Key tracking</i>: how much the cutoff follows the pitch of the note. At <code>1.00</code> the cutoff doubles for every octave above middle C and halves for every octave below, so the brightness stays constant across the keyboard. At <code>0.00</code> every note uses the same cutoff, so high notes sound duller than low notes."),
        ("Vel&gt;Cut", "0.00 &ndash; 1.00", "0.00", "<i>Velocity to cutoff</i>: harder key presses open the filter further (up to 3 octaves up for the hardest, 3 down for the softest, at <code>1.00</code>). Gives expressive, piano-like response."),
    ])
    m.h(3, "filter-24", "12 dB and 24 dB compared")
    m.p("At the same cutoff the 24 dB filter sounds darker because its corner is lower, and it loses bass as you add resonance: the feedback that creates the peak reduces the low-frequency level by up to about 14 dB at full resonance. Raise the cutoff a little, or turn up the volume, to compensate. This is how a real Moog ladder filter behaves.")
    m.h(3, "filter-whistle", "Self-oscillation: the whistle")
    m.p("A real analogue ladder filter, with the resonance turned all the way up, starts to oscillate by itself and produces a clean sine wave at its cutoff frequency, even with no input signal. SnakeOil Synth reproduces this. In <b>24 dB</b> mode, with the per-voice filter, once <b>Resonance</b> goes above <code>0.90</code> a pure sine wave at the <i>effective</i> cutoff fades in. It reaches full strength at <code>1.00</code>. The whistle:")
    m.ul(["follows the cutoff <i>after</i> the filter envelope, key tracking, velocity, LFOs and mod matrix have all acted on it, so it can be swept and modulated;",
          "passes through the amp envelope, so it starts, sustains and releases with the note;",
          "needs no input at all: you can set both oscillator levels to <code>0.00</code> and still hear it;",
          "is absent in 12 dB mode, when the filter is bypassed (<b>Cutoff</b> at 20000), and when <b>Master-bus filter</b> is on."])
    m.h(3, "filter-tuned", "Playing the whistle in tune")
    m.p("Set <b>Key Trk</b> to <code>1.00</code> and <b>Cutoff</b> to <code>261.63</code> (the frequency of middle C) with <b>Env Amt</b>, <b>Vel&gt;Cut</b> and any LFO or matrix routing to the filter at zero. The whistle then plays <b>exactly the pitch of the note you press</b> on every key, and you can play it like a pure-tone instrument. To tune it to another octave or interval, multiply 261.63 by the interval's frequency ratio:")
    m.table(("Whistle plays", "Cutoff (Hz)", "Whistle plays", "Cutoff (Hz)"), [
        ("Same pitch as the note", "261.63", "Perfect fourth above", "349.23"),
        ("Octave below", "130.81", "Perfect fifth above", "392.00"),
        ("Octave above", "523.25", "Minor third above", "311.13"),
        ("Two octaves above", "1046.50", "Major third above", "329.63"),
    ], cls="compact", widths=("30%", "20%", "30%", "20%"))
    m.p("Key tracking below <code>1.00</code> makes the whistle drift flat of the note as you play higher (and sharp as you play lower); above <code>1.00</code> is not available. The whistle is tied to the note's own pitch, so <b>pitch bend</b>, oscillator 2 <b>Coarse/Fine</b> and unison <b>Detune</b> move the oscillators but not the whistle.")
    m.tag("Cutoff", "Resonance", "Low-pass filter", "Filter slope (12 dB, 24 dB)", "Self-oscillation", "Whistle", "Key tracking", "Velocity to cutoff", "Master-bus filter", "Moog ladder", "Filter")

    # ---- Filter env
    m.h(2, "fenv", "Filter Env")
    m.crop("Filter Env")
    m.p("A four-stage <b>ADSR</b> (attack, decay, sustain, release) envelope that moves the filter cutoff over the life of each note, by the amount set with <b>Env Amt</b> in the <b>Filter</b> box. With a positive amount: when you press a key the filter opens over the <b>Attack</b> time, falls over <b>Decay</b> to the <b>Sustain</b> level while you hold the key, and closes over <b>Release</b> when you let go. Nothing is heard from this envelope while <b>Env Amt</b> is <code>0.00</code>.")
    m.controls([
        ("Attack", "0.001 &ndash; 5.00 s", "0.005", "Time for the filter to open after the key goes down. Short = a snappy &ldquo;pluck&rdquo;; long = a slow swell."),
        ("Decay", "0.001 &ndash; 5.00 s", "0.30", "Time to fall from the peak to the sustain level."),
        ("Sustain", "0.00 &ndash; 1.00", "0.30", "The level (as a fraction of the peak) the filter settles on while the key is held."),
        ("Release", "0.001 &ndash; 10.00 s", "0.30", "Time for the filter to close after the key is released."),
    ])
    m.note("note", "Attack, Decay and Release are on a logarithmic scale: the first part of the knob covers short times in fine detail. The value box shows seconds. An envelope can also be changed while a note is sounding; the change is heard from the next audio block.")
    m.tag("ADSR", "Envelope", "Filter envelope")

    # ---- Amp env
    m.h(2, "aenv", "Amp Envelope")
    m.crop("Amp Envelope")
    m.p("The amplitude <b>ADSR</b> shapes the volume of every note. It has the same four stages as the filter envelope. Without it a note would start and stop abruptly. <b>Release</b> also controls how long a note rings after you release the key, and that is what the tail slots (section [[ref:voices-tails]]) are for.")
    m.controls([
        ("Attack", "0.001 &ndash; 5.00 s", "0.006", "Time for a note to rise to full volume. Short for plucks and keys; long for pads and swells."),
        ("Decay", "0.001 &ndash; 5.00 s", "0.12", "Time to fall from full volume to the sustain level."),
        ("Sustain", "0.00 &ndash; 1.00", "0.75", "The volume held while the key stays down. At <code>0.00</code> the note dies away by itself (percussive); at <code>1.00</code> there is no decay."),
        ("Release", "0.001 &ndash; 10.00 s", "0.18", "Time for a note to fade out after the key is released."),
    ])
    m.tag("Amp envelope", "Attack", "Decay", "Sustain", "Release")

    # ---- LFO
    m.h(2, "lfo", "LFO")
    m.crop("LFO")
    m.p("An <b>LFO</b> (low-frequency oscillator) is a slow oscillator, below the range of hearing, that moves another control up and down in a repeating pattern: vibrato (pitch), wah-wah or wobble (filter), pulse-width movement, or tremolo (volume). The box holds two independent LFOs, <b>LFO 1</b> on the left and <b>LFO 2</b> on the right, with identical controls. They are shared by all voices.")
    m.controls([
        ("Rate", "0.05 &ndash; 20.00 Hz", "5.0", "Speed in cycles per second."),
        ("Depth", "0.00 &ndash; 1.00", "0.00", "How strongly the LFO moves its destination. At <code>0.00</code> the LFO is off and does no work. (An LFO that is used as a source in the mod matrix keeps running even at depth 0, and then does not move its own destination.)"),
        ("Wave", "sine, triangle, saw, square, random, random-glide", "sine", "Shape of the movement. <b>sine</b>: smooth. <b>triangle</b>: linear up and down. <b>saw</b>: ramps up, then jumps back. <b>square</b>: jumps between two values. <b>random</b>: sample and hold, a new random value each cycle. <b>random-glide</b>: a new random target each cycle, reached by a smooth glide."),
        ("Dest", "LFO 1: pitch, filter, pwm, amp, lfo2-rate<br>LFO 2: pitch, filter, pwm, amp, lfo1-rate", "LFO 1: pitch<br>LFO 2: filter", "What the LFO moves (table below)."),
    ])
    m.table(("Destination", "What moves", "Amount at Depth 1.00"), [
        ("pitch", "The pitch of all oscillators (vibrato).", "&plusmn;2 semitones"),
        ("filter", "The filter cutoff (wah, wobble).", "&plusmn;3 octaves"),
        ("pwm", "The pulse width of both oscillators' square waves.", "&plusmn;0.25 (limited to the 0&ndash;0.50 range)"),
        ("amp", "The volume (tremolo).", "From full level down to silence"),
        ("lfo2-rate / lfo1-rate", "The <i>speed</i> of the other LFO, not the sound. The other LFO still drives its own destination as usual.", "&plusmn;2 octaves of speed (clamped to 0.01&ndash;40 Hz)"),
    ], cls="compact", widths=("21%", "52%", "27%"))
    m.p("If both LFOs aim at the same destination their movements add (for pitch, filter and pulse width) or multiply (for volume). The two LFOs can modulate each other's speed at once; this is stable and gives unpredictable, evolving movement. The pitch bend wheel is independent of the LFOs.")
    m.note("tip", "A classic wobble bass: <b>LFO 1</b> <b>Wave</b> triangle, <b>Rate</b> 2 Hz, <b>Depth</b> 0.7, <b>Dest</b> filter, with <b>Cutoff</b> at 600 Hz and <b>Resonance</b> 0.5.")
    m.note("note", "LFO values update once per audio block (a few milliseconds), which is smooth for musical rates and slightly stepped at extreme settings.")
    m.tag("LFO", "Vibrato", "Tremolo", "Wobble", "Random (sample and hold)", "Random-glide")

    # ---- Mod matrix
    m.h(2, "matrix", "Mod Matrix")
    m.crop("Mod Matrix")
    m.p("The <b>mod matrix</b> connects a <b>source</b> (something that moves) to a <b>destination</b> (a control to move) with a <b>scale</b> that says how much. It has eight rows, each with <b>Source</b>, <b>Scale</b> and <b>Destination</b>. Rows with a source or destination of <b>none</b>, or a scale of <code>0.00</code>, do nothing and cost nothing. The matrix starts empty.")
    m.controls([
        ("Source", "none, Note Number, LFO 1, LFO 2, Mod Wheel, Aftertouch", "none", "What drives the row (table in section [[ref:matrix-src]])."),
        ("Scale", "&minus;1.00 &ndash; +1.00 (&minus;100% &ndash; +100%)", "0.00", "How much of the source is applied, positive or negative. Double-click to reset to 0."),
        ("Destination", "none and 33 targets", "none", "What the row changes (section [[ref:matrix-dst]])."),
    ])
    m.h(3, "matrix-rule", "The scaling rule: relative modulation")
    m.p("The scale is <b>relative to the destination's current value</b>:")
    m.p("<b>effective value = current value &times; (1 + scale &times; source)</b>")
    m.p("and the result is held inside the destination's own range. Several rows on the same destination add their <code>scale &times; source</code> terms first. The knob always keeps the value you set; the matrix works on top of it.")
    m.p("<b>Example.</b> <b>Cutoff</b> is at 1000 Hz. A row <b>Mod Wheel</b> &rarr; <b>Filter: Cutoff</b> with a scale of <code>+0.50</code> gives 1000 Hz with the wheel down and 1500 Hz with the wheel fully up. With a scale of <code>&minus;0.50</code> it would close to 500 Hz instead.")
    m.note("warn", "Because the rule is relative, <b>a destination whose current value is 0 stays at 0</b>. Resonance at 0.00, Osc 2 Level at 0.00, PWM at 0.00, Env Amt at 0.00 or Reverb Amount at 0.00 cannot be moved by the matrix. Give the destination a small non-zero value first.")
    m.h(3, "matrix-src", "Sources")
    m.table(("Source", "Range", "Notes"), [
        ("none", "&ndash;", "The row is off."),
        ("Note Number", "&minus;1 to +1", "Pitch of the note: 0 at middle C (MIDI 60), reaching +1 five octaves above and &minus;1 five octaves below. It is evaluated per voice, so two notes held at once are modulated differently. For global destinations (effects, tempo) it means the last note played."),
        ("LFO 1, LFO 2", "&minus;1 to +1", "The raw LFO output, independent of that LFO's own <b>Depth</b> knob."),
        ("Mod Wheel", "0 to +1", "MIDI controller 1."),
        ("Aftertouch", "0 to +1", "Channel pressure (and polyphonic key pressure, treated as channel pressure)."),
    ], cls="compact", widths=("20%", "17%", "63%"))
    m.p("Wheel and aftertouch are smoothed from block to block so that changes do not click.")
    m.h(3, "matrix-dst", "Destinations")
    m.table(("Destination", "Range it is held in", "Type"), [
        ("Osc 1: Level", "0 &ndash; 1", "per voice"),
        ("Osc 1: PWM", "0 &ndash; 0.5", "per voice"),
        ("Osc 1: Sq Level", "0 &ndash; 1", "per voice"),
        ("Osc 2: Level", "0 &ndash; 1", "per voice"),
        ("Osc 2: Tune", "&minus;12 &ndash; +12 semitones", "per voice"),
        ("Osc 2: Fine", "&minus;0.5 &ndash; +0.5 cents", "per voice"),
        ("Osc 2: PWM", "0 &ndash; 0.5", "per voice"),
        ("Noise: Level", "0 &ndash; 1", "per voice"),
        ("Modulation Amount", "0 &ndash; 1", "per voice"),
        ("Tempo", "40 &ndash; 240 BPM", "global"),
        ("Filter: Cutoff", "20 &ndash; 20000 Hz", "per voice"),
        ("Filter: Resonance", "0 &ndash; 1", "per voice"),
        ("Filter: Env Amount", "&minus;1 &ndash; +1", "per voice"),
        ("Filter: Key Trk", "0 &ndash; 1", "per voice"),
        ("Filter: Vel&gt;Cut", "0 &ndash; 1", "per voice"),
        ("Filter Env: Attack / Decay / Sustain / Release", "as the knobs (times 1 ms &ndash; 5 s or 10 s; sustain 0 &ndash; 1)", "per voice"),
        ("Amp Env: Attack / Decay / Sustain / Release", "as the knobs", "per voice"),
        ("Chorus: Depth", "0 &ndash; 1", "global"),
        ("Delay: Time", "1 &ndash; 4000 ms (relative to the time in force: the synced time if Sync is on)", "global"),
        ("Delay: Feedback", "0 &ndash; 0.95", "global"),
        ("Delay: Tone", "0 &ndash; 0.9", "global"),
        ("Reverb: Amount", "0 &ndash; 1", "global"),
        ("Reverb: Size", "0.5 &ndash; 0.98", "global"),
        ("Reverb: Damping", "0 &ndash; 0.9", "global"),
        ("Bitcrush: Crush", "0 &ndash; 1", "global"),
        ("Unison: Detune", "0 &ndash; 50 cents", "per voice"),
        ("Unison: Spread", "0 &ndash; 1", "per voice"),
    ], cls="compact", widths=("38%", "47%", "15%"))
    m.p("<b>Per voice</b> destinations are evaluated separately for each note; <b>global</b> destinations (effects, tempo) are evaluated once per audio block and apply to the whole sound. The delay time moves smoothly, so a modulated delay glides in pitch instead of clicking.")
    m.note("tip", "Examples: <b>Mod Wheel</b> &rarr; <b>Filter: Cutoff</b> (+0.8) for a brightness wheel; <b>Aftertouch</b> &rarr; <b>Modulation Amount</b> to bring in FM with finger pressure; <b>Note Number</b> &rarr; <b>Amp Env: Decay</b> (&minus;0.5) so high notes decay faster than low notes; <b>LFO 2</b> &rarr; <b>Reverb: Size</b> for a breathing room.")
    m.tag("Mod matrix", "Modulation source", "Modulation destination", "Mod wheel", "Aftertouch", "Note number")

    # ---- Effects
    m.h(2, "fx", "Effects")
    m.crop("Effects")
    m.p("The <b>Effects</b> box is a stereo effects chain on the whole mix. Each of the four effects has its own on/off button at the head of its block, and its controls underneath. An effect that is off is not processed and costs nothing; its controls keep their values. The effects run in this order, regardless of how they appear on screen: <b>chorus &rarr; delay &rarr; reverb &rarr; bitcrush</b>.")
    m.h(3, "fx-chorus", "Chorus")
    m.p("The chorus thickens the sound by mixing it with slightly delayed, slowly moving copies of itself. It is a stereo chorus: the left and right channels are modulated in opposite directions, which widens the image.")
    m.controls([("Chorus (button)", "Off / On", "Off", "Switches the chorus."),
                ("Depth", "0.00 &ndash; 1.00", "0.30", "How far the delay swings. Low values give a subtle thickening; high values give an obvious seasick wobble.")])
    m.h(3, "fx-delay", "Delay")
    m.p("A stereo echo. The delay repeats the sound after a set time and feeds a part of each repeat back into the delay, so the echoes trail away.")
    m.controls([
        ("Delay (button)", "Off / On", "Off", "Switches the delay."),
        ("Time", "200 &ndash; 4000 ms", "300", "Time between echoes (logarithmic knob). Not used while <b>Sync</b> is on."),
        ("Ping-pong", "Off / On", "Off", "Bounces the echoes between the left and right speakers."),
        ("Feedback", "0.00 &ndash; 0.95", "0.35", "How much of each echo is fed back. Low = one or two repeats; high = long trailing repeats. Values near the top can build up and ring for a long time."),
        ("Tone", "0.00 &ndash; 0.90", "0.25", "Darkness of the echoes: higher values filter more highs from each repeat so echoes fade into the background."),
        ("Sync", "Off / On", "Off", "Locks the delay time to the tempo: the host tempo in the plug-in, the <b>BPM</b> in the <b>Tempo</b> box when no host tempo is available."),
        ("Division", "1/1, 1/2, 1/2., 1/4, 1/4., 1/4T, 1/8, 1/8., 1/8T, 1/16, 1/16.", "1/8", "The length of one echo when synced, as a note value. A dot (.) means dotted (1.5 times as long); T means triplet (two thirds as long)."),
    ])
    m.table(("Division", "Beats", "Division", "Beats"), [
        ("1/1 (whole note)", "4", "1/4T (quarter triplet)", "2/3"),
        ("1/2", "2", "1/8", "1/2"),
        ("1/2. (dotted half)", "3", "1/8. (dotted eighth)", "3/4"),
        ("1/4", "1", "1/8T (eighth triplet)", "1/3"),
        ("1/4. (dotted quarter)", "3/2", "1/16", "1/4"),
        ("", "", "1/16. (dotted sixteenth)", "3/8"),
    ], cls="compact", widths=("30%", "20%", "30%", "20%"))
    m.p("With <b>Sync</b> on, the echo time is 60000 &divide; BPM &times; beats, in milliseconds, held inside the delay's range of 1&ndash;4000 ms (so a whole-note delay below 60 BPM stops at 4000 ms). The <b>Time</b> knob keeps its own value for when you switch <b>Sync</b> off.")
    m.h(3, "fx-reverb", "Reverb")
    m.p("A stereo reverb that simulates the sound of a room. The left and right channels are processed slightly differently for a wide, natural space.")
    m.controls([("Reverb (button)", "Off / On", "Off", "Switches the reverb."),
                ("Amount", "0.00 &ndash; 1.00", "0.30", "How much reverb is mixed in (the wet level)."),
                ("Size", "0.50 &ndash; 0.98", "0.84", "Room size: how long the reverb tail rings. Near the top the tail rings for a long time."),
                ("Damping", "0.00 &ndash; 0.90", "0.25", "How quickly the highs die away in the tail. Higher = darker, softer tail.")])
    m.h(3, "fx-crush", "Bitcrush")
    m.p("A lo-fi effect. It reduces the number of bits used to represent the sound and the number of samples per second, which adds grit, noise and aliasing.")
    m.controls([("Bitcrush (button)", "Off / On", "Off", "Switches the effect."),
                ("Crush", "0.00 &ndash; 1.00", "0.50", "One control for both reductions together: bit depth falls from 16 bits (<code>0.00</code>) to 8 bits (<code>0.50</code>) to 2 bits (<code>1.00</code>), while the sample rate falls to 1/4 at <code>0.50</code> and 1/7 at <code>1.00</code>.")])
    m.tag("Effects", "Chorus", "Delay", "Echo", "Ping-pong", "Tempo sync", "Reverb", "Bitcrush", "Dotted and triplet")

    # ---- Unison
    m.h(2, "unison", "Unison")
    m.crop("Unison")
    m.p("<b>Unison</b> stacks several slightly detuned voices on every key you press, the classic way to make a sound big, wide and &ldquo;supersaw&rdquo;-like.")
    m.controls([
        ("Voices", "1 &ndash; 12", "1", "Voices stacked per note. <code>1</code> = unison off. Each unison voice uses one voice from the pool, so the number of notes you can hold at the same time falls to 12 divided by this number (3 voices = 4 notes, 4 voices = 3 notes, 12 voices = 1 note)."),
        ("Detune", "0 &ndash; 50 cents", "15", "Pitch spread of the outermost voices, in cents. The other voices spread evenly between them. Small values (5&ndash;15) thicken; large values sound out of tune and shimmering."),
        ("Spread", "0.00 &ndash; 1.00", "0.50", "Stereo width of the stack. <code>0.00</code>: all voices in the centre. <code>1.00</code>: the outer voices are placed far left and far right."),
        ("Tails n/n", "12 or 24", "12", "Shows the tail slots (section [[ref:voices-tails]]). Click to switch between 12 and 24."),
    ])
    m.p("Each voice in the stack is scaled by 1/&radic;(number of voices), so the loudness stays about the same as you add voices. Each voice starts with a random phase from a fixed sequence, so stacked voices do not cancel or beat the same way every time, yet a given sequence of notes always sounds identical.")
    m.note("note", "Unison <b>Detune</b> and <b>Spread</b> also work on top of the other settings. A value of 0 stays at 0 under the mod matrix's relative rule.")
    m.tag("Unison", "Supersaw", "Detune", "Spread", "Stereo width")

    # ---- Glide
    m.h(2, "glide", "Glide")
    m.crop("Glide")
    m.p("Glide (also called portamento) slides smoothly from the pitch of the previous note to the pitch of the new one.")
    m.controls([
        ("Time", "0.00 &ndash; 2.00 s", "0.00", "How long the slide takes. <code>0.00</code> = glide off. The slide is a straight line in semitones, so it sounds even across any interval."),
        ("Legato only", "Off / On", "Off", "When on, the glide only happens if you are still holding another key when you press the next one (playing legato). Detached notes then start at their own pitch."),
    ])
    m.tag("Glide", "Portamento", "Legato")

    # ------------------------------------------------------------------ 7
    m.h(1, "voices", "Voices, polyphony and tails")
    m.h(2, "voices-poly", "Polyphony")
    m.p("SnakeOil Synth is <b>polyphonic</b>: it plays several notes at once. It has <b>12 playable voices</b>. One voice is used per held key, or several per key with unison (section [[ref:unison]]). When you play a thirteenth note while twelve voices are held, the oldest held note is given a very short fade (10 milliseconds, too fast to hear as a gap) and the new note takes its place, so you do not get a click.")
    m.h(2, "voices-tails", "Tail slots")
    m.p("When you release a key the note does not stop at once: it rings out over the <b>Release</b> time of the amp envelope. A synthesizer that gives the released note's voice to the next note at that moment would cut the tail off with an audible click or &ldquo;pop&rdquo;. To avoid this SnakeOil Synth keeps extra voices, called <b>tail slots</b>, for released notes only. The notes you are holding use the 12 playable voices; the notes you have let go of ring out in the tail slots.")
    m.ul(["The label under the <b>Unison</b> controls reads <b>Tails n/m</b>: <i>n</i> released notes are still ringing, out of <i>m</i> slots in use. It turns amber when all slots are full.",
          "Click the label to switch <i>m</i> between <b>12</b> (the default) and <b>24</b> slots. More slots allow longer, denser releases (pads, reverb-like tails, fast playing with a long <b>Release</b>) without notes being cut, at the price of more processing while those tails ring.",
          "Slots are shared with unused playable voices: if you hold fewer than 12 voices, more released notes than slots can ring. Only when the whole pool is full does the quietest ringing tail get taken over, and it is taken over smoothly: it keeps its phase, filter state and envelope level, so there is no click.",
          "The setting is saved with your project (it is a mode of the synth and cannot be automated)."])
    m.note("tip", "Heavy patches (unison, 24 dB filter, all effects, a long release) cost the most processing. If your host's meter shows the CPU near its limit or you hear crackles, set the tail slots to 12, shorten the <b>Release</b>, narrow the unison, or switch on <b>Master-bus filter</b>.")
    m.note("note", "The Python reference program uses 6 playable tail slots and offers 6/12 as its toggle. The plug-in doubles that to 12/24 because the C++ engine has much more headroom.")
    m.tag("Polyphony", "Tail slots", "Voice stealing", "Release tails")

    # ------------------------------------------------------------------ 8
    m.h(1, "midi", "MIDI implementation")
    m.p("SnakeOil Synth listens to all 16 MIDI channels at once (omni) and does not send MIDI. Notes arrive in the standard way; the following messages are understood:")
    m.table(("Message", "Effect"), [
        ("Note on / note off", "Plays and releases notes (MIDI notes 0&ndash;127, concert pitch A4 = 440 Hz). Velocity 1&ndash;127 sets loudness (and, with <b>Vel&gt;Cut</b>, brightness) when <b>Velocity</b> is on; with it off every note plays at velocity 100."),
        ("Pitch bend", "Bends all pitches by up to &plusmn;2 semitones. The range is fixed."),
        ("Controller 1 (mod wheel)", "Available as the <b>Mod Wheel</b> source in the mod matrix. It does nothing by itself; route it with a matrix row."),
        ("Controller 64 (sustain pedal)", "Values 64 and above hold released notes until the pedal is lifted; below 64 releases them."),
        ("Controller 121 (reset all controllers)", "Resets pitch bend, mod wheel and aftertouch to zero and lifts the sustain pedal."),
        ("Controller 123 (all notes off)", "Releases every note (the &ldquo;panic&rdquo; message)."),
        ("Channel pressure / polyphonic aftertouch", "Available as the <b>Aftertouch</b> source in the mod matrix (polyphonic pressure is treated as channel pressure)."),
        ("Other controllers, program change, MIDI clock", "Ignored. Use your host to map controllers to knobs (the knobs are ordinary automatable parameters). Tempo comes from the host."),
    ], cls="compact", widths=("33%", "67%"))
    m.tag("MIDI", "Pitch bend", "Sustain pedal", "All notes off", "Mod wheel")

    # ------------------------------------------------------------------ 9
    m.h(1, "recipes", "Sound design recipes")
    m.p("These are starting points. Start from the factory setting (every control at its default) for each recipe, change only the controls listed, and then adjust by ear. Controls not listed stay at their defaults. Values are what you see in the value boxes.")
    m.h(2, "rec-whistle", "A pure self-oscillating whistle, in tune")
    m.table(("Box", "Setting"), [
        ("Oscillator 1", "<b>Level</b> 0.00 (so only the whistle is heard)"),
        ("Filter", "<b>Slope</b> 24 dB; <b>Resonance</b> 1.00; <b>Cutoff</b> 261.63; <b>Key Trk</b> 1.00; <b>Env Amt</b> 0.00; <b>Vel&gt;Cut</b> 0.00"),
        ("Amp Envelope", "<b>Attack</b> 0.02; <b>Release</b> 0.40"),
        ("Effects", "<b>Reverb</b> on, <b>Amount</b> 0.30"),
    ], cls="compact", widths=("22%", "78%"))
    m.p("Play single notes: a clean sine at exactly the pitch you press. Add a little <b>Level</b> to oscillator 1 and sweep <b>Cutoff</b> to bring the saw back in underneath the whistle. Use an LFO to <b>pitch</b> for vibrato; see section [[ref:filter-tuned]] for tuning to other intervals.")
    m.h(2, "rec-pad", "Wide unison pad")
    m.table(("Box", "Setting"), [
        ("Oscillator 2", "<b>Level</b> 0.50; <b>Octave up</b> off; <b>Fine</b> 0.30"),
        ("Filter", "<b>Slope</b> 24 dB; <b>Cutoff</b> 1800; <b>Key Trk</b> 0.50"),
        ("Amp Envelope", "<b>Attack</b> 0.80; <b>Sustain</b> 0.80; <b>Release</b> 1.50"),
        ("Unison", "<b>Voices</b> 3; <b>Detune</b> 18; <b>Spread</b> 0.80"),
        ("Effects", "<b>Chorus</b> on (<b>Depth</b> 0.30); <b>Reverb</b> on (<b>Amount</b> 0.35, <b>Size</b> 0.90)"),
    ], cls="compact", widths=("22%", "78%"))
    m.p("Three unison voices still let you hold four notes at once. Use the 24-slot tail setting when you play with the sustain pedal.")
    m.h(2, "rec-bass", "Plucky bass")
    m.table(("Box", "Setting"), [
        ("Oscillator 1", "<b>Octave down</b> on; <b>Sq Level</b> 0.30; <b>PWM</b> 0.25"),
        ("Filter", "<b>Slope</b> 24 dB; <b>Cutoff</b> 400; <b>Resonance</b> 0.30; <b>Env Amt</b> 0.60"),
        ("Filter Env", "<b>Attack</b> 0.001; <b>Decay</b> 0.25; <b>Sustain</b> 0.00"),
        ("Amp Envelope", "<b>Attack</b> 0.001; <b>Decay</b> 0.30; <b>Sustain</b> 0.20; <b>Release</b> 0.15"),
    ], cls="compact", widths=("22%", "78%"))
    m.h(2, "rec-wobble", "Wobble bass")
    m.table(("Box", "Setting"), [
        ("Oscillator 1", "<b>Octave down</b> on"),
        ("Filter", "<b>Slope</b> 24 dB; <b>Cutoff</b> 600; <b>Resonance</b> 0.50"),
        ("LFO 1", "<b>Rate</b> 2.0; <b>Depth</b> 0.70; <b>Wave</b> triangle; <b>Dest</b> filter"),
        ("Amp Envelope", "<b>Sustain</b> 1.00; <b>Release</b> 0.15"),
    ], cls="compact", widths=("22%", "78%"))
    m.p("Raise <b>Rate</b> for faster wobble. Set the delay to <b>Sync</b> and the same tempo for a groove that locks to your song.")
    m.h(2, "rec-lead", "Mod-wheel lead")
    m.table(("Box", "Setting"), [
        ("Oscillator 1", "<b>PWM</b> 0.30"),
        ("Filter", "<b>Slope</b> 12 dB; <b>Cutoff</b> 1000; <b>Resonance</b> 0.30"),
        ("Mod Matrix, row 1", "<b>Source</b> Mod Wheel; <b>Scale</b> +0.80; <b>Destination</b> Filter: Cutoff"),
        ("Mod Matrix, row 2", "<b>Source</b> Aftertouch; <b>Scale</b> +0.50; <b>Destination</b> Osc 1: PWM"),
        ("Glide", "<b>Time</b> 0.08; <b>Legato only</b> on"),
    ], cls="compact", widths=("22%", "78%"))
    m.p("With the wheel down the lead is dark; pushing the wheel up opens the filter. Legato playing slides between notes.")
    m.h(2, "rec-hat", "Noise hi-hat")
    m.table(("Box", "Setting"), [
        ("Oscillator 1", "<b>Level</b> 0.00"),
        ("Noise", "<b>Level</b> 0.60; <b>Color</b> white"),
        ("Filter", "<b>Cutoff</b> 3000; <b>Resonance</b> 0.20"),
        ("Amp Envelope", "<b>Attack</b> 0.001; <b>Decay</b> 0.12; <b>Sustain</b> 0.00; <b>Release</b> 0.10"),
    ], cls="compact", widths=("22%", "78%"))
    m.p("Shorten <b>Decay</b> for a closed hat, lengthen it for an open hat. Pink or brown noise with a lower cutoff gives snare-like thuds.")
    m.h(2, "rec-echo", "Tempo-synced stereo echo")
    m.table(("Box", "Setting"), [
        ("Effects: Delay", "<b>Delay</b> on; <b>Sync</b> on; <b>Division</b> 1/8.; <b>Feedback</b> 0.40; <b>Tone</b> 0.30; <b>Ping-pong</b> on"),
        ("Effects: Reverb", "<b>Reverb</b> on; <b>Amount</b> 0.20"),
    ], cls="compact", widths=("22%", "78%"))
    m.p("The dotted-eighth echo against a straight beat is a classic rhythmic effect. The echoes follow the tempo of your host project.")
    m.h(2, "rec-lofi", "Lo-fi grit")
    m.p("Switch on <b>Bitcrush</b> with <b>Crush</b> at <code>0.50</code> (8 bits, a quarter of the sample rate). Raise <b>Crush</b> for more destruction, lower it for a subtle dirtiness. Put <b>Reverb</b> on afterwards: the effects run chorus, delay, reverb, bitcrush, so the crushed signal includes the reverb tail.")
    m.tag("Recipes")

    # ------------------------------------------------------------------ 10
    m.h(1, "trouble", "Troubleshooting")
    m.table(("Symptom", "Likely cause and remedy"), [
        ("No sound at all", "Check that the track is armed and its input is set to your MIDI device; that the instrument is not muted; and that <b>Volume</b> in <b>Master</b> is above 0 and oscillator 1's <b>Level</b> above 0. In the standalone application, open <b>Options</b> and confirm the audio output and MIDI input devices. Press a key and watch the meter: if it moves, the synth is making sound and the problem is downstream."),
        ("The sound is very quiet", "Oscillator 1 at <code>1.00</code> and <b>Volume</b> at <code>0.80</code> is moderate by design. Raise the track volume or <b>Volume</b> (up to 1.20). Heavy filtering (24 dB, low cutoff, high resonance) removes a lot of level: raise <b>Cutoff</b>."),
        ("The filter does nothing, or it is stuck open", "A <b>Cutoff</b> of 20000 Hz (fully right) bypasses the filter. Lower it. If an LFO or the mod matrix pushes the cutoff above 20 kHz, the filter is bypassed for that moment."),
        ("The whistle does not appear", "It needs <b>Slope</b> 24 dB, <b>Resonance</b> above 0.90, the per-voice filter (<b>Master-bus filter</b> off) and a cutoff below 20000 Hz. See section [[ref:filter-whistle]]."),
        ("The whistle is out of tune", "Set <b>Key Trk</b> to <code>1.00</code> and <b>Cutoff</b> to <code>261.63</code>, with <b>Env Amt</b> and <b>Vel&gt;Cut</b> at 0 and no LFO or matrix row on the filter. See section [[ref:filter-tuned]]."),
        ("Oscillator 2 is not heard, or FM/AM/ring does nothing", "Oscillator 2 <b>Level</b> is 0.00 by default, and the modulation <b>Amount</b> must be above 0.00. In <b>ring</b> and <b>am</b> modes oscillator 2 must be audible."),
        ("The mod matrix row has no effect", "The destination's current value may be 0, which the relative rule keeps at 0 (section [[ref:matrix-rule]]). Also check that both <b>Source</b> and <b>Destination</b> are not <b>none</b> and that <b>Scale</b> is not 0.00. Mod wheel and aftertouch sources are zero until you move the wheel or press the keys."),
        ("Notes get cut off when I play chords", "Only 12 voices are playable, shared with unison: with <b>Voices</b> 4 you can hold only 3 notes. Lower the unison, and keep <b>Tails</b> at 24 for long releases."),
        ("Crackles, clicks or dropouts", "The CPU may be overloaded. Increase the audio buffer size, set the tail slots to 12, shorten <b>Release</b>, narrow unison, switch on <b>Master-bus filter</b>, switch off effects you do not need. A high <b>Feedback</b> in the delay or a long reverb also keeps the voices and effects busy."),
        ("The sound leans to the right", "Chorus and reverb are stereo effects whose channels differ slightly by design; with unison the voices are panned by <b>Spread</b>. Set <b>Spread</b> to 0 and switch off chorus and reverb to hear the centred sound. A lopsided output with these off is a bug: please report it with your settings."),
        ("CLIP lights up", "The output reaches full scale. Lower <b>Volume</b>, play fewer voices, or switch on <b>Auto Limiter</b>. A little clipping is smoothed by the soft clipper, but constant clipping sounds harsh."),
        ("The whole sound got quieter and does not recover (auto limiter on)", "The auto limiter holds its gain reduction until half a second of silence, or until you switch it off and on. This is by design."),
        ("A knob or box is cut off, or I cannot see the whole window", "Drag the bottom-right corner to resize the window, or scroll with the mouse wheel or the scroll bar. The window is laid out for about 1,470 by 810 pixels."),
        ("The mouse wheel does not turn the knobs", "By design: the wheel scrolls the window. Drag the knob or type a number in the value box instead."),
        ("The plug-in does not show up in my host", "Make sure the whole <code>SnakeOil Synth.vst3</code> folder is inside a folder that your host scans (section [[ref:install-vst3]]) and run a rescan. In Sonar look under <b>Synth</b>, as an <b>Instrument</b>. If it was scanned before an update, run a forced rescan."),
        ("Parameter names in my host look duplicated", "In this version the host sees the short on-screen names, so several parameters are named <b>Level</b>, <b>Attack</b>, <b>Depth</b> and so on. The host lists them in the order of Appendix A (Oscillator 1 first, Mod Matrix last), which helps you tell them apart."),
        ("The GR readout never changes", "In this version <b>GR off</b> is a fixed placeholder (section [[ref:master]]). The limiter itself works; watch the meter."),
    ], cls="compact", widths=("27%", "73%"))
    m.tag("Troubleshooting", "Crackles", "CPU")

    # ------------------------------------------------------------------ 11
    m.h(1, "specs", "Specifications")
    m.table(("Item", "Value"), [
        ("Product", "SnakeOil Synth 0.1.0 (pre-release)"),
        ("Formats", "VST3 (64-bit), standalone application"),
        ("Platform", "Windows 10 / 11, 64-bit"),
        ("Plug-in category", "Instrument / Synth (VST3)"),
        ("Audio output", "Stereo (a mono output bus is also accepted and receives the left channel)"),
        ("MIDI input", "One input, all 16 channels (omni)"),
        ("Playable voices", "12 (unison voices share them)"),
        ("Tail slots", "12 or 24, selectable"),
        ("Oscillators", "Oscillator 1: band-limited saw plus optional pulse layer, octave switch. Oscillator 2: band-limited pulse, octave switch, coarse &plusmn;12 st, fine &plusmn;0.5 cent"),
        ("Modulation modes", "off, FM (index 0&ndash;8), AM, ring, hard sync"),
        ("Noise", "White, pink (&minus;3 dB/octave), brown (&minus;6 dB/octave)"),
        ("Filter", "Low-pass, 20&ndash;20000 Hz, resonant, 12 dB/octave or Moog-style 24 dB/octave ladder with self-oscillation whistle, per voice or master bus"),
        ("Envelopes", "Two ADSR: amp and filter. Times 1 ms &ndash; 5 s (attack, decay) and 1 ms &ndash; 10 s (release)"),
        ("LFOs", "Two, 0.05&ndash;20 Hz, six waves, five destinations each"),
        ("Mod matrix", "8 rows; 6 sources; 33 destinations"),
        ("Unison", "1&ndash;12 voices, detune 0&ndash;50 cents, stereo spread"),
        ("Glide", "0&ndash;2 s, optional legato-only"),
        ("Effects", "Chorus, delay (200&ndash;4000 ms, ping-pong, tempo sync), reverb, bitcrush; all stereo"),
        ("Output stage", "Master volume 0&ndash;1.2, optional auto limiter (ceiling 0.98), soft clipper, stereo peak meter"),
        ("Pitch bend range", "&plusmn;2 semitones (fixed)"),
        ("Parameters", "85, all automatable"),
        ("Internal precision", "64-bit floating point"),
        ("Latency", "The plug-in adds no latency of its own (it reports zero latency)"),
        ("Tail length reported to host", "0 (the plug-in does not ask the host to keep it running after the music stops)"),
    ], cls="compact", widths=("28%", "72%"))
    m.note("note", "Because the plug-in does not report a tail length, some hosts stop processing the track a moment after the last note ends. Long reverb or delay tails on a track that has no more notes may then be cut off in a few hosts; keep the track running (for example with a long empty MIDI item) if that happens.")

    # ------------------------------------------------------------------ appendices
    m.h(1, "params", "Appendix A: all parameters")
    m.p("Every parameter your host can automate, in window order. <b>Name</b> is the on-screen label; the host shows the same short name. Ranges and defaults are those of the value box; logarithmic controls move in even musical steps.")
    import json
    from pathlib import Path
    data = json.load(open(Path(__file__).resolve().parents[2] / "plugin" / "params.json"))["params"]
    rows = []
    for p in data:
        if p["group"] == "Mod Matrix":
            continue
        if p["kind"] == "toggle":
            rng, dflt = "Off / On", "On" if p["default"] else "Off"
        elif p["kind"] == "choice":
            rng = ", ".join(p["choices"])
            dflt = str(p["default"])
        else:
            lo, hi = p["minimum"], p["maximum"]
            rng = "%g &ndash; %g%s" % (lo, hi, " (log)" if p["scale"] == "log" else "")
            dflt = "%g" % p["default"]
        rows.append((p["group"], p["label"], rng.replace("-", "&minus;") if False else rng, dflt))
    m.table(("Box", "Name", "Range", "Default"), rows, cls="compact", widths=("19%", "18%", "50%", "13%"))
    m.p("The <b>Mod Matrix</b> contributes 24 more parameters: <code>Source</code>, <code>Scale</code> and <code>Destination</code> for each of the eight rows. Source and Destination are lists (sections [[ref:matrix-src]] and [[ref:matrix-dst]]); Scale ranges from &minus;1 to +1 with a default of 0.")

    m.h(1, "freq", "Appendix B: note frequencies")
    m.p("Equal temperament, A4 = 440 Hz. Useful for choosing a filter <b>Cutoff</b> that matches a note when <b>Key Trk</b> is at 0 (and as a reference for the whistle of section [[ref:filter-tuned]]).")
    names = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
    rows = []
    for octave in range(1, 7):
        row = ["<b>Octave %d</b>" % octave]
        for n in names:
            midi = 12 * (octave + 1) + names.index(n)
            f = 440.0 * 2 ** ((midi - 69) / 12.0)
            row.append("%.2f" % f)
        rows.append(row)
    m.table(["&nbsp;"] + names, rows, cls="compact", widths=["12%"] + ["7.3%"] * 12)
    m.p("Middle C (MIDI note 60) is C4 = 261.63 Hz.")

    m.h(1, "glossary", "Glossary")
    terms = [
        ("ADSR", "Attack, Decay, Sustain, Release: the four stages of an envelope."),
        ("Aftertouch", "Pressure applied to a key after it is down. Used as a modulation source."),
        ("Aliasing", "Inharmonic, metallic distortion produced when a digital oscillator makes harmonics above half the sample rate. SnakeOil Synth's oscillators are built to avoid it."),
        ("AM (amplitude modulation)", "One signal changes the volume of another."),
        ("Automation", "Recorded movement of a control by the host over time."),
        ("Bitcrusher", "An effect that lowers the bit depth and the sample rate of the sound."),
        ("Carrier / modulator", "In modulation, the carrier is the signal that is changed and the modulator is the signal that changes it."),
        ("Cent", "One hundredth of a semitone."),
        ("Cutoff", "The frequency above which a low-pass filter starts to remove sound."),
        ("DAW (host)", "A digital audio workstation: the program, such as REAPER or Sonar, that runs the plug-in."),
        ("dBFS", "Decibels relative to full scale; 0 dBFS is the loudest level a digital signal can have."),
        ("Detune", "A small difference in pitch between voices that makes the sound thicker."),
        ("Envelope", "A shape that changes a control over the life of a note."),
        ("FM (frequency/phase modulation)", "One oscillator changes the pitch (phase) of another very quickly, creating new overtones."),
        ("Feedback", "Sending part of a signal's output back to its input."),
        ("Glide (portamento)", "A smooth slide between the pitches of successive notes."),
        ("Hard sync", "One oscillator restarts another's cycle each time it completes its own."),
        ("Key tracking", "Making the filter cutoff follow the pitch of the key."),
        ("LFO", "Low-frequency oscillator, used to move controls slowly."),
        ("Limiter", "A circuit that keeps peaks below a ceiling."),
        ("Low-pass filter", "A filter that passes low frequencies and cuts high ones."),
        ("MIDI", "The standard protocol for sending notes and controller messages between instruments and computers."),
        ("Mod matrix", "A table of connections between modulation sources and destinations."),
        ("Octave", "A doubling of frequency; twelve semitones."),
        ("Ping-pong", "A stereo delay whose echoes alternate between the left and right channels."),
        ("Pitch bend", "A MIDI wheel or lever that bends the pitch of all notes."),
        ("Polyphony", "The ability to play several notes at the same time."),
        ("Pulse width (PWM)", "The proportion of each cycle a pulse wave spends high. Pulse-width modulation changes it over time."),
        ("Resonance", "A boost of frequencies at the filter cutoff."),
        ("Ring modulation", "Multiplying two signals, which produces the sum and difference of their frequencies."),
        ("Sample and hold", "Taking a value at intervals and holding it until the next one: the LFO &ldquo;random&rdquo; wave."),
        ("Self-oscillation", "A filter with enough resonance feedback produces a sound of its own."),
        ("Semitone", "One twelfth of an octave; the distance between neighbouring piano keys."),
        ("Soft clipper", "A circuit that rounds off peaks smoothly instead of cutting them flat."),
        ("Tail slot", "A spare voice kept for a released note while its release rings out."),
        ("Unison", "Several detuned voices playing the same note."),
        ("Velocity", "How hard a key is struck, 1 to 127."),
        ("VST3", "A plug-in format created by Steinberg."),
        ("Voice", "One note's worth of oscillators, filter and envelopes."),
    ]
    m.table(("Term", "Meaning"), terms, cls="compact", widths=("27%", "73%"))
