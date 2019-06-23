/*
 *  Copyright (c) 2018-2019, djcj <djcj@gmx.de>
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

#include <MediaInfo/MediaInfo.h>
#include <ZenLib/Ztring.h>

#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>

using namespace MediaInfoLib;
using namespace ZenLib;

void get_info(MediaInfo &mi, Ztring &info)
{
  Ztring ztr;
  int video_count, audio_count, text_count, count;

  info.clear();

  ztr = mi.Get(Stream_General, 0, __T("CompleteName"));
  if (!ztr.empty()) {
    info += ztr + __T("\n\n");
  }

  ztr = mi.Get(Stream_General, 0, __T("Format"));
  if (!ztr.empty()) {
    info += ztr + __T(": ");
  }
  ztr = mi.Get(Stream_General, 0, __T("FileSize/String"));
  if (!ztr.empty()) {
    info += ztr + __T(", ");
  }
  ztr = mi.Get(Stream_General, 0, __T("Duration/String"));
  if (!ztr.empty()) {
    info += ztr + __T(", ");
  }

  ztr = info.substr(info.length() - 2);
  if (ztr == __T(": ") || ztr == __T(", ")) {
    info.pop_back();
    info.pop_back();
    info += __T("\n");
  }

  if ((video_count = mi.Count_Get(Stream_Video, -1)) > 0) {
    info += __T("Video streams: ");
    ztr = mi.Get(Stream_General, 0, __T("Video_Format_List"));
    if (ztr.empty()) {
      info += Ztring::ToZtring(video_count);
    } else {
      info += ztr;
    }
    info += __T("\n");
  }

  if ((audio_count = mi.Count_Get(Stream_Audio, -1)) > 0) {
    info += __T("Audio streams: ");
    ztr = mi.Get(Stream_General, 0, __T("Audio_Format_List"));
    if (ztr.empty()) {
      info += Ztring::ToZtring(audio_count);
    } else {
      info += ztr;
    }
    info += __T("\n");
  }

  if ((text_count = mi.Count_Get(Stream_Text, -1)) > 0) {
    info += __T("Text streams: ");
    ztr = mi.Get(Stream_General, 0, __T("Text_Format_List"));
    if (ztr.empty()) {
      info += Ztring::ToZtring(text_count);
    } else {
      info += ztr;
    }
    info += __T("\n");
  }

  if ((count = mi.Count_Get(Stream_Image, -1)) > 0) {
    info += __T("Image streams: ");
    ztr = mi.Get(Stream_General, 0, __T("Image_Format_List"));
    if (ztr.empty()) {
      info += Ztring::ToZtring(count);
    } else {
      info += ztr;
    }
    info += __T("\n");
  }

  if ((count = mi.Count_Get(Stream_Menu, -1)) > 0) {
    info += __T("Menu streams: ");
    ztr = mi.Get(Stream_General, 0, __T("Menu_Format_List"));
    if (ztr.empty()) {
      info += Ztring::ToZtring(count);
    } else {
      info += ztr;
    }
    info += __T("\n");
  }

  if ((count = mi.Count_Get(Stream_Other, -1)) > 0) {
    info += __T("Other streams: ");
    ztr = mi.Get(Stream_General, 0, __T("Other_Format_List"));
    if (ztr.empty()) {
      info += Ztring::ToZtring(count);
    } else {
      info += ztr;
    }
    info += __T("\n");
  }

  ztr = mi.Get(Stream_General, 0, __T("Title"));
  if (!ztr.empty()) {
    info += __T("Title: ") + ztr + __T("\n");
  }
  ztr = mi.Get(Stream_General, 0, __T("Encoded_Date"));
  if (!ztr.empty()) {
    info += __T("Encoded Date: ") + ztr + __T("\n");
  }
  ztr = mi.Get(Stream_General, 0, __T("Encoded_Application"));
  if (!ztr.empty()) {
    info += __T("Writing application: ") + ztr + __T("\n");
  }
  ztr = mi.Get(Stream_General, 0, __T("Encoded_Library"));
  if (!ztr.empty()) {
    info += __T("Writing library: ") + ztr + __T("\n");
  }

  ztr = mi.Get(Stream_General, 0, __T("Attachments"));
  if (!ztr.empty()) {
    info += __T("Attachments: ") + ztr + __T("\n");
  }


  /* iterate through video streams */

  for (int i = 0; i < video_count; ++i) {
    info += __T("\nVideo");
    if (video_count > 1) {
      info += __T(" ") + Ztring::ToZtring(i + 1);
    }
    info += __T("\n");

    ztr = mi.Get(Stream_Video, i, __T("Language/String"));
    if (!ztr.empty()) {
      info += ztr + __T(", ");
    }

    Ztring width = mi.Get(Stream_Video, i, __T("Width"));
    Ztring height = mi.Get(Stream_Video, i, __T("Height"));
    if (!width.empty() && !height.empty()) {
      info += width + __T("*") + height;
      ztr = mi.Get(Stream_Video, i, __T("DisplayAspectRatio/String"));
      if (!ztr.empty()) {
        info += __T(" (") + ztr + __T(")");
      }
      info += __T(", ");
    }

    ztr = mi.Get(Stream_Video, i, __T("FrameRate/String"));
    if (!ztr.empty()) {
      info += ztr + __T(", ");
    }
    ztr = mi.Get(Stream_Video, i, __T("BitRate/String"));
    if (!ztr.empty()) {
      info += ztr + __T(", ");
    }
    ztr = mi.Get(Stream_Video, i, __T("Format"));
    if (!ztr.empty()) {
      info += ztr;
      ztr = mi.Get(Stream_Video, i, __T("Format_Profile"));
      if (!ztr.empty()) {
        info += __T(" (") + ztr + __T(")");
      }
      info += __T(", ");
    }

    if (info.substr(info.length() - 2) == __T(", ")) {
      info.pop_back();
      info.pop_back();
      info += __T("\n");
    }

    ztr = mi.Get(Stream_Video, i, __T("Title"));
    if (!ztr.empty()) {
      info += __T("Title: ") + ztr + __T("\n");
    }
  }


  /* iterate through audio streams */

  for (int i = 0; i < audio_count; ++i) {
    info += __T("\nAudio ");
    if (audio_count > 1) {
      info += __T(" ") + Ztring::ToZtring(i + 1);
    }
    info += __T("\n");

    ztr = mi.Get(Stream_Audio, i, __T("Language/String"));
    if (!ztr.empty()) {
      info += ztr + __T(", ");
    }

    ztr = mi.Get(Stream_Audio, i, __T("BitRate/String"));
    if (!ztr.empty()) {
      info += ztr;
      ztr = mi.Get(Stream_Audio, i, __T("BitRate_Mode/String"));
      if (!ztr.empty()) {
        info += __T(" (") + ztr + __T(")");
      }
      info += __T(", ");
    }

    ztr = mi.Get(Stream_Audio, i, __T("Channel(s)/String"));
    if (!ztr.empty()) {
      info += ztr + __T(", ");
    }
    ztr = mi.Get(Stream_Audio, i, __T("SamplingRate/String"));
    if (!ztr.empty()) {
      info += ztr + __T(", ");
    }
    ztr = mi.Get(Stream_Audio, i, __T("BitDepth/String"));
    if (!ztr.empty()) {
      info += ztr + __T(", ");
    }
    ztr = mi.Get(Stream_Audio, i, __T("Format"));
    if (!ztr.empty()) {
      info += ztr;
      ztr = mi.Get(Stream_Audio, i, __T("Format_Profile"));
      if (!ztr.empty()) {
        info += __T(" ") + ztr;
      }
      info += __T(", ");
    }

    if (info.substr(info.length() - 2) == __T(", ")) {
      info.pop_back();
      info.pop_back();
      info += __T("\n");
    }

    ztr = mi.Get(Stream_Audio, i, __T("Title"));
    if (!ztr.empty()) {
      info += __T("Title: ") + ztr + __T("\n");
    }
  }


  /* iterate through text streams */

  for (int i = 0; i < text_count; ++i) {
    info += __T("\nText");
    if (text_count > 1) {
      info += __T(" ") + Ztring::ToZtring(i + 1);
    }
    info += __T("\n");

    ztr = mi.Get(Stream_Text, i, __T("Language/String"));
    if (!ztr.empty()) {
      info += ztr + __T(", ");
    }

    ztr = mi.Get(Stream_Text, i, __T("Format"));
    if (!ztr.empty()) {
      info += ztr;
      ztr = mi.Get(Stream_Text, i, __T("MuxingMode"));
      if (!ztr.empty()) {
        info += __T(" (") + ztr + __T(")");
      }
      info += __T(", ");
    }

    if (info.substr(info.length() - 2) == __T(", ")) {
      info.pop_back();
      info.pop_back();
      info += __T("\n");
    }

    ztr = mi.Get(Stream_Text, i, __T("Title"));
    if (!ztr.empty()) {
      info += __T("Title: ") + ztr + __T("\n");
    }
  }
}

