//#include<Windows.h>
//#include<stdio.h>
//
//void Routin(int* n) {
//	printf("My argument is %d\n", *n);
//}
//
//int main() {
//	int i, P[5];
//	DWORD Id;
//	HANDLE hHandles[5];
//	for (i = 0; i < 5; i++) {
//		P[i] = i;
//		hHandles[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Routin, &P[i], 0, &Id);
//		printf("Thread %d was created\n", Id);
//	}
//	for (i = 0; i < 5; i++) {
//		WaitForSingleObject(hHandles[i], INFINITE);
//	}
//}