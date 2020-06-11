/*
 * A. Задача для второклассника
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Вам даны два числа. Необходимо найти их произведение.
 *
 * Входные данные
 * Входные данные состоят из двух строк, на каждой из которых находится целое одно целое число,
 * длина которого не превосходит двухсот пятидесяти тысяч символов.
 *
 * Выходные данные
 * Выведите произведение данных чисел.
 *
 * Примеры
 * Входные данные
 * 2
 * 2
 * Выходные данные
 * 4
 *
 * Входные данные
 * 1
 * -1
 * Выходные данные
 * -1
 */

/*
 * Подробнее о ДПФ, БПФ:
 * https://proglib.io/p/fourier-transform/
 * https://ru.wikipedia.org/wiki/Дискретное_преобразование_Фурье
 * https://neerc.ifmo.ru/wiki/index.php?title=Дискретное_преобразование_Фурье
 * https://drive.google.com/file/d/1IdjyWinAT3Qo9oSonFj21VASBCJsxltB/view
 * https://cp-algorithms.com/algebra/fft.html
 * https://neerc.ifmo.ru/wiki/index.php?title=Быстрое_преобразование_Фурье
 * https://ru.wikipedia.org/wiki/Быстрое_преобразование_Фурье
 * https://e-maxx.ru/algo/fft_multiply
 * Дополнительные материалы:
 * https://ru.wikipedia.org/wiki/Схема_Горнера
 */

#define _USE_MATH_DEFINES

#include <random>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <math.h>
#include <array>
#include <utility>
#include <iomanip>
#include <optional>
#include <complex>
#include <string>
#include <numeric>
#include <cctype>
#include <utility>


/**
 * @brief Проверка на равенство чисел с плавающей точкой.
 * @tparam FloatType Тип чисел: float или double.
 * @param lhs Первое число.
 * @param rhs Второе число.
 * @param eps Точность проверки.
 * @return True, если числа равны, иначе - false.
 */
template<typename FloatType>
bool is_equal(FloatType lhs, FloatType rhs, FloatType eps=1e-6) {
    return std::abs(lhs - rhs) < eps;
}

/**
 * @brief Содержит операции, необходимые для быстрого преобразования Фурье. Просто namespace с private-методами.
 */
class fft {
public:
    /**
     * @brief Выполнить преобразование Фурье.
     * @details Нерекурсивная реализация.
     * @param samples Отсчёты сигнала, от которого берём прямое преобразование, или спектр для обратного преобразования.
     * @param inverse True, если обратное преобразование, false - прямое.
     * @return Спектр сигнала при прямом преобразовании, отсчёты сигнала - при обратном.
     */
    static std::vector<std::complex<double>>
    transfrom(std::vector<std::complex<double>> samples, bool inverse = false) {
        pad(samples);
        /*
         * Вначале к вектору a применяется поразрядно обратная перестановка, для чего вычисляется количество
         * значащих бит (lg n) в числе n, и для каждой позиции i находится соответствующая ей позиция,
         * битовая запись которой есть битовая запись числа i, записанная в обратном порядке.
         * Если получившаяся в результате позиция оказалась больше i, то элементы в этих двух позициях надо обменять
         * (если не это условие, то каждая пара обменяется дважды, и в итоге ничего не произойдёт).
         */
        samples = sort_by_reverse_index(samples);

        /*
         * Мы применили поразрядно обратную перестановку элементов.
         * Теперь выполним всю работу, выполняемую нижним уровнем рекурсии, т.е. вектор отсчётов разделим на пары
         * элементов, для каждого применим преобразование бабочки, в результате в векторе отсчётов будут находиться
         * результаты работы нижнего уровня рекурсии. На следующем шаге разделим вектор a на четвёрки элементов,
         * к каждой применим преобразование бабочки, в результате получим ДПФ для каждой четвёрки.
         * И так далее, наконец, на последнем шаге мы, получив результаты ДПФ для двух половинок вектора отсчётов,
         * применим к ним преобразование бабочки и получим ДПФ для всего вектора отсчётов.
         */

        for (size_t len = 2; len <= samples.size(); len <<= 1) {
            /*
             * Выполняется (log(n) - 1) стадий алгоритма, на k-ой из которых (k=2...log(n)) вычисляются ДПФ для блоков
             * длины 2^k. Для всех этих блоков будет одно и то же значение первообразного корня, которое запоминается
             * в переменной w_current.
             * Цикл по i итерируется по блокам,
             * а вложенный в него цикл по j применяет преобразование бабочки ко всем элементам блока.
             */
            double arg = 2 * M_PI / len * (inverse ? -1 : 1);
            std::complex<double> w_current(cos(arg), sin(arg));
            for (int i = 0; i < samples.size(); i += len) {
                std::complex<double> w(1);
                for (int j = 0; j < len / 2; ++j) {
                    std::complex<double> u = samples[i + j], v = samples[i + j + len / 2] * w;
                    samples[i + j] = u + v;
                    samples[i + j + len / 2] = u - v;
                    w *= w_current;
                }
            }
        }
        /*
         * Если указан флаг inverse = true, то w_n заменяется на w_n^-1, а каждый элемент результата делится на 2
         * (учитывая, что эти деления на 2 произойдут в каждом уровне рекурсии, то в итоге как раз получится,
         * что все элементы поделятся на n).
         */
        if (inverse) {
            std::for_each(samples.begin(), samples.end(), [&samples](auto &sample) { sample /= samples.size(); });
        }

        return samples;
    }

private:
    fft() = default;  // запретим создание экзампляров класса

    /**
     * @brief Дополнить последовательность отсчётов до длины равной степени двойки.
     * @param samples Последовательность отсчётов.
     */
    static void pad(std::vector<std::complex<double>> &samples);

    /**
     * @brief Определяет наименьшую степень двойки, не меньшую заданного числа.
     * @details Используется знание о представлении чисел с плавающей точкой в памяти.
     * @details Подробнее: https://ru.wikipedia.org/wiki/Число_двойной_точности
     * @param number Число, которое нужно округлить вверх до степени двойки.
     * @return Число, равное степени двойки. Минимальное, но не меньше, чем number.
     */
    static size_t round_pow2(size_t number);

    /**
     * @brief Поразрядно-обратная перестановка элементов. Bit-reversal permutation.
     * @details В позиции i каждого элемента a[i] инвертируем порядок битов, и переупорядочим элементы массива в соответствии с новыми индексами.
     * @param samples Массив отсчётов сигнала или спектра.
     * @return Результат поразрядно-обратной перестановки элементов.
     */
    static std::vector<std::complex<double>> sort_by_reverse_index(const std::vector<std::complex<double>> &samples);

    /**
     * @brief Инвертировать порядок бит в числе.
     * @tparam NumberType2 Тип данных числа.
     * @param input Число.
     * @return Число с обратным порядком бит.
     */
    template<typename NumberType2>
    static NumberType2 reverse_bits(NumberType2 input);

    /**
     * @brief Инвертировать порядок байт в числе.
     * @tparam NumberType2 Тип данных числа.
     * @param input Число.
     * @return Число с обратным порядком байт.
     */
    template<typename NumberType2>
    static NumberType2 reverse_bytes(NumberType2 input);
};


std::vector<std::complex<double>> fft::sort_by_reverse_index(const std::vector<std::complex<double>> &samples) {
    // Найдём нужную перестановку.
    std::vector<size_t> indexes(samples.size());
    size_t i = 0;
    std::iota(indexes.begin(), indexes.end(), i++);
    sort(indexes.begin(), indexes.end(), [&](size_t i, size_t j) { return reverse_bits(i) < reverse_bits(j); });

    // Пересортируем элементы
    std::vector<std::complex<double>> result;
    result.reserve(samples.size());
    for (auto idx: indexes) {
        result.push_back(samples[idx]);
    }

    return result;
}

size_t fft::round_pow2(size_t number) {
    double x = static_cast<double>(number - 1);  // если убрать "- 1", то "не меньше" в описании нужно заменить на "больше"
    auto answer = reinterpret_cast<unsigned int *>(&x);
    ++answer;
    return static_cast<size_t>(1) << (((*answer & 0x7FF00000) >> 20) - 1022);
}

void fft::pad(std::vector<std::complex<double>> &samples) {
    size_t new_size = round_pow2(samples.size());
    samples.resize(new_size);
}

template<typename NumberType2>
NumberType2 fft::reverse_bits(NumberType2 input) {
    auto result = reverse_bytes(input);
    for (size_t i = 0; i < sizeof(NumberType2); ++i) {
        *(reinterpret_cast<uint8_t *>(&result) + i) = reverse_bits(*(reinterpret_cast<uint8_t *>(&result) + i));
    }
    return result;
}

template<typename NumberType2>
NumberType2 fft::reverse_bytes(NumberType2 input) {
    NumberType2 result{0};
    auto len = sizeof(NumberType2);
    for (size_t i = 0; i < len / 2; ++i) {
        std::swap(*(reinterpret_cast<uint8_t *>(&result) + i),
                  *(reinterpret_cast<uint8_t *>(&result) + len - 1 - i));
    }
    return result;
}

template<>
uint32_t fft::reverse_bytes<uint32_t>(uint32_t input) {
    return ((input >> 24) & 0x000000FF) | ((input >> 8) & 0x0000FF00) | ((input << 8) & 0x00FF0000) |
           ((input << 24) & 0xFF000000);
}

template<>
uint64_t fft::reverse_bytes<uint64_t>(uint64_t input) {
    uint64_t result = 0;
    result |= reverse_bytes(static_cast<uint32_t>(input >> 32));
    result |= static_cast<uint64_t>(reverse_bytes(static_cast<uint32_t>(input))) << 32;
    return result;
}

template<>
uint8_t fft::reverse_bits<uint8_t>(uint8_t input) {
    static const uint8_t table[] = {
            0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
            0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
            0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
            0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
            0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
            0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
            0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
            0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
            0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
            0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
            0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
            0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
            0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
            0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
            0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
            0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
            0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
            0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
            0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
            0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
            0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
            0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
            0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
            0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
            0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
            0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
            0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
            0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
            0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
            0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
            0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
            0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
    };
    return table[input];
}

/**
 * @brief Класс, описывающий многочлен с целочисленными коэффициентами.
 */
class Polynom {
public:
    Polynom() = default;

    /**
     * @brief Конструирует многочлен из набора коэффициентов.
     * @param coefficients Массив коэффициентов. Первый - свободный член. Последний - коэффициент при максимальной степени x.
     */
    explicit Polynom(std::vector<int64_t> coefficients) : coefficients_(std::move(coefficients)) {}

    /**
     * @brief Представляет число в виде многочлена.
     * @tparam IntegerType Тип данных числа.
     * @param number Целое число.
     * @param base Основание системы счисления.
     */
    template<typename IntegerType>
    explicit Polynom(IntegerType number, int16_t base = 10) {
        while (number != 0) {
            coefficients_.emplace_back(number % base);
            number /= base;
        }
    }

    /**
     * @brief Представляет длинное число в виде многочлена.
     * @details Основание системы счисления == 10.
     * @param number Число, записанное в строке.
     */
    explicit Polynom(const std::string &number) {
        auto end = number.rend();
        if (!std::isdigit(number[0])) {
            std::advance(end, -1);
        }
        std::transform(number.rbegin(), end, std::back_inserter(coefficients_), [](const auto &c) {
            return static_cast<int64_t>(c - '0');
        });
    }

    /**
     * @brief Вычислить значение многочлена.
     * @param x Значение x. Аргумент многочлена. Точка, в которой вычисляем значение.
     * @return Значение многочлена.
     */
    int64_t calculate_value(int64_t x) const {
        int64_t result = 0;
        for (auto iter = coefficients_.rbegin(); iter != coefficients_.rend(); ++iter) {
            result *= x;
            result += *iter;
        }
        return result;
    }

    /**
     * @brief Преобразует многочлен в длинное целое число.
     * @details Основание системы счисления == 10.
     * @return Строка с записанным в ней целым числом.
     */
    std::string to_string_number() const {
        int16_t base = 10;
        std::string result;
        auto end = coefficients_.end();
        while (*(end - 1) == 0) {
            std::advance(end, -1);
        }
        int64_t shifted = 0;
        for (auto iter = coefficients_.begin(); iter != end; ++iter) {
            result += std::to_string((*iter + shifted) % base);
            shifted = (*iter + shifted) / base;
        }
        while (shifted != 0) {
            result += std::to_string(shifted % base);
            shifted /= base;
        }
        std::reverse(result.begin(), result.end());
        if (result.empty()) {
            return "0";
        }
        return result;
    }

    /**
     * @brief Умножение многочленов.
     * @details Использует fft для умножения.
     * @param rhs Второй множитель.
     * @return Значение произведения.
     */
    Polynom operator*(const Polynom &rhs) {
        /*
         * Пусть даны два многочлена A и B.
         * Посчитаем ДПФ для каждого из них.
         * Что происходит при умножении многочленов? В каждой точке их значения просто перемножаются.
         * Но это означает, что если мы перемножим спектры, просто умножив каждый элемент одного вектора на
         * соответствующий ему элемент другого вектора, то мы получим не что иное, как ДПФ от произведения.
         * Наконец, применяя обратное ДПФ, получаем произведение.
         *
         * Поэлементное умножение спектром, очевидно, требует для вычисления только O(n) операций.
         * Таким образом, если мы научимся вычислять ДПФ и обратное ДПФ за время O(n log n),
         * то и произведение двух полиномов (а, следовательно, и двух длинных чисел) мы сможем найти за O(n log n).
         *
         * Следует заметить, что, во-первых, два многочлена следует привести к одной степени
         * (просто дополнив коэффициенты одного из них нулями).
         * Во-вторых, в результате произведения двух многочленов степени n получается многочлен степени 2n-1.
         */

        std::vector<std::complex<double>> samples1(coefficients_.begin(), coefficients_.end()), samples2(
                rhs.coefficients_.begin(), rhs.coefficients_.end());

        size_t fft_length = samples1.size() + samples2.size() - 1;

        samples1.resize(fft_length);
        samples2.resize(fft_length);

        auto spectrum1 = fft::transfrom(samples1);
        auto spectrum2 = fft::transfrom(samples2);

        std::vector<std::complex<double>> production_spectrum;
        production_spectrum.reserve(spectrum1.size());
        std::transform(
                spectrum1.begin(), spectrum1.end(),
                spectrum2.begin(),
                std::back_inserter(production_spectrum),
                [](const auto &lhs, const auto &rhs) {
                    return lhs * rhs;
                });
        auto production_samples = fft::transfrom(production_spectrum, true);

        std::vector<int64_t> production_coefficients;
        production_coefficients.reserve(production_samples.size());
        for (const auto &sample: production_samples) {
            production_coefficients.push_back(std::llround(sample.real()));
        }
        return Polynom(production_coefficients);
    }

private:
    std::vector<int64_t> coefficients_;
};

/**
 * @brief Класс, объединяющий работы с длинными целыми числами.
 */
class LongInt {
public:
    LongInt() = default;

    /**
     * @brief Построить объект длинного числа из строчного представления числа.
     * @param number Число, записанное в строке.
     */
    explicit LongInt(const std::string &number) : polynom_(number) {
        if (number[0] == '-') {
            is_negative_ = true;
        }
    }

    /**
     * @brief Напечатать число в поток.
     * @param stream Поток вывода.
     */
    void print(std::ostream &stream = std::cout) {
        auto number = polynom_.to_string_number();
        if (is_negative_ && (number != "0")) {
            stream << '-';
        }
        stream << number << std::endl;
    }

    /**
     * @brief Умножение длинных чисел.
     * @param rhs Второй множитель.
     * @return Результат произведения.
     */
    LongInt operator*(const LongInt &rhs) {
        LongInt result;
        result.polynom_ = polynom_ * rhs.polynom_;
        result.is_negative_ = is_negative_ ^ rhs.is_negative_;
        return result;
    }

private:
    Polynom polynom_;
    bool is_negative_{false};
};


// Начало тестов

void test_from_task_1() {
    LongInt lhs("2");
    LongInt rhs("2");
    LongInt prod = lhs * rhs;
    std::stringstream ss;
    prod.print(ss);
    std::string result;
    ss >> result;
    auto true_result = std::to_string(4);
    assert(result == true_result);
}

void test_from_task_2() {
    LongInt lhs("1");
    LongInt rhs("-1");
    LongInt prod = lhs * rhs;
    std::stringstream ss;
    prod.print(ss);
    std::string result;
    ss >> result;
    auto true_result = std::to_string(-1);
    assert(result == true_result);
}

void test_negative() {
    LongInt lhs("-1");
    LongInt rhs("-1");
    LongInt prod = lhs * rhs;
    std::stringstream ss;
    prod.print(ss);
    std::string result;
    ss >> result;
    auto true_result = std::to_string(1);
    assert(result == true_result);
}

void test_one_symbol() {
    LongInt lhs("7");
    LongInt rhs("6");
    LongInt prod = lhs * rhs;
    std::stringstream ss;
    prod.print(ss);
    std::string result;
    ss >> result;
    auto true_result = std::to_string(42);
    assert(result == true_result);
}

void test_two_symbols() {
    LongInt lhs("-13");
    LongInt rhs("17");
    LongInt prod = lhs * rhs;
    std::stringstream ss;
    prod.print(ss);
    std::string result;
    ss >> result;
    auto true_result = std::to_string(-221);
    assert(result == true_result);
}

void test_zero() {
    LongInt lhs("0");
    LongInt rhs("-17");
    LongInt prod = lhs * rhs;
    std::stringstream ss;
    prod.print(ss);
    std::string result;
    ss >> result;
    auto true_result = std::to_string(0);
    assert(result == true_result);
}

void test_zeros() {
    LongInt lhs("0");
    LongInt rhs("0");
    LongInt prod = lhs * rhs;
    std::stringstream ss;
    prod.print(ss);
    std::string result;
    ss >> result;
    auto true_result = std::to_string(0);
    assert(result == true_result);
}

void test_long() {
    {
        LongInt lhs("156468461846841612654498");
        LongInt rhs("11684165165416541614654");
        LongInt prod = lhs * rhs;
        std::stringstream ss;
        prod.print(ss);
        std::string result;
        ss >> result;
        auto true_result = "1828203351397173961590219912347471835955813692";
        assert(result == true_result);
    }
    {
        LongInt lhs("1390824942691875931654");
        LongInt rhs("373294235818098884803");
        LongInt prod = lhs * rhs;
        std::stringstream ss;
        prod.print(ss);
        std::string result;
        ss >> result;
        auto true_result = "519186934138915001194112299224174047254162";
        assert(result == true_result);
    }
    {
        LongInt lhs("11111111111111111111111111111111");
        LongInt rhs("3");
        LongInt prod = lhs * rhs;
        std::stringstream ss;
        prod.print(ss);
        std::string result;
        ss >> result;
        auto true_result = "33333333333333333333333333333333";
        assert(result == true_result);
    }
    {
        LongInt lhs("11111111111111111111111111111111");
        LongInt rhs("100000");
        LongInt prod = lhs * rhs;
        std::stringstream ss;
        prod.print(ss);
        std::string result;
        ss >> result;
        auto true_result = "1111111111111111111111111111111100000";
        assert(result == true_result);
    }
    {
        LongInt lhs("156468461846841612654498");
        LongInt rhs("-11684165165416541614654");
        LongInt prod = lhs * rhs;
        std::stringstream ss;
        prod.print(ss);
        std::string result;
        ss >> result;
        auto true_result = "-1828203351397173961590219912347471835955813692";
        assert(result == true_result);
    }
    {
        LongInt lhs("1828203351397173961590219912347471835955813692");
        LongInt rhs("0");
        LongInt prod = lhs * rhs;
        std::stringstream ss;
        prod.print(ss);
        std::string result;
        ss >> result;
        auto true_result = "0";
        assert(result == true_result);
    }
}

void test_random_big() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int64_t> numbers_generator(-1000000, 1000000);
    for (auto i = 0; i < 10000; ++i) {
        int64_t lhs = numbers_generator(rd);
        int64_t rhs = numbers_generator(rd);
        int64_t prod = lhs * rhs;
        LongInt lhs_long(std::to_string(lhs));
        LongInt rhs_long(std::to_string(rhs));
        LongInt prod_long = lhs_long * rhs_long;
        std::stringstream ss;
        prod_long.print(ss);
        std::string result;
        ss >> result;
        auto true_result = std::to_string(prod);
        assert(result == true_result);
    }
}

void test_random_small() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int64_t> numbers_generator(-20, 20);
    for (auto i = 0; i < 1000; ++i) {
        int64_t lhs = numbers_generator(rd);
        int64_t rhs = numbers_generator(rd);
        int64_t prod = lhs * rhs;
        LongInt lhs_long(std::to_string(lhs));
        LongInt rhs_long(std::to_string(rhs));
        LongInt prod_long = lhs_long * rhs_long;
        std::stringstream ss;
        prod_long.print(ss);
        std::string result;
        ss >> result;
        auto true_result = std::to_string(prod);
        assert(result == true_result);
    }
}

void test_random_long() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int64_t> numbers_generator(0, 9);
    for (auto i = 0; i < 100; ++i) {
        std::string lhs_str, rhs_str;
        if (numbers_generator(rd)>=5) {
            lhs_str += "-";
        }
        if (numbers_generator(rd)>=5) {
            rhs_str += "-";
        }
        for(auto j=0;j<10000;++j)
        {
            lhs_str += std::to_string(numbers_generator(rd));
            rhs_str += std::to_string(numbers_generator(rd));
        }
        LongInt lhs_long(lhs_str);
        LongInt rhs_long(rhs_str);
        LongInt prod_long = lhs_long * rhs_long;
        std::stringstream ss;
        prod_long.print(ss);
        std::string result;
        ss >> result;
        assert(!result.empty());
    }
}

void test_random_very_long() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int64_t> numbers_generator(0, 9);
    for (auto i = 0; i < 1; ++i) {
        std::string lhs_str, rhs_str;
        if (numbers_generator(rd)>=5) {
            lhs_str += "-";
        }
        if (numbers_generator(rd)>=5) {
            rhs_str += "-";
        }
        for(auto j=0;j<100000;++j)
        {
            lhs_str += std::to_string(numbers_generator(rd));
            rhs_str += std::to_string(numbers_generator(rd));
        }
        LongInt lhs_long(lhs_str);
        LongInt rhs_long(rhs_str);
        LongInt prod_long = lhs_long * rhs_long;
        std::stringstream ss;
        prod_long.print(ss);
        std::string result;
        ss >> result;
        assert(!result.empty());
    }
}

void run_all_tests() {
    test_from_task_1();
    test_from_task_2();
    test_one_symbol();
    test_two_symbols();
    test_zero();
    test_long();
    test_negative();
    test_zeros();

    test_random_big();
    test_random_small();
    test_random_long();
    test_random_very_long();
}

// Конец тестов

int main(int argc, char *argv[]) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    if (argc > 1) {
        if (std::string(argv[1]) == "test")  // запуск тестов
        {
            run_all_tests();
            return 0;
        }
    }

    // Решение задачи
    std::string lhs_str, rhs_str;
    std::cin >> lhs_str >> rhs_str;
    assert(!lhs_str.empty());
    assert(!rhs_str.empty());
    LongInt lhs(lhs_str);
    LongInt rhs(rhs_str);
    LongInt prod = lhs * rhs;
    prod.print(std::cout);

    return 0;
}
