#pragma once
#include"Layer.h"

namespace YaoEngine {
	class imguiimport :public Layer
	{
	public:
		imguiimport();
		virtual ~imguiimport() override = default;
		virtual void OnUpdate(float ts) override {};
		virtual void OnEvent(Event& e) override {};
		virtual void OnImGuiRender(float ts) override;
		virtual void OnAttach() override;
		virtual void DeAttach() override;

		void BeginFrame(float timestep);
		void EndFrame();
		void RenderDockspace();

		bool WantsMouseCapture() const;
		bool WantsKeyboardCapture() const;
		void SetViewportTextureID(unsigned int textureID);
		void SetViewportState(unsigned int width, unsigned int height, float minX, float minY, bool focused, bool hovered);
		void SetViewportPlaying(bool playing) { m_ViewportPlaying = playing; }
		unsigned int GetViewportTextureID() const { return m_ViewportTextureID; }
		unsigned int GetViewportWidth() const { return m_ViewportWidth; }
		unsigned int GetViewportHeight() const { return m_ViewportHeight; }
		float GetViewportMinX() const { return m_ViewportMinX; }
		float GetViewportMinY() const { return m_ViewportMinY; }
		bool IsViewportFocused() const { return m_ViewportFocused; }
		bool IsViewportHovered() const { return m_ViewportHovered; }
		bool IsViewportPlaying() const { return m_ViewportPlaying; }
		unsigned int GetDockspaceID() const { return m_DockspaceID; }

	private:
		void CreateDeviceObjects();
		void DestroyDeviceObjects();
		void RenderDrawData(void* drawData);

		unsigned int m_ViewportTextureID = 0;
		unsigned int m_ViewportWidth = 0;
		unsigned int m_ViewportHeight = 0;
		float m_ViewportMinX = 0.0f;
		float m_ViewportMinY = 0.0f;
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		bool m_ViewportPlaying = true;
		unsigned int m_DockspaceID = 0;

		void* m_Context = nullptr;
		unsigned int m_FontTexture = 0;
		unsigned int m_ShaderHandle = 0;
		unsigned int m_VertexHandle = 0;
		unsigned int m_FragmentHandle = 0;
		unsigned int m_VboHandle = 0;
		unsigned int m_ElementsHandle = 0;
		unsigned int m_VaoHandle = 0;
		int m_AttribLocationTex = -1;
		int m_AttribLocationProjMtx = -1;
	};
}
