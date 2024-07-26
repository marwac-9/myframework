#include "LineNode.h"

LineNode::LineNode()
{
	this->TopDownTransform = glm::mat4(1);
	this->localPosition = glm::vec3(0.f,0.f,0.f);
}

LineNode::~LineNode()
{
}

void LineNode::UpdateNode(const LineNode& parentNode)
{
	this->localPosition = this->localPosition + parentNode.localPosition;
	for (size_t i = 0; i < children.size(); i++)
	{
		children.at(i)->UpdateNode(*this);
	}
}