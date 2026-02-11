#include <assert.h>

#include "node.h"
#include "stris.h"
#include "tex.h"
#include "util.h"

void
node_enable(struct node *node, struct tex *texture)
{
	assert(node);
	assert(texture);

	struct node **slot = NULL;

	for (size_t i = 0; i < LEN(stris.nodes); ++i) {
		if (!stris.nodes[i]) {
			slot = &stris.nodes[i];
			break;
		}
	}

	if (!slot)
		die("abort: node space exceeded\n");

	node->texture = texture;
	node->x = node->y = 0;

	*slot = node;
}

void
node_disable(struct node *node)
{
	for (size_t i = 0; i < LEN(stris.nodes); ++i) {
		if (stris.nodes[i] == node) {
			stris.nodes[i] = NULL;
			break;
		}
	}
}

void
node_move(struct node *node, int x, int y)
{
	assert(node);

	node->x = x;
	node->y = y;
}

void
node_show(struct node *node)
{
	assert(node);

	node->hide = 0;
}

void
node_hide(struct node *node)
{
	assert(node);

	node->hide = 1;
}

void
node_render(struct node *node)
{
	assert(node);

	if (!node->hide)
		tex_draw(node->texture, node->x, node->y);
}
