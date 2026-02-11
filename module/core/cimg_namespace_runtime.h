#ifndef CIMG_MODULE_CORE_CIMG_NAMESPACE_RUNTIME_H
#define CIMG_MODULE_CORE_CIMG_NAMESPACE_RUNTIME_H

    // Implement a tic/toc mechanism to display elapsed time of algorithms.
    inline cimg_uint64 tictoc(const bool is_tic) {
      cimg::mutex(2);
      static CImg<cimg_uint64> times(64);
      static unsigned int pos = 0;
      const cimg_uint64 t1 = cimg::time();
      if (is_tic) {
        // Tic.
        times[pos++] = t1;
        if (pos>=times._width)
          throw CImgArgumentException("cimg::tic(): Too much calls to 'cimg::tic()' without calls to 'cimg::toc()'.");
        cimg::mutex(2,0);
        return t1;
      }

      // Toc.
      if (!pos)
        throw CImgArgumentException("cimg::toc(): No previous call to 'cimg::tic()' has been made.");
      const cimg_uint64
        t0 = times[--pos],
        dt = t1>=t0?(t1 - t0):cimg::type<cimg_uint64>::max();
      const unsigned int
        edays = (unsigned int)(dt/86400000.),
        ehours = (unsigned int)((dt - edays*86400000.)/3600000.),
        emin = (unsigned int)((dt - edays*86400000. - ehours*3600000.)/60000.),
        esec = (unsigned int)((dt - edays*86400000. - ehours*3600000. - emin*60000.)/1000.),
        ems = (unsigned int)(dt - edays*86400000. - ehours*3600000. - emin*60000. - esec*1000.);
      if (!edays && !ehours && !emin && !esec)
        std::fprintf(cimg::output(),"%s[CImg]%*sElapsed time: %u ms%s\n",
                     cimg::t_red,1 + 2*pos,"",ems,cimg::t_normal);
      else {
        if (!edays && !ehours && !emin)
          std::fprintf(cimg::output(),"%s[CImg]%*sElapsed time: %u sec %u ms%s\n",
                       cimg::t_red,1 + 2*pos,"",esec,ems,cimg::t_normal);
        else {
          if (!edays && !ehours)
            std::fprintf(cimg::output(),"%s[CImg]%*sElapsed time: %u min %u sec %u ms%s\n",
                         cimg::t_red,1 + 2*pos,"",emin,esec,ems,cimg::t_normal);
          else{
            if (!edays)
              std::fprintf(cimg::output(),"%s[CImg]%*sElapsed time: %u hours %u min %u sec %u ms%s\n",
                           cimg::t_red,1 + 2*pos,"",ehours,emin,esec,ems,cimg::t_normal);
            else{
              std::fprintf(cimg::output(),"%s[CImg]%*sElapsed time: %u days %u hours %u min %u sec %u ms%s\n",
                           cimg::t_red,1 + 2*pos,"",edays,ehours,emin,esec,ems,cimg::t_normal);
            }
          }
        }
      }
      cimg::mutex(2,0);
      return dt;
    }

    // Return a temporary string describing the size of a memory buffer.
    inline const char *strbuffersize(const cimg_ulong size) {
      static CImg<char> res(256);
      cimg::mutex(5);
      if (size<1024LU) cimg_snprintf(res,res._width,"%lu byte%s",(unsigned long)size,size>1?"s":"");
      else if (size<1024*1024LU) { const float nsize = size/1024.f; cimg_snprintf(res,res._width,"%.1f Kio",nsize); }
      else if (size<1024*1024*1024LU) {
        const float nsize = size/(1024*1024.f); cimg_snprintf(res,res._width,"%.1f Mio",nsize);
      } else { const float nsize = size/(1024*1024*1024.f); cimg_snprintf(res,res._width,"%.1f Gio",nsize); }
      cimg::mutex(5,0);
      return res;
    }

    //! Display a simple dialog box, and wait for the user's response.
    /**
       \param title Title of the dialog window.
       \param msg Main message displayed inside the dialog window.
       \param button1_label Label of the 1st button.
       \param button2_label Label of the 2nd button (\c 0 to hide button).
       \param button3_label Label of the 3rd button (\c 0 to hide button).
       \param button4_label Label of the 4th button (\c 0 to hide button).
       \param button5_label Label of the 5th button (\c 0 to hide button).
       \param button6_label Label of the 6th button (\c 0 to hide button).
       \param logo Image logo displayed at the left of the main message.
       \param is_centered Tells if the dialog window must be centered on the screen.
       \return Index of clicked button (from \c 0 to \c 5), or \c -1 if the dialog window has been closed by the user.
       \note
       - Up to 6 buttons can be defined in the dialog window.
       - The function returns when a user clicked one of the button or closed the dialog window.
       - If a button text is set to 0, the corresponding button (and the following) will not appear in the dialog box.
       At least one button must be specified.
    **/
    template<typename t>
    inline int dialog(const char *const title, const char *const msg,
                      const char *const button1_label, const char *const button2_label,
                      const char *const button3_label, const char *const button4_label,
                      const char *const button5_label, const char *const button6_label,
                      const CImg<t>& logo, const bool is_centered=false) {
#if cimg_display==0
      cimg::unused(title,msg,button1_label,button2_label,button3_label,button4_label,button5_label,button6_label,
                   logo._data,is_centered);
      throw CImgIOException("cimg::dialog(): No display available.");
#else
      static const unsigned char
        black[] = { 0,0,0 }, white[] = { 255,255,255 }, gray[] = { 200,200,200 }, gray2[] = { 150,150,150 };

      // Create buttons and canvas graphics.
      CImgList<unsigned char> buttons, cbuttons, sbuttons;
      if (button1_label) {
        CImg<unsigned char>().draw_text(0,0,button1_label,black,gray,1,13).move_to(buttons);
        if (button2_label) {
          CImg<unsigned char>().draw_text(0,0,button2_label,black,gray,1,13).move_to(buttons);
          if (button3_label) {
            CImg<unsigned char>().draw_text(0,0,button3_label,black,gray,1,13).move_to(buttons);
            if (button4_label) {
              CImg<unsigned char>().draw_text(0,0,button4_label,black,gray,1,13).move_to(buttons);
              if (button5_label) {
                CImg<unsigned char>().draw_text(0,0,button5_label,black,gray,1,13).move_to(buttons);
                if (button6_label) {
                  CImg<unsigned char>().draw_text(0,0,button6_label,black,gray,1,13).move_to(buttons);
                }}}}}}
      if (!buttons._width)
        throw CImgArgumentException("cimg::dialog(): No buttons have been defined.");
      cimglist_for(buttons,l) buttons[l].resize(-100,-100,1,3);

      unsigned int bw = 0, bh = 0;
      cimglist_for(buttons,l) { bw = std::max(bw,buttons[l]._width); bh = std::max(bh,buttons[l]._height); }
      bw+=8; bh+=8;
      if (bw<64) bw = 64;
      if (bw>128) bw = 128;
      if (bh<24) bh = 24;
      if (bh>48) bh = 48;

      CImg<unsigned char> button(bw,bh,1,3);
      button.draw_rectangle(0,0,bw - 1,bh - 1,gray);
      button.draw_line(0,0,bw - 1,0,white).draw_line(0,bh - 1,0,0,white);
      button.draw_line(bw - 1,0,bw - 1,bh - 1,black).draw_line(bw - 1,bh - 1,0,bh - 1,black);
      button.draw_line(1,bh - 2,bw - 2,bh - 2,gray2).draw_line(bw - 2,bh - 2,bw - 2,1,gray2);
      CImg<unsigned char> sbutton(bw,bh,1,3);
      sbutton.draw_rectangle(0,0,bw - 1,bh - 1,gray);
      sbutton.draw_line(0,0,bw - 1,0,black).draw_line(bw - 1,0,bw - 1,bh - 1,black);
      sbutton.draw_line(bw - 1,bh - 1,0,bh - 1,black).draw_line(0,bh - 1,0,0,black);
      sbutton.draw_line(1,1,bw - 2,1,white).draw_line(1,bh - 2,1,1,white);
      sbutton.draw_line(bw - 2,1,bw - 2,bh - 2,black).draw_line(bw - 2,bh - 2,1,bh - 2,black);
      sbutton.draw_line(2,bh - 3,bw - 3,bh - 3,gray2).draw_line(bw - 3,bh - 3,bw - 3,2,gray2);
      sbutton.draw_line(4,4,bw - 5,4,black,1,0xAAAAAAAA,true).
        draw_line(bw - 5,4,bw - 5,bh - 5,black,1,0xAAAAAAAA,false);
      sbutton.draw_line(bw - 5,bh - 5,4,bh - 5,black,1,0xAAAAAAAA,false).
        draw_line(4,bh - 5,4,4,black,1,0xAAAAAAAA,false);
      CImg<unsigned char> cbutton(bw,bh,1,3);
      cbutton.draw_rectangle(0,0,bw - 1,bh - 1,black).draw_rectangle(1,1,bw - 2,bh - 2,gray2).
        draw_rectangle(2,2,bw - 3,bh - 3,gray);
      cbutton.draw_line(4,4,bw - 5,4,black,1,0xAAAAAAAA,true).
        draw_line(bw - 5,4,bw - 5,bh - 5,black,1,0xAAAAAAAA,false);
      cbutton.draw_line(bw - 5,bh - 5,4,bh - 5,black,1,0xAAAAAAAA,false).
        draw_line(4,bh - 5,4,4,black,1,0xAAAAAAAA,false);

      cimglist_for(buttons,ll) {
        CImg<unsigned char>(cbutton).
          draw_image(1 + (bw  -buttons[ll].width())/2,1 + (bh - buttons[ll].height())/2,buttons[ll]).
          move_to(cbuttons);
        CImg<unsigned char>(sbutton).
          draw_image((bw - buttons[ll].width())/2,(bh - buttons[ll].height())/2,buttons[ll]).
          move_to(sbuttons);
        CImg<unsigned char>(button).
          draw_image((bw - buttons[ll].width())/2,(bh - buttons[ll].height())/2,buttons[ll]).
          move_to(buttons[ll]);
      }

      CImg<unsigned char> canvas;
      if (msg)
        ((CImg<unsigned char>().draw_text(0,0,"%s",gray,0,1,13,msg)*=-1)+=200).resize(-100,-100,1,3).move_to(canvas);

      const unsigned int
        bwall = (buttons._width - 1)*(12 + bw) + bw,
        w = cimg::max(196U,36 + logo._width + canvas._width,24 + bwall),
        h = cimg::max(96U,36 + canvas._height + bh,36 + logo._height + bh),
        lx = 12 + (canvas._data?0:((w - 24 - logo._width)/2)),
        ly = (h - 12 - bh - logo._height)/2,
        tx = lx + logo._width + 12,
        ty = (h - 12 - bh - canvas._height)/2,
        bx = (w - bwall)/2,
        by = h - 12 - bh;

      if (canvas._data)
        canvas = CImg<unsigned char>(w,h,1,3).
          draw_rectangle(0,0,w - 1,h - 1,gray).
          draw_line(0,0,w - 1,0,white).draw_line(0,h - 1,0,0,white).
          draw_line(w - 1,0,w - 1,h - 1,black).draw_line(w - 1,h - 1,0,h - 1,black).
          draw_image(tx,ty,canvas);
      else
        canvas = CImg<unsigned char>(w,h,1,3).
          draw_rectangle(0,0,w - 1,h - 1,gray).
          draw_line(0,0,w - 1,0,white).draw_line(0,h - 1,0,0,white).
          draw_line(w - 1,0,w - 1,h - 1,black).draw_line(w - 1,h - 1,0,h - 1,black);
      if (logo._data) canvas.draw_image(lx,ly,logo);

      unsigned int xbuttons[6] = {};
      cimglist_for(buttons,lll) {
        xbuttons[lll] = bx + (bw + 12)*lll;
        canvas.draw_image(xbuttons[lll],by,buttons[lll]);
      }

      // Open window and enter events loop.
      CImgDisplay disp(canvas,title?title:" ",0,false,is_centered?true:false);
      if (is_centered) disp.move((CImgDisplay::screen_width() - disp.width())/2,
                                 (CImgDisplay::screen_height() - disp.height())/2);
      bool stop_flag = false, refresh = false;
      int oselected = -1, oclicked = -1, selected = -1, clicked = -1;
      while (!disp.is_closed() && !stop_flag) {
        if (refresh) {
          if (clicked>=0)
            CImg<unsigned char>(canvas).draw_image(xbuttons[clicked],by,cbuttons[clicked]).display(disp);
          else {
            if (selected>=0)
              CImg<unsigned char>(canvas).draw_image(xbuttons[selected],by,sbuttons[selected]).display(disp);
            else canvas.display(disp);
          }
          refresh = false;
        }
        disp.wait(15);
        if (disp.is_resized()) disp.resize(disp,false);

        if (disp.button()&1)  {
          oclicked = clicked;
          clicked = -1;
          cimglist_for(buttons,l)
            if (disp.mouse_y()>=(int)by && disp.mouse_y()<(int)(by + bh) &&
                disp.mouse_x()>=(int)xbuttons[l] && disp.mouse_x()<(int)(xbuttons[l] + bw)) {
              clicked = selected = l;
              refresh = true;
            }
          if (clicked!=oclicked) refresh = true;
        } else if (clicked>=0) stop_flag = true;

        if (disp.key()) {
          oselected = selected;
          switch (disp.key()) {
          case cimg::keyESC : selected = -1; stop_flag = true; break;
          case cimg::keyRETURN : if (selected<0) selected = 0; stop_flag = true; break;
          case cimg::keyTAB :
          case cimg::keyARROWRIGHT :
          case cimg::keyARROWDOWN : selected = (selected + 1)%buttons.width(); break;
          case cimg::keyARROWLEFT :
          case cimg::keyARROWUP : selected = (selected + buttons.width() - 1)%buttons.width(); break;
          }
          disp.set_key();
          if (selected!=oselected) refresh = true;
        }
      }
      if (!disp) selected = -1;
      return selected;
#endif
    }

    //! Display a simple dialog box, and wait for the user's response \specialization.
    inline int dialog(const char *const title, const char *const msg,
                      const char *const button1_label, const char *const button2_label,
                      const char *const button3_label, const char *const button4_label,
                      const char *const button5_label, const char *const button6_label,
                      const bool is_centered) {
      return dialog(title,msg,button1_label,button2_label,button3_label,button4_label,button5_label,button6_label,
                    CImg<unsigned char>::_logo40x38(),is_centered);
    }

    //! Evaluate math expression.
    /**
       \param expression C-string describing the formula to evaluate.
       \param x Value of the pre-defined variable \c x.
       \param y Value of the pre-defined variable \c y.
       \param z Value of the pre-defined variable \c z.
       \param c Value of the pre-defined variable \c c.
       \return Result of the formula evaluation.
       \note Set \c expression to \c 0 to keep evaluating the last specified \c expression.
       \par Example
       \code
       const double
       res1 = cimg::eval("cos(x)^2 + sin(y)^2",2,2), // will return '1'
       res2 = cimg::eval(0,1,1); // will return '1' too
       \endcode
    **/
    inline double eval(const char *const expression, const double x, const double y, const double z, const double c) {
      static const CImg<float> empty;
      return empty.eval(expression,x,y,z,c);
    }

    template<typename t>
    inline CImg<typename cimg::superset<double,t>::type> eval(const char *const expression, const CImg<t>& xyzc) {
      static const CImg<float> empty;
      return empty.eval(expression,xyzc);
    }

#endif
