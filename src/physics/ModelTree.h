#ifndef MODELTREE_H
#define MODELTREE_H

#include "Triangle.h"
#include "Box.h"
#include <vector>
#include <stdlib.h>

class ModelTreeNode
{
	public:
		ModelTreeNode();
		virtual ~ModelTreeNode();
		virtual void Add(Triangle* t, int layers);
		virtual float Test(Ray &r);
};

class ModelTreeParent : public ModelTreeNode
{
	public:
		ModelTreeParent(Vec3 min, Vec3 max);
		~ModelTreeParent();
		void Add(Triangle* t, int layers);
		float Test(Ray &r);
	private:
		ModelTreeNode* left;
		ModelTreeNode* right;

		Vec3 min;
		Vec3 max;

		bool InfrontOfPlane(Vec3 point, Vec3 &center, Vec3 &normal);
};

class ModelTreeLeaf : public ModelTreeNode
{
	public:
		ModelTreeLeaf();
		~ModelTreeLeaf();
		void Add(Triangle* t, int layers);
		float Test(Ray &r);
	private:
		std::vector<Triangle*> triangles;
};


#endif