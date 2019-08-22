//
//  Value.hpp
//  Database1
//


#ifndef Value_hpp
#define Value_hpp

#include <string>
#include <map>
#include <iostream>
#include "Errors.hpp"

namespace ECE141 {
    
    //NOTE: integer is a SIGNED value
    //NOTE: timestamp is an UNSIGNED value!!!!!
    
    enum class DataType {
        no_type='N', int_type='I', float_type='F', bool_type='B', timestamp_type='T', varchar_type='V',
        //our enums are associated w/chars to be readable in encoded storage blocks...
    };
    
    //USE: define a dynamic type that allows us to hold field data in rows...
    
    static std::map<DataType,std::string> getTypeName = {
        std::make_pair(DataType::no_type,         "N"),
        std::make_pair(DataType::int_type,        "I"),
        std::make_pair(DataType::float_type,      "F"),
        std::make_pair(DataType::bool_type,       "B"),
        std::make_pair(DataType::timestamp_type,  "T"),
        std::make_pair(DataType::varchar_type,    "V"),
    };
  
    static std::map<DataType,std::string> MapforTable = {
      std::make_pair(DataType::no_type,         "notype"),
      std::make_pair(DataType::int_type,        "integer"),
      std::make_pair(DataType::float_type,      "float"),
      std::make_pair(DataType::bool_type,       "bool"),
      std::make_pair(DataType::timestamp_type,  "timestamp"),
      std::make_pair(DataType::varchar_type,    "varchar"),
    };
  
    static std::map<std::string, DataType> MapForDecode = {
      std::make_pair("notype",    DataType::no_type),
      std::make_pair("integer",   DataType::int_type),
      std::make_pair("float",     DataType::float_type),
      std::make_pair("bool",      DataType::bool_type),
      std::make_pair("timestamp", DataType::timestamp_type),
      std::make_pair("varchar",   DataType::varchar_type),
    };
    
    class Value {
        
        //IMPORTANT!
        
        //This class needs multiple ctors() for each type (book, timest, int, float varchar)...
        //This class needs operator=, for each basic type...
        //This class needs conversion operator for each basic type
        
    private:
        size_t isize;
        DataType itype;
        union {
            short  data_notype = -1;
            int    data_int;
            float  data_float;
            bool   data_bool;
            time_t data_time;
            char   data_char[32];
        };
        
    public:
        Value(): isize(0), itype(DataType::no_type), data_notype(-1) {}           //default ctor
        Value(int aSize, DataType aType): isize(aSize), itype(aType) {}
        
        Value(const int &item){                                       //ctor for int
            data_int = item;
            isize = sizeof(data_int);
            itype = DataType::int_type;
        }
        
        Value(const float &item){                                     //ctor for float
            data_float = item;
            isize = sizeof(data_float);
            itype = DataType::float_type;
        }
        
        Value(const bool &item){                                       //ctor for bool
            data_bool = item;
            isize = sizeof(data_bool);
            itype = DataType::bool_type;
        }
        
        Value(const time_t &item){                                     //ctor for time_t
            data_time = item;
            isize = sizeof(data_time);
            itype = DataType::timestamp_type;
        }
        
        Value(const char* item){                                        //ctor for char*
            strcpy(data_char,item);
            isize = sizeof(data_char);
            itype = DataType::varchar_type;
        }
        
        Value(const std::string &item){                                 //ctor for char*
            strcpy(data_char,item.c_str());
            isize = sizeof(data_char);
            itype = DataType::varchar_type;
        }
        
        //##########################################################################
        size_t        getSize();
        DataType      getType() const;
        
        StatusResult  become(DataType aType); //cause the value to assume given type
        
        void          debugDump(std::ostream &anOutput);  //describe (emit) this object onto given stream...
        
        //Operator = overloading
        Value&          operator=(const int &aValue);
        Value&          operator=(const float &aValue);
        Value&          operator=(const bool &aValue);
        Value&          operator=(const time_t &aValue);
        Value&          operator=(const char* aValue);
        Value&          operator=(const std::string &aValue);
        Value&          operator=(const unsigned int aValue);
        //conversion operator  class to basic
        operator int();
        operator float();
        operator bool();
        operator time_t();
        operator char*();
        operator std::string() const;
        
        bool operator==(const Value &aValue) const;
        bool operator<(const Value &aValue) const;
        
    };
    
    //for holding a collection of key/values...
    using KeyValues = std::map<std::string,  Value>;
    
}

#endif /* Value_h */

