/* Burr Solver
 * Copyright (C) 2003-2006  Andreas R�ver
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef __WINDOW_WIDGETS_H__
#define __WINDOW_WIDGETS_H__

#include "grideditor.h"
#include "voxeldrawer.h"
#include "BlockList.h"
#include "Images.h"
#include "Layouter.h"

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Roller.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Progress.H>
#include <FL/fl_draw.h>

class guiGridType_c;
class piecePositions_c;

// my button, the only change it that the box is automatically set to engraved
class FlatButton : public Fl_Button {

public:

  FlatButton(int x, int y, int w, int h, const char * txt, const char * tt) : Fl_Button(x, y, w, h, txt) {
    box(FL_THIN_UP_BOX);
    tooltip(tt);
    clear_visible_focus();
  }

  FlatButton(int x, int y, int w, int h, const char * txt, const char * tt, Fl_Callback* cb, void * cb_para) : Fl_Button(x, y, w, h, txt) {
    box(FL_THIN_UP_BOX);
    tooltip(tt);
    callback(cb, cb_para);
    clear_visible_focus();
  }

  FlatButton(int x, int y, int w, int h, Fl_Image * img, Fl_Image * inact, const char * tt, Fl_Callback1* cb, long cb_para) : Fl_Button(x, y, w, h) {
    box(FL_THIN_UP_BOX);
    tooltip(tt);
    callback(cb, cb_para);
    image(img);
    deimage(inact);
    clear_visible_focus();
  }
};

class LFlatButton_c : public FlatButton, public layoutable_c {

public:

  LFlatButton_c(int x, int y, int w, int h, const char * txt, const char * tt) : FlatButton(0, 0, 0, 0, txt, tt), layoutable_c(x, y, w, h) {
  }

  LFlatButton_c(int x, int y, int w, int h, const char * txt, const char * tt, Fl_Callback* cb, void * cb_para) : FlatButton(0, 0, 0, 0, txt, tt, cb, cb_para), layoutable_c(x, y, w, h) {
  }

  LFlatButton_c(int x, int y, int w, int h, Fl_Image * img, Fl_Image * inact, const char * tt, Fl_Callback1* cb, long cb_para) : FlatButton(0, 0, 0, 0, img, inact, tt, cb, cb_para), layoutable_c(x, y, w, h) {
  }

  virtual void getMinSize(int *width, int *height) const {
    *width = 0;
    ((LFlatButton_c*)this)->measure_label(*width, *height);
    *width += 4;
    *height += 4;
  }
};

class ToggleButton : public Fl_Button {

  Fl_Callback *callback;
  void * callback_para;
  long para;


  public:
    ToggleButton(int x, int y, int w, int h, Fl_Callback *cb, void * cb_para, long para);

    void toggle(void) {
      value(1-value());
      if (callback)
        callback(this, callback_para);
    }

    long ButtonVal(void) { return para; }

};

class LToggleButton_c : public Fl_Button, public layoutable_c {

  Fl_Callback *callback;
  void * callback_para;
  long para;

  public:
    LToggleButton_c(int x, int y, int w, int h, Fl_Callback *cb, void * cb_para, long para);

    void toggle(void) {
      value(1-value());
      if (callback)
        callback(this, callback_para);
    }

    long ButtonVal(void) { return para; }

    virtual void getMinSize(int *width, int *height) const {
      *width = 0;
      ((LFlatButton_c*)this)->measure_label(*width, *height);
      *width += 4;
      *height += 4;
    }
};

class LineSpacer;

// the group for the square editor including the coloured marker and the slider for the z axis
class VoxelEditGroup_c : public Fl_Group, public layoutable_c {

  gridEditor_c * sqedit;
  Fl_Slider * zselect;
  LineSpacer * space;

public:

  VoxelEditGroup_c(int x, int y, int w, int h, puzzle_c * puzzle, const guiGridType_c * ggt);

  void newGridType(const guiGridType_c * ggt, puzzle_c * puzzle);

  void draw();

  void cb_Zselect(Fl_Slider* o) {
    sqedit->setZ(int(zselect->maximum() - o->value()));
  }

  void setZ(unsigned int val);
  int getZ(void) { return sqedit->getZ(); }

  void cb_Sqedit(gridEditor_c* o) { do_callback(this, user_data()); }

  int getReason(void) { return sqedit->getReason(); }

  bool getMouse(void) { return sqedit->getMouse(); }

  int getMouseX1(void) { return sqedit->getMouseX1(); }
  int getMouseY1(void) { return sqedit->getMouseY1(); }
  int getMouseX2(void) { return sqedit->getMouseX2(); }
  int getMouseY2(void) { return sqedit->getMouseY2(); }
  int getMouseZ(void) { return sqedit->getMouseZ(); }

  void setPuzzle(puzzle_c * puzzle, unsigned int num);

  void clearPuzzle(void) {
    sqedit->clearPuzzle();
  }

  void setColor(unsigned int num) {
    sqedit->setColor(num);
  }

  void deactivate(void) {
    sqedit->deactivate();
  }

  void activate(void) {
    sqedit->activate();
  }

  void editSymmetries(int syms) {
    sqedit->setTool(syms);
  }

  void editChoice(gridEditor_c::enTask c) {
    sqedit->setTask(c);
  }

  void editType(int type) {
    sqedit->setEditType(type);
  }

  virtual void getMinSize(int *width, int *height) const {
    *width = 0;
    ((LFlatButton_c*)this)->measure_label(*width, *height);
    *width += 4;
    *height += 4;
  }
};

// the transform group
class TransformButtons : public layouter_c {

  pixmapList_c pm;

public:

  TransformButtons(int x, int y, int w, int h);

  void cb_Press(long button) { do_callback(this, button); }
};

// the change size group
class ChangeSize : public layouter_c {

  LFl_Roller* SizeX;
  LFl_Roller* SizeY;
  LFl_Roller* SizeZ;

  Fl_Int_Input* SizeOutX;
  Fl_Int_Input* SizeOutY;
  Fl_Int_Input* SizeOutZ;

  Fl_Check_Button * ConnectX;
  Fl_Check_Button * ConnectY;
  Fl_Check_Button * ConnectZ;

  void calcNewSizes(int ox, int oy, int oz, int *nx, int *ny, int *nz);

public:

  ChangeSize(int w, int y, int w, int h);

  void cb_roll(long dir);
  void cb_input(long dir);

  int getX(void) { return (int)SizeX->value(); }
  int getY(void) { return (int)SizeY->value(); }
  int getZ(void) { return (int)SizeZ->value(); }

  void setXYZ(long x, long y, long z);
};

// the class that contains the tool tab
class ToolTab : public LFl_Tabs {

  ChangeSize * changeSize;
  LFl_Check_Button * toAll;
  puzzle_c * puzzle;
  unsigned int shape;
  pixmapList_c pm;

public:

  ToolTab(int x, int y, int w, int h);

  void setVoxelSpace(puzzle_c * puz, unsigned int sh);

  void cb_size(void);
  void cb_transform(long task);

  bool operationToAll(void) { return toAll->value() != 0; }
};

class BlockListGroup : public Fl_Group {

  Fl_Slider * Slider;
  BlockList * List;
  int callbackReason;

public:

  BlockListGroup(int x, int y, int w, int h, BlockList * l);

  void cb_slider(void) { List->setShift((int)Slider->value()); }
  void cb_list(void);

  int getReason(void) { return callbackReason; }
};

class LBlockListGroup_c : public BlockListGroup, public layoutable_c {

  public:

  LBlockListGroup_c(int x, int y, int w, int h, BlockList * l) : BlockListGroup(0, 0, 100, 100, l), layoutable_c(x, y, w, h) {}

  virtual void getMinSize(int *width, int *height) const {
    *width = 30;
    *height = 20;
  }
};

class ConstraintsGroup : public Fl_Group {

  Fl_Slider * Slider;
  ColorConstraintsEdit * List;
  int callbackReason;

public:

  ConstraintsGroup(int x, int y, int w, int h, ColorConstraintsEdit * l);

  void cb_slider(void) { List->setShift((int)Slider->value()); }
  void cb_list(void);

  int getReason(void) { return callbackReason; }
};

class LConstraintsGroup_c : public ConstraintsGroup, public layoutable_c {

  public:

  LConstraintsGroup_c(int x, int y, int w, int h, ColorConstraintsEdit * l) : ConstraintsGroup(0, 0, 100, 100, l), layoutable_c(x, y, w, h) {}

  virtual void getMinSize(int *width, int *height) const {
    *width = 30;
    *height = 20;
  }

};

// the groups with the 3d view and the zoom slider
class View3dGroup : public Fl_Group {

  voxelDrawer_c * View3D;
  Fl_Slider * slider;

public:

  View3dGroup(int x, int y, int w, int h, const guiGridType_c * ggt);

  void newGridType(const guiGridType_c * ggt);

  void cb_slider(void);

  void showNothing(void) { View3D->clearSpaces(); }
  void showSingleShape(const puzzle_c * puz, unsigned int shapeNum);
  void showProblem(const puzzle_c * puz, unsigned int probNum, unsigned int selShape);
  void showAssembly(const puzzle_c * puz, unsigned int probNum, unsigned int solNum);
  void showPlacement(const puzzle_c * puz, unsigned int probNum, unsigned int piece, unsigned char trans, int x, int y, int z);
  void showAssemblerState(const puzzle_c * puz, unsigned int probNum, const assembly_c * assm) {
    View3D->showAssemblerState(puz, probNum, assm);
  }


  void updatePositions(piecePositions_c *shifting);
  void updateVisibility(PieceVisibility * pcvis);
  void showColors(const puzzle_c * puz, bool show);

  void setMarker(int x1, int y1, int x2, int y2, int z, int type) { View3D->setMarker(x1, y1, x2, y2, z, type); }
  void hideMarker(void) { View3D->hideMarker(); }
  void useLightning(bool val) { View3D->useLightning(val); }

  double getZoom(void) { return slider->value(); }
  void setZoom(double v) { slider->value(v); cb_slider(); }

  void redraw(void) { View3D->redraw(); }

  voxelDrawer_c * getView(void) { return View3D; }
};

// a widget to separate 2 groups

class Separator : public Fl_Group {

public:

  Separator(int x, int y, int w, int h, const char * label, bool button);
};

class LSeparator_c : public Fl_Group, public layoutable_c  {

public:

  LSeparator_c(int x, int y, int w, int h, const char * label, bool button);

  virtual void getMinSize(int *width, int *height) const {
    *width = 10;
    *height = 10;
  }
};

// a group that can contain only buttons and one button is
// pressed while others are not
class ButtonGroup : public layouter_c {

  unsigned int currentButton;

public:

  ButtonGroup(int x, int y, int w, int h);

  Fl_Button * addButton(void);

  void cb_Push(Fl_Button * btn);

  unsigned int getSelected(void) { return currentButton; }
  void select(int num);
};

class ResultViewer : public Fl_Box, public layoutable_c {

private:

  puzzle_c * puzzle;
  unsigned int problem;
  Fl_Color bg;

public:

  ResultViewer(int x, int y, int w, int h, puzzle_c * p);
  void setPuzzle(puzzle_c * p, unsigned int prob);
//  void setcontent(void);
  void draw(void);

  virtual void getMinSize(int *width, int *height) const {
    *width = 4;
    *height = 4;
  }

};

// a status line containing text and a button to toggle
// between coloured and normal view
class StatusLine : public Fl_Group {

private:

  Fl_Check_Button * colors;
  Fl_Box * text;

public:

  StatusLine(int x, int y, int w, int h);

  void setText(const char * t);
  bool useColors(void) { return colors->value() != 0; }
  void callback(Fl_Callback* fkt, void * dat) { colors->callback(fkt, dat); }
};

// this window is used to display assert messages
class assertWindow : public Fl_Double_Window {

public:

  assertWindow(const char * text, assert_exception * a);

};

// a simple window containing a multi line input
class multiLineWindow : public Fl_Double_Window {

    Fl_Multiline_Input * inp;
    bool _saveChanges;

  public:
    multiLineWindow(const char * title, const char *label, const char *deflt = 0);

    const char * getText(void) { return inp->value(); }

    void hide(bool save) {
      _saveChanges = save;
      Fl_Double_Window::hide();
    }

    bool saveChanges(void) { return _saveChanges; }
};

class ProgressBar : public Fl_Progress {
  public:

    ProgressBar(int x, int y, int w, int h) : Fl_Progress(x, y, w, h) {}

    void draw(void);
};

class LProgressBar_c : public ProgressBar, public layoutable_c {
  public:

    LProgressBar_c(int x, int y, int w, int h) : ProgressBar(0, 0, 10, 10), layoutable_c(x, y, w, h) {}

    virtual void getMinSize(int *width, int *height) const {
      *width = 20;
      *height = 20;
    }
};

#endif
