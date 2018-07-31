#ifndef __C_COPENGL_VERTEX_DECLARATION_H_INCLUDED__
#define __C_COPENGL_VERTEX_DECLARATION_H_INCLUDED__

#include "RenderEngines/General/CIrrVertexDeclaration.h"
#include "COpenGLHardwareBuffer.h"

#include <vector>

namespace irr
{
    namespace video
    {
        class GLSLGpuShader;

        struct OpenGLVERTEXELEMENT
        {
            u16     Stream;     // Stream index
            u16     Offset;     // Offset in the stream in bytes
            GLint   Type;       // Data type
            GLint   Size;       // Data size
            GLint   Usage;      // Semantics
            u8      Normalized; // Normal Value
            u32     InstanceDivisor;
            bool    PreInstance;//
        };

        class COpenGLVertexDeclaration
            : public VertexDeclaration
        {
            core::array<OpenGLVERTEXELEMENT> VertexDeclarationMap;
            std::vector<irr::u32> VertexPitch;
            COpenGLDriver* Driver;

        public:
            COpenGLVertexDeclaration(COpenGLDriver* driver);
            virtual ~COpenGLVertexDeclaration();

            core::array<OpenGLVERTEXELEMENT> const& getVertexDeclaration();
            void createInputLayout(const COpenGLHardwareBuffer * hwBuffer);
            irr::u32 GetVertexPitch(irr::u8 inputSlot) const { return VertexPitch[inputSlot]; }
        };
    }
}

#endif // __C_COPENGL_VERTEX_DECLARATION_H_INCLUDED__