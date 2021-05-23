#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include "cJSON.h"
#include "cJSON.c"

/*      Account section
 username
 password
 followings
 followers
 bio
 tweets id
 */


/*      Tweet section
 tweet
 likes
 comments
 */


// refresh dadbeh.txt
//      following or follower username  --->   last read tweet id


char message[2000] = { };
char recv_message[2000] = { };
char token[31] = { };
char present_user[100] = { };
char token_arr[10][31] = { };
char user_arr[10][100] = { };
int maximum_index = 0;



char* replaceWord(const char* s, const char* oldW, const char* newW);
void lineReplacement(char filename[], int line_num, char content[]);
int repetitionChecker(char *filename, char *word);
void fileWriter(char filename[], char msg[]);
int isExisted(const char * filename);
void msgReceiver(int client_socket);
const char * tokenGenerator(void);
void followingStatus(int num);
int tokenChecker(char *tkn);
void changePassword(void);
void profileSearch(void);
void msgValidation(void);
void tweetProfile(void);
void refreshTweet(void);
void comment(void);
void signUp(void);
void logOut(void);
void tweet(void);
void logIn(void);
void like(void);
void bio(void);



/******************************  main  *************************************/

int main() {
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addrs;
    server_addrs.sin_family = AF_INET;
    server_addrs.sin_port = htons(12345);
    server_addrs.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr*) &server_addrs, sizeof(server_addrs));
    listen(server_socket, 5);

    int client_socket;
    
    while (strcmp(recv_message, "terminate") != 0) {
        memset(recv_message, 0, sizeof(recv_message));
        client_socket = accept(server_socket, NULL, NULL);
        msgReceiver(client_socket);
    }
            
    return 0;
}

/******************************  main  *************************************/





void msgReceiver(int client_socket){
    recv(client_socket, recv_message, sizeof(recv_message), 0);
    msgValidation();
    send(client_socket, message, sizeof(message)-1, 0);
}

void fileWriter(char filename[], char msg[]){
    FILE *fp = fopen(filename, "a");
    fputs(msg, fp);
    fputs("\n", fp);
    fclose(fp);
}

const char * tokenGenerator(){
    char *ptr = calloc(31, sizeof(char));
    memset(token, 0, sizeof(token));
    int decide;
    srand(time(NULL));
    for (int i = 0; i < 30; i++) {
        decide = rand() % 3;
        if (decide == 0) {
            token_arr[maximum_index][i] = token[i] = ptr[i] = (rand() % 10) + 48;
        }
        else if (decide == 1) {
            token_arr[maximum_index][i] = token[i] = ptr[i] = (rand() % 26) + 65;
        }
        else{
            token_arr[maximum_index][i] = token[i] = ptr[i] = (rand() % 26) + 97;
        }
    }
    maximum_index++;
    
    return ptr;
}

int isExisted(const char * filename){
    FILE *file;
    if ((file = fopen(filename, "r"))){
        fclose(file);
        return 1;
    }
    return 0;
}

void signUp(){
    char *user, *pass, temp[100];
    memset(temp, 0, sizeof(temp) - 1);
    user = strtok(NULL, ",");
    pass = strtok(NULL, " ");
    pass[strlen(pass) - 1] = 0;
    strcat(temp, user);
    strcat(temp, ".txt");
    if (isExisted(temp)) {
        memset(message, 0, sizeof(message));
        strcat(message ,"{\"type\":\"Error\",\"message\":\"This username is already taken.\"}");
        printf("Already taken username.\n");
    }
    else{
        memset(message, 0, sizeof(message));
        strcat(message ,"{\"type\":\"Successful\",\"message\":\"\"}");
        fileWriter(temp, user);
        fileWriter(temp, pass);
        fileWriter(temp, "0");
        fileWriter(temp, "0\n");
        
        memset(temp, 0, sizeof(temp));
        strcat(temp, "refresh ");
        strcat(temp, user);
        strcat(temp, ".txt");
        FILE *fp = fopen(temp, "w");
        fclose(fp);
        
        printf("User was made successfully.\n");
    }
}

void logIn(){
    char *str = strtok(recv_message, " ");
    char *username, *password, temp[100], tp[100];
    
    memset(temp, 0, sizeof(temp));
    memset(present_user, 0, sizeof(present_user));
    memset(tp, 0, sizeof(tp));
    
    username = strtok(NULL, ",");
    password = strtok(NULL, " ");
    
    strcat(present_user, username);
    strcat(temp, username);
    strcat(temp, ".txt");
    
    if (isExisted(temp)) {
        FILE *fp = fopen(temp, "r");
        fgets(tp, 100, fp);
        memset(tp, 0, sizeof(tp));
        fgets(tp, 100, fp);
        if (strcmp(tp, password) == 0) {
            memset(message, 0, sizeof(message));
            strcat(message ,"{\"type\":\"Token\",\"message\":\"");
            strcat(message ,tokenGenerator());
            strcat(message, "\"}");
            printf("%s has logged in    |     Token : %s\n", present_user, token);
            strcpy(user_arr[tokenChecker(token)], username);
        }
        else{
            memset(message, 0, sizeof(message));
            strcat(message ,"{\"type\":\"Error\",\"message\":\"Incorrect passwoed.\"}");
            printf("Incorrect password has been entered.\n");
        }
        fclose(fp);
    }
    else{
        memset(message, 0, sizeof(message));
        strcat(message ,"{\"type\":\"Error\",\"message\":\"No such username has been found!\"}");
        printf("No such username.\n");
    }
}

void bio(){
    char *str = strtok(recv_message, " "), *tkn, *bio_content, temp[100], tp[100];
    
    memset(temp, 0, sizeof(temp));
    strcat(temp, present_user);
    strcat(temp, ".txt");
    
    str = strtok(NULL, " ");
    tkn = strtok(NULL, " ");
    bio_content = strtok(NULL, "");
    tkn[strlen(tkn) - 1] = 0;
    bio_content[strlen(bio_content) - 2] = 0;

    memset(present_user, 0, sizeof(present_user));
    strcpy(present_user, user_arr[tokenChecker(tkn)]);
    printf("present user is : %s\n", present_user);
    
    if (strcmp(token, tkn) == 0) {
        FILE *fp = fopen(temp, "r");
        
        for (int i = 0; i < 5; i++) {
            memset(tp, 0, sizeof(tp));
            fgets(tp, 100, fp);
        }
        
        if (tp[0] != '\0' && tp[0] != '\n' && tp[0] != NULL) {
            memset(message, 0, sizeof(message));
            strcat(message ,"{\"type\":\"Error\",\"message\":\"You have already assigned your bio once.\"}");
        }
        else{
            memset(message, 0, sizeof(message));
            strcat(message ,"{\"type\":\"Successful\",\"message\":\"Your bio has been changed successfully.\"}");
            lineReplacement(temp, 5, bio_content);
        }
        
    }
    else{
        memset(message, 0, sizeof(message));
        strcat(message ,"{\"type\":\"Error\",\"message\":\"Wrong token!\"}");
    }
}

void msgValidation(){
    char temp_str[2000];
    memset(temp_str, 0, sizeof(temp_str)-1);
    strcat(temp_str, recv_message);
    char *str = strtok(temp_str, " ");
    if (strcmp(str, "signup") == 0) {
        return signUp();
    }
    else if(strcmp(str, "login") == 0){
        return logIn();
    }
    else if (strcmp(str, "set") == 0){
        return bio();
    }
    else if (strcmp(str, "follow") == 0){
        return followingStatus(0);
    }
    else if (strcmp(str, "unfollow") == 0){
        return followingStatus(1);
    }
    else if (strcmp(str, "send") == 0){
        return tweet();
    }
    else if (strcmp(str, "like") == 0){
        return like();
    }
    else if (strcmp(str, "comment") == 0){
        return comment();
    }
    else if (strcmp(str, "search") == 0){
        return profileSearch();
    }
    else if (strcmp(str, "change") == 0){
        return changePassword();
    }
    else if (strcmp(str, "profile") == 0){
        return tweetProfile();
    }
    else if (strcmp(str, "refresh") == 0){
        return refreshTweet();
    }
    else if (strcmp(str, "logout") == 0){
        return logOut();
    }
    else{
        memset(message, 0, sizeof(message));
        strcat(message ,"{\"type\":\"Error\",\"message\":\"Bad request format.\"}");
    }
}

void lineReplacement(char filename[], int line_num, char content[]){
    int counter = 1;
    char temp[500] = { };
    FILE *fp = fopen("temp.txt", "a"), *mainfp = fopen(filename, "r");
    
    while (fgets(temp, 500, mainfp) != NULL) {
        if (counter != line_num) {
            fputs(temp, fp);
            memset(temp, 0, sizeof(temp));
        }else{
            fputs(content, fp);
            fputs("\n", fp);
            memset(temp, 0, sizeof(temp));
        }
        counter++;
    }
    
    fclose(mainfp);
    remove(filename);
    fclose(fp);
    rename("temp.txt", filename);
}

char* replaceWord(const char* s, const char* oldW, const char* newW){
    char* result;
    int i, cnt = 0;
    int newWlen = strlen(newW);
    int oldWlen = strlen(oldW);
  
    for (i = 0; s[i] != '\0'; i++) {
        if (strstr(&s[i], oldW) == &s[i]) {
            cnt++;
  
            i += oldWlen - 1;
        }
    }
  
    result = (char*)malloc(i + cnt * (newWlen - oldWlen) + 1);
  
    i = 0;
    while (*s) {
        if (strstr(s, oldW) == s) {
            strcpy(&result[i], newW);
            i += newWlen;
            s += oldWlen;
        }
        else
            result[i++] = *s++;
    }
  
    result[i] = '\0';
    return result;
}

void followingStatus(int num){
    FILE *fp;
    char *str = strtok(recv_message, " "), *counter, *tkn, *username, temp1[1000] = { }, temp2[1000] = { }, fname[100] = { };
    
    tkn = strtok(NULL, " ");
    tkn[strlen(tkn) - 1] = 0;
    username = strtok(NULL, "\n");
    
    memset(present_user, 0, sizeof(present_user));
    strcpy(present_user, user_arr[tokenChecker(tkn)]);
    printf("present user is : %s\n", present_user);
    
    if (strcmp(present_user, username) == 0) {
        memset(message, 0, sizeof(message));
        strcat(message ,"{\"type\":\"Error\",\"message\":\"Yourself !!!\"}");
        return;
    }
    
    if (num == 0) {
        if (strcmp(tkn, token) == 0) {
            strcat(fname, username);
            strcat(fname, ".txt");
            fp = fopen(fname, "r");
            
            for (int i = 0; i < 3; i++) {
                fgets(temp1, 1000, fp);
                memset(temp1, 0, sizeof(temp1));
            }
            
            fgets(temp1, 1000, fp);
            temp1[strlen(temp1) - 1] = 0;
            strcat(temp1, " ");
            strcat(temp1, present_user);
            strcat(temp1, " ");
            strcpy(temp2, temp1);
            counter = strtok(temp1, " ");

            char buf[10];
            sprintf(buf,"%d",atoi(counter) + 1);
            strcpy(temp2, replaceWord(temp2, counter, buf));
            lineReplacement(fname, 4, temp2);
            
            if (repetitionChecker(fname, present_user) == 1) {
                char temp[500] = { };
                
                strcat(temp, "refresh ");
                strcat(temp, username);
                strcat(temp, ".txt");
                memset(fname, 0, sizeof(fname));
                strcat(fname, present_user);
                strcat(fname, ".txt");
                
                FILE *fp = fopen(temp, "a"), *file = fopen(fname, "r");
                fputs(present_user, fp);
                fputs(" 0\n", fp);
                
                for (int i = 0; i < 6; i++) {
                    memset(temp, 0, sizeof(temp));
                    fgets(temp, 500, file);
                }
                
                temp[strlen(temp) - 1] = 0;
                fputs(temp, fp);
                fputs(" \n", fp);
                fclose(fp);
            }
            
            memset(temp1, 0, sizeof(temp1));
            memset(temp2, 0, sizeof(temp2));
            memset(fname, 0, sizeof(fname));
            
            strcat(fname, present_user);
            strcat(fname, ".txt");
            fp = fopen(fname, "r");
                        
            for (int i = 0; i < 2; i++) {
                fgets(temp1, 1000, fp);
                memset(temp1, 0, sizeof(temp1));
            }
            
            fgets(temp1, 1000, fp);
            temp1[strlen(temp1) - 1] = 0;
            strcat(temp1, " ");
            strcat(temp1, username);
            strcat(temp1, " ");
            strcpy(temp2, temp1);
            counter = strtok(temp1, " ");
            
            memset(buf, 0, 10);
            sprintf(buf,"%d",atoi(counter) + 1);
            strcpy(temp2, replaceWord(temp2, counter, buf));
            lineReplacement(fname, 3, temp2);
            
            if (repetitionChecker(fname, username) == 1) {
                char temp[500] = { };
                
                strcat(temp, "refresh ");
                strcat(temp, present_user);
                strcat(temp, ".txt");
                memset(fname, 0, sizeof(fname));
                strcat(fname, username);
                strcat(fname, ".txt");
                
                FILE *fp = fopen(temp, "a"), *file = fopen(fname, "r");
                fputs(username, fp);
                fputs(" ", fp);
                fputs("0\n", fp);
                
                for (int i = 0; i < 6; i++) {
                    memset(temp, 0, sizeof(temp));
                    fgets(temp, 500, file);
                }
                temp[strlen(temp) - 1] = 0;
                fputs(temp, fp);
                fputs(" \n", fp);
                fclose(fp);
            }
            
            memset(message, 0, sizeof(message));
            strcat(message ,"{\"type\":\"Successful\",\"message\":\"User followed.\"}");
        }else{
            memset(message, 0, sizeof(message));
            strcat(message ,"{\"type\":\"Error\",\"message\":\"Wrong token!\"}");
        }
    }
    
    else{
        if (strcmp(tkn, token) == 0) {
            strcat(fname, username);
            strcat(fname, ".txt");
            
            fp = fopen(fname, "r");
            
            for (int i = 0; i < 3; i++) {
                fgets(temp1, 1000, fp);
                memset(temp1, 0, sizeof(temp1));
            }
            
            fgets(temp1, 1000, fp);
            temp1[strlen(temp1) - 1] = 0;
            strcpy(temp2, temp1);
            counter = strtok(temp1, " ");
        
            strcpy(temp2, replaceWord(temp2, present_user, " "));
            char buf[10];
            sprintf(buf,"%d",atoi(counter) - 1);
            
            strcpy(temp2, replaceWord(temp2, counter, buf));
            lineReplacement(fname, 4, temp2);
            
            char temp[500] = { }, addr[100] = { }, *s;
            strcat(addr, "refresh ");
            strcat(addr, username);
            strcat(addr, ".txt");
            
            if (repetitionChecker(fname, present_user) == 0) {
                FILE *file = fopen(addr, "r");
                int counter = 1;
                
                while (fgets(temp, 500, file)) {
                    s = strtok(temp, " ");
                    if (strcmp(s, present_user) == 0) {
                        lineReplacement(addr, counter, ".");
                    }
                    counter++;
                    memset(temp, 0, sizeof(temp));
                }
                
                fclose(file);
            }
            
            memset(temp1, 0, sizeof(temp1));
            memset(temp2, 0, sizeof(temp2));
            memset(fname, 0, sizeof(fname));
            
            strcat(fname, present_user);
            strcat(fname, ".txt");
            fp = fopen(fname, "r");
            
            for (int i = 0; i < 2; i++) {
                fgets(temp1, 1000, fp);
                memset(temp1, 0, sizeof(temp1));
            }
            
            fgets(temp1, 1000, fp);
            temp1[strlen(temp1) - 1] = 0;
            strcpy(temp2, temp1);
            counter = strtok(temp1, " ");
            
            strcpy(temp2, replaceWord(temp2, username, " "));
            memset(buf, 0, 10);
            sprintf(buf,"%d",atoi(counter) - 1);
            strcpy(temp2, replaceWord(temp2, counter, buf));
            lineReplacement(fname, 3, temp2);
            
            memset(addr, 0, sizeof(addr));
            memset(temp, 0, sizeof(temp));
            strcat(addr, "refresh ");
            strcat(addr, present_user);
            strcat(addr, ".txt");
            
            if (repetitionChecker(fname, username) == 0) {
                FILE *file = fopen(addr, "r");
                int counter = 1;
                
                while (fgets(temp, 500, file)) {
                    s = strtok(temp, " ");
                    if (strcmp(s, username) == 0) {
                        lineReplacement(addr, counter, ".");
                    }
                    counter++;
                    memset(temp, 0, sizeof(temp));
                }
                
                fclose(file);
            }
            memset(message, 0, sizeof(message));
            strcat(message ,"{\"type\":\"Successful\",\"message\":\"User unfollowed.\"}");
        }else{
            memset(message, 0, sizeof(message));
            strcat(message ,"{\"type\":\"Error\",\"message\":\"Wrong token!\"}");
        }
    }
}

void tweet(){
    char *str = strtok(recv_message, " "), *tkn, *content, filename[100] = { }, buf[100] = { }, temp[100] = { };
    int c;
    
    str = strtok(NULL, " ");
    tkn = strtok(NULL, " ");
    tkn[strlen(tkn) - 1] = 0;
    content = strtok(NULL, "\n");
    
    memset(present_user, 0, sizeof(present_user));
    strcpy(present_user, user_arr[tokenChecker(tkn)]);
    printf("present user is : %s\n", present_user);
    
    strcat(filename, present_user);
    strcat(filename, ".txt");
    
    FILE *id_num = fopen("id_num.txt", "r");
    
    while(fgets(temp, 100, id_num) != NULL){
        strcpy(buf, temp);
    }
    
    c = atoi(buf);
    memset(buf, 0, 100);
    sprintf(buf, "%d", c+1);
    
    fileWriter("id_num.txt", buf);
    fileWriter(filename, buf);
    
    char buf_file[100] = { };
    strcat(buf_file, buf);
    strcat(buf_file, ".txt");
    
    fileWriter(buf_file, content);
    fileWriter(buf_file, "0");
}

void like(){
    char *str = strtok(recv_message, " "), *tkn, *tweet_id, temp[100] = { }, buf[100] = { };
    int c;
    
    tkn = strtok(NULL, " ");
    tkn[strlen(tkn) - 1] = 0;
    tweet_id = strtok(NULL, "\n");
    
    strcat(temp, tweet_id);
    strcat(temp, ".txt");
    
    if (isExisted(temp)) {
        FILE *fp = fopen(temp, "r");
        fgets(buf, sizeof(buf), fp);
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf), fp);
        
        c = atoi(buf);
        memset(buf, 0, 100);
        sprintf(buf, "%d", c+1);
        
        lineReplacement(temp, 2, buf);
    }
    
    else{
        memset(message, 0, sizeof(message));
        strcat(message ,"{\"type\":\"Error\",\"message\":\"No such tweet id.\"}");
    }
    
}

void comment(){
    char *str = strtok(recv_message, " "), *tkn, *tweet_id, *content, fname[100] = { };
    
    tkn = strtok(NULL, " ");
    tkn[strlen(tkn) - 1] = 0;
    tweet_id = strtok(NULL, " ");
    tweet_id[strlen(tweet_id) - 1] = 0;
    content = strtok(NULL, "\n");
    
    memset(present_user, 0, sizeof(present_user));
    strcpy(present_user, user_arr[tokenChecker(tkn)]);
    printf("present user is : %s\n", present_user);
    
    strcat(fname, tweet_id);
    strcat(fname, ".txt");
    
    if (isExisted(fname) == 0) {
        memset(message, 0, sizeof(message));
        strcat(message ,"{\"type\":\"Error\",\"message\":\"No such tweet id.\"}");
    }
    
    FILE *fp = fopen(fname, "a");
    fputs(present_user, fp);
    fputs(" ", fp);
    fputs(content, fp);
    fputs(">", fp);
    fputs("\n", fp);
    
    fclose(fp);
    
    memset(message, 0, sizeof(message));
    strcat(message ,"{\"type\":\"Successful\",\"message\":\".\"}");
}


void profileSearch(){
    char *str = strtok(recv_message, " "), *tkn, *username, temp[500] = { }, temp1[500] = { }, fname[100] = { }, tp[500] = { }, *num_ptr, *tok_ptr1, *tok_ptr2, *result, *follow_check;
    int num;
    
    tkn = strtok(NULL, " ");
    tkn[strlen(tkn) - 1] = 0;
    username = strtok(NULL, "\n");
    
    memset(present_user, 0, sizeof(present_user));
    strcpy(present_user, user_arr[tokenChecker(tkn)]);
    printf("present user is : %s\n", present_user);
    
    strcat(fname, username);
    strcat(fname, ".txt");
    
    if (!isExisted(fname)) {
        memset(message, 0, sizeof(message));
        strcat(message ,"{\"type\":\"Error\",\"message\":\"No such username.\"}");
        return;
    }
    
    FILE *fp = fopen(fname, "r");
    
    cJSON *root = cJSON_CreateObject();
    cJSON *msg_obj = cJSON_CreateObject();
    cJSON *all_tweets = cJSON_CreateArray();
    
    fgets(temp, 500, fp);
    temp[strlen(temp) - 1] = 0;
    cJSON_AddItemToObject(msg_obj, "username", cJSON_CreateString(temp));
    
    memset(temp, 0, sizeof(temp));
    fgets(temp, 500, fp);
    
    memset(temp, 0, sizeof(temp));
    fgets(temp, 500, fp);
    num_ptr = strtok(temp, " ");
    num = atoi(num_ptr);
    cJSON_AddItemToObject(msg_obj, "numberOfFollowings", cJSON_CreateNumber(num));

    memset(temp, 0, sizeof(temp));
    fgets(temp, 500, fp);
    strcpy(temp1, temp);
    num_ptr = strtok(temp, " ");
    num = atoi(num_ptr);
    cJSON_AddItemToObject(msg_obj, "numberOfFollowers", cJSON_CreateNumber(num));
    
    follow_check = strstr(temp1, present_user);
    
    if (follow_check != NULL) {
        cJSON_AddItemToObject(msg_obj, "followStatus", cJSON_CreateString("Followed"));
    }
    else{
        cJSON_AddItemToObject(msg_obj, "followStatus", cJSON_CreateString("NotFollowed"));
    }
    
    memset(temp, 0, sizeof(temp));
    fgets(temp, 500, fp);
    temp[strlen(temp) - 1] = 0;
    cJSON_AddItemToObject(msg_obj, "bio", cJSON_CreateString(temp));
    
    
    while (fgets(temp, 500, fp) != NULL) {
        cJSON *tweet_obj = cJSON_CreateObject(), *comments = cJSON_CreateObject();
        memset(fname, 0, sizeof(fname));
        temp[strlen(temp) - 1] = 0;
        strcat(fname, temp);
        strcat(fname, ".txt");
        FILE *file = fopen(fname, "r");
        memset(tp, 0, sizeof(tp));
        fgets(tp, 500, file);
        cJSON_AddItemToObject(tweet_obj, "content", cJSON_CreateString(tp));
        memset(tp, 0, sizeof(tp));
        fgets(tp, 500, file);
        num = atoi(tp);
        cJSON_AddItemToObject(tweet_obj, "likes", cJSON_CreateNumber(num));
        num = atoi(temp);
        cJSON_AddItemToObject(tweet_obj, "id", cJSON_CreateNumber(num));
        memset(tp, 0, sizeof(tp));
        cJSON_AddItemToObject(tweet_obj, "author", cJSON_CreateString(username));
        while (fgets(tp, 500, file) != NULL) {
            tok_ptr1 = strtok(tp, " ");
            tok_ptr2 = strtok(NULL, ">");
            cJSON_AddItemToObject(comments, tok_ptr1, cJSON_CreateString(tok_ptr2));
            memset(tp, 0, sizeof(tp));
        }
        cJSON_AddItemToObject(tweet_obj, "comments", comments);
        cJSON_AddItemToArray(all_tweets, tweet_obj);
        fclose(file);
    }
    
    cJSON_AddItemToObject(msg_obj, "allTweets", all_tweets);
    cJSON_AddItemToObject(root, "type", cJSON_CreateString("Profile"));
    cJSON_AddItemToObject(root, "message", msg_obj);
    
    result = cJSON_Print(root);
    memset(message, 0, sizeof(message));
    strcat(message, result);
    printf("%s\n", result);
    fclose(fp);
}


void changePassword(void){
    char *str, *tkn, *new_pass, *old_pass, temp[500] = { }, tp[500] = { };
    str = strtok(recv_message, " ");
    str = strtok(NULL, " ");
    tkn = strtok(NULL, " ");
    old_pass = strtok(NULL, " ");
    new_pass = strtok(NULL, "\n");
    old_pass[strlen(old_pass) - 1] = 0;
    tkn[strlen(tkn) - 1] = 0;

    memset(present_user, 0, sizeof(present_user));
    strcpy(present_user, user_arr[tokenChecker(tkn)]);
    printf("present user is : %s\n", present_user);
    
    strcat(temp, present_user);
    strcat(temp, ".txt");
    
    FILE *fp = fopen(temp, "r");
    fgets(tp, 500, fp);
    memset(tp, 0, sizeof(tp));
    fgets(tp, 500, fp);
    tp[strlen(tp) - 1] = 0;
    fclose(fp);
    
    if (strcmp(tp, old_pass) == 0) {
        memset(message, 0, sizeof(message));
        strcat(message ,"true");
        lineReplacement(temp, 2, new_pass);
    }
    
    else{
        memset(message, 0, sizeof(message));
        strcat(message ,"wrong");
    }
}

void tweetProfile(void){
    char *str = strtok(recv_message, " "), *tkn, fname[100] = { }, temp[500] = { }, tp[500] = { }, *num_ptr, *result, *tok_ptr1, *tok_ptr2;
    int num;
    
    tkn = strtok(NULL, "\n");
    
    memset(present_user, 0, sizeof(present_user));
    strcpy(present_user, user_arr[tokenChecker(tkn)]);
    printf("present user is : %s\n", present_user);
    
    if (strcmp(token, tkn) == 0) {
        strcat(fname, present_user);
        strcat(fname, ".txt");
        
        FILE *fp = fopen(fname, "r");
        
        cJSON *root = cJSON_CreateObject();
        cJSON *msg_obj = cJSON_CreateObject();
        cJSON *all_tweets = cJSON_CreateArray();
        
        fgets(temp, 500, fp);
        temp[strlen(temp) - 1] = 0;
        cJSON_AddItemToObject(msg_obj, "username", cJSON_CreateString(temp));
        
        memset(temp, 0, sizeof(temp));
        fgets(temp, 500, fp);
        
        memset(temp, 0, sizeof(temp));
        fgets(temp, 500, fp);
        num_ptr = strtok(temp, " ");
        num = atoi(num_ptr);
        cJSON_AddItemToObject(msg_obj, "numberOfFollowings", cJSON_CreateNumber(num));

        memset(temp, 0, sizeof(temp));
        fgets(temp, 500, fp);
        num_ptr = strtok(temp, " ");
        num = atoi(num_ptr);
        cJSON_AddItemToObject(msg_obj, "numberOfFollowers", cJSON_CreateNumber(num));
        
        memset(temp, 0, sizeof(temp));
        fgets(temp, 500, fp);
        temp[strlen(temp) - 1] = 0;
        cJSON_AddItemToObject(msg_obj, "bio", cJSON_CreateString(temp));
        
        while (fgets(temp, 500, fp) != NULL) {
            cJSON *tweet_obj = cJSON_CreateObject(), *comments = cJSON_CreateObject();
            memset(fname, 0, sizeof(fname));
            temp[strlen(temp) - 1] = 0;
            strcat(fname, temp);
            strcat(fname, ".txt");
            FILE *file = fopen(fname, "r");
            memset(tp, 0, sizeof(tp));
            fgets(tp, 500, file);
            cJSON_AddItemToObject(tweet_obj, "content", cJSON_CreateString(tp));
            memset(tp, 0, sizeof(tp));
            fgets(tp, 500, file);
            num = atoi(tp);
            cJSON_AddItemToObject(tweet_obj, "likes", cJSON_CreateNumber(num));
            num = atoi(temp);
            cJSON_AddItemToObject(tweet_obj, "id", cJSON_CreateNumber(num));
            memset(tp, 0, sizeof(tp));
            cJSON_AddItemToObject(tweet_obj, "author", cJSON_CreateString(present_user));
            while (fgets(tp, 500, file) != NULL) {
                tok_ptr1 = strtok(tp, " ");
                tok_ptr2 = strtok(NULL, ">");
                cJSON_AddItemToObject(comments, tok_ptr1, cJSON_CreateString(tok_ptr2));
                memset(tp, 0, sizeof(tp));
            }
            cJSON_AddItemToObject(tweet_obj, "comments", comments);
            cJSON_AddItemToArray(all_tweets, tweet_obj);
        }
        
        cJSON_AddItemToObject(msg_obj, "allTweets", all_tweets);
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Profile"));
        cJSON_AddItemToObject(root, "message", msg_obj);
        
        result = cJSON_Print(root);
        memset(message, 0, sizeof(message));
        strcat(message, result);
        printf("%s\n", result);
        fclose(fp);
    }
    
    else{
        memset(message, 0, sizeof(message));
        strcat(message ,"{\"type\":\"Error\",\"message\":\"Wrong token!\"}");
    }
}

void refreshTweet(){
    char *str = strtok(recv_message, " "), temp[500] = { }, temp1[500] = { }, tp[500] = { }, addr[100] = { }, *user, *id, fname[100] = { }, fname1[100] = { }, c[10] = { }, *tok_ptr1, *tok_ptr2, *result, buf[10], *tkn, arr[100][4];
    int num, counter = 0;
    
    tkn = strtok(NULL, "\n");
    memset(present_user, 0, sizeof(present_user));
    strcpy(present_user, user_arr[tokenChecker(tkn)]);
    printf("present user is : %s\n", present_user);
    
    for (int i = 0; i < 100; i++) {
        strcpy(arr[i], "-1");
    }
    
    strcat(addr, "refresh ");
    strcat(addr, present_user);
    strcat(addr, ".txt");
    
    cJSON *root = cJSON_CreateObject();
    cJSON *msg_obj = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "type", cJSON_CreateString("List"));
    
    FILE *fp = fopen(addr, "r");
    
    while (fgets(temp, 500, fp) != NULL) {
        if (strcmp(temp, ".\n") == 0) {
            strcpy(arr[counter], "1");
            counter++;
            continue;
        }
        
        user = strtok(temp, " ");
        id = strtok(NULL, "\n");
        
        if (strcmp(user, "\n") == 0) {
            break;
        }
        
        strcat(fname, user);
        strcat(fname, ".txt");
        FILE *file = fopen(fname, "r");
        
        for (int i = 0; i < 5; i++) {
            fgets(temp1, 500, file);
            memset(temp1, 0, sizeof(temp1));
        }
                
        while (fgets(temp1, 500, file)) {
            int check = 1;
            temp1[strlen(temp1) - 1] = 0;
            if (strcmp(temp1, id) == 0 || strcmp(id, "0") == 0) {
                memset(temp1, 0, sizeof(temp1));
                do {
                    if (check) {
                        check = 0;
                        continue;
                    }
                    
                    temp1[strlen(temp1) - 1] = 0;
                    
                    if (strcmp(temp1, "\n") != 0 || temp1 != NULL) {
                        strcpy(c, temp1);
                    }

                    strcat(fname1, temp1);
                    strcat(fname1, ".txt");
                    cJSON *tweet_obj = cJSON_CreateObject(), *comments = cJSON_CreateObject();
                    FILE *file = fopen(fname1, "r");
                    memset(tp, 0, sizeof(tp));
                    fgets(tp, 500, file);
                    cJSON_AddItemToObject(tweet_obj, "content", cJSON_CreateString(tp));
                    memset(tp, 0, sizeof(tp));
                    fgets(tp, 500, file);
                    num = atoi(tp);
                    cJSON_AddItemToObject(tweet_obj, "likes", cJSON_CreateNumber(num));
                    num = atoi(temp1);
                    cJSON_AddItemToObject(tweet_obj, "id", cJSON_CreateNumber(num));
                    memset(tp, 0, sizeof(tp));
                    cJSON_AddItemToObject(tweet_obj, "author", cJSON_CreateString(user));
                    while (fgets(tp, 500, file) != NULL) {
                        tok_ptr1 = strtok(tp, " ");
                        tok_ptr2 = strtok(NULL, ">");
                        cJSON_AddItemToObject(comments, tok_ptr1, cJSON_CreateString(tok_ptr2));
                        memset(tp, 0, sizeof(tp));
                    }
                    cJSON_AddItemToObject(tweet_obj, "comments", comments);
                    cJSON_AddItemToArray(msg_obj, tweet_obj);
                    
                    memset(fname1, 0, sizeof(fname1));
                }while (fgets(temp1, 100, file));
            }
            memset(temp1, 0, sizeof(temp1));
        }
        strcpy(arr[counter], c);
        memset(fname, 0, sizeof(fname));
        memset(temp, 0, sizeof(temp));
        memset(c, 0, sizeof(c));
        counter++;
    }
    
    cJSON_AddItemToObject(root, "message", msg_obj);
    result = cJSON_Print(root);
    memset(message, 0, sizeof(message));
    strcat(message, result);
    printf("%s\n", result);
    fclose(fp);
    
    fp = fopen(addr, "r");
    
    for (int i = 0; strcmp(arr[i], "-1") != 0; i++) {
        if (strcmp(arr[i], "1") == 0) {
            fgets(temp, 500, fp);
            continue;
        }
        fgets(temp, 500, fp);
        user = strtok(temp, " ");
        strcat(temp1, user);
        strcat(temp1, " ");
        strcat(temp1, arr[i]);
        lineReplacement(addr, i + 1, temp1);
        memset(temp, 0, sizeof(temp));
        memset(temp1, 0, sizeof(temp1));
    }
}

void logOut(){
    char *str = strtok(recv_message, " "), *tkn;
    tkn = strtok(NULL, "\n");
    int i = tokenChecker(tkn);
    strcpy(token_arr[i], "-1");
    strcpy(user_arr[i], "-1");
}

int repetitionChecker(char *filename, char *word){
    char temp[500] = { }, *str;
    int counter = 0;
    FILE *fp = fopen(filename, "r");
    
    fgets(temp, 500, fp);
    memset(temp, 0, sizeof(temp));
    fgets(temp, 500, fp);
    memset(temp, 0, sizeof(temp));
    
    fgets(temp, 500, fp);
    str = strtok(temp, " ");
    
    while (str != NULL) {
        if (strcmp(str, word) == 0) {
            counter++;
        }
        str = strtok(NULL, " ");
    }
    
    memset(temp, 0, sizeof(temp));
    fgets(temp, 500, fp);
    str = strtok(temp, " ");
    
    while (str != NULL) {
        if (strcmp(str, word) == 0) {
            counter++;
        }
        str = strtok(NULL, " ");
    }
    
    return counter;
}


int tokenChecker(char *tkn){
    for (int i = 0; i < 10; i++) {
        if (strcmp(tkn, token_arr[i]) == 0) {
            return i;
        }
    }
    
    return -1;
}
