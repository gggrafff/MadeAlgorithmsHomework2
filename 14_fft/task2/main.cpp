/*
 * B. Дуэль
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Двое дуэлянтов решили выбрать в качестве места проведения поединка тёмную аллею.
 * Вдоль этой аллеи растёт n деревьев и кустов. Расстояние между соседними объектами равно одному метру.
 * Дуэль решили проводить по следующим правилам. Некоторое дерево выбирается в качестве стартовой точки.
 * Затем два дерева, находящихся на одинаковом расстоянии от исходного, отмечаются как места для стрельбы.
 * Дуэлянты начинают движение от стартовой точки в противоположных направлениях.
 * Когда соперники достигают отмеченных деревьев, они разворачиваются и начинают стрелять друг в друга.
 *
 * Дана схема расположения деревьев вдоль аллеи.
 * Требуется определить количество способов выбрать стартовую точку и места для стрельбы согласно правилам дуэли.
 *
 * Входные данные
 * Во входном файле содержится одна строка, состоящая из символов '0' и '1' — схема аллеи.
 * Деревья обозначаются символом '1', кусты — символом '0'.
 * Длина строки не превосходит 100000 символов.
 *
 * Выходные данные
 * Выведите количество способов выбрать стартовую точку и места для стрельбы согласно правилам дуэли.
 *
 * Примеры
 * Входные данные
 * 101010101
 * Выходные данные
 * 4
 *
 * Входные данные
 * 101001
 * Выходные данные
 * 0
 *
 * Примечание
 * В первом примере возможны следующие конфигурации дуэли (стартовое дерево и деревья для стрельбы выделены
 * жирным шрифтом): (1)0(1)0(1)0101, 10(1)0(1)0(1)01, 1010(1)0(1)0(1) и (1)010(1)010(1).
 */

/*
 * Подробнее о свёртке:
 * https://ru.wikipedia.org/wiki/Свёртка_(математический_анализ)
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
 * @brief Пространство имён, объединяющее работу с fft (реализация и применение).
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

    /**
     * @brief Произвести свёртку двух сигналов.
     * @tparam InputType Тип данных отсчётов входных сигналов.
     * @tparam OutputIntegerType Тип данных отсчётов свёртки. В данной реализации только целочисленный.
     * @param lhs Первая последовательность.
     * @param rhs Вторая последовательность.
     * @return Результат свёртки.
     */
    template<typename InputType, typename OutputIntegerType>
    static std::vector<OutputIntegerType>
    convolution(const std::vector<InputType> &lhs, const std::vector<InputType> &rhs) {
        std::vector<std::complex<double>> samples1(lhs.begin(), lhs.end()), samples2(rhs.begin(), rhs.end());

        size_t conv_length = samples1.size() + samples2.size() - 1;

        samples1.resize(conv_length);
        samples2.resize(conv_length);

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

        std::vector<OutputIntegerType> result;
        result.reserve(production_samples.size());
        for (const auto &sample: production_samples) {
            result.emplace_back(std::llround(sample.real()));
        }
        result.resize(conv_length);
        return result;
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
    double x = static_cast<double>(number -
                                   1);  // если убрать "- 1", то "не меньше" в описании нужно заменить на "больше"
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
 * @brief Функция, непосредственно решающая задачу. Ищет симметричные тройки единиц во входных данных.
 * @param bits Входные данные (массив true(1) и false(0)).
 * @return Количество интересующих симметричных троек единиц.
 */
size_t count_symmetrical_triples(const std::vector<bool> &bits) {
    /*
     * Произведём свёртку сигнала с самим собой.
     * В тех местах, где сигнал симметричен, мы получим большие значения свёртки.
     * Если значение свёртки >= 3, а в центре рассматриваемого кусочка сигнала стоит '1',
     * значит найдены симметричные тройки в этом кусочке сигнала.
     */
    if (bits.size() < 3) {
        return 0;
    }
    auto autoconv = fft::convolution<bool, int64_t>(bits, bits);
    size_t count = 0;
    for (size_t i = 2; i < autoconv.size() - 2; i += 2) {
        if (bits[i / 2]) {
            count += (autoconv[i] - 1) / 2;
        }
    }
    return count;
}

// Начало тестов

void test_from_task() {
    auto count = count_symmetrical_triples({1, 0, 1, 0, 1, 0, 1, 0, 1});
    assert(count == 4);
    count = count_symmetrical_triples({1, 0, 1, 0, 0, 1});
    assert(count == 0);
}

void test_simple() {
    auto count = count_symmetrical_triples({1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1});
    assert(count == 2);
    count = count_symmetrical_triples({0, 1, 0, 1, 0, 1, 0, 0});
    assert(count == 1);
}

void test_random() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> numbers_generator(0, 100);
    for (auto i = 0; i < 1000; ++i) {
        std::vector<bool> bits;
        auto length = numbers_generator(rd) + 1;
        bits.reserve(length);
        for (auto j = 0; j < length; ++j) {
            bits.push_back(numbers_generator(rd) >= 500);
        }
        auto count = count_symmetrical_triples(bits);
    }
}

void run_all_tests() {
    test_from_task();
    test_simple();
    test_random();
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
    std::string input;
    std::cin >> input;
    std::vector<bool> bits;
    for (auto c: input) {
        bits.push_back(c == '1');
    }
    auto count = count_symmetrical_triples(bits);
    std::cout << count;

    return 0;
}
