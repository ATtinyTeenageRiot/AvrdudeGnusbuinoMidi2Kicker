#include <iostream>
#include "libavrdude-tester.h"

using namespace std;

int do_cycles;
 
void updateProgress( void* cb_class,  void (cb_func) (void * arg, int p ), int i     )
{
	//c 
	cb_func(cb_class, i);
}

class UsbaspClass {
	public:
	void progressUpdate(int p)
	{
		cout << "c++ " << p << "\n";
	}	
	
	void upload(int i)
	{
		updateProgress((void *) this, UsbaspClass::callback_libavrdude_progress, i);
	}
	
	static void callback_libavrdude_progress( void * cb_class, int p)
	{
		//c++
		UsbaspClass * targetClass = (UsbaspClass *) cb_class;
		targetClass->progressUpdate(p);
	}
	
};


int main(int argc, char *argv[]) {
	UsbaspClass babyUsb;
UsbaspClass babyUsb2;
	
	//c++
	babyUsb.upload(100);
babyUsb2.upload(200);

	my_upload();

	printf("%i",do_cycles);
	
}