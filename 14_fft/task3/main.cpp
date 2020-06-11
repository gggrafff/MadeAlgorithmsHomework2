/*
 * C. ДНК Роботов
 *
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Последние достижения в технологии синтеза ДНК позволили провести эксперимент по созданию биороботов.
 *
 * Для облегчения задачи создания ПО для управления роботами было принято решение, что их ДНК будет состоять
 * из M=2n символов для некоторого n≥2. Кроме этого, по техническим причинам это будет не обычная строка,
 * а циклическая, то есть её можно начинать читать с любой позиции.
 *
 * Одной из целей эксперимента является изучение мутаций биороботов.
 * В результате продолжительных наблюдений было найдено много различных видов роботов.
 * Для понимания процесса мутации учёным необходимо решить следующую задачу.
 *
 * Для ДНК двух роботов требуется определить коэффициент их похожести.
 * Он вычисляется, как максимальное количество совпадающих символов при наилучшем совмещении этих ДНК.
 * Чем больше символов совпадает, тем лучше совмещение.
 *
 * Требуется написать программу, которая найдёт наилучшее совмещение двух ДНК.
 *
 * Входные данные
 * В первой строке входного файла записано одно число M(4≤M≤131072).
 * В следующих двух строках записаны ДНК двух роботов.
 * Обе ДНК — строки, состоящие ровно из M символов из множества {'A', 'C', 'G', 'T'}.
 *
 * Выходные данные
 * В выходной файл выведите два числа — максимальное количество совпадающих символов и значение оптимального
 * сдвига — неотрицательное количество символов второй ДНК, которые необходимо перенести из конца строки в
 * её начало для достижения наилучшего совмещения.
 *
 * Пример
 * Входные данные
 * 16
 * ACGTACGTACGTACGT
 * CGTACGTACGTACGTC
 *
 * Выходные данные
 * 15 1
 */

/*
 * Дополнительные материалы:
 * https://eduardgorbunov.github.io/assets/files/amc_778_seminar_08.pdf
 * https://cp-algorithms.com/algebra/fft.html
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
     * @brief Преобразование Фурье для двух вещественных сигналов.
     * @details Вещественные сигналы объединяются и производится комплексное преобразование.
     * @param x Первый сигнал.
     * @param y Второй сигнал.
     * @return Кортеж из двух спектров.
     */
    static std::tuple<std::vector<std::complex<double>>, std::vector<std::complex<double>>>
    double_transfrom(std::vector<double> x, std::vector<double> y) {
        using namespace std::complex_literals;

        x.resize(std::max(x.size(), y.size()));
        y.resize(std::max(x.size(), y.size()));

        // z(n) = x(n) + iy(n)
        // x(n) = (z(n) + z(n)∗)/2
        // y(n) = −i(z(n) − z(n)∗)/2
        std::vector<std::complex<double>> joined;
        for (size_t i = 0; i < x.size(); ++i) {
            joined.emplace_back(x[i], y[i]);
        }
        auto fft_joined = transfrom(joined);

        // FFT(x)[k] = (FFT(z)[k] + FFTN(z)[N − k]∗)/2
        // FFT(x)[k] = -i(FFT(z)[k] - FFTN(z)[N − k]∗)/2
        std::vector<std::complex<double>> fft_x, fft_y;
        fft_x.push_back((fft_joined[0] + std::conj(fft_joined[0])) / 2.0);
        fft_y.push_back(std::complex<double>(-1.0i) * (fft_joined[0] - std::conj(fft_joined[0])) / 2.0);

        size_t N = fft_joined.size();
        for (size_t i = 1; i < fft_joined.size(); ++i) {
            fft_x.push_back((fft_joined[i] + std::conj(fft_joined[N - i])) / 2.0);
            fft_y.push_back(std::complex<double>(-1.0i) * (fft_joined[i] - std::conj(fft_joined[N - i])) / 2.0);
        }
        return {fft_x, fft_y};
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

    /**
     * @brief Поиск подстроки в строке.
     * @param text Строка, в которой ищем.
     * @param pattern Подстрока, которую ищем.
     * @return Позиция первого вхождения, если подстрока найдена.
     */
    static std::optional<size_t> find_substring(const std::string &text, const std::string &pattern) {
        auto matches = calculate_matches(text, pattern);
        auto match = std::find(matches.begin(), matches.end(), 0);
        if (match == matches.end()) {
            return std::nullopt;
        }
        return std::distance(matches.begin(), match);
    }

    /**
     * @brief Циклическая корреляционная функция.
     * @details Скалярные произведения для всех циклических сдвигов строк друг относительно друга.
     * @tparam InputType Тип данных отсчётов входного сигнала.
     * @tparam OutputIntegerType Тип данных отсчётов корреляционной функции.
     * @param lhs Первый сигнал.
     * @param rhs Второй сигнал.
     * @return Корреляционная функция, вычисленная с циклическими сдвигами.
     */
    template<typename InputType, typename OutputIntegerType>
    static std::vector<OutputIntegerType> cyclic_correlation(std::vector<InputType> lhs, std::vector<InputType> rhs) {
        assert(lhs.size() == rhs.size());

        // Чтобы найти КФ, умея находить свёртку, нужно инвертировать порядок отсчётов в одном из сигналов.
        // Инвертируем первый массив и припишем к нему в конец n нулей.
        std::reverse(lhs.begin(), lhs.end());
        lhs.resize(lhs.size() * 2, 0);
        // А ко второму массиву просто припишем самого себя, чтобы имитировать циклические сдвиги.
        rhs.insert(rhs.end(), rhs.begin(), rhs.end());

        auto conv = convolution<InputType, OutputIntegerType>(lhs, rhs);

        return {next(conv.begin(), lhs.size() / 2 - 1), next(conv.begin(), 2 * lhs.size() / 2 - 1)};
    }

private:
    fft() = default;  // запретим создание экзампляров класса

    /**
     * @brief Вычисление вектора совпадений для поиска подстроки в строке.
     * @param text Строка, в которой ищем.
     * @param pattern Строка, которую ищем.
     * @return Вектор, в котором 0 в тех позициях, в которых подстрока входит в строку полностью.
     */
    static std::vector<size_t> calculate_matches(const std::string &text, std::string pattern) {
        /*
         * Идея БПФ-алгоритма поиска подстрок следующая.
         * Нужно найти подстроку (образец) p[0]..p[m-1] в строке (тексте) t[0]..t[n-1],
         * здесь p[i], t[j] - это символы некоторого алфавита.
         * Говорят, что подстрока входит с i-й позиции,если p[j] == t[i+j], j=0..m-1.
         * Если считать буквы алфавита различными целыми числами, то вхождение подстроки с i-й позиции эквивалентно
         * обнулению суммы квадратов: B[i]=sum{(p[j] - t[i+j])^2} = sum{p[j]^2 - 2 * p[j] * t[i+j] + t[i+j]^2} = 0,
         * а вычисление массива чисел B[i], i= 0..n-m позволяет определить все места вхождения подстроки в текст.
         */
        /*
         * Рассмотрим два полинома степени не выше n:
         * T(x) = t[n-1] * x^(n-1) + ... + t[1]*x + t[0],
         * P(x) = p[0] * x^(m-1) + ... + p[m-1]
         * Их произведение можно вычислить с помощью БПФ.
         */
        std::reverse(pattern.begin(), pattern.end());
        auto conv = convolution<char, int64_t>({text.begin(), text.end()}, {pattern.begin(), pattern.end()});
        /*
         * Посмотрим на коэффициент их произведения C(x) при x^(m-1+i) (0<=i<=n-m):
         * c[m-1+i] = sum(p[j] * t[j+i])
         * Как видим, это одно из слагаемых для B[i].
         * Таким образом, алгоритм для подсчета всех B[i] таков.
         * Сначала вычисляем коэффициенты произведения C(x) указанных многочленов.
         */
        /*
         * Вычисляем сумму квадратов S.
         * Сумма S = sum(p[j]^2) присутствует как слагаемое в каждом B[i].
         */
        auto s = std::accumulate(pattern.begin(), pattern.end(), 0,
                                 [](const auto &accum, const auto &b) { return accum + b * b; });
        /*
         * Считаем сумму первых m квадратов t[i], т.е. H = sum(t[i]^2, i=0..m-1)
         */
        auto h = std::accumulate(text.begin(), text.begin() + pattern.size(), 0,
                                 [](const auto &accum, const auto &b) { return accum + b * b; });
        std::vector<size_t> b;
        size_t current_symbol_idx = pattern.size() - 1;
        // Далее вычисляем B[0] = S - 2 * c[m-1] + H
        b.push_back(s - 2 * conv[current_symbol_idx] + h);
        current_symbol_idx += 1;
        while (current_symbol_idx < text.size()) {
            // Аналогично получим B[i] = B[i-1] + 2 * (c[m-2+i] - c[m-1+i]) − t[i-1]^2 + t[m-1+i]^2
            b.push_back(b.back() + 2 * conv[current_symbol_idx - 1] - 2 * conv[current_symbol_idx] -
                        text[b.size() - 1] * text[b.size() - 1] + text[current_symbol_idx] * text[current_symbol_idx]);
            current_symbol_idx += 1;
        }
        return b;
    }

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
 * @brief Поэлементное умножение двух массивов.
 * @tparam CollectionType Тип массивов.
 * @param lhs Первый массив.
 * @param rhs Второй массив.
 * @return Поэлементное произведение.
 */
template<typename CollectionType>
CollectionType piecemeal_multiplication(const CollectionType &lhs, const CollectionType &rhs) {
    assert(lhs.size() == rhs.size());
    CollectionType result;
    result.reserve(lhs.size());
    std::transform(
            lhs.begin(), lhs.end(),
            rhs.begin(),
            std::back_inserter(result),
            [](const auto &a, const auto &b) {
                return a * b;
            });
    return result;
}

/**
 * @brief Функция, непосредственно решающая задачу.
 * @param first Первая строка.
 * @param second Вторая строка.
 * @return Кортеж: количество совпавших символов, сдвиг.
 */
std::tuple<size_t, size_t> solve_task(std::string first, std::string second) {
    // С помощью fft мы умеем вычислять свёртку.
    // Чтобы вычислить КФ с помощью операции свёртки, нужно развернуть один из операндов.
    std::reverse(first.begin(), first.end());
    first.resize(first.size() * 2, 0);
    // Имитируем циклические сдвиги, продублировав строку.
    second.insert(second.end(), second.begin(), second.end());

    // Будем отдельно подсчитывать совпадения для разных букв.
    std::vector<double> first_only_a, first_only_c, first_only_g, first_only_t;
    std::vector<double> second_only_a, second_only_c, second_only_g, second_only_t;
    for (auto c: first) {
        first_only_a.push_back(c == 'A');
        first_only_c.push_back(c == 'C');
        first_only_g.push_back(c == 'G');
        first_only_t.push_back(c == 'T');
    }
    for (auto c: second) {
        second_only_a.push_back(c == 'A');
        second_only_c.push_back(c == 'C');
        second_only_g.push_back(c == 'G');
        second_only_t.push_back(c == 'T');
    }

    // Вычислим все спектры
    auto[spectrum_first_a, spectrum_first_c] = fft::double_transfrom(first_only_a, first_only_c);
    auto[spectrum_first_g, spectrum_first_t] = fft::double_transfrom(first_only_g, first_only_t);
    auto[spectrum_second_a, spectrum_second_c] = fft::double_transfrom(second_only_a, second_only_c);
    auto[spectrum_second_g, spectrum_second_t] = fft::double_transfrom(second_only_g, second_only_t);

    // Найдём спектры свёрток
    auto corr_a_spectrum = piecemeal_multiplication(spectrum_first_a, spectrum_second_a);
    auto corr_c_spectrum = piecemeal_multiplication(spectrum_first_c, spectrum_second_c);
    auto corr_g_spectrum = piecemeal_multiplication(spectrum_first_g, spectrum_second_g);
    auto corr_t_spectrum = piecemeal_multiplication(spectrum_first_t, spectrum_second_t);

    // Просуммируем спектры свёрток
    std::vector<std::complex<double>> sum_spectrums;
    for (size_t i = 0; i < corr_a_spectrum.size(); ++i) {
        sum_spectrums.push_back(corr_a_spectrum[i] + corr_c_spectrum[i] + corr_g_spectrum[i] + corr_t_spectrum[i]);
    }

    // Найдём сумму свёрток, произведя обратное преобразование
    auto corr_sum_samples = fft::transfrom(sum_spectrums, true);
    std::vector<int64_t> corr_sum;
    corr_sum.reserve(corr_sum_samples.size());
    for (const auto &sample: corr_sum_samples) {
        corr_sum.emplace_back(std::llround(sample.real()));
    }
    corr_sum = {next(corr_sum.begin(), first.size() / 2 - 1), next(corr_sum.begin(), 2 * first.size() / 2 - 1)};

    // Найдём максимальное совпадение и наилучший сдвиг
    auto max_corr = std::max_element(corr_sum.rbegin(), corr_sum.rend());
    auto shift = std::distance(corr_sum.rbegin(), max_corr) + 1;
    return {*max_corr, shift % (second.size() / 2)};
}


// Начало тестов


void test_full_match() {
    std::string first = "ACGTACGTACGTACGT";
    std::string second = "ACGTACGTACGTACGT";
    auto[max_corr1, shift1] = solve_task(first, second);
    assert(max_corr1 == 16);
    assert(shift1 % 4 == 0);
    assert(shift1 < 16);
    assert(first[shift1] == second[0]);
}

void test_match_with_shift() {
    std::string first = "ACGTACGTACGTACGT";
    std::string second = "CGTACGTACGTACGTA";
    auto[max_corr1, shift1] = solve_task(first, second);
    assert(max_corr1 == 16);
    assert(shift1 == 1);
    assert(first[shift1] == second[0]);
}


void test_from_task() {
    std::string first = "ACGTACGTACGTACGT";
    std::string second = "CGTACGTACGTACGTC";
    auto[max_corr1, shift1] = solve_task(first, second);
    assert(max_corr1 == 15);
    assert(shift1 == 1);
    assert(first[shift1] == second[0]);
}

void test_equals() {
    std::string first = "AAAA";
    std::string second = "AAAA";
    auto[max_corr1, shift1] = solve_task(first, second);
    assert(max_corr1 == 4);
}

void test_AC() {
    std::string first = "ACACAC";
    std::string second = "CACACA";
    auto[max_corr1, shift1] = solve_task(first, second);
    assert(max_corr1 == 6);
    assert(shift1 == 1);
}

void test_ACGT() {
    std::string first = "ACGT";
    std::string second = "GTAC";
    auto[max_corr1, shift1] = solve_task(first, second);
    assert(max_corr1 == 4);
    assert(shift1 == 2);
}

void test_without_shifting() {
    std::string first = "ACGT";
    std::string second = "ACGT";
    auto[max_corr1, shift1] = solve_task(first, second);
    assert(max_corr1 == 4);
    assert(shift1 == 0);
}

void test_AACAAC_GACAAC() {
    std::string first = "AACAAC";
    std::string second = "GACAAC";
    auto[max_corr1, shift1] = solve_task(first, second);
    assert(max_corr1 == 5);
    assert(shift1 == 3);
}

void test_random() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> numbers_generator(0, 100);
    for (auto i = 0; i < 100; ++i) {
        std::string first;
        auto length = numbers_generator(rd) % 3 + 1;
        while (first.size() < length) {
            switch (numbers_generator(rd) % 4) {
                case 0:
                    first += 'A';
                    break;
                case 1:
                    first += 'C';
                    break;
                case 2:
                    first += 'G';
                    break;
                case 3:
                    first += 'T';
                    break;
                default:
                    break;
            }
        }
        length = numbers_generator(rd) % 3 + 1;
        for (auto j = 0; j < length; ++j) {
            first.insert(first.end(), first.begin(), first.end());
        }
        auto shift = numbers_generator(rd) % first.size();
        std::string second = first.substr(shift);
        second += first.substr(0, shift);
        assert(first.size() == second.size());
        assert(first[shift] == second[0]);
        for (auto &c: second) {
            if (numbers_generator(rd) % 4 == 0) {
                switch (numbers_generator(rd) % 4) {
                    case 0:
                        c = 'A';
                        break;
                    case 1:
                        c = 'C';
                        break;
                    case 2:
                        c = 'G';
                        break;
                    case 3:
                        c = 'T';
                        break;
                    default:
                        break;
                }
            }
        }
        auto[max_corr1, shift1] = solve_task(first, second);
    }
}

void run_all_tests() {
    test_full_match();
    test_match_with_shift();
    test_from_task();
    test_equals();
    test_AC();
    test_ACGT();
    test_without_shifting();
    test_AACAAC_GACAAC();

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
    size_t M;
    std::string first;
    std::string second;
    std::cin >> M >> first >> second;
    auto[max_corr, shift] = solve_task(first, second);
    std::cout << max_corr << " " << shift;

    return 0;
}
