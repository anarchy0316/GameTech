#pragma once
/**
 * Author: Callum Rhodes <c.g.rhodes@ncl.ac.uk>
 */

class Constraint {
public:
	virtual ~Constraint() {}

	virtual void Update(float msec) = 0;
};