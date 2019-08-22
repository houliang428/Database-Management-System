//
//  DBManager.cpp
//  Database3


#include <iostream>
#include <string>
#include "DBManager.hpp"
#include "Tokenizer.hpp"
#include "Storage.hpp"
#include "Statement.hpp"
#include "keywords.hpp"
#include "Database.hpp"
#include "FolderReader.hpp"
#include "DBStatement.hpp"

namespace ECE141 {

  
  class FolderView : public View {
  public:
    FolderView(const FolderReader &aReader, const char *anExtension="db")
      : reader(aReader), extension(anExtension), stream(nullptr) {}
    
    virtual ~FolderView() {}
    
    // add operator() here to receive files from a Folder Reader...
    bool operator()(const std::string &aString) {
      (*stream) << aString << std::endl;
      return true;
    }
    
    //called for each db file in your storage folder...
    virtual bool show(std::ostream &aStream) {
      stream = &aStream;
      aStream << "Showing databases: \n";
      reader.each(*this, "db");
      return true;
    }
    
    const FolderReader  &reader;
    const char          *extension;
    std::ostream        *stream;
  };


  //===================================================================
  
  
  DBManager::DBManager(IInterpreter *aNext) : CommandProcessor(aNext) {
    activeDB=nullptr;
  }
  
  DBManager::~DBManager() {
    releaseDB();
  }
  
  DBManager& DBManager::releaseDB() {
    if(activeDB) {
      delete activeDB;
      activeDB=nullptr;
    }
    return *this;
  }
  
  // USE: act as statement factory for DB-commands...
  Statement* DBManager::getStatement(Tokenizer &aTokenizer) {
    Statement *theResult=nullptr;
    if(aTokenizer.size()) {
      Token theToken = aTokenizer.current();
      switch(theToken.keyword) {
        case Keywords::create_kw    : theResult = new CreateDBStatement(*this); break;
        case Keywords::drop_kw      : theResult = new DropDBStatement(*this); break;
        case Keywords::show_kw      : theResult = new ShowDBStatement(*this); break;
        case Keywords::describe_kw  : theResult = new DescribeDBStatement(*this); break;
        case Keywords::use_kw       : theResult = new UseDBStatement(*this); break;
        default: break;
      }
      if(theResult) {
        StatusResult theError = theResult->parse(aTokenizer);
        if(!theError) {
          delete theResult;
          theResult=nullptr;
        }
      }
    }
    return theResult;
  }
  
  // USE: call this to find (and open) db (storage) file with given name in known location...
  Database* DBManager::loadDatabase(const std::string &aName) const {
    Database *theResult=nullptr;
    if(FolderReader *theReader= FolderReader::factory(Storage::getDefaultStoragePath())) {
      if(theReader->exists(Storage::getDatabasePath(aName))) {
        theResult= new Database(aName, OpenExistingStorage{});
      }
      delete theReader;
    }
    return theResult;
  }
  
  // USE: called when it's time to create an actual DB file...
  StatusResult DBManager::createDatabase(const std::string &aName) {
    
    activeDB=releaseDB().loadDatabase(aName); //try to load DB with given name...
    if(!activeDB) {
      
      //if you're here, we can CREATE a db because the given name isn't used...
      
      try {
        
        //STUDENT:  create a new 'activeDB' database by constructing
        //a new Database object, use CreateNewStorage{} tag...
        
        activeDB =nullptr; //SET THIS TO NEW DB you created...
        
        //STUDENT: Write a message to std::out indicating the task has been completed...
        //std::cout << "created db " << aName << std::endl;
        
        //if it all goes well, return StatusResult{noError},
        //otherwise return appropriate error code...
        Database* newDB = new Database(aName, CreateNewStorage());
        activeDB = newDB;
        std::cout<<"created db " << aName << std::endl;
        
        return StatusResult{noError};
      }
      catch(...) {
        return StatusResult{storageFull};
      }
    }
    return StatusResult{noError};
  }
  
  // USE: call this to perform the dropping of a database (remove the file)...
  StatusResult DBManager::dropDatabase(const std::string &aName) {
    
    /* STUDENT -- Your tasks are:

     NOTE: If database name is same as activeDB, call releaseDB() first

      1. Call Storage::getDatabasePath(name) to get a full path to the db file
      2. Create a FolderReader for your platform, and call FolderReader->exists(name) to see if the system
         knows about the named file. If it doesn't, return an error.
      3. Assuming the name is a real database, call std::remove(name) to delete the file
      4. Make sure you clean up any objects you created
      5. Display a message (std::cout) to your user indicating the you deleted their file
      6. Return the appropriate error code
     
    */
    if(!activeDB){
      activeDB = loadDatabase(aName);
    }
    activeDB->DropIndexes();
    releaseDB();
    
    FolderReader* aFolderReader = FolderReader::factory(Storage::getDefaultStoragePath());
    
    if (!aFolderReader->exists(Storage::getDatabasePath(aName))){
      return StatusResult{unknownDatabase};
    }
    
    std::remove((Storage::getDatabasePath(aName)).c_str());
    delete aFolderReader;
    std::cout<< aName + ".db deleted"<<std::endl;
    return StatusResult{noError};
  }
  
  // USE: call this to have the DB dump all storage blocks in descriptive format... (DEBUG)
  StatusResult DBManager::describeDatabase(const std::string &aName) const {    
    if(Database *theDB=loadDatabase(aName)) {
      std::cout<<"Database Information: " << aName << std::endl;
      theDB->describe(std::cout);
      delete theDB;
      return StatusResult{noError};
    }
    return StatusResult{ECE141::unknownDatabase};
  }
  
  // USE: call this to have system display list of DB files from known storage location...
  StatusResult DBManager::showDatabases() const {
    
    //STUDENT: your tasks:
    //  1. make a folder reader...
    //  2. make a folderView (with given reader)...
    //  3. Tell the view to show itself on std::cout ...
    //  4. Make sure you aren't leaking resources you're creating...
    FolderReader *kReader = FolderReader::factory(Storage::getDefaultStoragePath());
    FolderView *kView = new FolderView(*kReader);
    kView->show(std::cout);
    delete kReader;
    delete kView;
    
    return StatusResult{noError};
  }
  
  // USE: call this to cause a DB object to be loaded into memory for use...
  StatusResult DBManager::useDatabase(const std::string &aName) {
    
    //STUDENT: your tasks:
    //  1. Try to load the database object with given name (see: loadDatabase())...
    //  2. If it worked, show a success msg, and return noError status, otherwise, return error status
    //  3. save loaded database into the activeDB data member!
    if(Database* newDB = loadDatabase(aName)){
      activeDB = newDB;
      std::cout<<"using "<< aName << std::endl;
      return StatusResult{noError};
    }
    
    return StatusResult{ECE141::unknownDatabase};
  }
  
  // USE: router and observer for db-commands...
  StatusResult DBManager::interpret(const Statement &aStatement) {
    return aStatement.run(std::cout);
  }
  
  
}
