/*
 * E. Покрытие кругом
 * ограничение по времени на тест 2.0 с
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Задано N точек на плоскости.
 * Найдите круг минимального радиуса, который покрывает все заданные точки.
 *
 * Входные данные
 * В первой строке входного файла содержится число N — количество точек (1 ≤ N ≤ 100 000).
 * Далее содержатся координаты точек — пара целых чисел, не превосходящих по модулю 10^6.
 *
 * Выходные данные
 * В первой строке выходных данных выведите координаты центра. На следующей строке выведите одно число — радиус круга.
 * Ваш ответ будет считаться правильным, если его абсолютная или относительная погрешность не будет превышать 10^-6.
 *
 * Пример
 * Входные данные
 * 3
 * 0 2
 * 0 0
 * 2 0
 * Выходные данные
 * 1.00000000000000000000 1.00000000000000000000
 * 1.41421356237309514547
 */

/*
 * Подробнее о покрытии кругом:
 * https://ru.wikipedia.org/wiki/Задача_о_наименьшей_окружности
 * https://neerc.ifmo.ru/wiki/index.php?title=Минимальная_охватывающая_окружность_множества_точек
 * http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.46.1450&rep=rep1&type=pdf
 * https://cyberleninka.ru/article/n/o-dvuh-podhodah-k-resheniyu-odnoy-klassicheskoy-zadachi-vychislitelnoy-geometrii-1/viewer
 * Построение окружности по трём точкам:
 * http://algolist.manual.ru/maths/geom/equation/circle.php
 */

/*
 * Классы окружности, вектора, точки взял из первой задачи и чуть-чуть допилил, но ничего не выпиливал.
 * Поэтому некоторые методы могут присутствовать, но не использоваться.
 */

#include <random>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <array>
#include <utility>
#include <iomanip>
#include <optional>


/**
 * @brief Проверка на равенство чисел с плавающей точкой.
 * @tparam FloatType Тип чисел: float или double.
 * @param lhs Первое число.
 * @param rhs Второе число.
 * @param eps Точность проверки.
 * @return True, если числа равны, иначе - false.
 */
template<typename FloatType>
bool is_equal(FloatType lhs, FloatType rhs, FloatType eps = 1e-6) {
    return std::abs(lhs - rhs) < eps;
}

template<typename NumberType>
class Vector;

/**
 * @brief Класс, описывающий точку на плоскости.
 * @tparam NumberType Тип данных для хранения координат.
 */
template<typename NumberType>
struct Point {
public:
    /**
     * @brief Конструктор из пары координат.
     * @tparam NumberType2 Тип данных координат.
     * @param x Координата x.
     * @param y Координата y.
     */
    template<typename NumberType2>
    Point(NumberType2 x, NumberType2 y):x_(x), y_(y) {}

    /**
     * @brief Конструктор из вектора.
     * @tparam NumberType2 Тип данных координат вектора.
     * @param vector Вектор.
     */
    template<typename NumberType2>
    Point(const Vector<NumberType2> &vector):x_(vector.get_x()), y_(vector.get_y()) {}

    /**
     * @brief Конструктор из точки.
     * @tparam NumberType2 Тип данных координат точки.
     * @param point Точка.
     */
    template<typename NumberType2>
    Point(const Point<NumberType2> &point):x_(point.get_x()), y_(point.get_y()) {}

    /**
     * @brief Сравнение точек.
     * @param rhs Точка, с которой сравниваем.
     * @return True, если точки равны, иначе - false.
     */
    bool operator==(const Point<NumberType> &rhs) const {
        // ToDo: comparison for float types
        return (this->x_ == rhs.x_) && (this->y_ == rhs.y_);
    }

    /**
     * @brief Получить координату x.
     * @return Координата x.
     */
    NumberType get_x() const {
        return x_;
    }

    /**
     * @brief Получить координату y.
     * @return Координата y.
     */
    NumberType get_y() const {
        return y_;
    }

private:
    NumberType x_{0};
    NumberType y_{0};
};

/**
 * @brief Класс, описывающий вектор на плоскости.
 * @tparam NumberType Тип данных для хранения координат.
 */
template<typename NumberType>
class Vector {
public:
    /**
     * @brief Конструктор по двум точкам.
     * @tparam NumberType2 Тип данных координат точек.
     * @param begin Начало вектора.
     * @param end Конец вектора.
     */
    template<typename NumberType2>
    Vector(Point<NumberType2> begin, Point<NumberType2> end):
            end_(end.get_x() - begin.get_x(), end.get_y() - begin.get_y()) {}

    /**
     * @brief Конструктор по одной точке. Начало вектора в начале координат.
     * @tparam NumberType2 Тип данных координат конца вектора.
     * @param end Конец вектора.
     */
    template<typename NumberType2>
    Vector(Point<NumberType2> end) : end_(end) {}

    /**
     * @brief Умножение вектора на число с плавающей точкой.
     * @param rhs Число, на которое умножаем.
     * @return Вектор с изменённой длиной.
     */
    Vector<double> operator*(const double rhs) const {
        return Vector<double>(
                Point<double>{static_cast<double>(end_.get_x()) * rhs, static_cast<double>(end_.get_y()) * rhs});
    }

    /**
     * @brief Сложение векторов.
     * @param rhs Второе слагаемое.
     * @return Сумма векторов.
     */
    Vector<NumberType> operator+(const Vector<NumberType> &rhs) const {
        return Vector<double>(Point<NumberType>{end_.get_x() + rhs.end_.get_x(), end_.get_y() + rhs.end_.get_y()});
    }

    /**
     * @brief Поворот вектора на 90 градусов против часовой стрелки.
     * @return Перпендикулярный вектор.
     */
    Vector turn_ccw_90() const {
        return Vector(Point<NumberType>(-end_.get_y(), end_.get_x()));
    }

    /**
    * @brief Поворот вектора на 90 градусов по часовой стрелке.
    * @return Перпендикулярный вектор.
    */
    Vector turn_cw_90() const {
        return Vector(Point<NumberType>(end_.get_y(), -end_.get_x()));
    }

    /**
     * @brief Скалярный квадрат вектора. Квадрат длины вектора.
     * @return Скалярный квадрат вектора.
     */
    NumberType square() const {
        return end_.get_x() * end_.get_x() + end_.get_y() * end_.get_y();
    }

    /**
     * @brief Найти длину вектора.
     * @return Длина вектора.
     */
    double length() const {
        return std::sqrt(static_cast<double>(square()));
    }

    /**
     * Получить конец вектора.
     * @return Точка - конец вектора.
     */
    Point<NumberType> get_end() const {
        return end_;
    }

    /**
     * @brief Получить x координату конца вектора.
     * @return X координата конца вектора.
     */
    NumberType get_x() const {
        return end_.get_x();
    }

    /**
     * @brief Получить y координату конца вектора.
     * @return Y координата конца вектора.
     */
    NumberType get_y() const {
        return end_.get_y();
    }

private:
    Point<NumberType> end_{0, 0};
};

/**
 * @brief Класс, описывающий окружность.
 * @tparam NumberType Тип данных для хранения координат центра и радиуса.
 */
template<typename NumberType>
class Circle {
public:
    /**
     * @brief Структура, описывающая пересечение окружностей.
     */
    struct IntersectionWithCircle {
        /**
         * @brief Характер пересечения.
         */
        enum Type {
            NOT_INTERSECT,  // не пересекаются
            TOUCH,  // касаются в одной точке
            INTERSECTION,  // пересекаются в двух точках
            EQUAL  // совпадают полностью
        };
        /**
         * @brief Расстояние от центра первой окружности до пересечения отрезка, соединяющего центры, с отрезком, соединяющим точки пересечения окружностей.
         */
        double OH{0.0};
        /**
         * @brief Расстояние от точки пересечения отрезка, соединяющего центры, с отрезком, соединяющим точки пересечения окружностей, до любой из точек пересечения окружностей.
         */
        double HP{0.0};
        /**
         * @brief Точка пересечения отрезка, соединяющего центры, с отрезком, соединяющим точки пересечения окружностей.
         */
        Point<double> H{0.0, 0.0};
        /**
         * @brief Точки пересечения. Могут отсутствовать.
         */
        std::array<std::optional<Point<double>>, 2> points{std::nullopt, std::nullopt};
        Type type{NOT_INTERSECT};  // Характер пересечения.
    };

    /**
     * @brief Конструктор окружности из центра и радиуса.
     * @param center Точка - центр окружности.
     * @param radius Радиус окружности.
     */
    Circle(Point<NumberType> center, NumberType radius) : g_{rd_()}, center_(center), radius_(radius) {}

    /**
     * @brief Строит минимальную окружность, охватывающую все заданные точки.
     * @tparam NumberType2 Тип данных координат точек.
     * @param covered Массив точек, которые должна покрывать окружность.
     */
    template<typename NumberType2>
    Circle(std::vector<Point<NumberType2>> covered): g_{rd_()}, center_{0, 0}, radius_{0} {
        assert(covered.size() >= 2);

        /*
         * Идея метода в том, что он генерирует случайную перестановку из точек, которые поступили на вход и в
         * получившимся порядке по одной добавляет их в текущую охватывающую окружность.
         */

        std::shuffle(covered.begin(), covered.end(), g_);  // генерируем перестановку

        encircle(covered[0], covered[1]);  // строим окружность по двум точкам

        for (size_t i = 2; i < covered.size(); ++i) {
            if (!contain(covered[i])) {  // если новая точка не лежит в окружности
                /*
                 * Если на каком-то шаге алгоритма текущая точка оказалась вне текущей минимальной окружности,
                 * то утверждается, что она лежит на границе новой окружности,
                 * которая будет содержать в себе ее и все предыдущие точки.
                 */
                cover_with_point({covered.begin(), covered.begin() + i}, covered[i]);  // строим окружность по всем просмотренным точкам
            }
        }
    }

    /**
     * @brief Лежит ли точка внутри окружности?
     * @tparam NumberType2 Тип данных координат точки.
     * @param point Проверяемая точка.
     * @return True, если точка внутри окружности или на границе.
     */
    template<typename NumberType2>
    bool contain(const Point<NumberType2> &point) {
        // Если расстояние от центра до точки меньше радиуса, то точка внутри окружности
        return (center_.get_x() - point.get_x()) * (center_.get_x() - point.get_x()) +
               (center_.get_y() - point.get_y()) * (center_.get_y() - point.get_y()) <= radius_ * radius_;
    }

    /**
     * @brief Проверка равенства окружностей.
     * @param rhs Окружность, с которой сравниваем.
     * @return True, если окружности равны.
     */
    bool operator==(const Circle<NumberType> &rhs) const {
        // ToDo: comparison for float types
        return (this->center_ == rhs.center_) && (this->radius_ == rhs.radius_);
    }

    /**
     * @brief Найти пересечение окружностей.
     * @param rhs Окружность, с которой пересекаем текущую.
     * @return Описание пересечения.
     */
    IntersectionWithCircle calculate_intersection(const Circle &rhs) {
        // Проверим окружности на равенство
        if (*this == rhs) {
            return IntersectionWithCircle{0.0, 0.0, {0.0, 0.0}, {std::nullopt, std::nullopt},
                                          IntersectionWithCircle::EQUAL};
        }

        // Определим два вектора, с которыми будем далее работать
        Vector<NumberType> center_to_center(this->center_, rhs.center_);
        auto perpendicular = center_to_center.turn_ccw_90();

        // Введём сокращения для удобства работы.
        auto r0 = this->radius_;
        auto r0_square = r0 * r0;
        auto r1 = rhs.radius_;
        auto r1_square = r1 * r1;
        auto c2c_square = center_to_center.square();

        // Для начала найдем расстояние между центрами окружностей. d = || P1 - P0 ||.
        // Если d > r0 + r1, тогда решений нет: круги лежат отдельно.
        // Аналогично в случае d < || r0 - r1 || - тогда нет решений, так как одна окружность находится внутри другой.
        if (c2c_square > (r0 + r1) * (r0 + r1)) {
            return IntersectionWithCircle{0.0, 0.0, {0.0, 0.0}, {std::nullopt, std::nullopt},
                                          IntersectionWithCircle::NOT_INTERSECT};
        }
        if (c2c_square < (r0 - r1) * (r0 - r1)) {
            return IntersectionWithCircle{0.0, 0.0, {0.0, 0.0}, {std::nullopt, std::nullopt},
                                          IntersectionWithCircle::NOT_INTERSECT};
        }

        // Имеем a^2 + h^2 = r0^2 and b^2 + h^2 = r1^2
        //  Используя равенство d = a + b, мы можем разрешить относительно a: a = (r0^2 - r1^2 + d^2 ) / (2*d)
        auto OH = (r0_square - r1_square + c2c_square) / (2 * center_to_center.length());
        Point<double> H = Vector<double>(this->center_) + center_to_center * (OH / center_to_center.length());
        // Если окружности касаются
        if ((c2c_square == (r0 + r1) * (r0 + r1)) || (c2c_square == (r0 - r1) * (r0 - r1))) {
            return IntersectionWithCircle{
                    std::abs(OH),
                    0.0, H,
                    {H, std::nullopt},
                    IntersectionWithCircle::TOUCH};
        }

        // Окружности пересекаются
        double HP = std::sqrt(r0_square - OH * OH);
        return IntersectionWithCircle{
                std::abs(OH),
                HP,
                H,
                {Vector<double>(H) + perpendicular * (HP / perpendicular.length()),
                 Vector<double>(H) + perpendicular * (-HP / perpendicular.length())},
                IntersectionWithCircle::INTERSECTION};
    }

    /**
     * @brief Получить центр окружности.
     * @return Точка - центр окружности.
     */
    const Point<NumberType> &get_center() const {
        return center_;
    }

    /**
     * @brief Получить радиус окружности
     * @return Радиус окружности.
     */
    NumberType get_radius() const {
        return radius_;
    }

private:

    /**
     * @brief Перестроить минимальную окружность, чтобы она покрыла новую точку, не потеряв покрытие старых.
     * @tparam NumberType2 Тип данных координат точек.
     * @param covered Уже покрытые точки.
     * @param point Непокрытая точка.
     */
    template<typename NumberType2>
    void cover_with_point(std::vector<Point<NumberType2>> covered, Point<NumberType2> point) {
        assert(covered.size() >= 1);

        std::shuffle(covered.begin(), covered.end(), g_);

        // Выделенная точка (которая передается в качестве второго аргумента) обязательно лежит на окружности.
        encircle(covered[0], point);

        for (size_t i = 1; i < covered.size(); ++i) {
            if (!contain(covered[i])) {
                cover_with_two_points({covered.begin(), covered.begin() + i}, covered[i], point);
            }
        }
    }

    /**
     * @brief Перестроить минимальную окружность, чтобы она покрыла две новые точки, не потеряв покрытие старых.
     * @tparam NumberType2 Тип данных координат точек.
     * @param covered Уже покрытые точки.
     * @param point1 Первая непокрытая точка
     * @param point2 Вторая непокрытая точка
     */
    template<typename NumberType2>
    void cover_with_two_points(std::vector<Point<NumberType2>> covered, Point<NumberType2> point1,
                               Point<NumberType2> point2) {
        assert(covered.size() >= 1);

        encircle(point1, point2);  // Две точки, которые будут лежать на построенной окружности.

        for (size_t i = 0; i < covered.size(); ++i) {
            if (!contain(covered[i])) {
                // Если три точки лежат на некоторой окружности, то они однозначно ее задают.
                encircle(covered[i], point1, point2);
            }
        }
    }

    /**
     * @brief Строит минимальную окружность по двум точкам.
     * @tparam NumberType2 Тип данных координат точек.
     * @param point1 Первая точка.
     * @param point2 Вторая точка.
     */
    template<typename NumberType2>
    void encircle(Point<NumberType2> point1, Point<NumberType2> point2) {
        auto diameter = Vector<NumberType2>(point1, point2);
        radius_ = diameter.length() / 2.0;
        center_ = Vector<double>(point1) + diameter * 0.5;
    }

    /**
     * @brief Строит окружность по трём точкам.
     * @tparam NumberType2 Тип данных координат точек.
     * @param point1 Первая точка.
     * @param point2 Вторая точка.
     * @param point3 Третья точка.
     */
    template<typename NumberType2>
    void encircle(Point<NumberType2> point1, Point<NumberType2> point2, Point<NumberType2> point3) {
        // Чтобы не допустить 0 в знаменателе в выражениях ниже, обработаем некоторые ситуации отдельно.

        // Если какая-то из координат у всех точек совпадает, то строим по двум крайним точкам.
        if ((point1.get_x() == point2.get_x()) && (point2.get_x() == point3.get_x())) {
            auto cmp = [](const Point<NumberType2>& lhs, const Point<NumberType2>& rhs){
                return lhs.get_x() < rhs.get_x();
            };
            auto left_point = std::min(point1, std::min(point2, point3, cmp), cmp);
            auto right_point = std::max(point1, std::max(point2, point3, cmp), cmp);
            encircle(left_point, right_point);
            return;
        }

        if ((point1.get_y() == point2.get_y()) && (point2.get_y() == point3.get_y())) {
            auto cmp = [](const Point<NumberType2>& lhs, const Point<NumberType2>& rhs){
                return lhs.get_y() < rhs.get_y();
            };
            auto bottom_point = std::min(point1, std::min(point2, point3, cmp), cmp);
            auto top_point = std::max(point1, std::max(point2, point3, cmp), cmp);
            encircle(bottom_point, top_point);
            return;
        }

        // В качестве точки 2 выбираем ту, у которой отличается координата x.
        if (point2.get_x() == point1.get_x()) {
            std::swap(point2, point3);
        } else if (point2.get_x() == point3.get_x()) {
            std::swap(point2, point1);
        }

        assert(point2.get_x() != point1.get_x());
        assert(point2.get_x() != point3.get_x());

        /*
         * Треугольник P1P2P3 вписан в данную окружность.
         * Центр вписанной окружности расположен в точке пересечения серединных перпендикуляров к сторонам треугольника.
         * Следовательно, для построения окружности по трём точкам достаточно:
         * 1) Найти координаты середины отрезка P1P2 (пусть это будет точка M) и составить уравнение
         *      серединного перпендикуляра a к отрезку P1P2 по точке M и вектору нормали P1P2.
         * 2) Аналогично найти координаты середины отрезка P2P3 (пусть это будет точка K) и составить уравнение
         *      серединного перпендикуляра b к отрезку P2P3 по точке K и вектору нормали P2P3.
         * 3) Найти координаты точки пересечения серединных перпендикуляров a и b,
         *      решив соответствующую систему двух линейных уравнений.
         */

        // Коэффициенты наклона прямых P1P2 и P2P3
        double tan_ox_1 = static_cast<double>(point2.get_y() - point1.get_y()) /
                          static_cast<double>(point2.get_x() - point1.get_x());
        double tan_ox_2 = static_cast<double>(point3.get_y() - point2.get_y()) /
                          static_cast<double>(point3.get_x() - point2.get_x());

        assert(tan_ox_1 != tan_ox_2);

        // Ищем пересечение прямых, выражаем x:
        double x_center = (tan_ox_1 * tan_ox_2 * static_cast<double>(point1.get_y() - point3.get_y()) +
                           tan_ox_2 * static_cast<double>(point1.get_x() + point2.get_x()) -
                           tan_ox_1 * static_cast<double>(point2.get_x() + point3.get_x())) / 2.0 / (tan_ox_2 - tan_ox_1);

        // Подставляем в уравнение прямой и находим y:
        double y_center = -1.0 / tan_ox_1 * (x_center - static_cast<double>(point1.get_x() + point2.get_x()) / 2.0) +
                          static_cast<double>(point1.get_y() + point2.get_y()) / 2.0;

        center_ = Point<NumberType>(x_center, y_center);
        radius_ = std::sqrt((point1.get_x() - x_center) * (point1.get_x() - x_center) +
                            (point1.get_y() - y_center) * (point1.get_y() - y_center));

        assert(center_ == center_);
        assert(radius_ == radius_);
    }

    Point<NumberType> center_;
    NumberType radius_;

    std::random_device rd_;
    std::mt19937 g_;
};


// Начало тестов

void test_from_task() {
    std::vector<Point<int64_t>> points{
            {0, 2},
            {0, 0},
            {2, 0}
    };
    Circle<double> circle(points);
    auto center = circle.get_center();
    assert(is_equal(center.get_x(), 1.0, 1e-6));
    assert(is_equal(center.get_y(), 1.0, 1e-6));
    assert(is_equal(circle.get_radius(), 1.41421356237309514547, 1e-6));
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
    size_t N{0};
    std::cin >> N;
    std::vector<Point<int64_t>> points{
    };
    for (size_t i = 0; i < N; ++i) {
        int64_t x{0}, y{0};
        std::cin >> x >> y;
        points.emplace_back(x, y);
    }
    Circle<double> circle(points);
    auto center = circle.get_center();
    std::cout << std::setprecision(10) << std::fixed;
    std::cout << center.get_x() << " " << center.get_y() << std::endl;
    std::cout << circle.get_radius();

    return 0;
}
