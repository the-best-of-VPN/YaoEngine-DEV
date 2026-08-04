#include "imguiimportLayer.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "YaoEngine.h"

namespace YaoEngine {

	namespace
	{
		GLFWwindow* GetGLFWWindow()
		{
			return static_cast<GLFWwindow*>(YaoEngine::Getinstance()->GetWindow()->GetNativeWindow());
		}

		bool CheckShader(unsigned int shader, const char* name)
		{
			int status = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			return status == GL_TRUE;
		}

		bool CheckProgram(unsigned int program)
		{
			int status = 0;
			glGetProgramiv(program, GL_LINK_STATUS, &status);
			return status == GL_TRUE;
		}
	}

	imguiimport::imguiimport()
		: Layer("imguiimport")
	{
	}

	void imguiimport::OnAttach()
	{
		IMGUI_CHECKVERSION();
		m_Context = ImGui::CreateContext();
		ImGui::SetCurrentContext(static_cast<ImGuiContext*>(m_Context));

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendPlatformName = "YaoEngine_GLFW_Minimal";
		io.BackendRendererName = "YaoEngine_OpenGL_Minimal";

		ImGui::StyleColorsDark();
		CreateDeviceObjects();
	}

	void imguiimport::DeAttach()
	{
		if (!m_Context)
			return;

		ImGui::SetCurrentContext(static_cast<ImGuiContext*>(m_Context));
		DestroyDeviceObjects();
		ImGui::DestroyContext(static_cast<ImGuiContext*>(m_Context));
		m_Context = nullptr;
	}

	void imguiimport::OnImGuiRender(float ts)
	{
		if (!m_Context)
			return;

		ImGui::SetCurrentContext(static_cast<ImGuiContext*>(m_Context));
		BeginFrame(ts);
		RenderDockspace();
		EndFrame();
	}

	bool imguiimport::WantsMouseCapture() const
	{
		if (!m_Context)
			return false;

		ImGui::SetCurrentContext(static_cast<ImGuiContext*>(m_Context));
		return ImGui::GetIO().WantCaptureMouse;
	}

	bool imguiimport::WantsKeyboardCapture() const
	{
		if (!m_Context)
			return false;

		ImGui::SetCurrentContext(static_cast<ImGuiContext*>(m_Context));
		return ImGui::GetIO().WantCaptureKeyboard;
	}

	void imguiimport::SetViewportTextureID(unsigned int textureID)
	{
		m_ViewportTextureID = textureID;
	}

	void imguiimport::SetViewportState(unsigned int width, unsigned int height, float minX, float minY, bool focused, bool hovered)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
		m_ViewportMinX = minX;
		m_ViewportMinY = minY;
		m_ViewportFocused = focused;
		m_ViewportHovered = hovered;
	}

	void imguiimport::BeginFrame(float timestep)
	{
		GLFWwindow* window = GetGLFWWindow();
		ImGuiIO& io = ImGui::GetIO();

		int windowWidth = 0;
		int windowHeight = 0;
		int framebufferWidth = 0;
		int framebufferHeight = 0;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

		io.DisplaySize = ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHeight));
		if (windowWidth > 0 && windowHeight > 0)
		{
			io.DisplayFramebufferScale = ImVec2(
				static_cast<float>(framebufferWidth) / static_cast<float>(windowWidth),
				static_cast<float>(framebufferHeight) / static_cast<float>(windowHeight)
			);
		}

		io.DeltaTime = timestep > 0.0f ? timestep : (1.0f / 60.0f);

		double mouseX = 0.0;
		double mouseY = 0.0;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
		io.MouseDown[0] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
		io.MouseDown[1] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
		io.MouseDown[2] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;

		ImGui::NewFrame();
	}

	void imguiimport::EndFrame()
	{
		ImGui::Render();
		RenderDrawData(ImGui::GetDrawData());
	}

	void imguiimport::RenderDockspace()
	{
		m_DockspaceID = ImGui::DockSpaceOverViewport();
	}

	void imguiimport::CreateDeviceObjects()
	{
		const char* vertexShader =
			"#version 330 core\n"
			"uniform mat4 ProjMtx;\n"
			"layout(location = 0) in vec2 Position;\n"
			"layout(location = 1) in vec2 UV;\n"
			"layout(location = 2) in vec4 Color;\n"
			"out vec2 Frag_UV;\n"
			"out vec4 Frag_Color;\n"
			"void main()\n"
			"{\n"
			"    Frag_UV = UV;\n"
			"    Frag_Color = Color;\n"
			"    gl_Position = ProjMtx * vec4(Position.xy, 0.0, 1.0);\n"
			"}\n";

		const char* fragmentShader =
			"#version 330 core\n"
			"uniform sampler2D Texture;\n"
			"in vec2 Frag_UV;\n"
			"in vec4 Frag_Color;\n"
			"layout(location = 0) out vec4 Out_Color;\n"
			"void main()\n"
			"{\n"
			"    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
			"}\n";

		m_VertexHandle = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(m_VertexHandle, 1, &vertexShader, nullptr);
		glCompileShader(m_VertexHandle);
		CheckShader(m_VertexHandle, "imgui vertex shader");

		m_FragmentHandle = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(m_FragmentHandle, 1, &fragmentShader, nullptr);
		glCompileShader(m_FragmentHandle);
		CheckShader(m_FragmentHandle, "imgui fragment shader");

		m_ShaderHandle = glCreateProgram();
		glAttachShader(m_ShaderHandle, m_VertexHandle);
		glAttachShader(m_ShaderHandle, m_FragmentHandle);
		glLinkProgram(m_ShaderHandle);
		CheckProgram(m_ShaderHandle);

		m_AttribLocationTex = glGetUniformLocation(m_ShaderHandle, "Texture");
		m_AttribLocationProjMtx = glGetUniformLocation(m_ShaderHandle, "ProjMtx");

		glGenBuffers(1, &m_VboHandle);
		glGenBuffers(1, &m_ElementsHandle);
		glGenVertexArrays(1, &m_VaoHandle);

		glBindVertexArray(m_VaoHandle);
		glBindBuffer(GL_ARRAY_BUFFER, m_VboHandle);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<void*>(offsetof(ImDrawVert, pos)));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<void*>(offsetof(ImDrawVert, uv)));
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), reinterpret_cast<void*>(offsetof(ImDrawVert, col)));

		unsigned char* pixels = nullptr;
		int width = 0;
		int height = 0;
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		glGenTextures(1, &m_FontTexture);
		glBindTexture(GL_TEXTURE_2D, m_FontTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		io.Fonts->SetTexID(static_cast<ImTextureID>(m_FontTexture));

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void imguiimport::DestroyDeviceObjects()
	{
		if (m_VaoHandle)
			glDeleteVertexArrays(1, &m_VaoHandle);
		if (m_VboHandle)
			glDeleteBuffers(1, &m_VboHandle);
		if (m_ElementsHandle)
			glDeleteBuffers(1, &m_ElementsHandle);
		if (m_ShaderHandle && m_VertexHandle)
			glDetachShader(m_ShaderHandle, m_VertexHandle);
		if (m_ShaderHandle && m_FragmentHandle)
			glDetachShader(m_ShaderHandle, m_FragmentHandle);
		if (m_VertexHandle)
			glDeleteShader(m_VertexHandle);
		if (m_FragmentHandle)
			glDeleteShader(m_FragmentHandle);
		if (m_ShaderHandle)
			glDeleteProgram(m_ShaderHandle);
		if (m_FontTexture)
		{
			glDeleteTextures(1, &m_FontTexture);
			ImGui::GetIO().Fonts->SetTexID(0);
		}

		m_VaoHandle = 0;
		m_VboHandle = 0;
		m_ElementsHandle = 0;
		m_ShaderHandle = 0;
		m_VertexHandle = 0;
		m_FragmentHandle = 0;
		m_FontTexture = 0;
	}

	void imguiimport::RenderDrawData(void* rawDrawData)
	{
		ImDrawData* drawData = static_cast<ImDrawData*>(rawDrawData);
		const int framebufferWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
		const int framebufferHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);
		if (framebufferWidth <= 0 || framebufferHeight <= 0)
			return;

		GLint lastProgram = 0;
		GLint lastTexture = 0;
		GLint lastArrayBuffer = 0;
		GLint lastElementArrayBuffer = 0;
		GLint lastVertexArray = 0;
		GLint lastViewport[4] = {};
		GLint lastScissorBox[4] = {};
		GLboolean lastBlend = glIsEnabled(GL_BLEND);
		GLboolean lastCullFace = glIsEnabled(GL_CULL_FACE);
		GLboolean lastDepthTest = glIsEnabled(GL_DEPTH_TEST);
		GLboolean lastScissorTest = glIsEnabled(GL_SCISSOR_TEST);
		glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &lastArrayBuffer);
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &lastElementArrayBuffer);
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &lastVertexArray);
		glGetIntegerv(GL_VIEWPORT, lastViewport);
		glGetIntegerv(GL_SCISSOR_BOX, lastScissorBox);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);
		glViewport(0, 0, framebufferWidth, framebufferHeight);

		const float left = drawData->DisplayPos.x;
		const float right = drawData->DisplayPos.x + drawData->DisplaySize.x;
		const float top = drawData->DisplayPos.y;
		const float bottom = drawData->DisplayPos.y + drawData->DisplaySize.y;
		const float orthoProjection[4][4] = {
			{ 2.0f / (right - left), 0.0f, 0.0f, 0.0f },
			{ 0.0f, 2.0f / (top - bottom), 0.0f, 0.0f },
			{ 0.0f, 0.0f, -1.0f, 0.0f },
			{ (right + left) / (left - right), (top + bottom) / (bottom - top), 0.0f, 1.0f },
		};

		glUseProgram(m_ShaderHandle);
		glUniform1i(m_AttribLocationTex, 0);
		glUniformMatrix4fv(m_AttribLocationProjMtx, 1, GL_FALSE, &orthoProjection[0][0]);
		glBindVertexArray(m_VaoHandle);

		ImVec2 clipOffset = drawData->DisplayPos;
		ImVec2 clipScale = drawData->FramebufferScale;
		for (int n = 0; n < drawData->CmdListsCount; ++n)
		{
			const ImDrawList* cmdList = drawData->CmdLists[n];
			glBindBuffer(GL_ARRAY_BUFFER, m_VboHandle);
			glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(cmdList->VtxBuffer.Size * sizeof(ImDrawVert)), cmdList->VtxBuffer.Data, GL_STREAM_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementsHandle);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(cmdList->IdxBuffer.Size * sizeof(ImDrawIdx)), cmdList->IdxBuffer.Data, GL_STREAM_DRAW);

			for (int cmdIndex = 0; cmdIndex < cmdList->CmdBuffer.Size; ++cmdIndex)
			{
				const ImDrawCmd* drawCommand = &cmdList->CmdBuffer[cmdIndex];
				if (drawCommand->UserCallback)
				{
					drawCommand->UserCallback(cmdList, drawCommand);
					continue;
				}

				ImVec2 clipMin((drawCommand->ClipRect.x - clipOffset.x) * clipScale.x, (drawCommand->ClipRect.y - clipOffset.y) * clipScale.y);
				ImVec2 clipMax((drawCommand->ClipRect.z - clipOffset.x) * clipScale.x, (drawCommand->ClipRect.w - clipOffset.y) * clipScale.y);
				if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
					continue;

				glScissor(
					static_cast<int>(clipMin.x),
					static_cast<int>(static_cast<float>(framebufferHeight) - clipMax.y),
					static_cast<int>(clipMax.x - clipMin.x),
					static_cast<int>(clipMax.y - clipMin.y)
				);
				const auto textureId = static_cast<unsigned int>(drawCommand->GetTexID());
				glBindTexture(GL_TEXTURE_2D, textureId);
				glDrawElements(
					GL_TRIANGLES,
					static_cast<GLsizei>(drawCommand->ElemCount),
					sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
					reinterpret_cast<void*>(static_cast<intptr_t>(drawCommand->IdxOffset * sizeof(ImDrawIdx)))
				);
			}
		}

		glUseProgram(lastProgram);
		glBindTexture(GL_TEXTURE_2D, lastTexture);
		glBindBuffer(GL_ARRAY_BUFFER, lastArrayBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lastElementArrayBuffer);
		glBindVertexArray(lastVertexArray);
		glViewport(lastViewport[0], lastViewport[1], lastViewport[2], lastViewport[3]);
		glScissor(lastScissorBox[0], lastScissorBox[1], lastScissorBox[2], lastScissorBox[3]);
		lastBlend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
		lastCullFace ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
		lastDepthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
		lastScissorTest ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
	}
}
