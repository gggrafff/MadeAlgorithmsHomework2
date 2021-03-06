/*
 * D. Нечёткий поиск
 * ограничение по времени на тест 3 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Леонид работает в молодом и перспективном стартапе, занимающемся расшифровкой генома человека.
 * По долгу службы, ему приходится решать сложные задачи нахождения определённых паттернов в длинных строках,
 * состояющих из букв 'A', 'T', 'G' и 'C'.
 *
 * Рассмотрим следующую ситуацию.
 * Имеется фрагмент цепочки ДНК человека, записанный в виде строки S.
 * Для анализа фрагмента требуется найти все вхождения строки T в строку S.
 * Однако, дело осложняется тем, что в исходном фрагменте цепочки могли присутствовать незначительные мутации,
 * которые, тем не менее, усложняют задачу поиска фрагмента. Леонид предложил следующий подход решения этой проблемы.
 *
 * Зафиксируем целое число k ≥ 0 — степень погрешности.
 * Будем говорить, что строка T имеет вхождение в строку S на позиции i (1 ≤ i ≤ |S| - |T| + 1),
 * если после прикладывания строки T в этой позиции, каждому символу строки T соответствует такой же символ в
 * строке S на расстоянии не более k.
 * Более формально, для любого j (1 ≤ j ≤ |T|) должен сущетвовать такой p (1 ≤ p ≤ |S|),
 * что |(i + j - 1) - p| ≤ k и S[p] = T[j].
 *
 * Например, в соответствии с данным определением, строка "ACAT" входит в строку "AGCAATTCAT" в позициях 2, 3 и 6.
 *
 * Обратите внимание, что при k = 0 данное определение превращается в обычное определения вхождения строки в строку.
 *
 * Помогите Леониду — посчитайте, в скольких позициях данная строка T входит в данную строку S при
 * заданной степени погрешности.
 *
 * Входные данные
 * В первой строке следуют три целых числа |S|, |T|, k (1 ≤ |T| ≤ |S| ≤ 200 000, 0 ≤ k ≤ 200 000) — длины
 * строк S и T и степень погрешности.
 * Во второй строке записана строка S.
 * В третьей строке записана строка T.
 * Обе строки состоят только из прописных букв 'A', 'T', 'G' и 'C'.
 *
 * Выходные данные
 * Выведите единственное число — количество вхождений T в S со степенью погрешности k по данному определению.
 *
 * Примеры
 * Входные данные
 * 10 4 1
 * AGCAATTCAT
 * ACAT
 * Выходные данные
 * 3
 * Примечание
 * Если вы вдруг разбираетесь в строении генома человека чуть лучше, чем автор задачи, и вы не прониклись
 * оригинальным подходом Леонида, не воспринимайте всё описанное выше всерьёз.
 */

/*
 * Решение этой задачи - это слегка допиленное решение предыдущей задачи.
 * При рассмотрении отдельных букв ставим единицы в векторах не только в позиции буквы,
 * но и на k позиций влево и вправо.
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
#include <queue>


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
     * @return Позиции всех вхождений подстроки в строку.
     */
    static std::vector<size_t> find_substring(const std::string &text, const std::string &pattern) {
        auto matches = calculate_matches(text, pattern);
        std::vector<size_t> result;
        auto match = std::find(matches.begin(), matches.end(), 0);
        while (match != matches.end()) {
            auto idx = std::distance(matches.begin(), match);
            assert(idx >= 0);
            result.push_back(idx);
            match = std::find(match + 1, matches.end(), 0);
        }
        return result;
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
    static std::vector<size_t> reverse_bits_permutation;

    // Найдём нужную перестановку.
    if (reverse_bits_permutation.size() != samples.size()) {
        reverse_bits_permutation.resize(samples.size());
        size_t i = 0;
        std::iota(reverse_bits_permutation.begin(), reverse_bits_permutation.end(), i++);
        sort(reverse_bits_permutation.begin(), reverse_bits_permutation.end(),
             [&](size_t i, size_t j) { return reverse_bits(i) < reverse_bits(j); });
    }

    // Пересортируем элементы
    std::vector<std::complex<double>> result;
    result.reserve(samples.size());
    for (auto idx: reverse_bits_permutation) {
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

size_t solve_task(const std::string &text, std::string pattern, size_t k) {
    if (text.size() < pattern.size()) {
        return 0;
    }
    if (text.size() == pattern.size())
    {
        return static_cast<long>(text == pattern);
    }
    size_t pattern_length = pattern.size();

    // С помощью fft мы умеем вычислять свёртку.
    // Чтобы вычислить КФ с помощью операции свёртки, нужно развернуть один из операндов.
    std::reverse(pattern.begin(), pattern.end());
    pattern.resize(text.size());

    // Будем отдельно подсчитывать совпадения для разных букв.
    // Сначала найдём, в каких позициях встречается каждая из букв
    std::queue<int64_t> indexes_a, indexes_c, indexes_g, indexes_t;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == 'A') {
            indexes_a.push(i);
        }
        if (text[i] == 'C') {
            indexes_c.push(i);
        }
        if (text[i] == 'G') {
            indexes_g.push(i);
        }
        if (text[i] == 'T') {
            indexes_t.push(i);
        }
    }
    // Теперь создадим массивы с отсчётами сигналов, соответствующих каждой из букв.
    // В позициях, в которых встречаются буквы, а также на k позиций влево и вправо
    // поставим значения 1.0, а в остальных местах 0.0.
    std::vector<double>
            text_a(text.size(), 0.0),
            text_c(text.size(), 0.0),
            text_g(text.size(), 0.0),
            text_t(text.size(), 0.0);
    std::vector<double>
            pattern_a(pattern.size(), 0.0),
            pattern_c(pattern.size(), 0.0),
            pattern_g(pattern.size(), 0.0),
            pattern_t(pattern.size(), 0.0);
    for (int64_t i = 0; i < text.size(); ++i) {
        while (!indexes_a.empty() && (i>indexes_a.front() + k))
        {
            indexes_a.pop();
        }
        while (!indexes_c.empty() && (i>indexes_c.front() + k))
        {
            indexes_c.pop();
        }
        while (!indexes_g.empty() && (i>indexes_g.front() + k))
        {
            indexes_g.pop();
        }
        while (!indexes_t.empty() && (i>indexes_t.front() + k))
        {
            indexes_t.pop();
        }
        if (!indexes_a.empty() &&
                (i>=(indexes_a.front() - k)) &&
                (i<=(indexes_a.front() + k)))
        {
            text_a[i] = 1.0;
        }
        if (!indexes_c.empty() &&
                (i>=(indexes_c.front() - k)) &&
                (i<=(indexes_c.front() + k)))
        {
            text_c[i] = 1.0;
        }
        if (!indexes_g.empty() &&
                (i>=(indexes_g.front() - k)) &&
                (i<=(indexes_g.front() + k)))
        {
            text_g[i] = 1.0;
        }
        if (!indexes_t.empty() &&
                (i>=(indexes_t.front() - k)) &&
                (i<=(indexes_t.front() + k)))
        {
            text_t[i] = 1.0;
        }
    }
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (pattern[i] == 'A') {
            pattern_a[i] = 1.0;
        }
        if (pattern[i] == 'C') {
            pattern_c[i] = 1.0;
        }
        if (pattern[i] == 'G') {
            pattern_g[i] = 1.0;
        }
        if (pattern[i] == 'T') {
            pattern_t[i] = 1.0;
        }
    }

    // Вычислим все спектры
    auto[spectrum_text_a, spectrum_text_c] = fft::double_transfrom(text_a, text_c);
    auto[spectrum_text_g, spectrum_text_t] = fft::double_transfrom(text_g, text_t);
    auto[spectrum_pattern_a, spectrum_pattern_c] = fft::double_transfrom(pattern_a, pattern_c);
    auto[spectrum_pattern_g, spectrum_pattern_t] = fft::double_transfrom(pattern_g, pattern_t);

    // Просуммируем спектры свёрток
    std::vector<std::complex<double>> sum_spectrums;
    sum_spectrums.reserve(spectrum_text_a.size());
    for (size_t i = 0; i < spectrum_text_a.size(); ++i) {
        sum_spectrums.push_back(
                spectrum_text_a[i] * spectrum_pattern_a[i] +
                        spectrum_text_c[i] * spectrum_pattern_c[i] +
                        spectrum_text_g[i] * spectrum_pattern_g[i] +
                        spectrum_text_t[i] * spectrum_pattern_t[i]);
    }

    // Найдём сумму свёрток, произведя обратное преобразование
    auto corr_sum_samples = fft::transfrom(sum_spectrums, true);

    auto count = std::count_if(corr_sum_samples.begin(), corr_sum_samples.end(),
                         [pattern_length](const auto &elem) { return std::llround(elem.real()) == pattern_length; });
    assert(count >= 0);
    return count;
}

// Начало тестов

void test_from_task() {
    auto count = solve_task("AGCAATTCAT", "ACAT", 1);
    assert(count == 3);
}


void run_all_tests() {
    test_from_task();
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
    size_t length_text{0}, length_pattern{0}, k{0};
    std::cin >> length_text >> length_pattern >> k;
    std::string text, pattern;
    std::cin >> text >> pattern;
    auto count = solve_task(text, pattern, k);
    std::cout << count;

    return 0;
}
