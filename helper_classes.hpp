/*
 *  Copyright (c) 2018-2020, djcj <djcj@gmx.de>
 *
 *  BSD 2-Clause License
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Menu_Item.H>

/* modified FLTK 1.3 header */
#include "Fl_Help_View.H"


class MyDndBox : public Fl_Box
{
public:
  MyDndBox(int X, int Y, int W, int H);

protected:
  int handle(int event);
};

class MyTextDisplay : public Fl_Text_Display
{
private:
  Fl_Menu_Item *_menu;

public:
  MyTextDisplay(int X, int Y, int W, int H);
  void menu(Fl_Menu_Item *m) { _menu = m; }

protected:
  int handle(int event);
};

class MyHelpView : public Fl_Help_View
{
private:
  Fl_Menu_Item *_menu;

public:
  MyHelpView(int X, int Y, int W, int H);
  void menu(Fl_Menu_Item *m) { _menu = m; }
  int is_selected() { return selected; }
  Fl_Help_View *has_current_view() { return current_view; }
  void copy_selection() { end_selection(1); }

protected:
  int handle(int event);
};

class MyTree : public Fl_Tree
{
private:
  Fl_Menu_Item *_menu;

public:
  MyTree(int X, int Y, int W, int H);
  void menu(Fl_Menu_Item *m) { _menu = m; }

protected:
  int handle(int event);
};

