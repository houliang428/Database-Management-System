//
//  View.hpp
//  Database3


#ifndef View_h
#define View_h

#include <iostream>

namespace ECE141 {

  //completely generic view, which you will subclass to show information
  class View {
  public:
    virtual         ~View() {}
    virtual bool    show(std::ostream &aStream)=0;
  };

}

#endif /* View_h */
