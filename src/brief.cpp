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

#include <MediaInfoDLL/MediaInfoDLL.h>
#include <string>

#include "mediainfo.hpp"


#define CHAPTER_UNKNOWN "unknown"


static bool get_info(MediaInfo &mi, std::string &buf, stream_t kind, size_t num, const String &param)
{
    std::string str = mi.Get(kind, num, param);

    if (!str.empty()) {
        buf += str;
        return true;
    }

    return false;
}


/* same as get_info() but clears the buffer first */
static bool get_info_clear(MediaInfo &mi, std::string &buf, stream_t kind, size_t num, const String &param)
{
    buf.clear();
    return get_info(mi, buf, kind, num, param);
}


static size_t get_stream_count(MediaInfo &mi, std::string &buf, stream_t kind, const char *type)
{
    size_t n = mi.Count_Get(kind, -1);

    if (n == 0) {
        return n;
    }

    buf += type;
    buf += " streams: ";

    std::string param = type;
    param += "_Format_List";

    if (!get_info(mi, buf, Stream_General, 0, param)) {
        buf += std::to_string(n);
    }

    buf += '\n';

    return n;
}


static std::string chapter_count(const std::string &beg, const std::string &end)
{
    try {
        int ibeg = std::stoi(beg);
        int iend = std::stoi(end);
        int n = iend - ibeg;

        if (n > 0) {
            return std::to_string(n) + " chapters";
        }
    }
    catch(...) {
        return CHAPTER_UNKNOWN;
    }

    return CHAPTER_UNKNOWN;
}


static void get_menu_count(MediaInfo &mi, std::string &buf)
{
    size_t n = mi.Count_Get(Stream_Menu, -1);

    if (n == 0) {
        return;
    }

    buf += "Menu streams: ";

    for (size_t i = 0; i < n; i++) {
        std::string beg, end;

        if (get_info(mi, beg, Stream_Menu, 0, "Chapters_Pos_Begin") &&
            get_info(mi, end, Stream_Menu, 0, "Chapters_Pos_End"))
        {
            buf += chapter_count(beg, end) + " / ";
        } else {
            buf += CHAPTER_UNKNOWN " / ";
        }
    }

    if (buf.ends_with(" / ")) {
        buf.erase(buf.size() - 3);
    }

    buf += '\n';
}


static void get_video_stream_infos(MediaInfo &mi, std::string &buf, size_t num, size_t count)
{
    std::string w, h, s;

    buf += "\nVideo";

    if (count > 1) {
        buf += ' ' + std::to_string(num + 1);
    }
    buf += '\n';

    if (get_info(mi, buf, Stream_Video, num, "Language/String")) {
        buf += ", ";
    }

    if (get_info(mi, w, Stream_Video, num, "Width") &&
        get_info(mi, h, Stream_Video, num, "Height"))
    {
        buf += w + '*' + h;
        if (get_info_clear(mi, s, Stream_Video, num, "DisplayAspectRatio/String")) {
            buf += " (" + s + ')';
        }
        buf += ", ";
    }

    if (get_info(mi, buf, Stream_Video, num, "FrameRate/String")) {
        buf += ", ";
    }
    if (get_info(mi, buf, Stream_Video, num, "BitRate/String")) {
        buf += ", ";
    }

    if (get_info(mi, buf, Stream_Video, num, "Format")) {
        if (get_info_clear(mi, s, Stream_Video, num, "Format_Profile")) {
            buf += " (" + s + ')';
        }
        buf += ", ";
    }

    if (buf.ends_with(", ")) {
        buf.erase(buf.size() - 2);
        buf += '\n';
    }

    if (get_info_clear(mi, s, Stream_Video, num, "Title")) {
        buf += "Title: " + s + '\n';
    }
}


static void get_audio_stream_infos(MediaInfo &mi, std::string &buf, size_t num, size_t count)
{
    std::string s;

    buf += "\nAudio";

    if (count > 1) {
        buf += ' ' + std::to_string(num + 1);
    }
    buf += '\n';

    if (get_info(mi, buf, Stream_Audio, num, "Language/String")) {
        buf += ", ";
    }

    if (get_info(mi, buf, Stream_Audio, num, "BitRate/String")) {
        if (get_info_clear(mi, s, Stream_Audio, num, "BitRate_Mode/String")) {
            buf += " (" + s + ')';
        }
        buf += ", ";
    }

    if (get_info(mi, buf, Stream_Audio, num, "Channel(s)/String")) {
        buf += ", ";
    }
    if (get_info(mi, buf, Stream_Audio, num, "SamplingRate/String")) {
        buf += ", ";
    }
    if (get_info(mi, buf, Stream_Audio, num, "BitDepth/String")) {
        buf += ", ";
    }

    if (get_info(mi, buf, Stream_Audio, num, "Format")) {
        if (get_info_clear(mi, s, Stream_Audio, num, "Format_Profile")) {
            buf += " (" + s + ')';
        }
        buf += ", ";
    }

    if (buf.ends_with(", ")) {
        buf.erase(buf.size() - 2);
        buf += '\n';
    }

    if (get_info_clear(mi, s, Stream_Audio, num, "Title")) {
        buf += "Title: " + s + '\n';
    }
}


static void get_text_stream_infos(MediaInfo &mi, std::string &buf, size_t num, size_t count)
{
    std::string s;

    buf += "\nText";

    if (count > 1) {
        buf += ' ' + std::to_string(num + 1);
    }
    buf += '\n';

    if (get_info(mi, buf, Stream_Text, num, "Language/String")) {
        buf += ", ";
    }

    if (get_info(mi, buf, Stream_Text, num, "Format")) {
        if (get_info_clear(mi, s, Stream_Text, num, "MuxingMode")) {
            buf += " (" + s + ')';
        }
        buf += ", ";
    }

    if (buf.ends_with(", ")) {
        buf.erase(buf.size() - 2);
        buf += '\n';
    }

    if (get_info_clear(mi, s, Stream_Text, num, "Title")) {
        buf += "Title: " + s + '\n';
    }
}


std::string MIGui::get_brief_data(MediaInfo &mi)
{
    std::string buf, file, s;

    /* generic info */

    if (get_info(mi, buf, Stream_General, 0, "CompleteName")) {
        buf += "\n\n";
    }

    if (get_info(mi, file, Stream_General, 0, "Format")) {
        if (get_info_clear(mi, s, Stream_General, 0, "Format_Version")) {
            file += " (" + s + ')';
        }
        file += ", ";
    }
    if (get_info(mi, file, Stream_General, 0, "FileSize/String")) {
        file += ", ";
    }
    if (get_info(mi, file, Stream_General, 0, "Duration/String")) {
        file += ", ";
    }

    if (file.ends_with(", ")) {
        file.erase(file.size() - 2);
        buf += "File: " + file + '\n';
    }

    size_t video_count = get_stream_count(mi, buf, Stream_Video, "Video");
    size_t audio_count = get_stream_count(mi, buf, Stream_Audio, "Audio");
    size_t text_count = get_stream_count(mi, buf, Stream_Text, "Text");
    get_stream_count(mi, buf, Stream_Image, "Image");
    get_menu_count(mi, buf);
    get_stream_count(mi, buf, Stream_Other, "Other");

    if (get_info_clear(mi, s, Stream_General, 0, "Title")) {
        buf += "Title: " + s + '\n';
    }
    if (get_info_clear(mi, s, Stream_General, 0, "Encoded_Date")) {
        buf += "Encoded Date: " + s + '\n';
    }
    if (get_info_clear(mi, s, Stream_General, 0, "Encoded_Application")) {
        buf += "Writing application: " + s + '\n';
    }
    if (get_info_clear(mi, s, Stream_General, 0, "Encoded_Library")) {
        buf += "Writing library: " + s + '\n';
    }
    if (get_info_clear(mi, s, Stream_General, 0, "Attachments")) {
        buf += "Attachments: " + s + '\n';
    }

    /* iterate through streams */

    for (size_t i = 0; i < video_count; i++) {
        get_video_stream_infos(mi, buf, i, video_count);
    }

    for (size_t i = 0; i < audio_count; i++) {
        get_audio_stream_infos(mi, buf, i, audio_count);
    }

    for (size_t i = 0; i < text_count; i++) {
        get_text_stream_infos(mi, buf, i, text_count);
    }

    return buf;
}

