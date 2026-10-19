/**
 Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 SPDX-License-Identifier: MIT
 Copyright (c) 2026 Carsten Janssen

 Permission is hereby  granted, free of charge, to any  person obtaining a copy
 of this software and associated  documentation files (the "Software"), to deal
 in the Software  without restriction, including without  limitation the rights
 to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
 copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
 IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
 FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
 AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
 LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
**/

#pragma once

#include <MediaInfoDLL/MediaInfoDLL.h>
#include <string>

using namespace MediaInfoDLL;


class dnd_box;
class Fl_Button;
class Fl_Choice;
class Fl_Double_Window;
class Fl_Help_View;
class Fl_Text_Buffer;
class Fl_Text_Display;


class MIGui
{
private:

    std::string m_file;

    Fl_Double_Window *m_win;
    dnd_box *m_dnd_area;
    Fl_Button *m_btsave;
    Fl_Choice *m_choice;
    Fl_Help_View *m_html;
    Fl_Text_Buffer *m_text_buff;
    Fl_Text_Display *m_text_disp;

    void do_open_file();
    void do_save_file();
    void do_dnd();
    void do_set_view();
    void do_quit();

    static std::string get_brief_data(MediaInfo &mi);

    void get_file_info();
    void init_main_window();

public:

    MIGui();
    ~MIGui();

    int show(const char *file);
};
