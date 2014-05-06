#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LEFT 0
#define RIGHT 1
#define MODE_INSERT 1
#define MODE_FIND 2
#define max(a,b) ((a)>(b)?(a):(b))

struct node {
  int m;
  int d;
  int i;
  struct node *left;
  struct node *right;
};

struct sbst {
  struct node *root;
  int max_mi; /* The maximum lcp in the tree */
  int max_i; /* The node i with m(i) == max_mi */
};

struct node *allocate_node(int m, int d, int i) {
  struct node *new_node = malloc(sizeof(*new_node));
  new_node->m = m;
  new_node->d = d;
  new_node->i = i;
  new_node->left = new_node->right = NULL;
  return new_node;
}

int lcp(char *str1, char *str2);

int find_insert_aux(struct sbst *tree, char *pattern, size_t pattern_len, char *text, size_t text_len, char mode, int text_i) {
  struct node *current, *next_current, *prev;
  int llcp, rlcp;
  int prev_dir;

  current = tree->root;
  llcp = rlcp = 0;

  while (current != NULL) {
    int max_pattern = max(llcp, rlcp);
    if (current->m > max_pattern) {
      next_current = (current->d == LEFT ? current->left : current->right);
    } else if (current->m < max_pattern) {
      if (llcp > rlcp) {
	next_current = current->right;
	if (current->d == LEFT) {
	  rlcp = current->m;
	}
      } else if (rlcp > llcp) {
	next_current = current->left;
	if (current->d == RIGHT) {
	  llcp = current->m;
	}
      }
    } else if (current->m == llcp && llcp > rlcp && current->d == RIGHT) {
      next_current = current->right;
    } else if (current->m == rlcp && rlcp > llcp && current->d == LEFT) {
      next_current = current->left;
    } else {
      int sub_lcp = lcp(pattern+current->m, text+current->m+current->i-1);
      int t = current->m + sub_lcp;
      if (t == pattern_len) {
	if (mode == MODE_FIND) {
	  return current->i;
	} else {
	  next_current = current->left;
	  llcp = t;
	}
      } else if (current->i+t-1 == text_len || pattern[t] > text[t+current->i-1]) {
	next_current = current->right;
	rlcp = t;
      } else {
	next_current = current->left;
	llcp = t;
      }
    }
    prev = current;
    prev_dir = (next_current == current->left ? LEFT : RIGHT);
    current = next_current;
  }
  if (mode == MODE_INSERT) {
    struct node *new_node = allocate_node(max(llcp, rlcp), (llcp > rlcp ? LEFT : RIGHT), text_i);
    if (prev_dir == LEFT)
      prev->left = new_node;
    else
      prev->right = new_node;
  }
  return 0;
}

void sbst_insert(struct sbst *tree, char *text, size_t text_size, int i) {
  (void) find_insert_aux(tree, text+i-1, text_size-i+1, text, text_size, MODE_INSERT, i);
}

int sbst_find(struct sbst *tree, char *text, size_t text_size, char *pattern, size_t pattern_size) {
  return find_insert_aux(tree, pattern, pattern_size, text, text_size, MODE_FIND, 0);
}

struct sbst *build_sbst(char *text) {
  if (*text == '\0')
    return NULL;

  size_t text_size = strlen(text);

  struct sbst *tree = malloc(sizeof(*tree));
  tree->root = allocate_node(0, 0, 1);
  
 for (int i = 1; text[i] != '\0'; i++)
   sbst_insert(tree, text, text_size, i+1);

  return tree;
}

void dump(struct node *n, char *text, int depth) {
  if (!n)
    return;
  for (int i = 0; i < depth; i++)
    putchar(' '), putchar(' ');
  printf("(%d|%d|%d|%s\n", n->m, n->d, n->i, text+n->i-1);
  dump(n->left, text, depth+1);
  dump(n->right, text, depth+1);
}

int lcp(char *str1, char *str2) {
  int i;
  for (i = 0; str1[i] != '\0' && str1[i] == str2[i]; i++);
  return i;
}

int main(void) {
  char *text = "banana";
  char *pattern = "ana";
  struct sbst *tree = build_sbst(text);
  //printf("%d\n", sbst_find(tree, text, strlen(text), pattern, strlen(pattern)));
  dump(tree->root, text, 0);
  return 0;
}




