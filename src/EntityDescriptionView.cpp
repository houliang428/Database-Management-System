//
//  EntityDescriptionView.cpp
//  Datatabase4


#include "EntityDescriptionView.hpp"
#include "Helpers.hpp"
#include "Entity.hpp"
#include "Attribute.hpp"
#include <iomanip>

//** USE: This view describes an entity (fields), suitable for console display...

namespace ECE141 {

  EntityDescriptionView::EntityDescriptionView(const Entity &anEntity)
   : entity{anEntity}, stream(nullptr) {}
  

  bool EntityDescriptionView::show(std::ostream &anOutput) {
    stream=&anOutput; //for use in callback...
    
    const char* theSep   ="+-------------------+--------------+------+------+----------------+------------------+";
    const char* theTitle ="| Field             | Type         | Null | Key  | Default        | Extra            |";
    
    anOutput << theSep << std::endl;
    anOutput << theTitle << std::endl;
    anOutput << theSep << std::endl;
    
    size_t fieldC = 18;
    size_t TypeC  = 13;
    size_t NullC  = 5;
    size_t KeyC   = 5;
    size_t DefaultC = 15;
    size_t ExtraC = 17;
    
    //STUDENT: Add your code here to output the list of fields, properly formatted...
    //         Feel free to completely change this class if you have a better approach...
    AttributeList content = entity.getAttributes();
    for(auto attr:content){
      //Name
      anOutput << "| " << attr.getName();
      for(size_t i = attr.getName().size(); i<fieldC; i++)  anOutput << " ";
      
      //Type
      anOutput << "| " << MapforTable[attr.getType()];
      for(size_t i = MapforTable[attr.getType()].size(); i<TypeC; i++)  anOutput << " ";
      
      
      //Check Nullable
      if(attr.isNullable()){
        anOutput << "| Y";
      }else{
        anOutput << "| N";
      }
      for(size_t i = 1; i<NullC; i++)  anOutput << " ";
      
      
      //Primary Key
      if(attr.isPrimaryKey()){
        anOutput << "| PRI";
        for(size_t i = 3; i<KeyC; i++)  anOutput << " ";
      }else{
        anOutput << "| ";
        for(size_t i = 0; i<KeyC; i++)  anOutput << " ";
      }
      
      //Default
      anOutput << "| ";
      for(size_t i = 0; i<DefaultC; i++)  anOutput << " ";
      
      //Extra
      if(attr.isAutoIncrement()){
        anOutput << "| auto";
        for(size_t i = 4; i<ExtraC; i++)  anOutput << " ";
      }else{
        anOutput << "| ";
        for(size_t i = 0; i<ExtraC; i++)  anOutput << " ";
      }
      
      
      anOutput << "|" << std::endl;
    }
    
    
    anOutput << theSep << std::endl;

    return true;
  }
  
}
