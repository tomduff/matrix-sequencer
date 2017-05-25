#include <Arduino.h>
#include "Display.h"
#include "Encoders.h"
#include "Buttons.h"
#include "Inputs.h"
#include "Outputs.h"
#include "Tracks.h"
#include "Utilities.h"

Display display = Display();
Encoders encoders = Encoders();
Buttons buttons = Buttons();
Inputs inputs = Inputs();
Outputs outputs = Outputs();
Tracks tracks = Tracks();

enum EditAction {
  NO_EDIT,
  LENGTH,
  PATTERN,
  OFFSET
};

#define EDIT_WAIT 10000

int cursor = 0;
int active = Control::ONE;
EditAction action = EditAction::NO_EDIT;
unsigned long lastEdit = 0;

void setup() {
  display.initialise();
  encoders.initialise();
  buttons.initialise();
  inputs.initialise();
  outputs.initialise();
}

void loop() {

  handleEvent(encoders.event());
  handleEvent(buttons.event());

  //drawTracks();

  if (action == EditAction::NO_EDIT || lastEdit == 0 || (millis() - lastEdit > EDIT_WAIT)) {
    display.hideCursor();
    clearEditAction();
  } else {
  }

  display.render();
}

void handleEvent(ControlEvent event) {
  switch(event.control) {
    case Control::ONE:
      if (event.type == ControlType::ENCODER) {
        handleLengthEdit(event.state);
      } else if (event.type == ControlType::BUTTON) {
        if(event.state == ButtonState::CLICKED) {
        } else if (event.state == ButtonState::HELD) {
          active = event.control;
        }
      }
      break;
    case Control::TWO:
      if (event.type == ControlType::ENCODER) {
        handlePatternCursor(event.state);
      } else if(event.state == ButtonState::CLICKED) {
        handlePatternEdit();
      } else if (event.state == ButtonState::HELD) {
        active = event.control;
      }
      break;
    case Control::THREE:
      if (event.type == ControlType::ENCODER) {
        handleOffsetEdit(event.state);
      } else if (event.state == ButtonState::CLICKED) {
      } else if (event.state == ButtonState::HELD) {
        active = event.control;
      }
      break;
    case Control::NONE:
      break;
  }
}

void setEditAction(EditAction current) {
  lastEdit = millis();
  action = current;
}

void clearEditAction() {
  lastEdit = 0;
  action = EditAction::NO_EDIT;
}

void moveCursor(int offset) {
    cursor += offset;
    Utilities::cycle(cursor, 0, tracks.length(active));
}

void handleLengthEdit(int change) {
  if (action != EditAction::LENGTH) {
    display.hideCursor();
    setEditAction(EditAction::LENGTH);
  }  else {
    tracks.updateLength(active, change);
  }
  lengthView();
}

void handlePatternCursor(int change) {
  if (action != EditAction::PATTERN) {
    display.showCursor();
    setEditAction(EditAction::PATTERN);
    patternView();
  } else {
    moveCursor(change);
    drawCursor();
  }
}

void handlePatternEdit() {
  if (action != EditAction::PATTERN) {
    display.showCursor();
    setEditAction(EditAction::PATTERN);
  } else {
    tracks.updatePattern(active, cursor);
  }
  patternView();
}

void handleOffsetEdit(int change) {
  if (action != EditAction::OFFSET) {
    display.hideCursor();
    setEditAction(EditAction::OFFSET);
  } else {
    tracks.offset(active, change);
  }
  offsetView();
}

void lengthView() {
  display.drawRowsBar(active * 2, tracks.length(active));
}

void drawCursor() {
  display.drawRowsCursor(active * 2, cursor);
}

void patternView() {
  display.drawRowsPattern(active * 2, tracks.pattern(active));
}

void offsetView() {
  display.drawRowsPattern(active * 2, tracks.pattern(active));
}
