# file(GLOB g3dlite_SRCS *.cpp) # don't need all of them

set(g3dlite_SRCS
	AABox.cpp
	Box.cpp
	Cone.cpp
	Crypto.cpp
	format.cpp
	Matrix3.cpp
	Plane.cpp
	System.cpp
	Triangle.cpp
	Vector3.cpp
	Vector4.cpp
	debugAssert.cpp
	fileutils.cpp
	g3dmath.cpp
	g3dfnmatch.cpp
	prompt.cpp
	stringutils.cpp
	Any.cpp
	BinaryFormat.cpp
	BinaryInput.cpp
	BinaryOutput.cpp
	Capsule.cpp
	CollisionDetection.cpp
	CoordinateFrame.cpp
    ConvexPolyhedron.cpp
	Cylinder.cpp
	Line.cpp
	LineSegment.cpp
	Log.cpp
	Matrix4.cpp
	MemoryManager.cpp
	Quat.cpp
	Random.cpp
	Ray.cpp
	ReferenceCount.cpp
	RegistryUtil.cpp
	Sphere.cpp
	TextInput.cpp
	TextOutput.cpp
	UprightFrame.cpp
	Vector2.cpp
    SplineBase.cpp
)

AppendDefaultIncludeDir()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
)

CreateLibrary("g3dlite" "${g3dlite_SRCS}" "STATIC" "ZLIB" "")

set_target_properties(g3dlite
    PROPERTIES
      FOLDER
        "dep")
