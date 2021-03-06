#include <iostream>

#include "svq/gfx/renderer_2d.h"
#include "svq/gfx/ogl/gl_common.h"

namespace svq{ namespace gfx{

#define RENDERER_MAX_SPRITES	1000
#define RENDERER_SPRITE_SIZE	RENDERER_VERTEX_SIZE * 4
#define RENDERER_BUFFER_SIZE	RENDERER_SPRITE_SIZE * RENDERER_MAX_SPRITES
#define RENDERER_INDICES_SIZE	RENDERER_MAX_SPRITES * 6
#define RENDERER_MAX_TEXTURES	32 - 1

const uint g_RequiredSystemUniformsCount = 2;
const std::string g_RequiredSystemUniforms[g_RequiredSystemUniformsCount] =
{
	"sys_ProjectionMatrix",
	"sys_ViewMatrix"
};

const uint sys_ProjectionMatrixIndex = 0;
const uint sys_ViewMatrixIndex = 1;

Renderer_2D::Renderer_2D(uint width, uint height)
		: m_IndexCount(0), m_ScreenSize(math::Vec2<uint>(width, height)), m_ViewportSize(math::Vec2<uint>(width, height)) {
	_init();
}


Renderer_2D::Renderer_2D(const math::Vec2<uint>& screenSize)
		: m_IndexCount(0), m_ScreenSize(screenSize), m_ViewportSize(screenSize) {
	_init();
}


Renderer_2D::~Renderer_2D() {
	delete m_IndexBuffer;
	delete m_VertexArray;
	delete m_Framebuffer;
	delete m_ScreenQuad;
	delete m_Shader;
}


void Renderer_2D::_init() {
	m_Target = RenderTarget::SCREEN;

	m_SystemUniforms.resize(g_RequiredSystemUniformsCount);

	m_Shader = Shader::createFromFile("test_sprite", "/Users/max/dev/sovoq/resources/shaders/test_sprite.shader");

	GLfloat transform[] = {
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
  };
	m_Shader->setUnifromMatrix4fv("transform", transform);
	m_Shader->bind();

	VertexBuffer* buffer = VertexBuffer::create(BufferType::DYNAMIC);
	buffer->resize(RENDERER_BUFFER_SIZE); // in fact it also binds actual buffer
	
	BufferLayout layout;
	layout.push<math::Vec3f>("POSITION"); // Position
	layout.push<math::Vec2f>("TEXCOORD"); // UV
	layout.push<float>("TEXID"); // Texture Index
	layout.push<float>("COLOR", 4, true); // Color
	buffer->setLayout(layout);


	m_VertexArray = VertexArray::create();
	m_VertexArray->pushBuffer(buffer);


	uint* indices = new uint[RENDERER_INDICES_SIZE];

	int32 offset = 0;

	for (int32 i = 0; i < RENDERER_INDICES_SIZE; i += 6) {
		indices[i] = offset + 0; indices[i + 1] = offset + 1; indices[i + 2] = offset + 2;
		indices[i + 3] = offset + 2; indices[i + 4] = offset + 3; indices[i + 5] = offset + 0;
		offset += 4;
	}

	m_IndexBuffer = IndexBuffer::create(indices, RENDERER_INDICES_SIZE);
	m_VertexArray->unbind();

	

	// Setup Framebuffer
	//m_Framebuffer = FrameBuffer::create(m_ViewportSize.x, m_ViewportSize.y);

	//m_FramebufferMaterial = new Material(ShaderFactory::SimpleShader());
	//m_FramebufferMaterial->SetUniform("pr_matrix", maths::mat4::Orthographic(0, (float)m_ScreenSize.x, (float)m_ScreenSize.y, 0, -1.0f, 1.0f));
	//m_FramebufferMaterial->SetTexture("u_Texture", m_Framebuffer->GetTexture());
	//m_ScreenQuad = MeshFactory::CreateQuad(0, 0, (float)m_ScreenSize.x, (float)m_ScreenSize.y);

	//m_PostEffects = new PostEffects();
	//m_PostEffectsBuffer = Framebuffer2D::Create(m_ViewportSize.x, m_ViewportSize.y);

	//delete buffer;
}


void Renderer_2D::push(const Renderable_2D* renderable){
	// TODO: first push all the renderables into separate map for later sorting
	if (!renderable->isVisible())
		return;

	//const math::Bbox2f& bounds = renderable->getBounds();
	//const math::Vec3f min = bounds.getMinimumBound();
	//const math::Vec3f max = bounds.getMaximumBound();

	const math::Vec4f color = renderable->getColor();
	const std::vector<math::Vec2f>& uv = renderable->getUVs();
	const Texture* texture = renderable->getTexture();

	float textureSlot = 0.0f;
	//if (texture)
	//	textureSlot = submitTexture(renderable->getTexture());

	//mat4 maskTransform = mat4::Identity();
	//float mid = m_Mask ? SubmitTexture(m_Mask->texture) : 0.0f;
	//float ms = 0.0f;

	//if (s_MaskEnabled && m_Mask != nullptr)
	//{
	//	maskTransform = mat4::Invert(m_Mask->transform);
	//	ms = SubmitTexture(m_Mask->texture);
	//}

	//math::Vec3f vertex = *m_TransformationBack * min;
	math::Vec3f vertex(0, 0, 0);
	m_Buffer->vertex = vertex;
	m_Buffer->uv = uv[0];
	//m_Buffer->mask_uv = maskTransform * vertex;
	m_Buffer->texture_id = textureSlot;
	//m_Buffer->mid = ms;
	m_Buffer->color = color;
	m_Buffer++;

	//vertex = *m_TransformationBack * vec3(max.x, min.y);
	vertex = math::Vec3f(20, 0, 0);
	m_Buffer->vertex = vertex;
	m_Buffer->uv = uv[1];
	//m_Buffer->mask_uv = maskTransform * vertex;
	m_Buffer->texture_id = textureSlot;
	//m_Buffer->mid = ms;
	m_Buffer->color = color;
	m_Buffer++;

	//vertex = *m_TransformationBack * max;
	vertex = math::Vec3f(10, 10, 0);
	m_Buffer->vertex = vertex;
	m_Buffer->uv = uv[2];
	//m_Buffer->mask_uv = maskTransform * vertex;
	m_Buffer->texture_id = textureSlot;
	//m_Buffer->mid = ms;
	m_Buffer->color = color;
	m_Buffer++;

	//vertex = *m_TransformationBack * vec3(min.x, max.y);
	vertex = math::Vec3f(0, 10, 0);
	m_Buffer->vertex = vertex;
	m_Buffer->uv = uv[3];
	//m_Buffer->mask_uv = maskTransform * vertex;
	m_Buffer->texture_id = textureSlot;
 	//m_Buffer->mid = ms;
	m_Buffer->color = color;
	m_Buffer++;

	m_IndexCount += 6;
}

void Renderer_2D::flush() {
	// TODO: sort renderables based in uint key and build the actial buffer

	m_Shader->bind();
	float aspect = (float)m_ScreenSize.y / (float)m_ScreenSize.x;
	GLfloat transform[] = {
    aspect, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 20.0f,
  };
	m_Shader->setUnifromMatrix4fv("transform", transform);
	m_Shader->bind();

	m_VertexArray->bind();
	m_IndexBuffer->bind();
	m_VertexArray->draw(m_IndexCount);
	m_IndexBuffer->unbind();
	m_VertexArray->unbind();

	m_IndexCount = 0;

	/*
	if (m_Target == RenderTarget::BUFFER) {
			assert(false); // Currently unsupported

			// Post Effects pass should go here!
			
			//if (s_PostEffectsEnabled && m_PostEffectsEnabled)
			//	m_PostEffects->renderPostEffects(m_Framebuffer, m_PostEffectsBuffer, m_ScreenQuad, m_IBO);

			// Display Framebuffer - potentially move to Framebuffer class
			
			//glBindFramebuffer(GL_FRAMEBUFFER, m_ScreenBuffer);
			//Renderer::setViewport(0, 0, m_ScreenSize.x, m_ScreenSize.y);
			//Renderer::setBlendFunction(RendererBlendFunction::SOURCE_ALPHA, RendererBlendFunction::ONE_MINUS_SOURCE_ALPHA);
			//m_FramebufferMaterial->bind();

			// TODO: None of this should be done here
			glActiveTexture(GL_TEXTURE0);
			//if (m_PostEffectsEnabled)
			//	m_PostEffectsBuffer->getTexture()->bind(m_FramebufferMaterial->getShader());
			//else
			//	m_Framebuffer->getTexture()->bind(m_FramebufferMaterial->getShader());

			//m_ScreenQuad->bind();
			m_IndexBuffer->bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
			m_IndexBuffer->unbind();
			//m_ScreenQuad->Unbind();
		}
		*/
}

void Renderer_2D::start() {
	if (m_Target == RenderTarget::BUFFER) {
			assert(false); // Currently Unsupported

			if (m_ViewportSize != m_Framebuffer->getSize()) {
				delete m_Framebuffer;
				m_Framebuffer = FrameBuffer::create(m_ViewportSize);
				
				if (m_PostEffectsEnabled) {
					delete m_PostEffectsBuffer;
					m_PostEffectsBuffer = FrameBuffer::create(m_ViewportSize);
				}
			}

			if (m_PostEffectsEnabled) {
				m_PostEffectsBuffer->bind();
				m_PostEffectsBuffer->clear();
			}

			m_Framebuffer->bind();
			m_Framebuffer->clear(); // TODO: Clear somewhere else, since this basically limits to one draw call
			//Renderer_2D::setBlendFunction(RendererBlendFunction::ONE, RendererBlendFunction::ZERO);
	} else {
		//Renderer_2D::setViewport(0, 0, m_ScreenSize.x, m_ScreenSize.y);
		//GL_CALL(glViewport(0, 0, m_ScreenSize.x,  m_ScreenSize.y));
	}
	
	m_VertexArray->bind();
	m_Buffer = m_VertexArray->getBuffer()->getPointer<VertexData>();
}

void Renderer_2D::stop() {
	m_VertexArray->getBuffer()->releasePointer();
	m_VertexArray->unbind();
}

}}