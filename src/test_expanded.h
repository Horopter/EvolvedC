# 1 "test.h"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "test.h"
# 1265 "test.h"
typedef struct ec_slist_int_node_s
{
  int data;
  struct ec_slist_int_node_s *next;
} ec_slist_int_node_t;
typedef struct ec_slist_int_conf_s
{
  void *mempool;
} ec_slist_int_conf_t;
typedef struct ec_slist_int_s ec_slist_int_t;
struct ec_slist_int_s
{
  ec_slist_int_node_t *head;
  ec_slist_int_node_t *tail;
  size_t size;
  EC_MUTEX_TYPE lock;
  void *mempool;
  void (*conf_init)(ec_slist_int_conf_t *conf);
  ec_status_t (*new)(ec_slist_int_t **list);
  ec_status_t (*new_conf)(const ec_slist_int_conf_t *, ec_slist_int_t **list);
  ec_status_t (*destroy)(ec_slist_int_t *list);
  ec_status_t (*destroy_cb)(ec_slist_int_t *list, void (*cb)(int));
  size_t (*struct_size)(void);
  ec_status_t (*splice)(ec_slist_int_t *l1, ec_slist_int_t *l2);
  ec_status_t (*splice_at)(ec_slist_int_t *l1, ec_slist_int_t *l2, size_t idx);
  ec_status_t (*add)(ec_slist_int_t *list, int e);
  ec_status_t (*add_at)(ec_slist_int_t *list, int e, size_t idx);
  ec_status_t (*add_all)(ec_slist_int_t *l1, ec_slist_int_t *l2);
  ec_status_t (*add_all_at)(ec_slist_int_t *list, ec_slist_int_t *l2, size_t idx);
  ec_status_t (*add_first)(ec_slist_int_t *list, int e);
  ec_status_t (*add_last)(ec_slist_int_t *list, int e);
  ec_status_t (*remove)(ec_slist_int_t *list, int e, int *out);
  ec_status_t (*remove_first)(ec_slist_int_t *list, int *out);
  ec_status_t (*remove_last)(ec_slist_int_t *list, int *out);
  ec_status_t (*remove_at)(ec_slist_int_t *list, size_t idx, int *out);
  ec_status_t (*remove_all)(ec_slist_int_t *list);
  ec_status_t (*remove_all_cb)(ec_slist_int_t *list, void (*cb)(int));
  ec_status_t (*get_at)(ec_slist_int_t *list, size_t idx, int *out);
  ec_status_t (*get_first)(ec_slist_int_t *list, int *out);
  ec_status_t (*get_last)(ec_slist_int_t *list, int *out);
  ec_status_t (*sublist)(ec_slist_int_t *list, size_t from, size_t to, ec_slist_int_t **out);
  ec_status_t (*copy_shallow)(ec_slist_int_t *list, ec_slist_int_t **out);
  ec_status_t (*copy_deep)(ec_slist_int_t *list, int (*cp)(int), ec_slist_int_t **out);
  ec_status_t (*replace_at)(ec_slist_int_t *list, int e, size_t idx, int *old);
  size_t (*contains)(ec_slist_int_t *list, int e);
  size_t (*contains_value)(ec_slist_int_t *list, int e, int (*cmp)(const void *, const void *));
  ec_status_t (*index_of)(ec_slist_int_t *list, int e, int (*cmp)(const void *, const void *), size_t *out_idx);
  ec_status_t (*to_array)(ec_slist_int_t *list, int **arr, size_t *len);
  ec_status_t (*reverse)(ec_slist_int_t *list);
  ec_status_t (*sort)(ec_slist_int_t *list, int (*cmp)(const void *, const void *));
  ec_status_t (*sort_in_place)(ec_slist_int_t *list, int (*cmp)(const void *, const void *));
  size_t (*size_fn)(ec_slist_int_t *list);
  ec_status_t (*foreach)(ec_slist_int_t *list, void (*op)(int));
  ec_status_t (*reduce)(ec_slist_int_t *list, void (*fn)(int *, int *, int *), int *result);
  ec_status_t (*filter_mut)(ec_slist_int_t *list, bool (*p)(const int));
  ec_status_t (*filter)(ec_slist_int_t *list, bool (*p)(const int), ec_slist_int_t **out);
  ec_status_t (*iter_init)(ec_slist_int_t *list, size_t *i);
  ec_status_t (*iter_next)(ec_slist_int_t *list, size_t *i, int *out);
  ec_status_t (*iter_remove)(ec_slist_int_t *list, size_t *i, int *out);
  ec_status_t (*iter_add)(ec_slist_int_t *list, size_t *i, int e);
  ec_status_t (*iter_replace)(ec_slist_int_t *list, size_t i, int e, int *old);
  size_t (*iter_index)(size_t i);
  ec_status_t (*zip_iter_init)(ec_slist_int_t *l1, ec_slist_int_t *l2, size_t *i);
  ec_status_t (*zip_iter_next)(ec_slist_int_t *l1, ec_slist_int_t *l2, size_t *i, int *o1, int *o2);
  ec_status_t (*zip_iter_add)(ec_slist_int_t *l1, ec_slist_int_t *l2, size_t *i, int e1, int e2);
  ec_status_t (*zip_iter_remove)(ec_slist_int_t *l1, ec_slist_int_t *l2, size_t *i, int *o1, int *o2);
  ec_status_t (*zip_iter_replace)(ec_slist_int_t *l1, ec_slist_int_t *l2, size_t i, int e1, int e2, int *o1, int *o2);
  size_t (*zip_iter_index)(size_t i);
};
static inline void ec_slist_int_lock(ec_slist_int_t *l) { EC_MUTEX_LOCK(l->lock); }
static inline void ec_slist_int_unlock(ec_slist_int_t *l) { EC_MUTEX_UNLOCK(l->lock); }
static inline void ec_slist_int_conf_init(ec_slist_int_conf_t *conf)
{
  if (conf)
    conf->mempool = NULL;
}
static inline ec_status_t ec_slist_int_new_conf(const ec_slist_int_conf_t *c, ec_slist_int_t **out)
{
  if (!c || !out)
    return EC_ERR_INVALID_ARG;
  EC_CHECK_MEMPOOL(c->mempool);
  ec_slist_int_t *lst = (ec_slist_int_t *)(ec_slist_int_t *)EC_MALLOC(sizeof(ec_slist_int_t));
  if (!lst)
    return EC_ERR_ALLOC;
  lst->head = NULL;
  lst->tail = NULL;
  lst->size = 0;
  lst->mempool = c->mempool;
  EC_MUTEX_INIT(lst->lock);
  ec_slist_int_init_functions(lst);
  *out = lst;
  return EC_OK;
}
static inline ec_status_t ec_slist_int_new(ec_slist_int_t **out)
{
  if (!out)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_conf_t def;
  ec_slist_int_conf_init(&def);
  return ec_slist_int_new_conf(&def, out);
}
static inline ec_status_t ec_slist_int_destroy(ec_slist_int_t *l)
{
  if (!l)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  ec_slist_int_node_t *c = l->head;
  while (c)
  {
    ec_slist_int_node_t *n = c->next;
    EC_FREE(c);
    c = n;
  }
  l->head = NULL;
  l->tail = NULL;
  l->size = 0;
  ec_slist_int_unlock(l);
  EC_MUTEX_DESTROY(l->lock);
  EC_FREE(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_destroy_cb(ec_slist_int_t *l, void (*cb)(int))
{
  if (!l)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  ec_slist_int_node_t *c = l->head;
  while (c)
  {
    ec_slist_int_node_t *n = c->next;
    if (cb)
      cb(c->data);
    EC_FREE(c);
    c = n;
  }
  l->head = NULL;
  l->tail = NULL;
  l->size = 0;
  ec_slist_int_unlock(l);
  EC_MUTEX_DESTROY(l->lock);
  EC_FREE(l);
  return EC_OK;
}
static inline size_t ec_slist_int_struct_size(void) { return sizeof(ec_slist_int_t); }
static inline ec_slist_int_node_t *ec_slist_int_make_node(ec_slist_int_t *l, int d)
{
  ec_slist_int_node_t *nd = (ec_slist_int_node_t *)(ec_slist_int_node_t *)EC_MALLOC(sizeof(ec_slist_int_node_t));
  if (nd)
  {
    nd->data = d;
    nd->next = NULL;
  }
  return nd;
}
static inline ec_status_t ec_slist_int_splice(ec_slist_int_t *l1, ec_slist_int_t *l2)
{
  if (!l1 || !l2)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l1);
  ec_slist_int_lock(l2);
  if (!l2->head)
  {
    ec_slist_int_unlock(l2);
    ec_slist_int_unlock(l1);
    return EC_OK;
  }
  if (!l1->head)
  {
    l1->head = l2->head;
    l1->tail = l2->tail;
    l1->size = l2->size;
  }
  else
  {
    l1->tail->next = l2->head;
    l1->tail = l2->tail;
    l1->size += l2->size;
  }
  l2->head = NULL;
  l2->tail = NULL;
  l2->size = 0;
  ec_slist_int_unlock(l2);
  ec_slist_int_unlock(l1);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_splice_at(ec_slist_int_t *l1, ec_slist_int_t *l2, size_t idx)
{
  if (!l1 || !l2)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l1);
  ec_slist_int_lock(l2);
  if (idx > l1->size)
  {
    ec_slist_int_unlock(l2);
    ec_slist_int_unlock(l1);
    return EC_ERR_OUT_OF_BOUNDS;
  }
  if (!l2->head)
  {
    ec_slist_int_unlock(l2);
    ec_slist_int_unlock(l1);
    return EC_OK;
  }
  if (idx == 0)
  {
    l2->tail->next = l1->head;
    l1->head = l2->head;
    if (l1->size == 0)
      l1->tail = l2->tail;
    l1->size += l2->size;
  }
  else if (idx == l1->size)
  {
    l1->tail->next = l2->head;
    l1->tail = l2->tail;
    l1->size += l2->size;
  }
  else
  {
    ec_slist_int_node_t *p = l1->head;
    for (size_t i = 0; i < idx - 1; i++)
      p = p->next;
    l2->tail->next = p->next;
    p->next = l2->head;
    l1->size += l2->size;
  }
  l2->head = NULL;
  l2->tail = NULL;
  l2->size = 0;
  ec_slist_int_unlock(l2);
  ec_slist_int_unlock(l1);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_add(ec_slist_int_t *l, int e)
{
  if (!l)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  ec_slist_int_node_t *nd = ec_slist_int_make_node(l, e);
  if (!nd)
  {
    ec_slist_int_unlock(l);
    return EC_ERR_ALLOC;
  }
  if (!l->head)
  {
    l->head = nd;
    l->tail = nd;
  }
  else
  {
    l->tail->next = nd;
    l->tail = nd;
  }
  l->size++;
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_add_at(ec_slist_int_t *l, int e, size_t i)
{
  if (!l)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  if (i > l->size)
  {
    ec_slist_int_unlock(l);
    return EC_ERR_OUT_OF_BOUNDS;
  }
  ec_slist_int_node_t *nd = ec_slist_int_make_node(l, e);
  if (!nd)
  {
    ec_slist_int_unlock(l);
    return EC_ERR_ALLOC;
  }
  if (i == 0)
  {
    nd->next = l->head;
    l->head = nd;
    if (l->size == 0)
      l->tail = nd;
  }
  else if (i == l->size)
  {
    l->tail->next = nd;
    l->tail = nd;
  }
  else
  {
    ec_slist_int_node_t *p = l->head;
    for (size_t x = 0; x < i - 1; x++)
      p = p->next;
    nd->next = p->next;
    p->next = nd;
  }
  l->size++;
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_add_all(ec_slist_int_t *l1, ec_slist_int_t *l2)
{
  if (!l1 || !l2)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l1);
  ec_slist_int_lock(l2);
  ec_slist_int_node_t *c = l2->head;
  while (c)
  {
    ec_slist_int_node_t *n = ec_slist_int_make_node(l1, c->data);
    if (!n)
    {
      ec_slist_int_unlock(l2);
      ec_slist_int_unlock(l1);
      return EC_ERR_ALLOC;
    }
    if (!l1->head)
    {
      l1->head = n;
      l1->tail = n;
    }
    else
    {
      l1->tail->next = n;
      l1->tail = n;
    }
    l1->size++;
    c = c->next;
  }
  ec_slist_int_unlock(l2);
  ec_slist_int_unlock(l1);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_add_all_at(ec_slist_int_t *l, ec_slist_int_t *l2, size_t i)
{
  if (!l || !l2)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  ec_slist_int_lock(l2);
  if (i > l->size)
  {
    ec_slist_int_unlock(l2);
    ec_slist_int_unlock(l);
    return EC_ERR_OUT_OF_BOUNDS;
  }
  ec_slist_int_node_t *dh = NULL, *dt = NULL;
  ec_slist_int_node_t *c = l2->head;
  while (c)
  {
    ec_slist_int_node_t *n = ec_slist_int_make_node(l, c->data);
    if (!n)
    {
      ec_slist_int_node_t *xx = dh;
      while (xx)
      {
        ec_slist_int_node_t *nx = xx->next;
        EC_FREE(xx);
        xx = nx;
      }
      ec_slist_int_unlock(l2);
      ec_slist_int_unlock(l);
      return EC_ERR_ALLOC;
    }
    if (!dh)
    {
      dh = n;
      dt = n;
    }
    else
    {
      dt->next = n;
      dt = n;
    }
    c = c->next;
  }
  if (i == 0)
  {
    dt->next = l->head;
    l->head = dh;
    if (l->size == 0)
      l->tail = dt;
  }
  else if (i == l->size)
  {
    l->tail->next = dh;
    l->tail = dt;
  }
  else
  {
    ec_slist_int_node_t *p = l->head;
    for (size_t x = 0; x < i - 1; x++)
      p = p->next;
    dt->next = p->next;
    p->next = dh;
  }
  l->size += l2->size;
  ec_slist_int_unlock(l2);
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_add_first(ec_slist_int_t *l, int e) { return ec_slist_int_add_at(l, e, 0); }
static inline ec_status_t ec_slist_int_add_last(ec_slist_int_t *l, int e) { return ec_slist_int_add(l, e); }
static inline ec_status_t ec_slist_int_remove(ec_slist_int_t *l, int e, int *out)
{
  if (!l)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  ec_slist_int_node_t *c = l->head, *p = NULL;
  while (c)
  {
    if (c->data == e)
    {
      if (out)
        *out = c->data;
      if (!p)
      {
        l->head = c->next;
        if (c == l->tail)
          l->tail = NULL;
      }
      else
      {
        p->next = c->next;
        if (c == l->tail)
          l->tail = p;
      }
      l->size--;
      EC_FREE(c);
      ec_slist_int_unlock(l);
      return EC_OK;
    }
    p = c;
    c = c->next;
  }
  ec_slist_int_unlock(l);
  return EC_ERR_NOT_FOUND;
}
static inline ec_status_t ec_slist_int_remove_first(ec_slist_int_t *l, int *out)
{
  if (!l)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  if (!l->head)
  {
    ec_slist_int_unlock(l);
    return EC_ERR_OUT_OF_BOUNDS;
  }
  ec_slist_int_node_t *h = l->head;
  if (out)
    *out = h->data;
  l->head = h->next;
  if (h == l->tail)
    l->tail = NULL;
  l->size--;
  EC_FREE(h);
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_remove_last(ec_slist_int_t *l, int *out)
{
  if (!l)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  if (!l->head)
  {
    ec_slist_int_unlock(l);
    return EC_ERR_OUT_OF_BOUNDS;
  }
  if (l->head == l->tail)
  {
    if (out)
      *out = l->head->data;
    EC_FREE(l->head);
    l->head = NULL;
    l->tail = NULL;
    l->size = 0;
    ec_slist_int_unlock(l);
    return EC_OK;
  }
  ec_slist_int_node_t *c = l->head;
  while (c->next != l->tail)
    c = c->next;
  if (out)
    *out = l->tail->data;
  EC_FREE(l->tail);
  l->tail = c;
  c->next = NULL;
  l->size--;
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_remove_at(ec_slist_int_t *l, size_t i, int *out)
{
  if (!l)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  if (i >= l->size)
  {
    ec_slist_int_unlock(l);
    return EC_ERR_OUT_OF_BOUNDS;
  }
  ec_slist_int_node_t *c = l->head, *p = NULL;
  for (size_t x = 0; x < i; x++)
  {
    p = c;
    c = c->next;
  }
  if (out)
    *out = c->data;
  if (!p)
  {
    l->head = c->next;
    if (c == l->tail)
      l->tail = NULL;
  }
  else
  {
    p->next = c->next;
    if (c == l->tail)
      l->tail = p;
  }
  l->size--;
  EC_FREE(c);
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_remove_all(ec_slist_int_t *l)
{
  if (!l)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  ec_slist_int_node_t *c = l->head;
  while (c)
  {
    ec_slist_int_node_t *n = c->next;
    EC_FREE(c);
    c = n;
  }
  l->head = NULL;
  l->tail = NULL;
  l->size = 0;
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_remove_all_cb(ec_slist_int_t *l, void (*cb)(int))
{
  if (!l)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  ec_slist_int_node_t *c = l->head;
  while (c)
  {
    ec_slist_int_node_t *n = c->next;
    if (cb)
      cb(c->data);
    EC_FREE(c);
    c = n;
  }
  l->head = NULL;
  l->tail = NULL;
  l->size = 0;
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_get_at(ec_slist_int_t *l, size_t i, int *o)
{
  if (!l || !o)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  if (i >= l->size)
  {
    ec_slist_int_unlock(l);
    return EC_ERR_OUT_OF_BOUNDS;
  }
  ec_slist_int_node_t *c = l->head;
  for (size_t x = 0; x < i; x++)
    c = c->next;
  *o = c->data;
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_get_first(ec_slist_int_t *l, int *o)
{
  if (!l || !o)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  if (!l->head)
  {
    ec_slist_int_unlock(l);
    return EC_ERR_OUT_OF_BOUNDS;
  }
  *o = l->head->data;
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_get_last(ec_slist_int_t *l, int *o)
{
  if (!l || !o)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  if (!l->tail)
  {
    ec_slist_int_unlock(l);
    return EC_ERR_OUT_OF_BOUNDS;
  }
  *o = l->tail->data;
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_sublist(ec_slist_int_t *l, size_t f, size_t t, ec_slist_int_t **out)
{
  if (!l || !out || f > t)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  if (t > l->size)
  {
    ec_slist_int_unlock(l);
    return EC_ERR_OUT_OF_BOUNDS;
  }
  ec_slist_int_conf_t cf;
  cf.mempool = l->mempool;
  ec_slist_int_unlock(l);
  ec_status_t st = ec_slist_int_new_conf(&cf, out);
  if (st != EC_OK)
    return st;
  ec_slist_int_lock(l);
  ec_slist_int_t *r = *out;
  ec_slist_int_lock(r);
  ec_slist_int_node_t *c = l->head;
  for (size_t i = 0; i < f; i++)
    c = c->next;
  for (size_t i = f; i < t; i++)
  {
    ec_slist_int_node_t *n = ec_slist_int_make_node(r, c->data);
    if (!n)
    {
      ec_slist_int_unlock(r);
      ec_slist_int_unlock(l);
      ec_slist_int_destroy(r);
      *out = NULL;
      return EC_ERR_ALLOC;
    }
    if (!r->head)
    {
      r->head = n;
      r->tail = n;
    }
    else
    {
      r->tail->next = n;
      r->tail = n;
    }
    r->size++;
    c = c->next;
  }
  ec_slist_int_unlock(r);
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_copy_shallow(ec_slist_int_t *l, ec_slist_int_t **out)
{
  if (!l || !out)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  ec_slist_int_conf_t cf;
  cf.mempool = l->mempool;
  ec_slist_int_unlock(l);
  ec_status_t st = ec_slist_int_new_conf(&cf, out);
  if (st != EC_OK)
    return st;
  ec_slist_int_lock(l);
  ec_slist_int_t *r = *out;
  ec_slist_int_lock(r);
  ec_slist_int_node_t *c = l->head;
  while (c)
  {
    ec_slist_int_node_t *n = ec_slist_int_make_node(r, c->data);
    if (!n)
    {
      ec_slist_int_unlock(r);
      ec_slist_int_unlock(l);
      ec_slist_int_destroy(r);
      *out = NULL;
      return EC_ERR_ALLOC;
    }
    if (!r->head)
    {
      r->head = n;
      r->tail = n;
    }
    else
    {
      r->tail->next = n;
      r->tail = n;
    }
    r->size++;
    c = c->next;
  }
  ec_slist_int_unlock(r);
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_copy_deep(ec_slist_int_t *l, int (*cp)(int), ec_slist_int_t **out)
{
  if (!l || !cp || !out)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  ec_slist_int_conf_t cf;
  cf.mempool = l->mempool;
  ec_slist_int_unlock(l);
  ec_status_t st = ec_slist_int_new_conf(&cf, out);
  if (st != EC_OK)
    return st;
  ec_slist_int_lock(l);
  ec_slist_int_t *r = *out;
  ec_slist_int_lock(r);
  ec_slist_int_node_t *c = l->head;
  while (c)
  {
    int nd = cp(c->data);
    ec_slist_int_node_t *n = ec_slist_int_make_node(r, nd);
    if (!n)
    {
      ec_slist_int_unlock(r);
      ec_slist_int_unlock(l);
      ec_slist_int_destroy(r);
      *out = NULL;
      return EC_ERR_ALLOC;
    }
    if (!r->head)
    {
      r->head = n;
      r->tail = n;
    }
    else
    {
      r->tail->next = n;
      r->tail = n;
    }
    r->size++;
    c = c->next;
  }
  ec_slist_int_unlock(r);
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_replace_at(ec_slist_int_t *l, int e, size_t i, int *old)
{
  if (!l)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  if (i >= l->size)
  {
    ec_slist_int_unlock(l);
    return EC_ERR_OUT_OF_BOUNDS;
  }
  ec_slist_int_node_t *c = l->head;
  for (size_t x = 0; x < i; x++)
    c = c->next;
  if (old)
    *old = c->data;
  c->data = e;
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline size_t ec_slist_int_contains(ec_slist_int_t *l, int e)
{
  if (!l)
    return 0;
  ec_slist_int_lock(l);
  ec_slist_int_node_t *c = l->head;
  while (c)
  {
    if (c->data == e)
    {
      ec_slist_int_unlock(l);
      return 1;
    }
    c = c->next;
  }
  ec_slist_int_unlock(l);
  return 0;
}
static inline size_t ec_slist_int_contains_value(ec_slist_int_t *l, int e, int (*cmp)(const void *, const void *))
{
  if (!l || !cmp)
    return 0;
  ec_slist_int_lock(l);
  ec_slist_int_node_t *c = l->head;
  while (c)
  {
    if (cmp(&c->data, &e) == 0)
    {
      ec_slist_int_unlock(l);
      return 1;
    }
    c = c->next;
  }
  ec_slist_int_unlock(l);
  return 0;
}
static inline ec_status_t ec_slist_int_index_of(ec_slist_int_t *l, int e, int (*cmp)(const void *, const void *), size_t *o)
{
  if (!l || !cmp || !o)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  size_t i = 0;
  ec_slist_int_node_t *c = l->head;
  while (c)
  {
    if (cmp(&c->data, &e) == 0)
    {
      *o = i;
      ec_slist_int_unlock(l);
      return EC_OK;
    }
    i++;
    c = c->next;
  }
  ec_slist_int_unlock(l);
  return EC_ERR_NOT_FOUND;
}
static inline ec_status_t ec_slist_int_to_array(ec_slist_int_t *l, int **arr, size_t *len)
{
  if (!l || !arr || !len)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  *len = l->size;
  int *tmp = (int *)EC_MALLOC((l->size) * sizeof(int));
  if (!tmp)
  {
    ec_slist_int_unlock(l);
    return EC_ERR_ALLOC;
  }
  ec_slist_int_node_t *c = l->head;
  size_t i = 0;
  while (c)
  {
    tmp[i++] = c->data;
    c = c->next;
  }
  ec_slist_int_unlock(l);
  *arr = tmp;
  return EC_OK;
}
static inline ec_status_t ec_slist_int_reverse(ec_slist_int_t *l)
{
  if (!l)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  ec_slist_int_node_t *p = NULL, *c = l->head;
  l->tail = l->head;
  while (c)
  {
    ec_slist_int_node_t *n = c->next;
    c->next = p;
    p = c;
    c = n;
  }
  l->head = p;
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_sort_in_place(ec_slist_int_t *l, int (*cmp)(const void *, const void *))
{
  if (!l || !cmp)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  if (l->size < 2)
  {
    ec_slist_int_unlock(l);
    return EC_OK;
  }
  size_t n = l->size;
  int *tmp = (int *)EC_MALLOC((n) * sizeof(int));
  if (!tmp)
  {
    ec_slist_int_unlock(l);
    return EC_ERR_ALLOC;
  }
  ec_slist_int_node_t *c = l->head;
  for (size_t i = 0; i < n; i++)
  {
    tmp[i] = c->data;
    c = c->next;
  }
  qsort(tmp, n, sizeof(int), cmp);
  c = l->head;
  for (size_t i = 0; i < n; i++)
  {
    c->data = tmp[i];
    c = c->next;
  }
  EC_FREE(tmp);
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_sort(ec_slist_int_t *l, int (*cmp)(const void *, const void *)) { return ec_slist_int_sort_in_place(l, cmp); }
static inline size_t ec_slist_int_size(ec_slist_int_t *l)
{
  if (!l)
    return 0;
  ec_slist_int_lock(l);
  size_t s = l->size;
  ec_slist_int_unlock(l);
  return s;
}
static inline ec_status_t ec_slist_int_foreach(ec_slist_int_t *l, void (*op)(int))
{
  if (!l || !op)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  ec_slist_int_node_t *c = l->head;
  while (c)
  {
    op(c->data);
    c = c->next;
  }
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_reduce(ec_slist_int_t *l, void (*fn)(int *, int *, int *), int *res)
{
  if (!l || !fn || !res)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  if (!l->size)
  {
    ec_slist_int_unlock(l);
    return EC_OK;
  }
  ec_slist_int_node_t *c = l->head;
  *res = c->data;
  c = c->next;
  while (c)
  {
    fn(res, &c->data, res);
    c = c->next;
  }
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_filter_mut(ec_slist_int_t *l, bool (*p)(const int))
{
  if (!l || !p)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  ec_slist_int_node_t *c = l->head, *pr = NULL;
  while (c)
  {
    if (!p(c->data))
    {
      ec_slist_int_node_t *n = c->next;
      if (!pr)
      {
        l->head = n;
        if (c == l->tail)
          l->tail = NULL;
      }
      else
      {
        pr->next = n;
        if (c == l->tail)
          l->tail = pr;
      }
      EC_FREE(c);
      l->size--;
      c = n;
    }
    else
    {
      pr = c;
      c = c->next;
    }
  }
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_filter(ec_slist_int_t *l, bool (*p)(const int), ec_slist_int_t **out)
{
  if (!l || !p || !out)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  ec_slist_int_conf_t cf;
  cf.mempool = l->mempool;
  ec_slist_int_unlock(l);
  ec_status_t st = ec_slist_int_new_conf(&cf, out);
  if (st != EC_OK)
    return st;
  ec_slist_int_lock(l);
  ec_slist_int_t *r = *out;
  ec_slist_int_lock(r);
  ec_slist_int_node_t *c = l->head;
  while (c)
  {
    if (p(c->data))
    {
      ec_slist_int_node_t *n = ec_slist_int_make_node(r, c->data);
      if (!n)
      {
        ec_slist_int_unlock(r);
        ec_slist_int_unlock(l);
        ec_slist_int_destroy(r);
        *out = NULL;
        return EC_ERR_ALLOC;
      }
      if (!r->head)
      {
        r->head = n;
        r->tail = n;
      }
      else
      {
        r->tail->next = n;
        r->tail = n;
      }
      r->size++;
    }
    c = c->next;
  }
  ec_slist_int_unlock(r);
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_iter_init(ec_slist_int_t *l, size_t *i)
{
  if (!l || !i)
    return EC_ERR_INVALID_ARG;
  *i = 0;
  return EC_OK;
}
static inline ec_status_t ec_slist_int_iter_next(ec_slist_int_t *l, size_t *i, int *out)
{
  if (!l || !i || !out)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l);
  if (*i >= l->size)
  {
    ec_slist_int_unlock(l);
    return EC_ITER_END;
  }
  ec_slist_int_node_t *c = l->head;
  for (size_t x = 0; x < *i; x++)
    c = c->next;
  *out = c->data;
  (*i)++;
  ec_slist_int_unlock(l);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_iter_remove(ec_slist_int_t *l, size_t *i, int *out)
{
  if (!l || !i)
    return EC_ERR_INVALID_ARG;
  if (*i == 0)
    return EC_ERR_INVALID_OP;
  size_t ri = (*i) - 1;
  ec_status_t st = ec_slist_int_remove_at(l, ri, out);
  if (st == EC_OK)
    (*i)--;
  return st;
}
static inline ec_status_t ec_slist_int_iter_add(ec_slist_int_t *l, size_t *i, int e)
{
  if (!l || !i)
    return EC_ERR_INVALID_ARG;
  ec_status_t st = ec_slist_int_add_at(l, e, *i);
  if (st == EC_OK)
    (*i)++;
  return st;
}
static inline ec_status_t ec_slist_int_iter_replace(ec_slist_int_t *l, size_t ii, int e, int *o)
{
  if (!l)
    return EC_ERR_INVALID_ARG;
  if (ii == 0)
    return EC_ERR_INVALID_OP;
  size_t ri = ii - 1;
  return ec_slist_int_replace_at(l, e, ri, o);
}
static inline size_t ec_slist_int_iter_index(size_t i) { return i; }
static inline ec_status_t ec_slist_int_zip_iter_init(ec_slist_int_t *l1, ec_slist_int_t *l2, size_t *i)
{
  if (!l1 || !l2 || !i)
    return EC_ERR_INVALID_ARG;
  *i = 0;
  return EC_OK;
}
static inline ec_status_t ec_slist_int_zip_iter_next(ec_slist_int_t *l1, ec_slist_int_t *l2, size_t *i, int *o1, int *o2)
{
  if (!l1 || !l2 || !i || !o1 || !o2)
    return EC_ERR_INVALID_ARG;
  ec_slist_int_lock(l1);
  ec_slist_int_lock(l2);
  if (*i >= l1->size || *i >= l2->size)
  {
    ec_slist_int_unlock(l2);
    ec_slist_int_unlock(l1);
    return EC_ITER_END;
  }
  ec_slist_int_node_t *c1 = l1->head;
  for (size_t x = 0; x < *i; x++)
    c1 = c1->next;
  ec_slist_int_node_t *c2 = l2->head;
  for (size_t x = 0; x < *i; x++)
    c2 = c2->next;
  *o1 = c1->data;
  *o2 = c2->data;
  (*i)++;
  ec_slist_int_unlock(l2);
  ec_slist_int_unlock(l1);
  return EC_OK;
}
static inline ec_status_t ec_slist_int_zip_iter_add(ec_slist_int_t *l1, ec_slist_int_t *l2, size_t *i, int e1, int e2)
{
  if (!l1 || !l2 || !i)
    return EC_ERR_INVALID_ARG;
  ec_status_t s1 = ec_slist_int_add_at(l1, e1, *i);
  ec_status_t s2 = ec_slist_int_add_at(l2, e2, *i);
  if (s1 == EC_OK && s2 == EC_OK)
  {
    (*i)++;
    return EC_OK;
  }
  return EC_ERR_ALLOC;
}
static inline ec_status_t ec_slist_int_zip_iter_remove(ec_slist_int_t *l1, ec_slist_int_t *l2, size_t *i, int *o1, int *o2)
{
  if (!l1 || !l2 || !i)
    return EC_ERR_INVALID_ARG;
  if (*i == 0)
    return EC_ERR_INVALID_OP;
  size_t ri = (*i) - 1;
  ec_status_t s1 = ec_slist_int_remove_at(l1, ri, o1);
  ec_status_t s2 = ec_slist_int_remove_at(l2, ri, o2);
  if (s1 == EC_OK && s2 == EC_OK)
  {
    (*i)--;
    return EC_OK;
  }
  return (s1 == EC_OK) ? s2 : s1;
}
static inline ec_status_t ec_slist_int_zip_iter_replace(ec_slist_int_t *l1, ec_slist_int_t *l2, size_t i, int e1, int e2, int *o1, int *o2)
{
  if (!l1 || !l2)
    return EC_ERR_INVALID_ARG;
  if (i == 0)
    return EC_ERR_INVALID_OP;
  size_t ri = i - 1;
  ec_status_t s1 = ec_slist_int_replace_at(l1, e1, ri, o1);
  ec_status_t s2 = ec_slist_int_replace_at(l2, e2, ri, o2);
  if (s1 == EC_OK && s2 == EC_OK)
    return EC_OK;
  return (s1 == EC_OK) ? s2 : s1;
}
static inline size_t ec_slist_int_zip_iter_index(size_t i) { return i; }
static inline void ec_slist_int_init_functions(ec_slist_int_t *l)
{
  do
  {
    \ l->conf_init = ec_slist_int_conf_init;
    l->new = ec_slist_int_new;
    l->new_conf = ec_slist_int_new_conf;
    l->destroy = ec_slist_int_destroy;
    l->destroy_cb = ec_slist_int_destroy_cb;
    l->struct_size = ec_slist_int_struct_size;
    l->splice = ec_slist_int_splice;
    l->splice_at = ec_slist_int_splice_at;
    l->add = ec_slist_int_add;
    l->add_at = ec_slist_int_add_at;
    l->add_all = ec_slist_int_add_all;
    l->add_all_at = ec_slist_int_add_all_at;
    l->add_first = ec_slist_int_add_first;
    l->add_last = ec_slist_int_add_last;
    l->remove = ec_slist_int_remove;
    l->remove_first = ec_slist_int_remove_first;
    l->remove_last = ec_slist_int_remove_last;
    l->remove_at = ec_slist_int_remove_at;
    l->remove_all = ec_slist_int_remove_all;
    l->remove_all_cb = ec_slist_int_remove_all_cb;
    l->get_at = ec_slist_int_get_at;
    l->get_first = ec_slist_int_get_first;
    l->get_last = ec_slist_int_get_last;
    l->sublist = ec_slist_int_sublist;
    l->copy_shallow = ec_slist_int_copy_shallow;
    l->copy_deep = ec_slist_int_copy_deep;
    l->replace_at = ec_slist_int_replace_at;
    l->contains = ec_slist_int_contains;
    l->contains_value = ec_slist_int_contains_value;
    l->index_of = ec_slist_int_index_of;
    l->to_array = ec_slist_int_to_array;
    l->reverse = ec_slist_int_reverse;
    l->sort = ec_slist_int_sort;
    l->sort_in_place = ec_slist_int_sort_in_place;
    l->size_fn = ec_slist_int_size;
    l->foreach = ec_slist_int_foreach;
    l->reduce = ec_slist_int_reduce;
    l->filter_mut = ec_slist_int_filter_mut;
    l->filter = ec_slist_int_filter;
    l->iter_init = ec_slist_int_iter_init;
    l->iter_next = ec_slist_int_iter_next;
    l->iter_remove = ec_slist_int_iter_remove;
    l->iter_add = ec_slist_int_iter_add;
    l->iter_replace = ec_slist_int_iter_replace;
    l->iter_index = ec_slist_int_iter_index;
    l->zip_iter_init = ec_slist_int_zip_iter_init;
    l->zip_iter_next = ec_slist_int_zip_iter_next;
    l->zip_iter_add = ec_slist_int_zip_iter_add;
    l->zip_iter_remove = ec_slist_int_zip_iter_remove;
    l->zip_iter_replace = ec_slist_int_zip_iter_replace;
    l->zip_iter_index = ec_slist_int_zip_iter_index;
  } while (0);
}
