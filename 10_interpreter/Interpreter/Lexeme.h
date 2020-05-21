#ifndef MADEALGORITHMSHOMEWORK2_LEXEME_H
#define MADEALGORITHMSHOMEWORK2_LEXEME_H

#include <string>
#include <vector>
#include <functional>

/**
 * @brief Описывает лексему и
 */
class Lexeme {
public:
    /**
     * @brief Перечисление всех лексем из грамматики языка.
     */
    enum LexemeType {
        // общие
        IDENTIFIER,
        CONSTANT,
        FACTOR,
        TERM,
        EXPRESSION,
        STATEMENT,
        ASSIGN,

        // арифметические
        PLUS,
        MINUS,
        DIVISION,
        MULTIPLICATION,

        // логические операции
        LOGICAL_AND,
        LOGICAL_OR,
        LOGICAL_XOR,
        LOGICAL_NOT,

        // операции сравнения
        LESS,
        GREATER,
        EQUAL,
        NOT_EQUAL,
        LESS_OR_EQUAL,
        GREATER_OR_EQUAL,

        // разделители
        END,
        LEFT_CIRCLE_BRACKET,
        RIGHT_CIRCLE_BRACKET,
        LEFT_SQUARE_BRACKET,
        RIGHT_SQUARE_BRACKET,
        COMMA,

        // массивы
        LIST
    };

    /**
     * @brief Получить название лексемы для вывода сообщения на экран.
     * @param type Тип лексемы.
     * @return Название лексемы.
     */
    static std::__cxx11::string LexemeTypeToString(LexemeType type);

    // далее следуют общие методы для всех лексем

    Lexeme() = default;
    /**
     * @brief Конструктор лексемы, не содержащей значения или действия.
     * @param type Тип лексемы.
     * @param code Её символьное представление в коде.
     */
    Lexeme(LexemeType type, const std::__cxx11::string &code) : type_(type), code_(code){}

    virtual ~Lexeme() = default;

    Lexeme(const Lexeme &) = default;
    Lexeme(Lexeme &&) = default;

    Lexeme &operator=(const Lexeme &) = default;
    Lexeme &operator=(Lexeme &&) = default;

    bool operator==(const Lexeme& rhs) const;
    bool operator!=(const Lexeme& rhs) const;

    // методы для получения и установки символьного представления лексемы в коде
    const std::__cxx11::string &get_code() const;
    void set_code(const std::__cxx11::string &code);

    // методы для получения и установки типа лексемы
    LexemeType get_type() const;
    void set_type(LexemeType type);

    /**
     * @brief Узнать, нужно ли после данной лексемы читать следующую при синтаксическом анализе.
     * @param next_type Тип следующей лексемы.
     * @return True, если нужно прочитать следующую лексему. Иначе - false.
     */
    bool need_next(LexemeType next_type) const;

    // Выводят информацию о лексеме для пользователя и для разработчика.
    void print() const;;
    void debug_print() const;;

    /**
     * @brief Содержит ли лексема действие.
     * @return True, если лексема содержит действие, иначе - false.
     */
    virtual bool is_action() const;
    /**
     * @brief Содержит ли лексема данные.
     * @return True, если лексема содержит данные, иначе - false.
     */
    virtual bool is_value () const;


    // далле следуют методы, касающиеся лексем, содержащих данные

    /**
     * @brief Тип данных, содержащихся в лексеме
     */
    enum ValueType {
        INTEGER,
        FLOAT,
        BOOL,
        ARR,
        NONE
    };

    /**
     * @brief Конструтор лексемы, содержащей данные.
     * @param type Тип лексемы.
     * @param code Символьное представление в коде.
     * @param type_value Тип данных, содержащихся в лексеме.
     * @param data Данные. Массив байт.
     */
    Lexeme(LexemeType type, const std::__cxx11::string &code, ValueType type_value, const std::vector<uint8_t> &data)
    : type_(type), code_(code), type_value_(type_value), data_(data) {}

    // методы получения и установки типа данных
    ValueType get_type_value() const;
    void set_type_value(ValueType type_value_);

    // если лексема является массивом, то этими методами можно получить и установить тип элементов массива
    ValueType get_type_subvalue() const;
    void set_type_subvalue(ValueType type_value_);

    // получение и установка данных
    const std::vector<uint8_t> &get_data() const;
    void set_data(const std::vector<uint8_t> &data);


    // далее следуют методы для лексем, содержащих действие

    /**
     * @brief Конструктор лексем, содержащих действие.
     * @param type Тип лексемы.
     * @param code Символьное представление в коде.
     * @param action Функция, которую содержит лексема.
     */
    Lexeme(LexemeType type, const std::__cxx11::string &code, const std::function<Lexeme(Lexeme &, Lexeme &)> &action)
    : type_(type), code_(code), action_(action) {}

    /**
     * @brief Выполнить действие.
     * @param lhs Первый аргумент.
     * @param rhs Второй аргумент.
     * @return Результат.
     */
    Lexeme exec_action(Lexeme &lhs, Lexeme &rhs) const;


private:
    LexemeType type_;
    std::__cxx11::string code_;

    ValueType type_value_ { NONE };
    ValueType type_subvalue_ { NONE };
    std::vector<uint8_t> data_;

    std::function<Lexeme(Lexeme &lhs, Lexeme &rhs)> action_;

    /**
     * @brief Всегда ли нужно после этой лексемы читать следующую при синтаксическом анализе.
     * @return True, если нужно читать следующую всегда.
     */
    bool need_next_always() const;
    /**
     * @brief Лексемы каких типов обязательно нужно читать после данной при синтаксическом анализе.
     * @return Массив типов лексем, которые должны быть обязательно считаны после данной.
     */
    std::vector<LexemeType> need_next_types() const;
};

#endif //MADEALGORITHMSHOMEWORK2_LEXEME_H
