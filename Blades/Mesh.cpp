#include "Mesh.h"

#include "ImGUI/imgui.h"
#include <unordered_map>

Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtrs)
{
	if (!IsStaticInitialized())
	{
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	for (auto& pb : bindPtrs)
	{
		if (auto pi = dynamic_cast<IndexBuffer*>(pb.get()))
		{
			AddIndexBuffer(std::unique_ptr<IndexBuffer>{pi});
			pb.release();
		}
		else
		{
			AddBind(std::move(pb));
		}
	}

	AddBind(std::make_unique<TransformConstantBuffer>(gfx, *this));
}

void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}



Node::Node(int id, const std::string name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept
	:
	meshPtrs(std::move(meshPtrs)),
	name(name),
	id(id)
{
	DirectX::XMStoreFloat4x4(&baseTransform, transform);
	DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
	const auto built =
		DirectX::XMLoadFloat4x4(&baseTransform) *
		DirectX::XMLoadFloat4x4(&appliedTransform) *
		accumulatedTransform;

	for (const auto pm : meshPtrs)
	{
		pm->Draw(gfx, built);
	}

	for (const auto& pc : childPtrs)
	{
		pc->Draw(gfx, built);
	}
}

void Node::ShowTree(std::optional<int>& selectedIndex, Node*& pSelectedNode) const noexcept
{
	// build up flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((GetID() == selectedIndex.value_or(-1)) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);

	const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)GetID(), node_flags, name.c_str());

	if (ImGui::IsItemClicked())
	{
		selectedIndex = GetID();
		pSelectedNode = const_cast<Node*>(this);
	}

	if (expanded)
	{		
		for (const auto& pChild : childPtrs)
		{
			pChild->ShowTree(selectedIndex, pSelectedNode);
		}
		ImGui::TreePop();
	}
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept
{
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&appliedTransform, transform);
}

void Node::SetBaseTransform(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&baseTransform, transform);
}

int Node::GetID() const noexcept
{
	return id;
}



class ModelWindow
{
public:
	void Show(const char* windowName, const Node& root)
	{
		windowName = windowName ? windowName : "Model";
		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, true);
			root.ShowTree(selectedIndex, pSelectedNode);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr)
			{
				auto& transform = transforms[*selectedIndex];
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
			}
		}
		ImGui::End();
	}

	DirectX::XMMATRIX GetTransform() const noexcept
	{
		const auto& transform = transforms.at(*selectedIndex);

		return DirectX::XMMatrixRotationRollPitchYaw(transform.pitch, transform.yaw, transform.roll)
			* DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}

	Node* GetSelectedNode() const noexcept
	{
		return pSelectedNode;
	}

private:
	std::optional<int> selectedIndex;
	Node* pSelectedNode;
	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, TransformParameters> transforms;
};

Model::Model(Graphics& gfx, const std::string fileName)
	:
	pWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded | 
		aiProcess_JoinIdenticalVertices
	);

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));
	}

	int nextID = 0;
	pRoot = ParseNode(nextID, *pScene->mRootNode);
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh)
{
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 n;
	};

	std::vector<Vertex> vertices;
	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vertices.push_back(
			{
				{
					mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z
				},
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i])
			}
		);
	}

	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color = { 0.6f, 0.6f, 0.6f };
		float specularIntensity = 0.7f;
		float specularPower = 50.0f;
		float padding[3];
	} pmc; // Material

	std::vector<std::unique_ptr<Bindable>> bindbalePtrs;

	bindbalePtrs.push_back(std::make_unique<VertexBuffer>(gfx, vertices));
	bindbalePtrs.push_back(std::make_unique<IndexBuffer>(gfx, indices));

	auto pVs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
	auto pvsbc = pVs->GetBytecode();

	bindbalePtrs.push_back(std::move(pVs));
	bindbalePtrs.push_back(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

	bindbalePtrs.push_back(std::make_unique<InputLayout>(gfx, ied, pvsbc));
	bindbalePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

	return std::make_unique<Mesh>(gfx, std::move(bindbalePtrs));
}

void Model::Draw(Graphics& gfx) const
{
	if (auto node = pWindow->GetSelectedNode())
	{
		node->SetAppliedTransform(pWindow->GetTransform());
	}
	pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
}

void Model::SetRootTransform(DirectX::FXMMATRIX transform) noexcept
{
	pRoot->SetBaseTransform(transform);
}

void Model::SetChildrenTransform(int child, DirectX::FXMMATRIX transform) noexcept
{
	pRoot->childPtrs[child]->SetBaseTransform(transform);
}

void Model::ShowWindow(const char* windowName)
{
	pWindow->Show(windowName, *pRoot);
}

Model::~Model()
{
}

std::unique_ptr<Node> Model::ParseNode(int& nextID, const aiNode& node) noexcept
{
	const auto transform = DirectX::XMMatrixTranspose(
		DirectX::XMLoadFloat4x4(
			reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
		)
	);

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(nextID++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextID, *node.mChildren[i]));
	}

	return pNode;
}