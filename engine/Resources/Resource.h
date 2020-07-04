#pragma once

#include "Core/Object.h"
#include "FileSystem/Guid.h"
#include "Serialization/Serializable.h"

namespace axlt {
	class Resource : public Object, public Serializable {

	public:

		Resource();
		explicit Resource( const Guid& guid );

		Guid guid; //TODO: Don't store outside of editor
	};
}
