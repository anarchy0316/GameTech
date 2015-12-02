#pragma once
/**
 * Author: Callum Rhodes <c.g.rhodes@ncl.ac.uk>
 */


#include "../../nclgl/Matrix4.h"

class InertialMatrixHelper {
public:

	static Matrix4 createSphereInertial(float mass, float radius) {
		float I = 0.4f*mass*radius*radius;
		float elements[] = {I, 0, 0, 0, 0, I, 0, 0, 0, 0, I, 0, 0, 0, 0, 1};
		return Matrix4(elements);
	}

	static Matrix4 createSphereInvInertial(float mass, float radius) {
		float I = 2.5f/(mass*radius*radius);
		float elements[] = {I, 0, 0, 0, 0, I, 0, 0, 0, 0, I, 0, 0, 0, 0, 1};
		return Matrix4(elements);
	}

	static Matrix4 createCuboidInertial(float mass, float width, float height, float depth) {
		float Ixx = 0.0833f*mass*(depth*depth + height*height);
		float Iyy = 0.0833f*mass*(width*width + depth*depth);
		float Izz = 0.0833f*mass*(width*width + height*height);
		float elements[] = {Ixx, 0, 0, 0, 0, Iyy, 0, 0, 0, 0, Izz, 0, 0, 0, 0, 1};
		return Matrix4(elements);
	}

	static Matrix4 createCuboidInvInertial(float mass, float width, float height, float depth) {
		float Ixx = 12.0f/(mass*(depth*depth + height*height));
		float Iyy = 12.0f/(mass*(width*width + depth*depth));
		float Izz = 12.0f/(mass*(width*width + height*height));
		float elements[] = {Ixx, 0, 0, 0, 0, Iyy, 0, 0, 0, 0, Izz, 0, 0, 0, 0, 1};
		return Matrix4(elements);
	}

	static Matrix4 createImmovableInvInertial() {
		float elements[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
		return Matrix4(elements);
	}

};