#include<pthread.h>
#include<stdio.h>

struct node{
    int x,y;
}num[110];


int shudu[10][10]={
    {1,2,3,4,5,6,7,8,9},
    {8,9,1,2,3,4,5,6,7},
    {9,1,2,3,4,5,6,7,8},
    {2,3,4,5,6,7,8,9,1},
    {3,4,5,6,7,8,9,1,2},
    {4,5,6,7,8,9,1,2,3},
    {5,6,7,8,9,1,2,3,4},
    {6,7,8,9,1,2,3,4,5},
    {7,8,9,1,2,3,4,5,6}
};//伪数独
int line[10],check[10][10];

void *checkx(void *arg){
    for(int i = 0;i <= 100;i++)
        line[i] = 0;
    int x = *(int *)arg;
    for (int i = 0; i < 9; i++) {
        if (!line[shudu[x][i]]) {
            line[shudu[x][i]] = 1;
        } else {
            check[0][x] = 1;
        }
    }

    return NULL;
}

void *checky(void *arg){
    for(int i = 0;i <= 100;i++)
        line[i] = 0;
    int y = *(int *)arg;
    for (int i = 0; i < 9; i++) {
        if (!line[shudu[i][y]]) {
            line[shudu[i][y]] = 1;
        } else {
            check[1][y] = 1;
        }
    }

    return NULL;
}

void *checkangel(void *arg){
    for(int i = 0;i <= 100;i++)
        line[i] = 0;
    node p = *(node *)arg;
    int px = p.x,py = p.y;
    for(int i = px;i <= px + 2;i++)
        for(int j = py;j <= py + 2;j++){
            if (!line[shudu[i][j]]){
                line[shudu[i][j]] = 1;
            } 
            else{
                check[2][px / 3 * 3 + py / 3] = 1;
            }
        }

    return NULL; 
}
int main(){
    int flag = 1;
    pthread_t s[10][10];
    for (int i = 0; i < 9; i++) {
        pthread_create(&s[0][i], NULL, checkx, &i);
        pthread_join(s[0][i], NULL);
    }
    for (int i = 0; i < 9; i++) {
        pthread_create(&s[1][i], NULL, checky, &i);
        pthread_join(s[1][i], NULL);
    }
    for (int i = 0; i < 9; i += 3) {
        for (int j = 0; j < 9; j += 3) {
            node tmp = {i, j};
            pthread_create(&s[2][i + j / 3], NULL, checkangel, &tmp);
            pthread_join(s[2][i + j / 3], NULL);
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 9; j++) {
            if (check[i][j]) {
                flag = 0;
                break;
            }
        }
    }
    puts("If the thread is legal, it will output legal; if it is illegal, it will output illegal.");
    if(flag){
        printf("legal\n");
    }
    else{
        printf("illegal\n");
    }
    return 0;
}