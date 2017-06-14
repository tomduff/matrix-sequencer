#include <Arduino.h>
#include "Display.h"
#include "Encoders.h"
#include "Buttons.h"
#include "Input.h"
#include "Output.h"
#include "Tracks.h"
#include "Utilities.h"
#include "ClockGenerator.h"
#include "Shuffle.h"

#define EDIT_WAIT 5000
#define CLOCK_WAIT 5000
#define EDIT_MODES 4
#define EDIT_TRACKS 3
#define OFF_BEAT 3

enum EditAction {
  NoAction,
  EditLength,
  EditPattern,
  EditOffset,
  EditPlayMode,
  EditOutMode,
  EditDivider,
  EditDividerType,
  EditPatternType,
  EditShuffle,
  EditMutation,
  EditMutationSeed
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
ClockGenerator clockGenerator = ClockGenerator();
Shuffle shuffle = Shuffle();
int edit = -1;
int cursor = 0;
int active = 0;
EditAction action = EditAction::NoAction;
unsigned long now =  0;
unsigned long lastEdit = 0;
unsigned long lastClock = 0;
bool lengthMarker = true;
bool offBeatOut = true;
bool clocked = false;

void setup() {
  initialiseEditModes();
  display.initialise();
  encoders.initialise();
  buttons.initialise();
  clock.initialise();
  reset.initialise();
  for(int track = 0; track < EDIT_TRACKS; ++track) outs[track].initialise();
  clearEditAction();
  display.indicateMode(edit);
  handleButtonHeld(Control::One);
}

void loop() {
  handleReset(reset.signal());

  if (!clockGenerator.isRunning()) handleClock(clock.signal());
  else handleClock(clockGenerator.tick());

  handleEncoderEvent(encoders.event());
  handleButtonEvent(buttons.event());

  if (lastEdit != 0 && (millis() - lastEdit) > EDIT_WAIT) {
      clearEditAction();
  }

  drawTracks();
  display.render();
}

void drawTracks() {
  for(int track = 0; track < EDIT_TRACKS; ++track) {
    if (track != active || action == EditAction::NoAction) display.drawPlayView(track, tracks.getPosition(track), tracks.getPattern(track), clocked);
  }
}

void handleReset(Signal signal) {
  if (signal == Signal::Rising) {
    shuffle.reset();
    tracks.reset();
  }
  if (signal == Signal::Rising || signal == Signal::High) display.indicateReset();
}

void handleClock(Signal signal) {
  shuffle.clock(signal);
  if (signal == Signal::Rising) tracks.stepOn();
  if (signal == Signal::Low && (now - lastClock) > CLOCK_WAIT) {
    clocked = false;
    lastClock = 0;
  } else if (signal == Signal::Rising || signal == Signal::High) {
    clocked = true;
    lastClock = now;
    display.indicateClock();
  }
  for (int track = 0; track < EDIT_TRACKS; ++track) handleStep(track);
  if (offBeatOut) handleStep(OFF_BEAT);
  else outs[OFF_BEAT].signal(signal, OutMode::Clock, 1);
}

void handleStep(int track) {
  int step = tracks.getStep(track);
  Signal signal = shuffle.tick(track, tracks.getShuffle(track));
  if (!tracks.getStepped(track) && Signal::Rising) signal = Signal::Low;
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
  if (event.state == ButtonState::Clicked) handleButtonClick(event.control);
  else if (event.state == ButtonState::Held) handleButtonHeld(event.control);
}

void handleButtonClick(Control control) {
  switch(control) {
    case Control::One:
      lastEdit = millis();
      editModes[edit].oneClick();
      break;
  case Control::Two:
      lastEdit = millis();
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
  display.indicateMode(edit);
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
  display.timeout();
}

void moveCursor(int Offset, int max) {
    cursor += Offset;
    Utilities::cycle(cursor, 0, max);
    display.setTrackCursor(active, cursor);
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
  lengthView();
}

void switchLengthMarker() {
  if (action != EditAction::EditLength) initialiseLengthEdit();
  else if (tracks.getPatternType(active) == PatternType::Programmed) lengthMarker = !lengthMarker;
  lengthView();
}

void initialiseLengthEdit() {
  lengthMarker = true;
  setEditAction(EditAction::EditLength);
}

void lengthView() {
  if (tracks.getPatternType(active) == PatternType::Programmed) display.drawLengthView(active, tracks.getStart(active), tracks.getEnd(active), lengthMarker);
  else if (tracks.getPatternType(active) == PatternType::Euclidean) display.drawLengthView(active, tracks.getLength(active));
}

void movePatternCursor(int change) {
  if (action != EditAction::EditPattern) {
    initialisePatternEdit();
  } else {
    if (tracks.getPatternType(active) == PatternType::Programmed) moveCursor(change, tracks.getLength(active));
    else if (tracks.getPatternType(active) == PatternType::Euclidean) tracks.setDensity(active, change);
  }
  patternView();
}

void patternEdit() {
  if (action != EditAction::EditPattern) initialisePatternEdit();
  else if (tracks.getPatternType(active) == PatternType::Programmed) tracks.updatePattern(active, cursor);
  patternView();
}

void initialisePatternEdit() {
  cursor = 0;
  setEditAction(EditAction::EditPattern);
}

void patternView() {
  if (tracks.getPatternType(active) == PatternType::Programmed) display.drawProgrammedView(active, tracks.getPattern(active));
  else if (tracks.getPatternType(active) == PatternType::Euclidean) display.drawEuclideanView(active, tracks.getPattern(active));
}

void offsetEdit(int change) {
  if (action != EditAction::EditOffset) {
    setEditAction(EditAction::EditOffset);
  } else {
    if (tracks.getPatternType(active) == PatternType::Programmed) tracks.rotatePattern(active, change);
    else if (tracks.getPatternType(active) == PatternType::Euclidean) tracks.setOffset(active, change);
  }
  display.drawOffsetView(active, tracks.getPattern(active));
}

void playModeEdit(int change) {
  if (action != EditAction::EditPlayMode) setEditAction(EditAction::EditPlayMode);
  else tracks.setPlayMode(active, change);
  display.drawPlayModeView(active, tracks.getPlayMode(active));
}

void outModeEdit(int change) {
  if (action != EditAction::EditOutMode) setEditAction(EditAction::EditOutMode);
  else tracks.setOutMode(active, change);
  display.drawOutModeView(active, tracks.getOutMode(active));
}

void dividerEdit(int change) {
  if (action != EditAction::EditDivider) setEditAction(EditAction::EditDivider);
  else tracks.setDivider(active, change);
  display.drawDividerView(active, tracks.getDivider(active), tracks.getDividerType(active));
}

void mutationEdit(int change) {
  if (action != EditAction::EditMutation) setEditAction(EditAction::EditMutation);
  else tracks.setMutation(active, change);
  display.drawMutationView(active, tracks.getMutation(active));
}

void switchDividerType() {
  if (action != EditAction::EditDividerType) setEditAction(EditAction::EditDividerType);
  else tracks.nextDividerType(active);
  display.drawDividerTypeView(active, tracks.getDividerType(active));
}

void switchPatternType() {
  if (action != EditAction::EditPatternType) setEditAction(EditAction::EditPatternType);
  else tracks.nextPatternType(active);
  display.drawPatternTypeView(active, tracks.getPatternType(active));
}

void switchMutationSeed() {
  if (action != EditAction::EditMutationSeed) setEditAction(EditAction::EditMutationSeed);
  else tracks.nextMutationSeed(active);
  display.drawMutationSeedView(active, tracks.getMutationSeed(active));
}

void shuffleEdit(int change) {
  if (action != EditAction::EditShuffle) setEditAction(EditAction::EditShuffle);
  else tracks.setShuffle(active, change);
  display.drawShuffleView(active, tracks.getShuffle(active));
}

void clockSpeedEdit(int change) {
  clockGenerator.setSpeed(change);
}

void clockWidthEdit(int change) {
  clockGenerator.setWidth(change);
}

void clockMulitplierEdit(int change) {
  clockGenerator.setMulitplier(change);
}

void startStopClock() {
  if(clockGenerator.isRunning()) clockGenerator.stop();
  else clockGenerator.start();
  clocked = clockGenerator.isRunning();
}

void switchOffBeatOut() {
  offBeatOut = !offBeatOut;
}

void initialiseEditModes() {
  editModes[0] = EditMode{lengthEdit, switchLengthMarker, movePatternCursor, patternEdit, offsetEdit};
  editModes[1] = EditMode{dividerEdit, switchDividerType, playModeEdit, switchPatternType, outModeEdit};
  editModes[2] = EditMode{shuffleEdit, noActionButton, mutationEdit, switchMutationSeed, noActionEncoder};
  editModes[3] = EditMode{clockSpeedEdit, startStopClock, clockWidthEdit, switchOffBeatOut, clockMulitplierEdit};
  edit = 0;
}

void noActionButton() {}
void noActionEncoder(int change) {}
