#include<Yao.h>
namespace YaoEngine {
	class Editor :public YaoEngine
	{
	public:
		Editor() :YaoEngine(){};
		virtual ~Editor() {};
		void run() override {};
	private:
		
	};
	YaoEngine* CreateApp()
	{
		return new Editor();
	}
}