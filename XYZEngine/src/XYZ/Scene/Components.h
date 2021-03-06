#pragma once
#include "XYZ/ECS/ECSManager.h"
#include "XYZ/ECS/Component.h"
#include "XYZ/ECS/Types.h"
#include "XYZ/Core/GUID.h"
#include "XYZ/Particle/ParticleEffect.h"
#include "XYZ/Renderer/SubTexture.h"
#include "XYZ/Script/ScriptPublicField.h"
#include "XYZ/Utils/Math/AABB.h"

#include "SceneCamera.h"
#include "AnimationController.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace XYZ {

	struct IDComponent : public Type<IDComponent>
	{
		IDComponent() = default;
		IDComponent(const GUID& id) 
			: ID(id)
		{}
		bool operator==(const IDComponent& other) const
		{
			return (std::string)ID == (std::string)other.ID;
		}
		GUID ID;
	};

	class TransformComponent : public Type<TransformComponent>
	{
	public:
		TransformComponent() = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation)
		{}
		
		glm::vec3 Translation = { 0.0f,0.0f,0.0f };
		glm::vec3 Rotation = { 0.0f,0.0f,0.0f };
		glm::vec3 Scale = { 1.0f,1.0f,1.0f };
		
		glm::mat4 GetTransform() const
		{		
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, { 1, 0, 0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.y, { 0, 1, 0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.z, { 0, 0, 1 });


			return glm::translate(glm::mat4(1.0f), Translation)
					* rotation
					* glm::scale(glm::mat4(1.0f), Scale);
		}


		void DecomposeTransform(const glm::mat4& transform)
		{
			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 translation;
			glm::vec3 skew;
			glm::vec4 perspective;
				
			glm::decompose(transform, Scale, rotation, Translation, skew, perspective);
			Rotation = glm::eulerAngles(rotation) * glm::pi<float>() / 180.f;
		}
	};

	struct SceneTagComponent : public Type<SceneTagComponent>
	{
		std::string Name;
		SceneTagComponent() = default;
		SceneTagComponent(const std::string& name)
			: Name(name)
		{}
		SceneTagComponent(const SceneTagComponent& other)
			: Name(other.Name)
		{}

		bool operator==(const SceneTagComponent& other) const
		{
			return Name == other.Name;
		}

		operator std::string& () { return Name; }
		operator const  std::string& () const { return Name; }
	};

	struct SpriteRenderer : public Type<SpriteRenderer>
	{
		SpriteRenderer() = default;
		SpriteRenderer(
			Ref<Material> material,
			Ref<SubTexture> subTexture,
			const glm::vec4& color,
			uint32_t sortLayer,
			bool isVisible = true
		);

		SpriteRenderer(const SpriteRenderer& other);
		SpriteRenderer(SpriteRenderer&& other) noexcept;


		SpriteRenderer& operator =(const SpriteRenderer& other);

		Ref<Material> Material;
		Ref<SubTexture> SubTexture;
		glm::vec4 Color;

		uint32_t SortLayer = 0;
		bool IsVisible = true;
	};


	struct CameraComponent : public Type<CameraComponent>
	{
		SceneCamera Camera;
		CameraComponent() = default;
	};


	struct AnimatorComponent : public Type<AnimatorComponent>
	{
		AnimatorComponent() = default;

		AnimationController Controller;
	};


	struct ParticleComponent : public Type<ParticleComponent>
	{
		ParticleComponent() = default;
		Ref<MaterialInstance> RenderMaterial;
		Ref<MaterialInstance> ComputeMaterial;
		Ref<ParticleEffect> ParticleEffect;

		uint32_t TextureID = 0;
	};

	struct PointLight2D : public Type<PointLight2D>
	{
		PointLight2D() = default;

		glm::vec3 Color = glm::vec3(1.0f);
		float Intensity = 1.0f;
	};


	struct Relationship : public Type<Relationship>
	{
		uint32_t Parent			 = NULL_ENTITY;
		uint32_t FirstChild		 = NULL_ENTITY;
		uint32_t PreviousSibling = NULL_ENTITY;
		uint32_t NextSibling	 = NULL_ENTITY;

		static void SetupRelation(uint32_t parent, uint32_t child, ECSManager& ecs);
		static void RemoveRelation(uint32_t child, ECSManager& ecs);
	};

	struct EntityScriptClass;
	struct ScriptComponent : public Type<ScriptComponent>
	{
		std::string ModuleName;
		std::vector<PublicField> Fields;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent & other) = default;
		ScriptComponent(const std::string & moduleName)
			: ModuleName(moduleName) {}

	private:
		EntityScriptClass* ScriptClass = nullptr;
		
		uint32_t Handle = 0;

		friend class ScriptEngine;
	};


	struct RigidBody2DComponent : public Type<RigidBody2DComponent>
	{
		enum class Type { Static, Dynamic, Kinematic };
		Type BodyType = Type::Dynamic;

		void* RuntimeBody = nullptr;
	};


	struct BoxCollider2DComponent : public Type<BoxCollider2DComponent>
	{
		glm::vec2 Size = glm::vec2(1.0f);
		float Density = 1.0f;
		float Friction = 0.0f;

		void* RuntimeFixture = nullptr;
	};
}