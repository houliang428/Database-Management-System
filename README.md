# Relational Database Management System
## A relational database management system which can be managed by SQL commands  
<hr>
This project acts like a real MySQL database system that can process SQL commands. You can create/drop databases and the program will encode the data and store it in the local machine. When you need to view the data, simply use the exact same commands in SQL by typing in "SELECT * FROM Database1" and the program will display the data in the terminal in a form.   

### Tokenizer, RGTokenizer  
Used for tokenize the command input into tokens. There are mainly three kinds of commands that need to be tokenized;
1. Meta-commands that control the system, such as the `Quit` command which terminates the app. 
2. Database-level commands (like "Create database xxx", or "Show databases")
3. Data-oriented commands (like "Create table...", "Show tables", "insert (...) into tablename", ...)  
     
### Statement, SQLStatement, DBStatement  
1. Statement is the basic abstract class of SQLStatement and DBStatement.  
2. The tokenizer parse the commands into SQLStatement or DBStatement based on the keywards.  
3. The statement will be routed to specific command processor for processing the command. 
 
 
### CommandProcessor, DBManager, SQLInterpreter  
Used for processing different command.

