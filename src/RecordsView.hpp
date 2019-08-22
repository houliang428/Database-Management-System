//
//  RecordsView.hpp
//  Datatabase4
//


#ifndef RecordsView_h
#define RecordsView_h

#include <iostream>
#include <iomanip>
#include "View.hpp"
#include "Entity.hpp"
#include "Storage.hpp"

namespace ECE141 {

  // USE: This view is used to show a record set in tabular format...
  class RecordsView : public View {
  public:
    
    RecordsView(const Entity &anEntity, RowCollection &aCollection)
      : collection{aCollection}, entity(anEntity) {}
    
    // USE: this is the main show() for the view, where it presents all the rows...
    bool show(std::ostream &anOutput) {
      //STUDENT: Implement this, to show each row (from aCollection) like this example:
      //

      // > select * from Names
      // +-----------+----------------------+
      // | id        | first_name           |
      // +-----------+----------------------+
      // | 1         | rickg                |
      // +-----------+----------------------+
      // | 2         | ramsin               |
      // +-----------+----------------------+
      std::string separate;
      AttributeList attrs = entity.getAttributes();
      for(auto attr:attrs){
        separate += "+";
        for(unsigned int i=0; i<=2*attr.getName().size(); i++)
          separate += "-";
      }
      separate +="+";
      
      anOutput << separate << std::endl;
      anOutput <<"|";
      for(auto attr:attrs){
        anOutput << " " << attr.getName();
        anOutput << std::setw((int)attr.getName().size()+1) << "|";
      }
      anOutput << std::endl;
      anOutput << separate << std::endl;
      
      RowList temp = collection.getRows();
      for(auto aRow:temp){
        KeyValues pairs = aRow->getColumns();
        anOutput <<"|";
        for(auto attr:attrs){
          std::string values = pairs[attr.getName()];
          anOutput<< " " << values;
          anOutput<< std::setw((int) (2*attr.getName().size()+1-values.size())) << "|";
        }
        anOutput << std::endl;
        anOutput << separate << std::endl;
      }

      return true; //or false ,if your view fails...
    }
    
  protected:
    RowCollection  &collection;
    const Entity   &entity;
  };

}

#endif /* RecordsView_h */
