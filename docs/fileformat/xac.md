## XAC Format
```c
#define DEF_HEADSIZE 28
#define DEF_FLAGSIZE 12

struct {
    char magic[4] = {'X', 'A', 'C', ' '};
    uint32_t unknown1;
    uint32_t unknown2;
    uint32_t dateoffset;  // Offset from the beginning, places offset++ places you just after the generated from file, and 4 bytes before the date, this is offset 0xC or 12 from the beginning
    void* unknown3[18];
    uint32_t flagoffset = dateoffset+DEF_HEADSIZE;  // Places you at some flags, perhaps default colour?
    uint32_t dataoffset = flagoffset+DEF_FLAGSIZE;   // This is the offset to the first byte in the first Data Header Entry
} header;

struct {
    /* The Data of a entry appears to come before the Type Name */
    void* data[0xa0];
    // element is followed by a UID, however the first instance of a element does not need one, with the next instance of the element starting out with UID 1
    std::string element = "polySurface"                         // 0xA0 Bytes large
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
                        || "British_Infantry_Diffuse"           // 0xA4 Bytes large ???
                        || "British_Infantry2_Diffuse"          // 0xA4 Bytes large ???
                        || "british_infantry_diffuse"           // 0xA4 Bytes large ???
                        || "Blockade_fence"                     // 0xA4 Bytes large ???
                        || "AustriaHungery_Infantry2_Diffuse"   // 0xA6 Bytes large ???
                        || "AustriaHungery_Infantry_Diffuse"    // 0xA4 Bytes large ???
                        || "Austrian_Tank_Diffuse"              // 0xB1 Bytes large ???
                        || "Tank"                               // 0xA0 Bytes large
                        || "Wheel_"                             // 0xA0 Bytes large, however numbers often appear at the end of the next elements data causing it to appear larger
                        || "Turret"                             // 0xA0 Bytes large
                        || "TexAnim"                            // 0xF3 Bytes large ???
                        || "Helmet"                             // 0xA0 Bytes large
                        || "flagmesh"                           // 0xA0 Bytes large
                        || "Top"                                // 0xA0 Bytes large
                        || "J"                                  // 0xA0 Bytes large
                        || "flag_normal"                        // 0xCF Bytes large ???


    // the element is then ended with what appears to be a UID of the type, presumably unique within the same xac file aswell as its accompying xsm files if any.
} entry;
```


