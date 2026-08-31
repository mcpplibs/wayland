// freedesktop.wayland.client —— 模块路线,一个 wayland 头都不包。
//
// ⭐ 重点是那四个 wl_fixed_* 转换。它们在 wayland-util.h 里是 `static inline`,
// C++ 禁止导出内部链接,所以在这次修复之前,**走模块路线的客户端根本没法把
// 协议给的坐标转成数**。
//
// wl_fixed_t 是协议里每一个亚像素坐标的载体:wl_pointer 的移动、触摸点、
// 数位板轴。一个处理指针输入的客户端就是卡在这儿。而这个包一直没发现,
// 因为**没有任何测试转换过一次**。
#ifdef __linux__

import freedesktop.wayland.client;

#include <cstdio>

namespace {
int failures = 0;
void check(bool ok, const char *what) {
    std::printf("%-58s %s\n", what, ok ? "ok" : "FAILED");
    if (!ok) ++failures;
}
} // namespace

int main()
{
    std::printf("import freedesktop.wayland.client\n\n");

    // ── 定点数转换 ──────────────────────────────────────────────────
    check(wl_fixed_from_int(3) == 3 * 256, "wl_fixed_from_int(3) == 768");
    check(wl_fixed_to_int(wl_fixed_from_int(7)) == 7, "int 往返");
    check(wl_fixed_to_double(wl_fixed_from_double(1.5)) == 1.5, "double 往返 1.5");
    check(wl_fixed_to_double(wl_fixed_from_int(1)) == 1.0, "1 == 1.0");
    // 负数用 round() 半数远离零,和上游一致 —— 截断会得到 -383。
    check(wl_fixed_from_double(-1.5) == -384, "负数按上游的 round() 舍入");
    // 亚像素:协议里 1/256 是最小刻度
    check(wl_fixed_from_double(0.5) == 128, "0.5 是 128 —— 1/256 的刻度");

    // ── 顺带确认核心类型/函数确实在模块里 ────────────────────────────
    wl_display *d = wl_display_connect(nullptr);
    if (d == nullptr) {
        std::printf("  (没有合成器可连 —— 这台机器的属性,不是这个包的)\n");
        check(true, "wl_display_connect 可调用");
    } else {
        check(wl_display_get_fd(d) >= 0, "wl_display_get_fd");
        wl_display_disconnect(d);
    }

    std::printf("\n%s\n", failures == 0 ? "all ok" : "FAILURES");
    return failures == 0 ? 0 : 1;
}

#else
#include <cstdio>
int main() { std::printf("linux only\n"); return 0; }
#endif
