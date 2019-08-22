//
//  Database.cpp
//  Database1


#include "Database.hpp"
#include "View.hpp"
#include "BlockVisitor.hpp"
#include "Entity.hpp"
#include "FolderReader.hpp"
#include <sstream>


namespace ECE141 {
  
  bool describeBlock(const Block &aBlock, uint32_t aBlockNum) {
    return true;
  }
  
  //==================================================================
  
  // USE: This view is used to do a debug dump of db/storage container...
  class DescribeDatabaseView : public View {
  public:
    DescribeDatabaseView(Storage &aStorage) : storage{aStorage}, stream(nullptr) {}
    
    StatusResult operator()(Storage &aStorage, const Block &aBlock, uint32_t aBlockNum) {
      std::string type;
      switch (aBlock.header.type) {
        case 'D':
          type = "Data";
          break;
          
        case 'E':
          type = "Entity";
          break;
          
        case 'I':
          type = "Index";
          break;
          
        case 'T':
          type = "TOC";
          break;
          
        default:
          type = "Free";
          break;
      }
      (*stream) << aBlockNum <<". block " << type << std::endl;
      
      return StatusResult{noError};
    }
    
    bool show(std::ostream &anOutput) {
      stream = &anOutput;
      storage.eachBlock(*this);
      return true;
    }
    
  protected:
    Storage       &storage;
    std::ostream  *stream;
  };
  
  //==================================================================
  
  // USE: This view is used to show list of entities in TOC....
  class ShowTablesView : public View {
  public:
    ShowTablesView(Block &aTOC) : toc{aTOC} {}
    
    bool show(std::ostream &anOutput) {
      for(int i=0;i<toc.entities.header.count;i++) {
        anOutput << toc.entities.items[i].name << std::endl;
      }
      return true;
    }
    
  protected:
    Block         &toc;
  };
  
  //==================================================================
  
  Database::Database(const std::string aName, CreateNewStorage)
  : name(aName), storage(aName, CreateNewStorage{}) {
  }
  
  Database::Database(const std::string aName, OpenExistingStorage)
  : name(aName), storage(aName, OpenExistingStorage{}) {
  }
  
  Database::~Database() {
    storage.writeBlock(0,storage.toc);
    saveEntities();
    saveIndexes();
    
    for (auto &thePair : entities) {
      delete thePair.second;
    }
    entities.clear();
    for(auto &thePair : PRI_index){
      delete thePair.second;
    }
    
  }
  
  // USE: a child object needs a named entity for processing...
  Entity* Database::getEntity(const std::string &aName) {

    if(entities.count(aName)) {
      return entities[aName];
    }
    else {
      //check unloaded schema from storage...
      PersistEntity *thePE = storage.findEntityInTOC(aName);
      if(thePE && thePE->blocknum>0) {
        Block theBlock;
        StatusResult theResult=storage.readBlock(thePE->blocknum, theBlock, sizeof(theBlock));
        if(theResult) {
          Entity *theEntity=new Entity(theBlock,thePE->blocknum);
          entities[aName]=theEntity;
          return theEntity;
        }
      }
    }
    return nullptr;
  }
  
  //Final:
  StatusResult Database::alterTable(Entity &anEntity, const Attribute &anAttribute, std::string action){
    StatusResult theResult;
    if(action=="add"){
      //change Entity
      anEntity.addAttribute(anAttribute);
      Block theBlock(anEntity);
      storage.writeBlock(anEntity.getBlockNum(), theBlock);
      
      //change rows
      Value theValue;
      if(anAttribute.getType()==DataType::int_type) theValue = -1;
      else if(anAttribute.getType()==DataType::float_type) theValue=0;
      else if(anAttribute.getType()==DataType::bool_type)  theValue=false;
      else if(anAttribute.getType()==DataType::timestamp_type)  theValue=0;
      else if(anAttribute.getType()==DataType::varchar_type)  theValue="";
      
      RowCollection theCollection;
      if((theResult=selectRows(theCollection, anEntity, Filters()))) {
        RowList& theRows=theCollection.getRows();
        for(auto *theRow : theRows) {
          theRow->addColumn(anAttribute.getName(), theValue);
          //Resave rows
          Block theBlock;
          const_cast<Entity&>(anEntity).encodeData(const_cast<KeyValues&>(theRow->getColumns()), theBlock);
          theBlock.header.type = 'D';
          theBlock.header.extra = anEntity.getHash();
          storage.writeBlock(theRow->getBlockNumber(), theBlock);
        }
      }
    }
    
    else if(action =="drop"){
      //drop action
    }
    else if(action=="modify"){
      //modify action
    }
    return theResult;
  }
  
  // USE:   Add a new table (entity);
  StatusResult Database::createTable(Entity *anEntity) {
    std::string &theName=anEntity->getName();
    
    Index* theIndex = new Index(anEntity->getPrimaryKey(),anEntity->name,anEntity->getPrimaryKeyType());
    PRI_index[theName] = theIndex;
    
    //create a new index binary file
    std::string thePath = storage.getIndexPath(anEntity->name);
    stream.clear();
    stream.open(thePath.c_str());
    if(!stream.is_open()) {
      std::cerr<<"can't open file"<<std::endl;
    }
    stream.close();
    
    
    entities[theName]=anEntity;
    
    Block theBlock;
    theBlock=(*anEntity); //convert from entity...
    //Entity aEntity(theBlock);
    
    StatusResult theResult= storage.addEntity(theName, theBlock);
    Entity *theEntity=entities[theName];
    theEntity->blockNum=theResult.value; //hang on to the blocknum...
    
    return theResult;
  }
  
  
  Index* Database::getIndexForTableField(const std::string tablename){
    if(PRI_index.count(tablename)){
      return PRI_index[tablename];
    }
    else{
      std::string thePath = storage.getIndexPath(tablename);
      std::ifstream istream(thePath.c_str());
      if(istream){
        std::string tablename;
        istream >> tablename;
        std::string field;
        istream >> field;
        std::string keyType;
        istream >> keyType;
        DataType theType = MapForDecode[keyType];
        
        Index *theIndex = new Index(field, tablename, theType);
        
        while(1){
          std::string keypairs;
          istream >> keypairs;
          if(istream.eof())   break;
          auto pos = keypairs.find(":");
          
          Value theValue = keypairs.substr(0,pos);
          theValue.become(theType);
          uint32_t blockNum = std::stoi(keypairs.substr(pos+1,-1));
          theIndex->addKeyValue(theValue, blockNum);
        }
        PRI_index[tablename] = theIndex;
        return theIndex;
      }
    }
    return nullptr;
  }
  
  
  // USE: called to actually delete rows from storage for table (via entity) with matching filters...
  StatusResult Database::deleteRows(const Entity &anEntity, const Filters &aFilters) {

    RowCollection theCollection;
    StatusResult theResult = selectRows(theCollection, anEntity, aFilters);
    Index* theIndex = PRI_index[anEntity.name];
    IndexType& pairs = theIndex->getIndexPairs();
    
    if(theResult){
      RowList& theRows=theCollection.getRows();
      for(auto *theRow : theRows) {
        storage.releaseBlock(theRow->getBlockNumber());
        
        //delete index
        KeyValues valuepairs = theRow->getColumns();
        for(auto rowpair:valuepairs){
          if(rowpair.first == const_cast<Entity&>(anEntity).getPrimaryKey())
            pairs.erase(rowpair.second);
        }
        
        
        if(!theResult) break;
      }
      return StatusResult{noError};
    }
    return theResult;
  }
  
  // USE: Call this to dump the db for debug purposes...
  StatusResult Database::describe(std::ostream &anOutput) {
    if(View *theView = new DescribeDatabaseView(storage)) {
      theView->show(anOutput);  //STUDENT you need to implement the view...
      delete theView;
    }
    return StatusResult{notImplemented};
  }
  
  // USE: call this to actually remove a table from the DB...
  StatusResult Database::dropTable(const std::string &aName) {
    
    //         1. find the Entity in storage TOC
    //         2  if found, ask storage to drop it
    //         3. if you cache entities on DB, remove it there too...
    if(PersistEntity *theEntity = storage.findEntityInTOC(aName)){
      storage.dropEntity(aName);
      //cached entities on DB, remove it
      if(entities.count(aName)){
        entities.erase(aName);
      }
      Entity temp(aName);
      DropTable tableDrop(temp.getHash());
      storage.eachBlock(tableDrop);
      
      
      delete PRI_index[aName];
      PRI_index.erase(aName);
      std::string thePath = storage.getIndexPath(aName);
      std::remove(thePath.c_str());

      return StatusResult{noError};
    }
    return StatusResult{unknownTable};
  }
  
  // USE: call this to add a row into the given database...
  StatusResult Database::insertRow(const Row &aRow, const std::string &aTableName) {
    if(Entity *theEntity=getEntity(aTableName)) {
      //   1. encode row into a block
      //   2. add block to storage
      //   3. maybe in the future add it to an index
      //   4. get the next auto_incr value from entity (if you need it for id field)...
      Row theRow(aRow);
      StatusResult theResult = storage.findFreeBlockNum();
      theRow.setBlockNumber(theResult.value);
      
      Value theValue;
      
      if(theEntity->validate(aRow.columns)){
        for(auto it=theRow.columns.begin(); it!=theRow.columns.end(); it++){
          if(it->first == theEntity->getPrimaryKey()){
            it->second = theEntity->getNextAutoIncrementValue();
            theValue = it->second;
          }
        }
      }
      
      Block theBlock(theRow.getBlockNumber());
      theBlock.header.type = 'D';
      theEntity->encodeData(theRow.columns, theBlock);
      //Use extra in header to save hash table string
      theBlock.header.extra = theEntity->hashString(aTableName.c_str());
      
      if(storage.addBlock(theBlock)){
        if(Index *theIndex = getIndexForTableField(aTableName)) {
          theIndex->addKeyValue(theValue, theRow.getBlockNumber());
        }
        return StatusResult{noError};
      }else{
        return StatusResult{storageFull};
      }
      
    }
    return StatusResult{unknownTable};
  }
  
  // USE: select a set of rows that match given filters (or all rows if filters are empty)...
  StatusResult Database::selectRows(RowCollection &aCollection, const Entity &anEntity,
                                    const Filters &aFilters, const StringList *aFields) {
    // Get blocks for this entity from storage, decode into rows, and add to collection
    //         NOTE:  aFields (if not empty) tells you which fields to load per row;
    //                otherwise load all fields (*)
    
    if(Index* theIndex = getIndexForTableField(const_cast<Entity&>(anEntity).getName())){
      IndexType pairs = theIndex->getIndexPairs();
      for(auto pair:pairs){
        uint32_t theBlockNum = pair.second;
        Block theBlock;
        if(StatusResult theResult= storage.readBlock(theBlockNum, theBlock)) {
          KeyValues theValues;
          theResult=anEntity.decodeData(theBlock, theValues, aFields);
          if(aFilters.matches(theValues)){
            Row *newRow = new Row(theValues);
            newRow->setBlockNumber(theBlockNum);
            aCollection.add(newRow);
          }
        }
      }
    }
    
    else{
      rowRetriever *rowsCollection = new rowRetriever(anEntity,aFilters,aFields,aCollection);
      if(storage.eachBlock(*rowsCollection)){
        return StatusResult{noError};
      }
    }
    return StatusResult{noError};
  }
  
  //USE: resave entities that were in memory and changed...
  StatusResult Database::saveEntities() {
    StatusResult theResult{noError};
    for (auto thePair : entities) {
      if(thePair.second->isDirty()) {
        Block theBlock=(*thePair.second);
        theResult=storage.writeBlock(thePair.second->blockNum, theBlock);
      }
    }
    return theResult;
  }
  
  StatusResult Database::saveIndexes() {
    StatusResult theResult{noError};
    
    for(auto it=PRI_index.begin(); it!=PRI_index.end(); it++){
      Index* theIndex = it->second;
      std::string thePath = storage.getIndexPath(theIndex->getName());
      stream.open (thePath.c_str(), std::ofstream::trunc);
      if(stream.is_open()) {
        stream << theIndex->getName() << std::endl;
        stream << theIndex->getField() << std::endl;
        stream << MapforTable[theIndex->getType()] << std::endl;
        
        for(auto pair:theIndex->getIndexPairs()){
          std::string value = pair.first;
          stream << value << ":" << pair.second << std::endl;;
        }
      }
      stream.close();
    }
    return theResult;
  }
  
  
  
  //USE: show the list of tables in this db...
  StatusResult Database::showTables(std::ostream &anOutput) {
    
    //STUDENT: create a ShowTablesView object, and call the show() method...
    //         That view is declared at the top of this file.
    ShowTablesView *tableView = new ShowTablesView(storage.toc);
    if( tableView->show(anOutput) ){
      delete tableView;
      return StatusResult{noError};
    }
    return StatusResult{notImplemented};
  }
  
  // USE: called to update records already in db (future assignment)...
  StatusResult Database::updateRow(const Row &aRow, const KeyValues &aKVList, const Entity &anEntity){
    //STUDENT: Implement this...
    if(anEntity.validate(aKVList)){
      KeyValues& theColumns = const_cast<KeyValues&>(aRow.getColumns());
      
      for(auto it = aKVList.begin(); it!=aKVList.end(); it++){
        theColumns[it->first] = aKVList.at(it->first);
      }
  
      Block theBlock;
      const_cast<Entity&>(anEntity).encodeData(const_cast<KeyValues&>(theColumns), theBlock);
      theBlock.header.type = 'D';
      theBlock.header.extra = anEntity.getHash();
      storage.writeBlock(aRow.getBlockNumber(), theBlock);
      return StatusResult{noError};
    }
 
    return StatusResult{invalidAttribute};
  }
  
  // USE: asks the DB if a table exists in storage...
  bool Database::tableExists(const std::string aName) {
    //STUDENT: implement this if you need it...
    if(getEntity(aName))  return true;
    return false;
  }
  
  void Database::DropIndexes(){
    if(entities.empty()){
      std::vector<std::string> entityName = storage.GetEntitiesName();
      for(auto name:entityName){
        std::string thePath = storage.getIndexPath(name);
        FolderReader* aFolderReader = FolderReader::factory(thePath.c_str());
        if(aFolderReader->exists(thePath)){
          std::remove(thePath.c_str());
        }
      }
      return;
    }
    
    
    for(auto it = entities.begin(); it!=entities.end(); it++){
      std::string thePath = storage.getIndexPath(it->second->getName());
      FolderReader* aFolderReader = FolderReader::factory(thePath.c_str());
      if(aFolderReader->exists(thePath)){
        std::remove(thePath.c_str());
      }
    }
    PRI_index.clear();
  }
  
}

