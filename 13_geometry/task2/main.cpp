/*
 * B. Пересечение полуплоскостей
 * ограничение по времени на тест 2 секунды
 * ограничение по памяти на тест 256 мегабайт
 * ввод стандартный ввод
 * вывод стандартный вывод
 *
 * Эта задача не имеет условия.
 *
 * Входные данные
 * Первая строка входного файла содержит единственное число n — число полуплоскостей,
 * площадь пересечения которых вам требуется посчитать (3 ≤ n ≤ 2000).
 * Следующие n строк содержат по три целых числа ai, bi, ci, по модулю не превосходящих 10000,
 * задающие полуплоскость ((x, y) принадлежит i-й полуплоскости, если a·x + b·y + c ≥ 0).
 * Гарантируется, что площадь пересечения конечна.
 *
 * Выходные данные
 * Одно вещественное число — площадь пересечения полуплоскостей с точностью до 10 - 6.
 *
 * Пример
 * Входные данные
 * 3
 * 1 0 0
 * 0 1 0
 * -1 -1 1
 * Выходные данные
 * 0.5000000000
 */

/*
 * Подробнее о пересечении полуплоскостей:
 * https://neerc.ifmo.ru/wiki/index.php?title=Пересечение_полуплоскостей,_связь_с_выпуклыми_оболочками
 * https://algocode.ru/files/course_aspb2019/main.pdf
 *
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
#include <tuple>
#include <deque>
#include <stack>


template<typename FloatType>
bool is_equal(FloatType lhs, FloatType rhs, FloatType eps) {
    return std::abs(lhs - rhs) < eps;
}

template<typename NumberType>
class Vector;

template<typename NumberType>
struct Point {
public:
    template<typename NumberType2>
    Point(NumberType2 x, NumberType2 y):x_(x), y_(y) {}

    template<typename NumberType2>
    Point(const Vector<NumberType2> &vector):x_(vector.get_x()), y_(vector.get_y()) {}

    template<typename NumberType2>
    Point(const Point<NumberType2> &point):x_(point.get_x()), y_(point.get_y()) {}

    bool operator==(const Point<NumberType> &rhs) const {
        // ToDo: comparison for float types
        return (this->x_ == rhs.x_) && (this->y_ == rhs.y_);
    }

    NumberType get_x() const {
        return x_;
    }

    NumberType get_y() const {
        return y_;
    }


    NumberType x_{0};
    NumberType y_{0};
};

template<typename NumberType>
class Vector {
public:
    template<typename NumberType2>
    Vector(Point<NumberType2> begin, Point<NumberType2> end) {
        end_ = end;
        end_.x_ -= begin.x_;
        end_.y_ -= begin.y_;
    }

    template<typename NumberType2>
    Vector(Point<NumberType2> end) : end_(end) {}

    Vector<double> operator*(const double rhs) const {
        return Vector<double>(Point<double>{static_cast<double>(end_.x_) * rhs, static_cast<double>(end_.y_) * rhs});
    }

    Vector<double> operator+(const Vector<double> &rhs) const {
        return Vector<double>(Point<double>{
                static_cast<double>(end_.x_) + rhs.end_.x_,
                static_cast<double>(end_.y_) + rhs.end_.y_
        });
    }

    Vector turn_ccw_90() const {
        return Vector(Point<NumberType>(-end_.y_, end_.x_));
    }

    Vector turn_cw_90() const {
        return Vector(Point<NumberType>(end_.y_, -end_.x_));
    }

    NumberType square() const {
        return end_.x_ * end_.x_ + end_.y_ * end_.y_;
    }

    double length() const {
        return std::sqrt(static_cast<double>(square()));
    }

    Point<NumberType> get_end() const {
        return end_;
    }

    NumberType get_x() const {
        return end_.x_;
    }

    NumberType get_y() const {
        return end_.y_;
    }

private:
    Point<NumberType> end_{0, 0};
};

template<typename NumberType>
class Circle {
public:
    struct IntersectionWithCircle {
        enum Type {
            NOT_INTERSECT,
            TOUCH,
            INTERSECTION,
            EQUAL
        };
        double OH{0.0};
        double HP{0.0};
        Point<double> H{0.0, 0.0};
        std::array<std::optional<Point<double>>, 2> points{std::nullopt, std::nullopt};
        Type type{NOT_INTERSECT};
    };

    Circle(Point<NumberType> center, NumberType radius) : center_(center), radius_(radius) {};

    bool operator==(const Circle<NumberType> &rhs) const {
        // ToDo: comparison for float types
        return (this->center_ == rhs.center_) && (this->radius_ == rhs.radius_);
    }

    IntersectionWithCircle calculate_intersection(const Circle &rhs) {
        if (*this == rhs) {
            return IntersectionWithCircle{0.0, 0.0, {0.0, 0.0}, {std::nullopt, std::nullopt},
                                          IntersectionWithCircle::EQUAL};
        }

        Vector<NumberType> center_to_center(this->center_, rhs.center_);
        auto perpendicular = center_to_center.turn_ccw_90();

        auto r0 = this->radius_;
        auto r0_square = r0 * r0;
        auto r1 = rhs.radius_;
        auto r1_square = r1 * r1;
        auto c2c_square = center_to_center.square();

        if (c2c_square > (r0 + r1) * (r0 + r1)) {
            return IntersectionWithCircle{0.0, 0.0, {0.0, 0.0}, {std::nullopt, std::nullopt},
                                          IntersectionWithCircle::NOT_INTERSECT};
        }
        if (c2c_square < (r0 - r1) * (r0 - r1)) {
            return IntersectionWithCircle{0.0, 0.0, {0.0, 0.0}, {std::nullopt, std::nullopt},
                                          IntersectionWithCircle::NOT_INTERSECT};
        }

        auto OH = (r0_square - r1_square + c2c_square) / (2 * center_to_center.length());
        Point<double> H = Vector<double>(this->center_) + center_to_center * (OH / center_to_center.length());
        if ((c2c_square == (r0 + r1) * (r0 + r1)) || (c2c_square == (r0 - r1) * (r0 - r1))) {
            return IntersectionWithCircle{
                    OH,
                    0.0, H,
                    {H, std::nullopt},
                    IntersectionWithCircle::TOUCH};
        }

        double HP = std::sqrt(r0_square - OH * OH);
        return IntersectionWithCircle{
                OH,
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

template<typename NumberType>
class Line {
public:
    struct IntersectionWithLine {
        /**
         * @brief Характер пересечения.
         */
        enum Type {
            NOT_INTERSECT,  // не пересекаются
            INTERSECTION,  // пересекаются
            EQUAL  // совпадают полностью
        };
        std::optional<Point<double>> point{std::nullopt};
        Type type{NOT_INTERSECT};  // Характер пересечения.
    };

    Line(NumberType a, NumberType b, NumberType c) : a_(a), b_(b), c_(c) {};

    IntersectionWithLine calculate_intersection(const Line &rhs) {
        if (a_ * rhs.b_ - rhs.a_ * b_ == 0) {
            if ((a_ * rhs.c_ - rhs.a_ * c_) && (b_ * rhs.c_ - rhs.b_ * c_)) {
                return {std::nullopt, IntersectionWithLine::EQUAL};
            } else {
                return {std::nullopt, IntersectionWithLine::NOT_INTERSECT};
            }
        }
        double x = -static_cast<double>(c_ * rhs.b_ - rhs.c_ * b_) / static_cast<double>(a_ * rhs.b_ - rhs.a_ * b_);
        double y = -static_cast<double>(a_ * rhs.c_ - rhs.a_ * c_) / static_cast<double>(a_ * rhs.b_ - rhs.a_ * b_);
        return {Point<double>(x, y), IntersectionWithLine::INTERSECTION};
    }

    bool operator==(const Line<NumberType> &rhs) const {
        // ToDo: comparison for float types
        return (this->a_ == rhs.a_) && (this->b_ == rhs.b_) && (this->c_ == rhs.c_);
    }


    NumberType get_a() const {
        return a_;
    }

    NumberType get_b() const {
        return b_;
    }

    NumberType get_c() const {
        return c_;
    }

    std::tuple<double, double> solve_y() const {
        return {-static_cast<double>(a_) / static_cast<double>(b_), -static_cast<double>(c_) / static_cast<double>(b_)};
    }

    bool is_parallel(const Line &rhs) const {
        return a_ * rhs.b_ == b_ * rhs.a_;
    }

private:
    NumberType a_{0};
    NumberType b_{0};
    NumberType c_{0};
};

template<typename NumberType>
class HalfPlane {
public:
    enum Type {
        POSITIVE,
        NEGATIVE
    };

    HalfPlane(Line<NumberType> line, Type type = POSITIVE, bool with_border = true) : line_(line), type_(type),
                                                                                      with_border_(with_border) {};

    bool operator==(const HalfPlane<NumberType> &rhs) const {
        return (this->line_ == rhs.line_) && (this->type_ == rhs.type_) && (this->with_border_ == rhs.with_border_);
    }

    double calculate_tangent_ox() const {
        return std::get<0>(line_.solve_y());
    }

    bool is_parallel(const HalfPlane &rhs) const {
        return line_.is_parallel(rhs.line_);
    }

    bool is_top() const {
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

    bool is_bottom() const {
        return !is_top();
    }

    bool contain(const HalfPlane &rhs) const {
        if (*this == rhs) {
            return true;
        }
        if (!is_parallel(rhs)) {
            return false;
        }
        if (is_top() != rhs.is_top()) {
            return false;
        }
        if (type_ == POSITIVE) {
            return rhs.line_.get_c() < line_.get_c();
        } else {
            return rhs.line_.get_c() > line_.get_c();
        }
    }

    bool contain_intersection(const HalfPlane &first, const HalfPlane &second) const {
        // matrix {{a, b, c},{d,e,f},{g,h,i}}
        // determinant a e i - a f h - b d i + b f g + c d h - c e g
        return
                ((first.line_.get_a() * second.line_.get_b() * line_.get_c() -
                  first.line_.get_a() * second.line_.get_c() * line_.get_b() -
                  first.line_.get_b() * second.line_.get_a() * line_.get_c() +
                  first.line_.get_b() * second.line_.get_c() * line_.get_a() +
                  first.line_.get_c() * second.line_.get_a() * line_.get_b() -
                  first.line_.get_c() * second.line_.get_b() * line_.get_a()) >= 0);
    }

    Type get_type() const {
        return type_;
    }

    Line<NumberType> get_line() const {
        return line_;
    }

private:
    Line<NumberType> line_;
    Type type_{POSITIVE};
    bool with_border_{true};
};

template<typename NumberType>
class Polygon {
public:
    template<typename NumberType2>
    Polygon(std::vector<HalfPlane<NumberType2>> halfplanes) {
        assert(halfplanes.size() > 2);

        std::sort(halfplanes.begin(), halfplanes.end(), [](const auto &lhs, const auto &rhs) {
            auto lhs_top = lhs.is_top();
            auto rhs_top = rhs.is_top();
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
                /*if (lhs_top){
                    return lhs.calculate_tangent_ox() < rhs.calculate_tangent_ox();
                }
                else {
                    return lhs.calculate_tangent_ox() > rhs.calculate_tangent_ox();
                }*/
            }
            return static_cast<int>(lhs_top) < static_cast<int>(rhs_top);
            //return lhs.calculate_tangent_ox() < rhs.calculate_tangent_ox();
        });
        assert(halfplanes.front().is_top() != halfplanes.back().is_top());

        /*size_t begin_bottom = 0;
        size_t end_bottom = 0;
        size_t begin_top = 0;
        size_t end_top = halfplanes.size();
        for (size_t i = 0; i < halfplanes.size() - 1; ++i) {
            if (halfplanes[i].is_top() != halfplanes[i + 1].is_top()) {
                end_bottom = i + 1;
                begin_top = i + 1;
                break;
            }
        }
        std::array<std::pair<size_t, size_t>, 2> bounds{
                std::pair<size_t, size_t>{begin_bottom, end_bottom},
                std::pair<size_t, size_t>{begin_top, end_top}};*/

        std::vector<HalfPlane<NumberType2>> filtered_halfplanes;
        //for (auto &bound: bounds) {
        auto begin = 0;//bound.first;
        auto end = halfplanes.size();// bound.second;

        while (true) {
            assert(end - begin > 1);
            if (end - begin > 1) {
                if (halfplanes[end - 1].contain(halfplanes[begin])) {
                    end -= 1;
                    continue;
                }
                if (halfplanes[begin].contain(halfplanes[end - 1])) {
                    begin += 1;
                    continue;
                }
                if (halfplanes[begin].contain(halfplanes[begin + 1])) {
                    begin += 1;
                    continue;
                }
                if (halfplanes[begin + 1].contain(halfplanes[begin])) {
                    halfplanes[begin + 1] = halfplanes[begin];
                    begin += 1;
                    continue;
                }
            }
            if (end - begin > 2) {
                if (halfplanes[begin].contain_intersection(halfplanes.back(), halfplanes[begin + 1])) {
                    begin += 1;
                    continue;
                }
            }
            filtered_halfplanes.push_back(halfplanes[begin]);
            begin += 1;
            break;
        }

        while (begin != end) {
            if (halfplanes[begin].contain(filtered_halfplanes.back())) {
                begin += 1;
                continue;
            }
            if (end - begin > 1) {
                if (halfplanes[begin].contain(halfplanes[begin + 1])) {
                    begin += 1;
                    continue;
                }
                if (halfplanes[begin + 1].contain(halfplanes[begin])) {
                    halfplanes[begin + 1] = halfplanes[begin];
                    begin += 1;
                    continue;
                }
                if (halfplanes[begin].contain_intersection(filtered_halfplanes.back(), halfplanes[begin + 1])) {
                    begin += 1;
                    continue;
                }
            } else {
                if (halfplanes[begin].contain_intersection(filtered_halfplanes.back(), filtered_halfplanes.front())) {
                    begin += 1;
                    continue;
                }
            }
            if (filtered_halfplanes.back().contain_intersection(
                    filtered_halfplanes[filtered_halfplanes.size() - 2],
                    halfplanes[begin])) {
                filtered_halfplanes.back() = halfplanes[begin];
            } else {
                filtered_halfplanes.push_back(halfplanes[begin]);
            }

            begin += 1;
        }
        if (filtered_halfplanes.back().contain_intersection(filtered_halfplanes[filtered_halfplanes.size() - 2], filtered_halfplanes.front())) {
            filtered_halfplanes.erase(filtered_halfplanes.end());
        }

        //}

        assert(filtered_halfplanes.size() > 2);
        assert(filtered_halfplanes.front().is_top() != filtered_halfplanes.back().is_top());

        for (size_t i = 0; i < filtered_halfplanes.size() - 1; ++i) {
            auto intersection = filtered_halfplanes[i].get_line().calculate_intersection(
                    filtered_halfplanes[i + 1].get_line());
            assert(intersection.type == Line<int64_t>::IntersectionWithLine::Type::INTERSECTION);
            assert(intersection.point.has_value());
            points.push_back(intersection.point.value());
        }
        auto intersection = filtered_halfplanes.back().get_line().calculate_intersection(
                filtered_halfplanes.front().get_line());
        assert(intersection.type == Line<int64_t>::IntersectionWithLine::Type::INTERSECTION);
        assert(intersection.point.has_value());
        points.push_back(intersection.point.value());
    }

    double calculate_area() const {
        double area = 0.0;
        for (size_t i = 0; i < points.size() - 1; ++i) {
            auto production_length =
                    points[i].get_x() * points[i + 1].get_y() - points[i].get_y() * points[i + 1].get_x();
            area += 0.5 * static_cast<double>(production_length);
        }
        auto production_length =
                points.back().get_x() * points.front().get_y() - points.back().get_y() * points.front().get_x();
        area += 0.5 * static_cast<double>(production_length);
        return std::abs(area);
    }

private:
    std::vector<Point<NumberType>> points;
};

// Начало тестов

void test_from_task() {
    std::vector<HalfPlane<int64_t>> halfplanes{
            {{1,  0,  0}},
            {{0,  1,  0}},
            {{-1, -1, 1}},
    };
    Polygon<double> figure(halfplanes);
    double area = figure.calculate_area();
    assert(is_equal(area, 0.5, 1e-6));
}

void test_double_figure() {
    std::vector<HalfPlane<int64_t>> halfplanes{
            {{1,  0,  0}},
            {{0,  1,  0}},
            {{-1, -1, 1}},
            {{1,  0,  0}},
            {{0,  1,  0}},
            {{-1, -1, 1}},
    };
    Polygon<double> figure(halfplanes);
    double area = figure.calculate_area();
    assert(is_equal(area, 0.5, 1e-6));
}

void test_rhombus() {
    std::vector<HalfPlane<int64_t>> halfplanes{
            {{1,  -1, 1}},
            {{-1, 1,  1}},
            {{-1, -1, 1}},
            {{1,  1,  1}},
    };
    Polygon<double> figure(halfplanes);
    double area = figure.calculate_area();
    assert(is_equal(area, 2.0, 1e-6));
}

void test_triangle_in_rhombus() {
    std::vector<HalfPlane<int64_t>> halfplanes{
            {{1,  -1, 1}},
            {{-1, 1,  1}},
            {{-1, -1, 1}},
            {{1,  1,  1}},

            {{1,  0,  0}},
            {{0,  1,  0}},
            {{-1, -1, 1}},
    };
    Polygon<double> figure(halfplanes);
    double area = figure.calculate_area();
    assert(is_equal(area, 0.5, 1e-6));
}

void test_square() {
    std::vector<HalfPlane<int64_t>> halfplanes{
            {{1,  0,  0}},
            {{0,  -1, 1}},
            {{0,  1,  0}},
            {{-1, 0,  1}},
    };
    Polygon<double> figure(halfplanes);
    double area = figure.calculate_area();
    assert(is_equal(area, 1.0, 1e-6));
}

void test_square_intersect_rhombus() {
    std::vector<HalfPlane<int64_t>> halfplanes{
            {{1,  0,  0}},
            {{0,  -1, 1}},
            {{1,  -1, 1}},
            {{-1, 1,  1}},
            {{0,  1,  0}},
            {{-1, 0,  1}},
            {{-1, -1, 1}},
            {{1,  1,  1}},
    };
    Polygon<double> figure(halfplanes);
    double area = figure.calculate_area();
    assert(is_equal(area, 0.5, 1e-6));
}

void test_octagon() {
    std::vector<HalfPlane<int64_t>> halfplanes{
            {{1,  -1, 3}},
            {{-1, -1, 3}},
            {{1,  1,  3}},
            {{-1, 1,  3}},
            {{0,  -1, 2}},
            {{-1, 0,  2}},
            {{1,  0,  2}},
            {{0,  1,  2}},
    };
    Polygon<double> figure(halfplanes);
    double area = figure.calculate_area();
    assert(is_equal(area, 14.0, 1e-6));
}

void test_square_intersect_rhombus_in_double_octagon() {
    std::vector<HalfPlane<int64_t>> halfplanes{
            // square
            {{1,  0,  0}},
            {{0,  -1, 1}},
            {{0,  1,  0}},
            {{-1, 0,  1}},
            // rhombus
            {{1,  -1, 1}},
            {{-1, 1,  1}},
            {{-1, -1, 1}},
            {{1,  1,  1}},
            // octagon
            {{1,  -1, 3}},
            {{-1, -1, 3}},
            {{1,  1,  3}},
            {{-1, 1,  3}},
            {{0,  -1, 2}},
            {{-1, 0,  2}},
            {{1,  0,  2}},
            {{0,  1,  2}},
            // octagon
            {{1,  -1, 3}},
            {{-1, -1, 3}},
            {{1,  1,  3}},
            {{-1, 1,  3}},
            {{0,  -1, 2}},
            {{-1, 0,  2}},
            {{1,  0,  2}},
            {{0,  1,  2}},
    };
    Polygon<double> figure(halfplanes);
    double area = figure.calculate_area();
    assert(is_equal(area, 0.5, 1e-6));
}

void run_all_tests() {
    test_from_task();
    test_rhombus();
    test_square();
    test_octagon();
    test_double_figure();
    test_triangle_in_rhombus();
    test_square_intersect_rhombus();
    test_square_intersect_rhombus_in_double_octagon();
}

// Конец тестов

int main(int argc, char *argv[]) {
    /*run_all_tests();
    return 0;*/

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
    std::vector<HalfPlane<int64_t>> halfplanes;
    for (size_t i = 0; i < N; ++i) {
        int64_t a{0}, b{0}, c{0};
        std::cin >> a >> b >> c;
        halfplanes.emplace_back(Line<int64_t>(a, b, c));
    }
    Polygon<double> figure(halfplanes);
    double area = figure.calculate_area();
    std::cout << std::setprecision(10) << std::fixed;
    std::cout << area;

    return 0;
}
