namespace YaoEngine {
	class YaoEngine {
	public:
		virtual ~YaoEngine() {};
		virtual void run() {};
	protected:
		YaoEngine() {};
	private:
		
		YaoEngine(const YaoEngine&) = delete;
		YaoEngine(YaoEngine&&) = delete;
		YaoEngine(YaoEngine&) = delete;

	};
	YaoEngine* CreateApp();
}