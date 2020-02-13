#pragma once
#include "stdint.h"

constexpr auto packet_magic = 0x12463171;
constexpr auto server_ip = 0x7F000001; // 127.0.0.1
constexpr auto server_port = 1112;

enum class PacketType
{
	uni_copy_memory,
	uni_get_base_address,
	uni_completed
};

struct PacketCopyMemory
{
	uint32_t uni_dest_process_id;
	uint64_t uni_dest_address;

	uint32_t uni_src_process_id;
	uint64_t uni_src_address;

	uint32_t uni_size;
};

struct PacketGetBaseAddress
{
	int uni_name_module[256];
	uint32_t uni_process_id;
	uint32_t uni_module_size;
};

struct PackedCompleted
{
	uint64_t uni_result;
};

struct PacketHeader
{
	uint32_t   uni_magic;
	PacketType uni_type;
};

struct Packet
{
	PacketHeader uni_header;
	union
	{
		PacketCopyMemory	 uni_copy_memory;
		PacketGetBaseAddress uni_get_base_address;
		PackedCompleted		 uni_completed;
	} data;
};