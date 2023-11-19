#pragma once
#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#include <memory>
#include <unordered_map>
#include "../file_system/file_reference.h"
#include <string>
#include "../reflection/reflection.h"

class Texture;

class API SkyBox : public FileReference, public Reflective, public std::enable_shared_from_this<SkyBox>
{
public:

	SkyBox();
	~SkyBox();

	/**
	* Create a skybox
	* @param front Front face
	* @param back Back face
	* @param up Up face
	* @param down Down face
	* @param left Left face
	* @param right Right face
	*/
	SkyBox(const std::shared_ptr<Texture>& front, const std::shared_ptr<Texture>& back, const std::shared_ptr<Texture>& up, const std::shared_ptr<Texture>& down, const std::shared_ptr<Texture>& left, const std::shared_ptr<Texture>& right);

	std::shared_ptr<Texture> front = nullptr;
	std::shared_ptr<Texture> back = nullptr;
	std::shared_ptr<Texture> up = nullptr;
	std::shared_ptr<Texture> down = nullptr;
	std::shared_ptr<Texture> left = nullptr;
	std::shared_ptr<Texture> right = nullptr;
	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflection() override;
	static std::shared_ptr<SkyBox> MakeSkyBox();
	void OnReflectionUpdated() override;
	void LoadFileReference() override;

private:
};