#ifndef CIMG_MODULE_DISPLAY_BASE_H
#define CIMG_MODULE_DISPLAY_BASE_H


  /*------------------------------------------------
   #
   #

   # Define the CImgDisplay structure
   #
   ----------------------------------*/
  //! Allow the creation of windows, display images on them and manage user events (keyboard, mouse and windows events).
  /**
     CImgDisplay methods rely on a low-level graphic library to perform: it can be either \b X-Window
     (X11, for Unix-based systems) or \b GDI32 (for Windows-based systems).
     If both libraries are missing, CImgDisplay will not be able to display images on screen, and will enter
     a minimal mode where warning messages will be outputted each time the program is trying to call one of the
     CImgDisplay method.

     The configuration variable \c cimg_display tells about the graphic library used.
     It is set automatically by \CImg when one of these graphic libraries has been detected.
     But, you can override its value if necessary. Valid choices are:
     - 0: Disable display capabilities.
     - 1: Use \b X-Window (X11) library.
     - 2: Use \b GDI32 library.

     Remember to link your program against \b X11 or \b GDI32 libraries if you use CImgDisplay.
  **/
  struct CImgDisplay {
    cimg_uint64 _timer, _fps_frames, _fps_timer;
    unsigned int _width, _height, _normalization;
    float _fps_fps, _min, _max;
    bool _is_fullscreen;
    char *_title;
    unsigned int _window_width, _window_height, _button, *_keys, *_released_keys;
    int _window_x, _window_y, _mouse_x, _mouse_y, _wheel;
    bool _is_closed, _is_resized, _is_moved, _is_event,
      _is_keyESC, _is_keyF1, _is_keyF2, _is_keyF3, _is_keyF4, _is_keyF5, _is_keyF6, _is_keyF7,
      _is_keyF8, _is_keyF9, _is_keyF10, _is_keyF11, _is_keyF12, _is_keyPAUSE, _is_key1, _is_key2,
      _is_key3, _is_key4, _is_key5, _is_key6, _is_key7, _is_key8, _is_key9, _is_key0,
      _is_keyBACKSPACE, _is_keyINSERT, _is_keyHOME, _is_keyPAGEUP, _is_keyTAB, _is_keyQ, _is_keyW, _is_keyE,
      _is_keyR, _is_keyT, _is_keyY, _is_keyU, _is_keyI, _is_keyO, _is_keyP, _is_keyDELETE,
      _is_keyEND, _is_keyPAGEDOWN, _is_keyCAPSLOCK, _is_keyA, _is_keyS, _is_keyD, _is_keyF, _is_keyG,
      _is_keyH, _is_keyJ, _is_keyK, _is_keyL, _is_keyRETURN, _is_keySHIFTLEFT, _is_keyZ, _is_keyX,
      _is_keyC, _is_keyV, _is_keyB, _is_keyN, _is_keyM, _is_keySHIFTRIGHT, _is_keyARROWUP, _is_keyCTRLLEFT,
      _is_keyAPPLEFT, _is_keyALT, _is_keySPACE, _is_keyALTGR, _is_keyAPPRIGHT, _is_keyMENU, _is_keyCTRLRIGHT,
      _is_keyARROWLEFT, _is_keyARROWDOWN, _is_keyARROWRIGHT, _is_keyPAD0, _is_keyPAD1, _is_keyPAD2, _is_keyPAD3,
      _is_keyPAD4, _is_keyPAD5, _is_keyPAD6, _is_keyPAD7, _is_keyPAD8, _is_keyPAD9, _is_keyPADADD, _is_keyPADSUB,
      _is_keyPADMUL, _is_keyPADDIV, _is_keyENTER;

    //@}
    //---------------------------
    //
    //! \name Plugins
    //@{
    //---------------------------

#ifdef cimgdisplay_plugin
#include cimgdisplay_plugin
#endif
#ifdef cimgdisplay_plugin1
#include cimgdisplay_plugin1
#endif
#ifdef cimgdisplay_plugin2
#include cimgdisplay_plugin2
#endif
#ifdef cimgdisplay_plugin3
#include cimgdisplay_plugin3
#endif
#ifdef cimgdisplay_plugin4
#include cimgdisplay_plugin4
#endif
#ifdef cimgdisplay_plugin5
#include cimgdisplay_plugin5
#endif
#ifdef cimgdisplay_plugin6
#include cimgdisplay_plugin6
#endif
#ifdef cimgdisplay_plugin7
#include cimgdisplay_plugin7
#endif
#ifdef cimgdisplay_plugin8
#include cimgdisplay_plugin8
#endif

    //@}
    //--------------------------------------------------------
    //
    //! \name Constructors / Destructor / Instance Management
    //@{
    //--------------------------------------------------------

    //! Destructor.
    /**
       \note If the associated window is visible on the screen, it is closed by the call to the destructor.
    **/
    ~CImgDisplay() {
      assign();
      delete[] _keys;
      delete[] _released_keys;
    }

    //! Construct an empty display.
    /**
       \note Constructing an empty CImgDisplay instance does not make a window appearing on the screen, until
       display of valid data is performed.
       \par Example
       \code
       CImgDisplay disp; // Does actually nothing
       ...
       disp.display(img); // Construct new window and display image in it
       \endcode
    **/
    CImgDisplay():
      _width(0),_height(0),_normalization(0),
      _min(0),_max(0),
      _is_fullscreen(false),
      _title(0),
      _window_width(0),_window_height(0),_button(0),_keys(0),_released_keys(0),
      _window_x(cimg::type<int>::min()),_window_y(cimg::type<int>::min()),
      _mouse_x(-1),_mouse_y(-1),_wheel(0),
      _is_closed(true),_is_resized(false),_is_moved(false),_is_event(false) {
      assign();
    }

    //! Construct a display with specified dimensions.
    /** \param width Window width.
        \param height Window height.
        \param title Window title.
        \param normalization Normalization type
        (<tt>0</tt>=none, <tt>1</tt>=always, <tt>2</tt>=once, <tt>3</tt>=pixel type-dependent, see normalization()).
        \param is_fullscreen Tells if fullscreen mode is enabled.
        \param is_closed Tells if associated window is initially visible or not.
        \note A black background is initially displayed on the associated window.
    **/
    CImgDisplay(const unsigned int width, const unsigned int height,
                const char *const title=0, const unsigned int normalization=3,
                const bool is_fullscreen=false, const bool is_closed=false):
      _width(0),_height(0),_normalization(0),
      _min(0),_max(0),
      _is_fullscreen(false),
      _title(0),
      _window_width(0),_window_height(0),_button(0),_keys(0),_released_keys(0),
      _window_x(cimg::type<int>::min()),_window_y(cimg::type<int>::min()),
      _mouse_x(-1),_mouse_y(-1),_wheel(0),
      _is_closed(true),_is_resized(false),_is_moved(false),_is_event(false) {
      assign(width,height,title,normalization,is_fullscreen,is_closed);
    }

    //! Construct a display from an image.
    /** \param img Image used as a model to create the window.
        \param title Window title.
        \param normalization Normalization type
        (<tt>0</tt>=none, <tt>1</tt>=always, <tt>2</tt>=once, <tt>3</tt>=pixel type-dependent, see normalization()).
        \param is_fullscreen Tells if fullscreen mode is enabled.
        \param is_closed Tells if associated window is initially visible or not.
        \note The pixels of the input image are initially displayed on the associated window.
    **/
    template<typename T>
    explicit CImgDisplay(const CImg<T>& img,
                         const char *const title=0, const unsigned int normalization=3,
                         const bool is_fullscreen=false, const bool is_closed=false):
      _width(0),_height(0),_normalization(0),
      _min(0),_max(0),
      _is_fullscreen(false),
      _title(0),
      _window_width(0),_window_height(0),_button(0),_keys(0),_released_keys(0),
      _window_x(cimg::type<int>::min()),_window_y(cimg::type<int>::min()),
      _mouse_x(-1),_mouse_y(-1),_wheel(0),
      _is_closed(true),_is_resized(false),_is_moved(false),_is_event(false) {
      assign(img,title,normalization,is_fullscreen,is_closed);
    }

    //! Construct a display from an image list.
    /** \param list The images list to display.
        \param title Window title.
        \param normalization Normalization type
        (<tt>0</tt>=none, <tt>1</tt>=always, <tt>2</tt>=once, <tt>3</tt>=pixel type-dependent, see normalization()).
        \param is_fullscreen Tells if fullscreen mode is enabled.
        \param is_closed Tells if associated window is initially visible or not.
        \note All images of the list, appended along the X-axis, are initially displayed on the associated window.
    **/
    template<typename T>
    explicit CImgDisplay(const CImgList<T>& list,
                         const char *const title=0, const unsigned int normalization=3,
                         const bool is_fullscreen=false, const bool is_closed=false):
      _width(0),_height(0),_normalization(0),
      _min(0),_max(0),
      _is_fullscreen(false),
      _title(0),
      _window_width(0),_window_height(0),_button(0),_keys(0),_released_keys(0),
      _window_x(cimg::type<int>::min()),_window_y(cimg::type<int>::min()),
      _mouse_x(-1),_mouse_y(-1),_wheel(0),
      _is_closed(true),_is_resized(false),_is_moved(false),_is_event(false) {
      assign(list,title,normalization,is_fullscreen,is_closed);
    }

    //! Construct a display as a copy of an existing one.
    /**
        \param disp Display instance to copy.
        \note The pixel buffer of the input window is initially displayed on the associated window.
    **/
    CImgDisplay(const CImgDisplay& disp):
      _width(0),_height(0),_normalization(0),
      _min(0),_max(0),
      _is_fullscreen(false),
      _title(0),
      _window_width(0),_window_height(0),_button(0),_keys(0),_released_keys(0),
      _window_x(cimg::type<int>::min()),_window_y(cimg::type<int>::min()),
      _mouse_x(-1),_mouse_y(-1),_wheel(0),
      _is_closed(true),_is_resized(false),_is_moved(false),_is_event(false) {
      assign(disp);
    }

    //! Take a screenshot.
    /**
       \param[out] img Output screenshot. Can be empty on input
    **/
    template<typename T>
    static void screenshot(CImg<T>& img) {
      return screenshot(0,0,cimg::type<int>::max(),cimg::type<int>::max(),img);
    }

#if cimg_display==0

    static void _no_display_exception() {
      throw CImgDisplayException("CImgDisplay(): No display available.");
    }

    //! Destructor - Empty constructor \inplace.
    /**
       \note Replace the current instance by an empty display.
    **/
    CImgDisplay& assign() {
      return flush();
    }

    //! Construct a display with specified dimensions \inplace.
    /**
    **/
    CImgDisplay& assign(const unsigned int width, const unsigned int height,
                        const char *const title=0, const unsigned int normalization=3,
                        const bool is_fullscreen=false, const bool is_closed=false) {
      cimg::unused(width,height,title,normalization,is_fullscreen,is_closed);
      _no_display_exception();
      return assign();
    }

    //! Construct a display from an image \inplace.
    /**
    **/
    template<typename T>
    CImgDisplay& assign(const CImg<T>& img,
                        const char *const title=0, const unsigned int normalization=3,
                        const bool is_fullscreen=false, const bool is_closed=false) {
      _no_display_exception();
      return assign(img._width,img._height,title,normalization,is_fullscreen,is_closed);
    }

    //! Construct a display from an image list \inplace.
    /**
    **/
    template<typename T>
    CImgDisplay& assign(const CImgList<T>& list,
                        const char *const title=0, const unsigned int normalization=3,
                        const bool is_fullscreen=false, const bool is_closed=false) {
      _no_display_exception();
      return assign(list._width,list._width,title,normalization,is_fullscreen,is_closed);
    }

    //! Construct a display as a copy of another one \inplace.
    /**
    **/
    CImgDisplay& assign(const CImgDisplay &disp) {
      _no_display_exception();
      return assign(disp._width,disp._height);
    }

#endif

    //! Return a reference to an empty display.
    /**
       \note Can be useful for writing function prototypes where one of the argument (of type CImgDisplay&)
       must have a default value.
       \par Example
       \code
       void foo(CImgDisplay& disp=CImgDisplay::empty());
       \endcode
    **/
    static CImgDisplay& empty() {
      static CImgDisplay _empty;
      return _empty.assign();
    }

    //! Return a reference to an empty display \const.
    static const CImgDisplay& const_empty() {
      static const CImgDisplay _empty;
      return _empty;
    }

#define cimg_fitscreen(dx,dy,dz) CImgDisplay::_fitscreen(dx,dy,dz,-25,-85,false), \
                                 CImgDisplay::_fitscreen(dx,dy,dz,-25,-85,true)
    static unsigned int _fitscreen(const unsigned int dx, const unsigned int dy, const unsigned int dz,
                                   const int dmin, const int dmax, const bool return_y) {
      const int
        u = CImgDisplay::screen_width(),
        v = CImgDisplay::screen_height();
      const float
        mw = dmin<0?cimg::round(u*-dmin/100.f):(float)dmin,
        mh = dmin<0?cimg::round(v*-dmin/100.f):(float)dmin,
        Mw = dmax<0?cimg::round(u*-dmax/100.f):(float)dmax,
        Mh = dmax<0?cimg::round(v*-dmax/100.f):(float)dmax;
      float
        w = (float)std::max(1U,dx),
        h = (float)std::max(1U,dy);
      if (dz>1) { w+=dz; h+=dz; }
      if (w<mw) { h = h*mw/w; w = mw; }
      if (h<mh) { w = w*mh/h; h = mh; }
      if (w>Mw) { h = h*Mw/w; w = Mw; }
      if (h>Mh) { w = w*Mh/h; h = Mh; }
      if (w<mw) w = mw;
      if (h<mh) h = mh;
      return std::max(1U,(unsigned int)cimg::round(return_y?h:w));
    }

    //@}
    //------------------------------------------
    //
    //! \name Overloaded Operators
    //@{
    //------------------------------------------

    //! Display image on associated window.
    /**
       \note <tt>disp = img</tt> is equivalent to <tt>disp.display(img)</tt>.
    **/
    template<typename t>
    CImgDisplay& operator=(const CImg<t>& img) {
      return display(img);
    }

    //! Display list of images on associated window.
    /**
       \note <tt>disp = list</tt> is equivalent to <tt>disp.display(list)</tt>.
    **/
    template<typename t>
    CImgDisplay& operator=(const CImgList<t>& list) {
      return display(list);
    }

    //! Construct a display as a copy of another one \inplace.
    /**
       \note Equivalent to assign(const CImgDisplay&).
     **/
    CImgDisplay& operator=(const CImgDisplay& disp) {
      return assign(disp);
    }

    //! Return \c false if display is empty, \c true otherwise.
    /**
       \note <tt>if (disp) { ... }</tt> is equivalent to <tt>if (!disp.is_empty()) { ... }</tt>.
    **/
    operator bool() const {
      return !is_empty();
    }

    //@}
    //------------------------------------------
    //
    //! \name Instance Checking
    //@{
    //------------------------------------------

    //! Return \c true if display is empty, \c false otherwise.
    /**
    **/
    bool is_empty() const {
      return !(_width && _height);
    }

    //! Return \c true if display is closed (i.e. not visible on the screen), \c false otherwise.
    /**
       \note
       - When a user physically closes the associated window, the display is set to closed.
       - A closed display is not destroyed. Its associated window can be show again on the screen using show().
    **/
    bool is_closed() const {
      return _is_closed;
    }

    //! Return \c true if display is visible (i.e. not closed by the user), \c false otherwise.
    bool is_visible() const {
      return !is_closed();
    }

    //! Return \c true if associated window has been resized on the screen, \c false otherwise.
    /**
    **/
    bool is_resized() const {
      return _is_resized;
    }

    //! Return \c true if associated window has been moved on the screen, \c false otherwise.
    /**
    **/
    bool is_moved() const {
      return _is_moved;
    }

    //! Return \c true if any event has occurred on the associated window, \c false otherwise.
    /**
    **/
    bool is_event() const {
      return _is_event;
    }

    //! Return \c true if current display is in fullscreen mode, \c false otherwise.
    /**
    **/
    bool is_fullscreen() const {
      return _is_fullscreen;
    }

    //! Return \c true if any key is being pressed on the associated window, \c false otherwise.
    /**
       \note The methods below do the same only for specific keys.
    **/
    bool is_key() const {
      return _is_keyESC || _is_keyF1 || _is_keyF2 || _is_keyF3 ||
        _is_keyF4 || _is_keyF5 || _is_keyF6 || _is_keyF7 ||
        _is_keyF8 || _is_keyF9 || _is_keyF10 || _is_keyF11 ||
        _is_keyF12 || _is_keyPAUSE || _is_key1 || _is_key2 ||
        _is_key3 || _is_key4 || _is_key5 || _is_key6 ||
        _is_key7 || _is_key8 || _is_key9 || _is_key0 ||
        _is_keyBACKSPACE || _is_keyINSERT || _is_keyHOME ||
        _is_keyPAGEUP || _is_keyTAB || _is_keyQ || _is_keyW ||
        _is_keyE || _is_keyR || _is_keyT || _is_keyY ||
        _is_keyU || _is_keyI || _is_keyO || _is_keyP ||
        _is_keyDELETE || _is_keyEND || _is_keyPAGEDOWN ||
        _is_keyCAPSLOCK || _is_keyA || _is_keyS || _is_keyD ||
        _is_keyF || _is_keyG || _is_keyH || _is_keyJ ||
        _is_keyK || _is_keyL || _is_keyRETURN ||
        _is_keySHIFTLEFT || _is_keyZ || _is_keyX || _is_keyC ||
        _is_keyV || _is_keyB || _is_keyN || _is_keyM ||
        _is_keySHIFTRIGHT || _is_keyARROWUP || _is_keyCTRLLEFT ||
        _is_keyAPPLEFT || _is_keyALT || _is_keySPACE || _is_keyALTGR ||
        _is_keyAPPRIGHT || _is_keyMENU || _is_keyCTRLRIGHT ||
        _is_keyARROWLEFT || _is_keyARROWDOWN || _is_keyARROWRIGHT ||
        _is_keyPAD0 || _is_keyPAD1 || _is_keyPAD2 ||
        _is_keyPAD3 || _is_keyPAD4 || _is_keyPAD5 ||
        _is_keyPAD6 || _is_keyPAD7 || _is_keyPAD8 ||
        _is_keyPAD9 || _is_keyPADADD || _is_keyPADSUB ||
        _is_keyPADMUL || _is_keyPADDIV || _is_keyENTER;
    }

    //! Return \c true if key specified by given keycode is being pressed on the associated window, \c false otherwise.
    /**
       \param keycode Keycode to test.
       \note Keycode constants are defined in the cimg namespace and are architecture-dependent. Use them to ensure
       your code stay portable (see cimg::keyESC).
       \par Example
       \code
       CImgDisplay disp(400,400);
       while (!disp.is_closed()) {
         if (disp.key(cimg::keyTAB)) { ... } // Equivalent to 'if (disp.is_keyTAB())'
         disp.wait();
       }
       \endcode
    **/
    bool is_key(const unsigned int keycode) const {
#define _cimg_iskey_test(k) if (keycode==cimg::key##k) return _is_key##k;
      _cimg_iskey_test(ESC); _cimg_iskey_test(F1); _cimg_iskey_test(F2); _cimg_iskey_test(F3);
      _cimg_iskey_test(F4); _cimg_iskey_test(F5); _cimg_iskey_test(F6); _cimg_iskey_test(F7);
      _cimg_iskey_test(F8); _cimg_iskey_test(F9); _cimg_iskey_test(F10); _cimg_iskey_test(F11);
      _cimg_iskey_test(F12); _cimg_iskey_test(PAUSE); _cimg_iskey_test(1); _cimg_iskey_test(2);
      _cimg_iskey_test(3); _cimg_iskey_test(4); _cimg_iskey_test(5); _cimg_iskey_test(6);
      _cimg_iskey_test(7); _cimg_iskey_test(8); _cimg_iskey_test(9); _cimg_iskey_test(0);
      _cimg_iskey_test(BACKSPACE); _cimg_iskey_test(INSERT); _cimg_iskey_test(HOME);
      _cimg_iskey_test(PAGEUP); _cimg_iskey_test(TAB); _cimg_iskey_test(Q); _cimg_iskey_test(W);
      _cimg_iskey_test(E); _cimg_iskey_test(R); _cimg_iskey_test(T); _cimg_iskey_test(Y);
      _cimg_iskey_test(U); _cimg_iskey_test(I); _cimg_iskey_test(O); _cimg_iskey_test(P);
      _cimg_iskey_test(DELETE); _cimg_iskey_test(END); _cimg_iskey_test(PAGEDOWN);
      _cimg_iskey_test(CAPSLOCK); _cimg_iskey_test(A); _cimg_iskey_test(S); _cimg_iskey_test(D);
      _cimg_iskey_test(F); _cimg_iskey_test(G); _cimg_iskey_test(H); _cimg_iskey_test(J);
      _cimg_iskey_test(K); _cimg_iskey_test(L); _cimg_iskey_test(RETURN);
      _cimg_iskey_test(SHIFTLEFT); _cimg_iskey_test(Z); _cimg_iskey_test(X); _cimg_iskey_test(C);
      _cimg_iskey_test(V); _cimg_iskey_test(B); _cimg_iskey_test(N); _cimg_iskey_test(M);
      _cimg_iskey_test(SHIFTRIGHT); _cimg_iskey_test(ARROWUP); _cimg_iskey_test(CTRLLEFT);
      _cimg_iskey_test(APPLEFT); _cimg_iskey_test(ALT); _cimg_iskey_test(SPACE); _cimg_iskey_test(ALTGR);
      _cimg_iskey_test(APPRIGHT); _cimg_iskey_test(MENU); _cimg_iskey_test(CTRLRIGHT);
      _cimg_iskey_test(ARROWLEFT); _cimg_iskey_test(ARROWDOWN); _cimg_iskey_test(ARROWRIGHT);
      _cimg_iskey_test(PAD0); _cimg_iskey_test(PAD1); _cimg_iskey_test(PAD2);
      _cimg_iskey_test(PAD3); _cimg_iskey_test(PAD4); _cimg_iskey_test(PAD5);
      _cimg_iskey_test(PAD6); _cimg_iskey_test(PAD7); _cimg_iskey_test(PAD8);
      _cimg_iskey_test(PAD9); _cimg_iskey_test(PADADD); _cimg_iskey_test(PADSUB);
      _cimg_iskey_test(PADMUL); _cimg_iskey_test(PADDIV); _cimg_iskey_test(ENTER);
      return false;
    }

    //! Return \c true if key specified by given keycode is being pressed on the associated window, \c false otherwise.
    /**
       \param keycode C-string containing the keycode label of the key to test.
       \note Use it when the key you want to test can be dynamically set by the user.
       \par Example
       \code
       CImgDisplay disp(400,400);
       const char *const keycode = "TAB";
       while (!disp.is_closed()) {
         if (disp.is_key(keycode)) { ... } // Equivalent to 'if (disp.is_keyTAB())'
         disp.wait();
       }
       \endcode
    **/
    bool& is_key(const char *const keycode) {
      static bool f = false;
      f = false;
#define _cimg_iskey_test2(k) if (!cimg::strcasecmp(keycode,#k)) return _is_key##k;
      _cimg_iskey_test2(ESC); _cimg_iskey_test2(F1); _cimg_iskey_test2(F2); _cimg_iskey_test2(F3);
      _cimg_iskey_test2(F4); _cimg_iskey_test2(F5); _cimg_iskey_test2(F6); _cimg_iskey_test2(F7);
      _cimg_iskey_test2(F8); _cimg_iskey_test2(F9); _cimg_iskey_test2(F10); _cimg_iskey_test2(F11);
      _cimg_iskey_test2(F12); _cimg_iskey_test2(PAUSE); _cimg_iskey_test2(1); _cimg_iskey_test2(2);
      _cimg_iskey_test2(3); _cimg_iskey_test2(4); _cimg_iskey_test2(5); _cimg_iskey_test2(6);
      _cimg_iskey_test2(7); _cimg_iskey_test2(8); _cimg_iskey_test2(9); _cimg_iskey_test2(0);
      _cimg_iskey_test2(BACKSPACE); _cimg_iskey_test2(INSERT); _cimg_iskey_test2(HOME);
      _cimg_iskey_test2(PAGEUP); _cimg_iskey_test2(TAB); _cimg_iskey_test2(Q); _cimg_iskey_test2(W);
      _cimg_iskey_test2(E); _cimg_iskey_test2(R); _cimg_iskey_test2(T); _cimg_iskey_test2(Y);
      _cimg_iskey_test2(U); _cimg_iskey_test2(I); _cimg_iskey_test2(O); _cimg_iskey_test2(P);
      _cimg_iskey_test2(DELETE); _cimg_iskey_test2(END); _cimg_iskey_test2(PAGEDOWN);
      _cimg_iskey_test2(CAPSLOCK); _cimg_iskey_test2(A); _cimg_iskey_test2(S); _cimg_iskey_test2(D);
      _cimg_iskey_test2(F); _cimg_iskey_test2(G); _cimg_iskey_test2(H); _cimg_iskey_test2(J);
      _cimg_iskey_test2(K); _cimg_iskey_test2(L); _cimg_iskey_test2(RETURN);
      _cimg_iskey_test2(SHIFTLEFT); _cimg_iskey_test2(Z); _cimg_iskey_test2(X); _cimg_iskey_test2(C);
      _cimg_iskey_test2(V); _cimg_iskey_test2(B); _cimg_iskey_test2(N); _cimg_iskey_test2(M);
      _cimg_iskey_test2(SHIFTRIGHT); _cimg_iskey_test2(ARROWUP); _cimg_iskey_test2(CTRLLEFT);
      _cimg_iskey_test2(APPLEFT); _cimg_iskey_test2(ALT); _cimg_iskey_test2(SPACE); _cimg_iskey_test2(ALTGR);
      _cimg_iskey_test2(APPRIGHT); _cimg_iskey_test2(MENU); _cimg_iskey_test2(CTRLRIGHT);
      _cimg_iskey_test2(ARROWLEFT); _cimg_iskey_test2(ARROWDOWN); _cimg_iskey_test2(ARROWRIGHT);
      _cimg_iskey_test2(PAD0); _cimg_iskey_test2(PAD1); _cimg_iskey_test2(PAD2);
      _cimg_iskey_test2(PAD3); _cimg_iskey_test2(PAD4); _cimg_iskey_test2(PAD5);
      _cimg_iskey_test2(PAD6); _cimg_iskey_test2(PAD7); _cimg_iskey_test2(PAD8);
      _cimg_iskey_test2(PAD9); _cimg_iskey_test2(PADADD); _cimg_iskey_test2(PADSUB);
      _cimg_iskey_test2(PADMUL); _cimg_iskey_test2(PADDIV); _cimg_iskey_test2(ENTER);
      return f;
    }

    //! Return \c true if specified key sequence has been typed on the associated window, \c false otherwise.
    /**
       \param keycodes_sequence Buffer of keycodes to test.
       \param length Number of keys in the \c keycodes_sequence buffer.
       \param remove_sequence Tells if the key sequence must be removed from the key history, if found.
       \note Keycode constants are defined in the cimg namespace and are architecture-dependent. Use them to ensure
       your code stay portable (see cimg::keyESC).
       \par Example
       \code
       CImgDisplay disp(400,400);
       const unsigned int key_seq[] = { cimg::keyCTRLLEFT, cimg::keyD };
       while (!disp.is_closed()) {
         if (disp.is_key_sequence(key_seq,2)) { ... } // Test for the 'CTRL+D' keyboard event
         disp.wait();
       }
       \endcode
    **/
    bool is_key_sequence(const unsigned int *const keycodes_sequence, const unsigned int length,
                         const bool remove_sequence=false) {
      if (keycodes_sequence && length) {
        const unsigned int
          *const ps_end = keycodes_sequence + length - 1,
          *const pk_end = (unsigned int*)_keys + 1 + 128 - length,
          k = *ps_end;
        for (unsigned int *pk = (unsigned int*)_keys; pk<pk_end; ) {
          if (*(pk++)==k) {
            bool res = true;
            const unsigned int *ps = ps_end, *pk2 = pk;
            for (unsigned int i = 1; i<length; ++i) res = (*(--ps)==*(pk2++));
            if (res) {
              if (remove_sequence) std::memset((void*)(pk - 1),0,sizeof(unsigned int)*length);
              return true;
            }
          }
        }
      }
      return false;
    }

#define _cimg_iskey_def(k) \
    bool is_key##k() const { \
      return _is_key##k; \
    }

    //! Return \c true if the \c ESC key is being pressed on the associated window, \c false otherwise.
    /**
       \note Similar methods exist for all keys managed by \CImg (see cimg::keyESC).
    **/
    _cimg_iskey_def(ESC); _cimg_iskey_def(F1); _cimg_iskey_def(F2); _cimg_iskey_def(F3);
    _cimg_iskey_def(F4); _cimg_iskey_def(F5); _cimg_iskey_def(F6); _cimg_iskey_def(F7);
    _cimg_iskey_def(F8); _cimg_iskey_def(F9); _cimg_iskey_def(F10); _cimg_iskey_def(F11);
    _cimg_iskey_def(F12); _cimg_iskey_def(PAUSE); _cimg_iskey_def(1); _cimg_iskey_def(2);
    _cimg_iskey_def(3); _cimg_iskey_def(4); _cimg_iskey_def(5); _cimg_iskey_def(6);
    _cimg_iskey_def(7); _cimg_iskey_def(8); _cimg_iskey_def(9); _cimg_iskey_def(0);
    _cimg_iskey_def(BACKSPACE); _cimg_iskey_def(INSERT); _cimg_iskey_def(HOME);
    _cimg_iskey_def(PAGEUP); _cimg_iskey_def(TAB); _cimg_iskey_def(Q); _cimg_iskey_def(W);
    _cimg_iskey_def(E); _cimg_iskey_def(R); _cimg_iskey_def(T); _cimg_iskey_def(Y);
    _cimg_iskey_def(U); _cimg_iskey_def(I); _cimg_iskey_def(O); _cimg_iskey_def(P);
    _cimg_iskey_def(DELETE); _cimg_iskey_def(END); _cimg_iskey_def(PAGEDOWN);
    _cimg_iskey_def(CAPSLOCK); _cimg_iskey_def(A); _cimg_iskey_def(S); _cimg_iskey_def(D);
    _cimg_iskey_def(F); _cimg_iskey_def(G); _cimg_iskey_def(H); _cimg_iskey_def(J);
    _cimg_iskey_def(K); _cimg_iskey_def(L); _cimg_iskey_def(RETURN);
    _cimg_iskey_def(SHIFTLEFT); _cimg_iskey_def(Z); _cimg_iskey_def(X); _cimg_iskey_def(C);
    _cimg_iskey_def(V); _cimg_iskey_def(B); _cimg_iskey_def(N); _cimg_iskey_def(M);
    _cimg_iskey_def(SHIFTRIGHT); _cimg_iskey_def(ARROWUP); _cimg_iskey_def(CTRLLEFT);
    _cimg_iskey_def(APPLEFT); _cimg_iskey_def(ALT); _cimg_iskey_def(SPACE); _cimg_iskey_def(ALTGR);
    _cimg_iskey_def(APPRIGHT); _cimg_iskey_def(MENU); _cimg_iskey_def(CTRLRIGHT);
    _cimg_iskey_def(ARROWLEFT); _cimg_iskey_def(ARROWDOWN); _cimg_iskey_def(ARROWRIGHT);
    _cimg_iskey_def(PAD0); _cimg_iskey_def(PAD1); _cimg_iskey_def(PAD2);
    _cimg_iskey_def(PAD3); _cimg_iskey_def(PAD4); _cimg_iskey_def(PAD5);
    _cimg_iskey_def(PAD6); _cimg_iskey_def(PAD7); _cimg_iskey_def(PAD8);
    _cimg_iskey_def(PAD9); _cimg_iskey_def(PADADD); _cimg_iskey_def(PADSUB);
    _cimg_iskey_def(PADMUL); _cimg_iskey_def(PADDIV); _cimg_iskey_def(ENTER);

    //@}
    //------------------------------------------
    //
    //! \name Instance Characteristics
    //@{
    //------------------------------------------

#if cimg_display==0

    //! Return width of the screen (current resolution along the X-axis).
    /**
    **/
    static int screen_width() {
      _no_display_exception();
      return 0;
    }

    //! Return height of the screen (current resolution along the Y-axis).
    /**
    **/
    static int screen_height() {
      _no_display_exception();
      return 0;
    }

#endif

    //! Return display width.
    /**
       \note The width of the display (i.e. the width of the pixel data buffer associated to the CImgDisplay instance)
       may be different from the actual width of the associated window.
    **/
    int width() const {
      return (int)_width;
    }

    //! Return display height.
    /**
       \note The height of the display (i.e. the height of the pixel data buffer associated to the CImgDisplay instance)
       may be different from the actual height of the associated window.
    **/
    int height() const {
      return (int)_height;
    }

    //! Return normalization type of the display.
    /**
       The normalization type tells about how the values of an input image are normalized by the CImgDisplay to be
       correctly displayed. The range of values for pixels displayed on screen is <tt>[0,255]</tt>.
       If the range of values of the data to display is different, a normalization may be required for displaying
       the data in a correct way. The normalization type can be one of:
       - \c 0: Value normalization is disabled. It is then assumed that all input data to be displayed by the
       CImgDisplay instance have values in range <tt>[0,255]</tt>.
       - \c 1: Value normalization is always performed (this is the default behavior).
       Before displaying an input image, its values will be (virtually) stretched
       in range <tt>[0,255]</tt>, so that the contrast of the displayed pixels will be maximum.
       Use this mode for images whose minimum and maximum values are not prescribed to known values
       (e.g. float-valued images).
       Note that when normalized versions of images are computed for display purposes, the actual values of these
       images are not modified.
       - \c 2: Value normalization is performed once (on the first image display), then the same normalization
       coefficients are kept for next displayed frames.
       - \c 3: Value normalization depends on the pixel type of the data to display. For integer pixel types,
       the normalization is done regarding the minimum/maximum values of the type (no normalization occurs then
       for <tt>unsigned char</tt>).
       For float-valued pixel types, the normalization is done regarding the minimum/maximum value of the image
       data instead.
    **/
    unsigned int normalization() const {
      return _normalization;
    }

    //! Return title of the associated window as a C-string.
    /**
       \note Window title may be not visible, depending on the used window manager or if the current display is
       in fullscreen mode.
    **/
    const char *title() const {
      return _title?_title:"";
    }

    //! Return width of the associated window.
    /**
       \note The width of the display (i.e. the width of the pixel data buffer associated to the CImgDisplay instance)
       may be different from the actual width of the associated window.
    **/
    int window_width() const {
      return (int)_window_width;
    }

    //! Return height of the associated window.
    /**
       \note The height of the display (i.e. the height of the pixel data buffer associated to the CImgDisplay instance)
       may be different from the actual height of the associated window.
    **/
    int window_height() const {
      return (int)_window_height;
    }

    //! Return X-coordinate of the associated window.
    /**
       \note The returned coordinate corresponds to the location of the upper-left corner of the associated window.
    **/
    int window_x() const {
      return _window_x;
    }

    //! Return Y-coordinate of the associated window.
    /**
       \note The returned coordinate corresponds to the location of the upper-left corner of the associated window.
    **/
    int window_y() const {
      return _window_y;
    }

    //! Return X-coordinate of the mouse pointer.
    /**
       \note
       - If the mouse pointer is outside window area, \c -1 is returned.
       - Otherwise, the returned value is in the range [0,width()-1].
    **/
    int mouse_x() const {
      return _mouse_x;
    }

    //! Return Y-coordinate of the mouse pointer.
    /**
       \note
       - If the mouse pointer is outside window area, \c -1 is returned.
       - Otherwise, the returned value is in the range [0,height()-1].
    **/
    int mouse_y() const {
      return _mouse_y;
    }

    //! Return current state of the mouse buttons.
    /**
       \note Three mouse buttons can be managed. If one button is pressed, its corresponding bit in the returned
       value is set:
       - bit \c 0 (value \c 0x1): State of the left mouse button.
       - bit \c 1 (value \c 0x2): State of the right mouse button.
       - bit \c 2 (value \c 0x4): State of the middle mouse button.

       Several bits can be activated if more than one button are pressed at the same time.
       \par Example
       \code
       CImgDisplay disp(400,400);
       while (!disp.is_closed()) {
         if (disp.button()&1) { // Left button clicked
           ...
         }
         if (disp.button()&2) { // Right button clicked
           ...
         }
         if (disp.button()&4) { // Middle button clicked
           ...
         }
         disp.wait();
       }
       \endcode
    **/
    unsigned int button() const {
      return _button;
    }

    //! Return current state of the mouse wheel.
    /**
       \note
       - The returned value can be positive or negative depending on whether the mouse wheel has been scrolled
       forward or backward.
       - Scrolling the wheel forward add \c 1 to the wheel value.
       - Scrolling the wheel backward subtract \c 1 to the wheel value.
       - The returned value cumulates the number of forward of backward scrolls since the creation of the display,
       or since the last reset of the wheel value (using set_wheel()). It is strongly recommended to quickly reset
       the wheel counter when an action has been performed regarding the current wheel value.
       Otherwise, the returned wheel value may be for instance \c 0 despite the fact that many scrolls have been done
       (as many in forward as in backward directions).
       \par Example
       \code
       CImgDisplay disp(400,400);
       while (!disp.is_closed()) {
         if (disp.wheel()) {
           int counter = disp.wheel(); // Read the state of the mouse wheel
           ... // Do what you want with 'counter'
           disp.set_wheel(); // Reset the wheel value to 0
         }
         disp.wait();
       }
       \endcode
    **/
    int wheel() const {
      return _wheel;
    }

    //! Return one entry from the pressed keys history.
    /**
       \param pos Index to read from the pressed keys history (index \c 0 corresponds to latest entry).
       \return Keycode of a pressed key or \c 0 for a released key.
       \note
       - Each CImgDisplay stores a history of the pressed keys in a buffer of size \c 128. When a new key is pressed,
       its keycode is stored in the pressed keys history. When a key is released, \c 0 is put instead.
       This means that up to the 64 last pressed keys may be read from the pressed keys history.
       When a new value is stored, the pressed keys history is shifted so that the latest entry is always
       stored at position \c 0.
       - Keycode constants are defined in the cimg namespace and are architecture-dependent. Use them to ensure
       your code stay portable (see cimg::keyESC).
    **/
    unsigned int& key(const unsigned int pos=0) const {
      static unsigned int key0;
      return pos<128?_keys[pos]:(key0 = 0);

    }

    //! Return one entry from the released keys history.
    /**
       \param pos Index to read from the released keys history (index \c 0 corresponds to latest entry).
       \return Keycode of a released key or \c 0 for a pressed key.
       \note
       - Each CImgDisplay stores a history of the released keys in a buffer of size \c 128. When a new key is released,
       its keycode is stored in the pressed keys history. When a key is pressed, \c 0 is put instead.
       This means that up to the 64 last released keys may be read from the released keys history.
       When a new value is stored, the released keys history is shifted so that the latest entry is always
       stored at position \c 0.
       - Keycode constants are defined in the cimg namespace and are architecture-dependent. Use them to ensure
       your code stay portable (see cimg::keyESC).
    **/
    unsigned int& released_key(const unsigned int pos=0) const {
      static unsigned int key0;
      return pos<128?_released_keys[pos]:(key0 = 0);
    }

    //! Return keycode corresponding to the specified string.
    /**
       \note Keycode constants are defined in the cimg namespace and are architecture-dependent. Use them to ensure
       your code stay portable (see cimg::keyESC).
       \par Example
       \code
       const unsigned int keyTAB = CImgDisplay::keycode("TAB"); // Return cimg::keyTAB
       \endcode
    **/
    static unsigned int keycode(const char *const keycode) {
#define _cimg_keycode(k) if (!cimg::strcasecmp(keycode,#k)) return cimg::key##k;
      _cimg_keycode(ESC); _cimg_keycode(F1); _cimg_keycode(F2); _cimg_keycode(F3);
      _cimg_keycode(F4); _cimg_keycode(F5); _cimg_keycode(F6); _cimg_keycode(F7);
      _cimg_keycode(F8); _cimg_keycode(F9); _cimg_keycode(F10); _cimg_keycode(F11);
      _cimg_keycode(F12); _cimg_keycode(PAUSE); _cimg_keycode(1); _cimg_keycode(2);
      _cimg_keycode(3); _cimg_keycode(4); _cimg_keycode(5); _cimg_keycode(6);
      _cimg_keycode(7); _cimg_keycode(8); _cimg_keycode(9); _cimg_keycode(0);
      _cimg_keycode(BACKSPACE); _cimg_keycode(INSERT); _cimg_keycode(HOME);
      _cimg_keycode(PAGEUP); _cimg_keycode(TAB); _cimg_keycode(Q); _cimg_keycode(W);
      _cimg_keycode(E); _cimg_keycode(R); _cimg_keycode(T); _cimg_keycode(Y);
      _cimg_keycode(U); _cimg_keycode(I); _cimg_keycode(O); _cimg_keycode(P);
      _cimg_keycode(DELETE); _cimg_keycode(END); _cimg_keycode(PAGEDOWN);
      _cimg_keycode(CAPSLOCK); _cimg_keycode(A); _cimg_keycode(S); _cimg_keycode(D);
      _cimg_keycode(F); _cimg_keycode(G); _cimg_keycode(H); _cimg_keycode(J);
      _cimg_keycode(K); _cimg_keycode(L); _cimg_keycode(RETURN);
      _cimg_keycode(SHIFTLEFT); _cimg_keycode(Z); _cimg_keycode(X); _cimg_keycode(C);
      _cimg_keycode(V); _cimg_keycode(B); _cimg_keycode(N); _cimg_keycode(M);
      _cimg_keycode(SHIFTRIGHT); _cimg_keycode(ARROWUP); _cimg_keycode(CTRLLEFT);
      _cimg_keycode(APPLEFT); _cimg_keycode(ALT); _cimg_keycode(SPACE); _cimg_keycode(ALTGR);
      _cimg_keycode(APPRIGHT); _cimg_keycode(MENU); _cimg_keycode(CTRLRIGHT);
      _cimg_keycode(ARROWLEFT); _cimg_keycode(ARROWDOWN); _cimg_keycode(ARROWRIGHT);
      _cimg_keycode(PAD0); _cimg_keycode(PAD1); _cimg_keycode(PAD2);
      _cimg_keycode(PAD3); _cimg_keycode(PAD4); _cimg_keycode(PAD5);
      _cimg_keycode(PAD6); _cimg_keycode(PAD7); _cimg_keycode(PAD8);
      _cimg_keycode(PAD9); _cimg_keycode(PADADD); _cimg_keycode(PADSUB);
      _cimg_keycode(PADMUL); _cimg_keycode(PADDIV); _cimg_keycode(ENTER);
      return 0;
    }

    //! Return the current refresh rate, in frames per second.
    /**
       \note Returns a significant value when the current instance is used to display successive frames.
       It measures the delay between successive calls to frames_per_second().
    **/
    float frames_per_second() {
      if (!_fps_timer) _fps_timer = cimg::time();
      const float delta = (float)((cimg::time() - _fps_timer)/1000.f);
      ++_fps_frames;
      if (delta>=1) {
        _fps_fps = _fps_frames/delta;
        _fps_frames = 0;
        _fps_timer = cimg::time();
      }
      return _fps_fps;
    }

    // Move current display window so that its content stays inside the current screen.
    CImgDisplay& move_inside_screen() {
      if (is_empty()) return *this;
      const int
        x0 = window_x(),
        y0 = window_y(),
        x1 = x0 + window_width() - 1,
        y1 = y0 + window_height() - 1,
        sw = CImgDisplay::screen_width(),
        sh = CImgDisplay::screen_height();
      if (x0<0 || y0<0 || x1>=sw || y1>=sh)
        move(std::max(0,std::min(x0,sw - x1 + x0)),
             std::max(0,std::min(y0,sh - y1 + y0)));
      return *this;
    }

    //@}
    //---------------------------------------
    //
    //! \name Window Manipulation
    //@{
    //---------------------------------------

#if cimg_display==0

    //! Display image on associated window.
    /**
       \param img Input image to display.
       \note This method returns immediately.
    **/
    template<typename T>
    CImgDisplay& display(const CImg<T>& img) {
      return assign(img);
    }

#endif

    //! Display list of images on associated window.
    /**
       \param list List of images to display.
       \param axis Axis used to append the images along, for the visualization (can be \c x, \c y, \c z or \c c).
       \param align Relative position of aligned images when displaying lists with images of different sizes
       (\c 0 for upper-left, \c 0.5 for centering and \c 1 for lower-right).
       \note This method returns immediately.
    **/
    template<typename T>
    CImgDisplay& display(const CImgList<T>& list, const char axis='x', const float align=0) {
      if (list._width==1) {
        const CImg<T>& img = list[0];
        if (img._depth==1 && (img._spectrum==1 || img._spectrum>=3) && _normalization!=1) return display(img);
      }
      CImgList<typename CImg<T>::ucharT> visu(list._width);
      unsigned int dims = 0;
      cimglist_for(list,l) {
        const CImg<T>& img = list._data[l];
        img._get_select(*this,_normalization,(img._width - 1)/2,(img._height - 1)/2,
                        (img._depth - 1)/2).move_to(visu[l]);
        dims = std::max(dims,visu[l]._spectrum);
      }
      cimglist_for(list,l) if (visu[l]._spectrum<dims) visu[l].resize(-100,-100,-100,dims,1);
      visu.get_append(axis,align).display(*this);
      return *this;
    }

#if cimg_display==0

    //! Show (closed) associated window on the screen.
    /**
       \note
       - Force the associated window of a display to be visible on the screen, even if it has been closed before.
       - Using show() on a visible display does nothing.
    **/
    CImgDisplay& show() {
      return assign();
    }

    //! Close (visible) associated window and make it disappear from the screen.
    /**
       \note
       - A closed display only means the associated window is not visible anymore. This does not mean the display has
       been destroyed.
       Use show() to make the associated window reappear.
       - Using close() on a closed display does nothing.
    **/
    CImgDisplay& close() {
      return assign();
    }

    //! Move associated window to a new location.
    /**
       \param pos_x X-coordinate of the new window location.
       \param pos_y Y-coordinate of the new window location.
       \note Depending on the window manager behavior, this method may not succeed (no exceptions are thrown
       nevertheless).
    **/
    CImgDisplay& move(const int pos_x, const int pos_y) {
      return assign(pos_x,pos_y);
    }

#endif

    //! Resize display to the size of the associated window.
    /**
       \param force_redraw Tells if the previous window content must be updated and refreshed as well.
       \note
       - Calling this method ensures that width() and window_width() become equal, as well as height() and
       window_height().
       - The associated window is also resized to specified dimensions.
    **/
    CImgDisplay& resize(const bool force_redraw=true) {
      resize(window_width(),window_height(),force_redraw);
      return *this;
    }

#if cimg_display==0

    //! Resize display to the specified size.
    /**
       \param width Requested display width.
       \param height Requested display height.
       \param force_redraw Tells if the previous window content must be updated and refreshed as well.
       \note The associated window is also resized to specified dimensions.
    **/
    CImgDisplay& resize(const int width, const int height, const bool force_redraw=true) {
      return assign(width,height,0,3,force_redraw);
    }

#endif

    //! Resize display to the size of an input image.
    /**
       \param img Input image to take size from.
       \param force_redraw Tells if the previous window content must be resized and updated as well.
       \note
       - Calling this method ensures that width() and <tt>img.width()</tt> become equal, as well as height() and
       <tt>img.height()</tt>.
       - The associated window is also resized to specified dimensions.
    **/
    template<typename T>
    CImgDisplay& resize(const CImg<T>& img, const bool force_redraw=true) {
      return resize(img._width,img._height,force_redraw);
    }

    //! Resize display to the size of another CImgDisplay instance.
    /**
       \param disp Input display to take size from.
       \param force_redraw Tells if the previous window content must be resized and updated as well.
       \note
       - Calling this method ensures that width() and <tt>disp.width()</tt> become equal, as well as height() and
       <tt>disp.height()</tt>.
       - The associated window is also resized to specified dimensions.
    **/
    CImgDisplay& resize(const CImgDisplay& disp, const bool force_redraw=true) {
      return resize(disp.width(),disp.height(),force_redraw);
    }

    // [internal] Render pixel buffer with size (wd,hd) from source buffer of size (ws,hs).
    template<typename t, typename T>
    static void _render_resize(const T *ptrs, const unsigned int ws, const unsigned int hs,
                               t *ptrd, const unsigned int wd, const unsigned int hd) {
      typedef typename cimg::last<T,cimg_ulong>::type ulongT;
      const ulongT one = (ulongT)1;
      CImg<ulongT> off_x(wd), off_y(hd + 1);
      if (wd==ws) off_x.fill(1);
      else {
        ulongT *poff_x = off_x._data, curr = 0;
        for (unsigned int x = 0; x<wd; ++x) {
          const ulongT old = curr;
          curr = (x + one)*ws/wd;
          *(poff_x++) = curr - old;
        }
      }
      if (hd==hs) off_y.fill(ws);
      else {
        ulongT *poff_y = off_y._data, curr = 0;
        for (unsigned int y = 0; y<hd; ++y) {
          const ulongT old = curr;
          curr = (y + one)*hs/hd;
          *(poff_y++) = ws*(curr - old);
        }
        *poff_y = 0;
      }
      ulongT *poff_y = off_y._data;
      for (unsigned int y = 0; y<hd; ) {
        const T *ptr = ptrs;
        ulongT *poff_x = off_x._data;
        for (unsigned int x = 0; x<wd; ++x) { *(ptrd++) = *ptr; ptr+=*(poff_x++); }
        ++y;
        ulongT dy = *(poff_y++);
        for ( ; !dy && y<hd; std::memcpy(ptrd,ptrd - wd,sizeof(t)*wd), ++y, ptrd+=wd, dy = *(poff_y++)) {}
        ptrs+=dy;
      }
    }

    //! Set normalization type.
    /**
       \param normalization New normalization mode.
    **/
    CImgDisplay& set_normalization(const unsigned int normalization) {
      _normalization = normalization;
      _min = _max = 0;
      return *this;
    }

#if cimg_display==0

    //! Set title of the associated window.
    /**
       \param format C-string containing the format of the title, as with <tt>std::printf()</tt>.
       \warning As the first argument is a format string, it is highly recommended to write
       \code
       disp.set_title("%s",window_title);
       \endcode
       instead of
       \code
       disp.set_title(window_title);
       \endcode
       if \c window_title can be arbitrary, to prevent nasty memory access.
    **/
    CImgDisplay& set_title(const char *const format, ...) {
      return assign(0,0,format);
    }

#endif

    //! Enable or disable fullscreen mode.
    /**
       \param is_fullscreen Tells is the fullscreen mode must be activated or not.
       \param force_redraw Tells if the previous window content must be displayed as well.
       \note
       - When the fullscreen mode is enabled, the associated window fills the entire screen but the size of the
       current display is not modified.
       - The screen resolution may be switched to fit the associated window size and ensure it appears the largest
       as possible.
       For X-Window (X11) users, the configuration flag \c cimg_use_xrandr has to be set to allow the screen
       resolution change (requires the X11 extensions to be enabled).
    **/
    CImgDisplay& set_fullscreen(const bool is_fullscreen, const bool force_redraw=true) {
      if (is_empty() || _is_fullscreen==is_fullscreen) return *this;
      return toggle_fullscreen(force_redraw);
    }

#if cimg_display==0

    //! Toggle fullscreen mode.
    /**
       \param force_redraw Tells if the previous window content must be displayed as well.
       \note Enable fullscreen mode if it was not enabled, and disable it otherwise.
    **/
    CImgDisplay& toggle_fullscreen(const bool force_redraw=true) {
      return assign(_width,_height,0,3,force_redraw);
    }

    //! Show mouse pointer.
    /**
       \note Depending on the window manager behavior, this method may not succeed
       (no exceptions are thrown nevertheless).
    **/
    CImgDisplay& show_mouse() {
      return assign();
    }

    //! Hide mouse pointer.
    /**
       \note Depending on the window manager behavior, this method may not succeed
       (no exceptions are thrown nevertheless).
    **/
    CImgDisplay& hide_mouse() {
      return assign();
    }

    //! Move mouse pointer to a specified location.
    /**
       \note Depending on the window manager behavior, this method may not succeed
       (no exceptions are thrown nevertheless).
    **/
    CImgDisplay& set_mouse(const int pos_x, const int pos_y) {
      return assign(pos_x,pos_y);
    }

#endif

    //! Simulate a mouse button release event.
    /**
       \note All mouse buttons are considered released at the same time.
    **/
    CImgDisplay& set_button() {
      _button = 0;
      _is_event = true;
#if cimg_display==1
      pthread_cond_broadcast(&cimg::X11_attr::ref().wait_event);
#elif cimg_display==2
      SetEvent(cimg::Win32_attr::ref().wait_event);
#endif
      return *this;
    }

    //! Simulate a mouse button press or release event.
    /**
       \param button Buttons event code, where each button is associated to a single bit.
       \param is_pressed Tells if the mouse button is considered as pressed or released.
    **/
    CImgDisplay& set_button(const unsigned int button, const bool is_pressed=true) {
      const unsigned int buttoncode = button==1U?1U:button==2U?2U:button==3U?4U:0U;
      if (is_pressed) _button |= buttoncode; else _button &= ~buttoncode;
      _is_event = buttoncode?true:false;
      if (buttoncode) {
#if cimg_display==1
        pthread_cond_broadcast(&cimg::X11_attr::ref().wait_event);
#elif cimg_display==2
        SetEvent(cimg::Win32_attr::ref().wait_event);
#endif
      }
      return *this;
    }

    //! Flush all mouse wheel events.
    /**
       \note Make wheel() to return \c 0, if called afterwards.
    **/
    CImgDisplay& set_wheel() {
      _wheel = 0;
      _is_event = true;
#if cimg_display==1
      pthread_cond_broadcast(&cimg::X11_attr::ref().wait_event);
#elif cimg_display==2
      SetEvent(cimg::Win32_attr::ref().wait_event);
#endif
      return *this;
    }

    //! Simulate a wheel event.
    /**
       \param amplitude Amplitude of the wheel scrolling to simulate.
       \note Make wheel() to return \c amplitude, if called afterwards.
    **/
    CImgDisplay& set_wheel(const int amplitude) {
      _wheel+=amplitude;
      _is_event = amplitude?true:false;
      if (amplitude) {
#if cimg_display==1
        pthread_cond_broadcast(&cimg::X11_attr::ref().wait_event);
#elif cimg_display==2
        SetEvent(cimg::Win32_attr::ref().wait_event);
#endif
      }
      return *this;
    }

    //! Flush all key events.
    /**
       \note Make key() to return \c 0, if called afterwards.
    **/
    CImgDisplay& set_key() {
      if (_keys) std::memset((void*)_keys,0,128*sizeof(unsigned int));
      if (_released_keys) std::memset((void*)_released_keys,0,128*sizeof(unsigned int));
      _is_keyESC = _is_keyF1 = _is_keyF2 = _is_keyF3 = _is_keyF4 = _is_keyF5 = _is_keyF6 = _is_keyF7 = _is_keyF8 =
        _is_keyF9 = _is_keyF10 = _is_keyF11 = _is_keyF12 = _is_keyPAUSE = _is_key1 = _is_key2 = _is_key3 = _is_key4 =
        _is_key5 = _is_key6 = _is_key7 = _is_key8 = _is_key9 = _is_key0 = _is_keyBACKSPACE = _is_keyINSERT =
        _is_keyHOME = _is_keyPAGEUP = _is_keyTAB = _is_keyQ = _is_keyW = _is_keyE = _is_keyR = _is_keyT = _is_keyY =
        _is_keyU = _is_keyI = _is_keyO = _is_keyP = _is_keyDELETE = _is_keyEND = _is_keyPAGEDOWN = _is_keyCAPSLOCK =
        _is_keyA = _is_keyS = _is_keyD = _is_keyF = _is_keyG = _is_keyH = _is_keyJ = _is_keyK = _is_keyL =
        _is_keyRETURN = _is_keySHIFTLEFT = _is_keyZ = _is_keyX = _is_keyC = _is_keyV = _is_keyB = _is_keyN =
        _is_keyM = _is_keySHIFTRIGHT = _is_keyARROWUP = _is_keyCTRLLEFT = _is_keyAPPLEFT = _is_keyALT = _is_keySPACE =
        _is_keyALTGR = _is_keyAPPRIGHT = _is_keyMENU = _is_keyCTRLRIGHT = _is_keyARROWLEFT = _is_keyARROWDOWN =
        _is_keyARROWRIGHT = _is_keyPAD0 = _is_keyPAD1 = _is_keyPAD2 = _is_keyPAD3 = _is_keyPAD4 = _is_keyPAD5 =
        _is_keyPAD6 = _is_keyPAD7 = _is_keyPAD8 = _is_keyPAD9 = _is_keyPADADD = _is_keyPADSUB = _is_keyPADMUL =
        _is_keyPADDIV = _is_keyENTER = false;
      _is_event = true;
#if cimg_display==1
      pthread_cond_broadcast(&cimg::X11_attr::ref().wait_event);
#elif cimg_display==2
      SetEvent(cimg::Win32_attr::ref().wait_event);
#endif
      return *this;
    }

    //! Simulate a keyboard press/release event.
    /**
       \param keycode Keycode of the associated key.
       \param is_pressed Tells if the key is considered as pressed or released.
       \note Keycode constants are defined in the cimg namespace and are architecture-dependent. Use them to ensure
       your code stay portable (see cimg::keyESC).
    **/
    CImgDisplay& set_key(const unsigned int keycode, const bool is_pressed=true) {
#define _cimg_set_key(k) if (keycode==cimg::key##k) _is_key##k = is_pressed;
      _cimg_set_key(ESC); _cimg_set_key(F1); _cimg_set_key(F2); _cimg_set_key(F3);
      _cimg_set_key(F4); _cimg_set_key(F5); _cimg_set_key(F6); _cimg_set_key(F7);
      _cimg_set_key(F8); _cimg_set_key(F9); _cimg_set_key(F10); _cimg_set_key(F11);
      _cimg_set_key(F12); _cimg_set_key(PAUSE); _cimg_set_key(1); _cimg_set_key(2);
      _cimg_set_key(3); _cimg_set_key(4); _cimg_set_key(5); _cimg_set_key(6);
      _cimg_set_key(7); _cimg_set_key(8); _cimg_set_key(9); _cimg_set_key(0);
      _cimg_set_key(BACKSPACE); _cimg_set_key(INSERT); _cimg_set_key(HOME);
      _cimg_set_key(PAGEUP); _cimg_set_key(TAB); _cimg_set_key(Q); _cimg_set_key(W);
      _cimg_set_key(E); _cimg_set_key(R); _cimg_set_key(T); _cimg_set_key(Y);
      _cimg_set_key(U); _cimg_set_key(I); _cimg_set_key(O); _cimg_set_key(P);
      _cimg_set_key(DELETE); _cimg_set_key(END); _cimg_set_key(PAGEDOWN);
      _cimg_set_key(CAPSLOCK); _cimg_set_key(A); _cimg_set_key(S); _cimg_set_key(D);
      _cimg_set_key(F); _cimg_set_key(G); _cimg_set_key(H); _cimg_set_key(J);
      _cimg_set_key(K); _cimg_set_key(L); _cimg_set_key(RETURN);
      _cimg_set_key(SHIFTLEFT); _cimg_set_key(Z); _cimg_set_key(X); _cimg_set_key(C);
      _cimg_set_key(V); _cimg_set_key(B); _cimg_set_key(N); _cimg_set_key(M);
      _cimg_set_key(SHIFTRIGHT); _cimg_set_key(ARROWUP); _cimg_set_key(CTRLLEFT);
      _cimg_set_key(APPLEFT); _cimg_set_key(ALT); _cimg_set_key(SPACE); _cimg_set_key(ALTGR);
      _cimg_set_key(APPRIGHT); _cimg_set_key(MENU); _cimg_set_key(CTRLRIGHT);
      _cimg_set_key(ARROWLEFT); _cimg_set_key(ARROWDOWN); _cimg_set_key(ARROWRIGHT);
      _cimg_set_key(PAD0); _cimg_set_key(PAD1); _cimg_set_key(PAD2);
      _cimg_set_key(PAD3); _cimg_set_key(PAD4); _cimg_set_key(PAD5);
      _cimg_set_key(PAD6); _cimg_set_key(PAD7); _cimg_set_key(PAD8);
      _cimg_set_key(PAD9); _cimg_set_key(PADADD); _cimg_set_key(PADSUB);
      _cimg_set_key(PADMUL); _cimg_set_key(PADDIV); _cimg_set_key(ENTER);
      if (is_pressed) {
        if (*_keys)
          std::memmove((void*)(_keys + 1),(void*)_keys,127*sizeof(unsigned int));
        *_keys = keycode;
        if (*_released_keys) {
          std::memmove((void*)(_released_keys + 1),(void*)_released_keys,127*sizeof(unsigned int));
          *_released_keys = 0;
        }
      } else {
        if (*_keys) {
          std::memmove((void*)(_keys + 1),(void*)_keys,127*sizeof(unsigned int));
          *_keys = 0;
        }
        if (*_released_keys)
          std::memmove((void*)(_released_keys + 1),(void*)_released_keys,127*sizeof(unsigned int));
        *_released_keys = keycode;
      }
      _is_event = keycode?true:false;
      if (keycode) {
#if cimg_display==1
        pthread_cond_broadcast(&cimg::X11_attr::ref().wait_event);
#elif cimg_display==2
        SetEvent(cimg::Win32_attr::ref().wait_event);
#endif
      }
      return *this;
    }

    //! Flush all display events.
    /**
       \note Remove all passed events from the current display.
    **/
    CImgDisplay& flush() {
      set_key().set_button().set_wheel();
      _is_resized = _is_moved = _is_event = false;
      _fps_timer = _fps_frames = _timer = 0;
      _fps_fps = 0;
      return *this;
    }

    //! Wait for any user event occurring on the current display.
    CImgDisplay& wait() {
      wait(*this);
      return *this;
    }

    //! Wait for any event occurring on the display \c disp1.
    static void wait(CImgDisplay& disp1) {
      disp1._is_event = false;
      while (!disp1._is_closed && !disp1._is_event) wait_all();
    }

    //! Wait for any event occurring either on the display \c disp1 or \c disp2.
    static void wait(CImgDisplay& disp1, CImgDisplay& disp2) {
      disp1._is_event = disp2._is_event = false;
      while ((!disp1._is_closed || !disp2._is_closed) &&
             !disp1._is_event && !disp2._is_event) wait_all();
    }

    //! Wait for any event occurring either on the display \c disp1, \c disp2 or \c disp3.
    static void wait(CImgDisplay& disp1, CImgDisplay& disp2, CImgDisplay& disp3) {
      disp1._is_event = disp2._is_event = disp3._is_event = false;
      while ((!disp1._is_closed || !disp2._is_closed || !disp3._is_closed) &&
             !disp1._is_event && !disp2._is_event && !disp3._is_event) wait_all();
    }

    //! Wait for any event occurring either on the display \c disp1, \c disp2, \c disp3 or \c disp4.
    static void wait(CImgDisplay& disp1, CImgDisplay& disp2, CImgDisplay& disp3, CImgDisplay& disp4) {
      disp1._is_event = disp2._is_event = disp3._is_event = disp4._is_event = false;
      while ((!disp1._is_closed || !disp2._is_closed || !disp3._is_closed || !disp4._is_closed) &&
             !disp1._is_event && !disp2._is_event && !disp3._is_event && !disp4._is_event) wait_all();
    }

    //! Wait for any event occurring either on the display \c disp1, \c disp2, \c disp3, \c disp4 or \c disp5.
    static void wait(CImgDisplay& disp1, CImgDisplay& disp2, CImgDisplay& disp3, CImgDisplay& disp4,
                     CImgDisplay& disp5) {
      disp1._is_event = disp2._is_event = disp3._is_event = disp4._is_event = disp5._is_event = false;
      while ((!disp1._is_closed || !disp2._is_closed || !disp3._is_closed || !disp4._is_closed || !disp5._is_closed) &&
             !disp1._is_event && !disp2._is_event && !disp3._is_event && !disp4._is_event && !disp5._is_event)
        wait_all();
    }

    //! Wait for any event occurring either on the display \c disp1, \c disp2, \c disp3, \c disp4, ... \c disp6.
    static void wait(CImgDisplay& disp1, CImgDisplay& disp2, CImgDisplay& disp3, CImgDisplay& disp4, CImgDisplay& disp5,
                     CImgDisplay& disp6) {
      disp1._is_event = disp2._is_event = disp3._is_event = disp4._is_event = disp5._is_event =
        disp6._is_event = false;
      while ((!disp1._is_closed || !disp2._is_closed || !disp3._is_closed || !disp4._is_closed || !disp5._is_closed ||
              !disp6._is_closed) &&
             !disp1._is_event && !disp2._is_event && !disp3._is_event && !disp4._is_event && !disp5._is_event &&
             !disp6._is_event) wait_all();
    }

    //! Wait for any event occurring either on the display \c disp1, \c disp2, \c disp3, \c disp4, ... \c disp7.
    static void wait(CImgDisplay& disp1, CImgDisplay& disp2, CImgDisplay& disp3, CImgDisplay& disp4, CImgDisplay& disp5,
                     CImgDisplay& disp6, CImgDisplay& disp7) {
      disp1._is_event = disp2._is_event = disp3._is_event = disp4._is_event = disp5._is_event =
        disp6._is_event = disp7._is_event = false;
      while ((!disp1._is_closed || !disp2._is_closed || !disp3._is_closed || !disp4._is_closed || !disp5._is_closed ||
              !disp6._is_closed || !disp7._is_closed) &&
             !disp1._is_event && !disp2._is_event && !disp3._is_event && !disp4._is_event && !disp5._is_event &&
             !disp6._is_event && !disp7._is_event) wait_all();
    }

    //! Wait for any event occurring either on the display \c disp1, \c disp2, \c disp3, \c disp4, ... \c disp8.
    static void wait(CImgDisplay& disp1, CImgDisplay& disp2, CImgDisplay& disp3, CImgDisplay& disp4, CImgDisplay& disp5,
                     CImgDisplay& disp6, CImgDisplay& disp7, CImgDisplay& disp8) {
      disp1._is_event = disp2._is_event = disp3._is_event = disp4._is_event = disp5._is_event =
        disp6._is_event = disp7._is_event = disp8._is_event = false;
      while ((!disp1._is_closed || !disp2._is_closed || !disp3._is_closed || !disp4._is_closed || !disp5._is_closed ||
              !disp6._is_closed || !disp7._is_closed || !disp8._is_closed) &&
             !disp1._is_event && !disp2._is_event && !disp3._is_event && !disp4._is_event && !disp5._is_event &&
             !disp6._is_event && !disp7._is_event && !disp8._is_event) wait_all();
    }

    //! Wait for any event occurring either on the display \c disp1, \c disp2, \c disp3, \c disp4, ... \c disp9.
    static void wait(CImgDisplay& disp1, CImgDisplay& disp2, CImgDisplay& disp3, CImgDisplay& disp4, CImgDisplay& disp5,
                     CImgDisplay& disp6, CImgDisplay& disp7, CImgDisplay& disp8, CImgDisplay& disp9) {
      disp1._is_event = disp2._is_event = disp3._is_event = disp4._is_event = disp5._is_event =
        disp6._is_event = disp7._is_event = disp8._is_event = disp9._is_event = false;
      while ((!disp1._is_closed || !disp2._is_closed || !disp3._is_closed || !disp4._is_closed || !disp5._is_closed ||
              !disp6._is_closed || !disp7._is_closed || !disp8._is_closed || !disp9._is_closed) &&
             !disp1._is_event && !disp2._is_event && !disp3._is_event && !disp4._is_event && !disp5._is_event &&
             !disp6._is_event && !disp7._is_event && !disp8._is_event && !disp9._is_event) wait_all();
    }

    //! Wait for any event occurring either on the display \c disp1, \c disp2, \c disp3, \c disp4, ... \c disp10.
    static void wait(CImgDisplay& disp1, CImgDisplay& disp2, CImgDisplay& disp3, CImgDisplay& disp4, CImgDisplay& disp5,
                     CImgDisplay& disp6, CImgDisplay& disp7, CImgDisplay& disp8, CImgDisplay& disp9,
                     CImgDisplay& disp10) {
      disp1._is_event = disp2._is_event = disp3._is_event = disp4._is_event = disp5._is_event =
        disp6._is_event = disp7._is_event = disp8._is_event = disp9._is_event = disp10._is_event = false;
      while ((!disp1._is_closed || !disp2._is_closed || !disp3._is_closed || !disp4._is_closed || !disp5._is_closed ||
              !disp6._is_closed || !disp7._is_closed || !disp8._is_closed || !disp9._is_closed || !disp10._is_closed) &&
             !disp1._is_event && !disp2._is_event && !disp3._is_event && !disp4._is_event && !disp5._is_event &&
             !disp6._is_event && !disp7._is_event && !disp8._is_event && !disp9._is_event && !disp10._is_event)
        wait_all();
    }

#if cimg_display==0

    //! Wait for any window event occurring in any opened CImgDisplay.
    static void wait_all() {
      return _no_display_exception();
    }

    //! Wait for a given number of milliseconds since the last call to wait().
    /**
       \param milliseconds Number of milliseconds to wait for.
       \note Similar to cimg::wait().
    **/
    CImgDisplay& wait(const unsigned int milliseconds) {
      cimg::wait(milliseconds,&_timer);
      return *this;
    }

    //! Render image into internal display buffer.
    /**
       \param img Input image data to render.
       \note
       - Convert image data representation into the internal display buffer (architecture-dependent structure).
       - The content of the associated window is not modified, until paint() is called.
       - Should not be used for common CImgDisplay uses, since display() is more useful.
    **/
    template<typename T>
    CImgDisplay& render(const CImg<T>& img) {
      return assign(img);
    }

    //! Paint internal display buffer on associated window.
    /**
       \note
       - Update the content of the associated window with the internal display buffer, e.g. after a render() call.
       - Should not be used for common CImgDisplay uses, since display() is more useful.
    **/
    CImgDisplay& paint() {
      return assign();
    }

    //! Take a snapshot of the current screen content.
    /**
       \param x0 X-coordinate of the upper left corner.
       \param y0 Y-coordinate of the upper left corner.
       \param x1 X-coordinate of the lower right corner.
       \param y1 Y-coordinate of the lower right corner.
       \param[out] img Output screenshot. Can be empty on input
    **/
    template<typename T>
    static void screenshot(const int x0, const int y0, const int x1, const int y1, CImg<T>& img) {
      cimg::unused(x0,y0,x1,y1,&img);
      _no_display_exception();
    }

    //! Take a snapshot of the associated window content.
    /**
       \param[out] img Output snapshot. Can be empty on input.
    **/
    template<typename T>
    const CImgDisplay& snapshot(CImg<T>& img) const {
      cimg::unused(img);
      _no_display_exception();
      return *this;
    }
#endif
#endif
