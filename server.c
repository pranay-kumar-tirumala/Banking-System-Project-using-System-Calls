#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<arpa/inet.h>
#include<string.h>
#include<time.h>
char cur_user[20];
struct accountDetails
{
	char username[20];
	char password[20];
	int account_number;
	int balance;
};
struct jointAccountDetails
{
	char username1[20];
	char password1[20];
	char username2[20];
	char password2[20];
	int account_number;
	int balance;
};
void mainMenu(int);
void displayMenu(int, struct accountDetails);
void jointDisplayMenu(int, struct jointAccountDetails,struct flock,int );
void admin_functions(int);
//In C language, we get a warning if we call a function before declaring it. So to avoid warnings, we added the corresponding function prototypes above.

int checkAccountExists(char username[])
{
	struct accountDetails user_record;
	int fd1 = open("Accounts.txt", O_CREAT | O_APPEND | O_RDWR , 0666);
	while(read(fd1, (char *)&user_record, sizeof(struct accountDetails)))
	{
		if(!strcmp(user_record.username, username))	//strcmp(char* s1, const char* s2) returns 0 if both strings s1 & s2 are identical
		{
			close(fd1);
			return 1;				//Returning 1, if the username already exists in the file.
		}
	}
	close(fd1);
	return 0;						//Returning 0, if the username DOESN'T exist in the file.
}
int checkJointAccountExists(char username1[],char username2[])
{
	struct jointAccountDetails user_record;
	int fd1 = open("Joint_Account.txt", O_CREAT | O_APPEND | O_RDWR, 0666);
	while(read(fd1, (char *)&user_record, sizeof(struct jointAccountDetails)))
	{
		if((!strcmp(user_record.username1, username1)) && (!strcmp(user_record.username2,username2)) || (!strcmp(user_record.username1, username2)) && (!strcmp(user_record.username2,username1)))
		{
			close(fd1);
			return 1;				//Returning 1, if both username1 & username2 already exists in the file.
		}
	}
	close(fd1);
	return 0;						//Returning 0, if the username DOESN'T exist in the file.
}

void createAccount(char username[], char password[],int sockfd)
{
	int fd1 = open("Accounts.txt", O_CREAT | O_APPEND | O_RDWR , 0666);
	struct accountDetails user_record;
	char congo[]="Congratulations !!! Account Created Successfully.\n";
	strcpy(user_record.username, username);
	strcpy(user_record.password, password);
	user_record.balance=0;
	srand(time(0));								//Using current time as seed for random generator
	user_record.account_number=(rand()%1000)+1000;				//Account Numbers of Single A/C start from 1000
	write(fd1, (char *)&user_record, sizeof(struct accountDetails));
	write(sockfd, congo, sizeof(congo));
	close(fd1);
}
void createJointAccount(char username1[], char password1[], char username2[], char password2[], int sockfd)
{
	int fd1 = open("Joint_Account.txt", O_CREAT | O_APPEND | O_RDWR , 0666);
	struct jointAccountDetails user_record;
	char congo[]="Congratulations !!! Account Created Successfully.\n";
	strcpy(user_record.username1, username1);
	strcpy(user_record.password1, password1);
	strcpy(user_record.username2, username2);
	strcpy(user_record.password2, password2);
	user_record.balance=0;
	srand(time(0));								//Using current time as seed for random generator
	user_record.account_number=(rand()%1000)+5000;				//Account Numbers of Joint A/C start from 5000
	write(fd1, (char *)&user_record, sizeof(struct jointAccountDetails));
	write(sockfd, congo, sizeof(congo));
	close(fd1);
}

void accountSignup(int sockfd,int flag)
{
	char username[20], password[20];
	read(sockfd, username, sizeof(username));
	read(sockfd, password, sizeof(password));
	if(!checkAccountExists(username))
	{
		createAccount(username, password, sockfd);
		if(flag==0)				mainMenu(sockfd);		//0 means user is not an Admin
		else if(flag==1)		admin_functions(sockfd);		//1 means user is an Admin
	}
	else
	{
		write(sockfd, "\nThis Username already exists\n", 31);
		if(flag==0)				mainMenu(sockfd);		//0 means user is not an Admin
		else if(flag==1)		admin_functions(sockfd);		//1 means user is an Admin
	}	
}
void jointAccountSignup(int sockfd, int flag)
{
	char username1[20], password1[20], username2[20], password2[20];
	read(sockfd, username1, sizeof(username1));
	read(sockfd, password1, sizeof(password1));
	read(sockfd, username2, sizeof(username2));
	read(sockfd, password2, sizeof(password2));
	if(!checkJointAccountExists(username1,username2))
	{
		createJointAccount(username1, password1, username2, password2, sockfd);
		if(flag==0)				mainMenu(sockfd);		//0 means user is not an Admin
		else if(flag==1)		admin_functions(sockfd);		//1 means user is an Admin
	}
	else
	{
		write(sockfd, "\nThis Username already exists\n", 31);
		if(flag==0)				mainMenu(sockfd);		//0 means user is not an Admin
		else if(flag==1)		admin_functions(sockfd);		//1 means user is an Admin
	}
}

void accountLogin(int sockfd)
{
	char username[20], password[20];
	read(sockfd, username, sizeof(username));
	read(sockfd, password, sizeof(password));
	char ic[]="\nInvalid Credentials !!! Try again.\n";
	struct accountDetails user_record;
	int flag=1;						//NOTE that, If flag is 1, then it means customer was able to login successfully.
	int fd1 = open("Accounts.txt", O_CREAT | O_APPEND | O_RDWR , 0666);
	while(read(fd1, (char *)&user_record, sizeof(struct accountDetails)))
	{
		if(!strcmp(user_record.username, username) && !strcmp(user_record.password, password))
		{
			write(sockfd, &flag, sizeof(flag));
			write(sockfd, "\nLogin Successful !!!\n", 24);
			displayMenu(sockfd,user_record);
			close(fd1);
			return;		
		}
	}
	close(fd1);
	flag=0;							//NOTE that, If flag is 0, then it means customer was UNABLE to login successfully.
	write(sockfd, &flag, sizeof(flag));
	write(sockfd, ic, sizeof(ic));
	mainMenu(sockfd);	
}
void jointAccountLogin(int sockfd,int fd)
{
	char username1[20], username2[20], password[20];
	struct flock lock;
	char ic[]="\nInvalid Credentials !!! Try again.\n";
	read(sockfd, username1, sizeof(username1));
	read(sockfd, username2, sizeof(username2));
	read(sockfd, password, sizeof(password));
	strcpy(cur_user,username1);

	struct jointAccountDetails user_record;
	int flag=1;						//NOTE that, If flag is 1, then it means customer was able to login successfully.
	int fd1 = open("Joint_Account.txt", O_CREAT | O_APPEND | O_RDWR , 0666);
	while(read(fd1, (char *)&user_record, sizeof(struct jointAccountDetails)))
	{
		if((!strcmp(user_record.username1, username1) && !strcmp(user_record.password1,password) && !strcmp(user_record.username2, username2)) || (!strcmp(user_record.username1, username2) && !strcmp(user_record.username2, username1) && !strcmp(user_record.password2,password)))
		{
			lseek(fd1,-sizeof(struct jointAccountDetails),SEEK_CUR);
			lock.l_type=F_WRLCK;
			lock.l_whence=SEEK_CUR;
			lock.l_start=0;
			lock.l_len=sizeof(struct jointAccountDetails);
			/*Locking*/	
			fcntl(fd1,F_SETLKW,&lock);
			write(sockfd, &flag, sizeof(flag));
			write(sockfd, "\nLogin Successful !!!\n", 24);
			jointDisplayMenu(sockfd,user_record,lock,fd1);
			close(fd1);
			return;		
		}
	}
	close(fd1);
	flag=0;							//NOTE that, If flag is 0, then it means customer was UNABLE to login successfully.
	write(sockfd,&flag,sizeof(flag));
	write(sockfd, ic, sizeof(ic));
	mainMenu(sockfd);
}

void viewAccountDetails(int sockfd, struct accountDetails user_record)
{
	write(sockfd,user_record.username, sizeof(user_record.username));
	write(sockfd,&user_record.account_number, sizeof(user_record.account_number));
	write(sockfd,&user_record.balance, sizeof(user_record.balance));
	displayMenu(sockfd,user_record);
}
void viewJointAccountDetails(int sockfd, struct jointAccountDetails user_record,struct flock lock,int fd)
{
	write(sockfd,user_record.username1, sizeof(user_record.username1));
	write(sockfd,user_record.username2, sizeof(user_record.username2));
	write(sockfd,&user_record.account_number, sizeof(user_record.account_number));
	write(sockfd,&user_record.balance, sizeof(user_record.balance));
	jointDisplayMenu(sockfd,user_record,lock,fd);
}

void findAccountDetails(struct accountDetails user_record)
{
	struct accountDetails temp;
	int fd1 = open("Accounts.txt", O_CREAT | O_RDWR , 0666);
	while(read(fd1, (char *)&temp, sizeof (struct accountDetails)))
	{
		if(!strcmp(user_record.username, temp.username))
		{
			temp.balance = user_record.balance;
			strcpy(temp.password,user_record.password);
			lseek(fd1,-sizeof(struct accountDetails),SEEK_CUR);
			write(fd1, (char *)&temp, sizeof(struct accountDetails));
			break;
		}
	}
}
void findJointAccountDetails(struct jointAccountDetails user_record)
{
	struct jointAccountDetails temp;
	int fd1 = open("Joint_Account.txt", O_CREAT | O_RDWR , 0666);
	while(read(fd1, (char *)&temp, sizeof (struct jointAccountDetails)))
	{
		if(!strcmp(user_record.username1, temp.username1) && !strcmp(user_record.username2, temp.username2))
		{
			temp.balance = user_record.balance;
			strcpy(temp.password1,user_record.password1);
			strcpy(temp.password2,user_record.password2);
			lseek(fd1,-sizeof(struct jointAccountDetails),SEEK_CUR);
			write(fd1, (char *)&temp, sizeof(struct jointAccountDetails));	
			break;
		}
	}
}

void deposit(int sockfd, struct accountDetails user_record)
{
	int amount;
	read(sockfd, &amount, sizeof(amount));
	user_record.balance = user_record.balance + amount;
    findAccountDetails(user_record);
	write(sockfd, &user_record.balance, sizeof(user_record.balance));
	displayMenu(sockfd,user_record);
}
void jointDeposit(int sockfd, struct jointAccountDetails user_record,struct flock lock,int fd)
{
	int amount;
	read(sockfd, &amount, sizeof(amount));
	user_record.balance = user_record.balance + amount;
    findJointAccountDetails(user_record);
	write(sockfd, &user_record.balance, sizeof(user_record.balance));
	jointDisplayMenu(sockfd,user_record,lock,fd);
}

void withdraw(int sockfd, struct accountDetails user_record)
{
	int amount, flag=0;
	read(sockfd, &amount, sizeof(amount));
	if(amount>=user_record.balance)
	{
		write(sockfd,&flag,sizeof(flag));
		write(sockfd, &user_record.balance, sizeof(user_record.balance));
	}
	else
	{
		flag=1;
		user_record.balance = user_record.balance - amount;
		findAccountDetails(user_record);
		write(sockfd,&flag,sizeof(flag));
		write(sockfd, &user_record.balance, sizeof(user_record.balance));
	}
	displayMenu(sockfd,user_record);
}
void jointWithdraw(int sockfd, struct jointAccountDetails user_record,struct flock lock, int fd)
{
	int amount, flag=0;
	read(sockfd, &amount, sizeof(amount));
	if(amount>=user_record.balance)
	{
		write(sockfd,&flag,sizeof(flag));
		write(sockfd, &user_record.balance, sizeof(user_record.balance));
	}
	else
	{
		flag=1;
		user_record.balance = user_record.balance - amount;
		findJointAccountDetails(user_record);
		write(sockfd,&flag,sizeof(flag));
		write(sockfd, &user_record.balance, sizeof(user_record.balance));
	}
	jointDisplayMenu(sockfd,user_record,lock,fd);
}

void passwordChange(int sockfd, struct accountDetails user_record)
{
	char password[20];
	read(sockfd,password,sizeof(password));
	strcpy(user_record.password,password);
	findAccountDetails(user_record);
	mainMenu(sockfd);
}
void jointPasswordChange(int sockfd, struct jointAccountDetails user_record,struct flock lock)
{
	char password[20];
	read(sockfd,password,sizeof(password));
    if(!strcmp(cur_user,user_record.username1))		strcpy(user_record.password1,password);
	else		strcpy(user_record.password2,password);
	findJointAccountDetails(user_record);
	mainMenu(sockfd);
}

void deleteAccount(int sockfd)
{
	char username[20];
	int fd1, fd2;
	char nae[]="No Account Exists with the given Username.\n";
	char ad[]="Account deleted Successfully\n";
	fd1 = open("Accounts.txt", O_CREAT | O_RDWR , 0666);
	fd2 = open("Accounts2.txt", O_CREAT | O_RDWR, 0666);
	read(sockfd, username, sizeof(username));
	if(!checkAccountExists(username))							//If the Account doesn't exist, we are displaying the appropriate message.
		write(sockfd, nae, sizeof(nae));		
	else											//If the Account exists, we proceed to delete that record.
	{	
		struct accountDetails user_record[1000], user_record1;
		int i=0;
		lseek(fd1, 0, SEEK_SET);							//Setting the file offset of Accounts.txt to the starting of the file.
		while(read(fd1, (char *)&user_record[i++], sizeof(struct accountDetails)));	//Going through the Accounts.txt file till we reach EOF.
		for(int j=0;j<i-1;j++)								//Iterating from 0 to (No. of records present in Accounts.txt file)-1
		{
			if(!strcmp(user_record[j].username, username))				//If the suer_record[j] is the account to be deleted,then we will do nothing and continue with the loop.
				continue;				
			else									//Else we write all those records into the Accounts2.txt file
				write(fd2,(char *)&user_record[j],sizeof(struct accountDetails));
		}
		lseek(fd2, 0, SEEK_SET);							//Setting the file offset of Accounts2.txt to the starting of the file.
		fd1=open("Accounts.txt",O_TRUNC|O_RDWR,0666);					//Deleting all the records present in the Accounts.txt file by opening it in O_TRUNC mode.
		lseek(fd1, 0, SEEK_SET);							//Setting the file offset of Accounts.txt to the starting of the file.
		while(read(fd2, (char *)&user_record1, sizeof(struct accountDetails)))
		{
			write(fd1,(char *)&user_record1,sizeof(struct accountDetails));		//Writing all the records written in Accounts2.txt back to Accounts.txt
		}
		lseek(fd1, 0, SEEK_SET);							//Setting the file offset of Accounts.txt to the starting of the file.
		write(sockfd, ad, sizeof(ad));							//Sending Account Deleted message to client.
		open("Accounts2.txt",O_TRUNC|O_RDWR,0666);					//Deleting all the records present in the Accounts2.txt file by opening it in O_TRUNC mode. 
	}
	close(fd1);
	close(fd2);
	admin_functions(sockfd);
}
void deleteJointAccount(int sockfd)
{
	char username1[20], username2[20];
	int fd1, fd2;
	char nae[]="No Account Exists with the given Username.\n";
	char ad[]="Account deleted Successfully\n";
	fd1 = open("Joint_Account.txt", O_CREAT | O_RDWR , 0666);
	fd2 = open("Joint_Account2.txt", O_CREAT | O_RDWR, 0666);
	read(sockfd, username1, sizeof(username1));
	read(sockfd, username2, sizeof(username2));
	if(!checkJointAccountExists(username1,username2))
		write(sockfd, nae, sizeof(nae));
	else
	{	
		struct jointAccountDetails user_record[1000], user_record1;
		int i=0;
		lseek(fd1, 0, SEEK_SET);
		while(read(fd1, (char *)&user_record[i++], sizeof(struct jointAccountDetails)));
		for(int j=0;j<i-1;j++)
		{	
			if((!strcmp(user_record[j].username1, username1)) && (!strcmp(user_record[j].username2,username2)) || (!strcmp(user_record[j].username1, username2)) && (!strcmp(user_record[j].username2,username1)))
				continue;
			else
				write(fd2,(char *)&user_record[j],sizeof(struct jointAccountDetails));
		}
		lseek(fd2, 0, SEEK_SET);
		fd1=open("Joint_Account.txt",O_TRUNC|O_RDWR,0666);
		lseek(fd1, 0, SEEK_SET);
		while(read(fd2, (char *)&user_record1, sizeof(struct jointAccountDetails)))
		{
			write(fd1,(char *)&user_record1,sizeof(struct jointAccountDetails));
		}
		lseek(fd1,0, SEEK_SET);
		write(sockfd, ad, sizeof(ad));
		open("Joint_Account2.txt",O_TRUNC|O_RDWR,0666);
	}
	close(fd1);
	close(fd2);
	admin_functions(sockfd);
}

void admin_functions(int sockfd)
{
	int choice;
	char username[20], password[20], username1[20], username2[20];
	read(sockfd, &choice, sizeof(choice));
	char ae[]="Account Exists.\n";
	char nae[]="No Account Exists with the given Username.\n";
	switch(choice)
	{
		case 1:	accountSignup(sockfd,1);					break;	//1 means the user is an Admin
		case 2:	jointAccountSignup(sockfd,1);					break;	//1 means the user is an Admin
		case 3: deleteAccount(sockfd);						break;
		case 4:	deleteJointAccount(sockfd);	     				break;
		case 5:	read(sockfd, username, sizeof(username));
				if(checkAccountExists(username))	
					write(sockfd, ae, sizeof(ae));
				else	
					write(sockfd, nae, sizeof(nae));
				admin_functions(sockfd);
				break;
		case 6:	read(sockfd, username1, sizeof(username1));
				read(sockfd, username2, sizeof(username2));
				if(checkJointAccountExists(username1,username2))	
					write(sockfd, ae, sizeof(ae));
				else	
					write(sockfd, nae, sizeof(nae));
				admin_functions(sockfd);
				break;
		default:mainMenu(sockfd);
	}
}
void adminLogin(int sockfd)
{
	char username[20], password[20];
	int flag=0;
	read(sockfd, username, sizeof(username));
	read(sockfd, password, sizeof(password));
	char wa[]="\nWelcome Admin !!!\n";
	char ic[]="\nInvalid Credentials !!! Try again.\n";
	if(!strcmp(username,"admin") && !strcmp(password,"password"))
	{	
		write(sockfd, &flag, sizeof(flag));
		write(sockfd, wa, sizeof(wa));
		admin_functions(sockfd);
	}
	else
	{	
		flag=1;
		write(sockfd, &flag, sizeof(flag));
		write(sockfd, ic, sizeof(ic));	
		mainMenu(sockfd);
	}
}

void displayMenu(int sockfd, struct accountDetails user_record)
{
	int choice;
	read(sockfd, &choice, sizeof(choice));
	switch(choice)
	{
		case 1: viewAccountDetails(sockfd,user_record);				break;
		case 2: deposit(sockfd,user_record);					break;
		case 3: withdraw(sockfd,user_record);					break;
		case 4: passwordChange(sockfd,user_record);				break;
		case 5: mainMenu(sockfd);						break;
	}
}
void jointDisplayMenu(int sockfd,struct jointAccountDetails user_record,struct flock lock,int fd)
{
	int choice;
	read(sockfd, &choice, sizeof(choice));
	switch(choice)
	{
		case 1: viewJointAccountDetails(sockfd,user_record,lock,fd);		break;
		case 2: jointDeposit(sockfd,user_record,lock,fd);			break;
		case 3: jointWithdraw(sockfd,user_record,lock,fd);			break;
		case 4: jointPasswordChange(sockfd,user_record,lock);			break;
		case 5: lock.l_type=F_UNLCK;
			fcntl(fd,F_SETLK,&lock); 
			mainMenu(sockfd);
			break;
	}
}

void mainMenu(int sockfd)
{
	int choice,fd;
	read(sockfd, &choice, sizeof(choice));
	switch(choice)
	{
		case 1: accountSignup(sockfd,0);					break;	//0 means the user is NOT an Admin
		case 2: accountLogin(sockfd);						break;
		case 3: jointAccountSignup(sockfd,0);					break;	//0 means the user is NOT an Admin
		case 4: jointAccountLogin(sockfd,fd);					break;
		case 5: adminLogin(sockfd);						break;
		case 6: exit(0);							break;
	}
}
int main(int argc,char *argv[])
{
	struct sockaddr_in server, client;
	int sockfd, client_len, new_socket;
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(3456);
	if(bind(sockfd,(struct sockaddr *)(&server), sizeof(server))<0)
		printf("Bind Error.\n");
	listen(sockfd,10);
	client_len = sizeof(client);
	while(1)
	{
		new_socket = accept(sockfd, (struct sockaddr *)&client, &client_len);
		if(!fork())
		{
			close(sockfd);
			mainMenu(new_socket);
			exit(0);
		}
		else		close(new_socket);
	}
	close(new_socket);
	close(sockfd);
	return 0;
}
