# Vbird_HLTS

面向有特殊教育需要、认知障碍或智力障碍学生的酒店职业训练工具，帮助学习者熟悉酒店布草配送、工作流程、基础知识与任务反馈，为后续实习和就业做准备。

> 本项目是教学与职业训练辅助工具，不用于医疗诊断或替代专业教育评估。

## 功能

- **业务学习**：通过图片分步骤学习酒店工作内容。
- **知识测验**：图片化选择题、成绩统计与答题反馈。
- **模拟实训**：以交互式场景练习打卡、领取任务、布草取放、库存管理、突发任务和工作汇报。
- **训练报告**：在本机生成 CSV 简报和操作记录，便于教师复盘。
- **适配教学现场**：支持界面缩放、视频教程、开发者坐标校准和文本样式配置。

## 隐私

应用离线运行，不包含网络上传功能。训练报告可能涉及学习者信息，因此公开版本默认不记录姓名、年龄、性别、班级，文件名也默认匿名。部署者应取得必要授权并安全保管报告。详见 [PRIVACY.md](PRIVACY.md)。

## 技术栈

- C++17
- Qt 6 Widgets
- Qt 6 Multimedia / MultimediaWidgets
- CMake 3.16+
- 主要面向 Windows 10/11

## 构建

### Windows 11 / PowerShell

安装 Qt 6（包含 Widgets、Multimedia、MultimediaWidgets）和 CMake，然后执行：

```powershell
cmake -S . -B build
cmake --build build --config Release
```

从仓库根目录启动程序，确保 `source/` 目录与生成的可执行文件所使用的工作目录关系正确：

```powershell
.\build\Release\HotelTrainingApp.exe
```

如使用 Qt Creator，可直接打开根目录的 `CMakeLists.txt`。

## 模块结构

- `test1.*`：业务学习幻灯片
- `test2.*`：知识测验与计分
- `test3.*`：酒店布草配送模拟
- `logger.*`：本地 CSV 训练报告
- `config_*.h`：界面、文本、资源路径与报告字段配置
- `source/`：原创教学图片和视频素材

## 数据保护建议

- 优先使用学生编号或化名。
- 不要把真实训练 CSV、截图或学生信息提交到 Git。
- 仅在有明确教学必要和授权时开启个人字段。
- 定期清理训练设备上的历史报告。

## 贡献

欢迎特殊教育教师、职业训练人员、酒店从业者、无障碍设计者和 Qt 开发者参与。提交前请阅读 [CONTRIBUTING.md](CONTRIBUTING.md) 和 [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md)。

## 许可证

代码及仓库内由作者原创、且未另行标注的配套素材采用 [MIT License](LICENSE)。
