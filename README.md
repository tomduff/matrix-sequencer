# matrix-sequencer

Implementation of a 3 track programmable or Euclidean pattern sequencer using the [SYINSI Euclidean Sequencer hardware](http://syinsi.com/shop/modules/euclidean-built/) and inspired by Tom Whitwell's Euclidean Sequencer as described [on MuffWiggler](https://www.muffwiggler.com/forum/viewtopic.php?t=45485&start=all&postdays=0&postorder=asc).

# Features
+ 3 tracks plus optional inverted track 1 out
+ Programmable 1-16 step sequences or Euclidean sequence per track
+ Forward, Backward, Pendulum or Random play per track
+ Shuffle amount per track
+ Clock divider per track
+ Randomly mutate patterns on each loop using either the original pattern, the last mutation or the inverse of the last mutation as the base for the next mutation
+ Selectable Trigger, Clock width or Gate out per track
+ Internal Clock - base speed, multiplier and width, optionally
+ Optionally send clock (internal or external) to inverted out

## Controls
### Edit Mode 1 - (I) Pattern Definition Settings
+ 1/Length
  + Rotate - Change length start or end length marker to define loop points
  + Click - **Programmed Mode :** Switch between start and end markers  **Euclidean mode:** only the end marker can be moved to define a length
  + Hold (~2s) - Make Track 1 the active editing track
+ 2/Density
  + Rotate - **Programmed Mode :** Move the edit cursor **Euclidean mode:** Change the density
  + Click - **Programmed Mode :** Invert the state of the step under the cursor (on/off) **Euclidean mode:** Nothing
  + Hold (~2s) - Make Track 2 the active editing track
+ 3/Offset
  + Rotate - Offset in the steps in the direction turned. Note - steps outside the current length remain in place.
  + Click - Change Edit Modes **indicated by 7th row of leds**
  + Hold (~2s) - Make Track 3 the active editing track


### Edit Mode 2 - (II) Track Configuration Settings
+ 1/Length
  + Rotate - Set the divider value **Beat Mode :** 1,2,4,8,16,32,64 **Triplet Mode :** 3,6,9,12,15,18,21,24
  + Click -  Switch between divider modes : Beat and Triplet
  + Hold (~2s) - Make Track 1 the active editing track
+ 2/Density
  + Rotate - Select play mode : (->) Forwards, (<-) Backwards, (><) Pendulum and (R) Random
  + Click - Switch between pattern modes : (P) Programmed and (E) Euclidean
  + Hold (~2s) - Make Track 2 the active editing track
+ 3/Offset
  + Rotate - Select output mode : Trigger (~20ms), Clock (match clock gate) and Gate (Output is high while step is active)
  + Click - Change Edit Modes **indicated by 7th row of leds**
  + Hold (~2s) - Make Track 3 the active editing track


### Edit Mode 3 - (III) Pattern Modifiers (Shuffle & Mutation) settings
+ 1/Length
  + Rotate - Change the shuffle amount (0-15) - works on both internal and external clock
  + Click -  Nothing
  + Hold (~2s) - Make Track 1 the active editing track
+ 2/Density
  + Rotate - Select Mutation factor  - the higher the probability (0 to 50%) the more likely steps will be flipped at the end of a loop
  + Click - Choose Mutation Seed for next cycle : (O) Original pattern, (C) Current mutated pattern, (I) Inverse of the Current mutated pattern inverted (if mutation = 0 this inverts the pattern on each loop)
  + Hold (~2s) - Make Track 2 the active editing track
+ 3/Offset
  + Rotate - Nothing
  + Click - Change Edit Modes **indicated by 7th row of leds**
  + Hold (~2s) - Make Track 3 the active editing track


### Edit Mode 4 - (IV) Clock settings
_Note - these settings are not currently persisted_
+ 1/Length
  + Rotate - Change the internal clock speed (~60bpm to 240bpm) changes by 1 bpm at a time so quite slow)
  + Click -  Start/Stop the internal clock (when running the external clock input is ignored)
  + Hold (~2s) - Make Track 1 the active editing track
+ 2/Density
  + Rotate - Change the clock pulse width (~3% to 90%)
  + Click - Send the clock to the offbeat output (the internal clock if running or the external clock if not)
  + Hold (~2s) - Make Track 2 the active editing track
+ 3/Offset
  + Rotate - Set a multiplier for the clock speed (1x,2x,4x,8x,16x)
  + Click - Change Edit Modes **indicated by 7th row of leds**
  + Hold (~2s) - Make Track 3 the active editing track

## Outputs
+ 1 - As programmed on Track 1
+ 2 - As programmed on Track 2
+ 3 - As programmed on Track 3
+ Offbeat - Inverse of Track 1 or the clock signal

## Guidance
**_This is a work in progress!_**
+ display reverts to a play view after ~5 seconds of not twiddling knobs
+ Saving of changes (if there are any) occurs when you switch edit mode or when the display reverts to the play view
+ sync resets on the rising edge
+ you should probably expect to loose your saved tracks each time you update while everything is in flux

## The Future
+ Improve the UX!
+ Configurable sync behaviour; rising or falling edge, reset and hold while high.
+ Adjust brightness and encoder sensitivity
+ I'm open to ideas
+ Some testing.... :)
