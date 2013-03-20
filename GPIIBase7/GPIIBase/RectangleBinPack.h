/** @file RectangleBinPack.h
	@author Jukka Jylänki

	This work is released to Public Domain, do whatever you want with it.

	@brief RectangleBinPack is a data structure for performing online rectangle bin 
packing.
*/
#ifndef RectangleBinPack_h
#define RectangleBinPack_h

/** Solves the online rectangle packing problem into a rectangular bin by maintaining 
	a binary tree of used and free rectangles of the bin.
	See
	- Rectangle packing: http://www.gamedev.net/community/forums/topic.asp?topic_id=392413
	- Packing lightmaps: http://www.blackpawn.com/texts/lightmaps/default.html
	
	Idea: Instead of just picking the first free rectangle to insert the new rect into,
	      check all free ones and pick the one that minimizes the resulting leftover 
area. */
class RectangleBinPack
{
public:
	/// A node of a binary tree. Internal nodes store a rectangle of used data,
	/// whereas leaf nodes track a rectangle of free space. All the rectangles stored
	/// in the nodes of the tree are disjoint.
	struct Node
	{
		Node() {left=nullptr;right=nullptr;x=0;y=0;width=0;height=0;};
		Node *left;
		Node *right;

		int x;
		int y;
		int width;
		int height;
	};

	/// Resets the bin into a given fixed initial size.
	void Init(int width, int height)
	{
		root.left = root.right = nullptr;
		root.x = root.y = 0;
		root.width = width;
		root.height = height;
	}

	/// Inserts a new rectangle of the given size into the bin.
	/// @return A pointer to the node that stores the newly added rectangle, or 0 
	///	if it didn't fit.
	Node *Insert(int width, int height)
	{
		return Insert(&root, width, height);
	}

private:
	Node root;

	/// Inserts a new rectangle in the subtree rooted at the given node.
	Node *Insert(Node *node, int width, int height)
	{
		// If this node is an internal node, try both leaves for possible space.
		// (The rectangle in an internal node stores used space, the leaves store free space)
		if (node->left || node->right)
		{
			if (node->left)
			{
				Node *newNode = Insert(node->left, width, height);
				if (newNode)
					return newNode;
			}
			if (node->right)
			{
				Node *newNode = Insert(node->right, width, height);
				if (newNode)
					return newNode;
			}
			return 0; // Didn't fit into either subtree!
		}

		// This node is a leaf, but can we fit the new rectangle here?
		if (width > node->width || height > node->height)
			return 0; // Too bad, no space.

		// The new cell will fit, split the remaining space along the shorter axis,
		// that is probably more optimal.
		int w = node->width - width;
		int h = node->height - height;
		node->left = new Node;
		node->right = new Node;
		if (w <= h) // Split the remaining space in horizontal direction.
		{
			node->left->x = node->x + width;
			node->left->y = node->y;
			node->left->width = w;
			node->left->height = height;

			node->right->x = node->x;
			node->right->y = node->y + height;
			node->right->width = node->width;
			node->right->height = h;
		}
		else // Split the remaining space in vertical direction.
		{
			node->left->x = node->x;
			node->left->y = node->y + height;
			node->left->width = width;
			node->left->height = h;

			node->right->x = node->x + width;
			node->right->y = node->y;
			node->right->width = w;
			node->right->height = node->height;
		}
		// Note that as a result of the above, it can happen that node->left or node->right
		// is now a degenerate (zero area) rectangle. No need to do anything about it,
		// like remove the nodes as "unnecessary" since they need to exist as children of
		// this node (this node can't be a leaf anymore).

		// This node is now a non-leaf, so shrink its area - it now denotes
		// *occupied* space instead of free space. Its children spawn the resulting
		// area of free space.
		node->width = width;
		node->height = height;
		return node;
	}
};

#endif

