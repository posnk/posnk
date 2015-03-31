#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
 
 
 
 
 
int main() {
 
 
 
    struct dirent **namelist;
    int n;
    FILE *f;
 
 
    if((f=fopen(".index","w")) == NULL) {
       printf("Sorry");
       return 0;
    }
    n = scandir("/", &namelist, 0, alphasort);
    if (n < 0)
        perror("scandir");
    else {
        while(n--) {
           fprintf(f,"&#37;s\n",namelist[n]);
           printf("%s\n", namelist[n]->d_name);
           free(namelist[n]);
        }
        free(namelist);
    }
 }
