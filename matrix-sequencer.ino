#include <Arduino.h>
#include "Display.h"
#include "Encoders.h"
#include "Buttons.h"
#include "Input.h"
#include "Output.h"
#include "Tracks.h"
#include "Utilities.h"

#define EDIT_WAIT 7500
#define EDIT_MODES 2
#define TRACKS 4
#define DRAWN_TRACKS 3

enum EditAction {
  NoAction,
  EditLength,
  EditPattern,
  EditOffset,
  EditPlayMode,
  EditOutMode,
  EditDivider,
  EditPatternType
};

struct EditMode {
  void (*oneRotate)(int);
  void (*oneClick)();
  void (*twoRotate)(int);
  void (*twoClick)();
  void (*threeRotate)(int);
};

Display display = Display();
Encoders encoders = Encoders();
Buttons buttons = Buttons();
Input clock = Input(0);
Input reset = Input(1);
Output outs[4] = {Output(11), Output(12), Output(13), Output(17)};
Tracks tracks = Tracks();
EditMode editModes[EDIT_MODES];
int edit = 0;
int cursor = 0;
int active = 0;
EditAction action = EditAction::NoAction;
unsigned long lastEdit = 0;
bool lengthMarker = true;

void setup() {
  initialiseEditModes();
  display.initialise();
  encoders.initialise();
  buttons.initialise();
  clock.initialise();
  reset.initialise();
  for(int track = 0; track < TRACKS; ++track) outs[track].initialise();
  clearEditAction();
  handleButtonHeld(Control::One);
}

void loop() {

  handleReset(reset.signal());
  handleClock(clock.signal());
  handleEncoderEvent(encoders.event());
  handleButtonEvent(buttons.event());

  if (action == EditAction::NoAction || lastEdit == 0 || (millis() - lastEdit > EDIT_WAIT)) {
    display.hideCursor(active);
    clearEditAction();
  }

  drawTracks();
  display.indicateMode(edit);
  display.render();
}

void handleReset(Signal signal) {
  if(signal == Signal::Rising) tracks.reset();
  if (signal == Signal::Rising || signal == Signal::High) display.indicateReset();
}

void handleClock(Signal signal) {
  if (signal == Signal::Rising) tracks.stepOn();
  if (signal == Signal::Rising || signal == Signal::High) display.indicateClock();
  for(int track = 0; track < TRACKS; ++track) handleStep(signal, track);
}

void handleStep(Signal signal, int track) {
  int step = tracks.getStep(track);
  int output = outs[track].signal(signal, tracks.getOutMode(track), step);
  if (output) display.indicateTrack(track);
}

void handleEncoderEvent(EncoderEvent event) {
  if (event.control != Control::NoControl) lastEdit = millis();
  switch(event.control) {
    case Control::One:
      editModes[edit].oneRotate(event.state);
      break;
    case Control::Two:
      editModes[edit].twoRotate(event.state);
      break;
    case Control::Three:
      editModes[edit].threeRotate(event.state);
      break;
    case Control::NoControl:
      break;
  }
}

void handleButtonEvent(ButtonEvent event) {
  if (event.control != Control::NoControl) lastEdit = millis();
  if (event.state == ButtonState::Clicked) handleButtonClick(event.control);
  else if (event.state == ButtonState::Held) handleButtonHeld(event.control);
}

void handleButtonClick(Control control) {
  switch(control) {
    case Control::One:
        editModes[edit].oneClick();
    break;
  case Control::Two:
        editModes[edit].twoClick();
      break;
    case Control::Three:
        nextEditMode();
      break;
    case Control::NoControl:
      break;
  }
}

void nextEditMode() {
  ++edit;
  Utilities::cycle(edit, 0, EDIT_MODES - 1);
  clearEditAction();
}

void handleButtonHeld(Control control) {
  active = control;
  cursor = 0;
  clearEditAction();
  display.indicateActiveTrack(active);
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
    drawCursor();
}

void lengthEdit(int change) {
  if (action != EditAction::EditLength) {
    initialiseLengthEdit();
  }  else {
    if (tracks.getPatternType(active) == PatternType::Programmed) {
      if (lengthMarker) tracks.setEnd(active, change);
      else tracks.setStart(active, change);
    } else if (tracks.getPatternType(active) == PatternType::Euclidean) {
      tracks.setLength(active, change);
    }
  }
  if (tracks.getPatternType(active) == PatternType::Programmed) display.drawLengthView(active, tracks.getStart(active), tracks.getEnd(active));
  else if (tracks.getPatternType(active) == PatternType::Euclidean) display.drawLengthView(active, tracks.getLength(active));
}

void switchLengthMarker() {
  if (action != EditAction::EditLength) {
    initialiseLengthEdit();
  }  else {
    if (tracks.getPatternType(active) == PatternType::Programmed) lengthMarker = !lengthMarker;
  }
  display.drawLengthView(active, tracks.getStart(active), tracks.getEnd(active));
}

void initialiseLengthEdit() {
  lengthMarker = true;
  display.hideCursor(active);
  setEditAction(EditAction::EditLength);
}

void movePatternCursor(int change) {
  if (action != EditAction::EditPattern) {
    initialisePatternEdit();
  } else {
    if (tracks.getPatternType(active) == PatternType::Programmed) moveCursor(change, tracks.getLength(active));
    else if (tracks.getPatternType(active) == PatternType::Euclidean) tracks.setDensity(active, change);
  }
  display.drawPatternView(active, tracks.getPattern(active));
}

void patternEdit() {
  if (action != EditAction::EditPattern) {
    initialisePatternEdit();
  } else {
    if (tracks.getPatternType(active) == PatternType::Programmed) tracks.updatePattern(active, cursor);
  }
  display.drawPatternView(active, tracks.getPattern(active));
}

void initialisePatternEdit() {
  cursor = 0;
  if (tracks.getPatternType(active) == PatternType::Programmed) display.showCursor(active);
  setEditAction(EditAction::EditPattern);
}

void offsetEdit(int change) {
  if (action != EditAction::EditOffset) {
    display.hideCursor(active);
    setEditAction(EditAction::EditOffset);
  } else {
    if (tracks.getPatternType(active) == PatternType::Programmed) tracks.rotatePattern(active, change);
    else if (tracks.getPatternType(active) == PatternType::Euclidean) tracks.setOffset(active, change);
  }
  display.drawPatternView(active, tracks.getPattern(active));
}

void playModeEdit(int change) {
  if (action != EditAction::EditPlayMode) {
    display.hideCursor(active);
    setEditAction(EditAction::EditPlayMode);
  } else {
    tracks.setPlayMode(active, change);
  }
  display.drawPlayModeView(active, tracks.getPlayMode(active));
}

void outModeEdit(int change) {
  if (action != EditAction::EditOutMode) {
    display.hideCursor(active);
    setEditAction(EditAction::EditOutMode);
  } else {
    tracks.setOutMode(active, change);
  }
  display.drawOutModeView(active, tracks.getOutMode(active));
}

void dividerEdit(int change) {
  if (action != EditAction::EditDivider) {
    display.hideCursor(active);
    setEditAction(EditAction::EditDivider);
  } else {
    tracks.setDivider(active, change);
  }
  display.drawDividerView(active, tracks.getDivider(active), tracks.getDividerType(active));
}

void switchDividerType() {
  if (action != EditAction::EditDivider) {
    display.hideCursor(active);
    setEditAction(EditAction::EditDivider);
  } else {
    tracks.nextDividerType(active);
  }
  display.drawDividerView(active, tracks.getDivider(active), tracks.getDividerType(active));
}

void switchPatternType() {
  if (action != EditAction::EditPatternType) {
    display.hideCursor(active);
    setEditAction(EditAction::EditPatternType);
  } else {
    tracks.nextPatternType(active);
  }
  display.drawPatternTypeView(active, tracks.getPatternType(active));
}

void drawCursor() {
  display.showCursor(active);
  display.drawTrackCursor(active, cursor);
}

void drawTracks() {
  for(int track = 0; track < DRAWN_TRACKS; ++track) {
    if(track != active || action == EditAction::NoAction) display.drawPlayView(track, tracks.getPosition(track), tracks.getPattern(track));
  }
}

void initialiseEditModes() {
  editModes[0] = EditMode{lengthEdit, switchLengthMarker, movePatternCursor, patternEdit, offsetEdit};
  editModes[1] = EditMode{dividerEdit, switchDividerType, playModeEdit, switchPatternType, outModeEdit};
  edit = 0;
}

void noActionButton() {}
void noActionEncoder(int change) {}
