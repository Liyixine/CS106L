### **CS106L 课程笔记：Lecture 9 - 继承与多态**

**课程：** CS106L, Spring 2025
**日期：** 2025年春季学期
**核心主题：** C++ 中的继承 (Inheritance), 虚函数 (Virtual Functions), 动态分派 (Dynamic Dispatch), 以及面向对象设计原则 (Composition over Inheritance)。

---

### **Part 1: 课前回顾 - C++ 类与内存**

在深入继承之前，课程首先回顾了C++中 `class` 的一些基础，特别是与Python的对比，为后续理解性能和内存开销奠定基础。

#### **1.1 C++ 与 Python 的内存布局对比**

*   **Python (动态类型语言):**
    *   **内存开销大:** 为了支持动态类型和运行时类型检查，Python对象在内存中存储了大量**元数据 (metadata)**。
    *   **结构:** 一个Python对象实例（如 `p = Point(1, 2)`）在内存中除了数据 `_x` 和 `_y` 外，还包含了：
        *   `refcount`: 引用计数，用于自动垃圾回收。
        *   `type`: 指向类型信息的指针，使得 `isinstance()` 等运行时检查成为可能。
        *   `__dict__`: 一个字典，用于存储实例的属性，这本身又是一个复杂的对象。
    *   **结论:** 这种设计提供了极大的灵活性，但牺牲了内存效率。

*   **C++ (静态类型语言):**
    *   **内存开销小:** C++对象在内存中**只存储其数据成员**，不多不少。
    *   **结构:** 一个C++对象实例（如 `Point p {1, 2};`）在内存中就是 `int x` 和 `int y` 连续排列。
    *   **类型检查:** 所有的类型检查都在**编译时 (compile time)** 完成，运行时没有类型信息的开销。
    *   **结论:** C++的设计哲学是**“零开销抽象 (zero-cost abstraction)”**，你不用为你没有使用的东西付出代价。

#### **1.2 C++ 中的 `this` 指针**

*   **`this` 是什么:** 在类的成员函数中，`this` 是一个隐藏的指针，它指向调用该成员函数的**当前对象实例**。
*   **幕后工作原理:** 编译器会将成员函数调用（如 `p.getX()`）自动转换为一个普通函数调用，并将对象的地址作为第一个参数传递进去。
    ```cpp
    // 程序员的写法
    Point p {1, 2};
    int x = p.getX();

    // 编译器眼中的样子 (伪代码)
    Point p {1, 2};
    int x = Point_getX(&p); // 将p的地址作为'this'指针传递
    ```
*   **重要性:** 在成员函数中，`this->member` 可以明确地访问当前对象的成员，尤其是在成员函数参数名与成员变量名相同时，必须使用 `this` 来消除歧义。

---

### **Part 2: 核心概念 - 继承 (Inheritance)**

继承是面向对象编程的三大支柱之一，它允许我们创建基于现有类的新类。

#### **2.1 什么是继承？**

*   **核心思想:** 一个类可以**继承 (inherit)** 另一个类的属性和方法。这建立了一种 **"is-a" (是一个)** 的关系。
*   **术语:**
    *   **基类 (Base Class) / 父类 (Parent Class):** 被继承的类 (e.g., `Entity`)。
    *   **派生类 (Derived Class) / 子类 (Child Class):** 继承而来的新类 (e.g., `Player`)。
*   **目的:**
    *   **代码复用:** 将通用代码放在基类中，避免在多个子类中重复。
    *   **建立类型层次:** 模拟现实世界的分类关系 (e.g., `GasEngine` is-a `CombustionEngine` is-an `Engine`)。

#### **2.2 C++ 中的继承实现**

*   **语法:** `class Derived : [继承方式] Base { ... };`
*   **访问修饰符与继承方式:**
    *   **成员访问权限 (`public`, `protected`, `private`):**
        *   `public`: 任何地方都可以访问。
        *   `protected`: 只有**当前类及其所有子类**可以访问。
        *   `private`: 只有**当前类**可以访问。
    *   **继承方式 (`public`, `private`):**
        *   `public` 继承 (最常用): 基类的 `public` 成员在派生类中仍然是 `public`；`protected` 成员仍然是 `protected`。这是实现 "is-a" 关系的标准方式。
        *   `private` 继承 (默认): 基类的 `public` 和 `protected` 成员在派生类中都变成 `private`。

> **注意:** 在 `class` 定义中，如果你省略继承方式 (e.g., `class Player : Entity`)，C++ **默认为 `private` 继承**，这通常不是我们想要的。因此，最佳实践是**始终明确写出 `public` 继承**。

---

### **Part 3: 多态 (Polymorphism) 的引入与挑战**

多态意为“多种形态”，允许我们以统一的方式处理不同类型的对象。

#### **3.1 问题初现：对象切片 (Object Slicing)**

假设我们想用一个 `std::vector` 来管理所有的游戏实体：
```cpp
std::vector<Entity> entities;
entities.push_back(Player());
entities.push_back(Projectile());
```
这会导致一个致命问题：**对象切片**。

*   **原因:** `std::vector<Entity>` 为每个元素分配的内存大小是 `sizeof(Entity)`。而派生类 `Player` 的大小 (`sizeof(Player)`) 大于 `Entity`。
*   **过程:** 当你将一个 `Player` 对象存入这个 `vector` 时，只有它从 `Entity` 继承的那部分数据被复制了过去，`Player` 独有的成员（如 `hitpoints`）被**“切掉”并丢弃**了。
*   **结论:** "A `Projectile` doesn't 'fit' into an `Entity`"。我们**永远不能**按值存储具有多态性的对象。

#### **3.2 第一步解决方案：使用指针**

为了避免切片，我们应该存储指针，因为所有指针的大小都是相同的。
```cpp
std::vector<Entity*> entities;
entities.push_back(new Player());
entities.push_back(new Projectile());
```
这样，对象本身在堆上完整地创建，`vector` 中只存储指向它们的地址，避免了切片。

#### **3.3 新问题：静态分派 (Static Dispatch)**

即便使用了指针，下面的代码仍然无法正常工作：
```cpp
for (Entity* entity : entities) {
    entity->update(); // 问题：这里调用的总是 Entity::update()
}
```
*   **原因:** C++ 默认使用**静态分派**。
*   **编译时类型 vs. 运行时类型:**
    *   **编译时类型:** 变量被声明的类型。编译器看到 `entity` 是 `Entity*` 类型。
    *   **运行时类型:** 指针实际指向的对象的真实类型 (可能是 `Player`, `Projectile` 等)。
*   **静态分派:** 编译器在**编译时**，根据变量的**编译时类型** (`Entity*`) 来决定调用哪个函数。因此，它总是决定调用 `Entity::update()`。

---

### **Part 4: 终极解决方案 - 虚函数 (Virtual Functions)**

为了解决静态分派的问题，我们需要一种机制，让函数调用在**运行时**根据对象的**真实类型**来决定。这就是**动态分派 (Dynamic Dispatch)**。

#### **4.1 `virtual` 关键字**

*   **作用:** 在基类中，将一个函数声明为 `virtual`，即可启用该函数的动态分派。
    ```cpp
    class Entity {
    public:
        virtual void update(); // 启用了动态分派
        virtual void render();
    };
    ```
*   **`override` 关键字:** 在派生类中，使用 `override` 关键字明确表示你正在重写一个基类的虚函数。这是一个好习惯，可以让编译器帮你检查错误。

#### **4.2 虚函数的底层原理：`vtable` 机制**

*   **`vpointer` (虚函数指针):** 当一个类拥有虚函数时，编译器会在该类的**每个对象实例**中悄悄增加一个隐藏的指针，即 `vpointer`。
*   **`vtable` (虚函数表):** 对于**每个拥有虚函数的类**，编译器会创建一个静态的、唯一的表，即 `vtable`。这张表是一个函数指针数组，存储了该类所有虚函数的**正确入口地址**。
*   **工作流程 (动态分派):**
    1.  通过对象指针，找到对象内存。
    2.  从对象内存中，读取其 `vpointer` 的值。
    3.  通过 `vpointer`，找到该类对应的 `vtable`。
    4.  在 `vtable` 中，查找要调用的函数（如 `update`）的地址。
    5.  调用该地址指向的函数。
*   这个 `对象 -> vpointer -> vtable -> 函数` 的间接查找过程，确保了总能调用到对象真实类型的函数版本。

#### **4.3 虚函数的成本**

`virtual` 并非免费，它有两项开销，这也是为什么C++需要你**“选择加入 (opt-in)”**：
1.  **内存开销:** 每个对象多一个 `vpointer` 的大小 (在64位系统上是8字节)。
2.  **性能开销:** 每次调用都需要一次 `vtable` 查找，比直接的函数调用要慢。在对性能要求极高的领域（如量化金融），通常会避免使用虚函数。

#### **4.4 纯虚函数与抽象基类**

*   **纯虚函数:** 通过在虚函数声明后加 `= 0;` 来创建。
    ```cpp
    virtual void update() = 0; // 纯虚函数
    ```
*   **作用:**
    1.  **定义接口:** 它只有声明，没有实现。它规定了一个“合同”。
    2.  **创建抽象类:** 任何包含纯虚函数的类都是**抽象类**，**不能被实例化**。
    3.  **强制派生类实现:** 任何非抽象的派生类**必须**重写所有继承来的纯虚函数。

---

### **Part 5: 软件设计原则 - 组合与继承**

课程的最后，从更高的设计层面探讨了继承的适用范围。

#### **5.1 优先使用组合，而不是继承 (Prefer Composition over Inheritance)**

*   **继承 (Inheritance):** 代表 **"is-a"** 关系。它是一种强耦合的、白盒的复用。
*   **组合 (Composition):** 代表 **"has-a"** 关系。一个类将其他类的对象作为自己的成员。
*   **核心原则:** 当逻辑关系是“拥有/包含”时，滥用继承会导致不合逻辑、僵化且脆弱的设计。
    *   **错误示例:** `class Car : public Engine` (车 **是** 一个引擎)。
    *   **正确示例:** `class Car { Engine engine; }` (车 **拥有** 一个引擎)。

#### **5.2 组合与继承的结合**

最强大的设计往往是将两者结合，各司其职：
1.  **使用组合构建整体结构:** `Car` 类拥有一个 `Engine*` 指针。这确立了 "has-a" 关系。**指针**巧妙运用!
2.  **使用继承管理组件的多样性:** 创建一个 `Engine` 的继承体系 (`GasEngine`, `ElectricEngine` 等)。这确立了 "is-a" 关系。

```cpp
// 顶层使用组合，成员是指向基类的指针
class Car {
    Engine* engine;
    //...
};

// 用继承来提供多种具体实现
class Engine {};
class CombustionEngine : public Engine {};
class GasEngine : public CombustionEngine {};
class ElectricEngine : public Engine {};
```
这种模式（与**PIMPL**思想相关）兼具了灵活性和扩展性，是现代C++设计中的常用技巧。