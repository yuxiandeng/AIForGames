#pragma once

#include "BTTick.h"

#include <vector>

enum class BT_State : uint8_t
{
	SUCCESS,
	FAILURE,
	RUNNING,
	ERROR
};

class BTNode
{
public:
	BTNode() = default;
	virtual ~BTNode() = default;
	
	std::vector<BTNode*>& GetChildren() { return mChildren; }

	void Enter(BTTick inTick);
	void Open(BTTick inTick);
	BT_State Execute(BTTick inTick);
	void Close(BTTick inTick);
	void Exit(BTTick inTick);

	BT_State Run(BTTick inTick);

	uint8_t GetID() { return ID; }

protected:
	virtual void OnEnter(BTTick inTick) {}
	virtual void OnOpen(BTTick inTick) {}
	virtual BT_State OnExecute(BTTick inTick) {}
	virtual void OnClose(BTTick inTick) {}
	virtual void OnExit(BTTick inTick) {}

	std::vector<BTNode*> mChildren;
	uint8_t ID;	
};