#ifndef CIMG_MODULE_DISPLAY_X11_H
#define CIMG_MODULE_DISPLAY_X11_H

#if cimg_display==1

    Atom _wm_window_atom, _wm_protocol_atom;
    Window _window, _background_window;
    Colormap _colormap;
    XImage *_image;
    void *_data;

#ifdef cimg_use_xshm
    XShmSegmentInfo *_shminfo;
#endif

    // Internal functions.
    void _assign(const unsigned int dimw, const unsigned int dimh, const char *const p_title=0,
                 const unsigned int normalization_type=3,
                 const bool is_fullscreen=false, const bool closed_flag=false) {
      cimg::mutex(14);
      if (!_keys) _keys = new unsigned int[128];
      if (!_released_keys) _released_keys = new unsigned int[128];

      // Allocate space for window title.
      const char *const np_title = p_title?p_title:"";
      const unsigned int s = (unsigned int)std::strlen(np_title) + 1;
      char *const tmp_title = s?new char[s]:0;
      if (s) std::memcpy(tmp_title,np_title,s*sizeof(char));

      // Destroy previous display window if existing.
      if (!is_empty()) assign(false);

      // Open X11 display and retrieve graphical properties.
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display* &dpy = X11_attr.display;
      if (!dpy) {
        dpy = XOpenDisplay(0);
        if (!dpy)
          throw CImgDisplayException(_cimgdisplay_instance
                                     "assign(): Failed to open X11 display.",
                                     cimgdisplay_instance);

        X11_attr.nb_bits = DefaultDepth(dpy,DefaultScreen(dpy));
        if (X11_attr.nb_bits!=8 && X11_attr.nb_bits!=16 &&
            X11_attr.nb_bits!=24 && X11_attr.nb_bits!=32)
          throw CImgDisplayException(_cimgdisplay_instance
                                     "assign(): Invalid %u bits screen mode detected "
                                     "(only 8, 16, 24 and 32 bits modes are managed).",
                                     cimgdisplay_instance,
                                     X11_attr.nb_bits);
        XVisualInfo vtemplate;
        vtemplate.visualid = XVisualIDFromVisual(DefaultVisual(dpy,DefaultScreen(dpy)));
        int nb_visuals;
        XVisualInfo *vinfo = XGetVisualInfo(dpy,VisualIDMask,&vtemplate,&nb_visuals);
        if (vinfo && vinfo->red_mask<vinfo->blue_mask) X11_attr.is_blue_first = true;
        X11_attr.byte_order = ImageByteOrder(dpy);
        XFree(vinfo);
      }

      X11_attr.lock();
      if (!X11_attr.events_thread) {
        X11_attr.events_thread = new pthread_t;
        pthread_create(X11_attr.events_thread,0,_events_thread,0);
      }

      // Set display variables.
      _width = std::min(dimw,(unsigned int)screen_width());
      _height = std::min(dimh,(unsigned int)screen_height());
      _normalization = normalization_type<4?normalization_type:3;
      _is_fullscreen = is_fullscreen;
      _window_x = _window_y = cimg::type<int>::min();
      _is_closed = closed_flag;
      _title = tmp_title;
      flush();

      // Create X11 window (and LUT, if 8bits display).
      if (_is_fullscreen) {
        if (!_is_closed) _init_fullscreen();
        const unsigned int sx = screen_width(), sy = screen_height();
        XSetWindowAttributes attr_set;
        attr_set.override_redirect = 1;
        _window = XCreateWindow(dpy,DefaultRootWindow(dpy),(sx - _width)/2,(sy - _height)/2,_width,_height,0,0,
                                InputOutput,CopyFromParent,CWOverrideRedirect,&attr_set);
      } else
        _window = XCreateSimpleWindow(dpy,DefaultRootWindow(dpy),0,0,_width,_height,0,0L,0L);

      XSelectInput(dpy,_window,
                   ExposureMask | StructureNotifyMask | ButtonPressMask | KeyPressMask | PointerMotionMask |
                   EnterWindowMask | LeaveWindowMask | ButtonReleaseMask | KeyReleaseMask);

      XStoreName(dpy,_window,_title?_title:" ");
      if (X11_attr.nb_bits==8) {
        _colormap = XCreateColormap(dpy,_window,DefaultVisual(dpy,DefaultScreen(dpy)),AllocAll);
        _set_colormap(_colormap,3);
        XSetWindowColormap(dpy,_window,_colormap);
      }

      static const char *const _window_class = cimg_appname;
      XClassHint *const window_class = XAllocClassHint();
      window_class->res_name = (char*)_window_class;
      window_class->res_class = (char*)_window_class;
      XSetClassHint(dpy,_window,window_class);
      XFree(window_class);

      _window_width = _width;
      _window_height = _height;

      // Create XImage.
#ifdef cimg_use_xshm
      _shminfo = 0;
      if (XShmQueryExtension(dpy)) {
        _shminfo = new XShmSegmentInfo;
        _image = XShmCreateImage(dpy,DefaultVisual(dpy,DefaultScreen(dpy)),X11_attr.nb_bits,
                                 ZPixmap,0,_shminfo,_width,_height);
        if (!_image) { delete _shminfo; _shminfo = 0; }
        else {
          _shminfo->shmid = shmget(IPC_PRIVATE,_image->bytes_per_line*_image->height,IPC_CREAT|0777);
          if (_shminfo->shmid==-1) { XDestroyImage(_image); delete _shminfo; _shminfo = 0; }
          else {
            _shminfo->shmaddr = _image->data = (char*)(_data = shmat(_shminfo->shmid,0,0));
            if (_shminfo->shmaddr==(char*)-1) {
              shmctl(_shminfo->shmid,IPC_RMID,0); XDestroyImage(_image); delete _shminfo; _shminfo = 0;
            } else {
              _shminfo->readOnly = 0;
              X11_attr.is_shm_enabled = true;
              XErrorHandler oldXErrorHandler = XSetErrorHandler(_assign_xshm);
              XShmAttach(dpy,_shminfo);
              XSync(dpy,0);
              XSetErrorHandler(oldXErrorHandler);
              if (!X11_attr.is_shm_enabled) {
                shmdt(_shminfo->shmaddr); shmctl(_shminfo->shmid,IPC_RMID,0); XDestroyImage(_image);
                delete _shminfo; _shminfo = 0;
              }
            }
          }
        }
      }
      if (!_shminfo)
#endif
        {
          const cimg_ulong buf_size = (cimg_ulong)_width*_height*(X11_attr.nb_bits==8?1:
                                                                  (X11_attr.nb_bits==16?2:4));
          _data = std::malloc(buf_size);
          _image = XCreateImage(dpy,DefaultVisual(dpy,DefaultScreen(dpy)),X11_attr.nb_bits,
                                ZPixmap,0,(char*)_data,_width,_height,8,0);
        }

      _wm_window_atom = XInternAtom(dpy,"WM_DELETE_WINDOW",0);
      _wm_protocol_atom = XInternAtom(dpy,"WM_PROTOCOLS",0);
      XSetWMProtocols(dpy,_window,&_wm_window_atom,1);

      if (_is_fullscreen) XGrabKeyboard(dpy,_window,1,GrabModeAsync,GrabModeAsync,CurrentTime);
      if (X11_attr.nb_cimg_displays>=512) {
        X11_attr.unlock();
        throw CImgDisplayException("CImgDisplay::assign(): Max number of displays (512) already opened.");
      }
      X11_attr.cimg_displays[X11_attr.nb_cimg_displays++] = this;
      if (!_is_closed) _map_window(); else _window_x = _window_y = cimg::type<int>::min();
      X11_attr.unlock();
      cimg::mutex(14,0);
    }

    static int _assign_xshm(Display *dpy, XErrorEvent *error) {
      cimg::unused(dpy,error);
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      X11_attr.is_shm_enabled = false;
      return 0;
    }

    void _desinit_fullscreen() {
      if (!_is_fullscreen) return;
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display *const dpy = X11_attr.display;
      XUngrabKeyboard(dpy,CurrentTime);

#ifdef cimg_use_xrandr
      if (X11_attr.resolutions && X11_attr.curr_resolution) {
        XRRScreenConfiguration *config = XRRGetScreenInfo(dpy,DefaultRootWindow(dpy));
        XRRSetScreenConfig(dpy,config,DefaultRootWindow(dpy),0,X11_attr.curr_rotation,CurrentTime);
        XRRFreeScreenConfigInfo(config);
        XSync(dpy,0);
        X11_attr.curr_resolution = 0;
      }
#endif
      if (_background_window) XDestroyWindow(dpy,_background_window);
      _background_window = 0;
      _is_fullscreen = false;
    }

    static void* _events_thread(void*) { // Thread to manage events for all opened display windows
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      X11_attr.events_thread_running = true;

      Display *const dpy = X11_attr.display;
      XEvent event;
      while (X11_attr.events_thread_running) {
        X11_attr.lock();
        bool is_event = XCheckTypedEvent(dpy,ClientMessage,&event);
        if (!is_event) is_event = XCheckMaskEvent(dpy,
                                                  ExposureMask | StructureNotifyMask | ButtonPressMask |
                                                  KeyPressMask | PointerMotionMask | EnterWindowMask |
                                                  LeaveWindowMask | ButtonReleaseMask | KeyReleaseMask,&event);
        if (is_event) { // Find CImgDisplay associated to event
          for (unsigned int k = 0; k<X11_attr.nb_cimg_displays; ++k)
            if (event.xany.window==X11_attr.cimg_displays[k]->_window) {
              if (!X11_attr.cimg_displays[k]->_is_closed && X11_attr.events_thread_running)
                X11_attr.cimg_displays[k]->_process_event(&event);
              break;
            }
          X11_attr.unlock();
        } else {
          X11_attr.unlock();
          cimg::sleep(8);
        }
        pthread_testcancel();
      }
      return 0;
    }

    void _init_fullscreen() {
      if (!_is_fullscreen || _is_closed) return;
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display *const dpy = X11_attr.display;
      _background_window = 0;

#ifdef cimg_use_xrandr
      int foo;
      if (XRRQueryExtension(dpy,&foo,&foo)) {
        XRRRotations(dpy,DefaultScreen(dpy),&X11_attr.curr_rotation);
        if (!X11_attr.resolutions) {
          X11_attr.resolutions = XRRSizes(dpy,DefaultScreen(dpy),&foo);
          X11_attr.nb_resolutions = (unsigned int)foo;
        }
        if (X11_attr.resolutions) {
          X11_attr.curr_resolution = 0;
          for (unsigned int i = 0; i<X11_attr.nb_resolutions; ++i) {
            const unsigned int
              nw = (unsigned int)(X11_attr.resolutions[i].width),
              nh = (unsigned int)(X11_attr.resolutions[i].height);
            if (nw>=_width && nh>=_height &&
                nw<=(unsigned int)(X11_attr.resolutions[X11_attr.curr_resolution].width) &&
                nh<=(unsigned int)(X11_attr.resolutions[X11_attr.curr_resolution].height))
              X11_attr.curr_resolution = i;
          }
          if (X11_attr.curr_resolution>0) {
            XRRScreenConfiguration *config = XRRGetScreenInfo(dpy,DefaultRootWindow(dpy));
            XRRSetScreenConfig(dpy,config,DefaultRootWindow(dpy),
                               X11_attr.curr_resolution,X11_attr.curr_rotation,CurrentTime);
            XRRFreeScreenConfigInfo(config);
            XSync(dpy,0);
          }
        }
      }
      if (!X11_attr.resolutions)
        cimg::warn(_cimgdisplay_instance
                   "init_fullscreen(): Xrandr extension not supported by the X server.",
                   cimgdisplay_instance);
#endif

      const unsigned int sx = screen_width(), sy = screen_height();
      if (sx==_width && sy==_height) return;
      XSetWindowAttributes attr_set;

      attr_set.background_pixel = XBlackPixel(dpy,XDefaultScreen(dpy));
      attr_set.override_redirect = 1;
      _background_window = XCreateWindow(dpy,DefaultRootWindow(dpy),0,0,sx,sy,0,0,
                                         InputOutput,CopyFromParent,CWBackPixel | CWOverrideRedirect,&attr_set);
      XEvent event;
      XSelectInput(dpy,_background_window,StructureNotifyMask);
      XMapRaised(dpy,_background_window);
      do XWindowEvent(dpy,_background_window,StructureNotifyMask,&event);
      while (event.type!=MapNotify);

      XWindowAttributes attr;
      do {
        XGetWindowAttributes(dpy,_background_window,&attr);
        if (attr.map_state!=IsViewable) { XSync(dpy,0); cimg::sleep(10); }
      } while (attr.map_state!=IsViewable);
    }

    void _map_window() {
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display *const dpy = X11_attr.display;
      bool is_exposed = false, is_mapped = false;
      XWindowAttributes attr;
      XEvent event;
      XMapRaised(dpy,_window);
      do { // Wait for the window to be mapped
        XWindowEvent(dpy,_window,StructureNotifyMask | ExposureMask,&event);
        switch (event.type) {
        case MapNotify : is_mapped = true; break;
        case Expose : is_exposed = true; break;
        }
      } while (!is_exposed || !is_mapped);
      do { // Wait for the window to be visible
        XGetWindowAttributes(dpy,_window,&attr);
        if (attr.map_state!=IsViewable) { XSync(dpy,0); cimg::sleep(10); }
      } while (attr.map_state!=IsViewable);
      _window_x = attr.x;
      _window_y = attr.y;
    }

    void _paint(const bool wait_expose=true) {
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      if (_is_closed || !_image) return;
      Display *const dpy = X11_attr.display;
      if (wait_expose) { // Send an expose event sticked to display window to force repaint
        XEvent event;
        event.xexpose.type = Expose;
        event.xexpose.serial = 0;
        event.xexpose.send_event = 1;
        event.xexpose.display = dpy;
        event.xexpose.window = _window;
        event.xexpose.x = 0;
        event.xexpose.y = 0;
        event.xexpose.width = width();
        event.xexpose.height = height();
        event.xexpose.count = 0;
        XSendEvent(dpy,_window,0,0,&event);
      } else { // Repaint directly (may be called from the expose event)
        GC gc = DefaultGC(dpy,DefaultScreen(dpy));

#ifdef cimg_use_xshm
        if (_shminfo) XShmPutImage(dpy,_window,gc,_image,0,0,0,0,_width,_height,1);
        else XPutImage(dpy,_window,gc,_image,0,0,0,0,_width,_height);
#else
        XPutImage(dpy,_window,gc,_image,0,0,0,0,_width,_height);
#endif
      }
    }

    void _process_event(XEvent *const p_event) {
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display *const dpy = X11_attr.display;
      XEvent &event = *p_event;
      bool is_event = false;

      switch (event.type) {
      case ClientMessage : {
        if ((int)event.xclient.message_type==(int)_wm_protocol_atom &&
            (int)event.xclient.data.l[0]==(int)_wm_window_atom) {
          XUnmapWindow(X11_attr.display,_window);
          _is_closed = is_event = true;
        }
      } break;
      case ConfigureNotify : {
        while (XCheckWindowEvent(dpy,_window,StructureNotifyMask,&event)) {}
        const unsigned int nw = event.xconfigure.width, nh = event.xconfigure.height;
        const int nx = event.xconfigure.x, ny = event.xconfigure.y;
        if (nw && nh && (nw!=_window_width || nh!=_window_height)) {
          _window_width = nw; _window_height = nh; _mouse_x = _mouse_y = -1;
          XResizeWindow(dpy,_window,_window_width,_window_height);
          _is_resized = is_event = true;
        }
        if (nx!=_window_x || ny!=_window_y) {
          _window_x = nx;
          _window_y = ny;
          _is_moved = is_event = true;
        }
      } break;
      case Expose : {
        while (XCheckWindowEvent(dpy,_window,ExposureMask,&event)) {}
        _paint(false);
        if (_is_fullscreen) {
          XWindowAttributes attr;
          do {
            XGetWindowAttributes(dpy,_window,&attr);
            if (attr.map_state!=IsViewable) { XSync(dpy,0); cimg::sleep(10); }
          } while (attr.map_state!=IsViewable);
          XSetInputFocus(dpy,_window,RevertToParent,CurrentTime);
        }
      } break;
      case ButtonPress : {
        do {
          _mouse_x = event.xmotion.x; _mouse_y = event.xmotion.y;
          if (_mouse_x<0 || _mouse_y<0 || _mouse_x>=width() || _mouse_y>=height()) _mouse_x = _mouse_y = -1;
          switch (event.xbutton.button) {
          case 1 : set_button(1); break;
          case 3 : set_button(2); break;
          case 2 : set_button(3); break;
          }
        } while (XCheckWindowEvent(dpy,_window,ButtonPressMask,&event));
      } break;
      case ButtonRelease : {
        do {
          _mouse_x = event.xmotion.x; _mouse_y = event.xmotion.y;
          if (_mouse_x<0 || _mouse_y<0 || _mouse_x>=width() || _mouse_y>=height()) _mouse_x = _mouse_y = -1;
          switch (event.xbutton.button) {
          case 1 : set_button(1,false); break;
          case 3 : set_button(2,false); break;
          case 2 : set_button(3,false); break;
          case 4 : set_wheel(1); break;
          case 5 : set_wheel(-1); break;
          }
        } while (XCheckWindowEvent(dpy,_window,ButtonReleaseMask,&event));
      } break;
      case KeyPress : {
        char tmp = 0; KeySym ksym;
        XLookupString(&event.xkey,&tmp,1,&ksym,0);
        set_key((unsigned int)ksym,true);
      } break;
      case KeyRelease : {
        char keys_return[32]; // Check that the key has been physically unpressed
        XQueryKeymap(dpy,keys_return);
        const unsigned int kc = event.xkey.keycode, kc1 = kc/8, kc2 = kc%8;
        const bool is_key_pressed = kc1>=32?false:(keys_return[kc1]>>kc2)&1;
        if (!is_key_pressed) {
          char tmp = 0; KeySym ksym;
          XLookupString(&event.xkey,&tmp,1,&ksym,0);
          set_key((unsigned int)ksym,false);
        }
      } break;
      case EnterNotify: {
        while (XCheckWindowEvent(dpy,_window,EnterWindowMask,&event)) {}
        _mouse_x = event.xmotion.x;
        _mouse_y = event.xmotion.y;
        if (_mouse_x<0 || _mouse_y<0 || _mouse_x>=width() || _mouse_y>=height()) _mouse_x = _mouse_y = -1;
      } break;
      case LeaveNotify : {
        while (XCheckWindowEvent(dpy,_window,LeaveWindowMask,&event)) {}
        _mouse_x = _mouse_y = -1; is_event = true;
      } break;
      case MotionNotify : {
        while (XCheckWindowEvent(dpy,_window,PointerMotionMask,&event)) {}
        _mouse_x = event.xmotion.x;
        _mouse_y = event.xmotion.y;
        if (_mouse_x<0 || _mouse_y<0 || _mouse_x>=width() || _mouse_y>=height()) _mouse_x = _mouse_y = -1;
        is_event = true;
      } break;
      }
      if (is_event) {
        _is_event = true;
        pthread_cond_broadcast(&X11_attr.wait_event);
      }
    }

    template<typename T>
    void _resize(T pixel_type, const unsigned int ndimx, const unsigned int ndimy, const bool force_redraw) {
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display *const dpy = X11_attr.display;
      cimg::unused(pixel_type);

#ifdef cimg_use_xshm
      if (_shminfo) {
        XShmSegmentInfo *const nshminfo = new XShmSegmentInfo;
        XImage *const nimage = XShmCreateImage(dpy,DefaultVisual(dpy,DefaultScreen(dpy)),
                                               X11_attr.nb_bits,ZPixmap,0,nshminfo,ndimx,ndimy);
        if (!nimage) { delete nshminfo; return; }
        else {
          nshminfo->shmid = shmget(IPC_PRIVATE,ndimx*ndimy*sizeof(T),IPC_CREAT | 0777);
          if (nshminfo->shmid==-1) { XDestroyImage(nimage); delete nshminfo; return; }
          else {
            nshminfo->shmaddr = nimage->data = (char*)shmat(nshminfo->shmid,0,0);
            if (nshminfo->shmaddr==(char*)-1) {
              shmctl(nshminfo->shmid,IPC_RMID,0); XDestroyImage(nimage); delete nshminfo; return;
            } else {
              nshminfo->readOnly = 0;
              X11_attr.is_shm_enabled = true;
              XErrorHandler oldXErrorHandler = XSetErrorHandler(_assign_xshm);
              XShmAttach(dpy,nshminfo);
              XFlush(dpy);
              XSetErrorHandler(oldXErrorHandler);
              if (!X11_attr.is_shm_enabled) {
                shmdt(nshminfo->shmaddr);
                shmctl(nshminfo->shmid,IPC_RMID,0);
                XDestroyImage(nimage);
                delete nshminfo;
                return;
              } else {
                T *const ndata = (T*)nimage->data;
                if (force_redraw) _render_resize((T*)_data,_width,_height,ndata,ndimx,ndimy);
                else std::memset(ndata,0,sizeof(T)*ndimx*ndimy);
                XShmDetach(dpy,_shminfo);
                XDestroyImage(_image);
                shmdt(_shminfo->shmaddr);
                shmctl(_shminfo->shmid,IPC_RMID,0);
                delete _shminfo;
                _shminfo = nshminfo;
                _image = nimage;
                _data = (void*)ndata;
              }
            }
          }
        }
      } else
#endif
        {
          T *ndata = (T*)std::malloc(ndimx*ndimy*sizeof(T));
          if (force_redraw) _render_resize((T*)_data,_width,_height,ndata,ndimx,ndimy);
          else std::memset(ndata,0,sizeof(T)*ndimx*ndimy);
          _data = (void*)ndata;
          XDestroyImage(_image);
          _image = XCreateImage(dpy,DefaultVisual(dpy,DefaultScreen(dpy)),
                                X11_attr.nb_bits,ZPixmap,0,(char*)_data,ndimx,ndimy,8,0);
        }
    }

    void _set_colormap(Colormap& cmap, const unsigned int dim) {
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      XColor *const colormap = new XColor[256];
      switch (dim) {
      case 1 : { // colormap for greyscale images
        for (unsigned int index = 0; index<256; ++index) {
          colormap[index].pixel = index;
          colormap[index].red = colormap[index].green = colormap[index].blue = (unsigned short)(index<<8);
          colormap[index].flags = DoRed | DoGreen | DoBlue;
        }
      } break;
      case 2 : { // colormap for RG images
        for (unsigned int index = 0, r = 8; r<256; r+=16)
          for (unsigned int g = 8; g<256; g+=16) {
            colormap[index].pixel = index;
            colormap[index].red = colormap[index].blue = (unsigned short)(r<<8);
            colormap[index].green = (unsigned short)(g<<8);
            colormap[index++].flags = DoRed | DoGreen | DoBlue;
          }
      } break;
      default : { // colormap for RGB images
        for (unsigned int index = 0, r = 16; r<256; r+=32)
          for (unsigned int g = 16; g<256; g+=32)
            for (unsigned int b = 32; b<256; b+=64) {
              colormap[index].pixel = index;
              colormap[index].red = (unsigned short)(r<<8);
              colormap[index].green = (unsigned short)(g<<8);
              colormap[index].blue = (unsigned short)(b<<8);
              colormap[index++].flags = DoRed | DoGreen | DoBlue;
            }
      }
      }
      XStoreColors(X11_attr.display,cmap,colormap,256);
      delete[] colormap;
    }

    // Public functions.
    static int screen_width() {
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display *const dpy = X11_attr.display;
      int res = 0;
      if (!dpy) {
        Display *const _dpy = XOpenDisplay(0);
        if (!_dpy)
          throw CImgDisplayException("CImgDisplay::screen_width(): Failed to open X11 display.");
        res = DisplayWidth(_dpy,DefaultScreen(_dpy));
        XCloseDisplay(_dpy);
      } else {

#ifdef cimg_use_xrandr
        if (X11_attr.resolutions && X11_attr.curr_resolution)
          res = X11_attr.resolutions[X11_attr.curr_resolution].width;
        else res = DisplayWidth(dpy,DefaultScreen(dpy));
#else
        res = DisplayWidth(dpy,DefaultScreen(dpy));
#endif
      }
      return res;
    }

    static int screen_height() {
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display *const dpy = X11_attr.display;
      int res = 0;
      if (!dpy) {
        Display *const _dpy = XOpenDisplay(0);
        if (!_dpy)
          throw CImgDisplayException("CImgDisplay::screen_height(): Failed to open X11 display.");
        res = DisplayHeight(_dpy,DefaultScreen(_dpy));
        XCloseDisplay(_dpy);
      } else {

#ifdef cimg_use_xrandr
        if (X11_attr.resolutions && X11_attr.curr_resolution)
          res = X11_attr.resolutions[X11_attr.curr_resolution].height;
        else res = DisplayHeight(dpy,DefaultScreen(dpy));
#else
        res = DisplayHeight(dpy,DefaultScreen(dpy));
#endif
      }
      return res;
    }

    static void wait_all() {
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      if (!X11_attr.display) return;
      pthread_mutex_lock(&X11_attr.mutex_wait_event);
      pthread_cond_wait(&X11_attr.wait_event,&X11_attr.mutex_wait_event);
      pthread_mutex_unlock(&X11_attr.mutex_wait_event);
    }

    CImgDisplay& wait(const unsigned int milliseconds) {
      cimg::wait(milliseconds,&_timer);
      return *this;
    }

    CImgDisplay& assign(const bool allow_terminate_events_thread=true) {
      if (!_keys) _keys = new unsigned int[128];
      if (!_released_keys) _released_keys = new unsigned int[128];
      if (is_empty()) return flush();
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display *const dpy = X11_attr.display;
      if (!dpy) return *this;
      X11_attr.lock();

      // Remove display window from event thread list.
      unsigned int i;
      for (i = 0; i<X11_attr.nb_cimg_displays && X11_attr.cimg_displays[i]!=this; ++i) {}
      for ( ; i<X11_attr.nb_cimg_displays - 1; ++i)
        X11_attr.cimg_displays[i] = X11_attr.cimg_displays[i + 1];
      --X11_attr.nb_cimg_displays;
      if (!X11_attr.nb_cimg_displays && allow_terminate_events_thread)
        X11_attr.unlock().terminate_events_thread().lock();

      // Destroy associated ressources.
      if (_is_fullscreen && !_is_closed) _desinit_fullscreen();

#ifdef cimg_use_xshm
      if (_shminfo) {
        XShmDetach(dpy,_shminfo);
        shmdt(_shminfo->shmaddr);
        shmctl(_shminfo->shmid,IPC_RMID,0);
        delete _shminfo;
        _shminfo = 0;
      }
#endif

      XDestroyImage(_image);
      if (X11_attr.nb_bits==8) XFreeColormap(dpy,_colormap);
      XDestroyWindow(dpy,_window);
      XSync(dpy,0);
      _window = 0; _colormap = 0; _data = 0; _image = 0;

      // Reset display variables.
      delete[] _title;
      _width = _height = _normalization = _window_width = _window_height = 0;
      _window_x = _window_y = cimg::type<int>::min();
      _is_fullscreen = false;
      _is_closed = true;
      _min = _max = 0;
      _title = 0;
      flush();

      X11_attr.unlock();
      return *this;
    }

    CImgDisplay& assign(const unsigned int dimw, const unsigned int dimh, const char *const title=0,
                        const unsigned int normalization_type=3,
                        const bool is_fullscreen=false, const bool closed_flag=false) {
      if (!dimw || !dimh) return assign();
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      _assign(dimw,dimh,title,normalization_type,is_fullscreen,closed_flag);
      _min = _max = 0;
      std::memset(_data,0,(X11_attr.nb_bits==8?sizeof(unsigned char):
                           (X11_attr.nb_bits==16?sizeof(unsigned short):sizeof(unsigned int)))*
                  (size_t)_width*_height);
      return paint();
    }

    template<typename T>
    CImgDisplay& assign(const CImg<T>& img, const char *const title=0,
                        const unsigned int normalization_type=3,
                        const bool is_fullscreen=false, const bool closed_flag=false) {
      if (!img) return assign();
      CImg<T> tmp;
      const CImg<T>& nimg = (img._depth==1)?img:(tmp=img.get_projections2d((img._width - 1)/2,
                                                                           (img._height - 1)/2,
                                                                           (img._depth - 1)/2));
      _assign(nimg._width,nimg._height,title,normalization_type,is_fullscreen,closed_flag);
      if (_normalization==2) _min = (float)nimg.min_max(_max);
      return render(nimg).paint();
    }

    template<typename T>
    CImgDisplay& assign(const CImgList<T>& list, const char *const title=0,
                        const unsigned int normalization_type=3,
                        const bool is_fullscreen=false, const bool closed_flag=false) {
      if (!list) return assign();
      CImg<T> tmp;
      const CImg<T> img = list>'x', &nimg = (img._depth==1)?img:(tmp=img.get_projections2d((img._width - 1)/2,
                                                                                           (img._height - 1)/2,
                                                                                           (img._depth - 1)/2));
      _assign(nimg._width,nimg._height,title,normalization_type,is_fullscreen,closed_flag);
      if (_normalization==2) _min = (float)nimg.min_max(_max);
      return render(nimg).paint();
    }

    CImgDisplay& assign(const CImgDisplay& disp) {
      if (!disp) return assign();
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      _assign(disp._width,disp._height,disp._title,disp._normalization,disp._is_fullscreen,disp._is_closed);
      std::memcpy(_data,disp._data,(X11_attr.nb_bits==8?sizeof(unsigned char):
                                    X11_attr.nb_bits==16?sizeof(unsigned short):
                                    sizeof(unsigned int))*(size_t)_width*_height);
      return paint();
    }

    CImgDisplay& resize(const int nwidth, const int nheight, const bool force_redraw=true) {
      if (!nwidth || !nheight || (is_empty() && (nwidth<0 || nheight<0))) return assign();
      if (is_empty()) return assign(nwidth,nheight);
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display *const dpy = X11_attr.display;
      const unsigned int
        tmpdimx = (nwidth>0)?nwidth:(-nwidth*width()/100),
        tmpdimy = (nheight>0)?nheight:(-nheight*height()/100),
        dimx = tmpdimx?tmpdimx:1,
        dimy = tmpdimy?tmpdimy:1;
      if (_width!=dimx || _height!=dimy || _window_width!=dimx || _window_height!=dimy) {
        show();
        X11_attr.lock();
        if (_window_width!=dimx || _window_height!=dimy) {
          XWindowAttributes attr;
          for (unsigned int i = 0; i<10; ++i) {
            XResizeWindow(dpy,_window,dimx,dimy);
            XGetWindowAttributes(dpy,_window,&attr);
            if (attr.width==(int)dimx && attr.height==(int)dimy) break;
            cimg::wait(5,&_timer);
          }
        }
        if (_width!=dimx || _height!=dimy) switch (X11_attr.nb_bits) {
          case 8 :  { unsigned char pixel_type = 0; _resize(pixel_type,dimx,dimy,force_redraw); } break;
          case 16 : { unsigned short pixel_type = 0; _resize(pixel_type,dimx,dimy,force_redraw); } break;
          default : { unsigned int pixel_type = 0; _resize(pixel_type,dimx,dimy,force_redraw); }
          }
        _window_width = _width = dimx; _window_height = _height = dimy;
        X11_attr.unlock();
      }
      _is_resized = false;
      if (_is_fullscreen) move((screen_width() - _width)/2,(screen_height() - _height)/2);
      if (force_redraw) return paint();
      return *this;
    }

    CImgDisplay& toggle_fullscreen(const bool force_redraw=true) {
      if (is_empty()) return *this;
      if (force_redraw) {
        cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
        const cimg_ulong buf_size = (cimg_ulong)_width*_height*(X11_attr.nb_bits==8?1:(X11_attr.nb_bits==16?2:4));
        void *image_data = std::malloc(buf_size);
        std::memcpy(image_data,_data,buf_size);
        assign(_width,_height,_title,_normalization,!_is_fullscreen,false);
        std::memcpy(_data,image_data,buf_size);
        std::free(image_data);
        return paint();
      }
      return assign(_width,_height,_title,_normalization,!_is_fullscreen,false);
    }

    CImgDisplay& show() {
      if (is_empty() || !_is_closed) return *this;
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      X11_attr.lock();
      _is_closed = false;
      if (_is_fullscreen) _init_fullscreen();
      _map_window();
      X11_attr.unlock();
      return paint();
    }

    CImgDisplay& close() {
      if (is_empty() || _is_closed) return *this;
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display *const dpy = X11_attr.display;
      X11_attr.lock();
      if (_is_fullscreen) _desinit_fullscreen();
      XUnmapWindow(dpy,_window);
      _window_x = _window_y = cimg::type<int>::min();
      _is_closed = true;
      X11_attr.unlock();
      return *this;
    }

    CImgDisplay& move(const int posx, const int posy) {
      if (is_empty()) return *this;
      show();
      if (_window_x!=posx || _window_y!=posy) {
        cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
        Display *const dpy = X11_attr.display;
        X11_attr.lock();
        XMoveWindow(dpy,_window,posx,posy);
        _window_x = posx;
        _window_y = posy;
        X11_attr.unlock();
      }
      _is_moved = false;
      return paint();
    }

    CImgDisplay& show_mouse() {
      if (is_empty()) return *this;
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display *const dpy = X11_attr.display;
      X11_attr.lock();
      XUndefineCursor(dpy,_window);
      X11_attr.unlock();
      return *this;
    }

    CImgDisplay& hide_mouse() {
      if (is_empty()) return *this;
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display *const dpy = X11_attr.display;
      X11_attr.lock();
      static const char pix_data[8] = {};
      XColor col;
      col.red = col.green = col.blue = 0;
      Pixmap pix = XCreateBitmapFromData(dpy,_window,pix_data,8,8);
      Cursor cur = XCreatePixmapCursor(dpy,pix,pix,&col,&col,0,0);
      XFreePixmap(dpy,pix);
      XDefineCursor(dpy,_window,cur);
      X11_attr.unlock();
      return *this;
    }

    CImgDisplay& set_mouse(const int posx, const int posy) {
      if (is_empty() || _is_closed) return *this;
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display *const dpy = X11_attr.display;
      X11_attr.lock();
      XWarpPointer(dpy,0L,_window,0,0,0,0,posx,posy);
      _mouse_x = posx; _mouse_y = posy;
      _is_moved = false;
      XSync(dpy,0);
      X11_attr.unlock();
      return *this;
    }

    CImgDisplay& set_title(const char *const format, ...) {
      if (is_empty()) return *this;
      char *const tmp = new char[1024];
      va_list ap;
      va_start(ap, format);
      cimg_vsnprintf(tmp,1024,format,ap);
      va_end(ap);
      if (!std::strcmp(_title,tmp)) { delete[] tmp; return *this; }
      delete[] _title;
      const unsigned int s = (unsigned int)std::strlen(tmp) + 1;
      _title = new char[s];
      std::memcpy(_title,tmp,s*sizeof(char));

      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display *const dpy = X11_attr.display;
      X11_attr.lock();
      XStoreName(dpy,_window,tmp);
      X11_attr.unlock();
      delete[] tmp;
      return *this;
    }

    template<typename T>
    CImgDisplay& display(const CImg<T>& img) {
      if (!img)
        throw CImgArgumentException(_cimgdisplay_instance
                                    "display(): Empty specified image.",
                                    cimgdisplay_instance);
      if (is_empty()) return assign(img);
      return render(img).paint(false);
    }

    CImgDisplay& paint(const bool wait_expose=true) {
      if (is_empty()) return *this;
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      X11_attr.lock();
      _paint(wait_expose);
      X11_attr.unlock();
      return *this;
    }

    template<typename T>
    CImgDisplay& render(const CImg<T>& img, const bool flag8=false) {
      if (!img)
        throw CImgArgumentException(_cimgdisplay_instance
                                    "render(): Empty specified image.",
                                    cimgdisplay_instance);
      if (is_empty()) return *this;
      if (img._depth!=1) return render(img.get_projections2d((img._width - 1)/2,(img._height - 1)/2,
                                                             (img._depth - 1)/2));

      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      if (X11_attr.nb_bits==8 && (img._width!=_width || img._height!=_height))
        return render(img.get_resize(_width,_height,1,-100,1));
      if (X11_attr.nb_bits==8 && !flag8 && img._spectrum==3) {
        static const CImg<typename CImg<T>::ucharT> default_colormap = CImg<typename CImg<T>::ucharT>::default_LUT256();
        return render(img.get_index(default_colormap,1,false));
      }

      const T
        *data1 = img._data,
        *data2 = (img._spectrum>1)?img.data(0,0,0,1):data1,
        *data3 = (img._spectrum>2)?img.data(0,0,0,2):data1;

      if (X11_attr.is_blue_first) cimg::swap(data1,data3);
      X11_attr.lock();

      if (!_normalization || (_normalization==3 && cimg::type<T>::string()==cimg::type<unsigned char>::string())) {
        _min = _max = 0;
        switch (X11_attr.nb_bits) {
        case 8 : { // 256 colormap, no normalization
          _set_colormap(_colormap,img._spectrum);
          unsigned char
            *const ndata = (img._width==_width && img._height==_height)?(unsigned char*)_data:
            new unsigned char[(size_t)img._width*img._height],
            *ptrd = (unsigned char*)ndata;
          switch (img._spectrum) {
          case 1 :
            for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
              (*ptrd++) = (unsigned char)*(data1++);
            break;
          case 2 : for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
              const unsigned char
                R = (unsigned char)*(data1++),
                G = (unsigned char)*(data2++);
              (*ptrd++) = (R&0xf0) | (G>>4);
            } break;
          default : for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
              const unsigned char
                R = (unsigned char)*(data1++),
                G = (unsigned char)*(data2++),
                B = (unsigned char)*(data3++);
              (*ptrd++) = (R&0xe0) | ((G>>5)<<2) | (B>>6);
            }
          }
          if (ndata!=_data) {
            _render_resize(ndata,img._width,img._height,(unsigned char*)_data,_width,_height);
            delete[] ndata;
          }
        } break;
        case 16 : { // 16 bits colors, no normalization
          unsigned short *const ndata = (img._width==_width && img._height==_height)?(unsigned short*)_data:
            new unsigned short[(size_t)img._width*img._height];
          unsigned char *ptrd = (unsigned char*)ndata;
          const unsigned int M = 248;
          switch (img._spectrum) {
          case 1 :
            if (X11_attr.byte_order)
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char val = (unsigned char)*(data1++), G = val>>2;
                ptrd[0] = (val&M) | (G>>3);
                ptrd[1] = (G<<5) | (G>>1);
                ptrd+=2;
              } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char val = (unsigned char)*(data1++), G = val>>2;
                ptrd[0] = (G<<5) | (G>>1);
                ptrd[1] = (val&M) | (G>>3);
                ptrd+=2;
              }
            break;
          case 2 :
            if (X11_attr.byte_order)
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)*(data2++)>>2;
                ptrd[0] = ((unsigned char)*(data1++)&M) | (G>>3);
                ptrd[1] = (G<<5);
                ptrd+=2;
              } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)*(data2++)>>2;
                ptrd[0] = (G<<5);
                ptrd[1] = ((unsigned char)*(data1++)&M) | (G>>3);
                ptrd+=2;
              }
            break;
          default :
            if (X11_attr.byte_order)
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)*(data2++)>>2;
                ptrd[0] = ((unsigned char)*(data1++)&M) | (G>>3);
                ptrd[1] = (G<<5) | ((unsigned char)*(data3++)>>3);
                ptrd+=2;
              } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)*(data2++)>>2;
                ptrd[0] = (G<<5) | ((unsigned char)*(data3++)>>3);
                ptrd[1] = ((unsigned char)*(data1++)&M) | (G>>3);
                ptrd+=2;
              }
          }
          if (ndata!=_data) {
            _render_resize(ndata,img._width,img._height,(unsigned short*)_data,_width,_height);
            delete[] ndata;
          }
        } break;
        default : { // 24 bits colors, no normalization
          unsigned int *const ndata = (img._width==_width && img._height==_height)?(unsigned int*)_data:
            new unsigned int[(size_t)img._width*img._height];
          if (sizeof(int)==4) { // 32 bits int uses optimized version
            unsigned int *ptrd = ndata;
            switch (img._spectrum) {
            case 1 :
              if (X11_attr.byte_order==cimg::endianness())
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  const unsigned char val = (unsigned char)*(data1++);
                  *(ptrd++) = (val<<16) | (val<<8) | val;
                }
              else
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                 const unsigned char val = (unsigned char)*(data1++);
                  *(ptrd++) = (val<<16) | (val<<8) | val;
                }
              break;
            case 2 :
              if (X11_attr.byte_order==cimg::endianness())
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) = ((unsigned char)*(data1++)<<16) | ((unsigned char)*(data2++)<<8);
              else
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) = ((unsigned char)*(data2++)<<16) | ((unsigned char)*(data1++)<<8);
              break;
            default :
              if (X11_attr.byte_order==cimg::endianness())
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) = ((unsigned char)*(data1++)<<16) | ((unsigned char)*(data2++)<<8) |
                    (unsigned char)*(data3++);
              else
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) = ((unsigned char)*(data3++)<<24) | ((unsigned char)*(data2++)<<16) |
                    ((unsigned char)*(data1++)<<8);
            }
          } else {
            unsigned char *ptrd = (unsigned char*)ndata;
            switch (img._spectrum) {
            case 1 :
              if (X11_attr.byte_order)
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  ptrd[0] = 0;
                  ptrd[1] = (unsigned char)*(data1++);
                  ptrd[2] = 0;
                  ptrd[3] = 0;
                  ptrd+=4;
                } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  ptrd[0] = 0;
                  ptrd[1] = 0;
                  ptrd[2] = (unsigned char)*(data1++);
                  ptrd[3] = 0;
                  ptrd+=4;
                }
              break;
            case 2 :
              if (X11_attr.byte_order) cimg::swap(data1,data2);
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                ptrd[0] = 0;
                ptrd[1] = (unsigned char)*(data2++);
                ptrd[2] = (unsigned char)*(data1++);
                ptrd[3] = 0;
                ptrd+=4;
              }
              break;
            default :
              if (X11_attr.byte_order)
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  ptrd[0] = 0;
                  ptrd[1] = (unsigned char)*(data1++);
                  ptrd[2] = (unsigned char)*(data2++);
                  ptrd[3] = (unsigned char)*(data3++);
                  ptrd+=4;
                } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  ptrd[0] = (unsigned char)*(data3++);
                  ptrd[1] = (unsigned char)*(data2++);
                  ptrd[2] = (unsigned char)*(data1++);
                  ptrd[3] = 0;
                  ptrd+=4;
                }
            }
          }
          if (ndata!=_data) {
            _render_resize(ndata,img._width,img._height,(unsigned int*)_data,_width,_height);
            delete[] ndata;
          }
        }
        }
      } else {
        if (_normalization==3) {
          if (sizeof(T)>1 && cimg::type<T>::string()!=cimg::type<bool>::string()) _min = (float)img.min_max(_max);
          else { _min = (float)cimg::type<T>::min(); _max = (float)cimg::type<T>::max(); }
        } else if ((_min>_max) || _normalization==1) _min = (float)img.min_max(_max);
        const float delta = _max - _min, mm = 255/(delta?delta:1.f);
        switch (X11_attr.nb_bits) {
        case 8 : { // 256 colormap, with normalization
          _set_colormap(_colormap,img._spectrum);
          unsigned char *const ndata = (img._width==_width && img._height==_height)?(unsigned char*)_data:
            new unsigned char[(size_t)img._width*img._height];
          unsigned char *ptrd = (unsigned char*)ndata;
          switch (img._spectrum) {
          case 1 : for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
              const unsigned char R = (unsigned char)((*(data1++) - _min)*mm);
              *(ptrd++) = R;
            } break;
          case 2 : for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
              const unsigned char
                R = (unsigned char)((*(data1++) - _min)*mm),
                G = (unsigned char)((*(data2++) - _min)*mm);
            (*ptrd++) = (R&0xf0) | (G>>4);
          } break;
          default :
            for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
              const unsigned char
                R = (unsigned char)((*(data1++) - _min)*mm),
                G = (unsigned char)((*(data2++) - _min)*mm),
                B = (unsigned char)((*(data3++) - _min)*mm);
              *(ptrd++) = (R&0xe0) | ((G>>5)<<2) | (B>>6);
            }
          }
          if (ndata!=_data) {
            _render_resize(ndata,img._width,img._height,(unsigned char*)_data,_width,_height);
            delete[] ndata;
          }
        } break;
        case 16 : { // 16 bits colors, with normalization
          unsigned short *const ndata = (img._width==_width && img._height==_height)?(unsigned short*)_data:
            new unsigned short[(size_t)img._width*img._height];
          unsigned char *ptrd = (unsigned char*)ndata;
          const unsigned int M = 248;
          switch (img._spectrum) {
          case 1 :
            if (X11_attr.byte_order)
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char val = (unsigned char)((*(data1++) - _min)*mm), G = val>>2;
                ptrd[0] = (val&M) | (G>>3);
                ptrd[1] = (G<<5) | (val>>3);
                ptrd+=2;
              } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char val = (unsigned char)((*(data1++) - _min)*mm), G = val>>2;
                ptrd[0] = (G<<5) | (val>>3);
                ptrd[1] = (val&M) | (G>>3);
                ptrd+=2;
              }
            break;
          case 2 :
            if (X11_attr.byte_order)
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)((*(data2++) - _min)*mm)>>2;
                ptrd[0] = ((unsigned char)((*(data1++) - _min)*mm)&M) | (G>>3);
                ptrd[1] = (G<<5);
                ptrd+=2;
              } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)((*(data2++) - _min)*mm)>>2;
                ptrd[0] = (G<<5);
                ptrd[1] = ((unsigned char)((*(data1++) - _min)*mm)&M) | (G>>3);
                ptrd+=2;
              }
            break;
          default :
            if (X11_attr.byte_order)
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)((*(data2++) - _min)*mm)>>2;
                ptrd[0] = ((unsigned char)((*(data1++) - _min)*mm)&M) | (G>>3);
                ptrd[1] = (G<<5) | ((unsigned char)((*(data3++) - _min)*mm)>>3);
                ptrd+=2;
              } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)((*(data2++) - _min)*mm)>>2;
                ptrd[0] = (G<<5) | ((unsigned char)((*(data3++) - _min)*mm)>>3);
                ptrd[1] = ((unsigned char)((*(data1++) - _min)*mm)&M) | (G>>3);
                ptrd+=2;
              }
          }
          if (ndata!=_data) {
            _render_resize(ndata,img._width,img._height,(unsigned short*)_data,_width,_height);
            delete[] ndata;
          }
        } break;
        default : { // 24 bits colors, with normalization
          unsigned int *const ndata = (img._width==_width && img._height==_height)?(unsigned int*)_data:
            new unsigned int[(size_t)img._width*img._height];
          if (sizeof(int)==4) { // 32 bits int uses optimized version
            unsigned int *ptrd = ndata;
            switch (img._spectrum) {
            case 1 :
              if (X11_attr.byte_order==cimg::endianness())
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  const unsigned char val = (unsigned char)((*(data1++) - _min)*mm);
                  *(ptrd++) = (val<<16) | (val<<8) | val;
                }
              else
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  const unsigned char val = (unsigned char)((*(data1++) - _min)*mm);
                  *(ptrd++) = (val<<24) | (val<<16) | (val<<8);
                }
              break;
            case 2 :
              if (X11_attr.byte_order==cimg::endianness())
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) =
                    ((unsigned char)((*(data1++) - _min)*mm)<<16) |
                    ((unsigned char)((*(data2++) - _min)*mm)<<8);
              else
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) =
                    ((unsigned char)((*(data2++) - _min)*mm)<<16) |
                    ((unsigned char)((*(data1++) - _min)*mm)<<8);
              break;
            default :
              if (X11_attr.byte_order==cimg::endianness())
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) =
                    ((unsigned char)((*(data1++) - _min)*mm)<<16) |
                    ((unsigned char)((*(data2++) - _min)*mm)<<8) |
                    (unsigned char)((*(data3++) - _min)*mm);
              else
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) =
                    ((unsigned char)((*(data3++) - _min)*mm)<<24) |
                    ((unsigned char)((*(data2++) - _min)*mm)<<16) |
                    ((unsigned char)((*(data1++) - _min)*mm)<<8);
            }
          } else {
            unsigned char *ptrd = (unsigned char*)ndata;
            switch (img._spectrum) {
            case 1 :
              if (X11_attr.byte_order)
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  const unsigned char val = (unsigned char)((*(data1++) - _min)*mm);
                  ptrd[0] = 0;
                  ptrd[1] = val;
                  ptrd[2] = val;
                  ptrd[3] = val;
                  ptrd+=4;
                } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  const unsigned char val = (unsigned char)((*(data1++) - _min)*mm);
                  ptrd[0] = val;
                  ptrd[1] = val;
                  ptrd[2] = val;
                  ptrd[3] = 0;
                  ptrd+=4;
                }
              break;
            case 2 :
              if (X11_attr.byte_order) cimg::swap(data1,data2);
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                ptrd[0] = 0;
                ptrd[1] = (unsigned char)((*(data2++) - _min)*mm);
                ptrd[2] = (unsigned char)((*(data1++) - _min)*mm);
                ptrd[3] = 0;
                ptrd+=4;
              }
              break;
            default :
              if (X11_attr.byte_order)
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  ptrd[0] = 0;
                  ptrd[1] = (unsigned char)((*(data1++) - _min)*mm);
                  ptrd[2] = (unsigned char)((*(data2++) - _min)*mm);
                  ptrd[3] = (unsigned char)((*(data3++) - _min)*mm);
                  ptrd+=4;
                } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  ptrd[0] = (unsigned char)((*(data3++) - _min)*mm);
                  ptrd[1] = (unsigned char)((*(data2++) - _min)*mm);
                  ptrd[2] = (unsigned char)((*(data1++) - _min)*mm);
                  ptrd[3] = 0;
                  ptrd+=4;
                }
            }
          }
          if (ndata!=_data) {
            _render_resize(ndata,img._width,img._height,(unsigned int*)_data,_width,_height);
            delete[] ndata;
          }
        }
        }
      }
      X11_attr.unlock();
      return *this;
    }

    template<typename T>
    static void screenshot(const int x0, const int y0, const int x1, const int y1, CImg<T>& img) {
      img.assign();
      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      Display *dpy = X11_attr.display;
      X11_attr.lock();
      if (!dpy) {
        dpy = XOpenDisplay(0);
        if (!dpy)
          throw CImgDisplayException("CImgDisplay::screenshot(): Failed to open X11 display.");
      }
      Window root = DefaultRootWindow(dpy);
      XWindowAttributes gwa;
      XGetWindowAttributes(dpy,root,&gwa);
      const int width = gwa.width, height = gwa.height;
      int _x0 = x0, _y0 = y0, _x1 = x1, _y1 = y1;
      if (_x0>_x1) cimg::swap(_x0,_x1);
      if (_y0>_y1) cimg::swap(_y0,_y1);

      XImage *image = 0;
      if (_x1>=0 && _x0<width && _y1>=0 && _y0<height) {
        _x0 = std::max(_x0,0);
        _y0 = std::max(_y0,0);
        _x1 = std::min(_x1,width - 1);
        _y1 = std::min(_y1,height - 1);
        image = XGetImage(dpy,root,_x0,_y0,_x1 - _x0 + 1,_y1 - _y0 + 1,AllPlanes,ZPixmap);

        if (image) {
          const unsigned long
            red_mask = image->red_mask,
            green_mask = image->green_mask,
            blue_mask = image->blue_mask;
          img.assign(image->width,image->height,1,3);
          T *pR = img.data(0,0,0,0), *pG = img.data(0,0,0,1), *pB = img.data(0,0,0,2);
          cimg_forXY(img,x,y) {
            const unsigned long pixel = XGetPixel(image,x,y);
            *(pR++) = (T)((pixel & red_mask)>>16);
            *(pG++) = (T)((pixel & green_mask)>>8);
            *(pB++) = (T)(pixel & blue_mask);
          }
          XDestroyImage(image);
        }
      }
      if (!X11_attr.display) XCloseDisplay(dpy);
      X11_attr.unlock();
      if (img.is_empty())
        throw CImgDisplayException("CImgDisplay::screenshot(): Failed to take screenshot "
                                   "with coordinates (%d,%d)-(%d,%d).",
                                   x0,y0,x1,y1);
    }

    template<typename T>
    const CImgDisplay& snapshot(CImg<T>& img) const {
      if (is_empty()) { img.assign(); return *this; }
      const unsigned char *ptrs = (unsigned char*)_data;
      img.assign(_width,_height,1,3);
      T
        *data1 = img.data(0,0,0,0),
        *data2 = img.data(0,0,0,1),
        *data3 = img.data(0,0,0,2);

      cimg::X11_attr &X11_attr = cimg::X11_attr::ref();
      if (X11_attr.is_blue_first) cimg::swap(data1,data3);
      switch (X11_attr.nb_bits) {
      case 8 : {
        for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
          const unsigned char val = *(ptrs++);
          *(data1++) = (T)(val&0xe0);
          *(data2++) = (T)((val&0x1c)<<3);
          *(data3++) = (T)(val<<6);
        }
      } break;
      case 16 : {
        if (X11_attr.byte_order) for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
          const unsigned char
            val0 = ptrs[0],
            val1 = ptrs[1];
          ptrs+=2;
          *(data1++) = (T)(val0&0xf8);
          *(data2++) = (T)((val0<<5) | ((val1&0xe0)>>5));
          *(data3++) = (T)(val1<<3);
          } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
          const unsigned short
            val0 = ptrs[0],
            val1 = ptrs[1];
          ptrs+=2;
          *(data1++) = (T)(val1&0xf8);
          *(data2++) = (T)((val1<<5) | ((val0&0xe0)>>5));
          *(data3++) = (T)(val0<<3);
        }
      } break;
      default : {
        if (X11_attr.byte_order) for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
          ++ptrs;
          *(data1++) = (T)ptrs[0];
          *(data2++) = (T)ptrs[1];
          *(data3++) = (T)ptrs[2];
          ptrs+=3;
          } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
            *(data3++) = (T)ptrs[0];
            *(data2++) = (T)ptrs[1];
            *(data1++) = (T)ptrs[2];
            ptrs+=3;
            ++ptrs;
          }
      }
      }
      return *this;
    }

    // GDI32-based implementation.
    //----------------------------
#endif
#endif
