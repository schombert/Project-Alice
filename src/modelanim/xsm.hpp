#pragma once

namespace xsm {

struct xsm_header {
	char magic[4] = {'X', 'S', 'M', ' '}; // Offset 0x0h	--- Magic Value
	uint16_t fp_num;					  // Offset 0x2h	--- First MagicNum, appears to always be 01 00
	uint16_t sp_num; // Offset 0x4h	--- Second MagicNUm, appears to be 00 01 or 00 00, changing depending on if Maya X64 was used
					 // or not, might be false though
};

struct xsm_metadata_section {
	uint32_t section_id;			// Offset 0x0h	--- Appears to use C9 00 00 00 for the metadata section
	uint32_t unknown1;				// Offset 0x4h	---
	uint32_t unk_ident;				// Offset 0x8h	--- Appears to always be 02 00 00 00, same as in XAC
	uint32_t unknown2;				// Offset 0xCh	--- Appears to always be 00 00 00 3F ???
	uint32_t unknown3;				// Offset 0x10h	---
	uint32_t unknown4;				// Offset 0x14h	---
	uint32_t unknown5;				// Offset 0x18h	---
	uint32_t toolversize			// Offset 0x1Ch	--- Is how large the CString with the Tool Name and Version in it is
		char toolname[toolversize]; // Offset 0x20h	--- Is the tool that generated the file
	uint32_t workfilesize;			//		--- Is how large the CString with the file location is
	char filename[workfilesize];	//		--- Is the originial, source file, the current file was generated from
	uint32_t datesize;				//		--- Is how large the CString with the date of creation in it
	char datestring[datesize];		//		--- Is the CString with the date of creation in it, Format is MMM-DD-YYYY
};

struct xsm_data_section_header {
	uint32_t section_id; // Offset 0x0h	--- Appears to use 00 00 00 00 for the data section
	uint32_t unknown2;	 // Offset 0x4h	--- Appears to always be CA 00 00 00, significance unknown
	uint32_t unknown3;	 // Offset 0x8h
	uint32_t unknown4;	 // Offset 0xCh	---
	uint32_t numentries; // Offset 0x10h	--- Appears to Contain how many Data Section Entries
};

} // namespace xsm
