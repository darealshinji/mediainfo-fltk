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

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Tree.H>
#include <FL/filename.H>
#include <MediaInfoDLL/MediaInfoDLL.h>
#include <fstream>
#include <iostream>
#include <string>

#include "fnfc.hpp"
#include "dnd.hpp"
#include "mediainfo.hpp"


/* generate callback function and set callback */
#define MAKE_CALLBACK(NAME) \
    static auto NAME##_cb = [] (Fl_Widget *, void *p) { \
        reinterpret_cast<decltype(this)>(p)->do_##NAME(); \
    }

#define METHODCB(WIDGET, NAME) \
    do { \
        MAKE_CALLBACK(NAME); \
        WIDGET->callback(NAME##_cb, this); \
    } while (0)


/* output list order */
enum {
    V_BRIEF,
    V_FULL,
    V_COMPLETE,
    V_HTML,
    V_XML,
    V_MAXML,
    V_MPEG7,
    V_JSON,
    V_CSV,
    V_LAST
};


void MIGui::do_open_file()
{
    FNFC fc(FNFC::BROWSE_FILE, "Select a file");
    const char *file = fc.select();

    if (file && *file) {
        m_file = file;
        do_set_view();
    }
}


void MIGui::do_save_file()
{
    std::ofstream ofs;
    const char *filter, *preset;

    int val = m_choice->value();

#define FLTR(DESC, EXT) \
    DESC "\t" EXT "\n" \
    "All Files\t*"

    switch (val)
    {
    case V_XML:
    case V_MAXML:
    case V_MPEG7:
        filter = FLTR("XML", "*.xml");
        preset = "info.xml";
        break;
    case V_HTML:
        filter = FLTR("HTML", "*.html,*.htm");
        preset = "info.html";
        break;
    case V_JSON:
        filter = FLTR("JSON", "*.json");
        preset = "info.json";
        break;
    case V_CSV:
        filter = FLTR("CSV", "*.csv");
        preset = "info.csv";
        break;
    default:
        filter = FLTR("Plain Text", "*.txt");
        preset = "info.txt";
        break;
    }

    /* select output file path */
    FNFC fc(FNFC::BROWSE_SAVE_FILE, "Save as");
    fc.options(FNFC::SAVEAS_CONFIRM | FNFC::NEW_FOLDER | FNFC::USE_FILTER_EXT);
    fc.filter(filter);
    fc.preset_file(preset);

    const char *file = fc.select();

    if (!file || !*file) {
        return;
    }

    /* open file for writing */
    ofs.open(file);

    if (!ofs.is_open()) {
        fl_message_title("Error");
        fl_alert("Cannot open file for writing:\n%s", file);
        return;
    }

    /* save data */
    if (val == V_HTML) {
        const char *data = m_html->value();
        ofs.write(data, strlen(data));
    } else {
        ofs.write(m_text_buff->text(), m_text_buff->length());
    }
}


void MIGui::do_dnd()
{
    const char *text = Fl::event_text();

    if (!text || !*text) {
        return;
    }

    /* newline separator */
    const char *p = strchr(text, '\n');

    if (!p) {
        return;
    }

    /* copy first item, without newline */
    std::string str(text, p - text);

    if (str.starts_with("file://")) {
        /* decode URI in place */
        str.erase(0, 7);
        fl_decode_uri(std::data(str));
    }

    if (str.starts_with('/')) {
        m_file = str.c_str();
        do_set_view();
    }
}


void MIGui::do_set_view()
{
    MediaInfo mi;
    std::string buf;
    const char *output = "Text";

    if (m_choice->value() == V_HTML) {
        m_text_disp->hide();
        m_html->show();
    } else {
        m_html->hide();
        m_text_disp->show();
    }

    /* get file info if m_file is not empty */
    if (m_file.empty()) {
        return;
    }

    if (!mi.Open(m_file)) {
        fl_message_title("Error");
        fl_alert("Failed to open file: %s", m_file.c_str());
        return;
    }

    /* activate "Save as" button */
    m_btsave->activate();

    mi.Option("Complete", "0");

    switch (m_choice->value())
    {
    case V_CSV:   output = "CSV";    break;
    case V_XML:   output = "XML";    break;
    case V_MAXML: output = "MAXML";  break;
    case V_MPEG7: output = "MPEG-7"; break;
    case V_JSON:  output = "JSON";   break;

    case V_COMPLETE:
        mi.Option("Complete", "1");
        output = "Text";
        break;

    case V_FULL:
        output = "Text";
        break;

    case V_HTML:
        mi.Option("Output", "HTML");
        buf = mi.Inform();
        m_text_buff->remove(0, m_text_buff->length());
        m_html->value(buf.c_str());
        return;

    case V_BRIEF:
    default:
        buf = get_brief_data(mi);
        m_html->value("");
        m_text_buff->replace(0, m_text_buff->length(), buf.c_str());
        return;
    }

    mi.Option("Output", output);
    buf = mi.Inform();
    m_html->value("");
    m_text_buff->replace(0, m_text_buff->length(), buf.c_str());
}


void MIGui::init_main_window()
{
    const int bt_w = 90;
    const int bt_h = 28;
    int x, y, w, h;

    MAKE_CALLBACK(set_view);
    MAKE_CALLBACK(dnd);

    /* initialize ALL menu entries */
    const char *menu[V_LAST];
    menu[V_BRIEF]    = "Brief";
    menu[V_COMPLETE] = "Complete";
    menu[V_CSV]      = "CSV";
    menu[V_FULL]     = "Full";
    menu[V_HTML]     = "HTML";
    menu[V_JSON]     = "JSON";
    menu[V_MAXML]    = "XML (MediaArea)";
    menu[V_MPEG7]    = "MPEG-7";
    menu[V_XML]      = "XML";


    /* main window */
    w = 640;
    h = 480;
    m_win = new dnd_window(w, h, "Simple MediaInfo GUI");
    m_win->dnd(dnd_cb, this);

    if (MediaInfoDLL_IsLoaded()) {
        /* append MediaInfo version to label */
        std::string l = m_win->label();
        l += " (";
        l += MediaInfo::Option_Static("Info_Version");
        l += ")";
        m_win->copy_label(l.c_str());
    }

        /* top area */
        x = 0;
        y = 0;
        w = m_win->w();
        h = 15 + bt_h;
        auto g_top = new Fl_Group(x, y, w, h);

            /* Open */
            x = 10;
            y = 10;
            w = bt_w;
            h = bt_h;
            auto bt_open = new Fl_Button(x, y, w, h, "Open");
            bt_open->clear_visible_focus();
            METHODCB(bt_open, open_file);

            /* View choice */
            x += w + 10;
            y = 10;
            w = 144;
            h = bt_h;
            m_choice = new Fl_Choice(x, y, w, h);
            for (int i = 0; i < V_LAST; i++) {
                m_choice->add(menu[i], 0, set_view_cb, this);
            }
            m_choice->value(0);
            m_choice->clear_visible_focus();

            /* Save as */
            x += w + 10;
            y = 10;
            w = bt_w;
            h = bt_h;
            m_btsave = new Fl_Button(x, y, w, h, "Save as");
            m_btsave->clear_visible_focus();
            m_btsave->deactivate();
            METHODCB(m_btsave, save_file);

            /* dummy */
            x += w + 1;
            y = 10;
            w = 1;
            h = 1;
            auto dummy = new Fl_Box(FL_NO_BOX, x, y, w, h, NULL);

        g_top->end();
        g_top->resizable(dummy);

        /* main area */
        x = 10;
        y = g_top->h();
        w = m_win->w() - 20;
        h = m_win->h() - g_top->h() - 10;

        /* text stuff */
        m_text_disp = new Fl_Text_Display(x, y, w, h);
        m_text_disp->textfont(FL_SCREEN);
        m_text_disp->buffer(m_text_buff);
        m_text_disp->color(fl_lighter(fl_lighter(FL_BACKGROUND_COLOR)));

        /* HTML */
        m_html = new Fl_Help_View(x, y, w, h);
        m_html->hide();

    m_win->end();
    m_win->resizable(m_text_disp);

    /* minimum width and height */
    w = m_btsave->x() + m_btsave->w() + 10;
    h = 150;
    m_win->size_range(w, h, Fl::w(), Fl::h());
}


MIGui::MIGui()
{
    MediaInfoDLL_Load();
    m_text_buff = new Fl_Text_Buffer();
    init_main_window();
}


MIGui::~MIGui()
{
    delete m_win;
    delete m_text_buff;
    MediaInfoDLL_UnLoad();
}


int MIGui::show(const char *file)
{
    if (!MediaInfoDLL_IsLoaded()) {
        fl_message_title("Error");
        fl_alert("Failed to load %s", MEDIAINFODLL_NAME);
        Fl::hide_all_windows();
        return 1;
    } else if (m_win->shown()) {
        return 0;
    }

    if (file && *file) {
        m_file = file;
        do_set_view();
    }

    m_win->show();

    /* position at desktop center */
    //m_win->wait_for_expose();
    m_win->position(
        (Fl::w() - m_win->decorated_w()) / 2,
        (Fl::h() - m_win->decorated_h()) / 2
    );

    return 0;
}
