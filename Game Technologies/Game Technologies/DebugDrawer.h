#pragma once
/**
 * Author: Callum Rhodes <c.g.rhodes@ncl.ac.uk>
 */

class DebugDrawer {
public:
	virtual ~DebugDrawer() {}

	virtual void DebugDraw() = 0;
};