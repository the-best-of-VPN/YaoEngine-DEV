#include"Layer.h"
#include<string>
#include<vector>
namespace YaoEngine {
	class Layerstack {
		public:
			Layerstack() {};
			~Layerstack() {  };
			std::vector<Layer*>& Get() { return m_layers; }
			void PushLayer(Layer* layer) { layer->OnAttach();m_layers.push_back(layer); }
			void PopLayer(Layer* layer) { layer->DeAttach();m_layers.erase(std::remove(m_layers.begin(), m_layers.end(), layer), m_layers.end()); }
	private:
		std::string m_name;
		std::vector<Layer*> m_layers;
	};
}