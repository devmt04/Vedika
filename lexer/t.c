#include <stdio.h>
#include <string.h>

int main(){
	char a[] = "153";
	int num = 0;
	int sum = 0;
	for(int i = 0;;i++){
		num = num*10 + (a[i]-'0');
		//printf("%d\n", num);
		sum += (a[i]-'0')*(a[i]-'0')*(a[i]-'0');
		if (i == strlen(a)-1){
			break;
		}
	}
	if(num == sum){
		printf("The number %d is a armstrogng number!\n", num);
	}
	return 0;
}