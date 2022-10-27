#pragma once

#include "Utils/Core.h"
#include <Brainfryer/Utils/Log.h>

#include <Brainfryer/Envs/DX12/DX12.h>

#include <cstdint>

namespace Brainfryer::Editor::DX12
{
	using namespace ::Brainfryer::DX12;

	enum D3DCOMPILER_STRIP_FLAGS
	{
		D3DCOMPILER_STRIP_REFLECTION_DATA = 0x00000001,
		D3DCOMPILER_STRIP_DEBUG_INFO      = 0x00000002,
		D3DCOMPILER_STRIP_TEST_BLOBS      = 0x00000004,
		D3DCOMPILER_STRIP_PRIVATE_DATA    = 0x00000008,
		D3DCOMPILER_STRIP_ROOT_SIGNATURE  = 0x00000010,
		D3DCOMPILER_STRIP_FORCE_DWORD     = 0x7fffffff,
	};

	enum D3D_BLOB_PART
	{
		D3D_BLOB_INPUT_SIGNATURE_BLOB,
		D3D_BLOB_OUTPUT_SIGNATURE_BLOB,
		D3D_BLOB_INPUT_AND_OUTPUT_SIGNATURE_BLOB,
		D3D_BLOB_PATCH_CONSTANT_SIGNATURE_BLOB,
		D3D_BLOB_ALL_SIGNATURE_BLOB,
		D3D_BLOB_DEBUG_INFO,
		D3D_BLOB_LEGACY_SHADER,
		D3D_BLOB_XNA_PREPASS_SHADER,
		D3D_BLOB_XNA_SHADER,
		D3D_BLOB_PDB,
		D3D_BLOB_PRIVATE_DATA,
		D3D_BLOB_ROOT_SIGNATURE,
		D3D_BLOB_DEBUG_NAME,
		D3D_BLOB_TEST_ALTERNATE_SHADER = 0x8000,
		D3D_BLOB_TEST_COMPILE_DETAILS,
		D3D_BLOB_TEST_COMPILE_PERF,
		D3D_BLOB_TEST_COMPILE_REPORT,
	};

	using D3D_SHADER_DATA = struct _D3D_SHADER_DATA
	{
		LPCVOID pBytecode;
		SIZE_T  BytecodeLength;
	};

	static constexpr std::uint32_t D3DCOMPILE_DEBUG                              = 1 << 0;
	static constexpr std::uint32_t D3DCOMPILE_SKIP_VALIDATION                    = 1 << 1;
	static constexpr std::uint32_t D3DCOMPILE_SKIP_OPTIMIZATION                  = 1 << 2;
	static constexpr std::uint32_t D3DCOMPILE_PACK_MATRIX_ROW_MAJOR              = 1 << 3;
	static constexpr std::uint32_t D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR           = 1 << 4;
	static constexpr std::uint32_t D3DCOMPILE_PARTIAL_PRECISION                  = 1 << 5;
	static constexpr std::uint32_t D3DCOMPILE_FORCE_VS_SOFTWARE_NO_OPT           = 1 << 6;
	static constexpr std::uint32_t D3DCOMPILE_FORCE_PS_SOFTWARE_NO_OPT           = 1 << 7;
	static constexpr std::uint32_t D3DCOMPILE_NO_PRESHADER                       = 1 << 8;
	static constexpr std::uint32_t D3DCOMPILE_AVOID_FLOW_CONTROL                 = 1 << 9;
	static constexpr std::uint32_t D3DCOMPILE_PREFER_FLOW_CONTROL                = 1 << 10;
	static constexpr std::uint32_t D3DCOMPILE_ENABLE_STRICTNESS                  = 1 << 11;
	static constexpr std::uint32_t D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY     = 1 << 12;
	static constexpr std::uint32_t D3DCOMPILE_IEEE_STRICTNESS                    = 1 << 13;
	static constexpr std::uint32_t D3DCOMPILE_OPTIMIZATION_LEVEL0                = 1 << 14;
	static constexpr std::uint32_t D3DCOMPILE_OPTIMIZATION_LEVEL1                = 0;
	static constexpr std::uint32_t D3DCOMPILE_OPTIMIZATION_LEVEL2                = 1 << 14 | 1 << 15;
	static constexpr std::uint32_t D3DCOMPILE_OPTIMIZATION_LEVEL3                = 1 << 15;
	static constexpr std::uint32_t D3DCOMPILE_RESERVED16                         = 1 << 16;
	static constexpr std::uint32_t D3DCOMPILE_RESERVED17                         = 1 << 17;
	static constexpr std::uint32_t D3DCOMPILE_WARNINGS_ARE_ERRORS                = 1 << 18;
	static constexpr std::uint32_t D3DCOMPILE_RESOURCES_MAY_ALIAS                = 1 << 19;
	static constexpr std::uint32_t D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES = 1 << 20;
	static constexpr std::uint32_t D3DCOMPILE_ALL_RESOURCES_BOUND                = 1 << 21;
	static constexpr std::uint32_t D3DCOMPILE_DEBUG_NAME_FOR_SOURCE              = 1 << 22;
	static constexpr std::uint32_t D3DCOMPILE_DEBUG_NAME_FOR_BINARY              = 1 << 23;

	static constexpr std::uint32_t D3DCOMPILE_EFFECT_CHILD_EFFECT   = 1 << 0;
	static constexpr std::uint32_t D3DCOMPILE_EFFECT_ALLOW_SLOW_OPS = 1 << 1;

	static constexpr std::uint32_t D3DCOMPILE_FLAGS2_FORCE_ROOT_SIGNATURE_LATEST = 0;
	static constexpr std::uint32_t D3DCOMPILE_FLAGS2_FORCE_ROOT_SIGNATURE_1_0    = 1 << 4;
	static constexpr std::uint32_t D3DCOMPILE_FLAGS2_FORCE_ROOT_SIGNATURE_1_1    = 1 << 5;

	static constexpr std::uint32_t D3DCOMPILE_SECDATA_MERGE_UAV_SLOTS         = 0x1;
	static constexpr std::uint32_t D3DCOMPILE_SECDATA_PRESERVE_TEMPLATE_SLOTS = 0x2;
	static constexpr std::uint32_t D3DCOMPILE_SECDATA_REQUIRE_TEMPLATE_MATCH  = 0x4;

	static constexpr std::uint32_t D3D_DISASM_ENABLE_COLOR_CODE            = 0x00000001;
	static constexpr std::uint32_t D3D_DISASM_ENABLE_DEFAULT_VALUE_PRINTS  = 0x00000002;
	static constexpr std::uint32_t D3D_DISASM_ENABLE_INSTRUCTION_NUMBERING = 0x00000004;
	static constexpr std::uint32_t D3D_DISASM_ENABLE_INSTRUCTION_CYCLE     = 0x00000008;
	static constexpr std::uint32_t D3D_DISASM_DISABLE_DEBUG_INFO           = 0x00000010;
	static constexpr std::uint32_t D3D_DISASM_ENABLE_INSTRUCTION_OFFSET    = 0x00000020;
	static constexpr std::uint32_t D3D_DISASM_INSTRUCTION_ONLY             = 0x00000040;
	static constexpr std::uint32_t D3D_DISASM_PRINT_HEX_LITERALS           = 0x00000080;

	static constexpr std::uint32_t D3D_GET_INST_OFFSETS_INCLUDE_NON_EXECUTABLE = 0x00000001;

	static constexpr std::uint32_t D3D_COMPRESS_SHADER_KEEP_ALL_PARTS = 0x00000001;

	static const ID3DInclude* D3D_COMPILE_STANDARD_FILE_INCLUDE = reinterpret_cast<ID3DInclude*>(1);

	extern "C"
	{
		using pD3DCompile     = HRESULT (*)(LPCVOID pSrcData, SIZE_T SrcDataSize, LPCSTR pFileName, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2, ID3DBlob** ppCode, ID3DBlob** ppErrorMsgs);
		using pD3DPreprocess  = HRESULT (*)(LPCVOID pSrcData, SIZE_T SrcDataSize, LPCSTR pFileName, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, ID3DBlob** ppCodeText, ID3DBlob** ppErrorMsgs);
		using pD3DDisassemble = HRESULT (*)(LPCVOID pSrcData, SIZE_T SrcDataSize, UINT Flags, LPCSTR szComments, ID3DBlob** ppDisassembly);
		HRESULT D3DCompile(LPCVOID pSrcData, SIZE_T SrcDataSize, LPCSTR pSourceName, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2, ID3DBlob** ppCode, ID3DBlob** ppErrorMsgs);
		HRESULT D3DCompile2(LPCVOID pSrcData, SIZE_T SrcDataSize, LPCSTR pSourceName, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2, UINT SecondaryDataFlags, LPCVOID pSecondaryData, SIZE_T SecondaryDataSize, ID3DBlob** ppCode, ID3DBlob** ppErrorMsgs);
		HRESULT D3DCompileFromFile(LPCWSTR pFileName, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2, ID3DBlob** ppCode, ID3DBlob** ppErrorMsgs);
		HRESULT D3DPreprocess(LPCVOID pSrcData, SIZE_T SrcDataSize, LPCSTR pSourceName, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, ID3DBlob** ppCodeText, ID3DBlob** ppErrorMsgs);
		HRESULT D3DGetDebugInfo(LPCVOID pSrcData, SIZE_T SrcDataSize, ID3DBlob** ppDebugInfo);
		HRESULT D3DReflect(LPCVOID pSrcData, SIZE_T SrcDataSize, const GUID& pInterface, void** ppReflector);
		HRESULT D3DReflectLibrary(LPCVOID pSrcData, SIZE_T SrcDataSize, const GUID& riid, LPVOID* ppReflector);
		HRESULT D3DDisassemble(LPCVOID pSrcData, SIZE_T SrcDataSize, UINT Flags, LPCSTR szComments, ID3DBlob** ppDisassembly);
		HRESULT D3DDisassembleRegion(LPCVOID pSrcData, SIZE_T SrcDataSize, UINT Flags, LPCSTR szComments, SIZE_T StartByteOffset, SIZE_T NumInsts, SIZE_T* pFinishByteOffset, ID3DBlob** ppDisassembly);
		HRESULT D3DCreateLinker(struct ID3D11Linker** ppLinker);
		HRESULT D3DLoadModule(LPCVOID pSrcData, SIZE_T cbSrcDataSize, struct ID3D11Module** ppModule);
		HRESULT D3DCreateFunctionLinkingGraph(UINT uFlags, struct ID3D11FunctionLinkingGraph** ppFunctionLinkingGraph);
		HRESULT D3DGetTraceInstructionOffsets(LPCVOID pSrcData, SIZE_T SrcDataSize, UINT Flags, SIZE_T StartInstIndex, SIZE_T NumInsts, SIZE_T* pOffsets, SIZE_T* pTotalInsts);
		HRESULT D3DGetInputSignatureBlob(LPCVOID pSrcData, SIZE_T SrcDataSize, ID3DBlob** ppSignatureBlob);
		HRESULT D3DGetOutputSignatureBlob(LPCVOID pSrcData, SIZE_T SrcDataSize, ID3DBlob** ppSignatureBlob);
		HRESULT D3DGetInputAndOutputSignatureBlob(LPCVOID pSrcData, SIZE_T SrcDataSize, ID3DBlob** ppSignatureBlob);
		HRESULT D3DStripShader(LPCVOID pShaderBytecode, SIZE_T BytecodeLength, UINT uStripFlags, ID3DBlob** ppStrippedBlob);
		HRESULT D3DGetBlobPart(LPCVOID pSrcData, SIZE_T SrcDataSize, D3D_BLOB_PART Part, UINT Flags, ID3DBlob** ppPart);
		HRESULT D3DSetBlobPart(LPCVOID pSrcData, SIZE_T SrcDataSize, D3D_BLOB_PART Part, UINT Flags, LPCVOID pPart, SIZE_T PartSize, ID3DBlob** ppNewShader);
		HRESULT D3DCreateBlob(SIZE_T Size, ID3DBlob** ppBlob);
		HRESULT D3DCompressShaders(UINT uNumShaders, D3D_SHADER_DATA* pShaderData, UINT uFlags, ID3DBlob** ppCompressedData);
		HRESULT D3DDecompressShaders(LPCVOID pSrcData, SIZE_T SrcDataSize, UINT uNumShaders, UINT uStartIndex, UINT* pIndices, UINT uFlags, ID3DBlob** ppShaders, UINT* pTotalShaders);
		HRESULT D3DDisassemble10Effect(struct ID3D10Effect* pEffect, UINT Flags, ID3DBlob** ppDisassembly);
	}

	inline std::vector<std::uint8_t> CompileShader(LPCWSTR pFileName, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2)
	{
		Com<ID3D10Blob> code;
		Com<ID3D10Blob> errorMsg;
		if (!HRVLog(D3DCompileFromFile(pFileName, pDefines, pInclude, pEntrypoint, pTarget, Flags1, Flags2, code, errorMsg)))
		{
			Log::GetOrCreateLogger("Editor")->critical("{}", std::string_view { reinterpret_cast<const char*>(errorMsg->GetBufferPointer()),
			                                                                    reinterpret_cast<const char*>(errorMsg->GetBufferPointer()) + errorMsg->GetBufferSize() });
			return {};
		}

		return std::vector<std::uint8_t>(reinterpret_cast<const std::uint8_t*>(code->GetBufferPointer()),
		                                 reinterpret_cast<const std::uint8_t*>(code->GetBufferPointer()) + code->GetBufferSize());
	}
} // namespace Brainfryer::Editor::DX12