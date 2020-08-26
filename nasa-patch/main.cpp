#include <iostream>
#include "kernel_ctx/kernel_ctx.h"
#include "mem_ctx/mem_ctx.hpp"
#include "patch_ctx/patch_ctx.hpp"

int __cdecl main(int argc, char** argv)
{
	if (!nasa::load_drv())
		return NULL;

	nasa::kernel_ctx kernel;

	if (!nasa::unload_drv())
		return NULL;

	if (!kernel.clear_piddb_cache(nasa::drv_key, util::get_file_header((void*)raw_driver)->TimeDateStamp))
	{
		std::cerr << "[+] failed to clear piddb cache" << std::endl;
		return -1;
	}

	nasa::mem_ctx my_proc(kernel, GetCurrentProcessId());
	nasa::patch_ctx kernel_patch(&my_proc);

	const auto function_addr =
		reinterpret_cast<void*>(
			util::get_module_export("win32kbase.sys", "NtDCompositionRetireFrame"));

	const auto new_patch_page = kernel_patch.patch(function_addr);
	std::cout << "[+] new_patch_page: " << new_patch_page << std::endl;
	*(short*)new_patch_page = 0xDEAD;
	std::cout << "[+] kernel MZ (before patch): " << std::hex << kernel.rkm<short>(function_addr) << std::endl;
	kernel_patch.enable();
	std::cout << "[+] kernel MZ (patch enabled): " << std::hex << kernel.rkm<short>(function_addr) << std::endl;
	kernel_patch.disable();
	std::cout << "[+] kernel MZ (patch disabled): " << std::hex << kernel.rkm<short>(function_addr) << std::endl;
	std::cin.get();
}
