#include <stdio.h>
#include <conio.h>
#include <Windows.h>
int main(){
    int array[5];
    printf("Please, enter 5 unsigned decimal numbers from 0 to 255 inclusively: \n");
    for(int i = 0; i < 5; i++){
        scanf("%hhu", &array[i]);
    }
    printf("\n\nEntered numbers (decimal):\n");
    for(int i = 0; i < 5; i++)
        printf("%d ", array[i]);
    HMODULE hModule = GetModuleHandleW(NULL);
    if(hModule != NULL){

        for(int i = 0; i < 5; i++){
            array[i] = (array[i] ^ 0x5A) & 0x4D & (byte)i;
        }
    }

    printf("\n\nResult (decimal):\n");
    for(size_t i = 0; i < 5; i++){
        printf("%d ", array[i]);
    }
    printf("\n\nPress any key...\n");
    getch();
    return 0;
}
