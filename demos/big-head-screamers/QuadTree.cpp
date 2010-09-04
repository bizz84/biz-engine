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
		col = (int)(N * (iter->pos[0] - min[0]) / (max[0] - min[0]));
		//if (col == N) col = N - 1;
		// row along z axis
		row = (int)(N * (iter->pos[1] - min[1]) / (max[1] - min[1]));
		//if (row == N) row = N - 1;

		assert(col >= 0 && col < N);
		assert(row >= 0 && row < N);

		bins[row][col].push_back(&*iter);
	}
}

list<Enemy *> *QuadTree::GetBin(const Point2 &pos)
{
	int col = (int)(N * (pos[0] - min[0]) / (max[0] - min[0]));
	int row = (int)(N * (pos[1] - min[1]) / (max[1] - min[1]));
	if (col < 0 || col >= N || row < 0 || row >= N)
		return NULL;
	return &bins[row][col];
}


// QuadTree implementation
/*void QuadTree::Clear()
{
	for (index i = 0; i < N; i++)
	{
		for (index j = 0; j < N; j++)
		{
			bins[i][j].clear();
		}
	}
}*/

void QuadTree::Insert(Enemy *enemy, const Point2 &min, const Point2 &max)
{
	// col along x axis
	unsigned int col = (unsigned int)(N * (enemy->pos[0] - min[0]) / (max[0] - min[0]));
	// row along z axis
	unsigned int row = (unsigned int)(N * (enemy->pos[1] - min[1]) / (max[1] - min[1]));

	bins[row][col].push_back(enemy);
}
