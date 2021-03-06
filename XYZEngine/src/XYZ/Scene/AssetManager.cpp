#include "stdafx.h"
#include "AssetManager.h"

#include "Scene.h"


// TODO: delete
#include <locale>
#include <codecvt>

namespace XYZ {
	AssetManager::AssetManager(const std::string& directory)
		:
		m_Directory(directory)
	{
		std::wstring unicodeDir(directory.begin(), directory.end());
		m_FileWatcher = FileWatcher::Create(unicodeDir);
		m_FileWatcher->AddListener(this);
		m_FileWatcher->Start();
	}
	AssetManager::~AssetManager()
	{
		m_FileWatcher->Stop();	
	}

	void AssetManager::Serialize()
	{
		for (auto it : m_Assets)
			it.second->Serialize();
	}

	void AssetManager::OnFileChange(const std::wstring& filepath)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		std::string convertedStr = converter.to_bytes(filepath);
		std::replace(convertedStr.begin(), convertedStr.end(), '\\', '/');
		
		//std::cout << "File changed " << convertedStr << std::endl;
	}
	void AssetManager::OnFileAdded(const std::wstring& filepath)
	{
	}
	void AssetManager::OnFileRemoved(const std::wstring& filepath)
	{
	}
	void AssetManager::OnFileRenamed(const std::wstring& filepath)
	{
	}
}