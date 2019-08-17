# include "Filler.h"

// Конструктор по умолчанию
Filler::Filler()
{
    Reset();
}


//Иницифализирующий конструктор
Filler::Filler(std::string routeFileName, unsigned int isFixedFirstRow)
{
    std::fstream routeFile;
    
    Reset();
    
    routeFile.open(routeFileName.c_str(), std::ios_base::in);
    if (routeFile.is_open())
    {
        _routeFileName = routeFileName;
        ReadRoute(routeFile);
        _isFixedFirstRow = isFixedFirstRow;
        
        if (_isFixedFirstRow)
        {
            for (unsigned int columnId = 0; columnId < Rank; columnId++)
            {
                _square[0][columnId] = columnId;
            }
        }
        
        routeFile.close();
        
        _isInitialized = Yes;
    }
}


// Считывание маршрута обхода клеток
int Filler::ReadRoute(std::istream& is)
{
    int result = ResultError;
    
    is >> _routeLength;
    
    if (_routeLength <= MaxRouteLength)
    {
        for (unsigned int stepId = 0; stepId < _routeLength; stepId++)
        {
            is >> _route[stepId][0];
            is >> _route[stepId][1];
        }
        
        result = ResultOK;
    }
    else
    {
        _routeLength = 0;
        result = ResultError;
    }
    
    return result;
}


// Сброс значений объекта
void Filler::Reset()
{
    _isInitialized = No;
    _isFixedFirstRow = No;
    
    for (unsigned int rowId = 0; rowId < Rank; rowId++)
    {
        for (unsigned int columnId = 0; columnId < Rank; columnId++)
        {
            _square[rowId][columnId] = Empty;
        }
    }
    
    _routeLength = 0;

    for (unsigned int stepId = 0; stepId < MaxRouteLength; stepId++)
    {
        _route[stepId][0] = Empty;
        _route[stepId][1] = Empty;
    }
    
    _routeFileName = "";
    combinationsCount = 0;
}


// Запуск создания комбинаций из заполненных клеток
int Filler::Start()
{
    int result = ResultOK;
    
    unsigned int rowId;
    unsigned int columnId;
    int stepId;
    
    int cellValue;
    int oldCellValue;
    unsigned int isGet;
    unsigned int bitIndex;
    unsigned int freeValuesMask;
    
    unsigned int flagsPrimary;                 // "Массив" флагов-битов задействования значений на главной диагонали
    unsigned int flagsSecondary;               // "Массив" флагов-битов задействования значений на побочной диагонали
    unsigned int flagsColumns[Rank];           // "Матрица" значений, использовавшихся в столбцах - columns[значение][столбец] = 0|1. 0 - значение занято. 1 - свободно.
    unsigned int flagsRows[Rank];              // "Матрица" значений, использовавшихся в строках - rows[строка][значение] = 0|1
    unsigned int flagsCellsHistory[Rank][Rank];// "Куб" значений, которые использовались для формирования построенной части квадрата - cellsHistory[строка][столбец][значение]

    // Задание первоначальных значений
        // Задание первоначальных значений обычным переменным
        rowId = 0;
        columnId = 0;
        stepId = 0;
        
        cellValue = Empty;
        oldCellValue = Empty;
        isGet = No;
        bitIndex = 0;
        freeValuesMask = 0;
        
        // Задание первоначальных значений флагам
        if (_isFixedFirstRow)
        {
            // Задание значений для маршрутов с фиксированной строкой
                // Выставление флагов задействования значений на диагоналях
                flagsPrimary = ((1u << Rank) - 1) ^ 1u;
                flagsSecondary = ((1u << Rank) - 1) ^ (1u << (Rank - 1));
                
                // Выставление флагов задействования значений фиксированной строки в строках и столбцах квадрата
                for (unsigned int i = 0; i < Rank; i++)
                {
                    if (i == 0)
                    {
                        flagsRows[i] = 0;
                    }
                    else
                    {
                        flagsRows[i] = (1u << Rank) - 1u;
                    }
                    
                    flagsColumns[i] = ((1u << Rank) - 1u) ^ (1u << i);
                }
                
                // Выставление флагов истории задействования значений в строках и столбцах квадрата
                for (unsigned int i = 0; i < Rank; i++)
                {
                    for (unsigned int j = 0; j < Rank; j++)
                    {
                        if (i == 0)
                        {
                            // Выставление флагов истории задействования значений фиксированной строки в строках и столбцах квадрата
                            flagsCellsHistory[0][j] = ((1u << Rank) - 1u) ^ (1u << j);
                        }
                        else
                        {
                            // Сброс флагов истории задействования значений 
                            flagsCellsHistory[i][j] = ((1u << Rank) - 1u);
                        }
                    }
                }
        }
        else
        {
            // Сброс флагов задействования значений на диагоналях
            flagsPrimary = (1u << Rank) - 1u;
            flagsSecondary = (1u << Rank) - 1u;

            // Сброс флагов задействования значений фиксированной строки в строках и столбцах квадрата
            for (unsigned int i = 0; i < Rank; i++)
            {
                flagsRows[i] = (1u << Rank) - 1u;
                flagsColumns[i] = (1u << Rank) - 1u;
            }
            
            // Сброс флагов истории задействования значений в строках и столбцах квадрата
            for (unsigned int i = 0; i < Rank; i++)
            {
                for (unsigned int j = 0; j < Rank; j++)
                {
                    flagsCellsHistory[i][j] = ((1u << Rank) - 1u);
                }
            }
        }

    // Генерация комбинаций
    if (_isInitialized == Yes)
    {
        // Обход и заполнение клеток по заданному маршруту
        do
        {
            // Подбор значения для очередной клетки квадрата
            if (_route[stepId][0] >= 0 && _route[stepId][1] >= 0)
            {
                // Подбираем новое значение для клетки (rowId, columnId)
                    // Считываем координаты клетки
                    rowId = _route[stepId][0];
                    columnId = _route[stepId][1];

                    // Подбираем значение для клетки с заданными координатами [rowId][columnId]
                        // Сбрасываем значения переменных
                        cellValue = Empty;
                        freeValuesMask = (1u << Rank) - 1;
                        // Подбираем значение для клетки
                            // Применяем маску главной диагонали
                            if (rowId == columnId)
                            {
                                freeValuesMask &= flagsPrimary;
                            }
                            // Применяем маску побочной диагонали
                            if (Rank - rowId - 1 == columnId)
                            {
                                freeValuesMask &= flagsSecondary;
                            }

                            // Применяем маску строк, столбцов и истории значений
                            freeValuesMask &= flagsColumns[columnId] & flagsRows[rowId] & flagsCellsHistory[rowId][columnId];
                            
                            // Определяем минимально возможное для использования в клетке значение
                            # ifdef _MSC_VER
                            isGet = _BitScanForward(&bitIndex, freeValuesMask);
                            cellValue = bitIndex;
                            # else
                            bitIndex = __builtin_ffs(freeValuesMask);
                            if (bitIndex > 0)
                            {
                                isGet = Yes;
                                cellValue = bitIndex - 1;
                            }
                            else
                            {
                                isGet = No;
                            }
                            # endif
                            
                        // Обработка результата поиска
                        if (isGet)
                        {
                            // Обработка найденного нового значения
                                // Считывание текущего значения
                                oldCellValue = _square[rowId][columnId];

                                // Запись нового значения
                                    // Записываем значение в квадрат
                                    _square[rowId][columnId] = cellValue;
                                    // Отмечаем значение в столбцах
                                    flagsColumns[columnId] ^= 1u << cellValue;
                                    // Отмечаем значение в строках
                                    flagsRows[rowId] ^= 1u << cellValue;
                                    // Отмечаем значение в диагоналях
                                    if (rowId == columnId)
                                    {
                                        flagsPrimary ^= 1u << cellValue;
                                    }
                                    if (rowId == Rank - 1 - columnId)
                                    {
                                        flagsSecondary ^= 1u << cellValue;
                                    }
                                    // Отмечаем значение в истории значений клетки
                                    flagsCellsHistory[rowId][columnId] ^= 1u << cellValue;

                                // Возвращение предыдущего значения без зачистки истории 
                                // (так как мы работаем с этой клеткой)
                                if (oldCellValue != Empty)
                                {
                                    // Возвращаем значение в столбцы
                                    flagsColumns[columnId] |= 1u << oldCellValue;
                                    // Возвращаем значение в строки
                                    flagsRows[rowId] |= 1u << oldCellValue;
                                    // Возвращаем значение в диагонали
                                    if (rowId == columnId)
                                    {
                                        flagsPrimary |= 1u << oldCellValue;
                                    }
                                    if (rowId == Rank - 1 - columnId)
                                    {
                                        flagsSecondary |= 1u << oldCellValue;
                                    }
                                }

                                // Обработка достижения новой клетки
                                if (stepId == static_cast<int>(_routeLength - 1))
                                {
                                    // Обработка достижения конца маршрута
                                    combinationsCount++;
                                }
                                else
                                {
                                    // Делаем шаг вперёд
                                    stepId++;
                                }
                        }
                        else
                        {
                            // Обработка факта ненахождения нового значения в клетке (rowId; columnId)
                                // Возвращаем текущее значение из квадрата в массивы
                                // Считываем текущее значение
                                cellValue = _square[rowId][columnId];
                                // Возвращаем значение в служебные массивы
                                if (cellValue != Empty)
                                {
                                    // Возвращаем значение в столбцы
                                    flagsColumns[columnId] |= 1u << cellValue;
                                    // Возвращаем значение в строки
                                    flagsRows[rowId] |= 1u << cellValue;
                                    // Возвращаем значение в диагонали
                                    if (rowId == columnId)
                                    {
                                        flagsPrimary |= 1u << cellValue;
                                    }
                                    if (rowId == Rank - 1 - columnId)
                                    {
                                        flagsSecondary |= 1u << cellValue;
                                    }
                                    // Сбрасываем клетку квадрата
                                    _square[rowId][columnId] = Empty;
                                    // Зачищаем историю клетки (rowId, columnId)
                                    flagsCellsHistory[rowId][columnId] = (1u << Rank) - 1;
                                }

                                // Делаем шаг назад
                                stepId--;
                        }
            }
            else
            {
                // Выставление флага ошибки из-за получения в пути некорректных данных
                result = ResultError;
            }
        }
        while (stepId >= 0 && result != ResultError);
    }
    else
    {
        result = ResultError;
    }
    
    return result;
}
