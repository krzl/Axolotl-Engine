#include "stdafx.h"
#include "Object.h"

#include "Common/Random.h"

namespace axlt {

	uint32_t Object::instanceIdCounter = 0;

	Object::Object() {
		instanceId = ++instanceIdCounter;
		sceneFileId = RandomNumber<uint32_t>(); // TODO: Check if number is not used in current scene file, this doesn't need to be used for resources
	}

	uint32_t Object::GetInstanceId() const {
		return instanceId;
	}

	uint32_t Object::GetSceneFileId() const {
		return sceneFileId;
	}
}