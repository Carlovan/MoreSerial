#ifndef _HIDSERIAL
#define _HIDSERIAL

#include <iostream>

#include <vector>
#include <queue>
#include "hidapi.h"
#include <cstdint>
#include <cassert>

namespace MoreSerial
{
	// Stores the info about an HID device (wrapper around hid_device_info from hidapi)
	struct HIDDeviceInfo {
		const std::string  path;
		const uint16_t     vendor_id;
		const uint16_t     product_id;
		const std::wstring serial_number;
		const uint16_t     release_number;
		const std::wstring manufacturer_string;
		const std::wstring product_string;
		const int32_t      interface_number;

		HIDDeviceInfo(hid_device_info &info)
			: path{info.path ? info.path : ""},
			  vendor_id{info.vendor_id},
			  product_id{info.product_id},
			  serial_number{info.serial_number ? info.serial_number : L""},
			  release_number{info.release_number},
			  manufacturer_string{info.manufacturer_string ? info.manufacturer_string : L""},
			  product_string{info.product_string ? info.product_string : L""},
			  interface_number{info.interface_number}
		{}
	};

	// Represent an HID device
	class HIDDevice
	{
	private:
		hid_device *handle;
		std::queue<uint8_t> buffer;

		// Read a single incoming packet and add it to the buffer waiting for the given timeout (-1 = infinity)
		bool read_packet(int timeout = 1) {
			if(is_open()) {
				constexpr size_t bufsize = 1024;
				uint8_t buf[bufsize];
				size_t count = hid_read_timeout(handle, buf, bufsize, timeout);
				if(count > 0) {
					for(int i = 0; i < count; i++)
						buffer.push(buf[i]);
					return true;
				}
				else {
					return false;
				}
			}
			return false;
		}

		// Read all the incoming packets
		void read_all_packets(size_t max_count=-1) {
			for(size_t count = 0; (max_count < 0 || count < max_count) && read_packet(); count++);
		}

	public:
		const HIDDeviceInfo info;
		HIDDevice(hid_device_info &_info) : info{_info}, handle{nullptr}, buffer{} {}
		HIDDevice() =delete;
		~HIDDevice() {
			close();
		}

		// Returns a vector containing all the devices currently available in the system
		static std::vector<HIDDevice> enumerate() {
			std::vector<HIDDevice> out;
			hid_device_info *devs, *cur_dev;
			devs = hid_enumerate(0x0, 0x0);
			cur_dev = devs;
			while(cur_dev) {
				out.emplace_back(*cur_dev);
				cur_dev = cur_dev->next;
			}
			hid_free_enumeration(devs);
			return out;
		}

		void open() {
			if(!is_open()) {
				handle = hid_open(info.vendor_id, info.product_id, NULL);
			}
		}

		void close() {
			if(is_open()) {
				hid_close(handle);
				handle = nullptr;
			}
		}

		bool is_open() {
			return handle != nullptr;
		}

		// Reads a single byte from the device (blocking)
		char read() {
			if(buffer.empty())
				read_packet(-1);
			if(!buffer.empty()) {
				auto tmp = buffer.front();
				buffer.pop();
				return tmp;
			}
			return 0;
		}

		// Reads at most `length` bytes from the device (non-blocking)
		std::vector<uint8_t> read(size_t length) {
			while(buffer.size() < length && read_packet());
			std::vector<uint8_t> out;
			out.reserve(std::min(length, buffer.size()));
			for(size_t i = 0; i < length && !buffer.empty(); i++) {
				out.push_back(buffer.front());
				buffer.pop();
			}
			return out;
		}

		// Reads data from the device until it gets the given `terminator` character; it is not included (blocking)
		std::vector<uint8_t> read_until(char terminator) {
			std::vector<uint8_t> out;
			while(true) {
				if(buffer.empty())
					read_packet(-1);
				while(!buffer.empty()) {
					uint8_t tmp = buffer.front();
					buffer.pop();
					if(tmp == terminator)
						return out;
					out.push_back(tmp);
				}
			}
		}
	};
}

#endif
