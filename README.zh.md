# Pico Nuke All

[English](README.md) | 中文

擦除 **Waveshare RP2350-One** 的外部 Flash，并返回 BOOTSEL 模式，供重新安装固件

## 功能

- 自动识别 Flash 容量，并校验擦除结果
- 在 RAM 中运行，操作期间保持红色状态灯
- 保留 OTP、安全启动和安全锁定设置
- 支持 PicoForge All 的固件签名与更新流程

## 下载与使用

从 [Releases](https://github.com/XiaoNetwork-Astral/pico-nuke-all/releases/latest) 下载 `pico_nuke_all-1.7-unsigned.uf2`

> [!WARNING]
> 发布的固件未签名，需要自行签名后再安装；可使用 **[PicoForge All](https://github.com/XiaoNetwork-Astral/pico-forge-all) → 固件**，选择本地签名密钥进行签名；已开启安全启动的设备必须使用原来的受信任密钥
>
> 运行 Nuke 会永久删除外部 Flash 中的全部固件、凭据、PIN 和设置；请先准备好已签名的 Pico All 恢复固件；OTP 和硬件锁定不会被清除

1. 在 PicoForge All → 固件中选择 Nuke UF2 和签名密钥，完成签名
2. 点击“刷写”，确认擦除提示；指示灯闪烁时按下并松开设备按键（BOOTSEL）
3. 完成后安装已签名的 [Pico All](https://github.com/XiaoNetwork-Astral/pico-all/releases/latest) 固件

从 BOOTSEL 直接加载时，Nuke 会立即开始执行，不会再次等待按键；校验完成后，Flash 中仅保留 picotool 使用的少量 `NUKE` 标记

| 指示灯 | 含义 |
| --- | --- |
| 红色呼吸 | 当前 Pico All 固件等待擦除确认 |
| 红色常亮 | 正在擦除／校验；完成后在 BOOTSEL 中等待恢复固件 |
| 红色快闪 | 擦除或校验失败 |

## 构建

需要 Arm 工具链、CMake、Ninja 和 Pico SDK 2.3.1

```sh
git clone https://github.com/XiaoNetwork-Astral/pico-nuke-all.git
cd pico-nuke-all
cmake -S . -B build -G Ninja -DPICO_SDK_PATH=/path/to/pico-sdk -DPICO_BOARD=waveshare_rp2350_one
cmake --build build
```

生成未签名的 `build/flash_nuke.uf2`；源码也支持 `pico` 和 `pico2`，指示灯依各板硬件而定；`tests/` 中的主机测试通过模拟 Flash 验证流程，不操作实际设备

## 许可与致谢

采用 [BSD-3-Clause](LICENSE)，基于 Phil Howard 的 [Pico Universal Flash Nuke](https://github.com/Gadgetoid/pico-universal-flash-nuke) 和 Raspberry Pi 的 Flash Nuke 示例
