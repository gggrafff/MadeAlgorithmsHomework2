/*
 * A. Две окружности
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Вам заданы две окружности.
 * Необходимо выяснить, пересекаются ли заданные окружности и найти точки их пересечения.
 *
 * Входные данные
 * Первая строка файлов содержит количество тестов — целое число от 1 до 10 000.
 * Далее следуют тесты. Каждый тест имеет вид:
 * Две строки, по три числа в каждой — координаты центра и радиус сначала первой, а затем второй окружности.
 * Все числа — целые, по модулю не превосходящие 10 000. А радиус еще и положительный.
 *
 * Выходные данные
 * На каждый тест выведите ответ в следующем формате:
 * На отдельной строке выходного файла выведите количество точек пересечения (0, 1, 2 или 3, если их бесконечно много).
 * В случае одной точки пересечения выведите во второй строке координаты этой точки.
 * В случае двух точек пересечения выведите во второй строке координаты точки H, в третьей длины векторов OH и HP,
 *      в следующих двух строках должны находиться координаты точек пересечения.
 *      Эти две точки можно вывести в произвольном порядке.
 *
 * Пример
 * Входные данные
 * 4
 * 3 4 5
 * 11 4 2
 * 3 4 5
 * 11 4 3
 * 3 4 5
 * 11 4 4
 * 3 4 5
 * 3 4 5
 * Выходные данные
 * 0
 * 1
 * 8.0000000000 4.0000000000
 * 2
 * 7.5625000000 4.0000000000
 * 4.5625000000 2.0453835215
 * 7.5625000000 6.0453835215
 * 7.5625000000 1.9546164785
 * 3
 *
 * Примечание
 * Ответ однозначен.
 * Координаты и длины нужно вывести с не менее чем 6-ю верными знаками после запятой.
 * Заметим, что все входные данные — целые числа, поэтому величина "сколько точек пересечения" может быть определена точно.
 * https://espresso.codeforces.com/b7965bec846abccbcf9fc695c18afd3ccb1c4b77.png
 */

/*
 * Подробнее о пересечении окружностей:
 * https://neerc.ifmo.ru/wiki/index.php?title=Пересечение_окружностей - тут что-то неправильно, похоже
 * https://e-maxx.ru/algo/circles_intersection
 * http://algolist.manual.ru/maths/geom/intersect/circlecircle2d.php - тут норм
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
bool is_equal(FloatType lhs, FloatType rhs, FloatType eps=1e-6) {
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
        return Vector<double>(Point<double>{static_cast<double>(end_.get_x()) * rhs, static_cast<double>(end_.get_y()) * rhs});
    }

    /**
     * @brief Сложение векторов.
     * @param rhs Второе слагаемое.
     * @return Сумма векторов.
     */
    Vector<NumberType> operator+(const Vector<NumberType> &rhs) const {
        return Vector<double>(Point<NumberType>{end_.get_x() + rhs.end_.get_x(),end_.get_y() + rhs.end_.get_y()});
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
    Circle(Point<NumberType> center, NumberType radius) : center_(center), radius_(radius) {};

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

private:
    Point<NumberType> center_;
    NumberType radius_;
};



// Начало тестов

void test_from_task() {
    Circle<int32_t> circle1(Point<int32_t>{3, 4}, 5);
    Circle<int32_t> circle2(Point<int32_t>{11, 4}, 2);
    auto intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::NOT_INTERSECT);

    circle1 = Circle<int32_t>(Point<int32_t>{3, 4}, 5);
    circle2 = Circle<int32_t>(Point<int32_t>{11, 4}, 3);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::TOUCH);
    assert(intersection.points[0].has_value());
    assert(!intersection.points[1].has_value());
    assert(is_equal(8.0, intersection.points[0].value().get_x(), 1e-6));
    assert(is_equal(4.0, intersection.points[0].value().get_y(), 1e-6));

    circle1 = Circle<int32_t>(Point<int32_t>{3, 4}, 5);
    circle2 = Circle<int32_t>(Point<int32_t>{11, 4}, 4);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::INTERSECTION);
    assert(is_equal(7.5625000000, intersection.H.get_x(), 1e-6));
    assert(is_equal(4.0000000000, intersection.H.get_y(), 1e-6));
    assert(is_equal(4.5625000000, intersection.OH, 1e-6));
    assert(is_equal(2.0453835215, intersection.HP, 1e-6));
    assert(intersection.points[0].has_value());
    assert(is_equal(7.5625000000, intersection.points[0].value().get_x(), 1e-6));
    assert(is_equal(6.0453835215, intersection.points[0].value().get_y(), 1e-6));
    assert(intersection.points[1].has_value());
    assert(is_equal(7.5625000000, intersection.points[1].value().get_x(), 1e-6));
    assert(is_equal(1.9546164785, intersection.points[1].value().get_y(), 1e-6));

    circle1 = Circle<int32_t>(Point<int32_t>{3, 4}, 5);
    circle2 = Circle<int32_t>(Point<int32_t>{3, 4}, 5);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::EQUAL);


    circle1 = Circle<int32_t>(Point<int32_t>{11, 4}, 2);
    circle2 = Circle<int32_t>(Point<int32_t>{3, 4}, 5);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::NOT_INTERSECT);

    circle1 = Circle<int32_t>(Point<int32_t>{11, 4}, 3);
    circle2 = Circle<int32_t>(Point<int32_t>{3, 4}, 5);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::TOUCH);
    assert(intersection.points[0].has_value());
    assert(!intersection.points[1].has_value());
    assert(is_equal(8.0, intersection.points[0].value().get_x(), 1e-6));
    assert(is_equal(4.0, intersection.points[0].value().get_y(), 1e-6));

    circle1 = Circle<int32_t>(Point<int32_t>{11, 4}, 4);
    circle2 = Circle<int32_t>(Point<int32_t>{3, 4}, 5);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::INTERSECTION);
    assert(is_equal(7.5625000000, intersection.H.get_x(), 1e-6));
    assert(is_equal(4.0000000000, intersection.H.get_y(), 1e-6));
    assert(is_equal(8.0 - 4.5625000000, intersection.OH, 1e-6));
    assert(is_equal(2.0453835215, intersection.HP, 1e-6));
    assert(intersection.points[0].has_value());
    assert(is_equal(7.5625000000, intersection.points[0].value().get_x(), 1e-6));
    assert(is_equal(6.0453835215, intersection.points[0].value().get_y(), 1e-6) || is_equal(1.9546164785, intersection.points[0].value().get_y(), 1e-6));
    assert(intersection.points[1].has_value());
    assert(is_equal(7.5625000000, intersection.points[1].value().get_x(), 1e-6));
    assert(is_equal(6.0453835215, intersection.points[1].value().get_y(), 1e-6) || is_equal(1.9546164785, intersection.points[1].value().get_y(), 1e-6));

    circle1 = Circle<int32_t>(Point<int32_t>{3, 4}, 5);
    circle2 = Circle<int32_t>(Point<int32_t>{3, 4}, 5);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::EQUAL);
}

void test_internal() {
    Circle<int32_t> circle1(Point<int32_t>{0, 0}, 6);
    Circle<int32_t> circle2(Point<int32_t>{0, 0}, 5);
    auto intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::NOT_INTERSECT);

    circle1 = Circle<int32_t>(Point<int32_t>{0, 0}, 6);
    circle2 = Circle<int32_t>(Point<int32_t>{0, 0}, 7);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::NOT_INTERSECT);

    circle1 = Circle<int32_t>(Point<int32_t>{0, 0}, 6);
    circle2 = Circle<int32_t>(Point<int32_t>{1, 1}, 3);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::NOT_INTERSECT);

    circle1 = Circle<int32_t>(Point<int32_t>{1, 1}, 3);
    circle2 = Circle<int32_t>(Point<int32_t>{0, 0}, 6);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::NOT_INTERSECT);

    circle1 = Circle<int32_t>(Point<int32_t>{0, 0}, 6);
    circle2 = Circle<int32_t>(Point<int32_t>{5, 0}, 1);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::TOUCH);
    assert(intersection.points[0].has_value());
    assert(!intersection.points[1].has_value());
    assert(is_equal(6.0, intersection.points[0].value().get_x(), 1e-6));
    assert(is_equal(0.0, intersection.points[0].value().get_y(), 1e-6));

    circle1 = Circle<int32_t>(Point<int32_t>{5, 0}, 1);
    circle2 = Circle<int32_t>(Point<int32_t>{0, 0}, 6);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::TOUCH);
    assert(intersection.points[0].has_value());
    assert(!intersection.points[1].has_value());
    assert(is_equal(6.0, intersection.points[0].value().get_x(), 1e-6));
    assert(is_equal(0.0, intersection.points[0].value().get_y(), 1e-6));

    circle1 = Circle<int32_t>(Point<int32_t>{0, 0}, 6);
    circle2 = Circle<int32_t>(Point<int32_t>{5, 0}, 2);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::INTERSECTION);
    assert(intersection.points[0].has_value());
    assert(intersection.points[1].has_value());
}

void test_negative() {
    Circle<int32_t> circle1(Point<int32_t>{-7, -6}, 5);
    Circle<int32_t> circle2(Point<int32_t>{1, -6}, 2);
    auto intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::NOT_INTERSECT);

    circle1 = Circle<int32_t>(Point<int32_t>{-7, -6}, 5);
    circle2 = Circle<int32_t>(Point<int32_t>{1, -6}, 3);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::TOUCH);
    assert(intersection.points[0].has_value());
    assert(!intersection.points[1].has_value());
    assert(is_equal(8.0 - 10.0, intersection.points[0].value().get_x(), 1e-6));
    assert(is_equal(4.0 - 10.0, intersection.points[0].value().get_y(), 1e-6));

    circle1 = Circle<int32_t>(Point<int32_t>{-7, -6}, 5);
    circle2 = Circle<int32_t>(Point<int32_t>{1, -6}, 4);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::INTERSECTION);
    assert(is_equal(7.5625000000 - 10.0, intersection.H.get_x(), 1e-6));
    assert(is_equal(4.0000000000 - 10.0, intersection.H.get_y(), 1e-6));
    assert(is_equal(4.5625000000, intersection.OH, 1e-6));
    assert(is_equal(2.0453835215, intersection.HP, 1e-6));
    assert(intersection.points[0].has_value());
    assert(is_equal(7.5625000000 - 10.0, intersection.points[0].value().get_x(), 1e-6));
    assert(is_equal(6.0453835215 - 10.0, intersection.points[0].value().get_y(), 1e-6));
    assert(intersection.points[1].has_value());
    assert(is_equal(7.5625000000 - 10.0, intersection.points[1].value().get_x(), 1e-6));
    assert(is_equal(1.9546164785 - 10.0, intersection.points[1].value().get_y(), 1e-6));

    circle1 = Circle<int32_t>(Point<int32_t>{-7, -6}, 5);
    circle2 = Circle<int32_t>(Point<int32_t>{-7, -6}, 5);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::EQUAL);

    circle1 = Circle<int32_t>(Point<int32_t>{1, -6}, 2);
    circle2 = Circle<int32_t>(Point<int32_t>{-7, -6}, 5);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::NOT_INTERSECT);

    circle1 = Circle<int32_t>(Point<int32_t>{1, -6}, 3);
    circle2 = Circle<int32_t>(Point<int32_t>{-7, -6}, 5);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::TOUCH);
    assert(intersection.points[0].has_value());
    assert(!intersection.points[1].has_value());
    assert(is_equal(8.0 - 10.0, intersection.points[0].value().get_x(), 1e-6));
    assert(is_equal(4.0 - 10.0, intersection.points[0].value().get_y(), 1e-6));

    circle1 = Circle<int32_t>(Point<int32_t>{1, -6}, 4);
    circle2 = Circle<int32_t>(Point<int32_t>{-7, -6}, 5);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::INTERSECTION);
    assert(is_equal(7.5625000000 - 10.0, intersection.H.get_x(), 1e-6));
    assert(is_equal(4.0000000000 - 10.0, intersection.H.get_y(), 1e-6));
    assert(is_equal(8.0 - 4.5625000000, intersection.OH, 1e-6));
    assert(is_equal(2.0453835215, intersection.HP, 1e-6));
    assert(intersection.points[0].has_value());
    assert(is_equal(7.5625000000 - 10.0, intersection.points[0].value().get_x(), 1e-6));
    assert(is_equal(6.0453835215 - 10.0, intersection.points[0].value().get_y(), 1e-6) || is_equal(1.9546164785 - 10.0, intersection.points[0].value().get_y(), 1e-6));
    assert(intersection.points[1].has_value());
    assert(is_equal(7.5625000000 - 10.0, intersection.points[1].value().get_x(), 1e-6));
    assert(is_equal(6.0453835215 - 10.0, intersection.points[1].value().get_y(), 1e-6) || is_equal(1.9546164785 - 10.0, intersection.points[1].value().get_y(), 1e-6));

    circle1 = Circle<int32_t>(Point<int32_t>{-7, -6}, 5);
    circle2 = Circle<int32_t>(Point<int32_t>{-7, -6}, 5);
    intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::EQUAL);
}

void test_small_shift() {
    auto circle1 = Circle<int32_t>(Point<int32_t>{0, 0}, 10);
    auto circle2 = Circle<int32_t>(Point<int32_t>{0, 1}, 10);
    auto intersection = circle1.calculate_intersection(circle2);
    assert(intersection.type == Circle<int32_t>::IntersectionWithCircle::Type::INTERSECTION);
    assert(intersection.points[0].has_value());
    assert(intersection.points[1].has_value());
}

void run_all_tests() {
    test_from_task();
    test_internal();
    test_negative();
    test_small_shift();
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
    std::cout << std::setprecision(10) << std::fixed;
    for (size_t i = 0; i < N; ++i) {
        int64_t center1_x{0}, center1_y{0}, center2_x{0}, center2_y{0}, radius1{0}, radius2{0};
        std::cin >> center1_x >> center1_y >> radius1;
        std::cin >> center2_x >> center2_y >> radius2;
        Circle<int64_t> circle1(Point<int64_t>{center1_x, center1_y}, radius1);
        Circle<int64_t> circle2(Point<int64_t>{center2_x, center2_y}, radius2);
        auto intersection = circle1.calculate_intersection(circle2);
        switch (intersection.type) {
            case Circle<int64_t>::IntersectionWithCircle::Type::NOT_INTERSECT:
                std::cout << 0 << "\n";
                break;
            case Circle<int64_t>::IntersectionWithCircle::Type::EQUAL:
                std::cout << 3 << "\n";
                break;
            case Circle<int64_t>::IntersectionWithCircle::Type::TOUCH:
                std::cout << 1 << "\n";
                assert(intersection.points[0].has_value());
                std::cout << intersection.points[0].value().get_x() << " " << intersection.points[0].value().get_y() << "\n";
                break;
            case Circle<int64_t>::IntersectionWithCircle::Type::INTERSECTION:
                std::cout << 2 << "\n";
                assert(intersection.points[0].has_value());
                assert(intersection.points[1].has_value());
                std::cout << intersection.H.get_x() << " " << intersection.H.get_y() << "\n";
                std::cout << intersection.OH << " " << intersection.HP << "\n";
                std::cout << intersection.points[0].value().get_x() << " " << intersection.points[0].value().get_y() << "\n";
                std::cout << intersection.points[1].value().get_x() << " " << intersection.points[1].value().get_y() << "\n";
                break;
            default:
                assert(false);
        }
    }

    return 0;
}

