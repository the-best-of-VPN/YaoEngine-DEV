#include"./Log.h"
namespace YaoEngine {
	namespace LogSystem {
		Logger* YLog::YaoEngine = new Logger("YaoEngine");
		Logger* YLog::client = new Logger("cilent");
	}


}
