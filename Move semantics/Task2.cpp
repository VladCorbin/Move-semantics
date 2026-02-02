#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

class big_integer {
private:
    std::vector<int> digits;  // цифры числа (в обратном порядке: единицы ? десятки ? ...)
    bool is_negative = false;   // знак числа

    // Вспомогательная функция: удаляет ведущие нули
    void remove_leading_zeros() {
        while (digits.size() > 1 && digits.back() == 0) {
            digits.pop_back();
        }
        // Если осталось одно ноль — сохраняем, но убеждаемся, что не отрицательное
        if (digits.empty()) {
            digits.push_back(0);
            is_negative = false;
        }
    }

    // Вспомогательная функция: сравнивает два вектора цифр (без учёта знака)
    static bool abs_greater_or_equal(const std::vector<int>& a, const std::vector<int>& b) {
        if (a.size() != b.size()) return a.size() > b.size();
        for (size_t i = a.size(); i > 0; --i) {
            if (a[i - 1] != b[i - 1]) return a[i - 1] > b[i - 1];
        }
        return true;  // равны
    }

public:
    // Конструктор из строки
    explicit big_integer(const std::string& str) {
        if (str.empty()) throw std::invalid_argument("Empty string");

        size_t start = 0;
        if (str[0] == '-') {
            is_negative = true;
            start = 1;
        }
        else if (str[0] == '+') {
            start = 1;
        }

        if (start >= str.size()) throw std::invalid_argument("No digits after sign");

        for (size_t i = start; i < str.size(); ++i) {
            if (!isdigit(str[i])) throw std::invalid_argument("Non-digit character");
            digits.push_back(str[i] - '0');
        }

        std::reverse(digits.begin(), digits.end());  // теперь единицы в начале
        remove_leading_zeros();
    }

    // Конструктор по умолчанию (ноль)
    big_integer() : digits{ 0 }, is_negative(false) {}

    // Конструктор перемещения
    big_integer(big_integer&& other) noexcept
        : digits(std::move(other.digits)), is_negative(other.is_negative) {
        other.is_negative = false;
    }

    // Перемещающий оператор присваивания
    big_integer& operator=(big_integer&& other) noexcept {
        if (this != &other) {
            digits = std::move(other.digits);
            is_negative = other.is_negative;
            other.is_negative = false;
        }
        return *this;
    }

    // Оператор сложения
    big_integer operator+(const big_integer& other) const {
        big_integer result;

        // Случай: одно из чисел отрицательное
        if (is_negative != other.is_negative) {
            big_integer abs_this = *this;
            abs_this.is_negative = false;
            big_integer abs_other = other;
            abs_other.is_negative = false;

            if (abs_greater_or_equal(abs_this.digits, abs_other.digits)) {
                result = abs_this - abs_other;
                result.is_negative = is_negative;
            }
            else {
                result = abs_other - abs_this;
                result.is_negative = other.is_negative;
            }
            return result;
        }

        // Оба числа одного знака: складываем модули
        result.is_negative = is_negative;  // знак сохраняется
        const auto& a = digits;
        const auto& b = other.digits;
        size_t max_len = std::max(a.size(), b.size());
        int carry = 0;

        for (size_t i = 0; i < max_len || carry; ++i) {
            int sum = carry;
            if (i < a.size()) sum += a[i];
            if (i < b.size()) sum += b[i];

            result.digits.push_back(sum % 10);
            carry = sum / 10;
        }

        result.remove_leading_zeros();
        return result;
    }

    // Оператор умножения на число (int)
    big_integer operator*(int num) const {
        if (num == 0) return big_integer("0");

        big_integer result;
        result.is_negative = is_negative ^ (num < 0);  // XOR для знака
        num = std::abs(num);

        int carry = 0;
        for (size_t i = 0; i < digits.size() || carry; ++i) {
            int product = carry;
            if (i < digits.size()) product += digits[i] * num;
            result.digits.push_back(product % 10);
            carry = product / 10;
        }

        result.remove_leading_zeros();
        return result;
    }

    // Оператор вычитания (для внутренней логики сложения с разными знаками)
    big_integer operator-(const big_integer& other) const {
        // Реализуем через сложение с отрицательным
        big_integer neg_other = other;
        neg_other.is_negative = !neg_other.is_negative;
        return (*this) + neg_other;
    }

    // Вывод в поток
    friend std::ostream& operator<<(std::ostream& os, const big_integer& num) {
        if (num.is_negative && !(num.digits.size() == 1 && num.digits[0] == 0)) {
            os << '-';
        }
        for (auto it = num.digits.rbegin(); it != num.digits.rend(); ++it) {
            os << *it;
        }
        return os;
    }
};

// Пример использования
int main() {
    auto number1 = big_integer("114575");
    auto number2 = big_integer("78524");

    auto result = number1 + number2;
    std::cout << result << std::endl;  // 193099

    auto product = number1 * 3;
    std::cout << product << std::endl;    // 343725

    return 0;
}
