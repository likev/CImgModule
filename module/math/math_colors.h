#ifndef CIMG_MODULE_MATH_COLORS_H
#define CIMG_MODULE_MATH_COLORS_H

namespace cimg_library {
  namespace cimg {

    // Define character sequences for colored terminal output.
#ifdef cimg_use_vt100
    static const char t_normal[] = { 0x1b, '[', '0', ';', '0', ';', '0', 'm', 0 };
    static const char t_black[] = { 0x1b, '[', '0', ';', '3', '0', ';', '5', '9', 'm', 0 };
    static const char t_red[] = { 0x1b, '[', '0', ';', '3', '1', ';', '5', '9', 'm', 0 };
    static const char t_green[] = { 0x1b, '[', '0', ';', '3', '2', ';', '5', '9', 'm', 0 };
    static const char t_yellow[] = { 0x1b, '[', '0', ';', '3', '3', ';', '5', '9', 'm', 0 };
    static const char t_blue[] = { 0x1b, '[', '0', ';', '3', '4', ';', '5', '9', 'm', 0 };
    static const char t_magenta[] = { 0x1b, '[', '0', ';', '3', '5', ';', '5', '9', 'm', 0 };
    static const char t_cyan[] = { 0x1b, '[', '0', ';', '3', '6', ';', '5', '9', 'm', 0 };
    static const char t_white[] = { 0x1b, '[', '0', ';', '3', '7', ';', '5', '9', 'm', 0 };
    static const char t_bold[] = { 0x1b, '[', '1', 'm', 0 };
    static const char t_underscore[] = { 0x1b, '[', '4', 'm', 0 };
#else
    static const char t_normal[] = { 0 };
    static const char *const t_black = cimg::t_normal,
      *const t_red = cimg::t_normal,
      *const t_green = cimg::t_normal,
      *const t_yellow = cimg::t_normal,
      *const t_blue = cimg::t_normal,
      *const t_magenta = cimg::t_normal,
      *const t_cyan = cimg::t_normal,
      *const t_white = cimg::t_normal,
      *const t_bold = cimg::t_normal,
      *const t_underscore = cimg::t_normal;
#endif

  }
}

#endif
