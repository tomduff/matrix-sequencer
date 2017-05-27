#include <Arduino.h>
#include "Display.h"
#include "Encoders.h"
#include "Buttons.h"
#include "Input.h"
#include "Output.h"
#include "Tracks.h"
#include "Utilities.h"

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

  drawTracks();

  display.render();
}

void handleReset(Signal signal) {
  if(signal == Signal::Rising) tracks.reset();
  if (signal == Signal::Rising || signal == Signal::High) display.indicateReset();
}

void handleClock(Signal signal) {
  if (signal == Signal::Rising) tracks.stepOn();
  if (signal == Signal::Rising || signal == Signal::High) display.indicateClock();

  handleStep(signal, 0, &oneOut);
  handleStep(signal, 1, &twoOut);
  handleStep(signal, 2, &threeOut);
  handleStep(signal, 3, &offOut);
}

void handleStep(Signal signal, int track, Output* out) {
  int step = 0;
  if (track == 3) step = !tracks.getStep(0);
  else step = tracks.getStep(track);
  int output = out->signal(signal, tracks.getOutMode(track), step);
  if (output) display.indicateTrack(track);
}

void handleEvent(ControlEvent event) {
  if (event.control != Control::NoControl) lastEdit = millis();
  switch(event.control) {
    case Control::One:
      if (event.type == ControlType::EncoderControl) {
        handleLengthEdit(event.state);
      } else if (event.type == ControlType::ButtonControl) {
        if(event.state == ButtonState::Clicked) {
          handlePlayModeEdit();
        } else if (event.state == ButtonState::Held) {
          active = event.control;
        }
      }
      break;
    case Control::Two:
      if (event.type == ControlType::EncoderControl) {
        handlePatternCursor(event.state);
      } else if (event.type == ControlType::ButtonControl) {
        if(event.state == ButtonState::Clicked) {
          handlePatternEdit();
        } else if (event.state == ButtonState::Held) {
          active = event.control;
        }
      }
      break;
    case Control::Three:
      if (event.type == ControlType::EncoderControl) {
        handleOffsetEdit(event.state);
      } else if (event.type == ControlType::ButtonControl) {
        if (event.state == ButtonState::Clicked) {
          handleOutModeEdit();
        } else if (event.state == ButtonState::Held) {
          active = event.control;
        }
      }
      break;
    case Control::NoControl:
      break;
  }
}

void setEditAction(EditAction current) {
  tracks.save();
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
  display.drawLengthView(active, tracks.getLength(active));
}

void handlePatternCursor(int change) {
  if (action != EditAction::EditPattern) {
    cursor = 0;
    setEditAction(EditAction::EditPattern);
    display.drawPatternView(active, tracks.getPattern(active));
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
  display.drawPatternView(active, tracks.getPattern(active));
}

void handleOffsetEdit(int change) {
  if (action != EditAction::EditOffset) {
    display.hideCursor();
    setEditAction(EditAction::EditOffset);
  } else {
    tracks.applyOffset(active, change);
  }
  display.drawPatternView(active, tracks.getPattern(active));
}

void handlePlayModeEdit() {
  if (action != EditAction::EditPlayMode) {
    display.hideCursor();
    setEditAction(EditAction::EditPlayMode);
  } else {
    tracks.nextPlayMode(active);
  }
  display.drawPlayModeView(active, tracks.getPlayMode(active));
}

void handleOutModeEdit() {
  if (action != EditAction::EditOutMode) {
    display.hideCursor();
    setEditAction(EditAction::EditOutMode);
  } else {
    tracks.nextOutMode(active);
  }
  display.drawOutModeView(active, tracks.getOutMode(active));
}

void drawCursor() {
  display.showCursor();
  display.drawRowsCursor(active * 2, cursor);
}

void drawTracks() {
  for(int track = 0; track < 3; ++track) {
    if(track != active || action == EditAction::NoAction) {
      display.drawPlayView(track, tracks.getPosition(track), tracks.getPattern(track));
    }
  }
}
