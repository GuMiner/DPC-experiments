#pragma once

#include <ostream>
#include <vector>
#include <CL/sycl.hpp>

class DeviceQuerier {
public:
	static void OutputDeviceInfo(const cl::sycl::device& device);
	static void OutputAllDeviceInfo();
};

