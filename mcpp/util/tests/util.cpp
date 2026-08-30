// The macro mappings, instantiated. A template that only compiles proves
// nothing — these run the same traversals upstream's macros do and check the
// order and the removal guarantee.
//
// The list and array are wired up BY HAND rather than with wl_list_insert() and
// friends, deliberately: this package links no library (it is templates and a
// constant), and a test that pulled in libwayland-client to get four
// three-line functions would give it a dependency it does not have.

#include <wayland-util.h>

#include <cstdio>
#include <string>
#include <vector>

import freedesktop.wayland.util;

namespace {

int failures = 0;

void check(bool ok, const char *what)
{
    std::printf("%-58s %s\n", what, ok ? "ok" : "FAILED");
    if (!ok) ++failures;
}

struct node {
    int value;
    wl_list link;
};

// wl_list is a circular doubly-linked list with the head as a sentinel; these
// are upstream's own list.c, inlined so the test stays dependency-free.
void list_init(wl_list *l) noexcept { l->prev = l; l->next = l; }

void list_append(wl_list *head, wl_list *elm) noexcept
{
    wl_list *prev = head->prev;
    elm->prev = prev; elm->next = head;
    prev->next = elm;  head->prev = elm;
}

void list_remove(wl_list *elm) noexcept
{
    elm->prev->next = elm->next;
    elm->next->prev = elm->prev;
    elm->prev = nullptr; elm->next = nullptr;
}

bool list_empty(const wl_list *l) noexcept { return l->next == l; }

std::string joined(const std::vector<int> &v)
{
    std::string s;
    for (int x : v) { if (!s.empty()) s += ","; s += std::to_string(x); }
    return s;
}

} // namespace

int main()
{
    wl_list head;
    list_init(&head);

    node a{1, {}}, b{2, {}}, c{3, {}};
    list_append(&head, &a.link);
    list_append(&head, &b.link);
    list_append(&head, &c.link);

    // wl_container_of: recover the node from a pointer to its link member.
    check(wl_container_of<&node::link>(&b.link) == &b,
          "wl_container_of recovers the containing object");

    // wl_list_for_each
    {
        std::vector<int> seen;
        for (node *n : wl_list_each<&node::link>(&head)) seen.push_back(n->value);
        check(joined(seen) == "1,2,3", "wl_list_each visits 1,2,3 in order");
    }

    // wl_list_for_each_reverse
    {
        std::vector<int> seen;
        for (node *n : wl_list_each_reverse<&node::link>(&head)) seen.push_back(n->value);
        check(joined(seen) == "3,2,1", "wl_list_each_reverse visits 3,2,1");
    }

    // wl_list_for_each_safe: the current element may be removed inside the body.
    // This is the whole reason the _safe variant exists, so the test removes.
    {
        std::vector<int> seen;
        for (node *n : wl_list_each_safe<&node::link>(&head)) {
            seen.push_back(n->value);
            list_remove(&n->link);
        }
        check(joined(seen) == "1,2,3", "wl_list_each_safe survives removal mid-loop");
        check(list_empty(&head), "the list is empty afterwards");
    }

    // wl_array_for_each
    {
        int storage[3] = {10, 11, 12};
        wl_array arr{};
        arr.data = storage;
        arr.size = sizeof(storage);
        arr.alloc = sizeof(storage);
        std::vector<int> seen;
        for (int *p : wl_array_each<int>(&arr)) seen.push_back(*p);
        check(joined(seen) == "10,11,12", "wl_array_each walks the array");
    }

    check(WL_MARSHAL_FLAG_DESTROY == 1u, "WL_MARSHAL_FLAG_DESTROY keeps its value");

    std::printf("\n%d check(s) failed\n", failures);
    return failures == 0 ? 0 : 1;
}
