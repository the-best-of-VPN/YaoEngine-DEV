#include<Yao.h>
namespace YaoEngine {
	class Editor :public YaoEngine
	{
	public:
		Editor() :YaoEngine(){};
		virtual ~Editor() {};
	private:
		
	};
	YaoEngine* CreateApp()
	{
		return new Editor();
	}
}