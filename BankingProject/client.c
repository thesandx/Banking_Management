#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include<sys/stat.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include<error.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include <stdbool.h>
 #include <sys/stat.h>
#include <fcntl.h>
#define MAX 50
#include "transection.c"
#include "account.c"
#include "customer.c"

int socket_desc;

void loginmenu();
void addAccount();
void deleteAccount();
void modifyAccount();
void searchAccount();

void adminMenu();

void userMenu();

void addMoney();
void withdrawMoney();
void accountDetails();
void changePassword();
void checkBalance();




void addMoney(struct customer c){

printf("Enter the amount you want to add\n");
double amt;
scanf("%lf",&amt);
if(amt<0){
  printf("Amount must be positive\n");
  write(socket_desc,c.accountNumber,MAX);
  write(socket_desc,&amt,sizeof(double));
}
else{
  write(socket_desc,c.accountNumber,MAX);
  write(socket_desc,&amt,sizeof(double));
  int res;
  read(socket_desc,&res,sizeof(int));
  if(res==1){
    printf("Amount added successfully\n");
  }
  else{
    printf("Somethin went wrong\n");
  }
}

printf("Press any key to go back to Management Portal\n");
  char e;
  read(0,&e,sizeof(char));
  system("clear");
  userMenu(c);
}

void accountDetails(struct customer c){

   write(socket_desc,&c,sizeof(struct customer));
  
  int res;
  read(socket_desc,&res,sizeof(int));
  if(res==1){
    struct account a; 
    struct transection ptr[10];
    read(socket_desc,&a,sizeof(struct account));
    read(socket_desc,ptr,10*sizeof(struct transection));

    printf("account number is %s\n",a.accountNumber);
    printf("Balance is %0.2lf\n",a.balance);
    printf("your recent transection is as follows\n\n");
    int i;
    for(i=0;i<10;i++){
      struct transection tran;
      tran = ptr[i];
      if(&tran==NULL || tran.amount==0){
        break;
      }
      else{
        printf("Date- %s\n",tran.date);
        printf("Action- %s\n",tran.action==0 ?"Add" : "Withdraw");
        printf("Amount-%lf\n",tran.amount);
        printf("New balance %0.2lf\n\n\n",tran.newbalance);

      }


    }
  }
  else{
    printf("Somethin went wrong\n");
  }

  printf("Press any key to go back to Management Portal\n");
  char e;
  read(0,&e,sizeof(char));
  system("clear");
  userMenu(c);
}

void changePassword(struct customer c){

  printf("Enter new password\n");
  char new_pass[MAX];
  scanf("%s",new_pass);

  write(socket_desc,new_pass,MAX);
  write(socket_desc,c.username,MAX);



  int res;
  read(socket_desc,&res,sizeof(int));
  if(res==1){
    printf("Password changed successfully\n");
  }
  else{
    printf("Somethin went wrong\n");
  }


  printf("Press any key to go back to Management Portal\n");
  char e;
  read(0,&e,sizeof(char));
  system("clear");
  userMenu(c);
}

void checkBalance(struct customer c){

  write(socket_desc,c.accountNumber,MAX);
  
  int res;
  read(socket_desc,&res,sizeof(int));
  if(res==1){
    double balance;
    read(socket_desc,&balance,sizeof(double));
    printf("Balance is %0.2lf\n",balance);
  }
  else{
    printf("Somethin went wrong\n");
  }



  printf("Press any key to go back to Management Portal\n");
  char e;
  read(0,&e,sizeof(char));
  system("clear");
  userMenu(c);
}

void withdrawMoney(struct customer c){
  printf("Enter the amount you want to withdraw\n");
 double amt;
scanf("%lf",&amt);
if(amt<0){
  printf("Amount must be positive\n");
   write(socket_desc,c.accountNumber,MAX);
   write(socket_desc,&amt,sizeof(double));
}
else{
   write(socket_desc,c.accountNumber,MAX);
  write(socket_desc,&amt,sizeof(double));
  int res;
  read(socket_desc,&res,sizeof(int));
  if(res==1){
    printf("Amount withdrawn successfully\n");
    printf("Please collect your cash\n");
  }
  else{
    printf("Not sufficient balance\n");
  }
}


  printf("Press any key to go back to Management Portal\n");
  char e;
  read(0,&e,sizeof(char));
  system("clear");
  userMenu(c);
}



void userMenu(struct customer c){

  printf("Welcome to User Account Management Portal\n");
  printf("1.Deposit Money\n");
	printf("2.Withdraw money\n");
	printf("3.Check Balance\n");
	printf("4.Change Password\n");
  printf("5.Account Details\n");
	printf("6.Logout\n");
  printf("Choose one of the options:\n");
  int res;
  scanf("%d",&res);
  fflush(stdin);
  write(socket_desc,&res,sizeof(int));
  system("clear");

  switch(res){
    case 1:
      addMoney(c);
      break;
    case 2:
      withdrawMoney(c);
      break;
    case 3:
      checkBalance(c);
      break;
    case 4:
      changePassword(c);
      break;
    case 5:
      accountDetails(c);
    default:
      printf("successfully logout\n");
      loginmenu();
  }

}


void searchAccount(struct customer c){
   char username[MAX];
   printf("Enter Username:");
	  scanf("%s",username);
    fflush(stdin);

    write(socket_desc,username,MAX);
    int res;


   read(socket_desc,&res,sizeof(int));
   fflush(stdin);
	if(res==0){
		printf("No account found\n");
    
  }
  else{
		printf("account found\n");

    struct customer c1;
    struct account a;
    read(socket_desc,&c1,sizeof(struct customer));
    read(socket_desc,&a,sizeof(struct account)); 
    
    printf("Account details is following\n");
    printf("username -  %s\n",c1.username);
    printf("account type - %s\n",(int)c1.type==1?"Single":"Joint");
    printf("Accoung number -  %s\n",a.accountNumber);
    printf("Balance - %0.2lf\n",a.balance);

  }
  printf("Press any key to go back to Management Portal\n");
  char e;
  read(0,&e,sizeof(char));
  system("clear");
  adminMenu(c);



}

void deleteAccount(struct customer c){
  char username[MAX];
   printf("Enter Username:");
	  scanf("%s",username);
    fflush(stdin);

    write(socket_desc,username,MAX);
    int res;


   read(socket_desc,&res,sizeof(int));
   fflush(stdin);
	if(res==0){
		printf("No account found\n");   
  }
  else{
    printf("Account deleted\n");

  }
  printf("Press any key to go back to Management Portal\n");
  char e;
  read(0,&e,sizeof(char));
  system("clear");
  adminMenu(c);

}

void modifyAccount(struct customer c){
  printf("Choose Account type\n\n");
  printf("1.Change Password\n");
  printf("2.convert single account to joint Account\n");

  int acc;
  scanf("%d",&acc);

  write(socket_desc,&acc,sizeof(int));

  if(acc == 1){

    char username[MAX];
    char password[MAX];
   int type;
	

    printf("Enter Username:");
	  scanf("%s",username);
    fflush(stdin);
    printf("Enter new Password:");
	  scanf("%s",password);
    fflush(stdin);

    write(socket_desc,username,MAX);
    write(socket_desc,password,MAX);
    int res;
    read(socket_desc,&res,sizeof(int));
    if(res==0){
      printf("No user found\n");
    }
    else{
      printf("Password changed successfully\n");

    }

  adminMenu(c);

  }
  else if(acc==2){
      char user[MAX];
      char username[MAX];
      char password[MAX];
      printf("Enter username of account which you want to convert\n");
      scanf("%s",user);
      write(socket_desc,user,MAX);
      int res;
      read(socket_desc,&res,sizeof(int));
      if(res==0){
        printf("Invalid username\n");
        adminMenu(c);
      }
      else{
        printf("User found\n");
        printf("Enter username of new user\n");
        scanf("%s",username);
        printf("Enter password of new user\n");
        scanf("%s",password);

        write(socket_desc,username,MAX);
        write(socket_desc,password,MAX);

        int res1;
        read(socket_desc,&res1,sizeof(int));
        if(res1==0){

            printf("Something went wrong\n");
            
        }
        else{
          printf("Converted to joint account successfully\n");

        }
        adminMenu(c);
      }
  }
  else{
    printf("invalid choice");
    adminMenu(c);
  }
  


}



void addAccount(struct customer c){

  printf("Choose Account type\n\n");
  printf("1.Single Account\n");
  printf("2.joint Account\n");
  
  int acc;
  scanf("%d",&acc);

  write(socket_desc,&acc,sizeof(int));

  if(acc==1){
  char username[MAX];
  char password[MAX];
  int type;
	

    printf("Enter Username:");
	  scanf("%s",username);
    fflush(stdin);
    printf("Enter Password:");
	  scanf("%s",password);
    fflush(stdin);
    // printf("Enter Account type\n");
    // printf("0-Admin/1-normal");
    // scanf("%d",&type);

    write(socket_desc,username,MAX);
    write(socket_desc,password,MAX);
   // write(socket_desc,&type,sizeof(int));
  }
  else if(acc==2){

  char username1[MAX];
  char password1[MAX];
  char username2[MAX];
  char password2[MAX];
  int type;
	

    printf("Enter Username1:");
	  scanf("%s",username1);
    fflush(stdin);
    printf("Enter Password1:");
	  scanf("%s",password1);
    fflush(stdin);

    printf("Enter Username2:");
	  scanf("%s",username2);
    fflush(stdin);
    printf("Enter Password2:");
	  scanf("%s",password2);
    fflush(stdin);
    // printf("Enter Account type\n");
    // printf("0-Admin/1-normal");
    // scanf("%d",&type);

    write(socket_desc,username1,MAX);
    write(socket_desc,password1,MAX);
    write(socket_desc,username2,MAX);
    write(socket_desc,password2,MAX);


  }
  else{
    printf("Invalid choice\n");\
    adminMenu(c);
  }

   int res;


   read(socket_desc,&res,sizeof(int));
   fflush(stdin);
	if(res==0){
		printf("username already exists\n");
    
  }
  else{
    printf("Account created\n");

  }
  printf("Press any key to go back to Management Portal\n");
  char e;
  read(0,&e,sizeof(char));
  system("clear");
  adminMenu(c);
}

void adminMenu(struct customer c){
 printf("welcome %s(Admin)\n",c.username);
 printf("Welcome to Admin Account Management Portal\n");
  printf("1.Add Account\n");
	printf("2.Delete Account\n");
	printf("3.Modify Account\n");
	printf("4.Search Account\n");
	printf("5.Logout\n\n");
  printf("Choose one of the options:\n");
  int res;
  scanf("%d",&res);
  fflush(stdin);
  write(socket_desc,&res,sizeof(int));
  system("clear");

  switch(res){
    case 1:
      addAccount(c);
      break;
    case 2:
      deleteAccount(c);
      break;
    case 3:
     modifyAccount(c);
      break;
    case 4:
      searchAccount(c);
      break;
    default:
      printf("successfully logout\n");
      loginmenu();
  }




}


void loginmenu(){

    char username[MAX];
    char password[MAX];
	
	//printf(".Exit\n\n");

    printf("Enter username and password\n");
    printf("Enter Username:");
	  scanf("%s",username);
    //printf("Value of username is %s",username);
    fflush(stdin);
    printf("Enter Password:");
	  scanf("%s",password);
   // printf("Value of password is %s",password);
    fflush(stdin);

    write(socket_desc,username,MAX);
    write(socket_desc,password,MAX);

   int res;
	 read(socket_desc,&res,sizeof(int));
   fflush(stdin);
	if(res==0){
		printf("Invalid Login Details\n");
    
  }
  else{
    printf("Login successful\n");
    struct customer c;

    read(socket_desc,&c,sizeof(struct customer));

    if(c.type!=0){
      printf("welcome %s\n",c.username);
      userMenu(c);

    }
    else{
     adminMenu(c);

    }

  }



    
	

}



int main(int argc,char const *argv[]){


struct sockaddr_in server;
char buff[1024];

socket_desc = socket(AF_INET, SOCK_STREAM, 0);
if(socket_desc==-1)
  {
    perror("Error :");
    return 0;
  }
server.sin_family = AF_INET;
server.sin_addr.s_addr = INADDR_ANY;
server.sin_port =  htons(atoi(argv[1]));
if(connect(socket_desc, (void *)(&server), sizeof(server))==-1){
		perror("Error :");
        return 0;
	}
printf("Connection  Successfull\n\n\n");
  system("clear");
  printf("------------------------------------------------------\n");
  printf("---------------Banking Management System-------------\n");
  printf("------------------------------------------------------\n");
  printf("\n");
  printf("\n");
  printf("\n");
  
    loginmenu();

// do you client stuff here
//  write(socket_desc,"Hello server",13);
//     read(socket_desc,buff,sizeof(buff));

//     printf("Message from server is %s /n :",buff);


close(socket_desc);
return 0;

}