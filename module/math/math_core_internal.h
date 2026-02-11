#ifndef CIMG_MODULE_MATH_CORE_INTERNAL_H
#define CIMG_MODULE_MATH_CORE_INTERNAL_H

    // Define variables used internally by CImg.
#if cimg_display==1
    struct X11_attr {
      CImgDisplay **cimg_displays;
      unsigned int nb_cimg_displays;
      Display *display;
      pthread_t *events_thread;
      pthread_cond_t wait_event;
      pthread_mutex_t mutex_lock_display, mutex_wait_event;
      pthread_mutexattr_t attr;
      unsigned int nb_bits;
      bool is_blue_first, is_shm_enabled, byte_order, events_thread_running;

#ifdef cimg_use_xrandr
      XRRScreenSize *resolutions;
      Rotation curr_rotation;
      unsigned int curr_resolution, nb_resolutions;
#endif
      X11_attr():nb_cimg_displays(0),display(0),events_thread(0),nb_bits(0),
                 is_blue_first(false),is_shm_enabled(false),byte_order(false),events_thread_running(false) {
#ifdef __FreeBSD__
        XInitThreads();
#endif
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&mutex_wait_event, &attr);
        pthread_mutex_init(&mutex_lock_display, &attr);
        pthread_cond_init(&wait_event,0);
#ifdef cimg_use_xrandr
        resolutions = 0;
        curr_rotation = 0;
        curr_resolution = nb_resolutions = 0;
#endif
        cimg_displays = new CImgDisplay*[512];
      }

      ~X11_attr() {
        unlock();
        if (events_thread) {
          terminate_events_thread();
          delete events_thread;
        }
        pthread_cond_destroy(&wait_event);
        pthread_mutex_unlock(&mutex_wait_event);
        pthread_mutex_destroy(&mutex_wait_event);
        pthread_mutex_unlock(&mutex_lock_display);
        pthread_mutex_destroy(&mutex_lock_display);
        delete[] cimg_displays;
        if (display) { XCloseDisplay(display); }
      }

      X11_attr& lock() { // Lock display
        pthread_mutex_lock(&mutex_lock_display);
        return *this;
      }

      X11_attr& unlock() { // Unlock display
        pthread_mutex_unlock(&mutex_lock_display);
        return *this;
      }

      X11_attr& terminate_events_thread() {
        events_thread_running = false;
        pthread_join(*events_thread,0);
        delete events_thread;
        events_thread = 0;
        return *this;
      }

      static X11_attr& ref() { // Return shared instance across compilation modules
        static X11_attr ref;
        return ref;
      }

    }; // struct X11_attr { ...

#elif cimg_display==2
    struct Win32_attr {
      HANDLE wait_event;

      Win32_attr() {
        wait_event = CreateEvent(0,FALSE_WIN,FALSE_WIN,0);
      }

      static Win32_attr& ref() { // Return shared instance across compilation modules
        static Win32_attr ref;
        return ref;
      }
    }; // struct Win32_attr { ...

#elif cimg_display==3
    struct SDL3_attr {
      CImgDisplay **cimg_displays;
      unsigned int nb_cimg_displays;
      SDL_DisplayID display;
      SDL_ThreadID main_thread_id;
      const SDL_DisplayMode *mode;
      SDL_Mutex *mutex_lock_display; //, *mutex_wait_event;

      SDL3_attr():nb_cimg_displays(0),display(0),mode(0),mutex_lock_display(0) {
        bool init_failed = true;
        if (SDL_Init(SDL_INIT_VIDEO)) {
          display = SDL_GetPrimaryDisplay();
          if (display) {
            mode = SDL_GetCurrentDisplayMode(display);
            if (mode) {
              mutex_lock_display = SDL_CreateMutex();
              if (mutex_lock_display) init_failed = false;
              main_thread_id = SDL_GetCurrentThreadID();
            }
          }
        }
#if cimg_OS==1
        std::signal(SIGINT,SIG_DFL); // Restore default behavior for CTRL+C
#endif
        if (init_failed)
          throw CImgDisplayException("cimg::SDL3_attr(): %s",SDL_GetError());
        cimg_displays = new CImgDisplay*[512];
      }

      ~SDL3_attr() {
        unlock();
        SDL_DestroyMutex(mutex_lock_display);
        SDL_Quit();
        delete[] cimg_displays;
      }

      SDL3_attr& lock() { // Lock display
        SDL_LockMutex(mutex_lock_display);
        return *this;
      }

      SDL3_attr& unlock() { // Unlock display
        SDL_UnlockMutex(mutex_lock_display);
        return *this;
      }

      static SDL3_attr& ref() { // Return shared instance across compilation modules
        static SDL3_attr ref;
        return ref;
      }

    }; // struct SDL3_attr { ...

#endif

    struct Mutex_attr {
#if cimg_OS==1 && (defined(cimg_use_pthread) || cimg_display==1)
      pthread_mutex_t mutex[32];
      Mutex_attr() { for (unsigned int i = 0; i<32; ++i) pthread_mutex_init(&mutex[i],0); }
      void lock(const unsigned int n) { pthread_mutex_lock(&mutex[n]); }
      void unlock(const unsigned int n) { pthread_mutex_unlock(&mutex[n]); }
      int trylock(const unsigned int n) { return pthread_mutex_trylock(&mutex[n]); }
#elif cimg_OS==2
      HANDLE mutex[32];
      Mutex_attr() { for (unsigned int i = 0; i<32; ++i) mutex[i] = CreateMutex(0,FALSE_WIN,0); }
      void lock(const unsigned int n) { WaitForSingleObject(mutex[n],INFINITE); }
      void unlock(const unsigned int n) { ReleaseMutex(mutex[n]); }
      int trylock(const unsigned int) { return 0; }
#else
      Mutex_attr() {}
      void lock(const unsigned int) {}
      void unlock(const unsigned int) {}
      int trylock(const unsigned int) { return 0; }
#endif
      static Mutex_attr &ref() { // Return shared instance across compilation modules
        static Mutex_attr ref;
        return ref;
      }
    }; // struct Mutex_attr { ...

#if defined(cimg_use_magick)
    struct Magick_static {
      Magick_static() {
        Magick::InitializeMagick("");
      }
    }; // struct Magick_static { ...
    static Magick_static _Magick_static;
#endif

#if defined(cimg_use_fftw3) && !defined(cimg_use_fftw3_singlethread)
    struct FFTW3_static {
      FFTW3_static() {
        fftw_init_threads();
      }
    }; // struct FFTW3_static { ...
    static FFTW3_static _FFTW3_static;
#endif

#if cimg_display==1
    // Define keycodes for X11-based display (Unix).
    const unsigned int keyESC        = XK_Escape;
    const unsigned int keyF1         = XK_F1;
    const unsigned int keyF2         = XK_F2;
    const unsigned int keyF3         = XK_F3;
    const unsigned int keyF4         = XK_F4;
    const unsigned int keyF5         = XK_F5;
    const unsigned int keyF6         = XK_F6;
    const unsigned int keyF7         = XK_F7;
    const unsigned int keyF8         = XK_F8;
    const unsigned int keyF9         = XK_F9;
    const unsigned int keyF10        = XK_F10;
    const unsigned int keyF11        = XK_F11;
    const unsigned int keyF12        = XK_F12;
    const unsigned int keyPAUSE      = XK_Pause;
    const unsigned int key1          = XK_1;
    const unsigned int key2          = XK_2;
    const unsigned int key3          = XK_3;
    const unsigned int key4          = XK_4;
    const unsigned int key5          = XK_5;
    const unsigned int key6          = XK_6;
    const unsigned int key7          = XK_7;
    const unsigned int key8          = XK_8;
    const unsigned int key9          = XK_9;
    const unsigned int key0          = XK_0;
    const unsigned int keyBACKSPACE  = XK_BackSpace;
    const unsigned int keyINSERT     = XK_Insert;
    const unsigned int keyHOME       = XK_Home;
    const unsigned int keyPAGEUP     = XK_Page_Up;
    const unsigned int keyTAB        = XK_Tab;
    const unsigned int keyQ          = XK_q;
    const unsigned int keyW          = XK_w;
    const unsigned int keyE          = XK_e;
    const unsigned int keyR          = XK_r;
    const unsigned int keyT          = XK_t;
    const unsigned int keyY          = XK_y;
    const unsigned int keyU          = XK_u;
    const unsigned int keyI          = XK_i;
    const unsigned int keyO          = XK_o;
    const unsigned int keyP          = XK_p;
    const unsigned int keyDELETE     = XK_Delete;
    const unsigned int keyEND        = XK_End;
    const unsigned int keyPAGEDOWN   = XK_Page_Down;
    const unsigned int keyCAPSLOCK   = XK_Caps_Lock;
    const unsigned int keyA          = XK_a;
    const unsigned int keyS          = XK_s;
    const unsigned int keyD          = XK_d;
    const unsigned int keyF          = XK_f;
    const unsigned int keyG          = XK_g;
    const unsigned int keyH          = XK_h;
    const unsigned int keyJ          = XK_j;
    const unsigned int keyK          = XK_k;
    const unsigned int keyL          = XK_l;
    const unsigned int keyRETURN     = XK_Return;
    const unsigned int keySHIFTLEFT  = XK_Shift_L;
    const unsigned int keyZ          = XK_z;
    const unsigned int keyX          = XK_x;
    const unsigned int keyC          = XK_c;
    const unsigned int keyV          = XK_v;
    const unsigned int keyB          = XK_b;
    const unsigned int keyN          = XK_n;
    const unsigned int keyM          = XK_m;
    const unsigned int keySHIFTRIGHT = XK_Shift_R;
    const unsigned int keyARROWUP    = XK_Up;
    const unsigned int keyCTRLLEFT   = XK_Control_L;
    const unsigned int keyAPPLEFT    = XK_Super_L;
    const unsigned int keyALT        = XK_Alt_L;
    const unsigned int keySPACE      = XK_space;
    const unsigned int keyALTGR      = XK_Alt_R;
    const unsigned int keyAPPRIGHT   = XK_Super_R;
    const unsigned int keyMENU       = XK_Menu;
    const unsigned int keyCTRLRIGHT  = XK_Control_R;
    const unsigned int keyARROWLEFT  = XK_Left;
    const unsigned int keyARROWDOWN  = XK_Down;
    const unsigned int keyARROWRIGHT = XK_Right;
    const unsigned int keyPAD0       = XK_KP_0;
    const unsigned int keyPAD1       = XK_KP_1;
    const unsigned int keyPAD2       = XK_KP_2;
    const unsigned int keyPAD3       = XK_KP_3;
    const unsigned int keyPAD4       = XK_KP_4;
    const unsigned int keyPAD5       = XK_KP_5;
    const unsigned int keyPAD6       = XK_KP_6;
    const unsigned int keyPAD7       = XK_KP_7;
    const unsigned int keyPAD8       = XK_KP_8;
    const unsigned int keyPAD9       = XK_KP_9;
    const unsigned int keyPADADD     = XK_KP_Add;
    const unsigned int keyPADSUB     = XK_KP_Subtract;
    const unsigned int keyPADMUL     = XK_KP_Multiply;
    const unsigned int keyPADDIV     = XK_KP_Divide;
    const unsigned int keyENTER      = XK_KP_Enter;

#elif cimg_display==2
    // Define keycodes for GDI32-based display (Windows).
    const unsigned int keyESC        = VK_ESCAPE;
    const unsigned int keyF1         = VK_F1;
    const unsigned int keyF2         = VK_F2;
    const unsigned int keyF3         = VK_F3;
    const unsigned int keyF4         = VK_F4;
    const unsigned int keyF5         = VK_F5;
    const unsigned int keyF6         = VK_F6;
    const unsigned int keyF7         = VK_F7;
    const unsigned int keyF8         = VK_F8;
    const unsigned int keyF9         = VK_F9;
    const unsigned int keyF10        = VK_F10;
    const unsigned int keyF11        = VK_F11;
    const unsigned int keyF12        = VK_F12;
    const unsigned int keyPAUSE      = VK_PAUSE;
    const unsigned int key1          = '1';
    const unsigned int key2          = '2';
    const unsigned int key3          = '3';
    const unsigned int key4          = '4';
    const unsigned int key5          = '5';
    const unsigned int key6          = '6';
    const unsigned int key7          = '7';
    const unsigned int key8          = '8';
    const unsigned int key9          = '9';
    const unsigned int key0          = '0';
    const unsigned int keyBACKSPACE  = VK_BACK;
    const unsigned int keyINSERT     = VK_INSERT;
    const unsigned int keyHOME       = VK_HOME;
    const unsigned int keyPAGEUP     = VK_PRIOR;
    const unsigned int keyTAB        = VK_TAB;
    const unsigned int keyQ          = 'Q';
    const unsigned int keyW          = 'W';
    const unsigned int keyE          = 'E';
    const unsigned int keyR          = 'R';
    const unsigned int keyT          = 'T';
    const unsigned int keyY          = 'Y';
    const unsigned int keyU          = 'U';
    const unsigned int keyI          = 'I';
    const unsigned int keyO          = 'O';
    const unsigned int keyP          = 'P';
    const unsigned int keyDELETE     = VK_DELETE;
    const unsigned int keyEND        = VK_END;
    const unsigned int keyPAGEDOWN   = VK_NEXT;
    const unsigned int keyCAPSLOCK   = VK_CAPITAL;
    const unsigned int keyA          = 'A';
    const unsigned int keyS          = 'S';
    const unsigned int keyD          = 'D';
    const unsigned int keyF          = 'F';
    const unsigned int keyG          = 'G';
    const unsigned int keyH          = 'H';
    const unsigned int keyJ          = 'J';
    const unsigned int keyK          = 'K';
    const unsigned int keyL          = 'L';
    const unsigned int keyRETURN     = VK_RETURN;
    const unsigned int keySHIFTLEFT  = VK_SHIFT;
    const unsigned int keyZ          = 'Z';
    const unsigned int keyX          = 'X';
    const unsigned int keyC          = 'C';
    const unsigned int keyV          = 'V';
    const unsigned int keyB          = 'B';
    const unsigned int keyN          = 'N';
    const unsigned int keyM          = 'M';
    const unsigned int keySHIFTRIGHT = VK_SHIFT;
    const unsigned int keyARROWUP    = VK_UP;
    const unsigned int keyCTRLLEFT   = VK_CONTROL;
    const unsigned int keyAPPLEFT    = VK_LWIN;
    const unsigned int keyALT        = VK_LMENU;
    const unsigned int keySPACE      = VK_SPACE;
    const unsigned int keyALTGR      = VK_CONTROL;
    const unsigned int keyAPPRIGHT   = VK_RWIN;
    const unsigned int keyMENU       = VK_APPS;
    const unsigned int keyCTRLRIGHT  = VK_CONTROL;
    const unsigned int keyARROWLEFT  = VK_LEFT;
    const unsigned int keyARROWDOWN  = VK_DOWN;
    const unsigned int keyARROWRIGHT = VK_RIGHT;
    const unsigned int keyPAD0       = 0x60;
    const unsigned int keyPAD1       = 0x61;
    const unsigned int keyPAD2       = 0x62;
    const unsigned int keyPAD3       = 0x63;
    const unsigned int keyPAD4       = 0x64;
    const unsigned int keyPAD5       = 0x65;
    const unsigned int keyPAD6       = 0x66;
    const unsigned int keyPAD7       = 0x67;
    const unsigned int keyPAD8       = 0x68;
    const unsigned int keyPAD9       = 0x69;
    const unsigned int keyPADADD     = VK_ADD;
    const unsigned int keyPADSUB     = VK_SUBTRACT;
    const unsigned int keyPADMUL     = VK_MULTIPLY;
    const unsigned int keyPADDIV     = VK_DIVIDE;
    const unsigned int keyENTER      = VK_RETURN;

#elif cimg_display==3
    // Define keycodes for SDL3-based display.
    const unsigned int keyESC        = SDL_SCANCODE_ESCAPE;
    const unsigned int keyF1         = SDL_SCANCODE_F1;
    const unsigned int keyF2         = SDL_SCANCODE_F2;
    const unsigned int keyF3         = SDL_SCANCODE_F3;
    const unsigned int keyF4         = SDL_SCANCODE_F4;
    const unsigned int keyF5         = SDL_SCANCODE_F5;
    const unsigned int keyF6         = SDL_SCANCODE_F6;
    const unsigned int keyF7         = SDL_SCANCODE_F7;
    const unsigned int keyF8         = SDL_SCANCODE_F8;
    const unsigned int keyF9         = SDL_SCANCODE_F9;
    const unsigned int keyF10        = SDL_SCANCODE_F10;
    const unsigned int keyF11        = SDL_SCANCODE_F11;
    const unsigned int keyF12        = SDL_SCANCODE_F12;
    const unsigned int keyPAUSE      = SDL_SCANCODE_PAUSE;
    const unsigned int key1          = SDL_SCANCODE_1;
    const unsigned int key2          = SDL_SCANCODE_2;
    const unsigned int key3          = SDL_SCANCODE_3;
    const unsigned int key4          = SDL_SCANCODE_4;
    const unsigned int key5          = SDL_SCANCODE_5;
    const unsigned int key6          = SDL_SCANCODE_6;
    const unsigned int key7          = SDL_SCANCODE_7;
    const unsigned int key8          = SDL_SCANCODE_8;
    const unsigned int key9          = SDL_SCANCODE_9;
    const unsigned int key0          = SDL_SCANCODE_0;
    const unsigned int keyBACKSPACE  = SDL_SCANCODE_BACKSPACE;
    const unsigned int keyINSERT     = SDL_SCANCODE_INSERT;
    const unsigned int keyHOME       = SDL_SCANCODE_HOME;
    const unsigned int keyPAGEUP     = SDL_SCANCODE_PAGEUP;
    const unsigned int keyTAB        = SDL_SCANCODE_TAB;
    const unsigned int keyQ          = SDL_SCANCODE_Q;
    const unsigned int keyW          = SDL_SCANCODE_W;
    const unsigned int keyE          = SDL_SCANCODE_E;
    const unsigned int keyR          = SDL_SCANCODE_R;
    const unsigned int keyT          = SDL_SCANCODE_T;
    const unsigned int keyY          = SDL_SCANCODE_Y;
    const unsigned int keyU          = SDL_SCANCODE_U;
    const unsigned int keyI          = SDL_SCANCODE_I;
    const unsigned int keyO          = SDL_SCANCODE_O;
    const unsigned int keyP          = SDL_SCANCODE_P;
    const unsigned int keyDELETE     = SDL_SCANCODE_DELETE;
    const unsigned int keyEND        = SDL_SCANCODE_END;
    const unsigned int keyPAGEDOWN   = SDL_SCANCODE_PAGEDOWN;
    const unsigned int keyCAPSLOCK   = SDL_SCANCODE_CAPSLOCK;
    const unsigned int keyA          = SDL_SCANCODE_A;
    const unsigned int keyS          = SDL_SCANCODE_S;
    const unsigned int keyD          = SDL_SCANCODE_D;
    const unsigned int keyF          = SDL_SCANCODE_F;
    const unsigned int keyG          = SDL_SCANCODE_G;
    const unsigned int keyH          = SDL_SCANCODE_H;
    const unsigned int keyJ          = SDL_SCANCODE_J;
    const unsigned int keyK          = SDL_SCANCODE_K;
    const unsigned int keyL          = SDL_SCANCODE_L;
    const unsigned int keyRETURN     = SDL_SCANCODE_RETURN;
    const unsigned int keySHIFTLEFT  = SDL_SCANCODE_LSHIFT;
    const unsigned int keyZ          = SDL_SCANCODE_Z;
    const unsigned int keyX          = SDL_SCANCODE_X;
    const unsigned int keyC          = SDL_SCANCODE_C;
    const unsigned int keyV          = SDL_SCANCODE_V;
    const unsigned int keyB          = SDL_SCANCODE_B;
    const unsigned int keyN          = SDL_SCANCODE_N;
    const unsigned int keyM          = SDL_SCANCODE_M;
    const unsigned int keySHIFTRIGHT = SDL_SCANCODE_RSHIFT;
    const unsigned int keyARROWUP    = SDL_SCANCODE_UP;
    const unsigned int keyCTRLLEFT   = SDL_SCANCODE_LCTRL;
    const unsigned int keyAPPLEFT    = SDL_SCANCODE_LGUI;
    const unsigned int keyALT        = SDL_SCANCODE_LALT;
    const unsigned int keySPACE      = SDL_SCANCODE_SPACE;
    const unsigned int keyALTGR      = SDL_SCANCODE_RALT;
    const unsigned int keyAPPRIGHT   = SDL_SCANCODE_RGUI;
    const unsigned int keyMENU       = SDL_SCANCODE_MENU;
    const unsigned int keyCTRLRIGHT  = SDL_SCANCODE_RCTRL;
    const unsigned int keyARROWLEFT  = SDL_SCANCODE_LEFT;
    const unsigned int keyARROWDOWN  = SDL_SCANCODE_DOWN;
    const unsigned int keyARROWRIGHT = SDL_SCANCODE_RIGHT;
    const unsigned int keyPAD0       = SDL_SCANCODE_KP_0;
    const unsigned int keyPAD1       = SDL_SCANCODE_KP_1;
    const unsigned int keyPAD2       = SDL_SCANCODE_KP_2;
    const unsigned int keyPAD3       = SDL_SCANCODE_KP_3;
    const unsigned int keyPAD4       = SDL_SCANCODE_KP_4;
    const unsigned int keyPAD5       = SDL_SCANCODE_KP_5;
    const unsigned int keyPAD6       = SDL_SCANCODE_KP_6;
    const unsigned int keyPAD7       = SDL_SCANCODE_KP_7;
    const unsigned int keyPAD8       = SDL_SCANCODE_KP_8;
    const unsigned int keyPAD9       = SDL_SCANCODE_KP_9;
    const unsigned int keyPADADD     = SDL_SCANCODE_KP_PLUS;
    const unsigned int keyPADSUB     = SDL_SCANCODE_KP_MINUS;
    const unsigned int keyPADMUL     = SDL_SCANCODE_KP_MULTIPLY;
    const unsigned int keyPADDIV     = SDL_SCANCODE_KP_DIVIDE;
    const unsigned int keyENTER      = SDL_SCANCODE_KP_ENTER;

#else
    // Define random keycodes when no display is available.
    // (should rarely be used then!).
    const unsigned int keyESC        = 1U; //!< Keycode for the \c ESC key (architecture-dependent)
    const unsigned int keyF1         = 2U; //!< Keycode for the \c F1 key (architecture-dependent)
    const unsigned int keyF2         = 3U; //!< Keycode for the \c F2 key (architecture-dependent)
    const unsigned int keyF3         = 4U; //!< Keycode for the \c F3 key (architecture-dependent)
    const unsigned int keyF4         = 5U; //!< Keycode for the \c F4 key (architecture-dependent)
    const unsigned int keyF5         = 6U; //!< Keycode for the \c F5 key (architecture-dependent)
    const unsigned int keyF6         = 7U; //!< Keycode for the \c F6 key (architecture-dependent)
    const unsigned int keyF7         = 8U; //!< Keycode for the \c F7 key (architecture-dependent)
    const unsigned int keyF8         = 9U; //!< Keycode for the \c F8 key (architecture-dependent)
    const unsigned int keyF9         = 10U; //!< Keycode for the \c F9 key (architecture-dependent)
    const unsigned int keyF10        = 11U; //!< Keycode for the \c F10 key (architecture-dependent)
    const unsigned int keyF11        = 12U; //!< Keycode for the \c F11 key (architecture-dependent)
    const unsigned int keyF12        = 13U; //!< Keycode for the \c F12 key (architecture-dependent)
    const unsigned int keyPAUSE      = 14U; //!< Keycode for the \c PAUSE key (architecture-dependent)
    const unsigned int key1          = 15U; //!< Keycode for the \c 1 key (architecture-dependent)
    const unsigned int key2          = 16U; //!< Keycode for the \c 2 key (architecture-dependent)
    const unsigned int key3          = 17U; //!< Keycode for the \c 3 key (architecture-dependent)
    const unsigned int key4          = 18U; //!< Keycode for the \c 4 key (architecture-dependent)
    const unsigned int key5          = 19U; //!< Keycode for the \c 5 key (architecture-dependent)
    const unsigned int key6          = 20U; //!< Keycode for the \c 6 key (architecture-dependent)
    const unsigned int key7          = 21U; //!< Keycode for the \c 7 key (architecture-dependent)
    const unsigned int key8          = 22U; //!< Keycode for the \c 8 key (architecture-dependent)
    const unsigned int key9          = 23U; //!< Keycode for the \c 9 key (architecture-dependent)
    const unsigned int key0          = 24U; //!< Keycode for the \c 0 key (architecture-dependent)
    const unsigned int keyBACKSPACE  = 25U; //!< Keycode for the \c BACKSPACE key (architecture-dependent)
    const unsigned int keyINSERT     = 26U; //!< Keycode for the \c INSERT key (architecture-dependent)
    const unsigned int keyHOME       = 27U; //!< Keycode for the \c HOME key (architecture-dependent)
    const unsigned int keyPAGEUP     = 28U; //!< Keycode for the \c PAGEUP key (architecture-dependent)
    const unsigned int keyTAB        = 29U; //!< Keycode for the \c TAB key (architecture-dependent)
    const unsigned int keyQ          = 30U; //!< Keycode for the \c Q key (architecture-dependent)
    const unsigned int keyW          = 31U; //!< Keycode for the \c W key (architecture-dependent)
    const unsigned int keyE          = 32U; //!< Keycode for the \c E key (architecture-dependent)
    const unsigned int keyR          = 33U; //!< Keycode for the \c R key (architecture-dependent)
    const unsigned int keyT          = 34U; //!< Keycode for the \c T key (architecture-dependent)
    const unsigned int keyY          = 35U; //!< Keycode for the \c Y key (architecture-dependent)
    const unsigned int keyU          = 36U; //!< Keycode for the \c U key (architecture-dependent)
    const unsigned int keyI          = 37U; //!< Keycode for the \c I key (architecture-dependent)
    const unsigned int keyO          = 38U; //!< Keycode for the \c O key (architecture-dependent)
    const unsigned int keyP          = 39U; //!< Keycode for the \c P key (architecture-dependent)
    const unsigned int keyDELETE     = 40U; //!< Keycode for the \c DELETE key (architecture-dependent)
    const unsigned int keyEND        = 41U; //!< Keycode for the \c END key (architecture-dependent)
    const unsigned int keyPAGEDOWN   = 42U; //!< Keycode for the \c PAGEDOWN key (architecture-dependent)
    const unsigned int keyCAPSLOCK   = 43U; //!< Keycode for the \c CAPSLOCK key (architecture-dependent)
    const unsigned int keyA          = 44U; //!< Keycode for the \c A key (architecture-dependent)
    const unsigned int keyS          = 45U; //!< Keycode for the \c S key (architecture-dependent)
    const unsigned int keyD          = 46U; //!< Keycode for the \c D key (architecture-dependent)
    const unsigned int keyF          = 47U; //!< Keycode for the \c F key (architecture-dependent)
    const unsigned int keyG          = 48U; //!< Keycode for the \c G key (architecture-dependent)
    const unsigned int keyH          = 49U; //!< Keycode for the \c H key (architecture-dependent)
    const unsigned int keyJ          = 50U; //!< Keycode for the \c J key (architecture-dependent)
    const unsigned int keyK          = 51U; //!< Keycode for the \c K key (architecture-dependent)
    const unsigned int keyL          = 52U; //!< Keycode for the \c L key (architecture-dependent)
    const unsigned int keyRETURN     = 53U; //!< Keycode for the \c RETURN key (architecture-dependent)
    const unsigned int keySHIFTLEFT  = 54U; //!< Keycode for the \c SHIFTLEFT key (architecture-dependent)
    const unsigned int keyZ          = 55U; //!< Keycode for the \c Z key (architecture-dependent)
    const unsigned int keyX          = 56U; //!< Keycode for the \c X key (architecture-dependent)
    const unsigned int keyC          = 57U; //!< Keycode for the \c C key (architecture-dependent)
    const unsigned int keyV          = 58U; //!< Keycode for the \c V key (architecture-dependent)
    const unsigned int keyB          = 59U; //!< Keycode for the \c B key (architecture-dependent)
    const unsigned int keyN          = 60U; //!< Keycode for the \c N key (architecture-dependent)
    const unsigned int keyM          = 61U; //!< Keycode for the \c M key (architecture-dependent)
    const unsigned int keySHIFTRIGHT = 62U; //!< Keycode for the \c SHIFTRIGHT key (architecture-dependent)
    const unsigned int keyARROWUP    = 63U; //!< Keycode for the \c ARROWUP key (architecture-dependent)
    const unsigned int keyCTRLLEFT   = 64U; //!< Keycode for the \c CTRLLEFT key (architecture-dependent)
    const unsigned int keyAPPLEFT    = 65U; //!< Keycode for the \c APPLEFT key (architecture-dependent)
    const unsigned int keyALT        = 66U; //!< Keycode for the \c ALT key (architecture-dependent)
    const unsigned int keySPACE      = 67U; //!< Keycode for the \c SPACE key (architecture-dependent)
    const unsigned int keyALTGR      = 68U; //!< Keycode for the \c ALTGR key (architecture-dependent)
    const unsigned int keyAPPRIGHT   = 69U; //!< Keycode for the \c APPRIGHT key (architecture-dependent)
    const unsigned int keyMENU       = 70U; //!< Keycode for the \c MENU key (architecture-dependent)
    const unsigned int keyCTRLRIGHT  = 71U; //!< Keycode for the \c CTRLRIGHT key (architecture-dependent)
    const unsigned int keyARROWLEFT  = 72U; //!< Keycode for the \c ARROWLEFT key (architecture-dependent)
    const unsigned int keyARROWDOWN  = 73U; //!< Keycode for the \c ARROWDOWN key (architecture-dependent)
    const unsigned int keyARROWRIGHT = 74U; //!< Keycode for the \c ARROWRIGHT key (architecture-dependent)
    const unsigned int keyPAD0       = 75U; //!< Keycode for the \c PAD0 key (architecture-dependent)
    const unsigned int keyPAD1       = 76U; //!< Keycode for the \c PAD1 key (architecture-dependent)
    const unsigned int keyPAD2       = 77U; //!< Keycode for the \c PAD2 key (architecture-dependent)
    const unsigned int keyPAD3       = 78U; //!< Keycode for the \c PAD3 key (architecture-dependent)
    const unsigned int keyPAD4       = 79U; //!< Keycode for the \c PAD4 key (architecture-dependent)
    const unsigned int keyPAD5       = 80U; //!< Keycode for the \c PAD5 key (architecture-dependent)
    const unsigned int keyPAD6       = 81U; //!< Keycode for the \c PAD6 key (architecture-dependent)
    const unsigned int keyPAD7       = 82U; //!< Keycode for the \c PAD7 key (architecture-dependent)
    const unsigned int keyPAD8       = 83U; //!< Keycode for the \c PAD8 key (architecture-dependent)
    const unsigned int keyPAD9       = 84U; //!< Keycode for the \c PAD9 key (architecture-dependent)
    const unsigned int keyPADADD     = 85U; //!< Keycode for the \c PADADD key (architecture-dependent)
    const unsigned int keyPADSUB     = 86U; //!< Keycode for the \c PADSUB key (architecture-dependent)
    const unsigned int keyPADMUL     = 87U; //!< Keycode for the \c PADMUL key (architecture-dependent)
    const unsigned int keyPADDIV     = 88U; //!< Keycode for the \c PADDDIV key (architecture-dependent)
    const unsigned int keyENTER      = 89U; //!< Keycode for the \c ENTER key (architecture-dependent)
#endif

#endif
