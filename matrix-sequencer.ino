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
#define TRACK_ONE_INDICATOR 2
#define TRACK_TWO_INDICATOR 5
#define TRACK_THREE_INDICATOR 7
#define OFF_BEAT_INDICATOR 3

Display display = Display();
Encoders encoders = Encoders();
Buttons buttons = Buttons();
Input clock = Input(0);
//Input sync = Input(0);
Output oneOut = Output(11);
Output twoOut = Output(12);
Output threeOut = Output(13);
Output offOut = Output(17);
Tracks tracks = Tracks();

enum EditAction {
  NoAction,
  Length,
  Pattern,
  Offset
};

#define EDIT_WAIT 10000

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

void handleClock(Signal signal) {
  if (signal == Signal::Rising) tracks.stepOn();
  handleStep(signal, tracks.getStep(0), TRACK_ONE_INDICATOR, &oneOut);
  handleStep(signal, tracks.getStep(1), TRACK_TWO_INDICATOR, &twoOut);
  handleStep(signal, tracks.getStep(2), TRACK_THREE_INDICATOR, &threeOut);
  handleStep(signal, !tracks.getStep(0), OFF_BEAT_INDICATOR, &offOut);
  display.setLed(INDICATOR_ROW, CLOCK_INDICATOR, (signal == Signal::Rising || signal == Signal::High));
}

void handleStep(Signal signal, int step, int led, Output* out) {
  int output = out->signal(signal, step);
  display.setLed(INDICATOR_ROW, led, output);
}

void handleEvent(ControlEvent event) {
  switch(event.control) {
    case Control::One:
      if (event.type == ControlType::EncoderControl) {
        handleLengthEdit(event.state);
      } else if (event.type == ControlType::ButtonControl) {
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
      } else if (event.state == ButtonState::Held) {
        active = event.control;
      }
      break;
    case Control::NoControl:
      break;
  }
}

void setEditAction(EditAction current) {
  lastEdit = millis();
  action = current;
}

void clearEditAction() {
  lastEdit = 0;
  action = EditAction::NoAction;
}

void moveCursor(int Offset) {
    cursor += Offset;
    Utilities::cycle(cursor, 0, tracks.getLength(active));
}

void handleLengthEdit(int change) {
  if (action != EditAction::Length) {
    display.hideCursor();
    setEditAction(EditAction::Length);
  }  else {
    tracks.setLength(active, change);
  }
  lengthView();
}

void handlePatternCursor(int change) {
  if (action != EditAction::Pattern) {
    display.showCursor();
    setEditAction(EditAction::Pattern);
    patternView();
  } else {
    moveCursor(change);
    drawCursor();
  }
}

void handlePatternEdit() {
  if (action != EditAction::Pattern) {
    display.showCursor();
    setEditAction(EditAction::Pattern);
  } else {
    tracks.updatePattern(active, cursor);
  }
  patternView();
}

void handleOffsetEdit(int change) {
  if (action != EditAction::Offset) {
    display.hideCursor();
    setEditAction(EditAction::Offset);
  } else {
    tracks.applyOffset(active, change);
  }
  offsetView();
}

void lengthView() {
  display.drawRowsBar(active * 2, tracks.getLength(active));
}

void drawCursor() {
  display.drawRowsCursor(active * 2, cursor);
}

void patternView() {
  display.drawRowsPattern(active * 2, tracks.getPattern(active));
}

void offsetView() {
  display.drawRowsPattern(active * 2, tracks.getPattern(active));
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
