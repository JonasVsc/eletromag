#pragma once

#include "layer.h"

#include <vector>

class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void pushLayer(Layer* layer);
		
		void pushOverlay(Layer* overlay);

		void popLayer(Layer* layer);

		void popOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return mLayers.begin(); }
		
		std::vector<Layer*>::iterator end() { return mLayers.end(); }

	private:

		std::vector<Layer*> mLayers;
		
		std::vector<Layer*>::iterator mLayerInsert;
	};