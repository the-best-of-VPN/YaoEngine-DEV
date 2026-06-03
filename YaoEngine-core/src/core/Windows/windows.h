#pragma once
#include<core.h>
namespace YaoEngine
{
	struct Windowprop {
		unsigned int width, height;
		const char* title;
		Windowprop(unsigned int width = 1900, unsigned int height = 1900, const char* title = "YaoEngine") : width(width), height(height), title(title) {}
	};
	class YaoWindow
	{
	public:
		static Scope<YaoWindow> CreateWindowe(const Windowprop& prop=Windowprop());
		virtual ~YaoWindow() = default;
		virtual void OnUpdate() = 0;
		virtual void Clear() = 0;
		virtual void* GetNativeWindow() = 0;
		
	private:
		Windowprop m_windowprop;
	};
}