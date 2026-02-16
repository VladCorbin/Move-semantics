#include <iostream>
#include <string>
#include <algorithm>
#include <stdexcept>

class big_integer {
private:
    std::string digits;
    bool is_negative;

    void remove_leading_zeros() {
        size_t non_zero_pos = digits.find_first_not_of('0');
        if (non_zero_pos == std::string::npos) {
            digits = "0";
            is_negative = false;
        }
        else {
            digits = digits.substr(non_zero_pos);
        }
    }

    static bool abs_greater_or_equal(const std::string& a, const std::string& b) {
        if (a.length() != b.length()) {
            return a.length() > b.length();
        }
        return a >= b;
    }

public:
    explicit big_integer(const std::string& num = "0") : is_negative(false) {
        if (num.empty()) {
            throw std::invalid_argument("Empty string for big_integer");
        }

        size_t start = 0;
        if (num[0] == '-') {
            is_negative = true;
            start = 1;
        }
        else if (num[0] == '+') {
            start = 1;
        }

        digits = num.substr(start);

        for (char c : digits) {
            if (!std::isdigit(c)) {
                throw std::invalid_argument("Invalid character in big_integer: " + std::string(1, c));
            }
        }

        remove_leading_zeros();
    }

    big_integer(big_integer&& other) noexcept
        : digits(std::move(other.digits)), is_negative(other.is_negative) {
        other.is_negative = false;
    }

    big_integer& operator=(big_integer&& other) noexcept {
        if (this != &other) {
            digits = std::move(other.digits);
            is_negative = other.is_negative;
            other.is_negative = false;
        }
        return *this;
    }

    big_integer operator+(const big_integer& other) const {
        if (is_negative != other.is_negative) {
            big_integer abs_this = *this;
            abs_this.is_negative = false;
            big_integer abs_other = other;
            abs_other.is_negative = false;

            if (abs_this == abs_other) {
                return big_integer("0");
            }

            bool this_larger = abs_greater_or_equal(abs_this.digits, abs_other.digits);
            big_integer result;
            result.is_negative = this_larger ? is_negative : other.is_negative;

            const std::string& larger = this_larger ? abs_this.digits : abs_other.digits;
            const std::string& smaller = this_larger ? abs_other.digits : abs_this.digits;

            std::string res_digits;
            int borrow = 0;
            size_t len1 = larger.length(), len2 = smaller.length();

            for (size_t i = 0; i < len1; ++i) {
                int digit1 = larger[len1 - 1 - i] - '0';
                int digit2 = (i < len2) ? smaller[len2 - 1 - i] - '0' : 0;
                int diff = digit1 - digit2 - borrow;

                if (diff < 0) {
                    diff += 10;
                    borrow = 1;
                }
                else {
                    borrow = 0;
                }
                res_digits.push_back('0' + diff);
            }
            std::reverse(res_digits.begin(), res_digits.end());
            result.digits = res_digits;
            result.remove_leading_zeros();
            return result;
        }

        big_integer result;
        result.is_negative = is_negative;

        std::string res_digits;
        int carry = 0;
        size_t max_len = std::max(digits.length(), other.digits.length());

        for (size_t i = 0; i < max_len; ++i) {
            int digit1 = (i < digits.length()) ? digits[digits.length() - 1 - i] - '0' : 0;
            int digit2 = (i < other.digits.length()) ? other.digits[other.digits.length() - 1 - i] - '0' : 0;
            int sum = digit1 + digit2 + carry;

            res_digits.push_back('0' + (sum % 10));
            carry = sum / 10;
        }

        if (carry) {
            res_digits.push_back('0' + carry);
        }

        std::reverse(res_digits.begin(), res_digits.end());
        result.digits = res_digits;
        return result;
    }

    big_integer operator*(int multiplier) const {
        if (multiplier == 0) return big_integer("0");

        bool result_negative = is_negative ^ (multiplier < 0);
        multiplier = std::abs(multiplier);

        big_integer result;
        result.is_negative = result_negative;

        std::string res_digits;
        int carry = 0;

        for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
            int digit = (*it) - '0';
            int product = digit * multiplier + carry;
            res_digits.push_back('0' + (product % 10));
            carry = product / 10;
        }

        while (carry) {
            res_digits.push_back('0' + (carry % 10));
            carry /= 10;
        }

        std::reverse(res_digits.begin(), res_digits.end());
        result.digits = res_digits;
        result.remove_leading_zeros();
        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const big_integer& num) {
        if (num.is_negative && num.digits != "0") {
            os << '-';
        }
        os << num.digits;
        return os;
    }

    bool operator==(const big_integer& other) const {
        return is_negative == other.is_negative && digits == other.digits;
    }
};

int main() {
    try {
        auto number1 = big_integer("114575");
        auto number2 = big_integer("78524");
        auto result = number1 + number2;
        std::cout << result << std::endl;  // Выведет: 193099

        auto number3 = big_integer("123456789");
        auto result2 = number3 * 123;
        std::cout << result2 << std::endl;  // Выведет: 15185185047

        // Проверка перемещения
        big_integer temp("999999999999999999999999");
        big_integer moved = std::move(temp);
        std::cout << moved << std::endl;  // Выведет большое число

    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
