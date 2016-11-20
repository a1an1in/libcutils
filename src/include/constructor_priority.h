#ifndef __CONSTRUCTOR_PRIORITY_H__
#define __CONSTRUCTOR_PRIORITY_H__

enum constructor_priority_e{
	CONSTRUCTOR_PRIORITY_PRINT_LIBRARY_VERSION = 101,
	CONSTRUCTOR_PRIORITY_LIBALLOC_REGISTER_MODULES = 101,
	CONSTRUCTOR_PRIORITY_LIBDBG_REGISTER_MODULES = 101,
	CONSTRUCTOR_PRIORITY_SYNC_LOCK_REGISTER_MODULES = 102,
	CONSTRUCTOR_PRIORITY_REGISTER_MAP_MODULES = 103,
	CONSTRUCTOR_PRIORITY_DEBUGGER_CONSTRUCTOR = 110,
	CONSTRUCTOR_PRIORITY_DEFAULT_ALLOCATOR_CONSTRUCTOR = 111,
	CONSTRUCTOR_PRIORITY_CONCURRENT_CONSTRUCTOR_PRIOR = 112,
	CONSTRUCTOR_PRIORITY_OBJ_DEAMON = 113,
	CONSTRUCTOR_PRIORITY_REGISTER_CLASS = 114,
};

#endif
