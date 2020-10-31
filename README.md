# Banking-System-Project-using-System-Calls

This is a Banking system project implemented in C language using system calls.

It has the following functionalities:

- All account transactional details and customer information are stored in files.

- Account holders have to pass through a login system to enter their accounts.

- The application possess password-protected administrative access,thus preventing the whole management system from unauthorized access.

- Three types of login created: normal user, joint account user,administrator.

- Once you login as administrator, you can be able to add, delete, modify, search for a
specific account details.

- Once the customer connect to the server, they get the login and password prompt.
After successful login, they get to choose the operation : Deposit, Withdraw, Balance Enquiry, Password Change, View details, Exit.

- If the customer logs in as a joint account holder, proper file locking is implemented. If the customer wants to view the account details then read lock is set else if the customer wants to withdraw or deposit then write lock is used to protect the critical data section.

- Socket programming is used in this project, where Server maintains the data base and services multiple clients concurrently. Client program can connect to the server and access their specific account details.

- System calls are used instead of Library functions wherever it is possible in the project:
Process Management, File Management, File Locking, Multithreading and Inter Process Communication Mechanisms.


How to run ?

First, compile and execute server.c file in a terminal.

Then without closing this terminal, Open another(2nd) terminal.

Compile and execute client.c file on the 2nd terminal to view the working code.
