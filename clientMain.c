#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include "cJSON.h"
#include "cJSON.c"


char str[2000];
char token[2000];
char server_resp[2000];

void initConnection(void);
void firstMenu(void);
void signupMenu(void);
void loginMenu(void);
void mainMenu(void);
void timelineMenu(void);
void searchMenu(void);
void tweetMenu(void);
void personalMenu(void);
void Logout(void);




int main (){
    
    firstMenu();
    
    return 0;
}




void initConnection(){
    int net_socket;
    net_socket=socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(12345);
    server_address.sin_addr.s_addr=INADDR_ANY;
    
    int connection_check=connect(net_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    
    if (connection_check==-1)
        printf("Connection failed\n");
    
    memset(server_resp, 0, sizeof(server_resp)-1);

    send(net_socket, str, sizeof(str)-1, 0);
    recv(net_socket, server_resp, sizeof(server_resp)-1, 0);
    
    memset(str, 0, sizeof(str)-1);

    
    close(net_socket);
}


void firstMenu(){
    int input;
    
    printf("\t    Welcome!\n########################\n\n");
    printf("1- Sign up\n2- Log in\n3- Terminate\n\n");
    
    do {
        
        printf("Please enter a valid option : ");
        scanf("%d", &input);
        
    } while (input!=1 && input!=2 && input!=3);
    
    
    if(input==1)
        signupMenu();
    else if (input==2)
        loginMenu();
    else{
        strcat(str, "terminate");
        initConnection();
        exit(0);
    }
}


void signupMenu(){
    
    char user[31], pass[31];
    memset(user, 0, sizeof(user)-1);
    memset(pass, 0, sizeof(pass)-1);

    printf("________________________________________");
    printf("\n\nPlease enter your username : ");
    scanf("%s", user);
    printf("Please enter your password : ");
    scanf("%s", pass);

    strcat(str, "signup ");
    strcat(str, user);
    strcat(str, ", ");
    strcat(str, pass);
    strcat(str, "\n");

    initConnection();
    
    
    cJSON *strn = cJSON_Parse(server_resp);
    cJSON *type = cJSON_GetObjectItem(strn, "type");
    
    
    if (strcmp(type->valuestring, "Successful")==0){
        printf("You have signed up successfully.\n");
        printf("________________________________________\n");
        return loginMenu();
    }
    else{
        printf("Username already taken\n\n");
        printf("________________________________________\n");
        return signupMenu();
    }
}

void loginMenu(){
    printf("\t\t  Login menu\n\n");
    
    char user[31], pass[31];
    memset(user, 0, sizeof(user)-1);
    memset(pass, 0, sizeof(pass)-1);

    printf("________________________________________");
    printf("\n\nPlease enter your username : ");
    scanf("%s", user);
    printf("Please enter your password : ");
    scanf("%s", pass);

    strcat(str, "login ");
    strcat(str, user);
    strcat(str, ", ");
    strcat(str, pass);
    strcat(str, "\n");

    initConnection();
    
    cJSON *strn = cJSON_Parse(server_resp);
    cJSON *type = cJSON_GetObjectItem(strn, "type");
    cJSON *message = cJSON_GetObjectItem(strn, "message");

    
    if (strcmp(type->valuestring, "Token")==0){
        printf("You are logged in.\n");
        memset(token, 0, sizeof(token)-1);
        strcat(token, message->valuestring);
        printf("________________________________________\n\n\n");
        return mainMenu();
    }
    
    else if (strstr(message->valuestring, "already")){
        return mainMenu();
    }
    
    else{
        printf("%s\n\n", message->valuestring);
        return firstMenu();
    }
    
}


void mainMenu(){
    int input;
    
    printf("\n\t\t\t<< Main menu >>\n");
    printf("===================================================\n\n");
    printf("1- Tweet section\n2- Searching area\n3- Timeline\n4- Personal area\n5- Log out\n");
    
    do {
        printf("Please enter a valid option : ");
        scanf("%d", &input);
    } while (input!=1 && input!=2 && input!=3 && input!=4 && input!=5);
    
    printf("\n===================================================\n\n");

    if (input==1) {
        tweetMenu();
    }
    if (input==2) {
        searchMenu();
    }
    if (input==3) {
        timelineMenu();
    }
    if (input==4) {
        personalMenu();
    }
    if (input==5) {
        Logout();
    }
    
}


void tweetMenu(){
    printf("Your account :\n\n");
    
    strcat(str, "profile ");
    strcat(str, token);
    strcat(str, "\n");
    
    initConnection();

    cJSON *strn = cJSON_Parse(server_resp);
    cJSON *type = cJSON_GetObjectItem(strn, "type");
    
    
    if (strcmp(type->valuestring, "Profile")==0) {
        
        cJSON *message = cJSON_GetObjectItem(strn, "message");
        cJSON *user = cJSON_GetObjectItem(message, "username");
        cJSON *bio = cJSON_GetObjectItem(message, "bio");
        cJSON *num_follower = cJSON_GetObjectItem(message, "numberOfFollowers");
        cJSON *num_following = cJSON_GetObjectItem(message, "numberOfFollowings");
        cJSON *tweets = cJSON_GetObjectItem(message, "allTweets");


        printf("Username : %s\t|\tBio : %s\n", user->valuestring, bio->valuestring);
        printf("#Followers : %d\t#Followings : %d\n", num_follower->valueint, num_following->valueint);
        printf("Tweets : \n");
        

        cJSON *content = NULL;
        cJSON *comment = NULL;
        cJSON *like = NULL;

        
        
        for(int i=0; i<cJSON_GetArraySize(tweets); i++) {
            cJSON * subitem = cJSON_GetArrayItem(tweets, i);
            
            content = cJSON_GetObjectItem(subitem, "content");
            comment = cJSON_GetObjectItem(subitem, "comments");
            like = cJSON_GetObjectItem(subitem, "likes");
            
            printf("Tweet text : \n\t\t%s\n\n", content->valuestring);
            printf("Likes : %d\t|", like->valueint);
            printf("\tComments : %d\n", cJSON_GetArraySize(comment));
        }
        
        return mainMenu();
    }
}

void searchMenu(){
    char name[31];
    
    memset(name, 0, sizeof(name)-1);
    
    printf("Who are you seaching for : ");
    scanf("%s", name);
    
    strcat(str, "search ");
    strcat(str, token);
    strcat(str, ", ");
    strcat(str, name);
    strcat(str, "\n");
    
    
    initConnection();
    
    cJSON *strn = cJSON_Parse(server_resp);
    cJSON *type = cJSON_GetObjectItem(strn, "type");
    
    
    if (strcmp(type->valuestring, "Profile")==0) {
        
        cJSON *message = cJSON_GetObjectItem(strn, "message");
        cJSON *user = cJSON_GetObjectItem(message, "username");
        cJSON *bio = cJSON_GetObjectItem(message, "bio");
        cJSON *num_follower = cJSON_GetObjectItem(message, "numberOfFollowers");
        cJSON *num_following = cJSON_GetObjectItem(message, "numberOfFollowings");
        cJSON *fstatus = cJSON_GetObjectItem(message, "followStatus");
        cJSON *tweets = cJSON_GetObjectItem(message, "allTweets");


        printf("Username : %s\t|\tBio : %s\n", user->valuestring, bio->valuestring);
        printf("#Followers : %d\t#Followings : %d\n", num_follower->valueint, num_following->valueint);
        printf("Following status | %s\n\n", fstatus->valuestring);
        printf("Tweets : \n");
        
        
        
        cJSON *id = NULL;
        cJSON *author = NULL;
        cJSON *content = NULL;
        cJSON *comment = NULL;
        cJSON *like = NULL;

        
        
        for(int i=0; i<cJSON_GetArraySize(tweets); i++) {
            cJSON * subitem = cJSON_GetArrayItem(tweets, i);
            
            id = cJSON_GetObjectItem(subitem, "id");
            author = cJSON_GetObjectItem(subitem, "author");
            content = cJSON_GetObjectItem(subitem, "content");
            comment = cJSON_GetObjectItem(subitem, "comments");
            like = cJSON_GetObjectItem(subitem, "likes");
            
            printf("\tID : %d\n", id->valueint);
            printf("\tWritten by : %s\n", author->valuestring);
            printf("Tweet text : \n\t\t%s\n\n", content->valuestring);
            printf("\tLikes : %d\n", like->valueint);
            printf("\tComments : %d\n", cJSON_GetArraySize(comment));
            
            for (int k=0; k<cJSON_GetArraySize(comment); k++) {
                cJSON * item = cJSON_GetArrayItem(comment, k);
                printf("\t\t\t%s said : \n\t\t\t%s\n", item->string, item->valuestring);
            }
        }
        
        printf("\n1- Changing following status\n2- Back to main menu\n ");
        int inp;
        do {
            printf("Please enter a valid option : ");
            scanf("%d", &inp);
        } while (inp!=1 && inp!=2);
        
        if (inp==1) {
            
            memset(str, 0, sizeof(str)-1);
            
            if (strcmp(fstatus->valuestring, "NotFollowed")==0) {
                strcat(str, "follow ");
                strcat(str, token);
                strcat(str, ", ");
                strcat(str, user->valuestring);
                strcat(str, "\n");

                initConnection();
                
                cJSON * resp = cJSON_Parse(server_resp);
                cJSON * type = cJSON_GetObjectItem(resp, "type");
                cJSON * message = cJSON_GetObjectItem(resp, "message");
                
                printf("%s\n\n", message->valuestring);
            }
            else{
                strcat(str, "unfollow ");
                strcat(str, token);
                strcat(str, ", ");
                strcat(str, user->valuestring);
                strcat(str, "\n");

                
                initConnection();
                
                cJSON * resp = cJSON_Parse(server_resp);
                cJSON * type = cJSON_GetObjectItem(resp, "type");
                cJSON * message = cJSON_GetObjectItem(resp, "message");
                
                printf("%s\n\n", message->valuestring);
            }
        }
        return mainMenu();
    }
    
    else{
        printf("An error occured.\n\n");
        return mainMenu();
    }
    
}
void timelineMenu(){
    int input;
    printf("1- Send a tweet\n2- Show other's tweets\n");
    
    do {
        printf("Please enter a valid option : ");
        scanf("%d", &input);
    } while (input!=1 && input!=2);
    
    if (input==1) {
        printf("\n\nEnter the text of your tweet : ");
        char s[1000];
        memset(s, 0, sizeof(s)-1);
        fflush(stdin);
        fgets(s, 1000, stdin);
        
        strcat(str, "send tweet ");
        strcat(str, token);
        strcat(str, ", ");
        strcat(str, s);
        strcat(str, "\n");
        
        initConnection();
        
        printf("\nTweet sent successfully.\n\n");
        return mainMenu();
    }
    else{
        strcat(str, "refresh ");
        strcat(str, token);
        strcat(str, "\n");
        
        initConnection();
        
        cJSON * resp = cJSON_Parse(server_resp);
        cJSON * type = cJSON_GetObjectItem(resp, "type");
        cJSON * message = cJSON_GetObjectItem(resp, "message");
        
        cJSON *id = NULL;
        cJSON *author = NULL;
        cJSON *content = NULL;
        cJSON *comment = NULL;
        cJSON *like = NULL;
        
        if (strcmp(type->valuestring, "List")==0) {
            for (int i=0; i<cJSON_GetArraySize(message); i++) {
                cJSON * subitem = cJSON_GetArrayItem(message, i);
                
                id = cJSON_GetObjectItem(subitem, "id");
                author = cJSON_GetObjectItem(subitem, "author");
                content = cJSON_GetObjectItem(subitem, "content");
                comment = cJSON_GetObjectItem(subitem, "comments");
                like = cJSON_GetObjectItem(subitem, "likes");
                
                printf("User : %s\t| %d\n\n", author->valuestring, id->valueint);
                printf("Tweet :\t%s\n\n", content->valuestring);
                printf("Likes : %d\t|\tComments : %d\n\n\n", like->valueint, cJSON_GetArraySize(comment));
            }
            printf("1- Like a tweet\n2- Comment a tweet\n3- Back to menu\n\n");
            
            int a;
            
            do {
                printf("Please enter a valid option : ");
                scanf("%d", &a);
            } while (a!= 1 && a!=2 && a!=3);
            
            if (a==1) {
                char num[20];
                memset(num, 0, sizeof(num)-1);
                
                printf("Enter the id : ");
                scanf("%s", num);
                
                strcat(str, "like ");
                strcat(str, token);
                strcat(str, ", ");
                strcat(str, num);
                strcat(str, "\n");
                
                initConnection();
                
                cJSON * resp = cJSON_Parse(server_resp);
                cJSON * type = cJSON_GetObjectItem(resp, "type");
                cJSON * message = cJSON_GetObjectItem(resp, "message");
                
                if (strcmp(type->valuestring, "Error") == 0) {
                    printf("%s\n", message->valuestring);
                }
            }
            
            if (a==2) {
                char mg[100];
                memset(mg, 0, sizeof(mg)-1);
                char num[20];
                memset(num, 0, sizeof(num)-1);
                
                printf("Enter the id : ");
                scanf("%s", num);

                printf("Enter the comment : ");
                fflush(stdin);
                fgets(mg, 100, stdin);
                
                strcat(str, "comment ");
                strcat(str, token);
                strcat(str, ", ");
                strcat(str, num);
                strcat(str, ", ");
                strcat(str, mg);
                strcat(str, "\n");
                
                initConnection();
                
                cJSON * resp = cJSON_Parse(server_resp);
                cJSON * type = cJSON_GetObjectItem(resp, "type");
                cJSON * message = cJSON_GetObjectItem(resp, "message");
                
                
                if (strcmp(type->valuestring, "Error") == 0) {
                    printf("%s\n", message->valuestring);
                }
            }
            
            return mainMenu();
        }
        else{
            printf("An error ouccured!\n");
            return mainMenu();
        }
    }
}
void personalMenu(){
    printf("Personal settings : \n\n1- Set bio\n2-Change password\n");
    int inp;
    do {
        printf("Please enter a valid option : ");
        scanf("%d", &inp);
    } while (inp!=1 && inp!=2);
    
    if (inp==1) {
        char bio[100];
        memset(bio, 0, sizeof(bio)-1);
        
        printf("Enter your bio text : ");
        fflush(stdin);
        fgets(bio, 100, stdin);
        
        strcat(str, "set bio ");
        strcat(str, token);
        strcat(str, ", ");
        strcat(str, bio);
        strcat(str, "\n");
        
        initConnection();
        
        cJSON * resp = cJSON_Parse(server_resp);
        cJSON * message = cJSON_GetObjectItem(resp, "message");
        
        printf("%s\n", message->valuestring);
    }
    else{
        char current_pass[30];
        char new_pass[30];
        memset(current_pass, 0, sizeof(current_pass)-1);
        memset(new_pass, 0, sizeof(new_pass)-1);
        
        printf("Please enter your current password : ");
        scanf("%s", current_pass);
        
        printf("Please enter your new password : ");
        scanf("%s", new_pass);
        
        strcat(str, "change password ");
        strcat(str, token);
        strcat(str, ", ");
        strcat(str, current_pass);
        strcat(str, ", ");
        strcat(str, new_pass);
        strcat(str, "\n");
        
        initConnection();
        
        if (strcmp(server_resp, "wrong") == 0) {
            printf("Wrong password was entered.\n\n");
        }
        else{
            printf("Password has been changed successfully.\n\n");
        }
    }
    return mainMenu();
}
void Logout(){
    printf("You are logged out.\n\n");
    strcat(str, "logout ");
    strcat(str, token);
    
    initConnection();
    
    return firstMenu();
}
