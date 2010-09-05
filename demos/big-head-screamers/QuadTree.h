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
	//! Index type to two dimensional array
	typedef array_type::index index;

	//! Actual array
	array_type bins;

	// Area limits 
	Point2 min, max;

public:
	//! Constructor
	explicit QuadTree(const int n, ptr_vector<Enemy> &data);

	//! returns list associated with given position, or NULL if position outside
	//! regular area
	list<Enemy *> *GetBin(const Point2 &pos);

	//! Finds appropriate horizontal or vertical index
	inline int Slot(float pos, const int i) const
	{
		return (int)(N * (pos - min[i]) / (max[i] - min[i]));
	}
};

#endif