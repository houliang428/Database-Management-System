//
//  Storage.hpp
//  Database3
//


#ifndef Storage_hpp
#define Storage_hpp

#include <stdint.h>
#include <string>
#include <fstream>
#include "Errors.hpp"
#include <functional>
#include "Row.hpp"
#include "MemoryStream.hpp"
#include <unordered_map>

namespace ECE141 {
  
  
  const char kDataBlockType   = 'D';
  const char kEntityBlockType = 'E';
  const char kFreeBlockType   = 'F';
  const char kIndexBlockType  = 'I';
  
  
  struct BlockHeader {
    
    BlockHeader() : type('D') {reserved=0; extra=0; id=0;}
    
    BlockHeader(char aType) : type(aType)  {reserved=0; extra=0; id=0;}
    
    BlockHeader(const BlockHeader &aCopy) {
      type=aCopy.type;
      reserved=aCopy.reserved;
      extra=aCopy.extra;
      id=aCopy.id;
    }
    
    char      type;     //block type {[D]ata, [F]ree... }
    char      reserved; //maybe later?
    uint32_t  id;       //use this anyway you like
    uint32_t  extra;    //use this anyway you like
    
  };
  
  const size_t kPayloadSize = 1012; 
  
  //======= These are for persisting entities (tables) to storage...
  
  const size_t kStoredIdentifierLen = 15;
  
  struct PersistEntity { //a single entity
    PersistEntity() {name[0]=0; blocknum=0;};
    PersistEntity(const PersistEntity &aCopy) {
      
      size_t theLen=strlen(aCopy.name);
#if defined(__APPLE__) || defined(__linux)
      std::strncpy(name, aCopy.name, theLen);
#else
      strncpy_s(name, kStoredIdentifierLen, aCopy.name, theLen);
#endif
      name[theLen]=0;
      blocknum=aCopy.blocknum;
    }
    
    char      name[kStoredIdentifierLen+1];
    uint32_t  blocknum;
  };
  
  struct PersistEntitiesHeader {
    uint8_t           count; //how many attributes...
    uint8_t           reserved; //not used yet
  };
  
  const size_t kEntitiesPerBlock =
    (kPayloadSize-sizeof(PersistEntitiesHeader)) / sizeof(PersistEntity); //49-ish
  
  struct PersistEntities {
    PersistEntitiesHeader header;
    PersistEntity         items[kEntitiesPerBlock];
  };
  
  //======= This is for tracking free-blocks (NOT CURRENTLY USED) -- if you plan to do so...
  
  const size_t kFreeItemsPerBlock = kPayloadSize / sizeof(bool);
  struct FreeMap {
    bool items[kFreeItemsPerBlock];
  };
  
  //======== Define single block in the storage file... ==============
  
  struct Block {

    Block(char aType='D') : header(aType) {
      memset(data,0,sizeof(data));
    }
    
    Block(const Block &aCopy){
      header = aCopy.header;
      if(aCopy.header.type == 'E'){
        entities = aCopy.entities;
      }
      else if(aCopy.header.type == 'D'){
        strcpy(data, aCopy.data);
      }
    }
    
    Block(BlockHeader &aHeader) : header(aHeader), data() {}
    
    // USE: encode a key/value list into this block payload area...
    Block(const KeyValues  &aKVList) {
      //STUDENT: You need to implement this...
      /* BufferWriter Method
      ECE141::BufferWriter theWriter(data, kPayloadSize);
      theWriter << (uint8_t)aKVList.size(); //record number of fields being written...
      for(auto &thePair : aKVList) {
        theWriter << thePair.first << thePair.second;
      }
      */
      header.type = 'D';
      int index = 0;
      for(auto it = aKVList.begin(); it != aKVList.end(); it++){
        Value temp = it->second;
        std::string data_string = temp; //convert Value to string
        //Encode: field_name^data_type^data_string
        std::string encode = it->first + "^" + getTypeName[temp.getType()] + "^" + data_string + "^";
        strcpy(data+index, encode.c_str());
        index += (int) encode.size();
      }
    }
    
    
    //we use attributes[0] as table name...
    BlockHeader   header;
    union {
      char              data[kPayloadSize];
      PersistEntities   entities;
      //FreeMap         available;  //totally optional...
    };
  };
  
  //============================================================
  
  struct CreateNewStorage {};
  struct OpenExistingStorage {};
  class Storage;
  using StorageCallback = std::function<StatusResult(Storage &aStorage, const Block &aBlock, uint32_t aBlockNum)>;
  
  struct DoubleLinked{
    uint32_t blockNum;
    Block theBlock;
    DoubleLinked *prev;
    DoubleLinked *next;
    
    DoubleLinked(): blockNum(0),theBlock(Block()),prev(NULL),next(NULL){}
    DoubleLinked(uint32_t aKey, Block &aBlock): blockNum(aKey), theBlock(aBlock), prev(NULL),next(NULL){}
  };
  
  class LRUCache {
  public:
    LRUCache(int capacity=15) {
      size = capacity;
      head->next = tail;
      tail->prev = head;
      cur_size = 0;
    }
    
    Block&  get(uint32_t key);
    void    put(uint32_t key, Block& aBlock);
    bool    contains(uint32_t blockNum);
    
  private:
    int size, cur_size;
    DoubleLinked *head = new DoubleLinked();
    DoubleLinked *tail = new DoubleLinked();
    std::unordered_map<uint32_t,DoubleLinked*> hashmap;
    
    void addNode(DoubleLinked* node){
      node->prev = head;
      node->next = head->next;
      head->next->prev = node;
      head->next = node;
    }
    
    void remove(DoubleLinked* node){
      node->prev->next = node->next;
      node->next->prev = node->prev;
    }
    
    void update(DoubleLinked* node){
      remove(node);
      addNode(node);
    }
    
    void popBack(){
      DoubleLinked* node = tail->prev;
      remove(node);
      hashmap.erase(node->blockNum);
    }
  };
  
  
  using IndexType = std::map<Value, uint32_t>;
  
  class Index {
  public:
     //c++ typedef...
    Index(const std::string &aField, std::string aTableName, DataType aKeyType)
    : field(aField), tableName(aTableName), keyType(aKeyType){}
    
    Index&      addKeyValue(const Value &aKey, uint32_t aValue);
    Index&      removeKeyValue(const Value &aKey);
    bool        contains(const Value &aKey);
    uint32_t    getValue(const Value &aKey);
    IndexType&  getIndexPairs();
    std::string getName()  {return tableName;}
    std::string getField() {return field;}
    DataType    getType()  {return keyType;}
    
  protected:
    IndexType   index;
    std::string field;    //name of the field being indexed
    DataType    keyType;  //what Value datatype are we using for this field?
    std::string    tableName;
  };
  
  
  

  // USE: Our main class for managing storage...
  class Storage {
  public:
    
    Storage(const std::string aName, CreateNewStorage);
    Storage(const std::string aName, OpenExistingStorage);
    //Storage(const Storage &aStorage);
    
    ~Storage();
    
    static const char* getDefaultStoragePath();
    static std::string getDatabasePath(const std::string &aDBName); //build a path to db with given name...
    static std::string getIndexPath(const std::string &aTableName);
    StatusResult    loadTOC();  //get it from where you placed it in the storage file...
    StatusResult    makeEmpty(); //This works by default..
    StatusResult    setupEmptyStorage();
    uint32_t        getTotalBlockCount();

    StatusResult    readBlock(int aBlockNumber, Block &aBlock, std::size_t aBlockSize=sizeof(Block));
    StatusResult    writeBlock(int aBlockNumber, Block &aBlock);
    
    StatusResult    addBlock(Block &aBlock);
    StatusResult    releaseBlock(int aBlockNumber); //mark the block as free...
    
    StatusResult    addEntity(const std::string &aName, Block &aBlock);
    StatusResult    dropEntity(const std::string &aName);
    
    PersistEntity*  findEntityInTOC(const std::string &aName); //return NULL if not found...
    
    StatusResult    eachBlock(StorageCallback aCallback);
    std::vector<std::string>   GetEntitiesName();
    
    LRUCache        BlockCache;
  protected:
    Block           toc;
    
    std::fstream    stream;
    std::string     name;
    
    bool            isReady() const;
    StatusResult    findFreeBlockNum();
    
    friend class Database;
  };
  
}


#endif /* Storage_hpp */
