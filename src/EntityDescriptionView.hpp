//
//  EntityDescriptionView.hpp
//  Datatabase4


#ifndef EntityDescriptionView_hpp
#define EntityDescriptionView_hpp

#include <stdio.h>
#include <iostream>
#include "View.hpp"

namespace ECE141 {
  
  class Entity;
  class Attribute;
  
  //** USE: This view describes an entity (fields), suitable for console display...
  class EntityDescriptionView : public View {
  public:
        
    EntityDescriptionView(const Entity &anEntity);
    bool  show(std::ostream &anOutput);
    
  protected:
    const Entity  &entity;
    std::ostream  *stream;
  };
  
}

#endif /* EntityDescriptionView_hpp */


