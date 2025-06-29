# AttachmentSet - 类型安全的动态附件容器

## 特性概述

### 核心功能
- ✅ **动态类型存储**：支持任意类型的`shared_ptr`安全存储
- ✅ **引用计数维护**：自动管理`shared_ptr`生命周期
- ✅ **迭代器支持**：STL风格迭代器遍历所有附件

- 🚀 AnyAttachmentSet基于`std::any`+类型擦除实现动态类型(C++17)
- 🚀 AnyAttachment基于`std::shared_ptr<void>`

## 基本用法

```cpp
// 定义容器
AttachmentSet attachments;

// 存储附件
attachments.set<MyClass>("obj", std::make_shared<MyClass>());
attachments.set<int>("value", std::make_shared<int>(42));

// 获取附件
if (auto ptr = attachments.get<MyClass>("obj")) {
    ptr->doSomething();
}

// 类型检查
if (attachments.is<int>("value")) {
    // 安全操作...
}

// 遍历附件
for (auto it = attachments.begin(); it != attachments.end(); ++it) {
    if (it.is<MyClass>()) {
        auto obj = it.get<MyClass>();
    }
}
```
## 适用场景
- 需要动态关联数据的对象系统
- 插件架构中的扩展点实现
- 跨模块通信时的类型安全包装
- 无需预定义支持的类型, 可动态添加新类型支持