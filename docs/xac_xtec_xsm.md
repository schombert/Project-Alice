## XAC & XSM

XAC (EmotionF**X** **Ac**tor files) and XSM (EmotionF**X** **S**keletal **M**otion files) are what powers the 3D "toy soldiers" of the vanilla game.

Nowadays they're thoroughly documented in user-friendly C++ code thanks to the XacDecoder by Katerina; which is able to read and write XAC and XSM files.

One thing to consider is the automatic shadow casting that the engine has to do, the game engine by itself has to calculate a cylinder of collision to use as a base for "clicking" on things, such as clicking on a soldier, this should work well, until you are introduced with dynamic collision meshes, of course we won't cover those because your game will not need it.

An implementation will need to use special effects to animate the wake of the ships, since the movement of water is simulated using FX shaders, not of the "sea water", but of the wake itself.

Graphical cultures are hardcoded (with notable exceptions being CSA); otherwise, unit filenames take the following forms:
- `Generic_xxxx`: `infantry`, `cavalry`, `artillery`, etc.
- `TAGword_xxxx`: For example, `CHInese_cavalry` (in the game files, `Chinese_Cavalry` can be found instead).
- `GCword_xxxx`: For example, `European_Cavalry` (not used in vanilla, but still somewhat recognized)
- `xxxx_Early`: Level 1 - 2, where `xxxx = building_type`.
- `xxxx_Mid`: Same as above, but level 3 - 4.
- `xxxx_Late`: Same as above, but level 5 - 6.
- `xxxx_Full`: Building with unit standing on them (if naval = a ship, else = a regiment)
- `xxxx_Empty`: Opposite of above.
- `xxxx_yyyy_Full`/`xxxx_yyyy_Early`: (Where `yyyy = Early/Mid/Late`) Unused.
- Other: Hardcoded game models, examples are `wake`, `flag`, `floating_flag`, `shadow_addon`.

Shadow addon is just an EmotionFX addon you install on Autodesk Maya which allows you to add precomputed shadows to models easily. But we don't have the addon, unfortunely - but the game uses it for each standing army model atleast (as par of the attachment process).

The binary structure of both XAC and XSM files are in this word file:

### XAC
```
https://web.archive.org/web/20140816034951/https://dl.dropboxusercontent.com/u/60681258/xr/xac.txt
vec3d = float x, float y, float z
vec4d = float x, float y, float z, float w
quat = float x, float y, float z, float w
string = uint32 len, char[len]
matrix44 = vec4d col1, vec4d col2, vec4d col3, vec4d pos
file:
	byte magic[4] = 58 41 43 20 ("XAC ")
	byte majorVersion = 1
	byte minorVersion = 0
	byte bBigEndian
	byte multiplyOrder
	chunk[...]
		int32 chunkType
		int32 length (sometimes incorrect!)
		int32 version
		byte data[length]
chunk 7: metadata (v2)
	uint32 repositionMask
		1 = repositionPos
		2 = repositionRot
		4 = repositionScale
	int32 repositioningNode
	byte exporterMajorVersion
	byte exporterMinorVersion
	byte unused[2]
	float retargetRootOffset
	string sourceApp
	string origFileName
	string exportDate
	string actorName
chunk B: node hierarchy (v1)
	int32 numNodes
	int32 numRootNodes (number of nodes with parentId = -1)
	NodeData[numNodes]
		quat rotation
		quat scaleRotation
		vec3d position
		vec3d scale
		float unused[3]
		int32 -1 (?)
		int32 -1 (?)
		int32 parentNodeId (index of parent node or -1 for root nodes)
		int32 numChildNodes (number of nodes with parentId = this node's index)
		int32 bIncludeInBoundsCalc
		matrix44 transform
		float fImportanceFactor
		string name
chunk D: material totals (v1)
	int32 numTotalMaterials
	int32 numStandardMaterials
	int32 numFxMaterials
chunk 3: material definition (v2)
	vec4d ambientColor
	vec4d diffuseColor
	vec4d specularColor
	vec4d emissiveColor
	float shine
	float shineStrength
	float opacity
	float ior
	byte bDoubleSided
	byte bWireframe
	byte unused
	byte numLayers
	string name
	Layer[numLayers]:
		float amount
		float uOffset
		float vOffset
		float uTiling
		float vTiling
		float rotationInRadians
		int16 materialId (index of the material this layer belongs to = number of preceding chunk 3's)
		byte mapType
		byte unused
		string texture
chunk 1: mesh (v1)
	int32 nodeId
	int32 numInfluenceRanges
	int32 numVertices (total number of vertices of submeshes)
	int32 numIndices  (total number of indices of submeshes)
	int32 numSubMeshes
	int32 numAttribLayers
	byte bIsCollisionMesh (each node can have 1 visual mesh and 1 collision mesh)
	byte pad[3]
	VerticesAttribute[numAttribLayers]
		int32 type (determines meaning of data)
			0 = positions (vec3d)
			1 = normals (vec3d)
			2 = tangents (vec4d)
			3 = uv coords (vec2d)
			4 = 32-bit colors (uint32)
			5 = influence range indices (uint32) - index into the InfluenceRange[] array of chunk 2, indicating the bones that affect it
			6 = 128-bit colors
			typically: 1x positions, 1x normals, 2x tangents, 2x uv, 1x colors, 1x influence range indices
		int32 attribSize (size of 1 attribute, for 1 vertex)
		byte bKeepOriginals
		byte bIsScaleFactor
		byte pad[2]
		byte data[numVertices * attribSize]
	SubMesh[numSubMeshes]
		int32 numIndices
		int32 numVertices
		int32 materialId
		int32 numBones
		int32 relativeIndices[numIndices] (actual index = relative index + total number of vertices of preceding submeshes. each group of 3 sequential indices (vertices) defines a polygon)
		int32 boneIds[numBones] (unused)
chunk 2: skinning (v3)
	int32 nodeId
	int32 numLocalBones (number of distinct boneId's in InfluenceData)
	int32 numInfluences
	byte bIsForCollisionMesh
	byte pad[3]
	InfluenceData[numInfluences]
		float fWeight (0..1)   (for every vertex, the resulting transformed position is calculated for every influencing bone;
		int16 boneId            the final position is the weighted average of these positions using fWeight as weight)
		byte pad[2]
	InfluenceRange[bIsForCollisionMesh ? nodes[nodeId].colMesh.numInfluenceRanges : nodes[nodeId].visualMesh.numInfluenceRanges]
		int32 firstInfluenceIndex (index into InfluenceData)
		int32 numInfluences (number of InfluenceData entries relevant for one or more vertices, starting at firstInfluenceIndex)
chunk C: morph targets (v1)
	int32 numMorphTargets
	int32 lodMorphTargetIdx (presumably always 0; this is the index of a *collection* of numMorphTargets morph targets, not an
							 individual target, and an EmoActor only has one such collection)
	MorphTarget[numMorphTargets]
		float fRangeMin (at runtime, fMorphAmount must be >= fRangeMin)
		float fRangeMax (at runtime, fMorphAmount must be <= fRangeMax)
		int32 lodLevel (LOD of visual mesh; presumably always 0)
		int32 numDeformations
		int32 numTransformations
		int32 phonemeSetBitmask (indicates which phonemes the morph target can be used for - facial animation)
			0x1: neutral
			0x2: M, B, P, X
			0x4: AA, AO, OW
			0x8: IH, AE, AH, EY, AY, H
			0x10: AW
			0x20: N, NG, CH, J, DH, D, G, T, K, Z, ZH, TH, S, SH
			0x40: IY, EH, Y
			0x80: UW, UH, OY
			0x100: F, V
			0x200: L, EL
			0x400: W
			0x800: R, ER
		string name
		Deformation[numDeformations]
			int32 nodeId
			float fMinValue
			float fMaxValue
			int32 numVertices
			DeformVertex16 positionOffsets[numVertices]
				uint16 x (fXOffset = fMinValue + (fMaxValue - fMinValue)*(x / 65535); vecDeformedPos.fX = vecPos.fX + fXOffset*fMorphAmount)
				uint16 y
				uint16 z
			DeformVertex8 normalOffsets[numVertices]
				byte x (fXOffset = x/127.5 - 1.0; vecDeformedNormal.fX = vecNormal.fX + fXOffset * fMorphAmount)
				byte y
				byte z
			DeformVertex8 tangentOffsets[numVertices] (offsets for first tangent)
			uint32 vertexIndices[numVertices] (index of the node's visual mesh vertex which the offsets apply to)
		Transformation[numTransformations] (appears to be unused, i.e. numTransformations = 0)
			int32 nodeId
			quat rotation
			quat scaleRotation
			vec3d pos
			vec3d scale
```

## XSM

EmotionFX is meant to capture real-life motion using motion capture (oh you don't tell me?). This means that storage of such data is done in an easy to read and understand animation file format that has absolutely no incongruencies and shadow binary blobs that are hardcoded into the game engine itself... except, it is.

Every model has an `UniversalManip`, which is an universal manipulator (no way) BUT, if the game engine can't find one in the model it MUST insert it to the model BEFORE loading, otherwise you'll be met with rigging issues. You can't make a boneless XAC, but you can make an animationless XAC, this is because XSM animations failing to load means the model will not move at all.

```
https://web.archive.org/web/20140816034945/https://dl.dropboxusercontent.com/u/60681258/xr/xsm.txt
vec3d = float x, float y, float z
quat16 = int16 x, int16 y, int16 z, int16 w; fX = x / 32767
string = uint32 len, char[len]
file:
	byte magic[4] = 58 53 4D 20 ("XSM ")
	byte majorVersion = 1
	byte minorVersion = 0
	byte bBigEndian
	byte pad
	chunk[...]
		int32 chunkType
		int32 length
		int32 version
		byte data[length]
chunk C9: metadata (v2)
	float unused = 1.0f
	float fMaxAcceptableError
	int32 fps
	byte exporterMajorVersion
	byte exporterMinorVersion
	byte pad[2]
	string sourceApp
	string origFileName
	string exportDate
	string motionName
chunk CA: bone animation (v2)
	int32 numSubMotions
	SkeletalSubMotion[numSubMotions]:
		quat16 poseRot
		quat16 bindPoseRot
		quat16 poseScaleRot
		quat16 bindPoseScaleRot
		vec3D posePos
		vec3D poseScale
		vec3D bindPosePos
		vec3D bindPoseScale
		int32 numPosKeys
		int32 numRotKeys
		int32 numScaleKeys
		int32 numScaleRotKeys
		float fMaxError
		string nodeName
		// fTime of first item in each array must be 0
		PosKey[numPosKeys]:
			vec3d pos
			float fTime
		RotKey[numRotKeys]:
			quat16 rot
			float fTime
		ScaleKey[numScaleKeys]:
			vec3d scale
			float fTime
		ScaleRotKey[numScaleRotKeys]:
			quat16 rot
			float fTime
```
