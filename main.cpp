#include <iostream>
#include "hidserial.h"

int main() {
	auto devs = MoreSerial::HIDDevice::enumerate(); // Get all the available devices
	for(auto x : devs) {
		std::printf("%ls; %ls\n", x.info.manufacturer_string.c_str(), x.info.product_string.c_str()); // Show manufacturer and product name for every device
	}
	auto moo = devs.back();
	moo.open();
	auto val = moo.read_until('\n');
	std::cout<<std::string(val.begin(), val.end())<<std::endl;
	//moo.close(); // This is not needed since it's automatically called in the destructor
}
