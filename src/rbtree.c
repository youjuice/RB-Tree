#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void) {
  // TODO: initialize struct if needed
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));
  
  nil_node->color = RBTREE_BLACK;
  nil_node->parent = nil_node;
  nil_node->left = nil_node;
  nil_node->right = nil_node;

  p->root = nil_node;
  p->nil = nil_node;

  return p;
}



// ===================== Rotate =====================
// Left Rotate
void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right;               // y 지정
  x->right = y->left;                 // 1. 서브트리 이동 및 연결

  if (y->left != t->nil) {
    y->left->parent = x;
  }
  y->parent = x->parent;              // 2. 부모 연결
                                      // 3. x의 위치에 맞게 서브트리 삽입 
  if (x->parent == t->nil) {          // 3-1. x가 root 일 때
    t->root = y;                      
  }
  else if (x == x->parent->left) {    // 3-2. x가 오른쪽에 위치할 때
    x->parent->left = y;
  }
  else {                              // 3-3. x가 왼쪽에 위치할 때
    x->parent->right = y;
  }

  y->left = x;                        // 4. x 위치 조정
  x->parent = y;
}

// Right Rotate
void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left;                // y 지정
  x->left = y->right;                 // 1. 서브트리 이동 및 연결

  if (y->right != t->nil) {
    y->right->parent = x;
  }
  y->parent = x->parent;              // 2. 부모 연결
                                      // 3. x의 위치에 맞게 서브트리 삽입
  if (x->parent == t->nil) {          // 3-1. x가 root 일 때
    t->root = y;
  }
  else if (x == x->parent->right) {   // 3-2. x가 오른쪽에 위치할 때
    x->parent->right = y;
  }
  else {                              // 3-3. x가 왼쪽에 위치할 때
    x->parent->left = y;
  }

  y->right = x;                       // 4. x 위치 조정
  x->parent = y;
}



// ===================== Delete All =====================
// 후위 순회로 돌면서 삭제 
void postorder_delete(node_t *root, node_t *nil) {
  if (nil == root)
    return;

  postorder_delete(root->left, nil);
  postorder_delete(root->right, nil);
  free(root);
}

// 전체 트리 삭제 함수
void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  postorder_delete(t->root, t->nil);

  free(t->nil);
  free(t);
}



// ===================== Insert =====================
// Insert 함수
node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  z->key = key;

  node_t *y = t->nil;           // y를 nil로 초기화
  node_t *x = t->root;          // x를 root로 설정

  while (x != t->nil) {         // 루트 노드가 nil이 아닐 때까지
    y = x;                      // y에 루트 노드 저장
    if (z->key < x->key) {      // 타고 내려가면서 새로운 노드의 위치 탐색
      x = x->left;
    }
    else {
      x = x->right;
    }
  }

  z->parent = y;                // z의 부모 노드를 y로 변경
  if (y == t->nil) {            // z와 y의 관계 설정
    t->root = z;
  }
  else if (z->key < y->key) {
    y->left = z;
  }
  else {
    y->right = z;
  }

  z->left = t->nil;             // z의 자식 노드를 nil로 설정 (리프 노드로 삽입되므로)
  z->right = t->nil;
  z->color = RBTREE_RED;        // 새로운 z 노드의 색은 RED

  rbtree_insert_fixup (t, z);
  return z;
}

// Insert_fixup
void rbtree_insert_fixup(rbtree *t, node_t *z) {
  while (z->parent->color == RBTREE_RED) {
    if (z->parent == z->parent->parent->left) {     // 부모가 왼쪽 자식이라면,
      node_t *y = z->parent->parent->right;         // y에 삼촌 노드 할당
      if (y->color == RBTREE_RED) {                 // 삼촌이 빨간색이라면, Recoloring
        z->parent->color = RBTREE_BLACK;            // 부모 색 검정으로 변경
        y->color = RBTREE_BLACK;                    // 삼촌 색 검정으로 변경
        z->parent->parent->color = RBTREE_RED;      // 조상 색 빨강으로 변경
        z = z->parent->parent;                      // z 포인터 변경
      }
      else {                                        // 삼촌이 검정색이라면, Restructuring
        if (z == z->parent->right) {                // Case 1: Restructuring (Triangle)
          z = z->parent;
          left_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;            // Case 2: Restructuring (Line)
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }

    else {                                          // 부모가 오른쪽 자식이라면,
      node_t *y = z->parent->parent->left;          // y에 삼촌 노드 할당
      if (y->color == RBTREE_RED) {                 // 삼촌이 빨간색이라면, Recoloring
        z->parent->color = RBTREE_BLACK;            // 부모 색 검정으로 변경
        y->color = RBTREE_BLACK;                    // 삼촌 색 검정으로 변경
        z->parent->parent->color = RBTREE_RED;      // 조상 색 빨강으로 변경
        z = z->parent->parent;                      // z 포인터 변경
      }
      else {                                        // 삼촌이 검정색이라면, Restructuring
        if (z == z->parent->left) {                 // Case 1: Restructuring (Triangle)
          z = z->parent;
          right_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;            // Case 2: Restructuring (Line)
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;                    // 루트 색은 검정으로 유지
}



// ===================== Search =====================
node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *x = t->root;

  while (x != t->nil) {
    if (x->key > key) {
      x = x->left;
    }

    else if (x->key < key) {
      x = x->right;
    }

    else {
      // printf("%d", x->key);
      return x;
    }
  }
  // printf("%d", key);
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *x = t->root;
  node_t *min = t->root;

  while (x != t-> nil) {
    min = x;
    x = x->left;  
  }
  return min;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *x = t->root;
  node_t *max = t->root;

  while (x != t->nil) {
    max = x;
    x = x->right;
  }
  return max;
}

node_t *rbtree_minimum(const rbtree *t, node_t *sub_root) {
  node_t *x = sub_root;
  if (x == t->nil) {
    return x;
  }
  while (x->left != t->nil) {
    x = x->left;
  }
  return x;
}



// ===================== Delete =====================
// transplant
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  // u: 삭제할 노드 v: 대체할 노드
  if (u->parent == t->nil) {
    t->root = v;
  }
  else if (u == u->parent->left) {
    u->parent->left = v;
  }
  else {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

// delete
int rbtree_erase(rbtree *t, node_t *z) {
  // TODO: implement erase
  node_t *curr = z;                       // 삭제할 노드 저장
  color_t origin_color = curr->color;     // + 색 저장
  node_t *x;

  // 자식이 하나 있거나 없을 경우 (curr은 삭제되는 노드)
  if (z->left == t->nil) {
    x = z->right;
    rbtree_transplant(t, z, z->right);
  }
  else if (z->right == t->nil) {
    x = z->left;
    rbtree_transplant(t, z, z->left);
  }

  // 자식이 둘인 경우 (curr은 후임자)
  else {
    curr = rbtree_minimum(t, z->right);
    origin_color = curr->color;                 // 후임자의 색 저장
    x = curr->right;

    if (curr->parent == z) {                    // 후임자가 z의 자식일 경우
      x->parent = curr;
    }
    else {                                      // 후임자가 z의 자식이 아닐 경우
      rbtree_transplant(t, curr, curr->right);
      curr->right = z->right;               
      curr->right->parent = curr;
    }
    rbtree_transplant(t, z, curr);              // 대체 작업
    curr->left = z->left;
    curr->left->parent = curr;
    curr->color = z->color;
  }

  if (origin_color == RBTREE_BLACK) {           // 삭제되는 색이 검정이라면,
    rbtree_delete_fixup(t, x);                  // Fix_up 호출
  }
  free(z);
  return 0;
}

// delete_fixup
void rbtree_delete_fixup(rbtree *t, node_t *x) {
  while (x != t->root && x->color == RBTREE_BLACK) {
    // x가 왼쪽 자식일 때
    if (x == x->parent->left) {             
      node_t *w = x->parent->right;         
      if (w->color == RBTREE_RED) {                                               // Case 1: 형제 노드가 RED일 때
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }

      // Case 2,3,4: 형제 노드가 BLACK일 때 
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {    // Case 2: 형제의 자식들이 모두 BLACK일 때
        w->color = RBTREE_RED;
        x = x->parent;
      }

      else {
        if (w->right->color == RBTREE_BLACK) {                                    // Case 3: 형제의 오른쪽 자식이 BLACK일 때
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }

        w->color = x->parent->color;                                              // Case 4: 형제의 오른쪽 자식이 RED일 때
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      } 
    }

    // x가 오른쪽 자식일 때
    else {
      node_t *w = x->parent->left;
      if (w->color == RBTREE_RED) {                                               // Case 1: 형제 노드가 RED일 때
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }

      // Case 2,3,4: 형제 노드가 BLACK일 때
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {    // Case 2: 형제의 자식들이 모두 BLACK일 때
        w->color = RBTREE_RED;
        x = x->parent;
      }

      else {
        if (w->left->color == RBTREE_BLACK) {                                     // Case 3: 형제의 오른쪽 자식이 BLACK일 때
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }

        w->color = x->parent->color;                                              // Case 4: 형제의 오른쪽 자식이 RED일 때
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;                                                        // 루트 노드는 BLACK
}



// ===================== Array =====================
void inorder_array(node_t *root, node_t *nil, key_t *arr, int *index) {
  if (root == nil) {
    return;
  }

  inorder_array(root->left, nil, arr, index);

  arr[*index] = root->key;
  (*index)++;

  inorder_array(root->right, nil, arr, index);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  int *index = calloc(1, sizeof(int));
  inorder_array(t->root, t->nil, arr, index);
  free(index);
  return 0;
}
