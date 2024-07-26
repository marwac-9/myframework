#include "Shader.h"
#include <GL/glew.h>
#include <stdio.h>
#include "MyMathLib.h"
#include "ShaderBlock.h"
#include <memory>

/*

struct block_info_t
{
	std::string name;
	BlockType type;
	GLint index;
	GLint size;
	std::vector<uniform_info_t> uniforms;
};

*/
Shader::Shader(unsigned int ID, const std::string& sname, const ShaderPaths& sp)
{
	shaderID = ID;
	name = sname;
	shaderPaths = sp;
}

Shader::~Shader()
{
	glDeleteProgram(shaderID);
}

void Shader::LoadUniforms()
{
	std::unordered_map<std::string, uniform_info_t> uniforms;

	GLint numBlocks;
	glGetProgramiv(shaderID, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);

	std::vector<std::string> nameList;
	nameList.reserve(numBlocks);
	for (int blockIx = 0; blockIx < numBlocks; ++blockIx)
	{
		GLint nameLen;
		glGetActiveUniformBlockiv(shaderID, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);

		GLint binding;
		glGetActiveUniformBlockiv(shaderID, blockIx, GL_UNIFORM_BLOCK_BINDING, &binding);

		GLint blockSize;
		glGetActiveUniformBlockiv(shaderID, blockIx, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

		GLint nrOfUniformsInTheBlock;
		glGetActiveUniformBlockiv(shaderID, blockIx, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &nrOfUniformsInTheBlock);

		GLint* listOfUniformIndices = new GLint[nrOfUniformsInTheBlock];
		glGetActiveUniformBlockiv(shaderID, blockIx, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, listOfUniformIndices);

		std::vector<GLint> offsets(nrOfUniformsInTheBlock);
		glGetActiveUniformsiv(shaderID, nrOfUniformsInTheBlock, (GLuint*)listOfUniformIndices, GL_UNIFORM_OFFSET, offsets.data());

		std::vector<GLint> counts(nrOfUniformsInTheBlock);
		glGetActiveUniformsiv(shaderID, nrOfUniformsInTheBlock, (GLuint*)listOfUniformIndices, GL_UNIFORM_SIZE, counts.data());

		std::vector<GLint> types(nrOfUniformsInTheBlock);
		glGetActiveUniformsiv(shaderID, nrOfUniformsInTheBlock, (GLuint*)listOfUniformIndices, GL_UNIFORM_TYPE, types.data());

		std::vector<GLint> nameLengths(nrOfUniformsInTheBlock);
		glGetActiveUniformsiv(shaderID, nrOfUniformsInTheBlock, (GLuint*)listOfUniformIndices, GL_UNIFORM_NAME_LENGTH, nameLengths.data());

		std::vector<GLint> uniformBlockIndexes(nrOfUniformsInTheBlock);
		glGetActiveUniformsiv(shaderID, nrOfUniformsInTheBlock, (GLuint*)listOfUniformIndices, GL_UNIFORM_BLOCK_INDEX, uniformBlockIndexes.data()); //which uniform block the uniform belongs to, should be -1 for all since I use the list of indices from this block

		std::vector<GLint> arrayStrides(nrOfUniformsInTheBlock);
		glGetActiveUniformsiv(shaderID, nrOfUniformsInTheBlock, (GLuint*)listOfUniformIndices, GL_UNIFORM_ARRAY_STRIDE, arrayStrides.data());

		std::vector<GLint> matrixStrides(nrOfUniformsInTheBlock);
		glGetActiveUniformsiv(shaderID, nrOfUniformsInTheBlock, (GLuint*)listOfUniformIndices, GL_UNIFORM_MATRIX_STRIDE, matrixStrides.data());

		std::vector<GLint> isRowMajors(nrOfUniformsInTheBlock);
		glGetActiveUniformsiv(shaderID, nrOfUniformsInTheBlock, (GLuint*)listOfUniformIndices, GL_UNIFORM_IS_ROW_MAJOR, isRowMajors.data());

		std::vector<GLint> atomicCounterBufferIndexes(nrOfUniformsInTheBlock);
		glGetActiveUniformsiv(shaderID, nrOfUniformsInTheBlock, (GLuint*)listOfUniformIndices, GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX, atomicCounterBufferIndexes.data());

		if (nrOfUniformsInTheBlock != 0)
		{
			for (GLint i = 0; i < nrOfUniformsInTheBlock; ++i)
			{
				std::vector<GLchar> uniformName;
				uniformName.resize(nameLengths[i]);
				GLsizei tempReturnedSize;
				glGetActiveUniformName(shaderID, listOfUniformIndices[i], nameLengths[i], &tempReturnedSize, uniformName.data());
				uniform_info_t uniform_info = {};
				uniform_info.location = listOfUniformIndices[i];
				uniform_info.count = counts[i];
				uniform_info.type = types[i];
				uniform_info.offset = offsets[i];
				uniform_info.blockIndex = uniformBlockIndexes[i];
				uniform_info.isRowMajor = isRowMajors[i];
				uniform_info.arrayStride = arrayStrides[i];
				uniform_info.matrixStride = matrixStrides[i];
				uniform_info.atomicCounterBufferIndex = atomicCounterBufferIndexes[i];

				switch (types[i])
				{
				case GL_FLOAT:				uniform_info.size = 4; break;
				case GL_FLOAT_VEC2:			uniform_info.size = 4 * 2; break;
				case GL_FLOAT_VEC3:			uniform_info.size = 4 * 3; break;
				case GL_FLOAT_VEC4:			uniform_info.size = 4 * 4; break;
				case GL_DOUBLE:				uniform_info.size = 2 * 4; break;
				case GL_DOUBLE_VEC2:		uniform_info.size = 2 * 4 * 2; break;
				case GL_DOUBLE_VEC3:		uniform_info.size = 2 * 4 * 3; break;
				case GL_DOUBLE_VEC4:		uniform_info.size = 2 * 4 * 4; break;
				case GL_INT:				uniform_info.size = 4; break;
				case GL_INT_VEC2:			uniform_info.size = 4 * 2; break;
				case GL_INT_VEC3:			uniform_info.size = 4 * 3; break;
				case GL_INT_VEC4:			uniform_info.size = 4 * 4; break;
				case GL_UNSIGNED_INT:		uniform_info.size = 4; break;
				case GL_UNSIGNED_INT_VEC2:	uniform_info.size = 4 * 2; break;
				case GL_UNSIGNED_INT_VEC3:	uniform_info.size = 4 * 3; break;
				case GL_UNSIGNED_INT_VEC4:	uniform_info.size = 4 * 4; break;
				case GL_BOOL:				uniform_info.size = 4; break;
				case GL_BOOL_VEC2:			uniform_info.size = 4 * 2; break;
				case GL_BOOL_VEC3:			uniform_info.size = 4 * 3; break;
				case GL_BOOL_VEC4:			uniform_info.size = 4 * 4; break;
				case GL_FLOAT_MAT2:			uniform_info.size = 4 * 2 * 2; break;
				case GL_FLOAT_MAT3:			uniform_info.size = 4 * 3 * 3; break;
				case GL_FLOAT_MAT4:			uniform_info.size = 4 * 4 * 4; break;
				case GL_FLOAT_MAT2x3:		uniform_info.size = 4 * 2 * 3; break;
				case GL_FLOAT_MAT2x4:		uniform_info.size = 4 * 2 * 4; break;
				case GL_FLOAT_MAT3x2:		uniform_info.size = 4 * 3 * 2; break;
				case GL_FLOAT_MAT3x4:		uniform_info.size = 4 * 3 * 4; break;
				case GL_FLOAT_MAT4x2:		uniform_info.size = 4 * 4 * 2; break;
				case GL_FLOAT_MAT4x3:		uniform_info.size = 4 * 4 * 3; break;
				case GL_DOUBLE_MAT2:		uniform_info.size = 2 * 4 * 2 * 2; break;
				case GL_DOUBLE_MAT3:		uniform_info.size = 2 * 4 * 3 * 3; break;
				case GL_DOUBLE_MAT4:		uniform_info.size = 2 * 4 * 4 * 4; break;
				case GL_DOUBLE_MAT2x3:		uniform_info.size = 2 * 4 * 2 * 3; break;
				case GL_DOUBLE_MAT2x4:		uniform_info.size = 2 * 4 * 2 * 4; break;
				case GL_DOUBLE_MAT3x2:		uniform_info.size = 2 * 4 * 3 * 2; break;
				case GL_DOUBLE_MAT3x4:		uniform_info.size = 2 * 4 * 3 * 4; break;
				case GL_DOUBLE_MAT4x2:		uniform_info.size = 2 * 4 * 4 * 2; break;
				case GL_DOUBLE_MAT4x3:		uniform_info.size = 2 * 4 * 4 * 3; break;
				default:
					break;
				}

				uniforms.emplace(std::make_pair(std::string(uniformName.begin(), uniformName.end() - 1), uniform_info));
			}
		}
		GLint isBlockInVertexShader;
		glGetActiveUniformBlockiv(shaderID, blockIx, GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER, &isBlockInVertexShader);

		GLint isBlockInTesselationControlShader;
		glGetActiveUniformBlockiv(shaderID, blockIx, GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER, &isBlockInTesselationControlShader);

		GLint isBlockInTesselationEvaluationShader;
		glGetActiveUniformBlockiv(shaderID, blockIx, GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER, &isBlockInTesselationEvaluationShader);

		GLint isBlockInGeometryShader;
		glGetActiveUniformBlockiv(shaderID, blockIx, GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER, &isBlockInGeometryShader);

		GLint isBlockInFragmentShader;
		glGetActiveUniformBlockiv(shaderID, blockIx, GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER, &isBlockInFragmentShader);

		GLint isBlockInComputeShader;
		glGetActiveUniformBlockiv(shaderID, blockIx, GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER, &isBlockInComputeShader);

		std::vector<GLchar> name;
		name.resize(nameLen);
		glGetActiveUniformBlockName(shaderID, blockIx, nameLen, NULL, &name[0]);

		nameList.push_back(std::string());
		nameList.back().assign(name.begin(), name.end() - 1);
	}
}

bool Shader::HasUniformBuffer(int index)
{
	for (auto buffer : globalUniformBuffers)
	{
		if (buffer->index == index) return true;
	}

	for (auto buffer : materialUniformBuffers)
	{
		if (buffer->index == index) return true;
	}

	for (auto buffer : objectUniformBuffers)
	{
		if (buffer->index == index) return true;
	}

	return false;
}

bool Shader::HasShaderStorageBuffer(int index)
{
	for (auto buffer : globalShaderStorageBuffers)
	{
		if (buffer->index == index) return true;
	}

	for (auto buffer : materialShaderStorageBuffers)
	{
		if (buffer->index == index) return true;
	}

	for (auto buffer : objectShaderStorageBuffers)
	{
		if (buffer->index == index) return true;
	}

	return false;
}

void Shader::Clear()
{
	globalUniformBuffers.clear();
	materialUniformBuffers.clear();
	objectUniformBuffers.clear();
	globalShaderStorageBuffers.clear();
	materialShaderStorageBuffers.clear();
	objectShaderStorageBuffers.clear();
	outputs.clear();
	samplers.clear();
	attributes.clear();
}

void Shader::Bind()
{
	if (currentID != shaderID)
	{
		currentID = shaderID;
		glUseProgram(shaderID);
		if (renderTargets.size() > 0)
		{
			glDrawBuffers(renderTargets.size(), &renderTargets[0]);
		}
	}
}

void Shader::Execute()
{
	Bind();
}

void Shader::SetRenderTarget(int slot, GLenum renderTarget)
{
	if (slot < renderTargets.size())
	{
		int index = FindRenderTargetIndex(renderTarget);
		if (index != -1) renderTargets[index] = 0;
		renderTargets[slot] = renderTarget;
	}
	else
	{
		renderTargets.resize(slot + 1, 0);
		renderTargets[slot] = renderTarget;
	}
}

int Shader::FindRenderTargetIndex(GLenum renderTarget)
{
	for (size_t i = 0; i < renderTargets.size(); i++)
	{
		if (renderTargets[i] == renderTarget)
		{
			return i;
		}
	}
	return -1;
}

unsigned int Shader::currentID = -1;
//GL_SHADER_TYPE 0x8B4F
//GL_FLOAT_VEC2 0x8B50
//GL_FLOAT_VEC3 0x8B51
//GL_FLOAT_VEC4 0x8B52
//GL_INT_VEC2 0x8B53
//GL_INT_VEC3 0x8B54
//GL_INT_VEC4 0x8B55
//GL_BOOL 0x8B56
//GL_BOOL_VEC2 0x8B57
//GL_BOOL_VEC3 0x8B58
//GL_BOOL_VEC4 0x8B59
//GL_FLOAT_MAT2 0x8B5A
//GL_FLOAT_MAT3 0x8B5B
//GL_FLOAT_MAT4 0x8B5C
//GL_SAMPLER_1D 0x8B5D
//GL_SAMPLER_2D 0x8B5E
//GL_SAMPLER_3D 0x8B5F
//GL_SAMPLER_CUBE 0x8B60
//#define GL_INT 0x1404
//#define GL_UNSIGNED_INT 0x1405
//#define GL_FLOAT 0x1406