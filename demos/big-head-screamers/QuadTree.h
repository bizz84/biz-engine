/*****************************************************************************
 * Filename			CollisionDetector.h
 * 
 * License			GPLv3
 *
 * Author			Andrea Bizzotto (bizz84@gmail.com)
 *
 * Platform			LinuxX11 / OpenGL
 * 
 * Description		Collision Detector
 *
 *****************************************************************************/
#ifndef _QUAD_TREE_H_
#define _QUAD_TREE_H_


#include "boost/multi_array.hpp"
#include "boost/ptr_container/ptr_vector.hpp"
using namespace boost;
#include <list>
using namespace std;

#include "Vector.h"

class Enemy;

// QuadTree class
class QuadTree
{
	const int N;

	//! Two dimensional array of list<Enemy *> elements
	typedef boost::multi_array<list<Enemy *>, 2> array_type;
	typedef array_type::index index;

	array_type bins;

	Point2 min, max;

public:
	explicit QuadTree(const int n, ptr_vector<Enemy> &data);

	void Insert(Enemy *enemy, const Point2 &min, const Point2 &max);

	list<Enemy *> *GetBin(const Point2 &pos);

	inline int Slot(float pos, const int i) const
	{
		return (int)(N * (pos - min[i]) / (max[i] - min[i]));
	}
};

#endif