/*
 * C. Площади
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Даны n прямых на плоскости. Они делят плоскость на части, некоторые из которых конечны, некоторые — бесконечны. Найдите площади всех конечных частей.
 *
 * Входные данные
 * Первая строка содержит n — число прямых (1 ≤ n ≤ 80).
 * Каждая из следующих n строк содержит четыре целых числа x1, y1, x2 и y2 — координаты двух различных точек
 * на очередной прямой. Координаты не превышают 100 по абсолютной величине.
 * Прямые попарно различны.
 *
 * Выходные данные
 * В первой строке выведите k — число конечных частей.
 * В следующих k строках выведите их площади в неубывающем порядке.
 * Точность должна быть не хуже 10^-4. Не рассматривайте части, имеющие площадь меньшую 10^-8.
 *
 * Пример
 * Входные данные
 * 5
 * 0 0 1 0
 * 1 0 1 1
 * 1 1 0 1
 * 0 1 0 0
 * 0 0 1 1
 *
 * Выходные данные
 * 2
 * 0.5000000000
 * 0.5000000000
 */

/*
 * Решение похоже на один из подходов к решению задачи B.
 * Создаём большой многоугольник, делим его прямыми на части.
 * Только теперь сохраняем части по обе стороны от прямой.
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
bool is_equal(FloatType lhs, FloatType rhs, FloatType eps) {
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
 * @brief Класс, описывающий прямую линию.
 * @tparam NumberType Тип данных для хранения коэффициентов.
 */
template<typename NumberType>
class Line {
public:
    /**
     * @brief Структура для описания пересечения одной линии с другой.
     */
    struct IntersectionWithLine {
        /**
         * @brief Характер пересечения.
         */
        enum Type {
            NOT_INTERSECT,  // не пересекаются
            INTERSECTION,  // пересекаются
            EQUAL  // совпадают полностью
        };
        std::optional<Point<double>> point{std::nullopt};  // точка пересечения
        Type type{NOT_INTERSECT};  // Характер пересечения.
    };

    /**
     * @brief Конструктор прямой по трём коэффициентам в уравнении прямой ax+by+c=0
     * @param a Коэффициент при x. Задаёт наклон.
     * @param b Коэффициент при y. Задаёт наклон.
     * @param c Свободный коэффициент. Задаёт сдвиг.
     */
    Line(NumberType a, NumberType b, NumberType c) : a_(a), b_(b), c_(c) {};

    /**
     * @brief Построение прямой по двум точкам.
     * @param point1 Первая точка прямой.
     * @param point2 Вторая точка прямой.
     */
    Line(const Point<NumberType> &point1, const Point<NumberType> &point2) {
        /*
         * Записываем уравнение прямой в общем виде.
         * Подставляем в него координаты первой и второй точек.
         * Получаем простую систему из двух уравнений.
         * Находим любое её решение.
         * Подробнее: http://www.math.by/geometry/eqline.html
         */
        a_ = point1.get_y() - point2.get_y();
        b_ = point2.get_x() - point1.get_x();
        c_ = point1.get_x() * point2.get_y() - point2.get_x() * point1.get_y();
    }

    /**
     * @brief Найти пересечение с другой прямой.
     * @tparam NumberType2 Тип данных для хранения коэффициентов другой прямой.
     * @param rhs Другая прямая.
     * @return Описание пересечения прямых.
     */
    template<typename NumberType2>
    IntersectionWithLine calculate_intersection(const Line<NumberType2> &rhs) const {
        /*
         * Составляем систему из двух уравнений прямых и решаем методом Крамера.
         * Подробнее: http://www.e-maxx-ru.1gb.ru/algo/lines_intersection
         */
        if (a_ * rhs.get_b() - rhs.get_a() * b_ == 0) {  // Если знаменатель нулевой
            // то система решений не имеет (прямые параллельны и не совпадают) или имеет бесконечно
            // много (прямые совпадают). Если необходимо различить эти два случая, надо проверить, что коэффициенты C
            // прямых пропорциональны с тем же коэффициентом пропорциональности, что и коэффициенты A и B,
            // для чего достаточно посчитать два определителя, если они оба равны нулю, то прямые совпадают
            if ((a_ * rhs.get_c() - rhs.get_a() * c_ == 0) && (b_ * rhs.get_c() - rhs.get_b() * c_ == 0)) {
                return {std::nullopt, IntersectionWithLine::EQUAL};
            } else {
                return {std::nullopt, IntersectionWithLine::NOT_INTERSECT};
            }
        }
        double x = -static_cast<double>(c_ * rhs.get_b() - rhs.get_c() * b_) /
                   static_cast<double>(a_ * rhs.get_b() - rhs.get_a() * b_);
        double y = -static_cast<double>(a_ * rhs.get_c() - rhs.get_a() * c_) /
                   static_cast<double>(a_ * rhs.get_b() - rhs.get_a() * b_);
        return {Point<double>(x, y), IntersectionWithLine::INTERSECTION};
    }

    /**
     * @brief Сравнение прямых на равенство.
     * @details Равенство прямых - это не всегда равенство их коэффициентов.
     * @param rhs Второй операнд.
     * @return True, если прямые равны. Иначе - false.
     */
    bool operator==(const Line<NumberType> &rhs) const {
        auto intersection = calculate_intersection(rhs);
        return intersection.type == IntersectionWithLine::EQUAL;
    }

    /**
     * @brief Получить коэффициент A.
     * @return Коэффициент при x.
     */
    NumberType get_a() const {
        return a_;
    }

    /**
     * @brief Получить коэффициент B.
     * @return Коэффициент при y.
     */
    NumberType get_b() const {
        return b_;
    }

    /**
     * @brief Получить коэффициент C.
     * @return Свободный коэффициент.
     */
    NumberType get_c() const {
        return c_;
    }

    /**
     * @brief Выразить y из уравнения прямой.
     * @return Кортеж с двумя коэффициентами в выражении для y. Первый - тангенс угла наклона прямой, второй - сдвиг прямой по Oy.
     */
    std::tuple<double, double> solve_y() const {
        return {-static_cast<double>(a_) / static_cast<double>(b_), -static_cast<double>(c_) / static_cast<double>(b_)};
    }

    /**
     * @brief Найти значение y, соответствующее заданному x.
     * @param x Значение x, для которого ищем y.
     * @return Значение y.
     */
    double calculate_y(double x) const {
        //ToDo: обработать случай вертикальной прямой.
        auto[c1, c0] = solve_y();
        return c1 * x + c0;
    }

    /**
     * @brief Проверка прямых на параллельность.
     * @param rhs Прямая, с которой сравниваем текущую на параллельность.
     * @return True, если прямые параллельны. Иначе - false.
     */
    bool is_parallel(const Line &rhs) const {
        return a_ * rhs.b_ == b_ * rhs.a_;
    }

    /**
     * @brief Узнать, горизонтальна ли прямая.
     * @return True, если прямая горизонтальна. Иначе - false.
     */
    bool is_horizontal() const {
        return a_ == 0;
    }

    /**
     * @brief Узнать, вертикальна ли прямая.
     * @return True, если прямая вертикальна. Иначе - false.
     */
    bool is_vertical() const {
        return b_ == 0;
    }

private:
    NumberType a_{0};
    NumberType b_{0};
    NumberType c_{0};
};

/**
 * @brief Класс, описывающий полуплоскость.
 * @tparam NumberType Тип данных для хранения коэффициентов, задающих полуплоскость.
 */
template<typename NumberType>
class HalfPlane {
public:
    /**
     * @brief Тип полуплоскости
     * @brief POSITIVE, если ax+by+c>0
     * @brief NEGATIVE, если ax+by+c<0
     */
    enum Type {
        POSITIVE,
        NEGATIVE
    };

    /**
     * @brief Конструктор полуплоскости по задающей прямой и параметрам полуплоскости.
     * @param line Прямая - граница полуплоскости.
     * @param type Тип полуплоскости.
     * @param with_border Включает ли полуплоскость свою границу.
     * ToDo: сделать конструктор explicit, придётся исправить все тесты
     */
    HalfPlane(Line<NumberType> line, Type type = POSITIVE, bool with_border = true) : line_(line), type_(type),
                                                                                      with_border_(with_border) {};

    /**
     * @brief Сравнение полуплоскостей на равенство.
     * @param rhs Второй операнд.
     * @return True, если полуплоскости равны. Иначе - false.
     */
    bool operator==(const HalfPlane<NumberType> &rhs) const {
        return (this->line_ == rhs.line_) && (this->type_ == rhs.type_) && (this->with_border_ == rhs.with_border_);
    }

    /**
     * @brief Рассчитать тангенс угла наклона полуплоскости.
     * @return Тангенс угла наклона прямой, задающей полуплоскость.
     */
    double calculate_tangent_ox() const {
        return std::get<0>(line_.solve_y());
    }

    /**
     * @brief Проверка полуплоскостей на параллельность.
     * @param rhs Второй операнд.
     * @return True, если границы полуплоскостей параллельны. Иначе - false.
     */
    bool is_parallel(const HalfPlane &rhs) const {
        return line_.is_parallel(rhs.line_);
    }

    /**
     * @brief Полуплоскость смотрит вверх.
     * @details Более формально: точка с координатой (0, inf) принадлежит полуплоскости.
     * @details Вертикальные полуплоскости, которым принадлежит точка (inf, 0), тоже считаем смотрящими вверх.
     * @return True, если полуплоскость смотрит вверх.
     */
    bool is_upward() const {
        if (type_ == POSITIVE) {
            if (line_.get_b() != 0) {
                return line_.get_b() > 0;
            } else {
                assert(line_.get_a() != 0);
                return line_.get_a() < 0;
            }
        } else {
            if (line_.get_b() != 0) {
                return line_.get_b() < 0;
            } else {
                assert(line_.get_a() != 0);
                return line_.get_a() > 0;
            }
        }
    }

    /**
     * @brief Полуплоскость смотрит вправо.
     * @details Более формально: точка с координатой (inf, 0) принадлежит полуплоскости.
     * @details Горизонтальные полуплоскости тоже считаем смотрящими вправо (и влево тоже).
     * @return True, если полуплоскость смотрит вправо.
     */
    bool is_rightward() const {
        if (type_ == POSITIVE) {
            if (line_.get_a() != 0) {
                return line_.get_a() > 0;
            } else {
                assert(line_.get_b() != 0);
                return true;
                // return line_.get_b() < 0;
            }
        } else {
            if (line_.get_a() != 0) {
                return line_.get_a() < 0;
            } else {
                assert(line_.get_b() != 0);
                return true;
                // return line_.get_b() > 0;
            }
        }
    }

    /**
     * @brief Полуплоскость смотрит влево.
     * @details Если она не смотрит вправо или она горизонтальна.
     * @return True, если полуплоскость смотрит влево.
     */
    bool is_leftward() const {
        return !is_rightward() || line_.get_a() == 0;
    }

    /**
     * @brief Полуплоскость смотрит вниз.
     * @details Если она не смотрит вверх.
     * @return True, если полуплоскость смотрит вниз.
     */
    bool is_downward() const {
        return !is_upward();
    }

    /**
     * @brief Проверить, смотрят ли полуплоскости вместе вверх или вместе вниз.
     * @param rhs Второй операнд.
     * @return True, если полуплоскости вертикально сонаправленны.
     */
    bool vertically_codirectional(const HalfPlane &rhs) const {
        return is_upward() == rhs.is_upward();
    }

    /**
     * @brief Проверить, смотрят ли полуплоскости вместе вправо или вместе влево.
     * @param rhs Второй операнд.
     * @return True, если полуплоскости горизонтально сонаправленны.
     */
    bool horizontally_codirectional(const HalfPlane &rhs) const {
        return is_rightward() == rhs.is_rightward() || is_leftward() == rhs.is_leftward();
    }

    /**
     * @brief Проверить, смотрят ли полуплоскости вместе вверх или вместе вниз.
     * @param first Второй операнд.
     * @param second Третий операнд.
     * @return True, если полуплоскости вертикально сонаправленны.
     */
    bool vertically_codirectional(const HalfPlane &first, const HalfPlane &second) const {
        return (is_upward() == first.is_upward()) && (is_upward() == second.is_upward());
    }

    /**
     * @brief Проверить, смотрят ли полуплоскости вместе влево или вместе вправо.
     * @param first Второй операнд.
     * @param second Третий операнд.
     * @return True, если полуплоскости горизонтально сонаправленны.
     */
    bool horizontally_codirectional(const HalfPlane &first, const HalfPlane &second) const {
        return (is_rightward() == first.is_rightward()) && (is_rightward() == second.is_rightward());
    }

    /**
     * @brief Проверить, пересекаются ли границы трёх полуплоскостей в одной точке.
     * @param first Второй операнд.
     * @param second Третий операнд.
     * @return True, если границы всех трёх полуплоскостей пересекаются в одной точке.
     */
    bool is_point(const HalfPlane &first, const HalfPlane &second) {
        auto intersection_with_first = line_.calculate_intersection(first.line_);
        auto intersection_with_second = line_.calculate_intersection(second.line_);
        if (intersection_with_first.point.has_value() && intersection_with_second.point.has_value()) {
            if (intersection_with_first.point == intersection_with_second.point) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Образует ли пересечение трёх полуплоскостей треугольник?
     * @param first Второй операнд.
     * @param second Третий операнд.
     * @return True, если пересечение трёх полуплоскостей образует треугольник.
     */
    bool is_triangle(const HalfPlane &first, const HalfPlane &second) {
        bool triangle_or_point =
                this->contain_intersection(first, second) && first.contain_intersection(*this, second) &&
                second.contain_intersection(*this, first);
        bool point = is_point(first, second);
        return triangle_or_point && !point;
    }

    /**
     * @brief Проверить, покрывает ли одна полуплоскость другую полностью?
     * @param rhs Второй операнд.
     * @return True, если эта полуплоскость покрывает полностью переданную в качестве второго операнда.
     */
    bool contain_halfplane(const HalfPlane &rhs) const {
        if (*this == rhs) {
            return true;
        }
        if (!is_parallel(rhs)) {
            return false;
        }
        if (is_upward() != rhs.is_upward()) {
            return false;
        }
        if (line_.is_vertical()) {
            return rhs.line_.get_c() < line_.get_c();
        }
        if (is_upward()) {
            return rhs.line_.calculate_y(0) > line_.calculate_y(0);
        } else {
            return rhs.line_.calculate_y(0) < line_.calculate_y(0);
        }
    }

    /**
     * @brief Проверить, принадлежит ли точка полуплоскости?
     * @tparam NumberType2 Тип данных координат точки.
     * @param point Точка.
     * @return True, если точка принадлежит полуплоскости.
     */
    template<typename NumberType2>
    bool contain_point(const Point<NumberType2> &point) const {
        auto value = line_.get_a() * point.get_x() + line_.get_b() * point.get_y() + line_.get_c();
        if (with_border_ && (value == 0)) {
            return true;
        }
        if (type_ == POSITIVE) {
            return value > 0;
        } else {
            return value < 0;
        }
    }

    /**
     * @brief Проверить, принадлежит ли точка полуплоскости без учёта границы, даже если with_border == true?
     * @tparam NumberType2 Тип данных координат точки.
     * @param point Точка.
     * @return True, если точка принадлежит полуплоскости, но не её границе.
     */
    template<typename NumberType2>
    bool contain_point_without_border(const Point<NumberType2> &point) const {
        auto value = line_.get_a() * point.get_x() + line_.get_b() * point.get_y() + line_.get_c();
        if (type_ == POSITIVE) {
            return value > 0;
        } else {
            return value < 0;
        }
    }

    /**
     * @brief Проверить, принадлежит ли точка пересечения двух других полуплоскостей этой полуплоскости?
     * @param point first Первая полуплоскость.
     * @param point second Вторая полуплоскость.
     * @return True, если точка пересечения first и second принадлежит этой полуплоскости.
     */
    bool contain_intersection(const HalfPlane &first, const HalfPlane &second) const {
        auto intersection = first.line_.calculate_intersection(second.line_);
        if (!intersection.point.has_value()) {
            return false;
        }
        return contain_point(intersection.point.value());
    }

/**
     * @brief Проверить, принадлежит ли точка пересечения двух других полуплоскостей этой полуплоскости без учёта границы?
     * @param point first Первая полуплоскость.
     * @param point second Вторая полуплоскость.
     * @return True, если точка пересечения first и second принадлежит этой полуплоскости, но не её границе.
     */
    bool contain_intersection_without_border(const HalfPlane &first, const HalfPlane &second) const {
        auto intersection = first.line_.calculate_intersection(second.line_);
        if (!intersection.point.has_value()) {
            return false;
        }
        return contain_point_without_border(intersection.point.value());
    }

    /**
     * @brief Найти пересечение границы полуплоскости с другой прямой.
     * @tparam NumberType2 Тип данных для коэффициентов другой прямой.
     * @param line Другая прямая.
     * @return Описание пересечения границы и прямой.
     */
    template<typename NumberType2>
    typename Line<NumberType>::IntersectionWithLine calculate_intersection(const Line<NumberType2> &line) const {
        return line_.calculate_intersection(line);
    }

    /**
     * @brief Получить прямую, задающую границу полуплоскости.
     * @return Прямая, задающая границу полуплоскости.
     */
    Line<NumberType> get_line() const {
        return line_;
    }

private:
    Line<NumberType> line_;
    Type type_{POSITIVE};
    bool with_border_{true};
};

/**
 * @brief Класс, описывающий многоугольник.
 * @tparam NumberType Тип данных для хранения координат точек.
 */
template<typename NumberType>
class Polygon {
public:
    /**
     * @brief Конструирует многоугольник по набору точек-вершин.
     * @param points Точки, вершины многоугольника.
     */
    explicit Polygon(std::vector<Point<NumberType>> points) : points_{std::move(points)} {}

    /**
     * @brief Конструктор по умолчанию.
     */
    Polygon() = default;

    /**
     * @brief Оператор присваивания (копирования).
     * @param other Копируемый многоугольник.
     * @return Ссылка на новый объект.
     */
    Polygon& operator=(const Polygon& other) // copy assignment
    {
        if (this != &other) { // self-assignment check expected
            points_ = other.points_;
        }
        return *this;
    }

    /**
     * @brief Построить многоугольник как результат пересечения полуплоскостей.
     * @tparam NumberType2 Тип данных для хранения коэффициентов полуплоскостей.
     * @param halfplanes Массив полуплоскостей.
     */
    template<typename NumberType2>
    explicit Polygon(std::vector<HalfPlane<NumberType2>> halfplanes) {
        /*
         * Даны полуплоскости.
         * Их границы - это грани многоугольника выпуклого.
         * Сортируем все полуплоскости по углу так, чтобы они против часовой стрелки окружали этот многоугольник.
         * Проходим в цикле, убираем лишние параллельные (когда одна покрывает другую).
         * Затем проходим в цикле и обрабатываем отдельно верхние и нижние границы,
         * убираем такие плохие полуплоскости https://neerc.ifmo.ru/wiki/images/5/54/HalfSpaces.png,
         * которые покрывают пересение двух других.
         * Верхние и нижние почистили, потом справа и слева аналогино убираем полуплоскости, покрывающие пересечение других.
         * То, что остаётся, не должно содержать лишнего и должно образовывать выпуклый многоугольник.
         * Проходим в цикле и ищем точки пересечения текущей грани со следующей.
         * Получаем многоугольник.
         */

        assert(halfplanes.size() > 2);

        // Отсортируем полуплоскости по углу так, чтобы они были перечислены против часовой стрелки вокруг будущего многоугольника.
        std::sort(halfplanes.begin(), halfplanes.end(), [](const auto &lhs, const auto &rhs) {
            auto lhs_top = lhs.is_upward();
            auto rhs_top = rhs.is_upward();
            if (lhs_top == rhs_top) {
                auto lhs_tan_ox = lhs.calculate_tangent_ox();
                if (lhs_tan_ox == -std::numeric_limits<double>::infinity()) {
                    lhs_tan_ox = std::numeric_limits<double>::infinity();
                }
                auto rhs_tan_ox = rhs.calculate_tangent_ox();
                if (rhs_tan_ox == -std::numeric_limits<double>::infinity()) {
                    rhs_tan_ox = std::numeric_limits<double>::infinity();
                }
                return lhs_tan_ox < rhs_tan_ox;
            }
            return static_cast<int>(lhs_top) < static_cast<int>(rhs_top);
        });
        assert(halfplanes.front().is_upward() != halfplanes.back().is_upward());

        /*
         * Первый шаг фильтрации. Уберём лишние параллельные полуплоскости (те, которые включают другие).
         *
         * Среди полуплоскостей с одинаковым углом и оставим только ту, которая покрывается всеми другими.
         * Это легко сделать за линейное число шагов, так как полуплоскости с одинаковым углом будут располагаться
         * последовательно в отсортированном массиве. Таким образом мы избавились от прямых, которые заведомо нам не нужны.
         */
        //ToDo: везде далее в этой функции заменить операции с индексами на что-то более читабельное, например, итераторы.
        /*
         * Ниже я не удаляю ненужные полуплоскости, а просто смещаю индексы.
         */
        std::vector<HalfPlane<NumberType2>> filtered_halfplanes1;
        size_t begin = 0;
        size_t end = halfplanes.size();
        while (begin != end) {
            if (end - begin > 1) {
                // Проверяем, покрывает ли текущая следующую и наоборот
                if (halfplanes[begin].contain_halfplane(halfplanes[begin + 1])) {
                    begin += 1;
                    continue;
                }
                if (halfplanes[begin + 1].contain_halfplane(halfplanes[begin])) {
                    halfplanes[begin + 1] = halfplanes[begin];
                    begin += 1;
                    continue;
                }
            }

            filtered_halfplanes1.push_back(halfplanes[begin]);
            begin += 1;
        }

        begin = 0;
        end = filtered_halfplanes1.size();
        // проверим аналогично первую и последнюю
        if (filtered_halfplanes1.front().contain_halfplane(filtered_halfplanes1.back())) {
            begin += 1;
        } else if (filtered_halfplanes1.back().contain_halfplane(filtered_halfplanes1.front())) {
            end -= 1;
        }

        if (end - begin < 3) {
            return;  // многоугольник не может иметь менее трёх граней
        }
        if (filtered_halfplanes1[begin].is_upward() == filtered_halfplanes1[end - 1].is_upward()) {
            return;  // все грани многоугольника не могут смотреть одновременно вверх или одновременно вниз
        }

        /*
         * Второй шаг фильтрации. Уберём плохие тройки.
         * Рассматриваем сонаправленные полуплоскости (только вверх или только вниз).
         *
         * Рассмотрим последовательные полуплоскости h[i-1], h[i], h[i+1] в отсортированном массиве.
         * Если h[i] покрывает пересечение h[i-1] и h[i+1], то мы, очевидно, можем исключить ее из рассмотрения,
         * так как она не окажет влияния на ответ. Назовем такую ситуацию “плохой тройкой”
         */
        // ToDo: вынести проверку троки на плохость/хорошесть в отдельную функцию, это уменьшит повторы кода
        std::vector<HalfPlane<NumberType2>> filtered_halfplanes2;
        // добавим первый элемент, не проверяя, потому что левый и правый края обработаем ниже отдельно
        filtered_halfplanes2.push_back(filtered_halfplanes1[begin]);
        begin += 1;
        // если после первого элемента сразу встретили правый край фигуры, то добавим ещё одну грань, а край обработаем позже
        if (filtered_halfplanes2.back().is_upward() != filtered_halfplanes1[begin].is_upward()) {
            filtered_halfplanes2.push_back(filtered_halfplanes1[begin]);
            begin += 1;
        }

        // сохраним место, где встретим границу верхней и нижней границ фигуры (правый край фигуры)
        size_t updown_border = 0;

        while (begin < end) {
            // если грани не сонаправленны (одна верхняя, а одна нижняя)
            if (!filtered_halfplanes2.back().vertically_codirectional(filtered_halfplanes1[begin])) {
                // сохраним индекс правого края фигуры, добавим грань в новый массив, позже обработаем правый край отдельно
                updown_border = filtered_halfplanes2.size();
                filtered_halfplanes2.push_back(filtered_halfplanes1[begin]);
                begin += 1;
                continue;
            }
            if (end - begin >= 2) {
                // Если полуплоскость содержит пересечение двух соседних, то это кандидат на плохую тройку
                bool contain_intersection = filtered_halfplanes1[begin].contain_intersection(
                        filtered_halfplanes2.back(),
                        filtered_halfplanes1[begin + 1]);
                // в треугольнике всегда грань покрывает пересечение соседних, обнаружим этот случай
                bool is_triangle = filtered_halfplanes1[begin].is_triangle(filtered_halfplanes2.back(),
                                                                           filtered_halfplanes1[begin + 1]);
                // если грани сонаправлены (обе верхние или обе нижние)
                if (filtered_halfplanes1[begin].vertically_codirectional(filtered_halfplanes1[begin + 1])) {
                    if (!is_triangle && contain_intersection) {
                        begin += 1;  // тройка плохая, убираем грань
                        continue;
                    }
                }
            }
            if (filtered_halfplanes2.size() >= 2) {
                // после того как мы пропустили какие-то грани и собираемся добавить новую,
                // надо проверить, что старые уже добавленные грани не образуют с ней плохую тройку
                bool contain_intersection = filtered_halfplanes2.back().contain_intersection(
                        filtered_halfplanes2[filtered_halfplanes2.size() - 2],
                        filtered_halfplanes1[begin]);
                bool is_triangle = filtered_halfplanes2.back().is_triangle(
                        filtered_halfplanes2[filtered_halfplanes2.size() - 2],
                        filtered_halfplanes1[begin]);
                if (!is_triangle && contain_intersection) {
                    // тройка плохая, убираем последнюю добавленную грань
                    filtered_halfplanes2.erase(filtered_halfplanes2.end());
                    continue;
                }
            }

            filtered_halfplanes2.push_back(filtered_halfplanes1[begin]);
            begin += 1;
        }

        if (filtered_halfplanes2.size() < 3) {
            return;  // многоугольник не может иметь менее трёх граней
        }
        if (filtered_halfplanes2.front().is_upward() == filtered_halfplanes2.back().is_upward()) {
            return;  // все грани многоугольника не могут смотреть одновременно вверх или одновременно вниз
        }

        // Третий шаг фильтрации. Обработаем левый и правый края фигуры.
        // Произведём те же действия, что и выше, только не для верхних и нижних граней, а для левых и правых

        // Правый край
        auto begin_right = 0;
        auto end_right = filtered_halfplanes2.size();
        while (true) {
            if (end_right - begin_right < 3) {
                return;
            }
            bool contain_intersection1 = filtered_halfplanes2[begin_right].contain_intersection(
                    filtered_halfplanes2[begin_right + 1],
                    filtered_halfplanes2[end_right - 1]);
            bool is_triangle = filtered_halfplanes2[begin_right].is_triangle(filtered_halfplanes2[begin_right + 1],
                                                                             filtered_halfplanes2[end_right - 1]);
            if (!is_triangle && contain_intersection1) {
                begin_right += 1;
                continue;
            }

            bool contain_intersection2 = filtered_halfplanes2[end_right - 1].contain_intersection(
                    filtered_halfplanes2[end_right - 2], filtered_halfplanes2[begin_right]);
            is_triangle = filtered_halfplanes2[end_right - 1].is_triangle(filtered_halfplanes2[end_right - 2],
                                                                          filtered_halfplanes2[begin_right]);
            if (!is_triangle && contain_intersection2) {
                end_right -= 1;
                continue;
            }

            break;
        }

        // Левый край
        auto begin_left = updown_border;
        auto end_left = updown_border;

        assert(begin_right <= end_left);
        assert(end_left <= begin_left);
        assert(begin_left <= end_right);

        while (true) {
            if ((end_left - begin_right) + (end_right - begin_left) < 3) {
                return;
            }
            if (end_left == begin_right) {
                return;
            }
            if (end_right == begin_left) {
                return;
            }
            if (end_right - begin_left > 1) {
                bool contain_intersection = filtered_halfplanes2[begin_left].contain_intersection(
                        filtered_halfplanes2[begin_left + 1],
                        filtered_halfplanes2[end_left - 1]);
                bool is_triangle = filtered_halfplanes2[begin_left].is_triangle(filtered_halfplanes2[begin_left + 1],
                                                                                filtered_halfplanes2[end_left - 1]);
                if (!is_triangle && contain_intersection) {
                    begin_left += 1;
                    continue;
                }
            }
            if (end_left - begin_right > 1) {
                bool contain_intersection = filtered_halfplanes2[end_left - 1].contain_intersection(
                        filtered_halfplanes2[end_left - 2],
                        filtered_halfplanes2[begin_left]);
                bool is_triangle = filtered_halfplanes2[end_left - 1].is_triangle(filtered_halfplanes2[end_left - 2],
                                                                                  filtered_halfplanes2[begin_left]);
                if (!is_triangle && contain_intersection) {
                    end_left -= 1;
                    continue;
                }
            }

            break;
        }

        // Сохраним результат фильтрации граней
        std::vector<HalfPlane<NumberType2>> filtered_halfplanes3;
        assert(begin_right <= end_left);
        assert(end_left <= begin_left);
        assert(begin_left <= end_right);
        std::copy(filtered_halfplanes2.begin() + begin_right, filtered_halfplanes2.begin() + end_left,
                  std::back_inserter(filtered_halfplanes3));
        std::copy(filtered_halfplanes2.begin() + begin_left, filtered_halfplanes2.begin() + end_right,
                  std::back_inserter(filtered_halfplanes3));

        if (filtered_halfplanes3.size() < 3) {
            return;  // многоугольник не может иметь менее трёх граней
        }
        if (filtered_halfplanes3.front().is_upward() == filtered_halfplanes3.back().is_upward()) {
            return;  // все грани многоугольника не могут смотреть одновременно вверх или одновременно вниз
        }

        // Пара проверок, которым должен удовлетворять корректный многоугольник
        if (!filtered_halfplanes3[0].contain_intersection_without_border(
                filtered_halfplanes3[filtered_halfplanes3.size() - 2],
                filtered_halfplanes3[filtered_halfplanes3.size() - 1])) {
            return;
        }
        if (!filtered_halfplanes3[filtered_halfplanes3.size() - 1].contain_intersection_without_border(
                filtered_halfplanes3[0], filtered_halfplanes3[0 + 1])) {
            return;
        }

        for (size_t i = 0; i < filtered_halfplanes3.size() - 1; ++i) {
            auto intersection = filtered_halfplanes3[i].get_line().calculate_intersection(
                    filtered_halfplanes3[i + 1].get_line());
            assert(intersection.type == Line<int64_t>::IntersectionWithLine::Type::INTERSECTION);
            assert(intersection.point.has_value());
            points_.push_back(intersection.point.value());
        }
        auto intersection = filtered_halfplanes3.back().get_line().calculate_intersection(
                filtered_halfplanes3.front().get_line());
        assert(intersection.point.has_value());
        points_.push_back(intersection.point.value());
    }

    /**
     * @brief Расчёт площади многоугольника.
     * @details Формула площади Гаусса
     * @return Площадь многоугольника.
     */
    double calculate_area() const {
        /*
         * Формула определения площади определяется путём взятия каждого ребра многоугольника АВ и
         * вычисления площади треугольника АВО с вершиной в начале координат О через координаты вершин.
         * При обходе вокруг многоугольника образуются треугольники, включающие внутреннюю часть многоугольника
         * и расположенные снаружи его. Разница между суммой этих площадей и есть площадь самого многоугольника.
         *
         * Формула площади верна для любого самонепересекающегося (простого) многоугольника,
         * который может быть выпуклым или вогнутым.
         */
        if (points_.empty()) {
            return 0.0;
        }
        double area = 0.0;
        for (size_t i = 0; i < points_.size() - 1; ++i) {
            auto production_length =
                    points_[i].get_x() * points_[i + 1].get_y() - points_[i].get_y() * points_[i + 1].get_x();
            area += 0.5 * static_cast<double>(production_length);
        }
        auto production_length =
                points_.back().get_x() * points_.front().get_y() - points_.back().get_y() * points_.front().get_x();
        area += 0.5 * static_cast<double>(production_length);
        return std::abs(area);
    }

    /**
     * @brief Найти пересечение многоугольника с набором полуплоскостей.
     * @tparam NumberType2 Тип данных для хранения коэффициентов полуплоскостей.
     * @param halfplanes Массив полуплоскостей.
     */
    template<typename NumberType2>
    void cut(const std::vector<HalfPlane<NumberType2>> &halfplanes) {
        for (const auto &halfplane: halfplanes) {
            cut(halfplane);
        }
    }

    /**
     * @brief Найти пересечение многоугольника с набором прямых.
     * @tparam NumberType2 Тип данных для хранения коэффициентов полуплоскостей.
     * @param halfplanes Массив полуплоскостей.
     */
    template<typename NumberType2>
    std::vector<Polygon<NumberType>> cut(const std::vector<Line<NumberType2>> &lines) const {
        std::vector<Polygon<NumberType>> all_polygons;
        all_polygons.push_back(*this);
        for (const auto &line: lines) {
            std::vector<Polygon<NumberType>> splited_polygons;
            for (const auto &polygon: all_polygons) {
                auto new_polygons = polygon.cut(line);
                splited_polygons.insert(splited_polygons.end(), new_polygons.begin(), new_polygons.end());
            }
            all_polygons = splited_polygons;
        }
        return all_polygons;
    }

    /**
     * @brief Найти пересечение многоугольника с полуплоскостью.
     * @tparam NumberType2 Тип данных для хранения коэффициентов полуплоскости.
     * @param halfplane Полуплоскость.
     */
    template<typename NumberType2>
    void cut(const HalfPlane<NumberType2> &halfplane) {
        if (points_.empty()) {
            return;
        }

        std::vector<Point<NumberType>> new_points;  // вершины многоугольника - результат пересечения

        for (int i = 0; i < points_.size() - 1; ++i) {
            bool contained = halfplane.contain_point(points_[i]);
            bool contained_next = halfplane.contain_point(points_[i + 1]);

            if (contained) {  // если точка принадлежит полуплоскости, то она принадлежит пересечению
                new_points.push_back(points_[i]);
            }
            if (contained ^ contained_next) {  // если текущая точка принадлежит, а следующая - нет, или наоборот
                // то между этими точками полуплоскость пересекает границу многоугольника
                // ищем точку пересечения, она принадлежит результату пересечения
                auto intersection = halfplane.calculate_intersection(Line<NumberType>(points_[i], points_[i + 1]));
                assert(intersection.point.has_value());
                new_points.emplace_back(intersection.point.value());
            }
        }

        // отдельно обработаем первую и последнюю точки
        bool contained = halfplane.contain_point(points_[points_.size() - 1]);
        bool contained_next = halfplane.contain_point(points_[0]);
        if (contained) {
            new_points.push_back(points_[points_.size() - 1]);
        }
        if (contained ^ contained_next) {
            auto intersection = halfplane.calculate_intersection(
                    Line<NumberType>(points_[points_.size() - 1], points_[0]));
            assert(intersection.point.has_value());
            new_points.emplace_back(intersection.point.value());
        }

        points_ = new_points;  // сохраним новый многоугольник
    }

    /**
     * @brief Найти рассечение многоугольника прямой.
     * @tparam NumberType2 Тип данных для хранения коэффициентов полуплоскости.
     * @param line Прямая
     * @return Массив с двумя многоугольниками, полученными после рассечения.
     */
    template<typename NumberType2>
    std::vector<Polygon<NumberType>> cut(const Line<NumberType2> &line) const {
        HalfPlane<NumberType2> halfplane(line);

        if (points_.empty()) {
            return {};
        }

        std::vector<Point<NumberType>> contained_figure;  // вершины многоугольника - результат пересечения
        std::vector<Point<NumberType>> not_contained_figure;  // вершины многоугольника - результат пересечения

        for (int i = 0; i < points_.size() - 1; ++i) {
            bool contained = halfplane.contain_point(points_[i]);
            bool contained_next = halfplane.contain_point(points_[i + 1]);

            if (contained) {  // если точка принадлежит полуплоскости, то она принадлежит одной фигуре, иначе - другой
                contained_figure.push_back(points_[i]);
            } else {
                not_contained_figure.push_back(points_[i]);
            }

            if (contained ^ contained_next) {  // если текущая точка принадлежит, а следующая - нет, или наоборот
                // то между этими точками полуплоскость пересекает границу многоугольника
                // ищем точку пересечения, она принадлежит результату пересечения
                auto intersection = halfplane.calculate_intersection(Line<NumberType>(points_[i], points_[i + 1]));
                assert(intersection.point.has_value());
                contained_figure.emplace_back(intersection.point.value());
                not_contained_figure.emplace_back(intersection.point.value());
            }
        }

        // отдельно обработаем первую и последнюю точки
        bool contained = halfplane.contain_point(points_[points_.size() - 1]);
        bool contained_next = halfplane.contain_point(points_[0]);
        if (contained) {
            contained_figure.push_back(points_[points_.size() - 1]);
        } else {
            not_contained_figure.push_back(points_[points_.size() - 1]);
        }
        if (contained ^ contained_next) {
            auto intersection = halfplane.calculate_intersection(
                    Line<NumberType>(points_[points_.size() - 1], points_[0]));
            assert(intersection.point.has_value());
            contained_figure.emplace_back(intersection.point.value());
            not_contained_figure.emplace_back(intersection.point.value());
        }
        std::vector<Polygon<NumberType>> result;
        if (!contained_figure.empty()){
            result.emplace_back(contained_figure);
        }
        if (!not_contained_figure.empty()){
            result.emplace_back(not_contained_figure);
        }
        return result;  // вернём один-два новых многоугольника
    }

    /**
     * @brief Проверить, есть ли у многоугольника точка с нужной координатой.
     * @param coordinate Координата.
     * @return True, если точка с нужным значением координаты присутствует.
     */
    bool has_coordinate(NumberType coordinate) const {
        if (points_.empty()) {
            return false;
        }
        for (const auto &point: points_) {
            if (is_equal(point.get_x(), coordinate, EPS_)) {
                return true;
            }
            if (is_equal(point.get_y(), coordinate, EPS_)) {
                return true;
            }
        }
        return false;
    }

private:
    std::vector<Point<NumberType>> points_;
    const double EPS_ = 1e-8;
};


// Начало тестов

void test_from_task() {

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
    size_t n{0};
    std::cin >> n;
    std::vector<Line<int64_t>> lines;
    for (size_t i=0; i < n; ++i) {
        int64_t x1{0}, y1{0}, x2{0}, y2{0};
        std::cin >>x1>>y1>>x2>>y2;
        lines.emplace_back(Point<int64_t>{x1,y1}, Point<int64_t>{x2,y2});
    }
    const double MAX_COORDINATE = 1e6;
    Polygon<double> plane({
        Point<double>{-MAX_COORDINATE, -MAX_COORDINATE},
        Point<double>{MAX_COORDINATE, -MAX_COORDINATE},
        Point<double>{MAX_COORDINATE, MAX_COORDINATE},
        Point<double>{-MAX_COORDINATE, MAX_COORDINATE}
    });
    auto figures = plane.cut(lines);
    std::vector<double> areas;
    size_t count = 0;
    for(const auto& figure: figures) {
        if (!figure.has_coordinate(MAX_COORDINATE) && !figure.has_coordinate(-MAX_COORDINATE)) {
            auto current_area = figure.calculate_area();
            if (!is_equal(current_area, 0.0, 1e-8)){
                count += 1;
                areas.push_back(current_area);
            }
        }
    }
    std::cout << std::setprecision(10) << std::fixed;
    std::cout << count << "\n";
    std::sort(areas.begin(), areas.end());
    for(auto area: areas){
        std::cout << area << "\n";
    }
    return 0;
}
