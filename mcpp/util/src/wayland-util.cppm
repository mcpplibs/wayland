// freedesktop.wayland.util — wayland's public MACROS, as things a module can export.
//
// `export` names entities, and a macro is not one, so the fourteen macros in
// wayland's public headers cannot come through `import freedesktop.wayland.client;`. This
// module carries them instead — as a constant, a function template and a set
// of ranges, which is what each macro actually is once the preprocessor is out
// of the way.
//
// This is the one place the C API's SPELLING changes, and it changes because
// C++ has no way to keep it. Everything else in freedesktop.wayland.client / freedesktop.wayland.server
// is upstream's name, unchanged.
//
//     macro                              here
//     ---------------------------------  -------------------------------------
//     WL_MARSHAL_FLAG_DESTROY            WL_MARSHAL_FLAG_DESTROY   (identical)
//     wl_container_of(p, s, member)      wl_container_of<&T::member>(p)
//     wl_list_for_each(p, head, member)  for (T *p : wl_list_each<&T::member>(head))
//     wl_list_for_each_safe(...)         wl_list_each_safe<&T::member>(head)
//     wl_list_for_each_reverse(...)      wl_list_each_reverse<&T::member>(head)
//     wl_array_for_each(p, array)        for (T *p : wl_array_each<T>(array))
//
// The `_safe` variants keep upstream's guarantee: the next element is latched
// before the body runs, so the current one may be removed or freed.
module;

#include <wayland-util.h>

#include <cstddef>
#include <cstdint>

// The header defines these as macros; this module provides them as entities of
// the same name, so the macros have to go first — a macro would otherwise eat
// the declaration below it. Undefining here, inside the global module
// fragment, is local to this translation unit and does not reach consumers.
#undef wl_container_of
#undef wl_list_for_each
#undef wl_list_for_each_safe
#undef wl_list_for_each_reverse
#undef wl_list_for_each_reverse_safe
#undef wl_array_for_each
#undef WL_MARSHAL_FLAG_DESTROY

export module freedesktop.wayland.util;

export {

// WL_MARSHAL_FLAG_DESTROY — a plain constant, so the spelling survives intact.
inline constexpr std::uint32_t WL_MARSHAL_FLAG_DESTROY = 1u << 0;

// wl_container_of(ptr, sample, member): recover the containing object from a
// pointer to one of its members. The macro takes a sample POINTER only to read
// its type off it; a pointer-to-member says the same thing and is checked.
template <auto Member, class M>
constexpr auto wl_container_of(M *ptr) noexcept
{
    using T = decltype([]<class C, class V>(V C::*) -> C { return {}; }(Member));
    return reinterpret_cast<T *>(reinterpret_cast<char *>(ptr)
                                 - reinterpret_cast<std::ptrdiff_t>(
                                       &(static_cast<T *>(nullptr)->*Member)));
}

// wl_list_for_each and its variants. `Member` is the wl_list field the
// elements are linked through, exactly what the macro's third argument names.
template <auto Member, bool Reverse = false, bool Safe = false>
class wl_list_each_view {
    wl_list *head_;

    using T = decltype([]<class C, class V>(V C::*) -> C { return {}; }(Member));

    static T *from(wl_list *link) noexcept { return wl_container_of<Member>(link); }
    static wl_list *step(wl_list *l) noexcept { return Reverse ? l->prev : l->next; }

public:
    explicit wl_list_each_view(wl_list *head) noexcept : head_(head) {}

    class iterator {
        wl_list *head_, *cur_, *next_;
    public:
        iterator(wl_list *head, wl_list *cur) noexcept
            : head_(head), cur_(cur), next_(cur ? step(cur) : nullptr) {}
        T *operator*() const noexcept { return from(cur_); }
        iterator &operator++() noexcept
        {
            // Safe mode latches the successor before the body ran, so the
            // element just visited may have been removed or freed.
            cur_  = Safe ? next_ : step(cur_);
            next_ = step(cur_);
            return *this;
        }
        bool operator!=(std::nullptr_t) const noexcept { return cur_ != head_; }
    };

    iterator begin() const noexcept { return iterator(head_, step(head_)); }
    std::nullptr_t end() const noexcept { return nullptr; }
};

template <auto Member> auto wl_list_each(wl_list *head) noexcept
{ return wl_list_each_view<Member, false, false>(head); }

template <auto Member> auto wl_list_each_safe(wl_list *head) noexcept
{ return wl_list_each_view<Member, false, true>(head); }

template <auto Member> auto wl_list_each_reverse(wl_list *head) noexcept
{ return wl_list_each_view<Member, true, false>(head); }

template <auto Member> auto wl_list_each_reverse_safe(wl_list *head) noexcept
{ return wl_list_each_view<Member, true, true>(head); }

// wl_array_for_each(pos, array): a wl_array is a flat buffer of T, so this is
// just the range over it. Upstream's macro stops at `size`, and so does this.
template <class T>
class wl_array_each_view {
    wl_array *a_;
public:
    explicit wl_array_each_view(wl_array *a) noexcept : a_(a) {}

    // Yields T* rather than T&, because upstream's `pos` is a pointer:
    //     wl_array_for_each(pos, array)   ->   for (T *pos : wl_array_each<T>(array))
    class iterator {
        T *p_;
    public:
        explicit iterator(T *p) noexcept : p_(p) {}
        T *operator*() const noexcept { return p_; }
        iterator &operator++() noexcept { ++p_; return *this; }
        bool operator!=(const iterator &o) const noexcept { return p_ != o.p_; }
    };

    iterator begin() const noexcept { return iterator(static_cast<T *>(a_->data)); }
    iterator end() const noexcept
    {
        return iterator(reinterpret_cast<T *>(static_cast<char *>(a_->data) + a_->size));
    }
};

template <class T> auto wl_array_each(wl_array *a) noexcept
{ return wl_array_each_view<T>(a); }

} // export
