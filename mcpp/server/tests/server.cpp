// freedesktop.wayland.server —— 模块路线,一个头都不包。
//
// ⭐ 这个文件存在的理由是一个已发布的缺口:模块导出了 154 个名字,而
// `wayland-server-core.h` 里的四个 `static inline`(wl_signal_init / _add /
// _get / _emit)一个都没导出。
//
// 没人发现,是因为这个包的测试从没点过它们中的任何一个 —— 而它们不是边角:
// 每一次 wlroots 监听注册都是 `wl_signal_add(&thing->events.x, &listener)`。
// 缺了它们,**走模块路线写不了合成器**。这个缺口是被外面的一个最小 wlroots
// 合成器问出来的,不是被这里问出来的。
#ifdef __linux__

import freedesktop.wayland.server;

#include <cstdio>

namespace {
int failures = 0;
void check(bool ok, const char *what) {
    std::printf("%-58s %s\n", what, ok ? "ok" : "FAILED");
    if (!ok) ++failures;
}

int hits = 0;
void on_notify(wl_listener *, void *data) { hits += *static_cast<int *>(data); }
} // namespace

int main()
{
    std::printf("import freedesktop.wayland.server\n\n");

    // ── 一个真正的 signal/listener 循环,全部走模块 ──────────────────
    wl_signal sig{};
    wl_signal_init(&sig);
    check(sig.listener_list.next == &sig.listener_list, "wl_signal_init 建出空表");

    wl_listener l{};
    l.notify = on_notify;
    wl_signal_add(&sig, &l);
    check(sig.listener_list.next == &l.link, "wl_signal_add 把监听挂了上去");

    check(wl_signal_get(&sig, on_notify) == &l, "wl_signal_get 按回调找回监听");
    check(wl_signal_get(&sig, nullptr) == nullptr, "…找不到时返回 nullptr");

    int v = 41;
    wl_signal_emit(&sig, &v);
    v = 1;
    wl_signal_emit(&sig, &v);
    check(hits == 42, "wl_signal_emit 两次都调到了回调(41+1)");

    wl_list_remove(&l.link);
    check(wl_signal_get(&sig, on_notify) == nullptr, "摘掉之后就找不到了");

    // ── 顺带确认服务端的核心类型/函数确实在模块里 ────────────────────
    wl_display *d = wl_display_create();
    check(d != nullptr, "wl_display_create");
    if (d) {
        check(wl_display_get_event_loop(d) != nullptr, "wl_display_get_event_loop");
        wl_display_destroy(d);
    }

    std::printf("\n%s\n", failures == 0 ? "all ok" : "FAILURES");
    return failures == 0 ? 0 : 1;
}

#else
#include <cstdio>
int main() { std::printf("linux only\n"); return 0; }
#endif
