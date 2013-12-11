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
		virtual bool Test(Triangle &t);
		virtual bool Test(ModelTreeNode* mtp);
		virtual void Write(std::fstream* outfile, Triangle* first);
		virtual void Read(std::ifstream* infile, Triangle* first);
		virtual std::vector<std::vector<Triangle*>*> GetTriangles(Box b);
		virtual int GetType();

		virtual void SetPosition(Vec3 pos);
	protected:
		Vec3 Position;
};

class ModelTreeParent : public ModelTreeNode
{
	public:
		ModelTreeParent();
		ModelTreeParent(Vec3 min, Vec3 max);
		~ModelTreeParent();
		void Add(Triangle* t, int layers);
		float Test(Ray &r);
		bool Test(Triangle &t);
		bool Test(ModelTreeNode* mtp);
		void Write(std::fstream* outfile, Triangle* first);
		void Read(std::ifstream* infile, Triangle* first);
		std::vector<std::vector<Triangle*>*> GetTriangles(Box b);
		int GetType();
	
		ModelTreeNode* left;
		ModelTreeNode* right;

		Vec3 min;
		Vec3 max;

		void SetPosition(Vec3 pos);

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
		bool Test(Triangle &t);
		bool Test(ModelTreeNode* mtp);
		void Write(std::fstream* outfile, Triangle* first);
		void Read(std::ifstream* infile, Triangle* first);
		std::vector<std::vector<Triangle*>*> GetTriangles(Box b);
		int GetType();

		void SetPosition(Vec3 pos);

		std::vector<Triangle*> triangles;
};


#endif