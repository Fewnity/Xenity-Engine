#pragma once

#include <engine/reflection/enum_utils.h>

ENUM(IntegrityState,
	Integrity_Ok = 0,
	Integrity_Error_Non_Unique_Ids = 1 << 0,
	Integrity_Has_Wrong_Type_Files = 1 << 1,
	Integrity_Has_Empty_Path = 1 << 2,
	Integrity_Wrong_File_Position = 1 << 3,
	Integrity_Wrong_File_Size = 1 << 4,
	Integrity_Wrong_Meta_File_Position = 1 << 5,
	Integrity_Wrong_Meta_File_Size = 1 << 6);