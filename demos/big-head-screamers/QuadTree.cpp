/*****************************************************************************
 * Filename			CollisionDetector.cpp
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

#include "QuadTree.h"

#include "Enemy.h"

// TODO: how to speed up?
QuadTree::QuadTree(const int n, ptr_vector<Enemy> &data)
	: N(n), bins(boost::extents[N][N])
{
	if (data.empty())
		return;
	
	min = max = data.begin()->pos;
	
	ptr_vector<Enemy>::iterator iter;

	// Update all enemies position
	for (iter = data.begin() + 1; iter != data.end(); iter++)
	{
		//x 
		if (iter->pos[0] > max[0])
			max[0] = iter->pos[0];
		if (iter->pos[0] < min[0])
			min[0] = iter->pos[0];
		// y
		if (iter->pos[1] > max[1])
			max[1] = iter->pos[1];
		if (iter->pos[1] < min[1])
			min[1] = iter->pos[1];
	}
	// add small offset
	Point2 offset(0.001f * (max[0] - min[0]), 0.001f * (max[1] - min[1]));
	max += offset;
	min -= offset;
	// Fill table
	int col, row;
	for (iter = data.begin(); iter != data.end(); iter++)
	{
		// col along x axis
		col = Slot(iter->pos[0], 0);
		// row along z axis
		row = Slot(iter->pos[1], 1);

		//assert(col >= 0 && col < N);
		//assert(row >= 0 && row < N);

		bins[row][col].push_back(&*iter);
	}
}

list<Enemy *> *QuadTree::GetBin(const Point2 &pos)
{
	int col = Slot(pos[0], 0);
	if (col < 0 || col >= N)
		return NULL;
	int row = Slot(pos[1], 1);
	if (row < 0 || row >= N)
		return NULL;
	return &bins[row][col];
}
