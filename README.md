# microsynth

C++-based software for a Raspberry Pi 4-based tiny synthesizer.

# Background Research

## How do mechanical keyboard switches work?

![][https://raw.githubusercontent.com/penguinencounter/pltwfinal/refs/heads/main/media/keysw-diag.png]

When the key is pressed down, a small, flexible contact completes a circuit through the two wires of the key. For our project, we can use the fact that they function as simple switches to wire them directly to the GPIO ports on the Raspberry PI to allow for user input on the keyboard.

The other part of the key which is important to us is the type of keyswitch. There are three main ones: linear, tactile, or clicky. The clicking sound created by “clicky” switches is from an extra piece which “clicks” out of the way when the key is pressed making the sound and giving the tactile feedback. “Tactile” switches give a bump when pressed without a click sound. This is done with the profile of the part inside the switch which makes that bump when it pushes against a flexible part inside the switch. For our purposes, “linear” switches will be best since they have no tactile feedback and simply press smoothly in. This will not make the keyboard feel exactly like a piano or typical MIDI controller, however it will get us close while simplifying our CAD design.

