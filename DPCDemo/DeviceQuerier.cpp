#include <magic_enum.hpp>

#include "SimConstants.h"
#include "DeviceQuerier.h"

#pragma region Enum Conversions
std::ostream& operator<<(std::ostream& stream, const cl::sycl::info::partition_affinity_domain& config) {
	stream << magic_enum::enum_name(config);
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const cl::sycl::info::partition_property& config) {
	stream << magic_enum::enum_name(config);
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const cl::sycl::info::fp_config& config) {
	stream << magic_enum::enum_name(config);
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const cl::sycl::info::execution_capability& config) {
	stream << magic_enum::enum_name(config);
	return stream;
}
#pragma endregion

#pragma region Vector Conversions
template<class T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& items) {
	for (std::size_t i = 0; i < items.size(); ++i) {
		stream << "    " << items[i] << std::endl;
	}

	if (items.size() == 0)
	{
		stream << std::endl;
	}

	return stream;
}

template <int T>
std::ostream& operator<<(std::ostream& stream, const cl::sycl::id<T>& id) {
	stream << "[";
	for (int i = 0; i < T; i++) {
		stream << id[i];
		if (i != T - 1) {
			stream << ", ";
		}
	}

	stream << "]";
	return stream;
}
#pragma endregion

bool isHostDevice(const cl::sycl::device& device) {
	return device.get_info<cl::sycl::info::device::device_type>() == cl::sycl::info::device_type::host;
}

void DeviceQuerier::OutputAllDeviceInfo() {
	std::vector<cl::sycl::device> devices = cl::sycl::device::get_devices();
	for (const auto& dev : devices) {
		OutputDeviceInfo(dev);
	}
}

void DeviceQuerier::OutputDeviceInfo(const cl::sycl::device& dev)
{
	cl::sycl::platform plat = dev.get_platform();

	std::cout << "Name: "
		<< plat.get_info<cl::sycl::info::platform::name>()
		<< " (" << dev.get_info<cl::sycl::info::device::name>() << ")" << std::endl;
	std::cout << "  Device Type: "
		<< magic_enum::enum_name(dev.get_info<cl::sycl::info::device::device_type>()) << std::endl;
	std::cout << "  Vendor: "
		<< plat.get_info<cl::sycl::info::platform::vendor>()
		<< " (" << dev.get_info<cl::sycl::info::device::vendor>() << ")" << std::endl;
	std::cout << "  Version: "
		<< plat.get_info<cl::sycl::info::platform::version>()
		<< " (" << dev.get_info<cl::sycl::info::device::version>() << ")" << std::endl;
	std::cout << "  Profile: "
		<< plat.get_info<cl::sycl::info::platform::profile>()
		<< " (" << dev.get_info<cl::sycl::info::device::profile>() << ")" << std::endl;

#if VERBOSE_DEVICE_LOGS
	std::cout << "  Extensions: " << std::endl
		<< plat.get_info<cl::sycl::info::platform::extensions>();
	std::cout << "  Device Extensions: " << std::endl
		<< dev.get_info<cl::sycl::info::device::extensions>();

	std::cout << "  Preferred Vector Widths: " << std::endl
		<< "    char: " << dev.get_info<cl::sycl::info::device::preferred_vector_width_char>()
		<< ", short: " << dev.get_info<cl::sycl::info::device::preferred_vector_width_short>()
		<< ", int: " << dev.get_info<cl::sycl::info::device::preferred_vector_width_int>()
		<< ", long: " << dev.get_info<cl::sycl::info::device::preferred_vector_width_long>()
		<< std::endl
		<< "    half: " << dev.get_info<cl::sycl::info::device::preferred_vector_width_half>()
		<< ", float: " << dev.get_info<cl::sycl::info::device::preferred_vector_width_long>()
		<< ", double: " << dev.get_info<cl::sycl::info::device::preferred_vector_width_long>()
		<< std::endl;
	std::cout << "  Native Vector Widths: " << std::endl
		<< "    char: " << dev.get_info<cl::sycl::info::device::native_vector_width_char>()
		<< ", short: " << dev.get_info<cl::sycl::info::device::native_vector_width_short>()
		<< ", int: " << dev.get_info<cl::sycl::info::device::native_vector_width_int>()
		<< ", long: " << dev.get_info<cl::sycl::info::device::native_vector_width_long>()
		<< std::endl
		<< "    half: " << dev.get_info<cl::sycl::info::device::native_vector_width_half>()
		<< ", float: " << dev.get_info<cl::sycl::info::device::native_vector_width_long>()
		<< ", double: " << dev.get_info<cl::sycl::info::device::native_vector_width_long>()
		<< std::endl;

	std::cout << "  Driver Version: "
		<< dev.get_info<cl::sycl::info::device::driver_version>() << std::endl;
	std::cout << "  Profile: "
		<< dev.get_info<cl::sycl::info::device::profile>() << std::endl;
	std::cout << "  OpenCL Version: "
		<< dev.get_info<cl::sycl::info::device::opencl_c_version>() << std::endl;
	std::cout << "  Vendor ID: "
		<< dev.get_info<cl::sycl::info::device::vendor_id>() << std::endl;
	std::cout << "  Max Compute Units: "
		<< dev.get_info<cl::sycl::info::device::max_compute_units>() << std::endl;
	std::cout << "  Max Work Item Dimensions: "
		<< dev.get_info<cl::sycl::info::device::max_work_item_dimensions>() << std::endl;
	std::cout << "  Max Work Item Sizes: "
		<< dev.get_info<cl::sycl::info::device::max_work_item_sizes>() << std::endl;
	std::cout << "  Max Work Group Sizes: "
		<< dev.get_info<cl::sycl::info::device::max_work_group_size>() << std::endl;

	if (!isHostDevice(dev)) {
		// Crashes with the SYCL host device.
		std::cout << "  Max Clock Frequency: "
			<< dev.get_info<cl::sycl::info::device::max_clock_frequency>() << std::endl;
	}
	else {
		std::cout << "  Max Clock Frequency: UNKNOWN" << std::endl;
	}


	std::cout << "  Address Bits  : "
		<< dev.get_info<cl::sycl::info::device::address_bits>() << std::endl;
	std::cout << "  Max Memory Allocation  : "
		<< dev.get_info<cl::sycl::info::device::max_mem_alloc_size>() << std::endl;
	std::cout << "  Image Support? "
		<< dev.get_info<cl::sycl::info::device::image_support>() << std::endl;
	std::cout << "  Max Read Image Args: "
		<< dev.get_info<cl::sycl::info::device::max_read_image_args>() << std::endl;
	std::cout << "  Max Write Image Aargs: "
		<< dev.get_info<cl::sycl::info::device::max_write_image_args>() << std::endl;
	std::cout << "  Image Buffer Max Size: "
		<< dev.get_info<cl::sycl::info::device::image_max_buffer_size>() << std::endl;
	std::cout << "  Image Array Max Size: "
		<< dev.get_info<cl::sycl::info::device::image_max_array_size>() << std::endl;

	std::cout << "  Max Samplers: "
		<< dev.get_info<cl::sycl::info::device::max_samplers>() << std::endl;
	std::cout << "  Max Parameter Size: "
		<< dev.get_info<cl::sycl::info::device::max_parameter_size>() << std::endl;

	std::cout << "  Memory Base Address Align: "
		<< dev.get_info<cl::sycl::info::device::mem_base_addr_align>() << std::endl;
	std::cout << "  Global Memory Cache Type: "
		<< magic_enum::enum_name(dev.get_info<cl::sycl::info::device::global_mem_cache_type>()) << std::endl;
	std::cout << "  Global Memory Cache Line Size: "
		<< dev.get_info<cl::sycl::info::device::global_mem_cache_line_size>() << std::endl;
	std::cout << "  Global Memory Cache Size: "
		<< dev.get_info<cl::sycl::info::device::global_mem_cache_size>() << std::endl;
	std::cout << "  Global Memory Size: "
		<< dev.get_info<cl::sycl::info::device::global_mem_size>() << std::endl;
	std::cout << std::endl;

	std::cout << "  Image 2D Max height, width: ["
		<< dev.get_info<cl::sycl::info::device::image2d_max_height>() << ", "
		<< dev.get_info<cl::sycl::info::device::image2d_max_width>() << "]" << std::endl;
	std::cout << "  Image 3D Max height, width, depth: ["
		<< dev.get_info<cl::sycl::info::device::image3d_max_height>() << ", "
		<< dev.get_info<cl::sycl::info::device::image3d_max_width>() << ", "
		<< dev.get_info<cl::sycl::info::device::image3d_max_depth>() << "]" << std::endl;
	std::cout << std::endl;

	std::cout << "  FP Config: " << std::endl << "   half: " <<
		dev.get_info<cl::sycl::info::device::half_fp_config>() << "   single: " << std::endl <<
		dev.get_info<cl::sycl::info::device::single_fp_config>() << "   double: " << std::endl <<
		dev.get_info<cl::sycl::info::device::double_fp_config>();

	std::cout << "  Max Constant Buffer Size: "
		<< dev.get_info<cl::sycl::info::device::max_constant_buffer_size>() << std::endl;
	std::cout << "  Max Constant Args: "
		<< dev.get_info<cl::sycl::info::device::max_constant_args>() << std::endl;
	std::cout << "  Local Mem Type: "
		<< magic_enum::enum_name(dev.get_info<cl::sycl::info::device::local_mem_type>()) << std::endl;
	std::cout << "  Local Mem Size: "
		<< dev.get_info<cl::sycl::info::device::local_mem_size>() << std::endl;
	std::cout << "  Error Correction Support? "
		<< dev.get_info<cl::sycl::info::device::error_correction_support>() << std::endl;
	std::cout << "  Host Unified Memory? "
		<< dev.get_info<cl::sycl::info::device::host_unified_memory>() << std::endl;
	std::cout << "  Profiling Timer Resolution: "
		<< dev.get_info<cl::sycl::info::device::profiling_timer_resolution>() << std::endl;
	std::cout << "  Is Little Endian? "
		<< dev.get_info<cl::sycl::info::device::is_endian_little>() << std::endl;
	std::cout << "  Is Available? "
		<< dev.get_info<cl::sycl::info::device::is_available>() << std::endl;
	std::cout << "  Is Compiler Available? "
		<< dev.get_info<cl::sycl::info::device::is_compiler_available>() << std::endl;
	std::cout << "  Is Linker Available? "
		<< dev.get_info<cl::sycl::info::device::is_linker_available>() << std::endl;
	std::cout << "  Execution Capabilities: " << std::endl
		<< dev.get_info<cl::sycl::info::device::execution_capabilities>();
	std::cout << "  Queue Profiling? "
		<< dev.get_info<cl::sycl::info::device::queue_profiling>() << std::endl;
	std::cout << "  Built-in Kernels: "
		<< dev.get_info<cl::sycl::info::device::built_in_kernels>();
	std::cout << "  Printf buffer size: "
		<< dev.get_info<cl::sycl::info::device::printf_buffer_size>() << std::endl;
	std::cout << "  Preferred Interop User Sync? "
		<< dev.get_info<cl::sycl::info::device::preferred_interop_user_sync>() << std::endl;
	std::cout << "  Partition Max Sub Devices: "
		<< dev.get_info<cl::sycl::info::device::partition_max_sub_devices>() << std::endl;

	std::cout << "  Partition Properties: "
		<< dev.get_info<cl::sycl::info::device::partition_properties>();
	std::cout << "  Partition Affinity Domains: "
		<< dev.get_info<cl::sycl::info::device::partition_affinity_domains>();
	std::cout << "  Partition Type Property: "
		<< magic_enum::enum_name(dev.get_info<cl::sycl::info::device::partition_type_property>()) << std::endl;
	std::cout << "  Partition Type Affinity Domain: "
		<< magic_enum::enum_name(dev.get_info<cl::sycl::info::device::partition_type_affinity_domain>()) << std::endl;
	std::cout << "  Reference Count: "
		<< dev.get_info<cl::sycl::info::device::reference_count>() << std::endl;
#endif
}


