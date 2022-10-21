#pragma once

#include <concepts>
#include <memory>
#include <vector>

namespace Brainfryer
{
	class CommandList;

	enum class EContextAPI
	{
		None = 0,
		DX12
	};

	class Context
	{
	public:
		static bool SelectSpecificAPI(EContextAPI api);
		static bool SelectBestAPI();
		static void Destroy();

		static void ExecuteCommandLists(const std::vector<CommandList*>& commandLists);

		static void         WaitForGPU();
		static CommandList* NextFrame();

		static EContextAPI CurrentAPI();

		template <std::derived_from<Context> T>
		static T* Get();

	private:
		static std::unique_ptr<Context> s_CurrentContext;

	public:
		virtual ~Context() = default;

		virtual void executeCommandLists(const std::vector<CommandList*>& commandLists) = 0;

		virtual void          waitForGPU()       = 0;
		virtual CommandList*  nextFrame()        = 0;
		virtual std::uint32_t frameIndex() const = 0;
		virtual std::uint32_t frameCount() const = 0;

		virtual bool initialized() const = 0;

		virtual EContextAPI api() const = 0;
	};

	template <std::derived_from<Context> T>
	T* Context::Get()
	{
		if (T::API == s_CurrentContext->api())
			return static_cast<T*>(s_CurrentContext.get());
		return nullptr;
	}
} // namespace Brainfryer