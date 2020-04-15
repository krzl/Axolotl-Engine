#include "stdafx.h"
#include "ResourceDatabase.h"
#include <chrono>
#include <iostream>
#include <Resources/ModelResource.h>

using namespace axlt::editor;

int main() {
	const auto start = std::chrono::steady_clock::now();

	ResourceDatabase resourceDatabase;
	
	const auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "Time in seconds: " << elapsed.count() << '\n';
	
	return 0;
}
