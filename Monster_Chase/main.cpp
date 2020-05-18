#include <conio.h>
extern bool My_UnitTest();
extern bool HeapManager_UnitTest();

int main(int i_argc, char** i_argl)
{
	//My_UnitTest();
	HeapManager_UnitTest();
	_getch();
}