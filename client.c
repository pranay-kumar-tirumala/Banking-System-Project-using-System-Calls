#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<arpa/inet.h>
void mainMenu(int);
void displayMenu(int);
void jointDisplayMenu(int);
void admin_functions(int);
//In C language, we get a warning if we call a function before declaring it. So to avoid warnings, we added the corresponding function prototypes above.

void accountSignup(int sockfd, int flag)
{
	int account_id;
	char username[20],password[20], buffer[51];
	printf("\n*************************************** WELCOME TO NORMAL ACCOUNT SIGN-UP PAGE ****************************************\n");
	printf("Enter username: ");
		scanf(" %s",username);
	printf("Enter password: ");
		scanf(" %s",password);
	write(sockfd, username,sizeof(username));
	write(sockfd, password, sizeof(password));
	read(sockfd, buffer, sizeof(buffer));
	printf("%s", buffer);
	if(flag==1)		mainMenu(sockfd);
	else		admin_functions(sockfd);
}
void jointAccountSignup(int sockfd, int flag)
{
	char username1[20], password1[20], username2[20], password2[20], buffer[51];
	printf("\n**************************************** WELCOME TO JOINT ACCOUNT SIGN-UP PAGE ****************************************\n");
	printf("Enter first username: ");
		scanf(" %s", username1);
	printf("Enter password: ");
		scanf(" %s", password1);
	printf("Enter second username: ");
		scanf(" %s", username2);
	printf("Enter password: ");
		scanf(" %s", password2);
	write(sockfd, username1, sizeof(username1));
	write(sockfd, password1, sizeof(password1));
	write(sockfd, username2, sizeof(username2));
	write(sockfd, password2, sizeof(password2));
	read(sockfd, buffer, sizeof(buffer));
	printf("%s", buffer);
	if(flag==1)		mainMenu(sockfd);
	else		admin_functions(sockfd);
}

void accountLogin(int sockfd)
{
	char buffer[33];
	char username[20],password[20];
	int flag;
	printf("\n**************************************** WELCOME TO LOGIN PAGE ****************************************\n");
	printf("Enter your username : ");
		scanf(" %s",username);
	printf("Enter your password : ");
		scanf(" %s",password);
	write(sockfd, username,sizeof(username));
	write(sockfd, password, sizeof(password));
	read(sockfd, &flag, sizeof(flag));
	read(sockfd, buffer, sizeof(buffer));
	printf("%s",buffer);
	if(flag==1)			displayMenu(sockfd);	//NOTE that, If flag is 1, then it means customer was able to login successfully.
	else if(flag==0)		mainMenu(sockfd);	//else if flag is 0, then it means customer has entered incorrect credentials.
}
void jointAccountLogin(int sockfd)
{
	char buffer[30];
	char username1[20],password[20],username2[20];
	int flag;
	printf("\n**************************************** WELCOME TO JOINT ACCOUNT LOGIN PAGE ****************************************\n");
	printf("Enter first username : ");
		scanf(" %s",username1);
	printf("Enter second username : ");
		scanf(" %s",username2);
	printf("Enter password for first user : ");
		scanf(" %s",password);
	write(sockfd, username1,sizeof(username1));
	write(sockfd, username2,sizeof(username2));
	write(sockfd, password, sizeof(password));
	read(sockfd, &flag, sizeof(flag));
	read(sockfd, buffer, sizeof(buffer));
	printf("%s",buffer);
	if(flag==1)		jointDisplayMenu(sockfd);
	else		mainMenu(sockfd);
}

void viewAccountDetails(int sockfd)
{
	char username[20];
	int acc_id, balance;
	read(sockfd,username, sizeof(username));
	read(sockfd,&acc_id, sizeof(acc_id));
	read(sockfd,&balance, sizeof(balance));
	printf("\nYour Account Details :\nUsername: %s\tAccount ID: %d\tBalance: Rs.%d\n",username,acc_id,balance);
	displayMenu(sockfd);
}
void viewJointAccountDetails(int sockfd)
{
	char username1[20], username2[20];
	int acc_id, balance;
	read(sockfd,username1, sizeof(username1));
	read(sockfd,username2, sizeof(username2));
	read(sockfd,&acc_id, sizeof(acc_id));
	read(sockfd,&balance, sizeof(balance));
	printf("\nYour Account Details :\nUsername1: %s\tUsername2: %s\tAccount ID: %d\tBalance: Rs.%d\n",username1,username2,acc_id,balance);
	jointDisplayMenu(sockfd);
}

void deposit(int sockfd)
{
	int amount;
	printf("\nEnter amount to deposit: ");
		scanf("%d",&amount);
	printf("\nRs.%d deposited successfully !!!\n",amount);
	write(sockfd, &amount, sizeof(amount));
	read(sockfd, &amount, sizeof(amount));
	printf("\nUpdated Balance : %d\n",amount);
	displayMenu(sockfd);
}
void jointDeposit(int sockfd)
{
	int amount;
	printf("\nEnter amount to deposit: ");
		scanf("%d",&amount);
	printf("Rs.%d deposited successfully !!!\n",amount);
	write(sockfd, &amount, sizeof(amount));
	read(sockfd, &amount, sizeof(amount));
	printf("Updated Balance : %d",amount);
	jointDisplayMenu(sockfd);
}

void withdraw(int sockfd)
{
	int amount,flag;
	printf("\nEnter amount to withdraw: ");
		scanf("%d",&amount);
	write(sockfd, &amount, sizeof(amount));
	read(sockfd, &flag, sizeof(flag));
	if(flag==0)
	{
		printf("\nSorry !!! Insufficient Balance.\n");
		read(sockfd, &amount, sizeof(amount));
		printf("Current Balance: %d",amount);
	}
	else
	{
		printf("Rs.%d withdrawn Successfully !!!\n",amount);
		read(sockfd, &amount, sizeof(amount));
		printf("Updated Balance: %d\n",amount);
	}
	displayMenu(sockfd);
}
void jointWithdraw(int sockfd)
{
	int amount,flag;
	printf("\nEnter amount to withdraw: ");
		scanf("%d",&amount);
	write(sockfd, &amount, sizeof(amount));
	read(sockfd, &flag, sizeof(flag));
	if(flag==0)
	{
		printf("\nSorry !!! Insufficient Balance.\n");
		read(sockfd, &amount, sizeof(amount));
		printf("Current Balance : %d",amount);
	}
	else
	{	
		printf("Rs.%d withdrawn Successfully !!!\n",amount);
		read(sockfd, &amount, sizeof(amount));
		printf("Updated Balance : %d\n",amount);
	}
	jointDisplayMenu(sockfd);
}

void passwordChange(int sockfd)
{
	char password[20];
	printf("\nEnter the new password: ");
		scanf(" %s",password);
	write(sockfd,password,sizeof(password));
	printf("\nPassword Updated Successfully.\n");
	mainMenu(sockfd);	
}
void jointPasswordChange(int sockfd)
{
	char password[20];
	printf("\nEnter the new password: ");
		scanf(" %s",password);
	write(sockfd,password,sizeof(password));
	printf("\nPassword Updated Successfully.\n");
	mainMenu(sockfd);
}

void deleteAccount(int sockfd)
{
	char username[20],buffer[35];
	printf("\nEnter the username to be deleted: ");
		scanf(" %s", username);
	write(sockfd, username, sizeof(username));
	read(sockfd, buffer, sizeof(buffer));
	printf("%s",buffer);
	admin_functions(sockfd);
}
void deleteJointAccount(int sockfd)
{
	char username1[20], username2[20], buffer[35];
	printf("Enter Username 1 to delete: ");
		scanf(" %s",username1);
	printf("Enter Username 2 to delete: ");
		scanf(" %s",username2);
	write(sockfd, username1, sizeof(username1));
	write(sockfd, username2, sizeof(username2));
	read(sockfd, buffer, sizeof(buffer));
	printf("%s",buffer);
	admin_functions(sockfd);
}

void admin_functions(int sockfd)
{
	int choice;
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~You have access to perform the following operations:~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
	printf("\n1.Create an Account\n2.Create a Joint Account\n3.Delete an Account\n4.Delete a Joint Account\n5.Search an Account\n6.Search a Joint Account\n7.Logout\n\nEnter your choice: ");
		scanf("%d", &choice);
	char buffer[46], username[20], password[20], username1[20], username2[20];
	write(sockfd, &choice, sizeof(choice));
	switch(choice)
	{
		case 1:	accountSignup(sockfd,1);					break;	//1 means the user is an Admin
		case 2:	jointAccountSignup(sockfd,1);					break;	//1 means the user is an Admin
		case 3:	deleteAccount(sockfd);						break;
		case 4:	deleteJointAccount(sockfd);					break;
		case 5:	printf("\nEnter username to search: ");
					scanf(" %s", username);
				write(sockfd, username, sizeof(username));
				read(sockfd, &buffer, sizeof(buffer));
				printf(" %s", buffer);
				admin_functions(sockfd);
				break;
		case 6:	printf("\nEnter username 1 to search: ");
					scanf(" %s", username1);
				printf("\nEnter username 2 to search: ");
					scanf(" %s", username2);
				write(sockfd, username1, sizeof(username1));
				write(sockfd, username2, sizeof(username2));
				read(sockfd, &buffer, sizeof(buffer));
				printf("%s", buffer);
				admin_functions(sockfd);
				break;
		case 7:	mainMenu(sockfd);
	}
}
void adminLogin(int sockfd)
{
	char username[20], password[20], buffer[20];
	int flag;
	printf("\n**************************************** WELCOME TO ADMIN LOGIN PAGE ****************************************\n");
	printf("Enter admin username: ");
		scanf(" %s",username);
	printf("Enter admin password: ");
		scanf(" %s",password);
	write(sockfd, username, sizeof(username));
	write(sockfd, password, sizeof(password));
	read(sockfd, &flag, sizeof(flag));
	read(sockfd, buffer, sizeof(buffer));
	
	printf("%s",buffer);
	if(flag == 0)			admin_functions(sockfd);
	else if(flag==1)		mainMenu(sockfd);
}

void displayMenu(int sockfd)
{
	int choice;
	printf("----------------------------------------------------------------------------------------------------------------");
	printf("\n1.VIEW MY ACCOUNT DETAILS\n2.DEPOSIT MONEY\n3.WITHDRAW MONEY\n4.CHANGE PASSWORD\n5.EXIT\n\nEnter your choice: ");
		scanf("%d",&choice);
	write(sockfd, &choice, sizeof(choice));
	switch(choice)
	{
		case 1: viewAccountDetails(sockfd);					break;
		case 2: deposit(sockfd);							break;
		case 3: withdraw(sockfd);							break;
		case 4: passwordChange(sockfd);						break;
		case 5: mainMenu(sockfd);							break;
		default: printf("Please enter proper choice:\n");	break;
	}
}
void jointDisplayMenu(int sockfd)
{
	int choice;
	printf("----------------------------------------------------------------------------------------------------------------");
	printf("\n1.VIEW MY ACCOUNT DETAILS\n2.DEPOSIT MONEY\n3.WITHDRAW MONEY\n4.CHANGE PASSWORD\n5.EXIT\n\nEnter your choice: ");
		scanf("%d",&choice);
	write(sockfd, &choice, sizeof(choice));
	switch(choice)
	{
		case 1: viewJointAccountDetails(sockfd);				break;
		case 2: jointDeposit(sockfd);						break;
		case 3: jointWithdraw(sockfd);						break;
		case 4: jointPasswordChange(sockfd);					break;
		case 5: mainMenu(sockfd);						break;
	}
}

void mainMenu(int sockfd)
{
	int choice;
	printf("**************************************** WELCOME TO ONLINE BANKING HOME PAGE ****************************************\n");
	printf("\n1.CREATE A NEW ACCOUNT\n2.LOGIN TO YOUR ACCOUNT\n3.CREATE A NEW JOINT ACCOUNT\n4.LOGIN TO YOUR JOINT ACCOUNT\n5.ADMIN LOGIN\n6.EXIT\n\nEnter your choice: ");
		scanf("%d",&choice);
	write(sockfd, &choice, sizeof(choice));
	switch(choice)
	{
		case 1: accountSignup(sockfd,1);					break;	//1 means the user is not an Admin
		case 2: accountLogin(sockfd);						break;
		case 3: jointAccountSignup(sockfd,1);					break;	//1 means the user is not an Admin
		case 4: jointAccountLogin(sockfd);					break;
		case 5: adminLogin(sockfd);						break;
		case 6: exit(0);							break;
	}
}
int main(int argc,char *argv[])
{	
	struct sockaddr_in server;
	int sockfd;		//socket descriptor
	char buffer[80];
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(3456);
	connect(sockfd, (struct sockaddr *)(&server), sizeof(server));
	mainMenu(sockfd);
	close(sockfd);
	return 0;
}
