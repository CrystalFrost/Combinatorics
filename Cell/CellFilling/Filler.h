# pragma once

# include <string>
# include <fstream>

// Класс "Обходчик клеток" - в соответствии со считанным маршрутом, обходит клетки, формируя комбинации заполнения
class Filler
{
public:
    const static unsigned int Rank = 10;        // Ранг квадрата
    const static unsigned int MaxRouteLength = Rank*Rank;   // Максимальная длина маршрута
    const static unsigned int No = 0;           // Константа "Да"
    const static unsigned int Yes = 1;          // Константа "Нет"
    const static int Empty = -1;                // "Пустое" значение
    const static int ResultOK = 0;              // Флаг успешной отработки функции
    const static int ResultError = 1;           // Флаг возникновения ошибки при работе функции
    
    unsigned long long  combinationsCount;      // Число полученных комбинаций заполнения клеток
    
    Filler();                                   // Конструктор по умолчанию
    Filler(std::string routeFileName, unsigned int isFixedFirstRow);    // Инициализирующий конструктор
    int Start();                                // Запуск создания комбинаций из заполненных клеток
private:
   
    int ReadRoute(std::istream& is);            // Считывание маршрута из потока
    void Reset();                               // Сброс значений обходчика

    unsigned int _isInitialized;                // Флаг инициализированности обходчика клеток
    unsigned int _isFixedFirstRow;              // Флаг фиксации первой строки (для нормализованных ДЛК)
    
    int _square[Rank][Rank];                    // Массив с матрицей квадрата
    unsigned int _routeLength;                  // Фактическая длина машрута, считанная из файла
    int _route[MaxRouteLength][2];              // Массив с маршрутом обхода клеток. i-е значений - это адрес клетки, посещаемой на i-м шаге
    
    std::string _routeFileName;                 // Путь к файлу с маршрутом обхода клеток
};