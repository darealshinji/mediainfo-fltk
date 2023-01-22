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

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
//#include <FL/Fl_Help_View.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Double_Window.H>

/* modified FLTK 1.3 header */
#include "Fl_Help_View.H"

#include <iostream>
#include <string>
#include <vector>
#include <strings.h>
#include <string.h>

#ifdef MEDIAINFO_DYNAMIC
# include "MediaInfoDLL/MediaInfoDLL.h"
# define MEDIAINFONAMESPACE MediaInfoDLL;
#else
# include "MediaInfo/MediaInfo.h"
# define MEDIAINFONAMESPACE MediaInfoLib;
#endif
#include <ZenLib/Ztring.h>

#include "mediainfo.hpp"
#include "icon.h"

#define VENDOR  "https://github.com/darealshinji"
#define APPNAME "mediainfo-fltk"


using namespace MEDIAINFONAMESPACE;
using namespace ZenLib;

extern Ztring get_info(MediaInfo &mi);
static void tree_collapse_all_cb(Fl_Widget *, void *);

static Fl_Double_Window *win = NULL;
static Fl_Double_Window *about_win = NULL;
static MyTextDisplay *compact = NULL;
static MyTextDisplay *text = NULL;
static MyHelpView *html = NULL;
static MyTree *tree = NULL;
static const char *view_set = "compact";
static int *flags_expand;
static int *flags_collapse;


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

static void replace_string(const std::string &from, const std::string &to, std::string &s) {
  for (size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size()) {
    s.replace(pos, from.size(), to);
  }
}

static void load_file(const char *file)
{
  MediaInfo mi;
  Ztring ztr;
  Fl_Text_Buffer *buff;
  std::string str, root;
  std::vector<std::string> vec;
  size_t i, pos;

  if (!file) {
    return;
  }

  ztr = file;

  if (!mi.Open(ztr)) {
    return;
  }

  mi.Option(__T("Output"), __T("HTML"));
  ztr = mi.Inform();
  html->value(ztr.To_Local().c_str());

  /* compact info */
  ztr = get_info(mi);
  buff = compact->buffer();
  buff->remove(0, buff->length());  /* clear buffer */
  buff->text(ztr.To_Local().c_str());

  mi.Option(__T("Output"), __T("TEXT"));
  ztr = mi.Inform();
  str = ztr.To_Local().c_str();
  buff = text->buffer();
  buff->remove(0, buff->length());  /* clear buffer */
  buff->text(str.c_str());

  mi.Close();
  ztr.clear();


  /* split string into lines and save them in a vector */

  i = 0;
  pos = str.find('\n');

  while (pos != std::string::npos) {
    vec.push_back(str.substr(i, pos - i));
    i = ++pos;

    if ((pos = str.find('\n', pos)) == std::string::npos) {
      vec.push_back(str.substr(i, str.length()));
    }
  }


  /* tree items */

  tree->clear();
  tree->begin();

  for (const std::string &elem : vec) {
    std::string sub;

    if (elem == "") {
      continue;
    } else if (elem.size() < 44) {
      root = elem;
      continue;
    }

    sub = elem.substr(0, elem.find_last_not_of(' ', 40) + 1);
    replace_string("/", "\\/", sub);
    str = root + "/" + sub + "/";

    sub = elem.substr(43);
    replace_string("/", "\\/", sub);
    str += sub;

    tree->add(str.c_str());
  }

  tree->end();
  tree_collapse_all_cb(NULL, NULL);
}

static inline void do_nothing_cb(Fl_Widget *, void *) {
}

static void tree_expand_all_cb(Fl_Widget *, void *)
{
  Fl_Tree_Item *item = tree->first();

  while (item != tree->last()) {
    item = tree->next_item(item);
    tree->open(item);
  }
}

static void tree_collapse_all_cb(Fl_Widget *, void *)
{
  Fl_Tree_Item *item = tree->first();

  while (item != tree->last()) {
    item = tree->next_item(item);
    tree->close(item);
  }
}

static void dnd_dropped_cb(Fl_Widget *, void *)
{
  char *copy, *p;
  const char *item = Fl::event_text();

  if (strncmp(item, "file:///", 8) != 0) {
    return;
  }

  copy = strdup(item + 7);

  if ((p = strchr(copy, '\n')) == NULL) {
    free(copy);
    return;
  }

  copy[strlen(copy) - strlen(p)] = '\0';
  fl_decode_uri(copy);
  load_file(copy);
  free(copy);
}

static void view_compact_cb(Fl_Widget *, void *)
{
  text->hide();
  html->hide();
  tree->hide();
  view_set = "compact";
  *flags_expand = *flags_collapse = FL_MENU_INACTIVE;

  compact->show();
  compact->scroll(0, 0);
}

static void view_text_cb(Fl_Widget *, void *)
{
  compact->hide();
  html->hide();
  tree->hide();
  view_set = "text";
  *flags_expand = *flags_collapse = FL_MENU_INACTIVE;

  text->show();
  text->scroll(0, 0);
}

static void view_html_cb(Fl_Widget *, void *)
{
  compact->hide();
  text->hide();
  tree->hide();
  view_set = "html";
  *flags_expand = *flags_collapse = FL_MENU_INACTIVE;

  html->show();
  html->topline(0);
}

static void view_tree_cb(Fl_Widget *, void *)
{
  compact->hide();
  text->hide();
  html->hide();
  view_set = "tree";
  *flags_expand = *flags_collapse = 0;

  tree->show();
  tree->hposition(0);
  tree->vposition(0);
}

static void open_file_cb(Fl_Widget *, void *)
{
  Fl_Native_File_Chooser fc(Fl_Native_File_Chooser::BROWSE_FILE);
  fc.title("Select a file");

  if (fc.show() == 0) {
    load_file(fc.filename());
  }
}

static void about_cb(Fl_Widget *, void *)
{
  int x = win->x() + win->w()/2 - about_win->w()/2;
  int y = win->y() + win->h()/2 - about_win->h()/2;

  about_win->resize(x, y, about_win->w(), about_win->h());
  about_win->show();
  about_win->take_focus();
}

static void copy_selection(MyTextDisplay *disp)
{
  int start = 0, end = 0;

  Fl_Text_Buffer *buff = disp->buffer();
  buff->selection_position(&start, &end);
  char *sel = buff->text_range(start, end);

  Fl::copy(sel, end - start, 1);
  free(sel);
}

static inline void copy_compact_selection_cb(Fl_Widget *, void *) {
  copy_selection(compact);
}

static inline void copy_text_selection_cb(Fl_Widget *, void *) {
  copy_selection(text);
}

static inline void copy_html_selection_cb(Fl_Widget *, void *) {
  html->copy_selection();
}

static void copy_tree_selection_cb(Fl_Widget *, void *)
{
  Fl_Tree_Item *item = tree->last_selected_item();
  const char *l1, *l2, *l3;

  if (!item) {
    return;
  }

  if (item->depth() == 3) {
    Fl_Tree_Item *par = item->parent();
    l1 = par->parent()->label();
    l2 = item->parent()->label();
    l3 = item->label();
  } else if (item->depth() == 2 && item->has_children()) {
    l1 = item->parent()->label();
    l2 = item->label();
    l3 = item->child(0)->label();
  } else {
    l1 = l2 = l3 = NULL;
  }

  if (l1 && l2 && l3) {
    std::string s = "[" + std::string(l1) + "] " + std::string(l2) + ": " + std::string(l3);
    Fl::copy(s.c_str(), s.size(), 1);
  }
}

static void close_cb(Fl_Widget *, void *v)
{
  about_win->hide();
  win->hide();

  const char *path = reinterpret_cast<const char *>(v);

  if (path && *path) {
    Fl_Preferences pref(path, VENDOR, APPNAME);
    pref.set("view", view_set);
    pref.flush();
  }
}

static int mediainfo_fltk(const char *file)
{
  std::string conf;
  char *home, view_get[8] = {0};
  int *flags_compact, *flags_text, *flags_html, *flags_tree;

  /* http://fltk.org/str.php?L3465+P0+S-2+C0+I0+E0+V%25+QFL_SCREEN */
  Fl::set_font(FL_SCREEN, " mono");

  /* load preferences */
  if ((home = getenv("HOME")) != NULL) {
    conf = home;
    conf += "/.config";

    Fl_Preferences pref(conf.c_str(), VENDOR, APPNAME);

    if (!pref.get("view", view_get, "text", sizeof(view_get) - 1)) {
      view_get[0] = 0;
    }
  }

  /* menus */

  Fl_Menu_Item menu[] = {
    { "File", 0, NULL, NULL, FL_SUBMENU },
      { " Open file  ", 0, open_file_cb, NULL, FL_MENU_DIVIDER },
      { " Close window  ", 0, close_cb, (void *)conf.c_str() },
      {0},
    { "View", 0, NULL, NULL, FL_SUBMENU },
      { " Compact  ", 0, view_compact_cb, NULL, FL_MENU_RADIO },
      { " Text  ", 0, view_text_cb, NULL, FL_MENU_RADIO },
      { " HTML  ", 0, view_html_cb, NULL, FL_MENU_RADIO },
      { " Tree  ", 0, view_tree_cb, NULL, FL_MENU_RADIO|FL_MENU_DIVIDER },
      { " Expand all  ", 0, tree_expand_all_cb, NULL, FL_MENU_INACTIVE },
      { " Collapse all  ", 0, tree_collapse_all_cb, NULL, FL_MENU_INACTIVE },
      {0},
    { "Help", 0, NULL, NULL, FL_SUBMENU },
      { " About  ", 0, about_cb },
      {0},
    {0}
  };

  const short pos = 5;
  flags_compact  = &menu[pos+0].flags;
  flags_text     = &menu[pos+1].flags;
  flags_html     = &menu[pos+2].flags;
  flags_tree     = &menu[pos+3].flags;
  flags_expand   = &menu[pos+4].flags;
  flags_collapse = &menu[pos+5].flags;

  Fl_Menu_Item compact_menu[] = {
    { " Copy selection  ", 0, copy_compact_selection_cb, NULL, FL_MENU_DIVIDER },
    { " Dismiss  ", 0, do_nothing_cb },
    {0}
  };

  Fl_Menu_Item text_menu[] = {
    { " Copy selection  ", 0, copy_text_selection_cb, NULL, FL_MENU_DIVIDER },
    { " Dismiss  ", 0, do_nothing_cb },
    {0}
  };

  Fl_Menu_Item html_menu[] = {
    { " Copy selection  ", 0, copy_html_selection_cb, NULL, FL_MENU_DIVIDER },
    { " Dismiss  ", 0, do_nothing_cb },
    {0}
  };

  Fl_Menu_Item tree_menu[] = {
    { " Copy selection  ", 0, copy_tree_selection_cb },
    { " Expand all  ", 0, tree_expand_all_cb },
    { " Collapse all  ", 0, tree_collapse_all_cb, NULL, FL_MENU_DIVIDER },
    { " Dismiss  ", 0, do_nothing_cb },
    {0}
  };

  /* main window */
  Fl_Double_Window xwin(800, 600, "MediaInfo");
  win = &xwin;
  xwin.callback(close_cb, (void *)conf.c_str());

    Fl_Group g(0, 0, 800, 600);

      Fl_Group g_top(10, 0, 780, 30);
        Fl_Menu_Bar mbar(10, 0, 200, 30);
        mbar.box(FL_NO_BOX);
        mbar.menu(menu);
      g_top.end();
      g_top.resizable(NULL);

      Fl_Group g_inside(10, 30, 780, 560);

        Fl_Text_Buffer compact_buff;
        MyTextDisplay xcompact(10, 30, 780, 560);
        xcompact.buffer(&compact_buff);
        xcompact.menu(compact_menu);
        compact = &xcompact;

        Fl_Text_Buffer text_buff;
        MyTextDisplay xtext(10, 30, 780, 560);
        xtext.buffer(&text_buff);
        xtext.menu(text_menu);
        xtext.hide();
        text = &xtext;

        MyHelpView xhtml(10, 30, 780, 560);
        xhtml.menu(html_menu);
        xhtml.hide();
        html = &xhtml;

        MyTree xtree(10, 30, 780, 560);
        xtree.menu(tree_menu);
        xtree.hide();
        tree = &xtree;

      g_inside.end();

      MyDndBox dnd(10, 30, 780, 560);
      dnd.callback(dnd_dropped_cb);

    g.end();
    g.resizable(g_inside);

  xwin.end();
  xwin.resizable(g);
  xwin.position((Fl::w() - 800) / 2, (Fl::h() - 600) / 2); /* center */
  xwin.show();

  /* "about" window */
  Fl_Double_Window xabout_win(260, 130, "About");
    MediaInfo mi;

    const int version = Fl::api_version();
    int major = version / 10000;
    int minor = (version % 10000) / 100;
    int patch = version % 100;

    Ztring url = mi.Option(__T("Info_Url"));
    Ztring ztr =
      __T("<html><body bgcolor=silver><center><p></p>"
      "<p>") + mi.Option(__T("Info_Version")) + __T("<br><a href=\"") + url + __T("\">") + url + __T("</a></p>"
      "<p>FLTK ") + Ztring::ToZtring(major) + __T(".") + Ztring::ToZtring(minor) + __T(".") + Ztring::ToZtring(patch) +
      __T("<br><a href=\"https://www.fltk.org/\">https://www.fltk.org/</a></p>"
      "</center></body></html>");

    Fl_Help_View hv(0, 0, 260, 130);
    hv.box(FL_FLAT_BOX);
    hv.value(ztr.To_Local().c_str());

  xabout_win.end();
  about_win = &xabout_win;

  /* set view type */
  if (strcasecmp(view_get, "text") == 0) {
    *flags_text = FL_MENU_RADIO|FL_MENU_VALUE;
    view_text_cb(NULL, NULL);
  } else if (strcasecmp(view_get, "html") == 0) {
    *flags_html = FL_MENU_RADIO|FL_MENU_VALUE;
    view_html_cb(NULL, NULL);
  } else if (strcasecmp(view_get, "tree") == 0) {
    *flags_tree = FL_MENU_RADIO|FL_MENU_VALUE|FL_MENU_DIVIDER;
    *flags_expand = *flags_collapse = 0;
    view_tree_cb(NULL, NULL);
  } else {
    *flags_compact = FL_MENU_RADIO|FL_MENU_VALUE;
    /* no need to call view_compact_cb() because "compact" is already set as default */
  }

  /* load file */
  if (file && *file) {
    load_file(file);
  }

  return Fl::run();
}

int main(int argc, char *argv[])
{
  /* set icon */
  Fl_PNG_Image png(NULL, icon_png, icon_png_len);
  Fl_Window::default_icon(&png);

  /* load MediaInfo library */
#ifdef MEDIAINFO_DYNAMIC
  auto lambda_check = []() -> int { MEDIAINFO_TEST_INT; return 1; };

  if (lambda_check() == 0) {
    fl_message_title("Error");
    fl_alert("%s", "Unable to load " MEDIAINFODLL_NAME);
    Fl::run();
    return 1;
  }
#endif

  return mediainfo_fltk(argc > 1 ? argv[1] : NULL);
}

