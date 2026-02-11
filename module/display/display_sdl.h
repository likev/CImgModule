#ifndef CIMG_MODULE_DISPLAY_SDL_H
#define CIMG_MODULE_DISPLAY_SDL_H

#if cimg_display==3

    SDL_Window *_window;
    SDL_Renderer *_renderer;
    SDL_Texture *_texture;
    SDL_ThreadID _thread_id; // ID of the thread that created this display
    unsigned int *_data;
    bool _is_cursor_visible, _paint_request;

    // Internal functions.
    void _assign(const unsigned int dimw, const unsigned int dimh, const char *const p_title=0,
                 const unsigned int normalization_type=3,
                 const bool is_fullscreen=false, const bool closed_flag=false) {
      if (!_keys) _keys = new unsigned int[128];
      if (!_released_keys) _released_keys = new unsigned int[128];

      // Destroy previous display window if existing.
      if (!is_empty()) assign();

      cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();
      SDL3_attr.lock();

      // Allocate space for window title.
      const char *const np_title = p_title?p_title:"";
      const unsigned int s = (unsigned int)std::strlen(np_title) + 1;
      char *const tmp_title = s?new char[s]:0;
      if (s) std::memcpy(tmp_title,np_title,s*sizeof(char));

      // Set display variables.
      _width = std::min(dimw,(unsigned int)screen_width());
      _height = std::min(dimh,(unsigned int)screen_height());
      _normalization = normalization_type<4?normalization_type:3;
      _is_fullscreen = is_fullscreen;
      _window_x = _window_y = cimg::type<int>::min();
      _is_closed = closed_flag;
      _is_cursor_visible = true;
      _paint_request = false;
      _title = tmp_title;
      _thread_id = SDL_GetCurrentThreadID();
      flush();

      // Create window and renderer.
      if (!SDL_CreateWindowAndRenderer(_title,(int)_width,(int)_height,
                                       SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS |
                                       (_is_fullscreen?SDL_WINDOW_FULLSCREEN:0) |
                                       (_is_closed?SDL_WINDOW_HIDDEN:0),
                                       &_window,&_renderer)) {
        SDL3_attr.unlock();
        throw CImgDisplayException("CImgDisplay::assign(): %s",SDL_GetError());
      }
      SDL_RaiseWindow(_window);
      SDL_SetRenderDrawColor(_renderer,0,0,0,255);
      if (!_is_fullscreen)
        SDL_SetWindowPosition(_window,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED);
      _window_width = _width;
      _window_height = _height;
      _update_window_pos();

      // Create texture.
      _texture = SDL_CreateTexture(_renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,
                                   (int)_width,(int)_height);
      _data = new unsigned int[_width*_height];

      // Add to managed list of CImgDisplays.
      if (SDL3_attr.nb_cimg_displays>=512) {
        SDL3_attr.unlock();
        throw CImgDisplayException("CImgDisplay::assign(): Max number of displays (512) already opened.");
      }
      SDL3_attr.cimg_displays[SDL3_attr.nb_cimg_displays++] = this;
      SDL3_attr.unlock();
      paint();
    }

    void _process_event(const SDL_Event &event) {
      bool is_event = false;
      switch (event.type) {

        // Window events.
      case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        _is_closed = is_event = true;
        break;
      case SDL_EVENT_WINDOW_RESIZED: {
        int w,h;
        SDL_GetWindowSize(_window,&w,&h);
        _window_width = (unsigned int)w;
        _window_height = (unsigned int)h;
        _is_resized = _paint_request = is_event = true;
      } break;
      case SDL_EVENT_WINDOW_MOVED:
        _update_window_pos();
        _paint_request = is_event = true;
        break;
      case SDL_EVENT_WINDOW_FOCUS_GAINED:
      case SDL_EVENT_WINDOW_MINIMIZED:
      case SDL_EVENT_WINDOW_MAXIMIZED:
      case SDL_EVENT_WINDOW_RESTORED:
        _paint_request = is_event = true;
        break;
      case SDL_EVENT_WINDOW_OCCLUDED:
        _paint_request = true;
        break;

        // Mouse events.
      case SDL_EVENT_MOUSE_MOTION:
      case SDL_EVENT_WINDOW_MOUSE_ENTER: {
        float x,y;
        SDL_GetMouseState(&x,&y);
        if (x<0 || x>=_width || y<0 || y>=_height) {
          _mouse_x = _mouse_y = -1;
          SDL_ShowCursor();
        } else {
          _mouse_x = (int)x;
          _mouse_y = (int)y;
          if (_is_cursor_visible) SDL_ShowCursor(); else SDL_HideCursor();
        }
        is_event = true;
      } break;
      case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        _mouse_x = _mouse_y = -1;
        SDL_ShowCursor();
        is_event = true;
        break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
      case SDL_EVENT_MOUSE_BUTTON_UP: {
        SDL_MouseButtonFlags button = SDL_GetMouseState(0,0);
        _button = (button&1) | ((button&4)>>2)<<1 | ((button&2)>>1)<<2;
        is_event = true;
      } break;
      case SDL_EVENT_MOUSE_WHEEL:
        set_wheel((int)event.wheel.y);
        break;

        // Keyboard events.
      case SDL_EVENT_KEY_DOWN:
      case SDL_EVENT_KEY_UP:
        set_key((unsigned int)event.key.scancode,event.type==SDL_EVENT_KEY_DOWN);
        break;
      }
      if (is_event) { _is_event = true; }
    }

    CImgDisplay& _update_window_pos() {
      if (_is_closed) _window_x = _window_y = cimg::type<int>::min();
      else SDL_GetWindowPosition(_window,&_window_x,&_window_y);
      return *this;
    }

    // Public functions.
    static int screen_width() {
      cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();
      return SDL3_attr.mode->w;
    }

    static int screen_height() {
      cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();
      return SDL3_attr.mode->h;
    }

    static void wait_all() {
      process_events(true);
    }

    CImgDisplay& wait(const unsigned int milliseconds) {
      cimg::wait(milliseconds,&_timer);
      process_events(false);
      return *this;
    }

    // Process all events in event queue.
    static void process_events(bool wait_event) {
      cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();
      const SDL_ThreadID current_thread_id = SDL_GetCurrentThreadID();

      // Re-paint display windows from current thread if necessary.
      for (unsigned int k = 0; k<SDL3_attr.nb_cimg_displays; ++k) {
        CImgDisplay &disp = *SDL3_attr.cimg_displays[k];
        if (!disp.is_closed() && disp._paint_request && disp._thread_id==current_thread_id)
          SDL3_attr.cimg_displays[k]->paint();
      }

      // Make sure only the main thread process events.
      if (current_thread_id!=SDL3_attr.main_thread_id) {
        if (wait_event) cimg::sleep(8);
        return;
      }
      if (!wait_event) SDL3_attr.lock();

      // Dispatch global events to managed CImgDisplay instances.
      SDL_Event event;
      bool is_event, _wait_event = wait_event;
      do {
        is_event = _wait_event?SDL_WaitEvent(&event):SDL_PollEvent(&event);
        if (is_event) {
          if (event.type==SDL_EVENT_QUIT) {
            for (unsigned int k = 0; k<SDL3_attr.nb_cimg_displays; ++k) {
              CImgDisplay &disp = *SDL3_attr.cimg_displays[k];
              disp._is_closed = disp._is_event = true;
            }
          } else {
            SDL_Window *const window = SDL_GetWindowFromID(event.window.windowID);
            if (window) // Find CImgDisplay associated to event
              for (unsigned int k = 0; k<SDL3_attr.nb_cimg_displays; ++k) {
                CImgDisplay &disp = *SDL3_attr.cimg_displays[k];
                if (window==disp._window) {
                  if (!disp._is_closed) disp._process_event(event);
                  break;
                }
              }
          }
        }
        _wait_event = false;
      } while (is_event);

      if (!wait_event) SDL3_attr.unlock();
    }

    CImgDisplay& assign() {
      if (!_keys) _keys = new unsigned int[128];
      if (!_released_keys) _released_keys = new unsigned int[128];
      if (is_empty()) return flush();
      cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();
      SDL3_attr.lock();

      // Remove display window from event thread list.
      unsigned int i;
      for (i = 0; i<SDL3_attr.nb_cimg_displays && SDL3_attr.cimg_displays[i]!=this; ++i) {}
      for ( ; i<SDL3_attr.nb_cimg_displays - 1; ++i)
        SDL3_attr.cimg_displays[i] = SDL3_attr.cimg_displays[i + 1];
      --SDL3_attr.nb_cimg_displays;

      // Destroy associated ressources.
      SDL_DestroyRenderer(_renderer);
      SDL_DestroyWindow(_window);
      SDL_DestroyTexture(_texture);
      delete _data;
      _data = 0;
      _renderer = 0;
      _window = 0;
      _texture = 0;
      _thread_id = 0;
      _width = _height = _normalization = _window_width = _window_height = 0;
      _window_x = _window_y = cimg::type<int>::min();
      _is_fullscreen = false;
      _is_closed = true;
      _min = _max = 0;
      _is_cursor_visible = true;
      _paint_request = false;
      _title = 0;
      flush();
      SDL3_attr.unlock();
      return *this;
    }

    CImgDisplay& assign(const unsigned int dimw, const unsigned int dimh, const char *const title=0,
                        const unsigned int normalization_type=3,
                        const bool is_fullscreen=false, const bool closed_flag=false) {
      if (!dimw || !dimh) return assign();
      _assign(dimw,dimh,title,normalization_type,is_fullscreen,closed_flag);
      _min = _max = 0;
      std::memset(_data,0,sizeof(unsigned int)*_width*_height);
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
      return display(nimg);
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
      return display(nimg);
    }

    CImgDisplay& assign(const CImgDisplay& disp) {
      if (!disp) return assign();
      _assign(disp._width,disp._height,disp._title,disp._normalization,disp._is_fullscreen,disp._is_closed);
      std::memcpy(_data,disp._data,sizeof(unsigned int)*_width*_height);
      return paint();
    }

    CImgDisplay& move(const int posx, const int posy) {
      if (is_empty()) return *this;
      if (_window_x!=posx || _window_y!=posy) {
        cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();
        SDL3_attr.lock();
        SDL_SetWindowPosition(_window,posx,posy);
        _window_x = posx;
        _window_y = posy;
        SDL3_attr.unlock();
      }
      show();
      _is_moved = false;
      return *this;
    }

    CImgDisplay& resize(const int nwidth, const int nheight, const bool force_redraw=true) {
      if (!nwidth || !nheight || (is_empty() && (nwidth<0 || nheight<0))) return assign();
      if (is_empty()) return assign((unsigned int)nwidth,(unsigned int)nheight);
      cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();
      SDL3_attr.lock();
      const unsigned int
        tmpdimx = (nwidth>0)?nwidth:(-nwidth*_width/100),
        tmpdimy = (nheight>0)?nheight:(-nheight*_height/100),
        dimx = tmpdimx?tmpdimx:1,
        dimy = tmpdimy?tmpdimy:1;
      if (_width!=dimx || _height!=dimy || _window_width!=dimx || _window_height!=dimy) {
        if (_window_width!=dimx || _window_height!=dimy) {
          SDL_SetWindowSize(_window,(int)dimx,(int)dimy);
        }
        SDL3_attr.unlock();
        show();
        SDL3_attr.lock();
        unsigned int *const ndata = new unsigned int[dimx*dimy];
        if (force_redraw) _render_resize(_data,_width,_height,ndata,dimx,dimy);
        else std::memset(ndata,0,sizeof(unsigned int)*dimx*dimy);
        delete[] _data;
        _data = ndata;
        _window_width = _width = dimx;
        _window_height = _height = dimy;
      }
      _is_resized = false;
      SDL3_attr.unlock();
      if (_is_fullscreen) move((screen_width() - width())/2,(screen_height() - height())/2);
      if (force_redraw) paint();
      return *this;
    }

    CImgDisplay& toggle_fullscreen(const bool force_redraw=true) {
      if (is_empty()) return *this;
      if (!force_redraw) return assign(_width,_height,_title,_normalization,!_is_fullscreen,false);
      cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();
      SDL3_attr.lock();
      const cimg_ulong buf_size = (cimg_ulong)_width*_height*sizeof(unsigned int);
      unsigned int *odata = new unsigned int[_width*_height];
      std::memcpy(odata,_data,buf_size);
      SDL3_attr.unlock();
      assign(_width,_height,_title,_normalization,!_is_fullscreen,false);
      cimg::swap(_data,odata);
      delete[] odata;
      return paint();
    }

    CImgDisplay& show() {
      if (is_empty() || !_is_closed) return *this;
      cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();
      SDL3_attr.lock();
      _is_closed = false;
      if (_is_fullscreen) SDL_SetWindowFullscreen(_window,true);
      SDL_ShowWindow(_window);
      _update_window_pos();
      SDL3_attr.unlock();
      return paint();
    }

    CImgDisplay& close() {
      if (is_empty() || _is_closed) return *this;
      cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();
      SDL3_attr.lock();
      _is_closed = true;
      if (_is_fullscreen) SDL_SetWindowFullscreen(_window,false);
      SDL_HideWindow(_window);
      _window_x = _window_y = cimg::type<int>::min();
      SDL3_attr.unlock();
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

      cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();
      SDL3_attr.lock();
      delete[] _title;
      const unsigned int s = (unsigned int)std::strlen(tmp) + 1;
      _title = new char[s];
      std::memcpy(_title,tmp,s*sizeof(char));
      SDL_SetWindowTitle(_window,tmp);
      delete[] tmp;
      SDL3_attr.unlock();
      return *this;
    }

    CImgDisplay& show_mouse() {
      if (is_empty()) return *this;
      cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();
      SDL3_attr.lock();
      SDL_ShowCursor();
      _is_cursor_visible = true;
      SDL3_attr.unlock();
      return *this;
    }

    CImgDisplay& hide_mouse() {
      if (is_empty()) return *this;
      cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();
      SDL3_attr.lock();
      SDL_HideCursor();
      _is_cursor_visible = false;
      SDL3_attr.unlock();
      return *this;
    }

    CImgDisplay& set_mouse(const int posx, const int posy) {
      if (is_empty() || _is_closed || posx<0 || posy<0) return *this;
      if (!_is_closed) {
        cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();
        SDL3_attr.lock();
        SDL_WarpMouseInWindow(_window,(float)posx,(float)posy);
        _mouse_x = posx; _mouse_y = posy;
        SDL3_attr.unlock();
      }
      return *this;
    }

    template<typename T>
    CImgDisplay& display(const CImg<T>& img) {
      if (!img)
        throw CImgArgumentException(_cimgdisplay_instance
                                    "display(): Empty specified image.",
                                    cimgdisplay_instance);
      if (is_empty()) return assign(img);
      return render(img).paint();
    }

    CImgDisplay& paint() {
      if (_is_closed) return *this;
      cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();

      const SDL_ThreadID current_thread_id = SDL_GetCurrentThreadID();
      if (current_thread_id!=_thread_id) {
        // Make sure repaint is done by the thread that created the window.
        _paint_request = true;
        return *this;
      }

      SDL3_attr.lock();
      if (!_texture || _texture->w!=(int)_width || _texture->h!=(int)_height) {
        if (_texture) SDL_DestroyTexture(_texture);
        _texture = SDL_CreateTexture(_renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,
                                     (int)_width,(int)_height);
      }
      SDL_UpdateTexture(_texture,0,_data,_width*sizeof(unsigned int));
      SDL_RenderClear(_renderer);
      SDL_FRect rect;
      rect.x = rect.y = 0;
      rect.w = (float)_width;
      rect.h = (float)_height;
      SDL_RenderTexture(_renderer,_texture,0,&rect);
      SDL_RenderPresent(_renderer);
      _paint_request = false;
      SDL3_attr.unlock();
      return *this;
    }

    template<typename T>
    CImgDisplay& render(const CImg<T>& img) {
      if (!img)
        throw CImgArgumentException(_cimgdisplay_instance
                                    "render(): Empty specified image.",
                                    cimgdisplay_instance);

      if (is_empty()) return *this;
      if (img._depth!=1) return render(img.get_projections2d((img._width - 1)/2,(img._height - 1)/2,
                                                             (img._depth - 1)/2));
      cimg::SDL3_attr &SDL3_attr = cimg::SDL3_attr::ref();
      SDL3_attr.lock();
      const T
        *data1 = img._data,
        *data2 = (img._spectrum>=2)?img.data(0,0,0,1):data1,
        *data3 = (img._spectrum>=3)?img.data(0,0,0,2):data1;

      unsigned int
        *const ndata = (img._width==_width && img._height==_height)?_data:
        new unsigned int[(size_t)img._width*img._height],
        *ptrd = ndata;

      if (!_normalization || (_normalization==3 && cimg::type<T>::string()==cimg::type<unsigned char>::string())) {
        _min = _max = 0;
        switch (img._spectrum) {
        case 1 : {
          for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
            const unsigned char val = (unsigned char)*(data1++);
            *(ptrd++) = (unsigned int)((val<<24) | (val<<16) | (val<<8) | 255);
          }
        } break;
        case 2 : {
          for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
            const unsigned char
              R = (unsigned char)*(data1++),
              G = (unsigned char)*(data2++);
            *(ptrd++) = (unsigned int)((R<<24) | (G<<16) | 255);
          }
        } break;
        default : {
          for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
            const unsigned char
              R = (unsigned char)*(data1++),
              G = (unsigned char)*(data2++),
              B = (unsigned char)*(data3++);
            *(ptrd++) = (unsigned int)((R<<24) | (G<<16) | (B<<8) | 255);
          }
        }
        }
      } else {
        if (_normalization==3) {
          if (cimg::type<T>::is_float()) _min = (float)img.min_max(_max);
          else {
            _min = (float)cimg::type<T>::min();
            _max = (float)cimg::type<T>::max();
          }
        } else if ((_min>_max) || _normalization==1) _min = (float)img.min_max(_max);
        const float delta = _max - _min, mm = 255/(delta?delta:1.f);
        switch (img._spectrum) {
        case 1 : {
          for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
            const unsigned char val = (unsigned char)((*(data1++) - _min)*mm);
            *(ptrd++) = (unsigned int)((val<<24) | (val<<16) | (val<<8) | 255);
          }
        } break;
        case 2 : {
          for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
            const unsigned char
              R = (unsigned char)((*(data1++) - _min)*mm),
              G = (unsigned char)((*(data2++) - _min)*mm);
            *(ptrd++) = (unsigned int)((R<<24) | (G<<16) | 255);
          }
        } break;
        default : {
          for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
            const unsigned char
              R = (unsigned char)((*(data1++) - _min)*mm),
              G = (unsigned char)((*(data2++) - _min)*mm),
              B = (unsigned char)((*(data3++) - _min)*mm);
            *(ptrd++) = (unsigned int)((R<<24) | (G<<16) | (B<<8) | 255);
          }
        }
        }
      }
      if (ndata!=_data) { _render_resize(ndata,img._width,img._height,_data,_width,_height); delete[] ndata; }
      SDL3_attr.unlock();

//      process_events(false);
      return *this;
    }

    template<typename T>
    static void screenshot(const int, const int, const int, const int, CImg<T>&) {
      throw CImgDisplayException("screenshot(): Screenshot feature is not supported when using SDL3-based display.");
    }

    template<typename T>
    const CImgDisplay& snapshot(CImg<T>& img) const {
      if (is_empty()) { img.assign(); return *this; }
      const unsigned int *ptrs = _data;
      img.assign(_width,_height,1,3);
      T
        *data1 = img.data(0,0,0,0),
        *data2 = img.data(0,0,0,1),
        *data3 = img.data(0,0,0,2);
      for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
        const unsigned int val = *(ptrs++);
        *(data1++) = (T)(unsigned char)(val>>24);
        *(data2++) = (T)(unsigned char)((val>>16)&0xFF);
        *(data3++) = (T)(unsigned char)((val>>8)&0xFF);
      }
      return *this;
    }

#endif

    //@}
  }; // struct CImgDisplay { ...

#endif
