#pragma once

#include "DrawableBase.h"
#include "Topology.h"
#include "TransformConstantBuffer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "VertexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include <optional>

class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtrs);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
	friend class ModelWindow;
public:
	Node(int id, const std::string name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept;
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;
	
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	void SetBaseTransform(DirectX::FXMMATRIX transform) noexcept;

	int GetID() const noexcept;

private:
	void AddChild(std::unique_ptr<Node> pChild) noexcept;
	void ShowTree(std::optional<int>& selectedIndex, Node*& pSelectedNode) const noexcept;
private:
	std::string name;
	int id;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	
	DirectX::XMFLOAT4X4 baseTransform;
	DirectX::XMFLOAT4X4 appliedTransform;
};

class Model
{
public:
	Model(Graphics& gfx, const std::string fileName);
	void Draw(Graphics& gfx) const;
	void SetRootTransform(DirectX::FXMMATRIX transform) noexcept;
	void SetChildrenTransform(int child, DirectX::FXMMATRIX transform) noexcept;
	void ShowWindow(const char* windowName);
	~Model() noexcept;

private:
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh);
	std::unique_ptr<Node> ParseNode(int& nextID, const aiNode& node) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::unique_ptr<class ModelWindow> pWindow;
};