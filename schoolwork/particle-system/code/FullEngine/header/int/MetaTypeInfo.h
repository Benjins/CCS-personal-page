#ifndef META_TYPE_INFO_H
#define META_TYPE_INFO_H

#pragma once

struct MetaMemberInfo;

struct MetaStructInfo{
	char* name;
	MetaMemberInfo* members;
	int memberCount;
};

struct MetaMemberInfo{
	char* name;
	size_t offset;
	int type;
};

enum MetaType {
	MetaType_int,
	MetaType_float,
	MetaType_Vector2,
	MetaType_Vector3,
	MetaType_string,
	MetaType_bool,
};

#endif