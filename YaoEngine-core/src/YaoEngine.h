namespace YaoEngine {
	class YaoEngine {
	public:
		virtual ~YaoEngine();
		virtual void run();

		YaoEngine* Getinstance() { return m_instance; }

		//事件
		bool WindowCloseEvent() { return false; }
		bool WindowResizeEvent() { return false; }

	protected:
		YaoEngine() ;
	private:	
		YaoEngine(const YaoEngine&) = delete;
		YaoEngine(YaoEngine&&) = delete;
		YaoEngine(YaoEngine&) = delete;

		static YaoEngine* m_instance;
		bool m_running = true;
	};
	YaoEngine* CreateApp();
}