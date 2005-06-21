/* Burr Solver
 * Copyright (C) 2003-2005  Andreas R�ver
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


#include "MainWindow.h"

#include "config.h"

#ifdef HAVE_FLU
#include <FLU/Flu_File_Chooser.h>
#endif

#include <FL/Fl_Color_Chooser.H>

#include <xmlwrapp/xmlwrapp.h>

#include "gzstream.h"

static UserInterface * ui;

static const char * FileSelection(const char * title) {
#ifdef HAVE_FLU
    return flu_file_chooser(title, "*.xmpuzzle", "");
#else    
    return fl_file_chooser(title, "*.xmpuzzle", "");
#endif
}

static void cb_AddColor_stub(Fl_Widget* o, void* v) { ui->cb_AddColor(); }
void UserInterface::cb_AddColor(void) {

  unsigned char r, g, b;

  if (fl_color_chooser("New color", r, g, b)) {
    puzzle->addColor(r, g, b);
    colorSelector->redraw();
    changed = true;
  }
}


static void cb_RemoveColor_stub(Fl_Widget* o, void* v) { ui->cb_RemoveColor(); }
void UserInterface::cb_RemoveColor(void) {

  if (colorSelector->getSelection() == 0)
    fl_message("Can not delete the Neutral color, this color has to be there");
  else {
    puzzle->removeColor(colorSelector->getSelection());
    colorSelector->redraw();
    changed = true;
  }
}


static void cb_NewShape_stub(Fl_Widget* o, void* v) { ui->cb_NewShape(); }
void UserInterface::cb_NewShape(void) {

  // FIXME, all edit operations should be blocked while solving and should remove all
  // solutions, when they do exists

  PcSel->setSelection(puzzle->addShape(6, 6, 6));
  PcSel->redraw();
  changed = true;
}


static void cb_DeleteShape_stub(Fl_Widget* o, void* v) { ui->cb_DeleteShape(); }
void UserInterface::cb_DeleteShape(void) {

  unsigned int current = PcSel->getSelection();

  if (current < puzzle->shapeNumber()) {

    puzzle->removeShape(current);

    while (current >= puzzle->shapeNumber())
      current--;

    activateShape(current);

    changed = true;

  } else

    fl_message("No shape to delete selected!");

}


static void cb_CopyShape_stub(Fl_Widget* o, void* v) { ui->cb_CopyShape(); }
void UserInterface::cb_CopyShape(void) {

  unsigned int current = PcSel->getSelection();

  if (current < puzzle->shapeNumber()) {

    PcSel->setSelection(puzzle->addShape(new pieceVoxel_c(puzzle->getShape(current))));
    changed = true;

  } else

    fl_message("No shape to copy selected!");

}


static void cb_TaskSelectionTab_stub(Fl_Widget* o, void* v) { ui->cb_TaskSelectionTab((Fl_Tabs*)o); }
void UserInterface::cb_TaskSelectionTab(Fl_Tabs* o) {

  if (o->value() == TabPieces) {
    activateShape(PcSel->getSelection());
  } else if(o->value() == TabProblems) {
    if (problemSelector->getSelection() < puzzle->problemNumber())
      activateProblem(problemSelector->getSelection());
  } else if(o->value() == TabSolve) {
    if ((solutionProblem->getSelection() < puzzle->problemNumber()) &&
        (SolutionSel->value() < puzzle->probSolutionNumber(solutionProblem->getSelection()))) {
      activateSolution(solutionProblem->getSelection(), int(SolutionSel->value()));
    }
    updateSolutionStats();
  }
}


static void cb_TransformPiece_stub(Fl_Widget* o, long v) { ui->cb_TransformPiece(); }
void UserInterface::cb_TransformPiece(void) {
  activateShape(PcSel->getSelection());
}


static void cb_PcSel_stub(Fl_Widget* o, long v) { ui->cb_PcSel(v); }
void UserInterface::cb_PcSel(long reason) {

  switch(reason) {
  case PieceSelector::RS_CHANGEDSELECTION:
    activateShape(PcSel->getSelection());
    break;
  }
}

static void cb_SolProbSel_stub(Fl_Widget* o, long v) { ui->cb_SolProbSel(v); }
void UserInterface::cb_SolProbSel(long reason) {

  switch(reason) {
  case ProblemSelector::RS_CHANGEDSELECTION:

    updateSolutionStats();
    activateSolution(solutionProblem->getSelection(), (int)SolutionSel->value());
    break;
  }
}

static void cb_ColSel_stub(Fl_Widget* o, long v) { ui->cb_ColSel(v); }
void UserInterface::cb_ColSel(long reason) {

  switch(reason) {
  case PieceSelector::RS_CHANGEDSELECTION:
    pieceEdit->setColor(colorSelector->getSelection());
    break;
  }
}

static void cb_ProbSel_stub(Fl_Widget* o, long v) { ui->cb_ProbSel(v); }
void UserInterface::cb_ProbSel(long reason) {

  switch(reason) {
  case PieceSelector::RS_CHANGEDSELECTION:
    PiecesCountList->setPuzzle(puzzle, problemSelector->getSelection());
    colconstrList->setPuzzle(puzzle, problemSelector->getSelection());
    problemResult->setPuzzle(puzzle, problemSelector->getSelection());
    activateProblem(problemSelector->getSelection());
    break;
  }
}

static void cb_pieceEdit_stub(Fl_Widget* o, void* v) { ui->cb_pieceEdit((VoxelEditGroup*)o); }
void UserInterface::cb_pieceEdit(VoxelEditGroup* o) {

  switch (o->getReason()) {
  case SquareEditor::RS_MOUSEMOVE:
    if (o->getMouse())
      View3D->setMarker(o->getMouseX(), o->getMouseY(), o->getMouseZ());
    else
      View3D->hideMarker();
    break;
  case SquareEditor::RS_CHANGESQUARE:
    changed = true;
    break;
  }

  View3D->redraw();
}

static void cb_NewProblem_stub(Fl_Widget* o, void* v) { ui->cb_NewProblem(); }
void UserInterface::cb_NewProblem(void) {

  const char * name = fl_input("Enter name for the new problem", "Problem");

  if (name) {
    unsigned int prob = puzzle->addProblem();
    puzzle->probSetName(prob, name);
    changed = true;
    problemSelector->redraw();
    PcVis->setPuzzle(puzzle, solutionProblem->getSelection());
    activateProblem(prob);
  }
}

static void cb_DeleteProblem_stub(Fl_Widget* o, void* v) { ui->cb_DeleteProblem(); }
void UserInterface::cb_DeleteProblem(void) {

  if (problemSelector->getSelection() < puzzle->problemNumber()) {

    puzzle->removeProblem(problemSelector->getSelection());
    problemSelector->redraw();
    changed = true;
    activateProblem(problemSelector->getSelection());
  }
}

static void cb_CopyProblem_stub(Fl_Widget* o, void* v) { ui->cb_CopyProblem(); }
void UserInterface::cb_CopyProblem(void) {

  if (problemSelector->getSelection() < puzzle->problemNumber()) {

    unsigned int prob = puzzle->copyProblem(problemSelector->getSelection());

    puzzle->probSetName(prob, puzzle->probGetName(prob) + "_cp");

    problemSelector->redraw();
    changed = true;
    activateProblem(prob);
  }
}

static void cb_ShapeToResult_stub(Fl_Widget* o, void* v) { ui->cb_ShapeToResult(); }
void UserInterface::cb_ShapeToResult(void) {

  if (problemSelector->getSelection() >= puzzle->problemNumber()) {
    fl_message("First create a problem");
    return;
  }

  puzzle->probSetResult(problemSelector->getSelection(), shapeAssignmentSelector->getSelection());
  problemResult->setPuzzle(puzzle, problemSelector->getSelection());
  activateProblem(problemSelector->getSelection());

  changed = true;
}

static void cb_AddShapeToProblem_stub(Fl_Widget* o, void* v) { ui->cb_AddShapeToProblem(); }
void UserInterface::cb_AddShapeToProblem(void) {

  if (problemSelector->getSelection() >= puzzle->problemNumber()) {
    fl_message("First create a problem");
    return;
  }

  unsigned int prob = problemSelector->getSelection();

  changed = true;
  PiecesCountList->redraw();

  // first see, if there is already a the selected shape inside
  for (unsigned int i = 0; i < puzzle->probShapeNumber(prob); i++)
    if (puzzle->probGetShape(prob, i) == shapeAssignmentSelector->getSelection()) {
      puzzle->probSetShapeCount(prob, i, puzzle->probGetShapeCount(prob, i) + 1);
      PcVis->setPuzzle(puzzle, solutionProblem->getSelection());
      return;
    }

  puzzle->probAddShape(prob, shapeAssignmentSelector->getSelection(), 1);
  activateProblem(problemSelector->getSelection());
  PcVis->setPuzzle(puzzle, solutionProblem->getSelection());
}

static void cb_RemoveShapeFromProblem_stub(Fl_Widget* o, void* v) { ui->cb_RemoveShapeFromProblem(); }
void UserInterface::cb_RemoveShapeFromProblem(void) {

  if (problemSelector->getSelection() >= puzzle->problemNumber()) {
    fl_message("First create a problem");
    return;
  }

  unsigned int prob = problemSelector->getSelection();

  // first see, find the shape, and only if there is one, we decrement its count out remove it
  for (unsigned int i = 0; i < puzzle->probShapeNumber(prob); i++)
    if (puzzle->probGetShape(prob, i) == shapeAssignmentSelector->getSelection()) {
      if (puzzle->probGetShapeCount(prob, i) == 1)
        puzzle->probRemoveShape(prob, i);
      else
        puzzle->probSetShapeCount(prob, i, puzzle->probGetShapeCount(prob, i) - 1);

      changed = true;
      PiecesCountList->redraw();
      PcVis->setPuzzle(puzzle, solutionProblem->getSelection());
    }

  activateProblem(problemSelector->getSelection());
}

static void cb_AllowColor_stub(Fl_Widget* o, void* v) { ui->cb_AllowColor(); }
void UserInterface::cb_AllowColor(void) {

  if (problemSelector->getSelection() >= puzzle->problemNumber()) {
    fl_message("First create a problem");
    return;
  }

  if (colconstrList->GetSortByResult())
    puzzle->probAllowPlacement(problemSelector->getSelection(),
                               colorAssignmentSelector->getSelection()+1,
                               colconstrList->getSelection()+1);
  else
    puzzle->probAllowPlacement(problemSelector->getSelection(),
                               colconstrList->getSelection()+1,
                               colorAssignmentSelector->getSelection()+1);
  colconstrList->redraw();
  changed = true;
}

static void cb_DisallowColor_stub(Fl_Widget* o, void* v) { ui->cb_DisallowColor(); }
void UserInterface::cb_DisallowColor(void) {

  if (problemSelector->getSelection() >= puzzle->problemNumber()) {
    fl_message("First create a problem");
    return;
  }

  if (colconstrList->GetSortByResult())
    puzzle->probDisallowPlacement(problemSelector->getSelection(),
                                  colorAssignmentSelector->getSelection()+1,
                                  colconstrList->getSelection()+1);
  else
    puzzle->probDisallowPlacement(problemSelector->getSelection(),
                                  colconstrList->getSelection()+1,
                                  colorAssignmentSelector->getSelection()+1);
  colconstrList->redraw();
  changed = true;
}

static void cb_CCSortByResult_stub(Fl_Widget* o, void* v) { ui->cb_CCSort(1); }
static void cb_CCSortByPiece_stub(Fl_Widget* o, void* v) { ui->cb_CCSort(0); }
void UserInterface::cb_CCSort(bool byResult) {
  colconstrList->SetSortByResult(byResult);

  if (byResult) {
    BtnColSrtPc->activate();
    BtnColSrtRes->deactivate();
  } else {
    BtnColSrtPc->deactivate();
    BtnColSrtRes->activate();
  }
}

static void cb_BtnStart_stub(Fl_Widget* o, void* v) { ui->cb_BtnStart(); }
void UserInterface::cb_BtnStart(void) {

  if (solutionProblem->getSelection() >= puzzle->problemNumber()) {
    fl_message("Create a problem first");
    return;
  }

  if ((puzzle->probGetResult(solutionProblem->getSelection()) < 0) ||
      (puzzle->probGetResult(solutionProblem->getSelection()) > puzzle->shapeNumber())) {
    fl_message("A result shape must be defined");
    return;
  }

  assert(assmThread == 0);

  unsigned int prob = solutionProblem->getSelection();

  puzzle->probRemoveAllSolutions(prob);

  if (SolveDisasm->value() != 0)
    assmThread = new assemblerThread(puzzle, prob, assemblerThread::SOL_DISASM);
  else
    assmThread = new assemblerThread(puzzle, prob, assemblerThread::SOL_SAVE_ASM);

  assmThread->start();

  BtnStart->deactivate();
  BtnCont->deactivate();
  BtnStop->activate();
}

static void cb_BtnCont_stub(Fl_Widget* o, void* v) { ui->cb_BtnCont(); }
void UserInterface::cb_BtnCont(void) {

  unsigned int prob = solutionProblem->getSelection();

  if (prob >= puzzle->problemNumber()) {
    fl_message("First create a problem");
    return;
  }

  assert(assmThread == 0);

  if (SolveDisasm->value() != 0)
    assmThread = new assemblerThread(puzzle, prob, assemblerThread::SOL_DISASM);
  else
    assmThread = new assemblerThread(puzzle, prob, assemblerThread::SOL_SAVE_ASM);

  assmThread->start();

  BtnStart->deactivate();
  BtnCont->deactivate();
  BtnStop->activate();
}

static void cb_BtnStop_stub(Fl_Widget* o, void* v) { ui->cb_BtnStop(); }
void UserInterface::cb_BtnStop(void) {

  assert(assmThread);

  assmThread->stop();
}

static void cb_SolutionSel_stub(Fl_Widget* o, void* v) { ui->cb_SolutionSel((Fl_Value_Slider*)o); }
void UserInterface::cb_SolutionSel(Fl_Value_Slider* o) {
  activateSolution(solutionProblem->getSelection(), int(o->value()));
}

static void cb_SolutionAnim_stub(Fl_Widget* o, void* v) { ui->cb_SolutionAnim((Fl_Value_Slider*)o); }
void UserInterface::cb_SolutionAnim(Fl_Value_Slider* o) {
  o->take_focus();
  if (disassemble) {
    disassemble->setStep(o->value());
    View3D->updatePositions(disassemble);
  }
}

static void cb_PcVis_stub(Fl_Widget* o, void* v) { ui->cb_PcVis(); }
void UserInterface::cb_PcVis(void) {
  View3D->updateVisibility(PcVis);
}

static void cb_New_stub(Fl_Widget* o, void* v) { ui->cb_New(); }
void UserInterface::cb_New(void) {

  if (threadStopped()) {

    if (changed)
      if (fl_ask("Puzzle changed are you shure?") == 0)
        return;

    ReplacePuzzle(new puzzle_c());

    if (fname) {
      delete [] fname;
      fname = 0;
    }

    changed = false;

    activateShape(0);
  }
}

static void cb_Load_stub(Fl_Widget* o, void* v) { ui->cb_Load(); }
void UserInterface::cb_Load(void) {

  if (threadStopped()) {

    if (changed)
      if (fl_ask("Puzzle changed are you shure?") == 0)
        return;

    const char * f = FileSelection("Load Puzzle");

    tryToLoad(f);
  }
}

static void cb_Save_stub(Fl_Widget* o, void* v) { ui->cb_Save(); }
void UserInterface::cb_Save(void) {

  if (threadStopped()) {

    if (!fname)
      cb_SaveAs();

    else {
      ogzstream ostr(fname);
    
      if (ostr)
        ostr << puzzle->save();
    
      if (!ostr)
        fl_alert("puzzle NOT saved!!");
      else
        changed = false;
    }
  }
}

static void cb_SaveAs_stub(Fl_Widget* o, void* v) { ui->cb_SaveAs(); }
void UserInterface::cb_SaveAs(void) {

  if (threadStopped()) {
    const char * f = FileSelection("Save Puzzle as");
  
    if (f) {
  
      ofstream ostr(f);
  
      if (ostr)
        ostr << puzzle->save();
  
      if (!ostr)
        fl_alert("puzzle NOT saved!!!");
      else
        changed = false;
  
      if (fname) delete [] fname;
      fname = new char[strlen(f)+1];
      strcpy(fname, f);
  
      char nm[300];
      snprintf(nm, 299, "BurrTools - %s", fname);
      mainWindow->label(nm);
    }
  }
}

static void cb_Quit_stub(Fl_Widget* o, void* v) { ui->cb_Quit(); }
void UserInterface::cb_Quit(void) {
  if (changed)
    if (fl_ask("Puzzle changed are you shure?") == 0)
      return;
  mainWindow->hide();
}







bool UserInterface::threadStopped(void) {

  if (assmThread) {

    fl_message("Stop solving process first!");
    return false;
  }

  return true;
}

void UserInterface::tryToLoad(const char * f) {

  if (f) {

    xml::tree_parser parser(f);

    if (!parser) {

      char txt[500];

      snprintf(txt, 500, "Error parsing xml puzzle file, xml-syntax incorrect:\n %s", parser.get_error_message().c_str());
      fl_message(txt);

      return;
    }

    xml::document &doc = parser.get_document();

    puzzle_c * newPuzzle;

    try {
      newPuzzle = new puzzle_c(doc.get_root_node());
    }

    catch (load_error e) {
      fl_message(e.getText());
      cout << e.getNode();
      return;
    }

    if (fname) delete [] fname;
    fname = new char[strlen(f)+1];
    strcpy(fname, f);

    char nm[300];
    snprintf(nm, 299, "BurrTools - %s", fname);
    mainWindow->label(nm);

    ReplacePuzzle(newPuzzle);

    TaskSelectionTab->value(TabPieces);
    activateShape(PcSel->getSelection());

    changed = false;
  }
}

void UserInterface::ReplacePuzzle(puzzle_c * NewPuzzle) {

    // inform everybody
    colorSelector->setPuzzle(NewPuzzle);
    PcSel->setPuzzle(NewPuzzle);
    pieceEdit->setPuzzle(NewPuzzle, 0);
    problemSelector->setPuzzle(NewPuzzle);
    colorAssignmentSelector->setPuzzle(NewPuzzle);
    colconstrList->setPuzzle(NewPuzzle, 0);
    problemResult->setPuzzle(NewPuzzle, 0);
    shapeAssignmentSelector->setPuzzle(NewPuzzle);
    PiecesCountList->setPuzzle(NewPuzzle, 0);
    solutionProblem->setPuzzle(NewPuzzle);
    PcVis->setPuzzle(NewPuzzle, 0);

    SolutionSel->value(0);
    SolutionAnim->value(0);

    delete puzzle;
    puzzle = NewPuzzle;
}


Fl_Menu_Item UserInterface::menu_MainMenu[] = {
  {"New",     0, cb_New_stub, 0, 0, 0, 0, 14, 56},
  {"Load",    0, cb_Load_stub, 0, 0, 0, 0, 14, 56},
  {"Save",    0, cb_Save_stub, 0, 0, 0, 0, 14, 56},
  {"Save as", 0, cb_SaveAs_stub, 0, 128, 0, 0, 14, 56},
  {"Quit",    0, cb_Quit_stub, 0, 0, 3, 0, 14, 1},
  {0}
};


void UserInterface::show(int argn, char ** argv) {
  mainWindow->show();

  if (argn == 2)
    tryToLoad(argv[1]);
}

void UserInterface::activateClear(void) {
  View3D->showNothing();
  pieceEdit->clearPuzzle();
  pieceTools->setVoxelSpace(0);

  SolutionEmpty = true;
}

void UserInterface::activateShape(unsigned int number) {

  if ((number < puzzle->shapeNumber())) {

    pieceVoxel_c * p = puzzle->getShape(number);

    View3D->showSingleShape(puzzle, number);
    pieceEdit->setPuzzle(puzzle, number);
    pieceTools->setVoxelSpace(p);

    PcSel->setSelection(number);

  } else {

    View3D->showNothing();
    pieceEdit->clearPuzzle();
    pieceTools->setVoxelSpace(0);
  }

  SolutionEmpty = true;
}

void UserInterface::activateProblem(unsigned int prob) {

  View3D->showProblem(puzzle, prob);

  SolutionEmpty = true;
}

void UserInterface::activateSolution(unsigned int prob, unsigned int num) {

  if (disassemble) {
    delete disassemble;
    disassemble = 0;
  }

  if ((prob < puzzle->problemNumber()) && (num < puzzle->probSolutionNumber(prob))) {

    unsigned int shapeNumber = puzzle->probShapeNumber(prob);

    int * pcNum = new int[shapeNumber];

    unsigned int piece = 0;

    for (unsigned int i = 0; i < shapeNumber; i++) {
      pcNum[i] = puzzle->probGetShapeCount(prob, i);
      for (int j = 0; j < pcNum[i]; j++) {
        colors[2*piece] = i;
        colors[2*piece+1] = j;

        piece++;
      }
    }

    PcVis->setPuzzle(puzzle, prob);

    if (puzzle->probGetDisassembly(prob, num)) {
      SolutionAnim->show();
      SolutionAnim->range(0, puzzle->probGetDisassembly(prob, num)->sumMoves());

      SolutionsInfo->show();

      MovesInfo->show();
      MovesInfo->value(puzzle->probGetDisassembly(prob, num)->sumMoves());

      disassemble = new DisasmToMoves(puzzle->probGetDisassembly(prob, num), puzzle->probGetAssembly(prob, num),
                                      2*puzzle->probGetResultShape(prob)->getBiggestDimension());
      disassemble->setStep(SolutionAnim->value());

      View3D->showAssembly(puzzle, prob, num);
      View3D->updatePositions(disassemble);

    } else {

      SolutionAnim->range(0, 0);
      SolutionAnim->hide();
      MovesInfo->value(0);
      MovesInfo->hide();

      View3D->showAssembly(puzzle, prob, num);
    }

    SolutionEmpty = false;

  } else {

    View3D->showNothing();
    SolutionEmpty = true;

    SolutionAnim->hide();
    MovesInfo->hide();

    PcVis->setPuzzle(puzzle, solutionProblem->getSelection());

  }
}

void UserInterface::updateSolutionStats(void) {

  unsigned int prob = solutionProblem->getSelection();

  if (prob < puzzle->problemNumber()) {

    assembler_c * assm = puzzle->probGetAssembler(prob);
  
    float finished = (assm) ? assm->getFinished() : 0;
  
    SolvingProgress->value(100*finished);
  
    unsigned long numSol = puzzle->probSolutionNumber(prob);
  
    if (numSol > 0) {
  
      SolutionSel->show();
      SolutionsInfo->show();
      OutputSolutions->show();
  
      SolutionSel->range(0, numSol-1);
      SolutionsInfo->value(numSol);
      OutputSolutions->value(numSol);
  
      if (SolutionEmpty && (numSol > 0))
        activateSolution(solutionProblem->getSelection(), 0);
  
    } else {
  
      SolutionSel->range(0, 0);
      SolutionSel->hide();
      SolutionsInfo->hide();
      OutputSolutions->hide();
      SolutionAnim->hide();
      MovesInfo->hide();
    }

//    OutputIterations->value(puzzle->probGetAssembler(solutionProblem->getSelection())->getIterations());
//    OutputAssemblies->value(assmThread->getAssemblies());
  } else {

    SolutionSel->hide();
    SolutionsInfo->hide();
    OutputSolutions->hide();
    SolutionAnim->hide();
    MovesInfo->hide();
  }

  if (assmThread) {

    switch(assmThread->currentAction()) {
    case assemblerThread::ACT_PREPARATION:
      OutputActivity->value("prep.");
      break;
    case assemblerThread::ACT_REDUCE:
      if (puzzle->probGetAssembler(prob)) {
        char tmp[20];
        snprintf(tmp, 20, "red. %i", puzzle->probGetAssembler(prob)->getReducePiece());
        OutputActivity->value(tmp);
      } else
        OutputActivity->value("red.");
      break;
    case assemblerThread::ACT_ASSEMBLING:
      OutputActivity->value("assm.");
      break;
    case assemblerThread::ACT_DISASSEMBLING:
      OutputActivity->value("disassm.");
      break;
    case assemblerThread::ACT_PAUSING:
      OutputActivity->value("pause");
      break;
    case assemblerThread::ACT_FINISHED:
      OutputActivity->value("finished");
      break;
    }

    if (assmThread->getProblem() == solutionProblem->getSelection()) {

      BtnStart->deactivate();
      BtnCont->deactivate();
      BtnStop->activate();

    } else {

      BtnStart->deactivate();
      BtnCont->deactivate();
      BtnStop->deactivate();
    }

  } else if (prob < puzzle->problemNumber()) {

    assembler_c * assm = puzzle->probGetAssembler(prob);

    if (!assm)
      OutputActivity->value("nothing");
    else {

      if (assm->getFinished() >= 1)
        OutputActivity->value("finished");
      else
        OutputActivity->value("pause");
    }

    BtnStart->activate();
    BtnCont->activate();
    BtnStop->deactivate();

  } else {

    BtnStart->deactivate();
    BtnCont->deactivate();
    BtnStop->deactivate();
  }
}

void UserInterface::update(void) {

  if (assmThread) {

    // check, if the thread has stopped, if so then delete the object
    if ((assmThread->currentAction() == assemblerThread::ACT_PAUSING) ||
        (assmThread->currentAction() == assemblerThread::ACT_FINISHED)) {

      delete assmThread;
      assmThread = 0;
    }

    // update the window, either when the thread stopped and so the buttons need to
    // be updated, or then the thread works for the currently selected problem
    if (!assmThread || assmThread->getProblem() == solutionProblem->getSelection())
      updateSolutionStats();
  }
}

#define SZ_WINDOW_X 540                        // initial size of the window
#define SZ_WINDOW_Y 488
#define SZ_MENU_Y 25                           // hight of the menu
#define SZ_TOOL_X 190                          // initial width of the toolbar
#define SZ_TAB_Y 20                            // hight of the tabs in a tab
#define SZ_GAP 5                               // gap between elements
#define SZ_CONTENT_START_Y SZ_MENU_Y           // y start of the content area
#define SZ_CONTENT_Y (SZ_WINDOW_Y - SZ_MENU_Y) // initial hight of the content of the window
#define SZ_3DAREA_X (SZ_WINDOW_X - SZ_TOOL_X)
#define SZ_BUTTON_Y 20
#define SZ_TEXT_Y 15
#define SZ_SEPARATOR_Y 10

void UserInterface::CreateShapeTab(int x, int y, int w, int h) {

  TabPieces = new Fl_Group(x, y, w, h, "Shapes");
  TabPieces->tooltip("Edit shapes");

  x += SZ_GAP; y++;  w -= 2*SZ_GAP; h -= SZ_GAP + 1;

  Fl_Group * tile = new Fl_Tile(x, y, w, h);

  // calculate hight of different groups
  const int numGroups = 3;

  const int pieceFixedHight = SZ_SEPARATOR_Y + SZ_BUTTON_Y + SZ_GAP;
  const int colorsFixedHight = SZ_SEPARATOR_Y + SZ_BUTTON_Y + SZ_GAP;
  const int editFixedHight = SZ_SEPARATOR_Y + 135 + SZ_GAP;

  int hi = h - pieceFixedHight - colorsFixedHight - editFixedHight;

  assert(hi > 30);

  int pieceHight = hi/numGroups + pieceFixedHight;
  int colorsHight = hi/numGroups + colorsFixedHight;
  int editHight = hi - (hi/numGroups) * (numGroups-1) + editFixedHight;

  {
    int lh = colorsHight;

    Fl_Group* group = new Fl_Group(x, y, w, lh);
    group->box(FL_FLAT_BOX);
    new Separator(x, y, w, SZ_SEPARATOR_Y, "Colors", false);
    y += SZ_SEPARATOR_Y;
    lh -= SZ_SEPARATOR_Y;

    int bw = (w - SZ_GAP) / 2;
    {
      Fl_Group * o = new Fl_Group(x, y, bw+SZ_GAP, SZ_BUTTON_Y);
      o->resizable(new FlatButton(x          , y, bw, SZ_BUTTON_Y, "Add", "Add another color", cb_AddColor_stub));
      o->end();
    }
    {
      Fl_Group * o = new Fl_Group(x+bw+SZ_GAP, y, bw+SZ_GAP, SZ_BUTTON_Y);
      o->resizable(new FlatButton(x+SZ_GAP+bw, y, bw, SZ_BUTTON_Y, "Remove", "Remove selected color", cb_RemoveColor_stub));
      o->end();
    }
    y += SZ_BUTTON_Y + SZ_GAP;
    lh -= SZ_BUTTON_Y + SZ_GAP;

    colorSelector = new ColorSelector(x, y, w, lh, puzzle, true);
    Fl_Group * colGroup = new BlockListGroup(x, y, w, lh, colorSelector);
    colGroup->callback(cb_ColSel_stub);

    y += lh;

    group->resizable(colorSelector);
    group->end();
  }

  {
    int lh = pieceHight;

    Fl_Group* group = new Fl_Group(x, y, w, lh);
    group->box(FL_FLAT_BOX);
    new Separator(x, y, w, SZ_SEPARATOR_Y, "Shapes", true);
    y += SZ_SEPARATOR_Y;
    lh -= SZ_SEPARATOR_Y;

    int bw = (w - 2*SZ_GAP) / 3;
    {
      Fl_Group * o = new Fl_Group(x+0*SZ_GAP+0*bw, y, bw+SZ_GAP, SZ_BUTTON_Y);
      o->resizable(new FlatButton(x+0*SZ_GAP+0*bw, y, bw, SZ_BUTTON_Y, "New", "Add another piece", cb_NewShape_stub));
      o->end();
    }
    {
      Fl_Group * o = new Fl_Group(x+1*SZ_GAP+1*bw, y, bw+SZ_GAP, SZ_BUTTON_Y);
      o->resizable(new FlatButton(x+1*SZ_GAP+1*bw, y, bw, SZ_BUTTON_Y, "Delete", "Delete selected piece", cb_DeleteShape_stub));
      o->end();
    }
    {
      Fl_Group * o = new Fl_Group(x+2*SZ_GAP+2*bw, y, bw+SZ_GAP, SZ_BUTTON_Y);
      o->resizable(new FlatButton(x+2*SZ_GAP+2*bw, y, w-2*SZ_GAP-2*bw, SZ_BUTTON_Y, "Copy", "Copy selected piece", cb_CopyShape_stub));
      o->end();
    }
    y += SZ_BUTTON_Y + SZ_GAP;
    lh -= SZ_BUTTON_Y + SZ_GAP;

    PcSel = new PieceSelector(x, y, w, lh, puzzle);
    PcSel->setSelection(0xFFFF);
    Fl_Group * selGroup = new BlockListGroup(x, y, w, lh, PcSel);
    selGroup->callback(cb_PcSel_stub);

    y += lh;

    group->resizable(selGroup);
    group->end();
  }

  {
    int lh = editHight;

    Fl_Group* group = new Fl_Group(x, y, w, lh);
    group->box(FL_FLAT_BOX);

    new Separator(x, y, w, SZ_SEPARATOR_Y, "Edit", true);
    y += SZ_SEPARATOR_Y;
    lh -= SZ_SEPARATOR_Y;

    pieceTools = new ToolTab(x, y, w, 135);
    pieceTools->callback(cb_TransformPiece_stub);
    pieceTools->end();
    y += 135 + SZ_GAP;
    lh -= 135 + SZ_GAP;
  
    pieceEdit = new VoxelEditGroup(x, y, w, lh, puzzle);
    pieceEdit->callback(cb_pieceEdit_stub);
    pieceEdit->end();
    y += lh;

    group->resizable(pieceEdit);
    group->end();
  }

  tile->end();

  TabPieces->resizable(tile);
  TabPieces->end();

  Fl_Group::current()->resizable(TabPieces);
}

void UserInterface::CreateProblemTab(int x, int y, int w, int h) {

  TabProblems = new Fl_Group(x, y, w, h, "Problems");
  TabProblems->tooltip("Edit problems");
  TabProblems->hide();

  x += SZ_GAP; y++; w -= 2*SZ_GAP; h -= SZ_GAP + 1;

  Fl_Group * tile = new Fl_Tile(x, y, w, h);

  // calculate hight of different groups
  const int problemsFixedHight = SZ_SEPARATOR_Y + SZ_BUTTON_Y + SZ_GAP;
  const int colorsFixedHight = SZ_SEPARATOR_Y + SZ_GAP;
  const int matrixFixedHight = SZ_BUTTON_Y + SZ_GAP;
  const int shapesFixedHight = SZ_SEPARATOR_Y + SZ_BUTTON_Y + 2*SZ_GAP;
  const int piecesFixedHight = SZ_SEPARATOR_Y + SZ_BUTTON_Y + SZ_GAP;

  int hi = h - problemsFixedHight - colorsFixedHight - matrixFixedHight - shapesFixedHight - piecesFixedHight;

  assert(hi > 30);

  int problemsHight = hi/5 + problemsFixedHight;
  int colorsHight = hi/5 + colorsFixedHight;
  int matrixHight = hi/5 + matrixFixedHight;
  int shapesHight = hi/5 + shapesFixedHight;
  int piecesHight = hi - (hi/5) * 4 + piecesFixedHight;

  {
    int lh = problemsHight;

    Fl_Group* group = new Fl_Group(x, y, w, lh);
    group->box(FL_FLAT_BOX);
    new Separator(x, y, w, SZ_SEPARATOR_Y, "Problems", false);
    y += SZ_SEPARATOR_Y;
    lh -= SZ_SEPARATOR_Y;

    int bw = (w - 2*SZ_GAP) / 3;
    {
      Fl_Group * o = new Fl_Group(x+0*SZ_GAP+0*bw, y, bw+SZ_GAP, SZ_BUTTON_Y);
      o->resizable(new FlatButton(x+0*SZ_GAP+0*bw, y, bw, SZ_BUTTON_Y, "New", "Add another problem", cb_NewProblem_stub));
      o->end();
    }
    {
      Fl_Group * o = new Fl_Group(x+1*SZ_GAP+1*bw, y, bw+SZ_GAP, SZ_BUTTON_Y);
      o->resizable(new FlatButton(x+1*SZ_GAP+1*bw, y, bw, SZ_BUTTON_Y, "Delete", "Delete selected problem", cb_DeleteProblem_stub));
      o->end();
    }
    {
      Fl_Group * o = new Fl_Group(x+2*SZ_GAP+2*bw, y, bw+SZ_GAP, SZ_BUTTON_Y);
      o->resizable(new FlatButton(x+2*SZ_GAP+2*bw, y, w-2*SZ_GAP-2*bw, SZ_BUTTON_Y, "Copy", "Copy selected problem", cb_CopyProblem_stub));
      o->end();
    }
    y += SZ_BUTTON_Y + SZ_GAP;
    lh -= SZ_BUTTON_Y + SZ_GAP;

    problemSelector = new ProblemSelector(x, y, w, lh, puzzle);
    Fl_Group * probGroup = new BlockListGroup(x, y, w, lh, problemSelector);
    probGroup->callback(cb_ProbSel_stub);


    group->resizable(probGroup);
    group->end();

    y += lh;
  }

  {
    int lh = colorsHight;

    Fl_Group* group = new Fl_Group(x, y, w, lh);
    group->box(FL_FLAT_BOX);

    new Separator(x, y, w, SZ_SEPARATOR_Y, "Color assigment", true);
    y += SZ_SEPARATOR_Y;
    lh -= SZ_SEPARATOR_Y;

    colorAssignmentSelector = new ColorSelector(x, y, w, lh, puzzle, false);
    Fl_Group * colGroup = new BlockListGroup(x, y, w, lh-SZ_GAP, colorAssignmentSelector);

    group->resizable(colGroup);
    group->end();
      
    y += lh;
  }

  {
    int lh = matrixHight;

    Fl_Group* group = new Fl_Group(x, y, w, lh);
    group->box(FL_FLAT_BOX);

    int hw = (w - 2*SZ_GAP) / 3;

    {
      Fl_Group * o = new Fl_Group(x, y, hw+SZ_GAP, SZ_BUTTON_Y);
      BtnColSrtPc = new FlatButton(x, y, hw, SZ_BUTTON_Y, "Srt Pc", "Sort Color Constraints by Piece", cb_CCSortByPiece_stub);
      BtnColSrtPc->deactivate();
      o->resizable(BtnColSrtPc);
      o->end();
    }

    new FlatButton(x+hw+SZ_GAP     , y, hw/2, SZ_BUTTON_Y, "@-12->", "Add Color", cb_AllowColor_stub);
    new FlatButton(x+hw+SZ_GAP+hw/2, y, hw/2, SZ_BUTTON_Y, "@-18->", "Add Color", cb_DisallowColor_stub);

    {
      Fl_Group * o = new Fl_Group(x+2*hw+SZ_GAP, y, hw+SZ_GAP, SZ_BUTTON_Y);
      BtnColSrtRes = new FlatButton(x+2*hw+2*SZ_GAP, y, w-2*(hw+SZ_GAP), SZ_BUTTON_Y, "Srt Res", "Sort Color Constraints by Result", cb_CCSortByResult_stub);
      o->resizable(BtnColSrtRes);
      o->end();
    }

    y += SZ_GAP + SZ_BUTTON_Y;
    lh -= SZ_GAP + SZ_BUTTON_Y;

    colconstrList = new ColorConstraintsEdit(x, y, w, lh, puzzle);
    Fl_Group * colGroup = new ConstraintsGroup(x, y, w, lh, colconstrList);

    group->resizable(colGroup);
    group->end();

    y += lh;
  }

  {
    int lh = shapesHight;

    Fl_Group* group = new Fl_Group(x, y, w, lh);
    group->box(FL_FLAT_BOX);

    int hw = (w - SZ_GAP)/2;

    new Separator(x, y, w, SZ_SEPARATOR_Y, "Piece assigment", true);
    y += SZ_SEPARATOR_Y;
    lh -= SZ_SEPARATOR_Y;

    {
      Fl_Group * o = new Fl_Group(x,           y, hw+SZ_GAP, SZ_BUTTON_Y);
      problemResult = new ResultViewer(x, y, hw, SZ_BUTTON_Y, puzzle);
      o->resizable(problemResult);
      o->end();
    }
    {
      Fl_Group * o = new Fl_Group(x+hw+SZ_GAP, y, w-hw, SZ_BUTTON_Y);
      o->resizable(new FlatButton(x+hw+SZ_GAP, y, w-hw-SZ_GAP, SZ_BUTTON_Y, "Set Result", "Set selected shape as result", cb_ShapeToResult_stub));
      o->end();
    }

    y += SZ_BUTTON_Y + SZ_GAP;
    lh -= SZ_BUTTON_Y + SZ_GAP;

    shapeAssignmentSelector = new PieceSelector(x, y, w, lh-SZ_GAP, puzzle);
    Fl_Group * shapeGroup = new BlockListGroup(x, y, w, lh-SZ_GAP, shapeAssignmentSelector);

    group->resizable(shapeGroup);
    group->end();

    y += lh;
  }

  {
    int lh = piecesHight;

    Fl_Group* group = new Fl_Group(x, y, w, lh);
    group->box(FL_FLAT_BOX);

    int hw = (w - SZ_GAP)/2;

    {
      Fl_Group * o = new Fl_Group(x,           y, hw+SZ_GAP, SZ_BUTTON_Y);
      o->resizable(new FlatButton(x,           y, hw         , SZ_BUTTON_Y, "+1", "Add another one of the selected shape", cb_AddShapeToProblem_stub));
      o->end();
    }
    {
      Fl_Group * o = new Fl_Group(x+hw+SZ_GAP, y, w-hw, SZ_BUTTON_Y);
      o->resizable(new FlatButton(x+hw+SZ_GAP, y, w-hw-SZ_GAP, SZ_BUTTON_Y, "-1", "Remove one of the selected shapes", cb_RemoveShapeFromProblem_stub));
      o->end();
    }

    y += SZ_BUTTON_Y + SZ_GAP;
    lh -= SZ_BUTTON_Y + SZ_GAP;

    PiecesCountList = new PiecesList(x, y, w, lh-SZ_GAP, puzzle);
    Fl_Group * shapeGroup = new BlockListGroup(x, y, w, lh, PiecesCountList);

    group->resizable(shapeGroup);
    group->end();

    y += lh;
  }

  tile->end();

  TabProblems->resizable(tile);
  TabProblems->end();
}

void UserInterface::CreateSolveTab(int x, int y, int w, int h) {

  TabSolve = new Fl_Group(x, y, w, h, "Solve");
  TabSolve->tooltip("Solve problems");
  TabSolve->hide();

  x += SZ_GAP; y++; w -= 2*SZ_GAP; h -= SZ_GAP + 1;

  Fl_Group * tile = new Fl_Tile(x, y, w, h);

  // calculate hight of different groups
  const int paramsFixedHight = SZ_SEPARATOR_Y + 4*SZ_BUTTON_Y + 5*SZ_GAP +  6*SZ_TEXT_Y;
  const int solutionsFixedHight = SZ_SEPARATOR_Y + 2*SZ_BUTTON_Y + 2*SZ_GAP + 2*SZ_TEXT_Y;

  int hi = h - paramsFixedHight - solutionsFixedHight;

  assert(hi > 30);

  int paramsHight = hi/2 + paramsFixedHight;
  int solutionsHight = hi - (hi/2) + solutionsFixedHight;

  {
    int lh = paramsHight;

    Fl_Group* group = new Fl_Group(x, y, w, lh);
    group->box(FL_FLAT_BOX);
    new Separator(x, y, w, SZ_SEPARATOR_Y, "Parameters", false);
    y += SZ_SEPARATOR_Y;
    lh -= SZ_SEPARATOR_Y;

    solutionProblem = new ProblemSelector(x, y, w, lh-SZ_GAP, puzzle);
    Fl_Group * shapeGroup = new BlockListGroup(x, y, w, lh-(paramsFixedHight-SZ_SEPARATOR_Y), solutionProblem);
    solutionProblem->callback(cb_SolProbSel_stub);

    group->resizable(shapeGroup);

    y += lh - (paramsFixedHight-SZ_SEPARATOR_Y);
    lh -= lh - (paramsFixedHight-SZ_SEPARATOR_Y);

    SolveDisasm = new Fl_Check_Button(x, y, w, SZ_BUTTON_Y, "Solve Disassembly");
    SolveDisasm->tooltip("Do also try to disassemble the assembled puzzles. Only puzzles that can be disassembled will be added to solutions.");
    y += SZ_BUTTON_Y;
    lh -= SZ_BUTTON_Y;

    (new Fl_Check_Button(x, y, w, SZ_BUTTON_Y, "Just Count"))->tooltip("Don\'t save the solutions, just count the number of them.");
    y += SZ_BUTTON_Y + SZ_GAP;
    lh -= SZ_BUTTON_Y + SZ_GAP;

    int bw = w - 2*SZ_GAP;

    int b1 = 8 * bw / 30;
    int b2 = 14 * bw / 30;
    int b3 = bw-b1-b2;

    {
      Fl_Group * o = new Fl_Group(x          , y, b1+SZ_GAP  , SZ_BUTTON_Y);
      BtnStart = new FlatButton(x, y, b1, SZ_BUTTON_Y, "Start", "Start new solving process, removing old result.", cb_BtnStart_stub);
      o->resizable(BtnStart);
      o->end();
    }
    {
      Fl_Group * o = new Fl_Group(x+b1+SZ_GAP, y, b2+SZ_GAP  , SZ_BUTTON_Y);
      BtnCont = new FlatButton(x+b1+SZ_GAP, y, b2, SZ_BUTTON_Y, "Continue", "Continue started process.", cb_BtnCont_stub);
      o->resizable(BtnCont);
      o->end();
    }
    {
      Fl_Group * o = new Fl_Group(x+2*SZ_GAP+b1+b2, y, b3+SZ_GAP  , SZ_BUTTON_Y);
      BtnStop = new FlatButton(x+2*SZ_GAP+b1+b2, y, b3, SZ_BUTTON_Y, "Stop", "Stop a currently running solution process.", cb_BtnStop_stub);
      o->resizable(BtnStop);
      o->end();
    }

    y += SZ_BUTTON_Y + SZ_GAP;
    lh -= SZ_BUTTON_Y + SZ_GAP;

    SolvingProgress = new Fl_Progress(x, y, w, SZ_BUTTON_Y, 0);
    SolvingProgress->tooltip("Percentage of solution space searched.");
    SolvingProgress->box(FL_ENGRAVED_BOX);
    SolvingProgress->selection_color((Fl_Color)4);
    SolvingProgress->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    y += SZ_BUTTON_Y + SZ_GAP;
    lh -= SZ_BUTTON_Y + SZ_GAP;

    OutputActivity = new Fl_Output(x+w/2, y, w/2, SZ_TEXT_Y, "Activity:");
    OutputActivity->box(FL_FLAT_BOX);
    OutputActivity->color(FL_BACKGROUND_COLOR);
    y += SZ_TEXT_Y + SZ_GAP;
    lh -= SZ_TEXT_Y + SZ_GAP;

    OutputAssemblies = new Fl_Value_Output(x+w/2, y, w/2, SZ_TEXT_Y, "Assemblies:");
    OutputAssemblies->box(FL_FLAT_BOX);
    y += SZ_TEXT_Y;
    lh -= SZ_TEXT_Y;

    OutputSolutions = new Fl_Value_Output(x+w/2, y, w/2, SZ_TEXT_Y, "Solutions:");
    OutputSolutions->box(FL_FLAT_BOX);
    y += SZ_TEXT_Y;
    lh -= SZ_TEXT_Y;

    OutputIterations = new Fl_Value_Output(x+w/2, y, w/2, SZ_TEXT_Y, "Iterations:");
    OutputIterations->box(FL_FLAT_BOX);
    y += SZ_TEXT_Y + SZ_GAP;
    lh -= SZ_TEXT_Y + SZ_GAP;

    (new Fl_Output(x+w/2, y, w/2, SZ_TEXT_Y, "Time used:"))->box(FL_NO_BOX);
    y += SZ_TEXT_Y;
    lh -= SZ_TEXT_Y;

    (new Fl_Output(x+w/2, y, w/2, SZ_TEXT_Y, "Time left:"))->box(FL_NO_BOX);
    y += SZ_TEXT_Y;
    lh -= SZ_TEXT_Y;

    assert(lh == 0);

    group->end();
  }

  {
    int lh = solutionsHight;

    Fl_Group* group = new Fl_Group(x, y, w, lh);
    group->box(FL_FLAT_BOX);
    new Separator(x, y, w, SZ_SEPARATOR_Y, "Solutions", true);
    y += SZ_SEPARATOR_Y;
    lh -= SZ_SEPARATOR_Y;

    SolutionsInfo = new Fl_Value_Output(x+w/2, y, w/2, SZ_TEXT_Y);
    SolutionsInfo->tooltip("Number of solutions");
    SolutionsInfo->box(FL_FLAT_BOX);
    y += SZ_TEXT_Y;
    lh -= SZ_TEXT_Y;

    SolutionSel = new Fl_Value_Slider(x, y, w, SZ_BUTTON_Y, "Solution");
    SolutionSel->tooltip("Select one Solution.");
    SolutionSel->type(1);
    SolutionSel->step(1);
    SolutionSel->callback(cb_SolutionSel_stub);
    SolutionSel->align(FL_ALIGN_TOP_LEFT);
    SolutionSel->box(FL_THIN_DOWN_BOX);
    y += SZ_BUTTON_Y + SZ_GAP;
    lh -= SZ_BUTTON_Y + SZ_GAP;
 
    MovesInfo = new Fl_Value_Output(x+w/2, y, w/2, SZ_TEXT_Y);
    MovesInfo->tooltip("Steps for complete disassembly");
    MovesInfo->box(FL_FLAT_BOX);
    y += SZ_TEXT_Y;
    lh -= SZ_TEXT_Y;

    SolutionAnim = new Fl_Value_Slider(x, y, w, SZ_BUTTON_Y, "Move");
    SolutionAnim->tooltip("Animate the disassembly.");
    SolutionAnim->type(1);
    SolutionAnim->callback(cb_SolutionAnim_stub);
    SolutionAnim->align(FL_ALIGN_TOP_LEFT);
    SolutionAnim->box(FL_THIN_DOWN_BOX);
    y += SZ_BUTTON_Y + SZ_GAP;
    lh -= SZ_BUTTON_Y + SZ_GAP;

    PcVis = new PieceVisibility(x, y, w, lh, puzzle);
    PcVis->tooltip("Change appearance of the pieces between normal, grid and invisible.");
    Fl_Group * shapeGroup = new BlockListGroup(x, y, w, lh, PcVis);
    shapeGroup->callback(cb_PcVis_stub);

      group->resizable(shapeGroup);
    group->end();
  }
  tile->end();

  TabSolve->resizable(tile);
  TabSolve->end();
}

UserInterface::UserInterface() {
  ui = this;

  assmThread = 0;
  fname = 0;
  disassemble = 0;

  puzzle = new puzzle_c();
  changed = false;

  for (int j = 0; j < 33; j++)
    visibility[j] = 0;

  mainWindow = new Fl_Double_Window(SZ_WINDOW_X, SZ_WINDOW_Y);
  mainWindow->label("BurrTools - unknown");
  mainWindow->user_data((void*)(this));

  MainMenu = new Fl_Menu_Bar(0, 0, SZ_WINDOW_X, SZ_MENU_Y);
  MainMenu->menu(menu_MainMenu);

  Fl_Tile * mainTile = new Fl_Tile(0, SZ_CONTENT_START_Y, SZ_WINDOW_X, SZ_CONTENT_Y);
  View3D = new View3dGroup(SZ_TOOL_X, SZ_CONTENT_START_Y, SZ_3DAREA_X, SZ_CONTENT_Y);
  new Fl_Group(0, SZ_CONTENT_START_Y, SZ_TOOL_X, SZ_CONTENT_Y);

  // this box paints the background behind the tab, because the tabs are partly transparent
  (new Fl_Box(FL_FLAT_BOX, 0, SZ_CONTENT_START_Y, SZ_TOOL_X, SZ_CONTENT_Y, 0))->color(FL_BACKGROUND_COLOR);

  // the tab for the tool bar
  TaskSelectionTab = new Fl_Tabs(0, SZ_CONTENT_START_Y, SZ_TOOL_X, SZ_CONTENT_Y);
  TaskSelectionTab->box(FL_THIN_UP_BOX);
  TaskSelectionTab->callback(cb_TaskSelectionTab_stub);

  // the three tabs
  CreateShapeTab(  0, SZ_CONTENT_START_Y+SZ_TAB_Y, SZ_TOOL_X, SZ_CONTENT_Y-SZ_TAB_Y);
  CreateProblemTab(0, SZ_CONTENT_START_Y+SZ_TAB_Y, SZ_TOOL_X, SZ_CONTENT_Y-SZ_TAB_Y);
  CreateSolveTab(  0, SZ_CONTENT_START_Y+SZ_TAB_Y, SZ_TOOL_X, SZ_CONTENT_Y-SZ_TAB_Y);

  mainWindow->resizable(mainTile);

  activateClear();
}

