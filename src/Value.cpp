//
//  Value.cpp
//  Database1


#include "Value.hpp"

namespace ECE141 {
    
    //Your implementation here...
    size_t Value::getSize(){
        return isize;
    }
    
    DataType Value::getType() const{
        return itype;
    }
    
    StatusResult Value::become(DataType aType){
        if(itype == aType)  return StatusResult();
        
        if(aType == DataType::no_type){
            data_notype = -1;
            isize = sizeof(data_notype);
        }
        
        else if(aType == DataType::int_type){
            switch (itype) {
                case DataType::float_type:
                    data_int = (int)data_float;
                    itype = aType;
                    break;
                    
                case DataType::bool_type:
                    data_int = data_bool?1:0;
                    itype = aType;
                    break;
                    
                case DataType::timestamp_type:
                    data_int = (int)data_time;
                    itype = aType;
                    break;
                    
                case DataType::varchar_type:
                    data_int = atoi(data_char);
                    itype = aType;
                    break;
                    
                default:
                    break;
            }
        }
        
        else if(aType == DataType::float_type){
            switch (itype) {
                case DataType::int_type:
                    data_float = data_int*1.0;
                    itype = aType;
                    break;
                    
                case DataType::bool_type:
                    data_float = data_bool?1.0:0.0;
                    itype = aType;
                    break;
                    
                case DataType::timestamp_type:
                    data_float = (int)data_time*1.0;
                    itype = aType;
                    break;
                    
                case DataType::varchar_type:
                    data_float = atof(data_char);
                    itype = aType;
                    break;
                    
                default:
                    break;
            }
        }
        
        else if(aType == DataType::bool_type){
            switch (itype) {
                case DataType::int_type:
                    data_bool = (bool)data_int;
                    itype = aType;
                    break;
                    
                case DataType::float_type:
                    data_bool = (bool)data_float;
                    itype = aType;
                    break;
                    
                case DataType::timestamp_type:
                    data_bool = (int)data_time;
                    itype = aType;
                    break;
                    
                case DataType::varchar_type:
                    data_bool = strcmp(data_char,"")!=0 ;   //return false if it's empty;
                    itype = aType;
                    break;
                    
                default:
                    break;
            }
        }
        
        else if(aType == DataType::timestamp_type){
            switch (itype) {
                case DataType::int_type:
                    data_time = (time_t)data_int;
                    itype = aType;
                    break;
                    
                case DataType::float_type:
                    data_time = (time_t)data_float;
                    itype = aType;
                    break;
                    
                case DataType::bool_type:
                    data_time = data_bool?1:0;
                    itype = aType;
                    break;
                    
                case DataType::varchar_type:
                    struct tm tm;
                    strptime(data_char, "%H:%M:%S", &tm);
                    data_time = mktime(&tm);  // t is now your desired time_t
                    itype = aType;
                    break;
                    
                default:
                    break;
            }
        }
        
        else if(aType == DataType::varchar_type){
            switch (itype) {
                case DataType::int_type:
                    strcpy(data_char, std::to_string(data_int).c_str());
                    isize = sizeof(data_char);
                    itype = aType;
                    break;
                    
                case DataType::float_type:
                    strcpy(data_char, std::to_string(data_float).c_str());
                    isize = sizeof(data_char);
                    itype = aType;
                    break;
                    
                case DataType::bool_type:
                    if(data_bool)   strcpy(data_char, "true");
                    else            strcpy(data_char, "false");
                    isize = sizeof(data_char);
                    itype = aType;
                    break;
                    
                case DataType::timestamp_type:
                    strcpy(data_char, std::to_string(data_time).c_str());
                    isize = sizeof(data_char);
                    itype = aType;
                    break;
                    
                default:
                    break;
            }
        }
        
        else{
            return StatusResult{unknownType};
        }
      
        return StatusResult(Errors::unknownType);
    }
    
    //################ debug information #############################
    void Value::debugDump(std::ostream &anOutput){
        anOutput<<"type: ";
        switch (itype) {
            case DataType::no_type:
                anOutput<<"no_type, no data " <<data_int<<std::endl;
                break;
                
            case DataType::int_type:
                anOutput<<"int, value: "      <<data_int<<std::endl;
                break;
                
            case DataType::float_type:
                anOutput<<"float, value: "    <<data_float<<std::endl;
                break;
                
            case DataType::bool_type:
                anOutput<<"bool, value: "     <<(data_bool?"true":"false")<<std::endl;
                break;
                
            case DataType::timestamp_type:
                anOutput<<"timestamp, value: "<<data_time<<std::endl;
                break;
                
            case DataType::varchar_type:
                anOutput<<"varchar, value: "  <<data_char<<std::endl;
                break;
                
            default:
                break;
        }
    }
    
    //################      operator =      ##################################
    
    Value& Value::operator=(const int &aValue){
        data_int = aValue;
        itype = DataType::int_type;
        isize = sizeof(data_int);
        return *this;
    }
    
    Value& Value::operator=(const float &aValue){
        data_float = aValue;
        itype = DataType::float_type;
        isize = sizeof(data_float);
        return *this;
    }
    
    Value& Value::operator=(const bool &aValue){
        data_bool = aValue;
        itype = DataType::bool_type;
        isize = sizeof(data_bool);
        return *this;
    }
    
    Value& Value::operator=(const time_t &aValue){
        data_time = aValue;
        itype = DataType::timestamp_type;
        isize = sizeof(data_time);
        return *this;
    }
    
    Value& Value::operator=(const char* aValue){
        strcpy(data_char, aValue);
        itype = DataType::varchar_type;
        isize = sizeof(data_char);
        return *this;
    }
    
    Value& Value::operator=(const std::string &aValue){
        strcpy(data_char, aValue.c_str());
        itype = DataType::varchar_type;
        isize = sizeof(data_char);
        return *this;
    }
  
    Value& Value::operator=(const unsigned int aValue){
      data_int = aValue;
      itype = DataType::int_type;
      isize = sizeof(data_int);
      return *this;
    }
    
    
    //#################     conversion operator    ######################################
    Value::operator int(){
        switch (itype){
            case DataType::int_type:
                return data_int;
                break;
                
            case DataType::float_type:
                return (int)data_float;
                break;
                
            case DataType::bool_type:
                return data_bool? 1 : 0;
                break;
                
            case DataType::timestamp_type:
                return (int)data_time;
                break;
                
            case DataType::varchar_type:
            case DataType::no_type:
                return StatusResult(Errors::unknownType);
                break;
                
            default:
                break;
        }
        return StatusResult(Errors::unknownType);
    }
    
    Value::operator float(){
        switch (itype){
            case DataType::int_type:
                return data_int*1.0;
                break;
                
            case DataType::float_type:
                return data_float;
                break;
                
            case DataType::bool_type:
                return data_bool? 1.0 : 0.0;
                break;
                
            case DataType::timestamp_type:
                return (float)data_time;
                break;
                
            case DataType::varchar_type:
            case DataType::no_type:
                return StatusResult(Errors::unknownType);
                break;
                
            default:
                break;
        }
        return StatusResult(Errors::unknownType);
    }
    
    Value::operator bool(){
        switch (itype){
            case DataType::int_type:
                return (bool)data_int;
                break;
                
            case DataType::float_type:
                return (bool)data_float;
                break;
                
            case DataType::bool_type:
                return data_bool;
                break;
                
            case DataType::timestamp_type:
                return bool(data_time);
                break;
                
            case DataType::varchar_type:
            case DataType::no_type:
                return StatusResult(Errors::unknownType);
                break;
                
            default:
                break;
        }
        return StatusResult(Errors::unknownType);
    }
    
    Value::operator time_t(){
        switch (itype){
            case DataType::int_type:
                return (time_t)data_int;
                break;
                
            case DataType::float_type:
                return (time_t)data_float;
                break;
                
            case DataType::bool_type:
                return (time_t)data_bool;
                break;
                
            case DataType::timestamp_type:
                return data_time;
                break;
                
            case DataType::varchar_type:
            case DataType::no_type:
                return StatusResult(Errors::unknownType);
                break;
                
            default:
                break;
        }
        return StatusResult(Errors::unknownType);
    }
    
    Value::operator char*(){
        std::string ss;
        char* temp;
        
        switch (itype){
            case DataType::int_type:
                ss= std::to_string(data_int);
                temp = new char[ss.size()+1];
                strcpy(temp, ss.c_str());
                return temp;
                break;
                
            case DataType::float_type:
                ss= std::to_string(data_float);
                temp = new char[ss.size()+1];
                strcpy(temp, ss.c_str());
                return temp;
                break;
                
            case DataType::bool_type:
                ss= data_bool? "true":"false";
                temp = new char[ss.size()+1];
                strcpy(temp, ss.c_str());
                return temp;
                break;
                
            case DataType::timestamp_type:
                ss= std::to_string(data_int);
                temp = new char[ss.size()+1];
                strcpy(temp, ss.c_str());
                return temp;
                break;
                
            case DataType::varchar_type:
                return data_char;
                
            case DataType::no_type:
                ss ="-1";
                temp = new char[ss.size()+1];
                strcpy(temp, ss.c_str());
                return temp;
                break;
                
            default:
                break;
        }
    }
    
    Value::operator std::string() const{
        if(itype == DataType::int_type)              return std::to_string(data_int);
        else if(itype == DataType::float_type)       return std::to_string(data_float);
        else if(itype == DataType::bool_type)        return data_bool? "true":"false";
        else if(itype == DataType::timestamp_type)   return std::to_string(data_time);
        else if(itype == DataType::varchar_type)     return std::string(data_char);
        else                                         return "-1";
    }
    
    bool Value::operator==(const Value &aValue) const {
        if(itype != aValue.getType())   return false;
        std::string s1 = *this;
        std::string s2 = aValue;
        return s1==s2;
    }
    
    bool Value::operator<(const Value &aValue) const{
        if(itype != aValue.getType() || itype==DataType::no_type){
            std::cerr << "Invalid Types!" << std::endl;
        }
        if(itype == DataType::int_type)  return data_int < aValue.data_int;
        else if(itype == DataType::float_type)  return data_float < aValue.data_float;
        else if(itype == DataType::bool_type)   return (data_bool) && (!aValue.data_bool);
        else if(itype == DataType::timestamp_type)  return data_time < aValue.data_time;
        else if(itype == DataType::varchar_type){
          std::string aLHS(data_char);
          std::string aRHS(aValue.data_char);
          return aLHS<aRHS;
        }
        return true;
    }
    

    
}
