# matrix-sequencer

Initial simple implementation of a 3 track programmable pattern sequencer using the [SYINSI Euclidean Sequencer hardware](http://syinsi.com/shop/modules/euclidean-built/) and inspired by Tom Whitwell's Euclidean Sequencer as described [on MuffWiggler](https://www.muffwiggler.com/forum/viewtopic.php?t=45485&start=all&postdays=0&postorder=asc)

## Controls
+ 1/Length
  + Rotate - Change length 1 - 16 steps
  + Click - Cycle through Play Modes, currently Forwards, Backwards, Random, Pendulum
  + Hold (~2s) - Make Track 1 the active editing track
+ 2/Density
  + Rotate - Move the edit cursor
  + Click - Invert the state of the step under the cursor (on/off)
  + Hold (~2s) - Make Track 2 the active editing track
+ 3/Offset
  + Rotate - Offset in the steps in the direction turned. Note - steps outside the current length remain in place.
  + Click - Cycle through Output Modes, currently Trigger (~10ms pulse), Clock (match clock width), Gate
  + Hold (~2s) - Make Track 3 the active editing track

## Outputs
+ 1 - As programmed on Track 1
+ 2 - As programmed on Track 2
+ 3 - As programmed on Track 3
+ Offbeat - Inverse of Track 1

## Guidance
This is a work in progress!
+ display reverts to a play view after ~5 seconds of not twiddling knobs
+ Saving of changes (if there are any) occurs when you switch edit mode or when the display reverts to the play view
+ sync resets on the rising edge

## The Future
If I can work out how to set up the controls so additional editing modes can be accessed:
+ Configurable sync behaviour; rising or falling edge, reset and hold while high.
+ More Play Modes; pendulum and variations on, random direction and random output as well as step.
+ Set a clock divider for a track
+ Add the Euclidean functionality so that a track can be Euclidean or programmed
+ I'm open to ideas
