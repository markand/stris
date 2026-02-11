#ifndef STRIS_NODE_H
#define STRIS_NODE_H

struct node {
	struct tex *texture;
	int hide;
	int x;
	int y;
#if 0
	float angle;
#endif
};

void
node_enable(struct node *node, struct tex *texture);

void
node_disable(struct node *node);

void
node_move(struct node *node, int x, int y);

void
node_show(struct node *node);

void
node_hide(struct node *node);

void
node_render(struct node *node);

#endif /* !STRIS_NODE_H */
