#include "server_shared.h"
#include "sockets.h"
#include "modul/Imports.h"
#include <ntstrsafe.h>

static uint64_t uni_copy_memory(const PacketCopyMemory& packet)
{
	PEPROCESS dest_process = nullptr;
	PEPROCESS src_process = nullptr;

	if (!NT_SUCCESS(PsLookupProcessByProcessId(HANDLE(packet.uni_dest_process_id), &dest_process)))
	{
		return uint64_t(STATUS_INVALID_CID);
	}

	if (!NT_SUCCESS(PsLookupProcessByProcessId(HANDLE(packet.uni_src_process_id), &src_process)))
	{
		ObDereferenceObject(dest_process);
		return uint64_t(STATUS_INVALID_CID);
	}

	SIZE_T   return_size = 0;
	NTSTATUS status = MmCopyVirtualMemory(
		src_process,
		(void*)packet.uni_src_address,
		dest_process,
		(void*)packet.uni_dest_address,
		packet.uni_size,
		UserMode,
		&return_size
	);
	ObDereferenceObject(dest_process);
	ObDereferenceObject(src_process);

	return uint64_t(status);
}

	static uint64_t uni_handle_get_base_address(const PacketGetBaseAddress& packet)
	{
		PEPROCESS process = nullptr;
		const NTSTATUS status = PsLookupProcessByProcessId(HANDLE(packet.uni_process_id), &process);
		uint64_t base_address = 0;
		char returnValue[256];
		for (int i = 0; i <= packet.uni_module_size; i++) {
			if (packet.uni_name_module[i] == 0)
				break;
			returnValue[i] = (char)packet.uni_name_module[i];
		}
		returnValue[packet.uni_module_size] = 0;
		if (NT_SUCCESS(status)) {
			KAPC_STATE state;
			KeStackAttachProcess(process, &state);
			if (PsGetProcessWow64Process(process) != NULL) {
				PPEB32 pPeb32 = (PPEB32)PsGetProcessWow64Process(process);
				for (PLIST_ENTRY32 pListEntry = (PLIST_ENTRY32)((PPEB_LDR_DATA32)pPeb32->Ldr)->InLoadOrderModuleList.Flink;
					pListEntry != &((PPEB_LDR_DATA32)pPeb32->Ldr)->InLoadOrderModuleList;
					pListEntry = (PLIST_ENTRY32)pListEntry->Flink)
				{
					PLDR_DATA_TABLE_ENTRY32 pEntry = CONTAINING_RECORD(pListEntry, LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);
					ANSI_STRING nameModule;
					UNICODE_STRING nameModuleGetClient;
					UNICODE_STRING nameModuleGet;

					RtlInitAnsiString(&nameModule, returnValue);
					RtlAnsiStringToUnicodeString(&nameModuleGetClient, &nameModule, TRUE);
					RtlInitUnicodeString(&nameModuleGet, (PWCH)pEntry->BaseDllName.Buffer);
					if (RtlCompareUnicodeString(&nameModuleGet, &nameModuleGetClient, TRUE) == 0)
						base_address = pEntry->DllBase;
				}
			}
			else {
				PPEB pPeb = PsGetProcessPeb(process);
				for (PLIST_ENTRY pListEntry = pPeb->Ldr->InMemoryOrderModuleList.Flink; pListEntry != &pPeb->Ldr->InMemoryOrderModuleList; pListEntry = pListEntry->Flink)
				{
					PLDR_DATA_TABLE_ENTRY pEntry = CONTAINING_RECORD(pListEntry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
					ANSI_STRING nameModule;
					UNICODE_STRING nameModuleGetClient;
					UNICODE_STRING nameModuleGet;

					RtlInitAnsiString(&nameModule, returnValue);
					RtlAnsiStringToUnicodeString(&nameModuleGetClient, &nameModule, TRUE);
					RtlInitUnicodeString(&nameModuleGet, (PWCH)pEntry->BaseDllName.Buffer);
					if (RtlCompareUnicodeString(&nameModuleGet, &nameModuleGetClient, TRUE) == 0)
						base_address = (uint64_t)pEntry->DllBase;

				}
			}
			KeUnstackDetachProcess(&state);
		}
		return base_address;
	}

	uint64_t handle_incoming_packet(const Packet& packet)
	{
		switch (packet.uni_header.uni_type)
		{
		case PacketType::uni_copy_memory:
			return uni_copy_memory(packet.data.uni_copy_memory);

		case PacketType::uni_get_base_address:
			return uni_handle_get_base_address(packet.data.uni_get_base_address);

		default:
			break;
		}

		return uint64_t(STATUS_NOT_IMPLEMENTED);
	}

	bool complete_request(const SOCKET client_connection, const uint64_t result)
	{
		Packet packet{ };

		packet.uni_header.uni_magic = packet_magic;
		packet.uni_header.uni_type = PacketType::uni_completed;
		packet.data.uni_completed.uni_result = result;

		return send(client_connection, &packet, sizeof(packet), 0) != SOCKET_ERROR;
	}