/*
 *  Copyright (c) 2018-2023, djcj <djcj@gmx.de>
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
#include <FL/Fl_Double_Window.H>
#include <string>

#include "helper_classes.hpp"

#ifdef MEDIAINFO_DYNAMIC
# include "MediaInfoDLL/MediaInfoDLL.h"
# define MEDIAINFONAMESPACE MediaInfoDLL;
#else
# include "MediaInfo/MediaInfo.h"
# define MEDIAINFONAMESPACE MediaInfoLib;
#endif
#include <ZenLib/Ztring.h>

using namespace MEDIAINFONAMESPACE;
using namespace ZenLib;


class mediainfo_fltk
{
public:
  mediainfo_fltk() {}
  ~mediainfo_fltk() {}

  static int launch(const char *file);

private:
  static Fl_Double_Window *win, *about_win;
  static MyTextDisplay *compact, *text;
  static MyHelpView *html;
  static MyTree *tree;
  static const char *view_set;
  static int *flags_expand, *flags_collapse;

  /* helpers */
  static Ztring get_info(MediaInfo &mi);
  static void replace_string(const std::string &from, const std::string &to, std::string &s);
  static void load_file(const char *file);
  static void copy_selection(MyTextDisplay *disp);

  /* callbacks */
  static void do_nothing_cb(Fl_Widget *, void *) {}
  static void tree_expand_all_cb(Fl_Widget *, void *);
  static void tree_collapse_all_cb(Fl_Widget *, void *);
  static void dnd_dropped_cb(Fl_Widget *, void *);
  static void view_compact_cb(Fl_Widget *, void *);
  static void view_text_cb(Fl_Widget *, void *);
  static void view_html_cb(Fl_Widget *, void *);
  static void view_tree_cb(Fl_Widget *, void *);
  static void open_file_cb(Fl_Widget *, void *);
  static void about_cb(Fl_Widget *, void *);
  static void close_cb(Fl_Widget *, void *v);

  static void copy_compact_selection_cb(Fl_Widget *, void *) {
    copy_selection(compact);
  }

  static void copy_text_selection_cb(Fl_Widget *, void *) {
    copy_selection(text);
  }

  static void copy_html_selection_cb(Fl_Widget *, void *) {
    html->copy_selection();
  }

  static void copy_tree_selection_cb(Fl_Widget *, void *);
};
