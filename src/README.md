# ECE141b -- Spring 2019 Final
## Altering a table
<hr>

We've all worked hard this term, to build a working database. Your last task, and an essential ingredient of life, is to deal with change. For this task, you will implement the `alter table |name| add |field_name| |type|` command, which alters an entity, and updates the associated row column(s) in your table.  After updating your code, you will execute the following statements:


```
CREATE DATABASE final
CREATE TABLE Names (id integer AUTO_INCREMENT PRIMARY KEY, first_name VARCHAR(30) NOT NULL)
INSERT INTO Names (first_name) VALUES ("rickg"), ("chandhini")
SELECT * from Names order by first_name
ALTER TABLE Names add last_name varchar(25)
UPDATE Names set last_name="Gessner" where id=1
UPDATE Names set last_name="Grandhi" where id=2
SELECT * from Names order by last_name
```

Your output will look (approximately) like this:
```
select * from Names order by first_name
+-----------+----------------------+
| id        | first_name           |
+-----------+----------------------+
| 1         | chandhini            |
+-----------+----------------------+
| 2         | rickg                |
+-----------+----------------------+

//any output from altering table here..
//any output from updating your table here...

select * from Names order by last_name
+-----------+----------------------+----------------------+
| id        | first_name           | last_name            |
+-----------+----------------------+----------------------+
| 1         | rick                 | gessner              |
+-----------+----------------------+----------------------+
| 2         | chandhini            | grandhi              |
+-----------+----------------------+----------------------+
```


## Step 1 - Setting up your project

The good news, is that the `Tokenizer`, and `AlterStatement` classes have all been updated for you in advance. Yay!  

Copy all your files from Assignment 8 into this your final project folder. Be sure not to overwrite the following files:

```
makefile 
student.json
SQLStatement.[*]
```

This version of SQLStatement contains three changes. First, it adds a new statement type (`AlterStatement`). Second, it factors utilty methods from the `CreateStatement` class to be reusable (for the `AlterStatement` class).  Third, it adds two small improvements to handle semi-colons better for scripts. 

## Step 2 - 

### 2A: Add the method `alterTable` to SQLInterpreter.[*]

In order to support the `alter table...` command, your `SQLInterpreter.[*]` files need to be updated. Please add the `alterTable` method (below) to both files: 

```
  StatusResult SQLInterpreter::alterTable(const std::string &aName, 
                                          const Attribute &anAttribute,
                                          const std::string action) {
    if(Database *theDatabase=getActiveDatabase()) {
      if(Entity *theEntity=theDatabase->getEntity(aName)) {
        return theDatabase->alterTable(*theEntity, anAttribute, action);
      }
    }
    return StatusResult{unknownDatabase};
  }
```

### 2B: Add a new case for 'alter_kw' in  `SQLInterpreter::getStatement()` in the .cpp file:

Add the following case to the switch statement in `SQLInterpreter::getStatement()` that creates statement objects:

```
  case Keywords::alter_kw   : theResult = new AlterTableStatement(*this); break;
```

### 2C: Add the method `alterTable` to Database.[*]

The new feature requires the addition of one new method to your database class. Add this to both `Database.hpp` and `Database.cpp`. This method will be called automatically by the `SQLInterpreter` when it tries to execute an `alter table...` command.


```
StatusResult alterTable(Entity &anEntity, 
                        const Attribute &anAttribute, 
                        std::string action);
```


## Step 3 - Developing your solution

The key to understanding this challenge, is to recognize that the `alter table` command actually affects two things. First, it changes the definition of your entity, adding a new attribute.  Second, it changes _every_ existing row in your database, and adds a new (empty) value for the newly added attribute (`last_name` in this case). 

It's very likely you already most of the code you need, from the commands you've already implemented. 

> DON'T PANIC -- if this is overwhelming, just do your best. A working solution is a great result. But if you can't accomplish that, then you can still get partial credit by writing methods with lots of comments that indicate what you would do. 

> ALTER TABLE is capable of making three types of changes to an entity: 1) ADD column; 2) DROP column; 3) MODIFY existing column. For your final, you are ONLY asked to alter your table using ADD column version.

### Changing your entity

To change your entity, load it from storage (or use the one you have in memory if it's already loaded).  Add the new attribute, and resave the entity to storage, using its original block_number. 

### Changing your rows

You could conventiently get a list of `Rows` by invoking `select * from table`. Now you have a RowCollection (or similar) list of `Rows`. For each element in your collection, add a new column (`last_name`) with a default value of "". Then save the row out, as if you were doing and `update` to its original block_number.  


## Part 4 - Submitting your solution

As usual, update the settings in your student.json file. Then commit your changes to github. 

Good luck everyone.



