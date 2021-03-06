#include "stdafx.h"
#include "Game.h"
#include "Entities/SystemBase.h"
#include "Graphics/Vulkan/Vulkan.h"
#include "Platform/Input.h"

namespace axlt {
	// ReSharper disable once CppInconsistentNaming
	Game GameInstance{};

	void Game::Init() {
		const uint32_t width = 1920; //TODO: Create config
		const uint32_t height = 1080; //TODO: Create config

		m_window.Init(width, height, "Test");
		if (vk::Initialize(m_window.m_hInstance, m_window.m_hWnd, width, height)) {
			printf("Vulkan initialization successful\n");
		}
	}

	void Game::SetImportFileSystem( FileSystem& fileSystem ) {
		this->fileSystem = &fileSystem;
	}

	void Game::Run() {
		Init();
		SystemBase::InitializeSystems();
		while (m_window.isRunning) {
			SystemBase::UpdateSystems();
			vk::Draw();
			input::ResetLastFrameInput();
			m_window.Update();
			if (m_window.hasResized) {
				vk::ResizeFramebuffer(m_window.width, m_window.height);
				m_window.hasResized = false;
			}
		}
		Shutdown();
	}

	void Game::Shutdown() {
		vk::Shutdown();
	}

	const Window& Game::GetWindow() const {
		return m_window;
	}
}
