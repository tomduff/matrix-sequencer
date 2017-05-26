#include <Arduino.h>
#include "Display.h"
#include "Encoders.h"
#include "Buttons.h"
#include "Input.h"
#include "Output.h"
#include "Tracks.h"
#include "Utilities.h"

#define INDICATOR_ROW 7
#define CLOCK_INDICATOR 0
#define RESET_INDICATOR 1
#define TRACK_ONE_INDICATOR 2
#define TRACK_TWO_INDICATOR 5
#define TRACK_THREE_INDICATOR 7
#define OFF_BEAT_INDICATOR 3

Display display = Display();
Encoders encoders = Encoders();
Buttons buttons = Buttons();
Input clock = Input(0);
Input reset = Input(1);
Output oneOut = Output(11);
Output twoOut = Output(12);
Output threeOut = Output(13);
Output offOut = Output(17);
Tracks tracks = Tracks();

enum EditAction {
  NoAction,
  EditLength,
  EditPattern,
  EditOffset,
  EditPlayMode,
  EditOutMode
};

#define EDIT_WAIT 7500

int cursor = 0;
int active = Control::One;
EditAction action = EditAction::NoAction;
unsigned long lastEdit = 0;

void setup() {
  display.initialise();
  encoders.initialise();
  buttons.initialise();
  clock.initialise();
  oneOut.initialise();
  twoOut.initialise();
  threeOut.initialise();
  offOut.initialise();
}

void loop() {

  handleReset(reset.signal());
  handleClock(clock.signal());
  handleEvent(encoders.event());
  handleEvent(buttons.event());

  if (action == EditAction::NoAction || lastEdit == 0 || (millis() - lastEdit > EDIT_WAIT)) {
    display.hideCursor();
    clearEditAction();
  }

  playView();

  display.render();
}

void handleReset(Signal signal) {
  if(signal == Signal::Rising) tracks.reset();
  display.setLed(INDICATOR_ROW, RESET_INDICATOR, (signal == Signal::Rising));
}

void handleClock(Signal signal) {
  if (signal == Signal::Rising) tracks.stepOn();
  handleStep(signal, tracks.getStep(0), tracks.getOutMode(0), TRACK_ONE_INDICATOR, &oneOut);
  handleStep(signal, tracks.getStep(1), tracks.getOutMode(1), TRACK_TWO_INDICATOR, &twoOut);
  handleStep(signal, tracks.getStep(2), tracks.getOutMode(2), TRACK_THREE_INDICATOR, &threeOut);
  handleStep(signal, !tracks.getStep(0), tracks.getOutMode(0), OFF_BEAT_INDICATOR, &offOut);
  display.setLed(INDICATOR_ROW, CLOCK_INDICATOR, (signal == Signal::Rising || signal == Signal::High));
}

void handleStep(Signal signal, int step, OutMode mode, int led, Output* out) {
  int output = out->signal(signal, mode, step);
  display.setLed(INDICATOR_ROW, led, output);
}

void handleEvent(ControlEvent event) {
  switch(event.control) {
    case Control::One:
      if (event.type == ControlType::EncoderControl) {
        handleLengthEdit(event.state);
      } else if (event.type == ControlType::ButtonControl) {
        handlePlayModeEdit();
        if(event.state == ButtonState::Clicked) {
        } else if (event.state == ButtonState::Held) {
          active = event.control;
        }
      }
      break;
    case Control::Two:
      if (event.type == ControlType::EncoderControl) {
        handlePatternCursor(event.state);
      } else if(event.state == ButtonState::Clicked) {
        handlePatternEdit();
      } else if (event.state == ButtonState::Held) {
        active = event.control;
      }
      break;
    case Control::Three:
      if (event.type == ControlType::EncoderControl) {
        handleOffsetEdit(event.state);
      } else if (event.state == ButtonState::Clicked) {
        handleOutModeEdit();
      } else if (event.state == ButtonState::Held) {
        active = event.control;
      }
      break;
    case Control::NoControl:
      break;
  }
}

void setEditAction(EditAction current) {
  tracks.save();
  lastEdit = millis();
  action = current;
}

void clearEditAction() {
  tracks.save();
  lastEdit = 0;
  action = EditAction::NoAction;
}

void moveCursor(int Offset, int max) {
    cursor += Offset;
    Utilities::cycle(cursor, 0, max);
}

void handleLengthEdit(int change) {
  if (action != EditAction::EditLength) {
    display.hideCursor();
    setEditAction(EditAction::EditLength);
  }  else {
    tracks.setLength(active, change);
  }
  lengthView();
}

void handlePatternCursor(int change) {
  if (action != EditAction::EditPattern) {
    cursor = 0;
    setEditAction(EditAction::EditPattern);
    patternView();
  } else {
    moveCursor(change, tracks.getLength(active));
  }
  drawCursor();
}

void handlePatternEdit() {
  if (action != EditAction::EditPattern) {
    cursor = 0;
    display.showCursor();
    setEditAction(EditAction::EditPattern);
  } else {
    tracks.updatePattern(active, cursor);
  }
  drawCursor();
  patternView();
}

void handleOffsetEdit(int change) {
  if (action != EditAction::EditOffset) {
    display.hideCursor();
    setEditAction(EditAction::EditOffset);
  } else {
    tracks.applyOffset(active, change);
  }
  offsetView();
}

void handlePlayModeEdit() {
  if (action != EditAction::EditPlayMode) {
    cursor = tracks.getPlayMode(active);
    display.hideCursor();
    setEditAction(EditAction::EditPlayMode);
  } else {
    moveCursor(1, 2);
    tracks.setPlayMode(active, (PlayMode) cursor);
  }
  playModeView();
}

void handleOutModeEdit() {
  if (action != EditAction::EditOutMode) {
    cursor = tracks.getPlayMode(active);
    display.hideCursor();
    setEditAction(EditAction::EditOutMode);
  } else {
    moveCursor(1, 2);
    tracks.setOutMode(active, (OutMode) cursor);
  }
  outModeView();
}

void lengthView() {
  display.drawRowsBar(active * 2, tracks.getLength(active));
}

void drawCursor() {
  display.showCursor();
  display.drawRowsCursor(active * 2, cursor);
}

void patternView() {
  display.drawRowsPattern(active * 2, tracks.getPattern(active));
}

void offsetView() {
  patternView();
}

void playModeView() {
  display.drawRowIndicator(active * 2, tracks.getPlayMode(active));
  display.drawRowPattern(active * 2 + 1, 0);
}

void outModeView() {
  display.drawRowPattern(active * 2, 0);
  display.drawRowsIndicator(active * 2 + 1, tracks.getOutMode(active));
}

void playView() {
  for(int track = 0; track < 3; ++track) {
    if(track != active || action == EditAction::NoAction) {
      int pattern = 0;
      int position = tracks.getPosition(track);
      if (position <= 7) {
        pattern = lowByte(tracks.getPattern(track));
        bitSet(pattern, position + 8);
      } else {
        pattern = highByte(tracks.getPattern(track));
        bitSet(pattern, position);
      }
      display.drawRowsPattern(track * 2, pattern);
    }
  }
}
