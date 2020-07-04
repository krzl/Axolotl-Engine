#pragma once
#include "Collections/Map.h"

namespace axlt {
	class Object {

	public:

		Object();

		uint32_t GetInstanceId() const;
		uint32_t GetSceneFileId() const;

	private:
		
		uint32_t instanceId;
		uint32_t sceneFileId;

		static uint32_t instanceIdCounter;
	};
}
