#ifndef MODELTREE_H
#define MODELTREE_H

#include "Triangle.h"
#include "Box.h"
#include <vector>
#include <stdlib.h>
#include <fstream>

class ModelTreeNode
{
	public:
		ModelTreeNode();
		virtual ~ModelTreeNode();
		virtual void Add(Triangle* t, int layers);
		virtual float Test(Ray &r);
		virtual void Write(std::fstream* outfile, Triangle* first);
		virtual void Read(std::ifstream* infile, Triangle* first);
		virtual int GetType();
};

class ModelTreeParent : public ModelTreeNode
{
	public:
		ModelTreeParent();
		ModelTreeParent(Vec3 min, Vec3 max);
		~ModelTreeParent();
		void Add(Triangle* t, int layers);
		float Test(Ray &r);
		void Write(std::fstream* outfile, Triangle* first);
		void Read(std::ifstream* infile, Triangle* first);
		int GetType();
	
		ModelTreeNode* left;
		ModelTreeNode* right;

		Vec3 min;
		Vec3 max;

	private:
		bool InfrontOfPlane(Vec3 point, Vec3 &center, Vec3 &normal);
};

class ModelTreeLeaf : public ModelTreeNode
{
	public:
		ModelTreeLeaf();
		~ModelTreeLeaf();
		void Add(Triangle* t, int layers);
		float Test(Ray &r);
		void Write(std::fstream* outfile, Triangle* first);
		void Read(std::ifstream* infile, Triangle* first);
		int GetType();

		std::vector<Triangle*> triangles;
};


#endif