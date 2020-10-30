#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include<sys/types.h>
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
#include<time.h>


#define MAX 50
#include "transection.c"
#include "account.c"
#include "customer.c"



int socket_desc, size_client, client_desc;
int customer_fd;

void login();
void addAccount();
void adminmanage();
void searchAccount();
void modifyAccount();
void deleteAccount();
void userManage();

void addMoney();
void withdrawMoney();
void accountDetails();
void changePassword();
void checkBalance();

void changePassword(){
    printf("inside change password\n");
    char password[MAX];
    char username[MAX];
    read(client_desc,password,MAX);
     read(client_desc,username,MAX);
   int res = 0;
   struct customer c;

  //customer_fd =open("customer_db",O_CREAT|O_RDWR,S_IRWXU);

  lseek(customer_fd,0,SEEK_SET);

 while(read(customer_fd,&c,sizeof(struct customer))>0){
     //printf("%s %s",c.username,c.password);
     //printf("value of admin match\n  = %d",strcmp(admin_usr,c.username)==0);
     // printf("value of pass match\n  = %d",strcmp(admin_pwd,c.password)==0);
    if(strcmp(username,c.username)==0){
      res=1;
      printf("user found\n");
      break;
    }
  }

  if(res==0){
      write(client_desc,&res,sizeof(int)); 
  }
  else{
      //change password.
    strcpy(c.password,password);
    lseek(customer_fd,-1*sizeof(struct customer),SEEK_CUR);
    write(customer_fd,&c,sizeof(struct customer));
    write(client_desc,&res,sizeof(int)); 
    printf("password changed\n");
  }
userManage();


}

void accountDetails(){
   printf("inside account details\n");
   struct customer c;
   read(client_desc,&c,sizeof(struct customer));

   //search the account number
    struct account a;
    int account_fd =open("account_db",O_CREAT|O_RDWR,S_IRWXU);
    lseek(account_fd,0,SEEK_SET);
    int res = 0;
    while(read(account_fd,&a,sizeof(struct account))>0){
     //printf("%s %s",c.username,c.password);
     //printf("value of admin match\n  = %d",strcmp(admin_usr,c.username)==0);
     // printf("value of pass match\n  = %d",strcmp(admin_pwd,c.password)==0);
    if(strcmp(c.accountNumber,a.accountNumber)==0){
      res=1;
      printf("account mil gaya\n");
      break;
    }
  }

  write(client_desc,&res,sizeof(int));
  if(res == 1){ 
    write(client_desc,&a,sizeof(struct account));
    printf("amount is %lf\n",a.balance);
    close(account_fd);
    //find transections

    struct transection ptr[10];
    //There is no way to know the size of a dynamically allocated array, 
    //you have to save it somewhere else. 
    int count=0;

     //search the account number
    struct transection tran;
    int tran_fd =open("transection_db",O_CREAT|O_RDWR,S_IRWXU);
    lseek(tran_fd,0,SEEK_SET);
    while(read(tran_fd,&tran,sizeof(struct transection))>0){
      printf("Date- %s\n",tran.date);
        printf("Action %s\n",tran.action==0 ?"Add" : "Withdraw");
        printf("Amount-%lf\n",tran.amount);
        printf("account num is-%s\n",tran.accountNumber);
        printf("New balance %0.2lf\n\n\n",tran.newbalance);
    if(strcmp(c.accountNumber,tran.accountNumber)==0){
      if(count<10){
        ptr[count] = tran;
        count++;
      printf("data verified,count number is %d\n",count);
      }
      else{
        printf("Break ho gaya,count valus is %d\n",count);
       break;
      }
    }
  }



  write(client_desc,ptr,10*sizeof(struct transection));
  close(tran_fd);
    
  }

  

  userManage();



}

void checkBalance(){
   printf("inside check balance\n");
   char accountnumber[MAX];
   read(client_desc,accountnumber,MAX);

   //search the account number
    struct account a;
    int account_fd =open("account_db",O_CREAT|O_RDWR,S_IRWXU);
    lseek(account_fd,0,SEEK_SET);
    int res = 0;
    while(read(account_fd,&a,sizeof(struct account))>0){
     //printf("%s %s",c.username,c.password);
     //printf("value of admin match\n  = %d",strcmp(admin_usr,c.username)==0);
     // printf("value of pass match\n  = %d",strcmp(admin_pwd,c.password)==0);
    if(strcmp(accountnumber,a.accountNumber)==0){
      res=1;
      printf("account mil gaya\n");
      break;
    }
  }

  write(client_desc,&res,sizeof(int));
  if(res == 1){ 
       int offset = sizeof(struct account);

  struct flock lock;
  lock.l_type=F_RDLCK; 
  lock.l_whence=SEEK_CUR; //seek_set par start se lag jayega.
  lock.l_start=offset;  
  lock.l_len=sizeof(struct account);
  printf("trying to get lock\n");
  fcntl(account_fd,F_SETLKW,&lock);//so it will wait for the lock,till it gets it.
  printf("lock mil gaya\n");
    write(client_desc,&a.balance,sizeof(double));
    printf("amount is %lf\n",a.balance);
    lock.l_type = F_UNLCK;
    fcntl(account_fd,F_SETLK,&lock);

    close(account_fd);
  }

  

  userManage();


}
void withdrawMoney(){
    printf("inside withdraw money\n");
    double amt;
    char accountnumber[MAX];
    read(client_desc,accountnumber,MAX);
    read(client_desc,&amt,sizeof(double));

    if(amt<0){
        userManage();
    }

    //search the account number
    struct account a;
    int account_fd =open("account_db",O_CREAT|O_RDWR,S_IRWXU);
    lseek(account_fd,0,SEEK_SET);
    int res = 0;
    while(read(account_fd,&a,sizeof(struct account))>0){
     //printf("%s %s",c.username,c.password);
     //printf("value of admin match\n  = %d",strcmp(admin_usr,c.username)==0);
     // printf("value of pass match\n  = %d",strcmp(admin_pwd,c.password)==0);
    if(strcmp(accountnumber,a.accountNumber)==0){
      res=1;
      printf("data verified\n");
      break;
    }
  }



  
  if(res == 1){
      int offset = sizeof(struct account);

  struct flock lock;
  lock.l_type=F_WRLCK; 
  lock.l_whence=SEEK_CUR; //seek_set par start se lag jayega.
  lock.l_start=offset;  
  lock.l_len=sizeof(struct account);
  printf("trying to get lock\n");
  fcntl(account_fd,F_SETLKW,&lock);//so it will wait for the lock,till it gets it.
  printf("lock mil gaya\n");
     // printf("amount going to be added is %lf\n",amt);
    a.balance  = a.balance-amt;
    if(a.balance<0){
        res=0;
    }
    else{
    lseek(account_fd,-1*sizeof(struct account),SEEK_CUR);
    write(account_fd,&a,sizeof(struct account)); 
    


   //add in transection
char text[100];
time_t now = time(NULL);
struct tm *t = localtime(&now);
strftime(text, sizeof(text)-1, "%d %m %Y %H:%M", t);
printf("Current Date: %s", text);

struct transection tran;

strcpy(tran.date,text);
strcpy(tran.accountNumber,accountnumber);
tran.action = 1;
tran.amount = amt;
tran.newbalance = a.balance;

int tran_fd = open("transection_db",O_CREAT|O_RDWR,S_IRWXU);
    lseek(tran_fd,0,SEEK_END);
    write(tran_fd,&tran,sizeof(struct transection));
    printf("added in transection\n");
    close(tran_fd);
    printf("amount withdrawn\n");
    }
    lock.l_type = F_UNLCK;
    fcntl(account_fd,F_SETLK,&lock);
     

     




    close(account_fd);

  }

  write(client_desc,&res,sizeof(int));

  userManage();
}


void addMoney(){
    printf("inside add money\n");
    double amt;
    char accountnumber[MAX];
    read(client_desc,accountnumber,MAX);
    read(client_desc,&amt,sizeof(double));

    if(amt<0){
        userManage();
    }

    //search the account number
    struct account a;
    int account_fd = open("account_db",O_CREAT|O_RDWR,S_IRWXU);
    lseek(account_fd,0,SEEK_SET);
    int res = 0;
    while(read(account_fd,&a,sizeof(struct account))>0){
    if(strcmp(accountnumber,a.accountNumber)==0){
      res=1;
      printf("account mil gaya\n");
      break;
    }
  }
  if(res == 1){

  int offset = sizeof(struct account);

  struct flock lock;
  lock.l_type=F_WRLCK; 
  lock.l_whence=SEEK_CUR; //seek_set par start se lag jayega.
  lock.l_start=offset;  
  lock.l_len=sizeof(struct account);
  printf("trying to get lock\n");
  fcntl(account_fd,F_SETLKW,&lock);//so it will wait for the lock,till it gets it.
  printf("lock mil gaya\n");

    a.balance  = a.balance+amt;
    lseek(account_fd,-1*sizeof(struct account),SEEK_CUR); //current location+offset
    write(account_fd,&a,sizeof(struct account)); 

    lock.l_type = F_UNLCK;
    fcntl(account_fd,F_SETLK,&lock);
    printf("amount added\n");

   

//add in transection
char text[100];
time_t now = time(NULL);
struct tm *t = localtime(&now);
strftime(text, sizeof(text)-1, "%d %m %Y %H:%M", t);
printf("Current Date: %s\n", text);

struct transection tran;

strcpy(tran.date,text);
strcpy(tran.accountNumber,accountnumber);
tran.action = 0;
tran.amount = amt;
tran.newbalance = a.balance;

int tran_fd = open("transection_db",O_CREAT|O_RDWR,S_IRWXU);
    lseek(tran_fd,0,SEEK_END);
    write(tran_fd,&tran,sizeof(struct transection));
    printf("added in transection\n");
 close(tran_fd);
 close(account_fd);
  }

  write(client_desc,&res,sizeof(int));

  userManage();
}


void userManage(){
    printf("inside user manage\n");
     int res;
    read(client_desc,&res,sizeof(int));

    printf("value of user choice is %d\n",res);

    switch(res){
    case 1:
      addMoney();
      break;
    case 2:
      withdrawMoney();
      break;
    case 5:
     accountDetails();
      break;
    case 4:
      changePassword();
      break;
    case 3:
      checkBalance();
      break;
    default:
      login();
  }

}

void searchAccount(){
    printf("inside search account\n");
    char username[MAX];
    read(client_desc,username,MAX);

    int res = 0;
   struct customer c;

  //customer_fd =open("customer_db",O_CREAT|O_RDWR,S_IRWXU);

  lseek(customer_fd,0,SEEK_SET);

 while(read(customer_fd,&c,sizeof(struct customer))>0){
     printf("%s %s %d %d \n",c.username,c.password,(int)c.status,(int)c.type);
     //printf("value of admin match\n  = %d",strcmp(admin_usr,c.username)==0);
     // printf("value of pass match\n  = %d",strcmp(admin_pwd,c.password)==0);
    if(strcmp(username,c.username)==0 && (int)c.status!=0){
      res=1;
      printf("user found\n");
      break;
    }
  }
      write(client_desc,&res,sizeof(int)); 
      if(res==1){
           
          //get related account details
          //search the account number
    struct account a;
    int account_fd =open("account_db",O_CREAT|O_RDWR,S_IRWXU);
    lseek(account_fd,0,SEEK_SET);
    while(read(account_fd,&a,sizeof(struct account))>0){
    if(strcmp(c.accountNumber,a.accountNumber)==0){
      printf("account mil gaya\n");
      break;
    }
  }
   write(client_desc,&c,sizeof(struct customer));
   write(client_desc,&a,sizeof(struct account)); 


      }
      adminmanage();
}

void deleteAccount(){
    printf("inside delete account\n");
    char username[MAX];
    read(client_desc,username,MAX);
   int res = 0;
   struct customer c;

  //customer_fd =open("customer_db",O_CREAT|O_RDWR,S_IRWXU);

  lseek(customer_fd,0,SEEK_SET);

 while(read(customer_fd,&c,sizeof(struct customer))>0){
     //printf("%s %s",c.username,c.password);
     //printf("value of admin match\n  = %d",strcmp(admin_usr,c.username)==0);
     // printf("value of pass match\n  = %d",strcmp(admin_pwd,c.password)==0);
    if(strcmp(username,c.username)==0){
      res=1;
      printf("user found\n");
      break;
    }
  }

  if(res==0){
      write(client_desc,&res,sizeof(int)); 
  }
  else{
      //delete account.
    c.status=false;
    lseek(customer_fd,-1*sizeof(struct customer),SEEK_CUR);
    write(customer_fd,&c,sizeof(struct customer));
    write(client_desc,&res,sizeof(int)); 
  }

adminmanage();

}

void modifyAccount(){

  printf("inside modify account\n");
  int acc;
  read(client_desc,&acc,sizeof(int));

  if(acc==1){
    printf("passwordchange hoga\n");
    char username[MAX];
    char password[MAX];

     read(client_desc,username,MAX);
     read(client_desc,password,MAX);
     //search the user
     printf("%s %s\n",username,password);
     int res = 0;
     struct customer c;

      lseek(customer_fd,0,SEEK_SET);

    while(read(customer_fd,&c,sizeof(struct customer))>0){
     //printf("%s %s",c.username,c.password);
     //printf("value of admin match\n  = %d",strcmp(admin_usr,c.username)==0);
     // printf("value of pass match\n  = %d",strcmp(admin_pwd,c.password)==0);
    if(strcmp(username,c.username)==0){
      res=1;
      printf("user exists\n");
      break;
    }
  }

  if(res==1){

    //change the password
     strcpy(c.password,password);
    lseek(customer_fd,-1*sizeof(struct customer),SEEK_CUR);
    write(customer_fd,&c,sizeof(struct customer));
    write(client_desc,&res,sizeof(int)); 
    printf("password changed\n");

  }
  else{
     write(client_desc,&res,sizeof(int));
  }

   adminmanage();

  }
  else if(acc==2){
      printf("joint account me convert hoga\n");
      char user[MAX];
      char username[MAX];
      char password[MAX];

      read(client_desc,user,MAX);

      //search the user
      int res = 0;
     struct customer c;

      lseek(customer_fd,0,SEEK_SET);

    while(read(customer_fd,&c,sizeof(struct customer))>0){
     //printf("%s %s",c.username,c.password);
     //printf("value of admin match\n  = %d",strcmp(admin_usr,c.username)==0);
     // printf("value of pass match\n  = %d",strcmp(admin_pwd,c.password)==0);
    if(strcmp(user,c.username)==0){
      res=1;
      printf("user exists\n");
      break;
    }
  }


   
  if(res==0){
    write(client_desc,&res,sizeof(int));
       adminmanage();
  }
  else{
     write(client_desc,&res,sizeof(int));
    read(client_desc,username,MAX);
    read(client_desc,password,MAX);

    printf("%s %s\n",username,password);
    c.type=2;
    lseek(customer_fd,-1*sizeof(struct customer),SEEK_CUR);
    write(customer_fd,&c,sizeof(struct customer));

    struct customer c1;
    strcpy(c1.username,username);
    strcpy(c1.password,password);
    c1.type= 2; //normal
    c1.status=1; //active
    strcpy(c1.accountNumber,c.accountNumber);
    
    lseek(customer_fd,0,SEEK_END);
    write(customer_fd,&c1,sizeof(struct customer));
    int res1=1;
    write(client_desc,&res1,sizeof(int));

    printf("joint account ban gaya\n");
    adminmanage();

  }





  }
 
 


}

void addAccount(){
    printf("inside add account\n");

    int acc;
    read(client_desc,&acc,sizeof(int));

    if(acc==1){
        printf("single account khulega\n");

   char username[MAX];
   char password[MAX];

  read(client_desc,username,MAX);
  read(client_desc,password,MAX);

  printf("%s %s\n",username,password);
    int res = 1;
   struct customer c;

  //customer_fd =open("customer_db",O_CREAT|O_RDWR,S_IRWXU);



  lseek(customer_fd,0,SEEK_SET);

 while(read(customer_fd,&c,sizeof(struct customer))>0){
     //printf("%s %s",c.username,c.password);
     //printf("value of admin match\n  = %d",strcmp(admin_usr,c.username)==0);
     // printf("value of pass match\n  = %d",strcmp(admin_pwd,c.password)==0);
    if(strcmp(username,c.username)==0){
      res=0;
      printf("user already exists\n");
      break;
    }
  }


  if(res==0){
       write(client_desc,&res,sizeof(int));
  }
  else{
    struct customer c1;
    strcpy(c1.username,username);
    strcpy(c1.password,password);
    c1.type= 1; //normal
    c1.status=1; //active
    strcpy(c1.accountNumber,username);
    strcat(c1.accountNumber,"1234");
    write(customer_fd,&c1,sizeof(struct customer));

    //assign account number
    struct account a1;
    strcpy(a1.accountNumber,username);
    strcat(a1.accountNumber,"1234");
    printf("account number is %s\n",a1.accountNumber);
    a1.balance = 0;
    a1.status = true;

    int account_fd =open("account_db",O_CREAT|O_RDWR,S_IRWXU);
    lseek(account_fd,0,SEEK_SET);
    write(account_fd,&a1,sizeof(struct account)); 
    printf("account created\n");
    close(account_fd);
    write(client_desc,&res,sizeof(int));

   }
  }
    else if(acc==2){
  printf("joint account khulega\n");
        char username1[MAX];
   char password1[MAX];
   char username2[MAX];
   char password2[MAX];

  read(client_desc,username1,MAX);
  read(client_desc,password1,MAX);
  read(client_desc,username2,MAX);
  read(client_desc,password2,MAX);


  printf("%s %s\n",username1,password1);
  printf("%s %s\n",username2,password2);
    int res = 1;
   struct customer c;

  //customer_fd =open("customer_db",O_CREAT|O_RDWR,S_IRWXU);



  lseek(customer_fd,0,SEEK_SET);

 while(read(customer_fd,&c,sizeof(struct customer))>0){
     //printf("%s %s",c.username,c.password);
     //printf("value of admin match\n  = %d",strcmp(admin_usr,c.username)==0);
     // printf("value of pass match\n  = %d",strcmp(admin_pwd,c.password)==0);
    if(strcmp(username1,c.username)==0 || strcmp(username2,c.username)==0){
      res=0;
      printf("user already exists\n");
      break;
    }
  }


  if(res==0){
       write(client_desc,&res,sizeof(int));
  }
  else{
    struct customer c1;
    struct customer c2;
    strcpy(c1.username,username1);
    strcpy(c1.password,password1);
    c1.type= 2; //normal
    c1.status=1; //active
    strcpy(c1.accountNumber,username1);
    strcat(c1.accountNumber,username2);
    strcat(c1.accountNumber,"1234");
    write(customer_fd,&c1,sizeof(struct customer));

    //2nd customer
      strcpy(c2.username,username2);
    strcpy(c2.password,password2);
    c2.type= 2; //normal
    c2.status=1; //active
    strcpy(c2.accountNumber,username1);
    strcat(c2.accountNumber,username2);
    strcat(c2.accountNumber,"1234");
    write(customer_fd,&c2,sizeof(struct customer));

    //assign account number
    struct account a1;
    strcpy(a1.accountNumber,username1);
    strcat(a1.accountNumber,username2);
    strcat(a1.accountNumber,"1234");
    printf("account number is %s\n",a1.accountNumber);
    a1.balance = 0;
    a1.status = true;

    int account_fd =open("account_db",O_CREAT|O_RDWR,S_IRWXU);
    lseek(account_fd,0,SEEK_SET);
    write(account_fd,&a1,sizeof(struct account)); 
    printf("account created\n");
    close(account_fd);
    write(client_desc,&res,sizeof(int));

   }
        

    }
    else{
        adminmanage();
    }
 adminmanage();
}

void adminmanage(){
    printf("inside admin manage\n");
    int res;
    read(client_desc,&res,sizeof(int));

    printf("value of admin choice is %d\n",res);

    switch(res){
    case 1:
      addAccount();
      break;
    case 2:
      deleteAccount();
      break;
    case 3:
       modifyAccount();
      break;
    case 4:
      searchAccount();
      break;
    default:
      login();
  }
}


void login(){
  char admin_usr[MAX];
  char admin_pwd[MAX];

  read(client_desc,admin_usr,MAX);
  read(client_desc,admin_pwd,MAX);

   printf("Account details are:%s %s\n",admin_usr,admin_pwd);

   //verify it to customer db.
   int res = 0;
   struct customer c;

  customer_fd =open("customer_db",O_CREAT|O_RDWR,S_IRWXU);

  lseek(customer_fd,0,SEEK_SET);

 while(read(customer_fd,&c,sizeof(struct customer))>0){
     //printf("%s %s",c.username,c.password);
     //printf("value of admin match\n  = %d",strcmp(admin_usr,c.username)==0);
     // printf("value of pass match\n  = %d",strcmp(admin_pwd,c.password)==0);
    if(strcmp(admin_usr,c.username)==0 && strcmp(admin_pwd,c.password)==0){
      res=1;
      printf("data verified\n");
      break;
    }
  }


 write(client_desc,&res,sizeof(int));
  if(res==0){
    printf("Invalid details\n");
     printf("admin account khulega\n");

   char username[MAX];
   char password[MAX];

  

  strcpy(username,"admin");
  strcpy(password, "12345");

  printf("%s %s\n",username,password);
    int res = 1;
   struct customer c;

  
  lseek(customer_fd,0,SEEK_SET);

 while(read(customer_fd,&c,sizeof(struct customer))>0){
     //printf("%s %s",c.username,c.password);
     //printf("value of admin match\n  = %d",strcmp(admin_usr,c.username)==0);
     // printf("value of pass match\n  = %d",strcmp(admin_pwd,c.password)==0);
    if(strcmp(username,c.username)==0){
      res=0;
      printf("user already exists\n");
      break;
    }
  }


  if(res==0){
       printf("admin account hai\n");
  }
  else{
    struct customer c1;
    strcpy(c1.username,username);
    strcpy(c1.password,password);
    c1.type= 0; //normal
    c1.status=1; //active
    strcpy(c1.accountNumber,username);
    strcat(c1.accountNumber,"1234");
    write(customer_fd,&c1,sizeof(struct customer));

  }
   login();
  }else{
    printf("Login verified\n");
    write(client_desc,&c,sizeof(struct customer));
    if(c.type!=0){
       userManage();
    }
    else{
       adminmanage();
    }
    

  }
}



int main(int argc,char const *argv[]){

char buff[1024];

struct sockaddr_in server, client;

socket_desc = socket(AF_INET, SOCK_STREAM, 0);
server.sin_family = AF_INET;
server.sin_addr.s_addr = INADDR_ANY;
server.sin_port = htons(atoi(argv[1]));
if(bind(socket_desc, (void *)(&server), sizeof(server)) < 0)
{
perror("Error on binding:");
exit(EXIT_FAILURE);
}
listen(socket_desc, 5);
size_client = sizeof(client);
while(1){
if((client_desc = accept(socket_desc, (struct sockaddr*)&client,&size_client)) < 0) {
perror("Error on accept:");
exit(EXIT_FAILURE);
}
if(fork() == 0){

    // verify credentials for the connected user and further
//stuff here

close(socket_desc);

login();

//  read(client_desc,buff,sizeof(buff));
//  printf("Message from client is %s /n :",buff);
//  write(client_desc,"ACK from server",16);



close(client_desc);
exit(EXIT_SUCCESS);
}else{
close(client_desc);
    }
}

return 0;

}