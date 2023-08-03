## XAC, XSM and XTEC file formats

Main link : https://aluigi.altervista.org/quickbms.htm

chrrox work >
- (model) xac(?) extractor : https://forum.xentax.com/viewtopic.php?p=52319#p52319
- (texture) xtex(?) extractor : https://forum.xentax.com/viewtopic.php?p=52491#p52491 (does not appear to be used in Vic2)
- (animation) xsm

### XAC
```c
#define DEF_HEADSIZE 28
#define DEF_FLAGSIZE 12

struct {
    char magic[4] = {'X', 'A', 'C', ' '};   // Offset 0x0h  --- Magic Value
    uint32_t version;                       // Offset 0x4h  --- changes depending on if the file was created by Maya 8.5 x64 or not, if it is then its 01 00 00 01 otherwise its 01 00 00 00
} xac_header;

struct {
    uint32_t section_id;                    // Offset 0x8h  --- Based off leafs aggregation, this requires further research though to make sure its valid, it is always 07 00 00 00
    uint32_t offset_unk;                    // Offset 0xCh  --- Unknown Variable
    uint32_t unknown1;                      // Offset 0x10h --- Unknown Variable
    uint32_t unknown2;                      // Offset 0x14h --- Unknown Variable, Appears to be another type of version identifier perhaps?
    uint32_t unknown3;                      // Offset 0x18h --- FF FF FF FF appears to be something unknown
    uint32_t unknown4;                      // Offset 0x1Ch --- Unknown Variable
    uint32_t unknown5;                      // Offset 0x20h --- Unknown Variable, Appears to always be 00 00 00 00 ?
    uint32_t toolversize;                   // Offset 0x24h --- Size of the Tool String
    char toolinfo[toolversize];             // Offset 0x28h --- Contains the information regarding what tool was used for creating the file
    uint32_t workfilesize;                  //              --- Appears to contain how long the workfile entry is
    char workinfo[workfilesize];            //              --- Contains the information regarding the originial file the XAC file was generated from
    uint32_t datestrsize;                   //              --- Contains how large the date string is, appears to always be 11 for Vic2s files
    char dateinfo[datestrsize];             //              --- Contains date when file was compiled
} metadata_section;

struct {
    uint32_t section_id;                    // Offset 0x0h  --- Appears to be 00 00 00 00 for this sectionid
    uint32_t unknown1;                      // Offset 0x4h  --- Appears to always be 0B 00 00 00
    uint32_t unknown2;                      // Offset 0x8h  --- Unknown Variable
    uint32_t unknown3;                      // Offset 0xCh  --- Appears to always be 01 00 00 00
    uint32_t numofelements;                 // Offset 0x10h --- Appears to contain the number of elements / Number of Data header Entries
    uint32_t unknown7;                      // Offset 0x14h --- Appears to be one of: (( 06 00 00 00 || 0B 00 00 00 || 07 00 00 00 || 04 00 00 00 ))
} skeleton_section_header; // Size: 24 bytes

struct {
    uint16_t unknown1[7];
    char div1[2];
    uint16_t unknown2[7];
    char div2[2];
    uint16_t unknown3[7];
    char div3[2];
    uint16_t unknown4;
    char div4[2];
    uint16_t unknown5;
    char div5[2];
    uint16_t unknown6[17];
    char div6[2];
    uint16_t unknown7[9];
    char div7[2];
    uint16_t unknown8[9];
    char div8[2];
    uint16_t unknown9[9];
    char div9[2];
    uint16_t unknown10;
    char div10[2];
    uint32_t elementnamesize;
    std::string type[elementnamesize];
} skeleton_section_entry;

struct {
    uint32_t section_id;                    // Offset 0x0h   <=>  Appears to be 0D 00 00 00 for this sectionid
    uint32_t unknown1;                      // Offset 0x4h   <=>  Appears to always be 0C 00 00 00
    uint32_t unknown2;                      // Offset 0x8h   <=>  Appears to always be 01 00 00 00
    uint32_t unknown3;                      // Offset 0xCh   <=>  Appears to always be 03 00 00 00 || 01 00 00 00      Perhaps number of textures?
    uint32_t unknown3;                      // Offset 0x10h  <=>  Appears to always be 03 00 00 00 || 01 00 00 00      Also perhaps number of textures
    uint32_t unknown4;                      // Offset 0x14h  <=>  Appears to always be 00 00 00 00
    uint32_t unknown5;                      // Offset 0x18h  <=>  Appears to always be 03 00 00 00
    uint32_t unknown6;                      // Offset 0x1Ch  <=>  Appears to be (( 6B 00 00 00 || 60 00 00 00 || 62 00 00 00 ))
    uint32_t unknown7;                      // Offset 0x20h  <=>  Appears to always be 02 00 00 00
} texture_section_header;

struct {
    /* The Data of a entry appears to come before the Type Name */
    void* unknown_data[0x9C];   // Total, including what we know, is 0xA0
    // element is followed by a UID, however the first instance of a element does not need one, with the next instance of the element starting out with UID 1
    uint16_t elementname_size;  // Precedes the element name, its size includes the element name and the UID
    char element[elementname_size] = "polySurface"                         // 0xA0 Bytes large
                                    || "pCube"                              // 0xA0 Bytes large
                                    || "pCylinder"                          // 0xA0 Bytes large
                                    || "ViewCompass"                        // 0xA0 Bytes large
                                    || "Manipulator"                        // 0xA0 Bytes large
                                    || "UniversalManip"                     // 0xA0 Bytes large
                                    || "Root"                               // 0xA0 Bytes large
                                    || "Hip"                                // 0xA0 Bytes large
                                    || "Left_leg_controlgroup"              // 0xA0 Bytes large
                                    || "ikHandle"                           // 0xA0 Bytes large
                                    || "Left_Hip"                           // 0xA0 Bytes large
                                    || "Left_Knee"                          // 0xA0 Bytes large
                                    || "Left_Foot"                          // 0xA0 Bytes large
                                    || "effector"                           // 0xA0 Bytes large
                                    || "Right_leg_controlgroup"             // 0xA0 Bytes large
                                    || "Right_Hip"                          // 0xA0 Bytes large
                                    || "Right_Knee"                         // 0xA0 Bytes large
                                    || "Right_Foot"                         // 0xA0 Bytes large
                                    || "Spine_Middle"                       // 0xA0 Bytes large
                                    || "Spine_Top"                          // 0xA0 Bytes large
                                    || "HelmetNode"                         // 0xA0 Bytes large
                                    || "Left_Shoulder"                      // 0xA0 Bytes large
                                    || "Left_Elbow"                         // 0xA0 Bytes large
                                    || "GunNode"                            // 0xA0 Bytes large
                                    || "Right_Shoulder"                     // 0xA0 Bytes large
                                    || "Right_Elbow"                        // 0xA0 Bytes large
                                    || "Right_Hand"                         // 0xA0 Bytes large
                                    || "joint"                              // 0xA0 Bytes large
                                    || "Tank"                               // 0xA0 Bytes large
                                    || "Wheel_"                             // 0xA0 Bytes large, however numbers often appear at the end of the next elements data causing it to appear larger
                                    || "Turret"                             // 0xA0 Bytes large
                                    || "Helmet"                             // 0xA0 Bytes large
                                    || "flagmesh"                           // 0xA0 Bytes large
                                    || "Top"                                // 0xA0 Bytes large
                                    || "J"                                  // 0xA0 Bytes large


    // the element is then ended with what appears to be a UID of the type, presumably unique within the same xac file aswell as its accompying xsm files if any.
} entry;


                        || "TexAnim"                            // 0xF3 Bytes large ???
                        || "British_Infantry_Diffuse"           // 0xA4 Bytes large ???
                        || "British_Infantry2_Diffuse"          // 0xA4 Bytes large ???
                        || "british_infantry_diffuse"           // 0xA4 Bytes large ???
                        || "Blockade_fence"                     // 0xA4 Bytes large ???
                        || "AustriaHungery_Infantry2_Diffuse"   // 0xA6 Bytes large ???
                        || "AustriaHungery_Infantry_Diffuse"    // 0xA4 Bytes large ???
                        || "Austrian_Tank_Diffuse"              // 0xB1 Bytes large ???
                        || "flag_normal"                        // 0xCF Bytes large ???

```

Program to readback XAC files:
```c++
#include <iostream>
#include <stdint.h>
#include <stdio.h>

struct xac_header {
	char magic[3];
	int32_t version;
	int32_t unknown_data;
	char junk[29];
};

int main(int argc, char *argv[]) {
	if (argc == 2) {
		FILE *fp = fopen(argv[1], "rb");
		xac_header header;
		fread(&header, sizeof(header), 1, fp);
		printf("Magic: %c.%c.%c\n",header.magic[0], header.magic[1], header.magic[2]);
		printf("Version: %d\n", header.version);
		printf("Unknown: %d\n", header.unknown_data);
		fclose(fp);
		return 0;
	}
	return -1;
}
```

QuickBMS script:
```python
#quickbms script
#War of Dragons
#from chrrox

open FDDE tbl2 1
set arcnum 0

goto 0x10001C 1

for i = 0
    get offset long 1
    get zsize long 1
    get null3 long 1
    get size long 1
   get arcnum long 1
        set NAME1 string "file0"
    set MYEXT string arcnum
    strlen MYEXTSZ MYEXT
    if MYEXTSZ == 1
        string NAME1 += "00"
    endif
    if MYEXTSZ == 2
        string name1 - 1
        string NAME1 += "0"
    endif
    if MYEXTSZ == 3
        string name1 - 1
        string NAME1 += ""
    endif
    string NAME1 += MYEXT
    string NAME1 += .data2
    open FDSE NAME1 0


    get null1 long 1
    get null2 long 1

    get name string 1
    Padding 4 1

    get null long 1

        log name offset zsize
next i
```

### XTEX

QuickBMS script:
```python
#quickbms script
#War of Dragons
#from chrrox

open FDDE tbl2 1
set arcnum 0


goto 0x10001C 1

for i = 0
    get offset long 1
    get zsize long 1
    get null3 long 1
    get size long 1
   get arcnum long 1
        set NAME1 string "file0"
    set MYEXT string arcnum
    strlen MYEXTSZ MYEXT
    if MYEXTSZ == 1
        string NAME1 += "00"
    endif
    if MYEXTSZ == 2
        string name1 - 1
        string NAME1 += "0"
    endif
    if MYEXTSZ == 3
        string name1 - 1
        string NAME1 += ""
    endif
    string NAME1 += MYEXT
    string NAME1 += .data2
    open FDSE NAME1 0


    get null1 long 1
    get null2 long 1

    get name string 1
    Padding 4 1

    get null long 1

        log name offset zsize
next i
```

### XSM
```c
struct xsm_header {
	char magic[4] = {'X', 'S', 'M', ' '}; // Offset 0x0h	--- Magic Value
	uint16_t fp_num;											// Offset 0x2h	--- First MagicNum, appears to always be 01 00
	uint16_t sp_num; // Offset 0x4h	--- Second MagicNUm, appears to be 00 01 or 00 00, changing depending on if Maya X64 was used
									 // or not, might be false though
};

struct xsm_metadata_section {
	uint32_t section_id;						// Offset 0x0h	--- Appears to use C9 00 00 00 for the metadata section
	uint32_t unknown1;							// Offset 0x4h	---
	uint32_t unk_ident;							// Offset 0x8h	--- Appears to always be 02 00 00 00, same as in XAC
	uint32_t unknown2;							// Offset 0xCh	--- Appears to always be 00 00 00 3F ???
	uint32_t unknown3;							// Offset 0x10h	---
	uint32_t unknown4;							// Offset 0x14h	---
	uint32_t unknown5;							// Offset 0x18h	---
	uint32_t toolversize						// Offset 0x1Ch	--- Is how large the CString with the Tool Name and Version in it is
			char toolname[toolversize]; // Offset 0x20h	--- Is the tool that generated the file
	uint32_t workfilesize;					//		--- Is how large the CString with the file location is
	char filename[workfilesize];		//		--- Is the originial, source file, the current file was generated from
	uint32_t datesize;							//		--- Is how large the CString with the date of creation in it
	char datestring[datesize];			//		--- Is the CString with the date of creation in it, Format is MMM-DD-YYYY
};

struct xsm_data_section_header {
	uint32_t section_id; // Offset 0x0h	--- Appears to use 00 00 00 00 for the data section
	uint32_t unknown2;	 // Offset 0x4h	--- Appears to always be CA 00 00 00, significance unknown
	uint32_t unknown3;	 // Offset 0x8h
	uint32_t unknown4;	 // Offset 0xCh	---
	uint32_t numentries; // Offset 0x10h	--- Appears to Contain how many Data Section Entries
};
```
