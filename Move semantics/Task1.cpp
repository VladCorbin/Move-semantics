#include <iostream>
#include <vector>
#include <string>

// Шаблонная функция: перемещает содержимое из src в dst
template <typename T>
void move_vectors(std::vector<T>& src, std::vector<T>& dst) {
    dst = std::move(src);  // Перемещаем данные из src в dst
}

int main() {
    // Исходный вектор
    std::vector<std::string> one = { "test_string1", "test_string2" };

    // Целевой вектор (пустой)
    std::vector<std::string> two;

    // Перемещаем содержимое из one в two
    move_vectors(one, two);

    // Проверяем результаты
    std::cout << "Vector 'one' size after move: " << one.size() << std::endl;  // 0
    std::cout << "Vector 'two' size after move: " << two.size() << std::endl;     // 2

    std::cout << "Contents of 'two':" << std::endl;
    for (const auto& str : two) {
        std::cout << str << std::endl;
    }

    return 0;
}
