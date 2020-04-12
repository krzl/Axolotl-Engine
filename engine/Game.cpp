#include "stdafx.h"
#include "Game.h"
#include "Entities/SystemBase.h"
#include "Graphics/Vulkan/Vulkan.h"

namespace axlt {

	// ReSharper disable once CppInconsistentNaming
	Game GameInstance{};

	void Game::Init() {
		const uint32_t width = 800; //TODO: Create config
		const uint32_t height = 600; //TODO: Create config`
		
		m_window.Init( width, height, "Test" );
		if( vk::Initialize( m_window.m_hInstance, m_window.m_hWnd, width, height ) ) {
			printf( "Vulkan initialization successful\n" );
		}
	}

	void Game::Run() {
		Init();
		while( m_window.IsRunning() ) {
			SystemBase::UpdateSystems();
			m_window.Update();
		}
		Shutdown();
	}

	void Game::Shutdown() {
		vk::Shutdown();
	}
}
