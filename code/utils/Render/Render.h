#pragma once
#include <vector>
#include <unordered_map>
#include "Box.h"
#include "Plane.h"
#include "BoundingBox.h"
#include "Attenuation.h"

namespace mwm
{
	class Matrix4;
}

class Object;
class DirectionalLight;
class SpotLight;
class PointLight;
class FrameBuffer;
class Texture;
class ParticleSystem;

class Render
{
	typedef unsigned int GLuint;
	typedef unsigned int GLenum;

	struct PostHDRBloom
	{
		float gamma;
		float exposure;
		float brightness;
		float contrast;
		float bloomIntensity;
		bool hdrEnabled;
		bool alignmentOffset;
		bool alignmentOffset2;
		bool alignmentOffset3;
		bool bloomEnabled;
		bool alignmentOffset4;
		bool alignmentOffset5;
		bool alignmentOffset6;
	};

public:

	static Render* Instance();

	int drawGeometry(const std::vector<Object*>& objects, const FrameBuffer * geometryBuffer, const GLenum * lightAttachmentsToDraw, const int countOfAttachments);
	int draw(const std::vector<Object*>& objects, const mwm::Matrix4& ViewProjection, const GLuint currentShaderID);
	int drawLight(const FrameBuffer* lightFrameBuffer, const FrameBuffer* geometryBuffer, const GLenum* lightAttachmentsToDraw, const int countOfAttachments);
	void drawSingle(const Object* object, const mwm::Matrix4& ViewProjection, const GLuint currentShaderID);
	int drawPicking(std::unordered_map<unsigned int, Object*>& pickingList, const FrameBuffer* pickingBuffer, const GLenum* attachments, const int countOfAttachments);
	int drawDepth(const std::vector<Object*>& objects, const mwm::Matrix4& ViewProjection, const GLuint currentShaderID);
	int drawCubeDepth(const std::vector<Object*>& objects, const std::vector<mwm::Matrix4>& ViewProjection, const GLuint currentShaderID, const Object* light);
	void drawSkyboxWithClipPlane(const FrameBuffer * lightFrameBuffer, const GLenum * lightAttachmentsToDraw, const int countOfAttachments, Texture* texture, const mwm::Vector4F& plane, const mwm::Matrix4& ViewMatrix);
	void drawSkybox(const FrameBuffer * lightFrameBuffer, const GLenum * lightAttachmentsToDraw, const int countOfAttachments, Texture* texture);
	void drawGSkybox(const FrameBuffer * lightFrameBuffer, const GLenum * lightAttachmentsToDraw, const int countOfAttachments, Texture* texture);
	int drawDirectionalLights(const std::vector<DirectionalLight*>& lights, const std::vector<Object*>& objects, const GLenum* lightBuffers, const int lightBuffersCount, const GLuint bufferToDrawTheLightTO, const std::vector<Texture*>& geometryTextures);
	int drawPointLights(const std::vector<PointLight*>& lights, const std::vector<Object*>& objects, const mwm::Matrix4& ViewProjection, const GLenum* buffers, const int buffersCount, const GLuint fboToDrawTheLightTO, const std::vector<Texture*>& geometryTextures);
	int drawSpotLights(const std::vector<SpotLight*>& lights, const std::vector<Object*>& objects, const mwm::Matrix4& ViewProjection, const GLenum* buffers, const int buffersCount, const GLuint fboToDrawTheLightTO, const std::vector<Texture*>& geometryTextures);
	void drawHDR(Texture* colorTexture, Texture* bloomTexture);
	void drawRegion(int posX, int posY, int width, int height, const Texture* texture);
	void AddPingPongBuffer(int width, int height);
	void AddMultiBlurBuffer(int width, int height, int levels = 4, double scaleX = 0.5, double scaleY = 0.5);
	void GenerateEBOs();
	void UpdateEBOs();
	Texture* PingPongBlur(Texture* sourceTexture, int outputLevel, float blurSize, GLuint blurShader);
	Texture* MultiBlur(Texture* sourceTexture, int outputLevel, float blurSize, GLuint blurShader);
	FrameBuffer* AddDirectionalShadowMapBuffer(int width, int height);
	Plane plane;
	Box box;
	BoundingBox boundingBox;
	FrameBuffer* dirShadowMapBuffer;
	Texture* dirShadowMapTexture;
	PostHDRBloom pb;
private:
	void BlurOnOneAxis(Texture* sourceTexture, GLuint destinationFbo, float offsetxVal, float offsetyVal, GLuint offsetHandle);
	Texture* BlurTexture(Texture* sourceTexture, std::vector<FrameBuffer*> startFrameBuffer, std::vector<FrameBuffer*> targetFrameBuffer, int outputLevel, float blurSize, GLuint shader, int windowWidth, int windowHeight);
	Texture* BlurTextureAtSameSize(Texture* sourceTexture, FrameBuffer* startFrameBuffer, FrameBuffer* targetFrameBuffer, int outputLevel, float blurSize, GLuint shader, int windowWidth, int windowHeight);
    Render();
    ~Render();
	
	FrameBuffer* pingPongBuffers[2];
	std::vector<FrameBuffer*> multiBlurBufferStart;
	std::vector<FrameBuffer*> multiBlurBufferTarget;
	bool onceP = true;
	bool onceS = true;
	bool onceD = true;
	bool onceHDR = true;
	
	struct GBVars
	{
		mwm::Matrix4F MVP;
		mwm::Matrix4F M;
		mwm::Vector4F MaterialProperties;
		mwm::Vector4F MaterialColor;
		mwm::Vector2F tiling;
		unsigned int objectID;
	};

	GBVars gb;

	struct LightVars
	{
		mwm::Vector3F lightInvDir;
		float shadowTransitionSize;
		float outerCutOff;
		float innerCutOff;
		float lightRadius;
		float lightPower;
		mwm::Vector3F lightColor;
		float ambient;
		mwm::Matrix4F depthBiasMVP;
		mwm::Matrix4F MVP;
		mwm::Vector3F lightPosition;
		Attenuation attenuation;
	};

	LightVars lb;

	struct CamVars
	{
		float width;
		float height;
		float near;
		float far;
		mwm::Vector3F cameraPos;
	};

	CamVars cb;

	GLuint uboGBVars;
	GLuint uboLBVars;
	GLuint uboCBVars;
	GLuint uboPBVars;
};