Node *tree_traversal(QNode *root, char * name) {
	if(root->node_type == LEAF) {
		Node * left_child = root->children[0].fchild;
		Node * right_child = root->children[1].fchild;
		if(contains(left_child, name)) return left_child;
		if(contains(right_child, name)) return right_child;
		return NULL;
	}
	Node * left = tree_traversal(root->children[0].qchild, name);
	Node * right = tree_traversal(root->children[1].qchild, name);
	if(left != NULL)
		return left;
	return right;
}

void add_user(QNode *root, char *username, int *ans) {
	QNode *temp = root;
	int i = 0;
	while(temp->node_type != LEAF) {
		temp = temp->children[ans[i]].qchild;
		i++;
	}
	Node * group = temp->children[ans[i]].fchild;

	Node * new_node = malloc(sizeof(Node));
	new_node->str = malloc(sizeof(char)*(strlen(username)+1));
	strcpy(new_node->str, username);
	new_node->next = NULL;

	if(group == NULL)
		temp->children[ans[i]].fchild = new_node;
	else {
		Node * tp = group;
		while(tp->next != NULL)
			tp = tp->next;
		tp->next = new_node;
	}
}

QNode *add_next_level (QNode *current, Node *list_node) {
	int str_len;

	str_len = strlen (list_node->str);
	current = (QNode *) calloc (1, sizeof(QNode));

	current->question =  (char *) calloc (str_len +1, sizeof(char));
	strncpy ( current->question, list_node->str, str_len );
	current->question [str_len] = '\0';
	current->node_type = REGULAR;

	if (list_node->next == NULL) {
		current->node_type = LEAF;
		return current;
	}

	current->children[0].qchild = add_next_level ( current->children[0].qchild, list_node->next);
	current->children[1].qchild = add_next_level ( current->children[1].qchild, list_node->next);

	return current;
}

void print_qtree (QNode *parent, int level) {
	int i;
	for (i=0; i<level; i++)
		printf("\t");

	printf ("%s type:%d\n", parent->question, parent->node_type);
	if(parent->node_type == REGULAR) {
		print_qtree (parent->children[0].qchild, level+1);
		print_qtree (parent->children[1].qchild, level+1);
	}
	else { //leaf node
		for (i=0; i<(level+1); i++)
			printf("\t");
		print_users (parent->children[0].fchild);
		for (i=0; i<(level+1); i++)
			printf("\t");
		print_users (parent->children[1].fchild);
	}
}

void print_users (Node *parent) {
	if (parent == NULL)
		printf("NULL\n");
	else {
		printf("%s, ", parent->str);
		while (parent->next != NULL) {
			parent = parent->next;
			printf("%s, ", parent->str);
		}
		printf ("\n");
	}
}
