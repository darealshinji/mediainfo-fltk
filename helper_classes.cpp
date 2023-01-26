/*
 *  Copyright (c) 2018-2023, djcj <djcj@gmx.de>
 *
 *  The MediaInfo icon is Copyright (c) 2002-2021, MediaArea.net SARL
 *  All rights reserved.
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

#include "helper_classes.hpp"


MyDndBox::MyDndBox(int X, int Y, int W, int H)
  : Fl_Box(X, Y, W, H)
{ }

int MyDndBox::handle(int event)
{
  switch (event) {
    case FL_DND_ENTER:
    case FL_DND_DRAG:
    case FL_DND_RELEASE:
      return 1;
    case FL_PASTE:
      do_callback();
      return 1;
  }
  return Fl_Box::handle(event);
}

MyTextDisplay::MyTextDisplay(int X, int Y, int W, int H)
  : Fl_Text_Display(X, Y, W, H)
{
  menu(NULL);
  textfont(FL_SCREEN);
  wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 1);
}

int MyTextDisplay::handle(int event)
{
  if (event == FL_PUSH && Fl::event_button() == FL_RIGHT_MOUSE) {
    _menu[0].flags = (buffer() && buffer()->selected()) ? FL_MENU_DIVIDER : FL_MENU_INACTIVE|FL_MENU_DIVIDER;
    if (active_r() && window()) window()->cursor(FL_CURSOR_DEFAULT);

    const Fl_Menu_Item *m = _menu->popup(Fl::event_x(), Fl::event_y());
    if (m) {
      m->do_callback(NULL);
      return 1;
    }
  }
  return Fl_Text_Display::handle(event);
}

MyHelpView::MyHelpView(int X, int Y, int W, int H)
  : Fl_Help_View(X, Y, W, H)
{
  menu(NULL);
}

int MyHelpView::handle(int event)
{
  if (event == FL_PUSH && Fl::event_button() == FL_RIGHT_MOUSE) {
    _menu[0].flags = (is_selected() && has_current_view() == this) ? FL_MENU_DIVIDER : FL_MENU_INACTIVE|FL_MENU_DIVIDER;

    const Fl_Menu_Item *m = _menu->popup(Fl::event_x(), Fl::event_y());
    if (m) {
      m->do_callback(NULL);
      return 1;
    }
  }
  return Fl_Help_View::handle(event);
}

MyTree::MyTree(int X, int Y, int W, int H)
  : Fl_Tree(X, Y, W, H)
{
  menu(NULL);
  showroot(0);
}

int MyTree::handle(int event)
{
  if (event == FL_PUSH && Fl::event_button() == FL_RIGHT_MOUSE) {
    _menu[0].flags = last_selected_item() ? FL_MENU_DIVIDER : FL_MENU_INACTIVE|FL_MENU_DIVIDER;

    const Fl_Menu_Item *m = _menu->popup(Fl::event_x(), Fl::event_y());
    if (m) {
      m->do_callback(NULL);
      return 1;
    }
  }
  return Fl_Tree::handle(event);
}
