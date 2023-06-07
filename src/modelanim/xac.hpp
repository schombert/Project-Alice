#pragma once

namespace xac {

/*
 * NOTE :
 * 	The Structs described in this file have a potential to change as time progresses and more is figured out of the XAC File Format,
 * 	Generally speaking named variables can be *roughly* guaranteed to remain constant
 */

struct xac_header {
	char magic[4] = {'X', 'A', 'C', ' '}; // Offset 0x0h  --- Magic Value
	uint32_t version;					  // Offset 0x4h  --- changes depending on if the file was created by Maya 8.5 x64 or not, if it is then its 01 00 00
										  // 01 otherwise its 01 00 00 00
};										  // Size: 8 bytes

struct metadata_section {
	uint32_t section_id;		 // Offset 0x8h  --- Based off leafs aggregation, this requires further research though to make sure its valid, it
								 // is always 07 00 00 00
	uint32_t offset_unk;		 // Offset 0xCh  --- Unknown Variable
	uint32_t unknown1;			 // Offset 0x10h --- Unknown Variable
	uint32_t unknown2;			 // Offset 0x14h --- Unknown Variable, Appears to be another type of version identifier perhaps?
	uint32_t unknown3;			 // Offset 0x18h --- FF FF FF FF appears to be something unknown
	uint32_t unknown4;			 // Offset 0x1Ch --- Unknown Variable
	uint32_t unknown5;			 // Offset 0x20h --- Unknown Variable, Appears to always be 00 00 00 00 ?
	uint32_t toolversize;		 // Offset 0x24h --- Size of the Tool String
	char toolinfo[toolversize];	 // Offset 0x28h --- Contains the information regarding what tool was used for creating the file
	uint32_t workfilesize;		 //              --- Appears to contain how long the workfile entry is
	char workinfo[workfilesize]; //              --- Contains the information regarding the originial file the XAC file was generated from
	uint32_t datestrsize;		 //              --- Contains how large the date string is, appears to always be 11 for Vic2s files
	char dateinfo[datestrsize];	 //              --- Contains date when file was compiled
};								 // Size: 40 + (toolversize + workfilesize + datestrsize) bytes

struct miscinfo_section_header {
	uint32_t section_id;	// Offset 0x0h  --- Appears to be 00 00 00 00 for this sectionid
	uint32_t unknown1;		// Offset 0x4h  --- Appears to always be 0B 00 00 00
	uint32_t unknown2;		// Offset 0x8h  --- Unknown Variable
	uint32_t unknown3;		// Offset 0xCh  --- Appears to always be 01 00 00 00
	uint32_t numofelements; // Offset 0x10h --- Appears to contain the number of elements / Number of Data header Entries
	uint32_t unknown7;		// Offset 0x14h --- Appears to be one of: (( 06 00 00 00 || 0B 00 00 00 || 07 00 00 00 || 04 00 00 00 ))
};							// Size: 24 bytes

struct miscinfo_section_entry {
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
}; // Size: (160 + elementnamesize) bytes

struct texture_section_header {
	uint32_t section_id; // Offset 0x0h   <=>  Appears to be 0D 00 00 00 for this sectionid
	uint32_t unknown1;	 // Offset 0x4h   <=>  Appears to always be 0C 00 00 00
	uint32_t unknown2;	 // Offset 0x8h   <=>  Appears to always be 01 00 00 00
	uint32_t unknown3;	 // Offset 0xCh   <=>  Appears to always be 03 00 00 00 || 01 00 00 00      Perhaps number of textures?
	uint32_t unknown3;	 // Offset 0x10h  <=>  Appears to always be 03 00 00 00 || 01 00 00 00      Also perhaps number of textures
	uint32_t unknown4;	 // Offset 0x14h  <=>  Appears to always be 00 00 00 00
	uint32_t unknown5;	 // Offset 0x18h  <=>  Appears to always be 03 00 00 00
	uint32_t unknown6;	 // Offset 0x1Ch  <=>  Appears to be (( 6B 00 00 00 || 60 00 00 00 || 62 00 00 00 ))
	uint32_t unknown7;	 // Offset 0x20h  <=>  Appears to always be 02 00 00 00
};

/*
struct {
	// The Data of a entry appears to come before the Type Name
	void* unknown_data[0x9C];   // Total, including what we know, is 0xA0
	// element is followed by a UID, however the first instance of a element does not need one, with the next instance of the element
starting out with UID 1 uint16_t elementname_size;  // Precedes the element name, its size includes the element name and the UID char
element[elementname_size] = "polySurface"                         // 0xA0 Bytes large
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
									|| "Wheel_"                             // 0xA0 Bytes large, however numbers often appear at the end
of the next elements data causing it to appear larger
									|| "Turret"                             // 0xA0 Bytes large
									|| "Helmet"                             // 0xA0 Bytes large
									|| "flagmesh"                           // 0xA0 Bytes large
									|| "Top"                                // 0xA0 Bytes large
									|| "J"                                  // 0xA0 Bytes large


	// the element is then ended with what appears to be a UID of the type, presumably unique within the same xac file aswell as its
accompying xsm files if any. } entry;


						|| "TexAnim"                            // 0xF3 Bytes large ???
						|| "British_Infantry_Diffuse"           // 0xA4 Bytes large ???
						|| "British_Infantry2_Diffuse"          // 0xA4 Bytes large ???
						|| "british_infantry_diffuse"           // 0xA4 Bytes large ???
						|| "Blockade_fence"                     // 0xA4 Bytes large ???
						|| "AustriaHungery_Infantry2_Diffuse"   // 0xA6 Bytes large ???
						|| "AustriaHungery_Infantry_Diffuse"    // 0xA4 Bytes large ???
						|| "Austrian_Tank_Diffuse"              // 0xB1 Bytes large ???
						|| "flag_normal"                        // 0xCF Bytes large ???
*/

} // namespace xac
