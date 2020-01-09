#pragma once

#include <vector>
#include <type_traits>

namespace VertexSystem
{
	struct BGRAColor
	{
		unsigned char a;
		unsigned char r;
		unsigned char b;
		unsigned char g;
	};

	class VertexLayout
	{
	public:
		enum ElementType
		{
			Position2D,
			Position3D,
			Texture2D,
			Normal,
			Float3Color,
			Float4Color,
			BGRAColor
		};

		class Element
		{
		public:
			Element(ElementType type, size_t offset)
				:
				type(type),
				offset(offset)
			{};

			size_t GetOffsetAfter() const noexcept
			{
				return offset + Size();
			}

			size_t GetOffset() const
			{
				return offset;
			}

			size_t Size() const noexcept
			{
				return SizeOf(type);
			}

			size_t constexpr SizeOf(ElementType type) const noexcept
			{
				switch (type)
				{
				case VertexLayout::Position2D:
					return sizeof(DirectX::XMFLOAT2);
				case VertexLayout::Position3D:
					return sizeof(DirectX::XMFLOAT3);
				case VertexLayout::Texture2D:
					return sizeof(DirectX::XMFLOAT2);
				case VertexLayout::Normal:
					return sizeof(DirectX::XMFLOAT3);
				case VertexLayout::Float3Color:
					return sizeof(DirectX::XMFLOAT3);
				case VertexLayout::Float4Color:
					return sizeof(DirectX::XMFLOAT3);
				case VertexLayout::BGRAColor:
					return sizeof(unsigned int);
				}

				assert("Invalid elemeny type" & false);
				return 0u;
			}

			ElementType GetType() const noexcept
			{
				return type;
			}

		private:
			ElementType type;
			size_t offset;
		};

	public:
		template<ElementType Type>
		const Element& Resolve() const noexcept
		{
			for (auto& e : elements)
			{
				if (e.GetType() == Type)
				{
					return e;
				}
			}

			assert("Could not resolve element type" && false);
			return elements.front();
		}

		const Element& ResolveByIndex(size_t i) const noexcept
		{
			return elements[i];
		}

		template<ElementType Type>
		VertexLayout& Append() noexcept
		{
			elements.emplace_back(Type, Size());
			return *this;
		}

		size_t Size() const noexcept
		{
			return elements.empty() ? 0u : elements.back().GetOffsetAfter();
		}

		size_t GetElementCount() const noexcept
		{
			return elements.size();
		}

	private:
		std::vector<Element> elements;
	};

	class Vertex
	{
		friend class VertexBuffer;
	public:
		template<VertexSystem::VertexLayout::ElementType type>
		auto& Attr() noexcept
		{
			const auto& element = layout.Resolve<Type>();
			auto pAttribute = pData + element.GetOffset();

			if constexpr (Type == VertexLayout::Position2D)
			{
				return *reinterpret_cast<DirectX::XMFLOAT2*>(pAttribute);
			}

			if constexpr (Type == VertexLayout::Position3D)
			{
				return *reinterpret_cast<DirectX::XMFLOAT3*>(pAttribute);
			}

			if constexpr (Type == VertexLayout::Texture2D)
			{
				return *reinterpret_cast<DirectX::XMFLOAT2*>(pAttribute);
			}

			if constexpr (Type == VertexLayout::Normal)
			{
				return *reinterpret_cast<DirectX::XMFLOAT3*>(pAttribute);
			}

			if constexpr (Type == VertexLayout::Float3Color)
			{
				return *reinterpret_cast<DirectX::XMFLOAT3*>(pAttribute);
			}

			if constexpr (Type == VertexLayout::Float4Color)
			{
				return *reinterpret_cast<DirectX::XMFLOAT3*>(pAttribute);
			}
		}

		template<typename T>
		void SetAttributeByIndex(size_t i, T&& val) noexcept
		{
			const auto& element = layout.ResolveByIndex(i);
			auto pAttribute = pData + element.GetOffset();

			switch (element.GetType())
			{
			case VertexLayout::Position2D:
				SetAttributeByIndex<DirectX::XMFLOAT2>(pAttribute, std::forward<T>(val));
				break;

			case VertexLayout::Position3D:
				SetAttributeByIndex<DirectX::XMFLOAT3>(pAttribute, std::forward<T>(val));
				break;

			case VertexLayout::Texture2D:
				SetAttributeByIndex<DirectX::XMFLOAT2>(pAttribute, std::forward<T>(val));
				break;

			case VertexLayout::Normal:
				SetAttributeByIndex<DirectX::XMFLOAT3>(pAttribute, std::forward<T>(val));
				break;

			case VertexLayout::Float3Color:
				SetAttributeByIndex<DirectX::XMFLOAT3>(pAttribute, std::forward<T>(val));
				break;

			case VertexLayout::Float4Color:
				SetAttributeByIndex<DirectX::XMFLOAT3>(pAttribute, std::forward<T>(val));
				break;

			case VertexLayout::BGRAColor:
				SetAttributeByIndex<unsigned int>(pAttribute, std::forward<T>(val));
				break;

			default:
				assert("Bad element type" && false);
			}
		}

		Vertex(char* pData, const VertexLayout& layout)
			:
			pData(pData),
			layout(layout)
		{
			assert(pData != nullptr);
		}
	
		template<typename First, typename ...Rest>
		void SetAttributeByIndex(size_t i, First&& first, Rest&&... rest) noexcept
		{
			SetAttributeByIndex(i, std::forward<First>(first));
			SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
		}

		template<typename Dest, typename Src>
		void SetAttribute(char* pAttribute, Src&& val) noexcept
		{
			if constexpr (std::is_assignable<Dest, Src>::value)
			{
				*reinterpret_cast<Dest*>(pAttribute) = val;
			}
			else
			{
				assert("Parameter attribute type mismatch" && false);
			}
		}

	private:
		char* pData = nullptr;
		const VertexLayout& layout;
	};

	class ConstVertex
	{
	public:
		ConstVertex(const Vertex& v) noexcept
			:
			vertex(v)
		{}

		template<VertexLayout::ElementType Type>
		const auto& Attr() const noexcept
		{
			return const_cast<Vertex&>(vertex).Attr<Type>();
		}

	private:
		Vertex vertex;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(VertexLayout layout) noexcept
			:
			layout(std::move(layout))
		{}

		const char* GetData() const noexcept
		{
			return buffer.data();
		}

		const VertexLayout& GetLayout() const noexcept
		{
			return layout;
		}

		size_t Size()  const noexcept
		{
			return buffer.size() / layout.Size();
		}

		template<typename ...Params>
		void EmplaceBack(Params&&... params) noexcept
		{
			assert(sizeof...(params) == layout.GetElementCount() && "Param count doesn't match");
			buffer.resize(buffer.size() + layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}

		Vertex Back() noexcept
		{
			assert(buffer.size() != 0u);
			return Vertex{ buffer.data() + buffer.size() - layout.Size(), layout };
		}

		Vertex Front() noexcept
		{
			assert(buffer.size() != 0u);
			return Vertex{ buffer.data(), layout };
		}

		Vertex operator[](size_t i) noexcept
		{
			assert(i < Size());
			return Vertex{ buffer.data() + layout.Size() * i, layout };
		}

		ConstVertex Back() const noexcept
		{
			return const_cast<VertexBuffer*>(this)->Back();
		}

		ConstVertex Front() const noexcept
		{
			return const_cast<VertexBuffer*>(this)->Front();
		}

		Vertex operator[](size_t i) const noexcept
		{
			return const_cast<VertexBuffer&>(*this)[i];
		}

	private:
		std::vector<char> buffer;
		VertexLayout layout;
	};
}