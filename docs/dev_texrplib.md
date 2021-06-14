# Developer Documentation | C++ API

To use the C++ API compile from source or download prebuild binaries from the release page (will come with the first real release). The file is name `TexRPLib_SDK.zip`.

### Returned pointers

Many function of TexRPLib will return a pointer to an `TexRPLib::IGPU...`  interface object. Make sure to delete the pointer after usage by using the `TexRPDestroy(ptrObject)` macro.

### function TexRPLib::enumGpuInterfaces

``` c++
// Parameter - index: Zero based index of the gpu to retrive interface to
// Return value: Pointer to GPU Interface / nullptr
TEXRP_INTERFACE TexRPLib::IGPUInterface* TexRPLib::enumGpuInterfaces(unsigned int index);
```

### class TexRPLib::IGPUInterface

```c++
// Return value: Name of the gpu (wide char)
virtual const wchar_t* TexRPLib::IGPUInterface::getGPUName();
```

```c++
// Return value: Local unique ID (LUID) of the GPU
virtual LUID TexRPLib::IGPUInterface::getGPUUniqueId();
```

```c++
// Return value: Number of bytes currently availible on the GPU
virtual UINT64 TexRPLib::IGPUInterface::getGPUFreeVRAM();
```

```c++
// Parameter - numBytes: Number of bytes to reserved for the application
// Return value: true when reservation succeeded
virtual bool TexRPLib::IGPUInterface::setVRAMReservation(UINT64 numByte);
```

```c++
// Return value: number of bytes reserved for the application
virtual UINT64 TexRPLib::IGPUInterface::getVRAMReservation();
```

```c++
// Return value: Pointer to newly created context
virtual TexRPLib::IGPUContext* createContex();
```

### class TexRPLib::IGPUContext

```c++
// Paramter - format: Input DXGI format
// Return value: true when format is supported as Shader Resource on your GPU
virtual bool TexRPLib::IGPUContext::checkInputSupport(DXGI_FORMAT format);
```

```c++
// Paramter - format: Input DXGI format
// Return value: true when format is supported as Render Target on your GPU
virtual bool TexRPLib::IGPUContext::checkOutputSupport(DXGI_FORMAT format);
```

```c++
// Return value: Pointer to newly created texture stack
virtual TexRPLib::IGPUTextureStack* TexRPLib::IGPUContext::createTextureStack();
```

```c++
// Parameter - modellPath: Path to the modell 
// Return Value: Pointer to newly create modell object
virtual TexRPLib::IGPUGeometryModell* TexRPLib::IGPUContext::openModell(LPCSTR modellPath);
```

```c++
// Parameter - arrMasks: Array of pointers to masks
// Parameter - ptrTextureStack: Pointer to GPU stack to use
// Parameter - outputIndex: Index of output texture (Stack)
// Parameter - mergCount: Number of textures to merge
// Parameter - arrSourceIndices: Array of indicies of source textures (Stack)
// Return Value: true on succeed
virtual bool TexRPLib::IGPUContext::mergTextures(TexRPLib::IGPUMask** arrMasks, TexRPLib::IGPUTextureStack* ptrTextureStack, UINT outputIndex, UINT mergCount, UINT* arrSourceIndices);
```

### class TexRPLib::IGPUGeometryModell

```c++
// Return value: Number of submodells in the geometry modell
virtual UINT TexRPLib::IGPUGeometryModell::getSubmodellCount();
```

```c++
// Parameter - index: Index of submodell to query information on
// Return value: Mesh name of submodell
virtual LPCSTR TexRPLib::IGPUGeometryModell::getSubmodellName(UINT index);
```

```c++
// Parameter - index: Index of submodell to query information on
// Return value: Material name of submodell
virtual LPCSTR TexRPLib::IGPUGeometryModell::getSubmodellMatterialName(UINT index);
```

```c++
// Parameter - index: Index of submodell to query information on
// Return value: Material name of submodell
virtual UINT TexRPLib::IGPUGeometryModell::getUVSetIndex(UINT index);
```

```c++
// Parameter - index: Index of submodell to query information on
// Return value: Number of verticies of submodell
virtual UINT TexRPLib::IGPUGeometryModell::getVerticesCount(UINT index);
```

```c++
// Parameter - index: Index of submodell to create mask for
// Parameter - sideLength: Resolution of the side of the mask (Real Mask will be: sideLength * sideLength)
// Return value: Newly create mask object
virtual TexRPLib::IGPUMask* TexRPLib::IGPUGeometryModell::createMask(UINT index, UINT sideLength);
```

### class TexRPLib::IGPUTextureStack

```c++
// Parameter - width: Maximum width of a single texture
// Parameter - height: Minimum height of a single texture
// Parameter - bpp: Maximum BPP of a single texture (32 / 64)
// Parameter - count: Maximum texture count
// Return value: true on succeed
virtual bool reset(UINT width, UINT height, UINT bpp, UINT count);
```

```c++
// Parameter - path: Path to texture to load
// Return value: Index to texture or UINT_MAX on error
virtual UINT loadFromDisk(LPCSTR path);
```

```c++
// Parameter - dummyPath: Dummy path to be used as output path when saving the textures
// Parameter - reference: Reference index to copy settings from (width, height, format, bpp, ...)
// Return value:Index to texture or UINT_MAX on error
virtual UINT createEmpty(LPCSTR dummyPath, UINT reference);
```

```c++
// Parameter - index: Index of texture to be cleared
// Return value: true on succeed
virtual bool clearTexture(UINT index);
```

```c++
// Parameter - index: Index to texture to be saved
// Return value: true on succeed
virtual bool safeToDisk(UINT index);
```

```c++
// Parameter - index: Index to texture to be renamed
// Parameter - newName: New name of texture
// Return value: true on succeed
virtual bool rename(UINT index, LPCSTR newName);
```

