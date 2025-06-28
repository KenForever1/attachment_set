# AttachmentSet - 类型安全的动态附件容器

## 特性概述

### 核心功能
- ✅ **动态类型存储**：支持任意类型的`shared_ptr`安全存储
- ✅ **类型安全访问**：编译期类型检查 + 运行时类型验证
- ✅ **引用计数维护**：自动管理`shared_ptr`生命周期
- ✅ **迭代器支持**：STL风格迭代器遍历所有附件

### 实现亮点
- 🚀 基于`std::any`+类型擦除实现动态类型
- 🔒 使用`std::type_index`保证类型安全
- 🧩 模板化接口提供编译期类型检查

## 基本用法

```cpp
// 定义容器
demo::AttachmentSet attachments;

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

## 设计优势

1. **类型安全**
   - 杜绝错误类型转换
   - 避免`std::bad_any_cast`异常

2. **性能优化**
   - 使用连续内存存储（std::vector）
   - 快速类型比较（type_index哈希）

3. **扩展性**
   - 无需预定义支持的类型
   - 可动态添加新类型支持

## 适用场景
- 需要动态关联数据的对象系统
- 插件架构中的扩展点实现
- 跨模块通信时的类型安全包装