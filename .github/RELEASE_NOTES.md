> [!WARNING]
> This firmware is unsigned. Sign it yourself before installation; you can use **[PicoForge All](https://github.com/BlueFunny19/picoforge-all) → Firmware** with your local signing key. Devices with Secure Boot enabled require their original trusted key.
> Running Nuke permanently erases external Flash, including firmware, credentials, PINs and settings. Prepare a signed Pico All recovery image first; OTP and hardware locks are preserved.

Updates:

- Added a dedicated Waveshare RP2350-One build with its onboard RGB status light
- Red breathing indicates pre-load confirmation; steady red continues through erasure, verification and the return to BOOTSEL
- Verifies the erased Flash and the final picotool marker; errors use fast red blinking
- Runs from RAM and leaves OTP, Secure Boot and Secure Lock unchanged
- Release images are unsigned; automatic build-time signing has been removed

---

> [!WARNING]
> 此固件未签名，需要自行签名后再安装；可使用 **[PicoForge All](https://github.com/BlueFunny19/picoforge-all) → 固件**，选择本地签名密钥进行签名；已开启安全启动的设备必须使用原来的受信任密钥
> 运行 Nuke 会永久擦除外部 Flash，包括固件、凭据、PIN 和设置；请先准备好已签名的 Pico All 恢复固件；OTP 和硬件锁定会保留

更新内容：

- 新增 Waveshare RP2350-One 专用构建，支持板载 RGB 状态灯
- 加载前以红色呼吸提示确认；擦除、校验及完成后返回 BOOTSEL 时保持红色常亮
- 校验 Flash 擦除结果和 picotool 标记，出错时以红色快闪提示
- 在 RAM 中运行，保留 OTP、安全启动和安全锁定
- 发布文件统一为未签名固件，移除构建流程中的自动签名
