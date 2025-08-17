### **CS106L 课程笔记：第五讲 - 流 (Streams)**

#### **1. 核心思想：为什么需要流？**

*   **问题**: 为编程语言设计一个通用的输入/输出（I/O）功能极其困难（Bjarne Stroustrup语）。数据可能来自键盘、文件或网络，也可能要被发送到屏幕、文件或网络。为每一种情况都写一套独立的逻辑会非常复杂和繁琐。
*   **解决方案**: **流 (Stream)**。流是C++中用于处理I/O的**核心抽象**。
*   **定义**: 流是一个**通用的接口**，它提供了一个统一的、一致的方式来在程序中读取和写入数据，而无需关心数据的具体来源或目的地。它就像一个**通道**，将数据的复杂来源和去向细节隐藏起来。

#### **2. 流的分类与基本操作**

所有流都可以被分为两大基本类型，它们在功能上是**互补的**：

*   **输入流 (Input Streams)**
    *   **目的**: 从数据源**读取**数据到程序中。
    *   **基类**: `std::istream`
    *   **操作符**: `>>` (提取运算符, Extraction Operator)。它从流中“提取”数据并存入变量。
    *   **示例**: `std::cin` (标准输入流，通常是键盘)、`std::ifstream` (输入文件流)。

*   **输出流 (Output Streams)**
    *   **目的**: 将程序中的数据**写入**到目的地。
    *   **基类**: `std::ostream`
    *   **操作符**: `<<` (插入运算符, Insertion Operator)。它将变量的数据“插入”到流中。
    *   **示例**: `std::cout` (标准输出流，通常是屏幕)、`std::ofstream` (输出文件流)。

*   **可读可写流**: 继承自 `std::iostream` 的流（如 `stringstream`）同时具备输入和输出流的特性。

#### **3. 深入理解流的工作机制**

##### **A. 缓冲 (Buffering)**

缓冲是流为了**提高效率**而采用的核心机制。与外部设备（如屏幕、硬盘）的交互速度很慢，频繁的I/O操作会严重影响性能。

*   **输出缓冲**: 当你使用 `cout << "data";` 时，数据**不会**立即显示在屏幕上。它首先被放入 `cout` 内部的一个**缓冲区（购物车）**。当满足特定条件时，缓冲区中的所有内容才会被**一次性地“冲刷”（Flush）**到目的地。
*   **输入缓冲**: 当你从键盘输入时，字符首先被放入 `cin` 的**输入缓冲区（传送带）**。程序会优先从缓冲区读取数据，只有当缓冲区为空时，才会暂停并等待用户输入。

##### **B. 冲刷 (Flushing) 与 `'\n'` vs `std::endl`**

*   `std::endl`: 做**两件事**：1. 向流中插入一个换行符 `'\n'`；2. **立即强制冲刷缓冲区**。在循环中大量使用会导致性能低下。
*   `'\n'`: 只做**一件事**：向流中插入一个换行符。它本身不保证冲刷。冲刷由系统在特定时机（如缓冲区满、程序结束）自动进行。
*   **重要澄清**: 当 `cout` 连接到交互式终端时，通常采用**行缓冲**策略，此时 `'\n'` 也会触发冲刷，使其表现得像 `endl`。要获得真正的性能提升，尤其是在文件输出或性能竞赛中，应在程序开头使用 `std::ios_base::sync_with_stdio(false);` 来关闭这种默认行为。
*   **结论**: 除非必须立即看到输出，否则**为了性能，应始终优先使用 `'\n'`**。

##### **C. 其他标准流**

*   `std::cerr`: 标准**错误**流。**无缓冲**，保证错误信息能立即输出，即使程序即将崩溃。
*   `std::clog`: 标准**日志**流。**有缓冲**，用于记录非紧急的程序事件，性能比 `cerr` 好。

#### **4. 三种主要流的应用**

##### **A. 文件流 (File Streams)**

*   **`std::ofstream` (Output File Stream)**: 向文件写入。
    ```cpp
    // 默认打开模式：清空并写入 (truncate)
    std::ofstream my_file("output.txt"); 

    // 追加模式：在文件末尾添加内容
    std::ofstream my_file_append("output.txt", std::ios::app);

    if (my_file.is_open()) {
        my_file << "Hello, CS106L!\n";
        my_file << "This is a new line.\n";
    }
    my_file.close(); // 结束时关闭文件
    ```*   **`std::ifstream` (Input File Stream)**: 从文件读取。
    ```cpp
    std::ifstream my_file("input.txt");
    std::string line;
    if (my_file.is_open()) {
        // 读取文件的第一行
        std::getline(my_file, line); 
        std::cout << line << '\n';
    }
    ```

##### **B. 字符串流 (String Streams)**

`std::stringstream` 是一个在内存中操作的、极其强大的**格式转换工具**。

*   **用途1：解析 (Parsing)** - 将字符串分解为不同类型的数据。
    ```cpp
    std::string data = "Bjarne 65 C++";
    std::stringstream ss(data);
    
    std::string name;
    int age;
    std::string lang;

    ss >> name >> age >> lang; // name="Bjarne", age=65, lang="C++"
    ```
*   **用途2：格式化 (Formatting)** - 将不同类型数据组合成一个字符串。
    ```cpp
    std::stringstream ss;
    std::string name = "Fabio";
    int year = 2025;

    ss << "Lecturer: " << name << ", Year: " << year;
    std::string result = ss.str(); // result = "Lecturer: Fabio, Year: 2025"
    ```

#### **5. 最重要的陷阱：混合使用 `cin >>` 和 `getline`**

这是本节课**最关键的实践性问题**。

*   **问题根源**:
    1.  `std::cin >> var;` 从缓冲区读取数据后，会将作为分隔符的**换行符 `\n` 留在缓冲区**中。
    2.  后续的 `std::getline(std::cin, line);` 会立即读到这个被留下的 `\n`，并认为它已经读取完一整行（一个空行），导致 `line` 变量变为空字符串。

*   **错误示例**:
    ```cpp
    int age;
    std::string name;
    std::cout << "Enter age: ";
    std::cin >> age; // 用户输入 25 并回车，'\n' 被留下
    std::cout << "Enter full name: ";
    std::getline(std::cin, name); // getline 立即读到'\n'，name 为空！
    ```

*   **最终建议**: **不要混合使用 `cin >>` 和 `getline`！** 为了写出健壮的代码，推荐的模式是：**始终使用 `getline` 读取整行输入，然后用 `stringstream` 来解析这一行中的数据。**

---